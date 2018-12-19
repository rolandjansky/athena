/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  DataArray.h
//  L1TopoEvent
//  Created by Joerg Stelzer on 11/22/12.

#ifndef __L1TopoEvent__DataArray__
#define __L1TopoEvent__DataArray__

/** @brief base class for TOB arrays
 **/

#include <iostream>

namespace TCS {
   
   class DataArray {
   public:
      // constructor
      DataArray(const std::string & name) : m_name(name) {};

      // destructor
      virtual ~DataArray(){};

      const std::string & name() const { return m_name; }

      virtual void print(std::ostream&) const = 0;

   private:
      std::string m_name;
   };

std::ostream & operator<< (std::ostream & o, const TCS::DataArray&);
}



#endif /* defined(__L1TopoEvent__DataArray__) */
