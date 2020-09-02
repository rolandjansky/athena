#ifndef EVENTCONTAINERS_DELETER_H
//#define EVENTCONTAINERS_DELETER_H

#if 0
  struct deleter
  {
    void operator() (const void* p);
  };
#endif


  typedef void deleter_f (const void* p);

  class void_unique_ptr
    : public std::unique_ptr<const void, deleter_f*>
  {
  public:
    using std::unique_ptr<const void, deleter_f*>::unique_ptr;

    template <class T>
    struct Deleter
    {
      static void deleter (const void* p)
      {
        delete reinterpret_cast<const T*>(p);
      }
    };

    template <class T>
    void_unique_ptr(std::unique_ptr<T> p)
      : std::unique_ptr<const void, deleter_f*> (p.release(),
                                                 Deleter<T>::deleter)
    {
    }
  };
  
#endif
