// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_DataArrayImpl
#define L1TopoEvent_DataArrayImpl

#include <iostream>
#include <vector>

#include "L1TopoEvent/DataArray.h"

namespace TCS {
   template<class DATA>
   class DataArrayImpl {
   public:

      typedef std::vector<DATA*>                   data_t;
      typedef typename data_t::iterator            iterator;
      typedef typename data_t::const_iterator      const_iterator;

      // constructor
      DataArrayImpl(unsigned int reserve = 0) :
         m_reserve(reserve)
      {
         m_data.reserve(reserve);
      }

      // destructor
      virtual ~DataArrayImpl(){};

      size_t size() const { return m_data.size(); }

      size_t capacity() const { return m_data.capacity(); }

      void clear() { m_data.clear(); }

      virtual void push_back(const DATA& tob);

      void pop_back() { m_data.pop_back(); }

      iterator begin() { return m_data.begin(); }
      const_iterator begin() const { return m_data.begin(); }

      iterator end() { return m_data.end(); }
      const_iterator end() const { return m_data.end(); }

      DATA& operator[](const int i) { return *m_data[i]; }
      const DATA& operator[](const int i) const { return *m_data[i]; }

   protected:
      data_t       m_data;
      unsigned int m_reserve {0};
   };

}

template<class DATA>
void TCS::DataArrayImpl<DATA>::push_back(const DATA& tob) {
   m_data.push_back(DATA::createOnHeap(tob));
}

#endif
