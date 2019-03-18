/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_PrescaledClock
#define TrigConf_PrescaledClock

#include <iosfwd>
#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {  
   class PrescaledClock : public L1DataBaseclass  {
   public:
      PrescaledClock();
      ~PrescaledClock() = default;
    
      // setters
      int clock1() const { return m_clock1; }
      int clock2() const { return m_clock2; }
      
      // getters
      void setClock1( int clock1 ) { m_clock1 = clock1; }
      void setClock2( int clock2 ) { m_clock2 = clock2; }
      
      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   private:
      int  m_clock1;
      int  m_clock2;
   };
}

#endif
