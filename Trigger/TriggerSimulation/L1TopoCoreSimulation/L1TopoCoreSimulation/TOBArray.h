/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  TOBArray.h
//  TopoCore
//
//  Created by Joerg Stelzer on 11/17/12.
//

#ifndef __TopoCore__TOBArray__
#define __TopoCore__TOBArray__

#include "L1TopoCoreSimulation/DataArray.h"

#include <iostream>
#include <vector>

namespace TCS {

   class GenericTOB;
   class CompositeTOB;

   typedef bool (sort_fnc)(GenericTOB* tob1, GenericTOB* tob2);

   class TOBArray : public DataArray {
   public:

      typedef std::vector<TCS::GenericTOB*>  data_t;
      typedef data_t::iterator               iterator;
      typedef data_t::const_iterator         const_iterator;
      
      // default constructor
      TOBArray(const std::string & name = "TOBArray") : DataArray(name) {};

      void push_back(const GenericTOB& tob);
      void push_back(const CompositeTOB& tob);

      void clear() { m_data.clear(); }

      void sort(sort_fnc);

      // const access
      const_iterator begin() const { return m_data.begin(); }
      const_iterator end() const { return m_data.end(); }

      // print method (can be invoked via <<)
      virtual void print(std::ostream&) const;
      
   private:
      
      virtual size_t getSize() const { return m_data.size(); }

      virtual void do_pop_back() { m_data.pop_back(); }

      // data vector
      data_t m_data;
   };
   
} // end of namespace TCS

std::ostream & operator<< (std::ostream &, const TCS::TOBArray&);


#endif /* defined(__TopoCore__TOBArray__) */
