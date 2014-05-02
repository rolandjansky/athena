/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ConstLink_h
#define ConstLink_h 1
#ifndef NULL
#define NULL            0
#endif
/**     @brief Smart links to reference-counted pointers.  Used in this
 *      package for calorimeter cells
 **/

/**	Template Smart Pointer Class.  This reference counted
 *	link allocates on demand. It audits the total pointer
 *	count and collects the garbage when nobody's looking.
 */
template <class T>
class ConstLink 
{
  public:
  

  /**
   * @brief Constructor
   */
  ConstLink();
  
  /**
   * @brief Copy Constructor
   */
  ConstLink(const ConstLink< T > &right);
  
  //	Constructor
  /**
   * @brief Constructor
   */
  ConstLink (const T *target);
  
  /**
   * @brief Destructor 
   */
  virtual ~ConstLink();
  
  /**
   * @brief Assignment
   */
  ConstLink< T > & operator=(const ConstLink< T > &right);
  
  /**
   * @brief Equality
   */
  int operator==(const ConstLink< T > &right) const;
  
  /**
   * @brief Inequality
   */
  int operator!=(const ConstLink< T > &right) const;
  
  
  /**
   * @brief Relational operator
   */
  int operator<(const ConstLink< T > &right) const;
  
  /**
   * @brief Relational operator
   */
  int operator>(const ConstLink< T > &right) const;
  
  /**
   * @brief Relational operator
   */
  int operator<=(const ConstLink< T > &right) const;
  
  /**
   * @brief Relational operator
   */
  int operator>=(const ConstLink< T > &right) const;
  
  
  
  /**
   *     @brief Dereference:  (*t).method();
   */
  virtual const T & operator * () const;
  
  /**
   *     @brief Dereference: t->method()
   */
  virtual const T * operator -> () const;
  
  /**
   *      @brief Check pointer validity:  if (t) {...}
   */
  operator bool () const;
  


  private: 
  /**
   * @brief Raw pointer to reference counted object.
   */ 
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
