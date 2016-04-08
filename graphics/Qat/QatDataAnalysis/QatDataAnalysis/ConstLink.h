#ifndef ConstLink_h
#define ConstLink_h 1
#ifndef NULL
#define NULL            0
#endif
//     Smart links to reference-counted pointers.

//	Template Smart Pointer Class.  This reference counted
//	link allocates on demand. It audits the total pointer
//	count and collects the garbage when nobody's looking.
//
template <class T>
class ConstLink 
{
  public:
  

  // Constructor
  ConstLink();
  
  // Copy Constructor
  ConstLink(const ConstLink< T > &right);
  
  // Constructor
  ConstLink (const T *target);
  
  // Destructor 
  virtual ~ConstLink();
  
  // Assignment
  ConstLink< T > & operator=(const ConstLink< T > &right);
  
  // Equality
  int operator==(const ConstLink< T > &right) const;
  
  // Inequality
  int operator!=(const ConstLink< T > &right) const;
  
  
  // Relational operator
  int operator<(const ConstLink< T > &right) const;
  
  // Relational operator
  int operator>(const ConstLink< T > &right) const;
  
  // Relational operator
  int operator<=(const ConstLink< T > &right) const;
  
  // Relational operator
  int operator>=(const ConstLink< T > &right) const;
  
  
  
  // Dereference:  (*t).method();
  virtual const T & operator * () const;
  
  // Dereference: t->method()
  virtual const T * operator -> () const;
  
  // Check pointer validity:  if (t) {...}
  operator bool () const;
  


  private: 

  // Raw pointer to reference counted object.
  mutable T *ptr;
};



template <class T>
inline ConstLink<T>::ConstLink()
  :ptr(NULL)
{
}

template <class T>
inline ConstLink<T>::ConstLink(const ConstLink<T> &right)
  :ptr(right.ptr)
{
  if (ptr) ptr->ref();
}

template <class T>
inline ConstLink<T>::ConstLink (const T *target)
  :ptr(const_cast<T *> (target))
{
  if (target) target->ref();
}


template <class T>
inline ConstLink<T>::~ConstLink()
{
  if (ptr) ptr->unref();
}


template <class T>
inline ConstLink<T> & ConstLink<T>::operator=(const ConstLink<T> &right)
{
  if (this!=&right) {
    if (ptr) ptr->unref();
    ptr = right.ptr;
    if (ptr) ptr->ref();
  }
  return *this;
}


template <class T>
inline int ConstLink<T>::operator==(const ConstLink<T> &right) const
{
  return ptr==right.ptr;
}

template <class T>
inline int ConstLink<T>::operator!=(const ConstLink<T> &right) const
{
  return ptr!=right.ptr;
}


template <class T>
inline int ConstLink<T>::operator<(const ConstLink<T> &right) const
{
  return ptr<right.ptr;
}

template <class T>
inline int ConstLink<T>::operator>(const ConstLink<T> &right) const
{
  return ptr>right.ptr;
}

template <class T>
inline int ConstLink<T>::operator<=(const ConstLink<T> &right) const
{
  return ptr<=right.ptr;
}

template <class T>
inline int ConstLink<T>::operator>=(const ConstLink<T> &right) const
{
  return ptr>=right.ptr;
}



template <class T>
inline const T & ConstLink<T>::operator * () const
{
  return *ptr;
}

template <class T>
inline const T * ConstLink<T>::operator -> () const
{
  return ptr;
}

template <class T>
inline ConstLink<T>::operator bool () const
{
  return ptr;
}

#endif
