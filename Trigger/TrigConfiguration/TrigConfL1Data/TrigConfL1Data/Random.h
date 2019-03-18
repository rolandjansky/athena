// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_Random
#define TrigConf_Random

#include <iosfwd>
#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
   class Random : public L1DataBaseclass {
   public:
      Random();
      ~Random() = default;
    
      // getters
      uint32_t              cuts(unsigned int idx) const { return m_cuts[idx]; }
      const std::string &   rndmName(unsigned int idx) const { return m_names[idx]; }
      int                   rate1() const { return m_rate1; }
      int                   rate2() const { return m_rate2; }
      float                 getRateFromCut(unsigned int idx) const;

      // setters
      void setRndmName( uint32_t idx, const std::string& name ) { m_names[idx] = name; }
      void setCut( uint32_t idx, uint32_t cut ) { m_cuts[idx] = cut; }
      void setRate1( int rate ) { m_rate1 = rate; }
      void setRate2( int rate ) { m_rate2 = rate; }   

      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ostream & xmlfile, int indentLevel=0, int indentWidth=2) const;

   private:
      int         m_rate1;
      int         m_rate2;
      std::string m_names[4];
      uint32_t    m_cuts[4];
   };
}
#endif
