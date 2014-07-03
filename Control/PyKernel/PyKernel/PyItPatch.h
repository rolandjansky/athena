/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PYANALYSISCORE_PYITPATCH_H
#define PYANALYSISCORE_PYITPATCH_H

// An utility class to remove '*' from a class type
template <class T> struct NonPointer
{
  typedef T Type;
};
template <class T> struct NonPointer<T *>
{
  typedef T Type;
};

// An utility class to remove 'const' from a class type
template <class T> struct NonConst
{
  typedef T Type;
};
template <class T> struct NonConst<const T>
{
  typedef T Type;
};


/**
   This class provides some pathces for an iterator. 
   Some c++ classes have methods to return iterators instead of vectors.
   In this case people need to use iterators directly. PyKernel addes next()
   and patches __eq__()/__ne__() instead. Then one can use the iterator like
   as a python iterator.
**/
template <class IT>
struct PyItPatch
{
  PyItPatch() : m_cache(0) {}
  PyItPatch(IT &it) : m_it(it)
  { 
    m_cache = new typename NonConst<typename NonPointer<typename IT::pointer>::Type>::Type();
  }
  virtual ~PyItPatch() { if (m_cache!=0) delete m_cache; }

  // next() for python iterator
  typename IT::reference next()
  {
    // this implementation is needed for LCG dict
    // 'return *m_it++' doesn't compile
    *m_cache = *m_it;
    ++m_it;
    return *m_cache;
  }

  // __eq__() for python iterator
  bool eq(IT & rhs)
  {
    return rhs == m_it;
  }

  // __ne__() for python iterator
  bool ne(IT & rhs)
  {
    return !eq(rhs);
  }

private:
  IT m_it;
  typename NonConst<typename NonPointer<typename IT::pointer>::Type>::Type *m_cache;
};

#endif

