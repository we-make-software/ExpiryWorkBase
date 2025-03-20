#include "../TheRequirements/TheRequirements.h"
struct ExpiryWorkBaseBenchmark {
    u64 period,execution;
};
struct ExpiryWorkBase {
    u8 cancelled;
    struct mutex lock;
    struct ExpiryWorkBase*prev;
    void*parent;
    void(*bindDelete)(void*,struct ExpiryWorkBaseBenchmark); 
    struct delayed_work work;
    ktime_t start,get;
    struct list_head list;
};
static bool StoppingExpiryWorkBase;
bool IsStoppingExpiryWorkBaseFalse(void);
bool IsStoppingExpiryWorkBaseFalse(void){
    return !StoppingExpiryWorkBase;
}
EXPORT_SYMBOL(IsStoppingExpiryWorkBaseFalse);
void*GetExpiryWorkBasePrevious(struct ExpiryWorkBase*);
void*GetExpiryWorkBasePrevious(struct ExpiryWorkBase*ewb){
    return ewb&&ewb->prev?ewb->prev->parent:NULL;
}
EXPORT_SYMBOL(GetExpiryWorkBasePrevious);
bool LockExpiryWorkBase(struct ExpiryWorkBase*);
bool LockExpiryWorkBase(struct ExpiryWorkBase*ewb){
    if(!ewb||ewb->cancelled)return false;
    mutex_lock(&ewb->lock);
    return true;
}
EXPORT_SYMBOL(LockExpiryWorkBase);
bool UnlockExpiryWorkBase(struct ExpiryWorkBase*);
bool UnlockExpiryWorkBase(struct ExpiryWorkBase*ewb){
    if(!ewb||ewb->cancelled)return false;
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
struct ExpiryWorkBaseBenchmark TheBenchmarksExpiryWorkBase(struct ExpiryWorkBase*,bool,bool);
void CancelExpiryWorkBase(struct ExpiryWorkBase*ewb){
    if(!ewb||!delayed_work_pending(&ewb->work))return;
    cancel_delayed_work_sync(&ewb->work);
    if(ewb->cancelled)return;
	mutex_lock(&ewb->lock);
    if(ewb->cancelled){
        mutex_unlock(&ewb->lock);
        return;
    }
    ewb->cancelled=true;  
	if(ewb->bindDelete)
        ewb->bindDelete(ewb->parent,TheBenchmarksExpiryWorkBase(ewb,false,false));     
	mutex_unlock(&ewb->lock);
    mutex_lock(&globalListLock);
    list_del(&ewb->list);
    mutex_unlock(&globalListLock);
    kfree(ewb);
}
EXPORT_SYMBOL(CancelExpiryWorkBase);
struct ExpiryWorkBaseBenchmark TheBenchmarksExpiryWorkBase(struct ExpiryWorkBase*ewb,bool consolePrint,bool cancelExpiry){
    struct ExpiryWorkBaseBenchmark benchmark={0,0};
    if(!ewb)return benchmark;
    u64 now=ktime_to_ns(ktime_get_real()); 
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
    if(!background_ewb->ewb->cancelled||background_ewb->ewb->parent){
        if(background_ewb->ewb->prev&&background_ewb->ewb->prev->cancelled)
            CancelExpiryWorkBase(background_ewb->ewb);
        else 
            ResetExpiryWorkBase(background_ewb->ewb);
    }
    kfree(background_ewb);
}
static bool ResetExpiryWorkBase(struct ExpiryWorkBase*);
static void*Get(struct ExpiryWorkBase*,bool);
static void*Get(struct ExpiryWorkBase*ewb,bool ios){
    if(!ewb||ewb->cancelled||(ewb->prev&&ewb->prev->cancelled))return NULL;
    if(ewb->prev){
        struct BackgroundExpiryWorkBase*background_ewb=kmalloc(sizeof(struct BackgroundExpiryWorkBase),GFP_KERNEL);
        if(background_ewb){
            background_ewb->ewb=ewb->prev;
            INIT_WORK(&background_ewb->work,BackgroundProcessExpiryWorkBase);
            queue_work(system_wq,&background_ewb->work);            
        }else if(ewb->prev&&!ewb->prev->cancelled)Get(ewb->prev,false);
    }
    if(!ios)
        ewb->get=ktime_get_real();
    if(!StoppingExpiryWorkBase)
        mod_delayed_work(system_wq,&ewb->work,msecs_to_jiffies(600000));
    return ewb->parent;
}
void*GetExpiryWorkBaseParent(struct ExpiryWorkBase*);
void*GetExpiryWorkBaseParent(struct ExpiryWorkBase*ewb){
    return ewb?Get(ewb,true):NULL;
}
EXPORT_SYMBOL(GetExpiryWorkBaseParent);
static bool ResetExpiryWorkBase(struct ExpiryWorkBase*ewb){
    if(StoppingExpiryWorkBase||!ewb||ewb->cancelled)return false;
	mutex_lock(&ewb->lock);
    if(ewb->cancelled){
        mutex_unlock(&ewb->lock);
        return false;
    }
	if(ewb->prev){
        if(ewb->prev->cancelled){
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
    CancelExpiryWorkBase(container_of(work,struct ExpiryWorkBase,work.work));
}
bool SetupExpiryWorkBase(struct ExpiryWorkBase**,struct ExpiryWorkBase*,void*,void(*)(void*,struct ExpiryWorkBaseBenchmark));
void(*bindDelete)(void*,struct ExpiryWorkBaseBenchmark); 
bool SetupExpiryWorkBase(struct ExpiryWorkBase**ewb,struct ExpiryWorkBase*previous,void*parent,void(*bindDelete)(void*,struct ExpiryWorkBaseBenchmark)){
    if(StoppingExpiryWorkBase||!ewb||!(*ewb=kmalloc(sizeof(struct ExpiryWorkBase),GFP_KERNEL)))return false;
    (*ewb)->cancelled=false;
    mutex_init(&(*ewb)->lock);
    INIT_DELAYED_WORK(&(*ewb)->work, ProcessExpiryWorkBaseToDo);
    mutex_lock(&globalListLock);
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
void StopExpiryWorkBase(void);
void StopExpiryWorkBase(void) {
    if(StoppingExpiryWorkBase)return;
    StoppingExpiryWorkBase=true;
    mutex_lock(&globalListLock);
    struct ExpiryWorkBase*ewb,*tmp;
    list_for_each_entry_safe(ewb,tmp,&globalList,list) {
        if (!delayed_work_pending(&ewb->work)) 
            CancelExpiryWorkBase(ewb);
    }
    mutex_unlock(&globalListLock);
    StoppingExpiryWorkBase=false;
}
EXPORT_SYMBOL(StopExpiryWorkBase);
bool SetAutoDeleteExpiryWorkBase(struct ExpiryWorkBase*,void(*)(void*,struct ExpiryWorkBaseBenchmark));
bool SetAutoDeleteExpiryWorkBase(struct ExpiryWorkBase*ewb,void(*bindDelete)(void*,struct ExpiryWorkBaseBenchmark)){
    if(StoppingExpiryWorkBase)return false;
    if(!ewb||GetExpiryWorkBaseParent(ewb))return false;
    ewb->bindDelete=bindDelete;
    return true;
}
EXPORT_SYMBOL(SetAutoDeleteExpiryWorkBase);
static void Layer0Start(void){
    StoppingExpiryWorkBase=false;
    INIT_LIST_HEAD(&globalList);
}
static void Layer0End(void){
    StoppingExpiryWorkBase=true;
}
Layer0Setup("ExpiryWorkBase",0,0)