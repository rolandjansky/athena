#ifndef Link_h_
#define Link_h_ 1

#include "QatDataAnalysis/ConstLink.h"
template<class T>
class Link : public ConstLink<T>
{


  public:

  // Constructor:
  Link();

  // Copy Constructor:
  Link(const Link<T> &right);

  // Promotion:
  explicit Link(const ConstLink<T> &right);

  // Construct from a pointer:
  Link (const T  *addr);

  // Destructor:
  virtual ~Link();

  // Assignment:
  Link<T> & operator = (const Link<T> & right);

  // 
  virtual T & operator * ();

  virtual T * operator -> ();
  
  virtual const T & operator * () const;
  
  virtual const T * operator -> () const;


};



template <class T>
inline Link<T>::Link()
  :ConstLink<T>(NULL)
{
}



template <class T>
inline Link<T>::Link(const Link<T> &right)
  :ConstLink<T>(right)
{
}

template <class T>
inline Link<T>::Link(const ConstLink<T> &right)
  :ConstLink<T>(right)
{
}

template <class T>
inline Link<T>::Link (const T *target)
  :ConstLink<T>(target)
{
}


template <class T>
inline Link<T>::~Link()
{
}


template <class T>
inline Link<T> & Link<T>::operator=(const Link<T> &right)
{
  ConstLink<T>::operator=(right);
  return *this;
}


template <class T>
inline T & Link<T>::operator * () 
{
  return const_cast<T &> (ConstLink<T>::operator*());
}

template <class T>
inline T * Link<T>::operator -> () 
{
  return const_cast<T *> (ConstLink<T>::operator -> ());
}


template <class T>
inline const T & Link<T>::operator * () const
{
  return ConstLink<T>::operator*();
}

template <class T>
inline const T * Link<T>::operator -> () const
{
  return ConstLink<T>::operator -> ();
}

#endif


