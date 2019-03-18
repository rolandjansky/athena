// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_BunchGroup
#define TrigConf_BunchGroup

#include <iosfwd>
#include <string>
#include <vector>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
  
   class BunchGroup : public L1DataBaseclass {
   public:
      BunchGroup();
      ~BunchGroup() = default;

      // Accessors
      unsigned int            internalNumber() const { return m_InternalNumber; }
      unsigned int            partition() const { return m_Partition; }
      const std::vector<int>& bunches() const { return m_Bunches; }

      void setInternalNumber( unsigned int i ) { m_InternalNumber = i; }
      void setPartition( unsigned int i ) { m_Partition = i; }
      void addBunch(int);

      void clear();
 
      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
      
   private:
      unsigned int        m_InternalNumber;
      unsigned int        m_Partition;
      std::vector<int>    m_Bunches;
   };

} 

#endif 
  
