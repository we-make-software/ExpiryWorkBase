#include "../TheRequirements/TheRequirements.h"
#ifndef ExpiryWorkBase_H
#define ExpiryWorkBase_H
struct ExpiryWorkBase {
    bool Invalid;
    struct ExpiryWorkBase*Previous;
    u8 Used[(2*sizeof(void*))+sizeof(struct mutex)+sizeof(struct delayed_work)];
};
extern void SetupExpiryWorkBase(struct ExpiryWorkBase*expiry_work_base,struct ExpiryWorkBase*previous,void*parent,void(*AutoDelete)(void*));
extern void CancelExpiryWorkBase(struct ExpiryWorkBase*expiry_work_base);
extern bool ResetExpiryWorkBase(struct ExpiryWorkBase*expiry_work_base);
extern void LockExpiryWorkBase(struct ExpiryWorkBase*expiry_work_base);
extern void UnlockExpiryWorkBase(struct ExpiryWorkBase*expiry_work_base);
#endif