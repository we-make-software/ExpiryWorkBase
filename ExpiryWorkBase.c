#include "../TheRequirements/TheRequirements.h"
static u32 magic;
struct ExpiryWorkBaseBenchmark {
    u64 period,execution;
};
struct ExpiryWorkBase {
    u32 magic;
    u8 prefix, control;
    u8 setup:6,cancelled:1,running:1;
    struct mutex lock;
    struct ExpiryWorkBase*prev;
    void*parent;
    void(*bindDelete)(void*,struct ExpiryWorkBaseBenchmark); 
    struct delayed_work work;
    ktime_t start,get;
    struct list_head list;
};
bool LockExpiryWorkBase(struct ExpiryWorkBase*);
bool LockExpiryWorkBase(struct ExpiryWorkBase*ewb){
    if(!ewb||ewb->magic!=magic||ewb->setup!=63||ewb->prefix<65||(ewb->prefix-ewb->setup)!=ewb->control||ewb->cancelled||ewb->running)return false;
    if(ewb->prev&&(ewb->prev->cancelled||ewb->prev->running))return false;
    mutex_lock(&ewb->lock);
    return true;
}
EXPORT_SYMBOL(LockExpiryWorkBase);
bool UnlockExpiryWorkBase(struct ExpiryWorkBase*);
bool UnlockExpiryWorkBase(struct ExpiryWorkBase*ewb){
    if(!ewb||ewb->magic!=magic||ewb->setup!=63||ewb->prefix<65||(ewb->prefix-ewb->setup)!=ewb->control||ewb->cancelled||ewb->running)return false;
    mutex_unlock(&ewb->lock);
    return true;
}
EXPORT_SYMBOL(UnlockExpiryWorkBase);
struct BackgroundExpiryWorkBase{
    struct work_struct work;
    struct ExpiryWorkBase*ewb;
};
static LIST_HEAD(globalList);
static DEFINE_MUTEX(globalListLock);
void CancelExpiryWorkBase(struct ExpiryWorkBase*ewb);
static struct kmem_cache*ewbcache;
struct ExpiryWorkBaseBenchmark TheBenchmarksExpiryWorkBase(struct ExpiryWorkBase*,bool,bool);
void CancelExpiryWorkBase(struct ExpiryWorkBase*ewb){
	if(!ewb||ewb->magic!=magic||ewb->setup!=63||ewb->prefix<65||(ewb->prefix-ewb->setup)!=ewb->control)return;
	mutex_lock(&ewb->lock);
    if(!ewb->running)
        cancel_delayed_work_sync(&ewb->work);
    if(ewb->cancelled){
        mutex_unlock(&ewb->lock);
        return;
    }
    ewb->cancelled=true;
	if(ewb->bindDelete)
        ewb->bindDelete(ewb->parent,TheBenchmarksExpiryWorkBase(ewb,false,false));
    kfree(ewb->parent);        
	mutex_unlock(&ewb->lock);
    mutex_lock(&ewb->lock);
    ewb->magic=0;
    mutex_unlock(&ewb->lock);
    mutex_lock(&globalListLock);
    list_del(&ewb->list);
    mutex_unlock(&globalListLock);
    kmem_cache_free(ewbcache, ewb);
}
EXPORT_SYMBOL(CancelExpiryWorkBase);
struct ExpiryWorkBaseBenchmark TheBenchmarksExpiryWorkBase(struct ExpiryWorkBase*ewb,bool consolePrint,bool cancelExpiry){
    struct ExpiryWorkBaseBenchmark benchmark = {0, 0};
    if(!ewb||ewb->magic!=magic||ewb->setup!=63||ewb->prefix<65||(ewb->prefix-ewb->setup)!=ewb->control)return benchmark;
    u64 now = ktime_to_ns(ktime_get_real()); 
    benchmark.period=now-ktime_to_ns(ewb->start); 
    benchmark.execution=now-ktime_to_ns(ewb->get);
    if(consolePrint)
        printk(KERN_INFO "Period: %llu ns\nExecution: %llu ns (%llu ms)\n", benchmark.period, benchmark.execution, benchmark.execution / 1000000);
    if(cancelExpiry)
        CancelExpiryWorkBase(ewb);
    return benchmark;
}
EXPORT_SYMBOL(TheBenchmarksExpiryWorkBase);
static void BackgroundProcessExpiryWorkBase(struct work_struct*work);
static bool ResetExpiryWorkBase(struct ExpiryWorkBase*expiry_work_base);
static void BackgroundProcessExpiryWorkBase(struct work_struct*work){
    struct BackgroundExpiryWorkBase*background_ewb=container_of(work,struct BackgroundExpiryWorkBase,work);
    if(!background_ewb)return;
    if(background_ewb->ewb->cancelled||background_ewb->ewb->running||!background_ewb->ewb->parent){
        kfree(background_ewb);
        return;
    }
    if(background_ewb->ewb->prev&&background_ewb->ewb->prev->cancelled&&background_ewb->ewb->prev->running){
        CancelExpiryWorkBase(background_ewb->ewb);
        kfree(background_ewb);
        return;
    }
    ResetExpiryWorkBase(background_ewb->ewb);
    kfree(background_ewb);
}
static void*Get(struct ExpiryWorkBase*,bool);
static void*Get(struct ExpiryWorkBase*ewb,bool IsOutSide){
    if(!ewb||ewb->cancelled||ewb->running)return NULL;
    if(ewb->prev){
        if(ewb->prev->cancelled&&ewb->prev->running)return NULL;
        struct BackgroundExpiryWorkBase*background_ewb=kmalloc(sizeof(struct BackgroundExpiryWorkBase),GFP_KERNEL);
        if(background_ewb){
            background_ewb->ewb=ewb->prev;
            INIT_WORK(&background_ewb->work,BackgroundProcessExpiryWorkBase);
            queue_work(system_wq,&background_ewb->work);            
        }else Get(ewb->prev,false);
    }
    if(!IsOutSide)
        ewb->get = ktime_get_real();
    mod_delayed_work(system_wq,&ewb->work,msecs_to_jiffies(600000));
    return ewb->parent;
}
void*GetExpiryWorkBaseParent(struct ExpiryWorkBase*);
void*GetExpiryWorkBaseParent(struct ExpiryWorkBase*ewb){
    if(!ewb||ewb->magic!=magic||ewb->setup!=63||ewb->prefix<65||(ewb->prefix-ewb->setup)!=ewb->control)return NULL;
    return Get(ewb,true);
}
EXPORT_SYMBOL(GetExpiryWorkBaseParent);
static bool ResetExpiryWorkBase(struct ExpiryWorkBase*ewb){
	if(!ewb||ewb->cancelled||ewb->running)return false;
	mutex_lock(&ewb->lock);
    if(ewb->cancelled||ewb->running){
        mutex_unlock(&ewb->lock);
        return false;
    }
	if(ewb->prev){
        if(ewb->prev->cancelled||ewb->prev->running){
            mutex_unlock(&ewb->lock);
            return false;
        }
        Get(ewb->prev,false);
    }
    mod_delayed_work(system_wq,&ewb->work,msecs_to_jiffies(600000));
	mutex_unlock(&ewb->lock);
	return true;
}
static void ProcessExpiryWorkBaseToDo(struct work_struct*);
static void ProcessExpiryWorkBaseToDo(struct work_struct*work){
    struct ExpiryWorkBase*ewb=container_of(work,struct ExpiryWorkBase,work.work);
    ewb->running=true;
    CancelExpiryWorkBase(ewb);
}
bool SetupExpiryWorkBase(struct ExpiryWorkBase**,struct ExpiryWorkBase*,void*,void(*)(void*));

