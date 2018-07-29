/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//
//  DataArray.h
//  L1TopoCoreSimulation
//
//  Created by Joerg Stelzer on 11/22/12.
//

#ifndef __L1TopoCoreSimulation__DataArray__
#define __L1TopoCoreSimulation__DataArray__

#include <iostream>

namespace TCS {
   
   class DataArray {
   public:
      // constructor
      DataArray(const std::string & name) :
         m_name(name)
      {}

      // destructor
      virtual ~DataArray(){};

      const std::string & name() const { return m_name; }

      size_t size() const { return getSize(); }

      void pop_back() { do_pop_back(); }

      // print method implementation is required
      virtual void print(std::ostream&) const = 0;

   private:
      virtual size_t getSize() const = 0;

      virtual void do_pop_back() = 0;

      std::string m_name;
   };
}

std::ostream & operator<< (std::ostream & o, const TCS::DataArray&);


#endif /* defined(__L1TopoCoreSimulation__DataArray__) */
