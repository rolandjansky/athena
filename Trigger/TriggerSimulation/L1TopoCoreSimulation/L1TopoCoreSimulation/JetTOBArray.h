/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  JetTOBArray.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/17/12.
//

#ifndef __TopoCore__JetTOBArray__
#define __TopoCore__JetTOBArray__

#include <iostream>
#include "L1TopoCoreSimulation/InputTOBArray.h"
#include "L1TopoCoreSimulation/JetTOB.h"
#include <vector>

namespace TCS {
   
   class JetTOBArray : public InputTOBArray {
   public:
      
      typedef std::vector<TCS::JetTOB*>  data_t;
      typedef data_t::iterator           iterator;
      typedef data_t::const_iterator     const_iterator;
      
      // default constructor
      JetTOBArray(const std::string & name = "JetTOBArray") : InputTOBArray(name) {};
      
      void push_back(const TCS::JetTOB& tob);

      void clear() { m_data.clear(); }
      
      const_iterator begin() const { return m_data.begin(); }
      const_iterator end() const { return m_data.end(); }
      
   private:
      
      // print method can be invoked via <<
      virtual void print(std::ostream&) const;

      virtual size_t getSize() const { return m_data.size(); }

      virtual void do_pop_back() { m_data.pop_back(); }

      data_t m_data;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__JetTOBArray__) */
