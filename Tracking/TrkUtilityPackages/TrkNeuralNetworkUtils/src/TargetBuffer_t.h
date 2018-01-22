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

  /** Helper class which adds a range check to the array access if NDEBUG is not defined.
   */
  class Buffer_t {
    friend class DoubleBuffer_t;
  public:
    Buffer_t(const std::vector<double> &buffer) {
      m_ptr=const_cast<double *>(&(buffer[0]));
#ifndef NDEBUG
      // debug code
      m_orig=buffer;
      m_maxIndex=buffer.size();
      m_bufferStart=&(buffer[0]);
      m_bufferEnd=&(buffer[buffer.size()]);
      m_checkOrig=true;
#endif
    }

  protected:
    // last two arguments are only needed for the optional range check.
    Buffer_t(double *buffer, std::vector<double>::size_type
#ifndef NDEBUG
               max_idx
#endif
             , const std::vector<double> &
#ifndef NDEBUG
               full_vector
#endif
) {
      m_ptr=buffer;
#ifndef NDEBUG
      // debug code
      m_maxIndex=max_idx;
      m_bufferStart=&(full_vector[0]);
      m_bufferEnd=&(full_vector[full_vector.size()]);
      m_checkOrig=false;
#endif
    }

  public:
    ~Buffer_t()
#ifndef NDEBUG
    noexcept(false)
#endif
{
#ifndef NDEBUG
      // debug code
      if (m_checkOrig) {
        if (m_maxIndex != m_orig.size()) {
          throw std::runtime_error("Changed size of const original");
        }
        for (std::vector<double>::size_type i=0; i<m_maxIndex; ++i) {
          if (m_orig[i] != m_ptr[i]) {
            throw std::runtime_error("Changed content of const original");
          }
        }
      }
#endif
    }

  public:
    const double &operator [](std::vector<double>::size_type idx) const {
      return m_ptr[rangeCheck(idx)];
    }
    double &operator [](std::vector<double>::size_type idx) {
      return m_ptr[rangeCheck(idx)];
    }
    const double &upper_bound_at(std::vector<double>::size_type idx) const {
      return m_ptr[rangeCheckUpperBound(idx)];
    }
    double &upper_bound_at(std::vector<double>::size_type idx) {
      return m_ptr[rangeCheckUpperBound(idx)];
    }

  private:
    double *m_ptr;
#ifdef NDEBUG
    std::vector<double>::size_type rangeCheck(std::vector<double>::size_type idx) const {
      return idx;
    }
    std::vector<double>::size_type rangeCheckUpperBound(std::vector<double>::size_type idx) const {
      return idx;
    }
#else
    // debug code
    std::vector<double>::size_type rangeCheck(std::vector<double>::size_type idx) const {
      if (idx>=m_maxIndex || &(m_ptr[idx])<m_bufferStart || &(m_ptr[idx])>=m_bufferEnd) {
        throwRangeError(idx);
      }
      return idx;
    }
    std::vector<double>::size_type rangeCheckUpperBound(std::vector<double>::size_type idx) const {
      // allow for idx==0
      if (idx>0 && (idx-1>=m_maxIndex || &(m_ptr[idx-1])<m_bufferStart || &(m_ptr[idx-1])>=m_bufferEnd)) {
        throwRangeError(idx);
      }
      return idx;
    }
    void throwRangeError(std::vector<double>::size_type idx) const;

    std::vector<double>::size_type m_maxIndex;
    const double *m_bufferStart;
    const double *m_bufferEnd;
    std::vector<double> m_orig;
    bool  m_checkOrig;
#endif
  };

  /** The temporary buffer for internal computations which is split into two halfs for consecutive layers.
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
