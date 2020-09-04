//  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoEvent_DataArray
#define L1TopoEvent_DataArray

#include <iostream>

namespace TCS {

   /** @brief base class for TOB arrays
    */
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

#endif
