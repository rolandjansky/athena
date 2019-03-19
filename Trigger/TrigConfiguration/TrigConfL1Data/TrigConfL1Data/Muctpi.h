/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_Muctpi
#define TrigConf_Muctpi

#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   class Muctpi : public L1DataBaseclass {
   public:
      Muctpi();
      ~Muctpi() = default;
    
      // getters
      int lowptThreshold()    const { return m_LowptThreshold; }
      int highptThreshold()   const { return m_HighptThreshold; }
      int maxCand()           const { return m_MaxCand; }

      // setters
      void setLowptThreshold( int l )     { m_LowptThreshold = l; }
      void setHighptThreshold( int h )    { m_HighptThreshold = h; }
      void setMaxCand( int m )            { m_MaxCand = m; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;
    
   private:
      int  m_LowptThreshold;
      int  m_HighptThreshold;
      int  m_MaxCand;
   };
}

#endif 
  
