/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  ClusterTOBArray.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/17/12.
//

#ifndef __TopoCore__ClusterTOBArray__
#define __TopoCore__ClusterTOBArray__

#include <iostream>
#include "L1TopoCoreSimulation/InputTOBArray.h"
#include "L1TopoCoreSimulation/ClusterTOB.h"
#include <vector>

namespace TCS {
   
   class ClusterTOBArray : public InputTOBArray {
   public:
      
      typedef std::vector<TCS::ClusterTOB*>  data_t;
      typedef data_t::iterator               iterator;
      typedef data_t::const_iterator         const_iterator;
      
      // default constructor
      ClusterTOBArray(const std::string & name = "ClusterTOBArray") : InputTOBArray(name) {};
      
      void push_back(const TCS::ClusterTOB& tob);

      void clear() { m_data.clear(); }
      
      //iterator begin() { return m_data.begin(); }
      const_iterator begin() const { return m_data.begin(); }
      
      //iterator end() { return m_data.end(); }
      const_iterator end() const { return m_data.end(); }
      
   private:

      // print method (can be invoked via <<)
      virtual void print(std::ostream&) const;

      virtual size_t getSize() const { return m_data.size(); }

      virtual void do_pop_back() { m_data.pop_back(); }

      data_t m_data;
   };
   
} // end of namespace TCS

#endif /* defined(__TopoCore__ClusterTOBArray__) */
