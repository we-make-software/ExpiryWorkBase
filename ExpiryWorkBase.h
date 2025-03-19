#ifndef ExpiryWorkBase_H
#define ExpiryWorkBase_H
#include "../TheRequirements/TheRequirements.h"
struct ExpiryWorkBase;
struct ExpiryWorkBaseBenchmark {
    u64 period,execution;
};
extern struct ExpiryWorkBaseBenchmark TheBenchmarksExpiryWorkBase(struct ExpiryWorkBase*,bool,bool);
extern bool SetAutoDeleteExpiryWorkBase(struct ExpiryWorkBase*,void(*)(void*,struct ExpiryWorkBaseBenchmark));
extern void*GetExpiryWorkBaseParent(struct ExpiryWorkBase*);
extern bool SetupExpiryWorkBase(struct ExpiryWorkBase**,struct ExpiryWorkBase*,void*,void(*)(void*));
extern void CancelExpiryWorkBase(struct ExpiryWorkBase*ewb);
extern bool LockExpiryWorkBase(struct ExpiryWorkBase*ewb);
extern bool UnlockExpiryWorkBase(struct ExpiryWorkBase*ewb);
extern bool CheckForCancellationExpiryWorkBase(struct ExpiryWorkBase*);
#define SetupEWB struct ExpiryWorkBase*ewb
#endif