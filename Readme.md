# **Expiry Work Base - Task Expiry & Hierarchical Management**

## **Overview**
**Expiry Work Base (EWB)** is a **memory management system** designed for **hierarchical task expiry**. It manages **time-limited data**, ensuring efficient **deletion** and **resource cleanup**.

## **Key Features**
- **Hierarchical Structure:** Tasks are **organized in layers**, ensuring lower layers **delete first** before the higher layers.
- **Automated Cleanup:** Tasks **expire automatically** unless they are **reset** or manually cancelled.
- **Optimized Performance:** Uses **`list_head`** to store active tasks efficiently.
- **Thread Safety:** **Mutex locking** ensures safe concurrent access.
- **Benchmarking Support:** Measures **task execution time**.

---

## **Public API**
### **1. Measure Task Execution Time**
```c
extern struct ExpiryWorkBaseBenchmark TheBenchmarksExpiryWorkBase(struct ExpiryWorkBase*,bool,bool);
```
- **Calculates execution time** and **expiry duration**.
- If `bool cancelExpiry` is `true`, the task is **automatically cancelled**.

### **2. Retrieve Parent Task**
```c
extern void*GetExpiryWorkBaseParent(struct ExpiryWorkBase*);
```
- Retrieves the **parent task** in the hierarchy.

### **3. Setup an Expiry Work Base**
```c
extern bool SetupExpiryWorkBase(struct ExpiryWorkBase**,struct ExpiryWorkBase*,void*,void(*)(void*));
```
- Initializes and **adds a new task** to the global list.
- **Auto-deletion function** can be provided.

### **4. Cancel an Expiry Work Base**
```c
extern void CancelExpiryWorkBase(struct ExpiryWorkBase*ewb);
```
- **Cancels a task** and removes it from the system.

### **5. Lock and Unlock a Task**
```c
extern bool LockExpiryWorkBase(struct ExpiryWorkBase*ewb);
extern bool UnlockExpiryWorkBase(struct ExpiryWorkBase*ewb);
```
- **Prevents modifications** while locked.
- **Unlocks the task** when modifications are allowed.

### **6. SetAutoDeleteExpiryWorkBase**
```c
extern void SetAutoDeleteExpiryWorkBase(struct ExpiryWorkBase*,void(*)(void*,struct ExpiryWorkBaseBenchmark));
```
- **Give the option to set function**.

---

### **7. Define Expiry Work Base Struct**
```c
#define SetupEWB struct ExpiryWorkBase*ewb
```
- **Macro for declaring an Expiry Work Base**.

---

## **Performance Considerations**
- **Tasks expire in a structured manner**, reducing unnecessary processing.
- **Only necessary locks are applied**, keeping it **lightweight and fast**.
- **Benchmarks provide real-time insights** into task execution.

---

## **Integration with TheRequirements**
**Expiry Work Base** is now part of **TheRequirements 0.1**, making it an essential part of the **layered execution tracking** system.

---

## **License**
This project is licensed under the **Do What The F*ck You Want To Public License (WTFPL)**.  
See the [LICENSE](LICENSE) file for more details.

---

**Pirasath Luxchumykanthan**  
