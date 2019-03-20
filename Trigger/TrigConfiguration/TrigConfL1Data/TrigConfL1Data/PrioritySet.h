/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_PrioritySet
#define TrigConf_PrioritySet

// STL etc.
#include <string>
#include <vector>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   class PrioritySet : public L1DataBaseclass  {
      
   public:

      static const unsigned int N_PRIORITIES = 256;

      PrioritySet();
      ~PrioritySet() = default;

      // Accessors
      const std::vector<std::string>& priorities() const { return m_Priorities; }
      void setPriorities( const std::string p[], const unsigned int size);
      void setPriorities(const std::vector<std::string>& vec);

      virtual void print(const std::string& indent="", unsigned int detail=1) const;

      virtual void writeXML(std::ostream & xmlfile, int indentLevel = 0, int indentWidth = 2) const;

   private:
      std::vector<std::string>   m_Priorities;
   };
}
#endif 
  
