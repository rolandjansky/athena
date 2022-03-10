/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TargetBuffer_t_H
#define TargetBuffer_t_H

#include <vector>
#ifndef NDEBUG
#  include <stdexcept>
#endif

namespace TTN {
  class DoubleBuffer_t;

  template <typename T_BaseType, typename T_BufferType=const T_BaseType, typename T_VectorType=const std::vector<T_BaseType> >

  /** Helper class which adds a range check to an array access if NDEBUG is not defined.
   *
   * The helper class only provides read only access to the array, but allows
   * a derived class to add write access if permitted by the base type.
   */
  class BufferBase_t {
    friend class DoubleBuffer_t;
    template <typename T1, typename T2,typename T3> friend class BufferBase_t;

  public:
    BufferBase_t(T_VectorType &buffer) {
      m_ptr=&(buffer[0]);
#ifndef NDEBUG
      // debug code
      m_maxIndex=buffer.size();
      m_bufferStart=buffer.data();
      m_bufferEnd=m_bufferStart + buffer.size();//assumed contiguous
#endif
    }

  protected:

    template <typename T1, typename T2,typename T3>
    BufferBase_t(const BufferBase_t<T1,T2,T3> &buffer)
      : m_ptr(buffer.m_ptr)
#ifndef NDEBUG
      , m_maxIndex(buffer.m_maxIndex)
      , m_bufferStart(buffer.m_bufferStart)
      , m_bufferEnd(buffer.m_bufferEnd)
#endif
    {}

    template <typename T1, typename T2,typename T3>
    BufferBase_t &
    operator=(const BufferBase_t<T1,T2,T3> &buffer)
    {
      m_ptr=buffer.m_ptr;
#ifndef NDEBUG
      m_maxIndex=buffer.m_maxIndex;
      m_bufferStart=buffer.m_bufferStart;
      m_bufferEnd=buffer.m_bufferEnd;
#endif
      return *this;
    }

    // last two arguments are only needed for the optional range check.
    BufferBase_t(T_BufferType *buffer,
                 typename std::vector<T_BaseType>::size_type max_idx,
                 const std::vector<double> &full_vector)
    {
      (void) max_idx;     //prevent unused argument warnings
      (void) full_vector;
      m_ptr=buffer;
#ifndef NDEBUG
      // debug code
      m_maxIndex=max_idx;
      m_bufferStart=full_vector.data();
      m_bufferEnd=m_bufferStart+full_vector.size();//assumed contiguous
#endif
    }

  public:
    const T_BaseType &operator [](typename std::vector<T_BaseType>::size_type idx) const {
      return m_ptr[optionalRangeCheck(idx)];
    }

    const T_BaseType &upper_bound_at(typename std::vector<T_BaseType>::size_type idx) const {
      return m_ptr[optionalRangeCheckUpperBound(idx)];
    }

  protected:
    T_BufferType *m_ptr;
#ifdef NDEBUG
    typename std::vector<T_BaseType>::size_type optionalRangeCheck(typename std::vector<T_BaseType>::size_type idx) const {
      return idx;
    }
    typename std::vector<T_BaseType>::size_type optionalRangeCheckUpperBound(typename std::vector<T_BaseType>::size_type idx) const {
      return idx;
    }
#else
    // debug code
    typename std::vector<T_BaseType>::size_type optionalRangeCheck(typename std::vector<T_BaseType>::size_type idx) const {
      if (idx>=m_maxIndex || &(m_ptr[idx])<m_bufferStart || &(m_ptr[idx])>=m_bufferEnd) {
        throwRangeError(idx);
      }
      return idx;
    }
    typename std::vector<T_BaseType>::size_type optionalRangeCheckUpperBound(typename std::vector<T_BaseType>::size_type idx) const {
      // allow for idx==0
      if (idx>0 && (idx-1>=m_maxIndex || &(m_ptr[idx-1])<m_bufferStart || &(m_ptr[idx-1])>=m_bufferEnd)) {
        throwRangeError(idx);
      }
      return idx;
    }
    void throwRangeError(typename std::vector<T_BaseType>::size_type idx) const;

