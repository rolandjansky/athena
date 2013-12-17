/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANP_MUTEX_H
#define ANP_MUTEX_H

/**********************************************************************************
 * @Package: TrigCostBase
 * @Class  : Thread
 * @Author : Rustem Ospanov
 *
 * @Brief  : 
 *
 * Wrapper classes for pthread_mutex_t and pthread_cond_t.
 * Also contains sccoped Mutex lock class.
 *
 **********************************************************************************/

#include <cstddef> 
#include <pthread.h>

namespace Anp
{
  template <class T> class Lock {
  public:
    
    //
    // Constructor locks mutex
    //
    Lock(T &obj) : obj_(obj) {
      obj_.Lock();
    }
    
    //
    // When leaving scope destructor unlocks mutex
      //
    ~Lock() {
      obj_.UnLock();
    }  
    
  private:
    
    T& obj_;
    
    //
    // These functions are private and not implemented by design
    //
    Lock();
    Lock(const Lock &);
    Lock& operator=(const Lock &);
    Lock* operator&();
  };
  
  class Mutex {      
  public:

    Mutex();
    ~Mutex();
    
    int Lock(void);
    int UnLock(void);
    int TryLock(void);
    
    pthread_mutex_t* get_pthread_mutex();
    
  private:
    
    //
    // Copy ctor and assignment are private and not implemented by design
    //
    Mutex(const Mutex &);
    Mutex& operator=(const Mutex &);
    
  private:
    
    pthread_mutex_t fMutex; 
  };
  
  
  class CondVar {      
  public:
    
    CondVar(int value = 0);
    ~CondVar();
    
    int Wait(Mutex &mutex);
    int Signal();
    
    void SetValue(int value);
    int GetValue();
    
  private:
    
    //
    // Copy ctor and assignment are private and not implemented by design
    //
    CondVar(const CondVar &);
    CondVar& operator=(const CondVar &);
    
  private:
    
    pthread_cond_t fCond;
    int fValue;
  };
  
  class Thread {
  public:
    
    Thread(void *(*start_routine)(void*), void *arg);
    ~Thread() {}
    
    CondVar& GetCond()  { return fCond; }
    Mutex&   GetMutex() { return fMutex; }
    
    int GetStatus() const { return fStatus; }
    
  private:
    
    Thread();
    Thread(const Thread &);
    Thread& operator=(const Thread &);
    
  private:
    
    CondVar      fCond;
    Mutex        fMutex;
    pthread_t    fThread;
    int          fStatus;
  };

  //
  // Inlined member functions, pthread calls must be hidden from CINT
  //
  inline Mutex::Mutex()
    :fMutex() {
    const int status_init = pthread_mutex_init(&fMutex, NULL);
    if(status_init != 0) { 
      std::cerr << "pthread_mutex_init() failed" << std::endl;
    }
  }
  
  inline Mutex::~Mutex() {
    const int status_destroy = pthread_mutex_destroy(&fMutex);
    if(status_destroy != 0) { 
      std::cerr << "pthread_mutex_destroy() failed" << std::endl;
    }
  }

  inline int Mutex::Lock(void) {
    const int status_lock = pthread_mutex_lock(&fMutex);
    if(status_lock != 0) { 
      std::cerr << "pthread_mutex_lock() failed" << std::endl;
    }
    return status_lock;
  }

  inline int Mutex::TryLock(void) {      
    const int status_trylock = pthread_mutex_trylock(&fMutex);
    if(status_trylock != 0) { 
      std::cerr << "pthread_mutex_trylock() failed" << std::endl;
    }
    return status_trylock;
  }

  inline int Mutex::UnLock(void) {      
    const int status_unlock = pthread_mutex_unlock(&fMutex);
    if(status_unlock != 0) { 
      std::cerr << "pthread_mutex_unlock() failed" << std::endl;
    }
    return status_unlock;
  }
  
  inline pthread_mutex_t* Mutex::get_pthread_mutex() {
    return &fMutex;
  }
  
  inline CondVar::CondVar(int value)
    :fValue(value) {
    const int status_init = pthread_cond_init(&fCond, NULL); 
    if(status_init != 0) {
      std::cerr << "pthread_cond_init() failed" << std::endl;
    }
  }
  
  inline CondVar::~CondVar() {
    const int status_destroy = pthread_cond_destroy(&fCond); 
    if(status_destroy != 0) {
      std::cerr << "pthread_cond_destroy() failed" << std::endl;
    }
  }
  
  inline int CondVar::Wait(Mutex &mutex) {
    const int status_wait = pthread_cond_wait(&fCond, mutex.get_pthread_mutex());
    if(status_wait != 0) {
      std::cerr << "pthread_cond_wait() failed" << std::endl;
    }
    return status_wait;
  }
  
  inline int CondVar::Signal() {
    const int status_signal = pthread_cond_signal(&fCond);
    if(status_signal != 0) {
      std::cerr << "pthread_cond_signal() failed" << std::endl;
    }
    return status_signal;
  }
  
  inline void CondVar::SetValue(int value) { fValue = value; }
  inline int  CondVar::GetValue() { return fValue; }  
  
  inline Thread::Thread(void *(*start_routine)(void*), void *arg) 
    : fStatus(-1)  {
    
    fStatus = pthread_create(&fThread, NULL, start_routine, arg);
    if(fStatus != 0) {
      std::cerr << "Thread ctor - failed to create pthread" << std::endl;
    }
  }
}

#endif
