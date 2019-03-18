/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_BunchGroupSet
#define TrigConf_BunchGroupSet

#include "TrigConfL1Data/BunchGroup.h"
#include "TrigConfL1Data/L1DataBaseclass.h"

#include <iosfwd>
#include <string>
#include <vector>
#include <inttypes.h>
#include <sys/types.h>

namespace TrigConf {
  
   class BunchGroupSet : public L1DataBaseclass {
   public:
      BunchGroupSet();
      BunchGroupSet(const std::vector<BunchGroup>&);
      ~BunchGroupSet() = default;

      // getters
      uint16_t menuPartition() const { return m_MenuPartition; }
      const std::vector<BunchGroup>& bunchGroups() const { return m_BunchGroups; }
      const std::vector<uint16_t>& bgPattern() const { return m_BGpattern; }

      // setters
      void setMenuPartition(uint16_t part) { m_MenuPartition = part; }
      void setBGName(uint32_t, const std::string&);
      void addBunchGroup(const BunchGroup&);
 

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;


   private:
      void fillPattern(uint32_t bitpos, const BunchGroup& bg);

      uint16_t                   m_MenuPartition;
      std::vector<BunchGroup>    m_BunchGroups;
      std::vector<uint16_t>      m_BGpattern;
   };

}

#endif 
  
