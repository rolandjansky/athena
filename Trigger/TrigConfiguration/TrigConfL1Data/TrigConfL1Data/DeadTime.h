/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_DeadTime
#define TrigConf_DeadTime

#include <iosfwd>
#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   class DeadTime : public L1DataBaseclass {
   public:
      DeadTime();
      ~DeadTime() = default;
    
      // getters
      int simple()        const { return m_simple; }
      int complex1Level() const { return m_complex1Level; }
      int complex1Rate()  const { return m_complex1Rate; }
      int complex2Level() const { return m_complex2Level; }
      int complex2Rate()  const { return m_complex2Rate; }

      // setters
      void setSimple       ( int i ) { m_simple = i; }
      void setComplex1Level( int i ) { m_complex1Level = i; }
      void setComplex1Rate ( int i ) { m_complex1Rate = i; }
      void setComplex2Level( int i ) { m_complex2Level = i; }
      void setComplex2Rate ( int i ) { m_complex2Rate = i; }

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   private:
      int  m_simple;
      int  m_complex1Level;
      int  m_complex1Rate;
      int  m_complex2Level;
      int  m_complex2Rate;
   };
}

#endif