    typename std::vector<T_BaseType>::size_type m_maxIndex;
    const T_BaseType *m_bufferStart;
    const T_BaseType *m_bufferEnd;
#endif
  };

  /** Reference to one of the halves provided by the @ref DoubleBuffer_t.
   *
   * This helper class provided read and write access.
   * The base class of this helper class provides compile-time optional range checks.
   */
  class Buffer_t : public BufferBase_t<double,double,std::vector<double> >
  {
  public:
    Buffer_t(std::vector<double> &buffer) : BufferBase_t<double,double, std::vector<double> >(buffer) {}
    Buffer_t(double  *buffer, typename std::vector<double>::size_type max_idx, const std::vector<double> &  full_vector)
      :  BufferBase_t<double,double,std::vector<double> >(buffer,max_idx,full_vector)
    {}

    double &operator [](typename std::vector<double>::size_type idx) {
      return m_ptr[optionalRangeCheck(idx)];
    }

    double &upper_bound_at(typename std::vector<double>::size_type idx) {
      return m_ptr[optionalRangeCheckUpperBound(idx)];
    }
  };

  /** Read only reference to one of the halves provided by the @ref DoubleBuffer_t.
   *
   * The base class of this helper class provides compile-time optional range checks.
   */
  class ConstBuffer_t : public BufferBase_t<double,const double, const std::vector<double> >
  {
  public:
    ConstBuffer_t(const Buffer_t &buffer) :  BufferBase_t<double,const double, const std::vector<double> >(buffer) {}
    ConstBuffer_t(const std::vector<double> &buffer) : BufferBase_t<double,const double, const std::vector<double> >(buffer) {}
    ConstBuffer_t &
    operator=(const Buffer_t &buffer) { BufferBase_t<double,const double, const std::vector<double> >::operator=(buffer); return *this;}
  };


  /** The temporary buffer for internal computations which is split into two halves.
   *
   * The buffer can be used in iterative computations which uses in each iteration
   * the output of the previous one. For such computations, the input is stored in
   * one half and the output in the other half, and the two buffers are swapped after
   * each iteration. Finally the output half of the last iteration is copied to the
   * beginning of the buffer and the buffer is shrunken to its final size.
   */
  class DoubleBuffer_t
  {
  public:
    void clear(unsigned int max_buffer) {
      resize(max_buffer);
      std::fill(m_buffer.begin(),m_buffer.end(),double{});
    }

    Buffer_t operator[](unsigned int fold_idx) {
      std::vector<double>::size_type max_idx=m_buffer.size()/2;
      return Buffer_t(&(m_buffer[(fold_idx%2) * max_idx ]), max_idx,m_buffer);
    }

    /** move the final result to its destination
     *
     * copy the indicated half to the beginning (if necessary) and shrink the buffer to its final size
     * Finally "move" the internal buffer to its final destination.
     */
    std::vector<double> releaseData(std::vector<double>::size_type final_size, unsigned int fold_idx) {
      if (fold_idx%2 !=0 ) {
        std::vector<double>::size_type max_idx=m_buffer.size()/2;
        std::vector<double>::const_iterator first_iter(m_buffer.begin()+max_idx);
        std::vector<double>::const_iterator last_iter(first_iter+max_idx);
        std::copy(first_iter,last_iter, m_buffer.begin());
      }
      m_buffer.resize(final_size);
      return std::move(m_buffer);
    }

  private:
    void resize(unsigned int max_buffer) {
      max_buffer*=2;
      if (max_buffer>m_buffer.size()) {
        m_buffer.resize(max_buffer,double{});
      }
    }

    std::vector<double> m_buffer;
  };

}
#endif