static bool useGlobalListUse=true;

bool SetupExpiryWorkBase(struct ExpiryWorkBase**ewb,struct ExpiryWorkBase*previous,void*parent,void(*bindDelete)(void*)){
    if(*ewb){
        if((*ewb)->setup==63&&(*ewb)->control==(*ewb)->prefix-(*ewb)->setup&&(*ewb)->magic==magic)return true;
        kfree(*ewb);
    }
    mutex_lock(&globalListLock);
    if(!useGlobalListUse){
        mutex_unlock(&globalListLock);
        return false;
    }
    *ewb = kmem_cache_alloc(ewbcache, GFP_KERNEL);
    if(!*ewb){
        mutex_unlock(&globalListLock);
        return false;
    }
    (*ewb)->parent=parent;
    (*ewb)->bindDelete=bindDelete;
    (*ewb)->prev=previous;
    (*ewb)->start=ktime_get_real();
    (*ewb)->get=ktime_get_real();
    ResetExpiryWorkBase(*ewb);
    list_add(&(*ewb)->list, &globalList) ;
    mutex_unlock(&globalListLock);
    return true;
}
EXPORT_SYMBOL(SetupExpiryWorkBase);
void StopExpiryWorkBase(void){
    if(!useGlobalListUse)return;    
    mutex_lock(&globalListLock);
    useGlobalListUse=false;
    mutex_unlock(&globalListLock);
    struct ExpiryWorkBase *ewb, *tmp;
    list_for_each_entry_safe(ewb, tmp, &globalList, list)
        CancelExpiryWorkBase(ewb);
    mutex_lock(&globalListLock);
    useGlobalListUse=true;
    mutex_unlock(&globalListLock);
}
void ewbinit(void *obj) {
    struct ExpiryWorkBase *ewb = (struct ExpiryWorkBase *)obj;
    ewb->parent = NULL; 
    ewb->bindDelete = NULL;
    ewb->prev = NULL;
    ewb->setup = 63;
    ewb->prefix = 65 + (get_random_u8() % 191);
    ewb->control = ewb->prefix - ewb->setup;
    ewb->cancelled = false;
    ewb->running = false;
    ewb->magic = magic;
    mutex_init(&ewb->lock);
    INIT_DELAYED_WORK(&ewb->work, ProcessExpiryWorkBaseToDo);
}

EXPORT_SYMBOL(StopExpiryWorkBase);
static void Layer0Start(void){
    magic=get_random_u32();
    INIT_LIST_HEAD(&globalList);
    ewbcache=kmem_cache_create("ewbcache", sizeof(struct ExpiryWorkBase),0, SLAB_HWCACHE_ALIGN,ewbinit);
}
static void Layer0End(void){
    if (ewbcache) {
        kmem_cache_destroy(ewbcache);
        ewbcache=NULL;
    }
}
Layer0Setup("ExpiryWorkBase",0,0)