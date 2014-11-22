/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_CaloSinCos
#define TrigConf_CaloSinCos

#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {

   class CaloSinCos : public L1DataBaseclass {
   public:

      CaloSinCos( const std::string& n, const int v,
                  int val1, int val2, int val3, int val4,
                  int val5, int val6, int val7, int val8,
                  int phimin, int phimax,int etamin,int etamax);

      CaloSinCos();

      virtual ~CaloSinCos();

      // getters
      int val1() const { return m_Val1; }
      int val2() const { return m_Val2; }
      int val3() const { return m_Val3; }
      int val4() const { return m_Val4; }
      int val5() const { return m_Val5; }
      int val6() const { return m_Val6; }
      int val7() const { return m_Val7; }
      int val8() const { return m_Val8; }
      int etamin() const { return m_EtaMin; }
      int etamax() const { return m_EtaMax; }
      int phimin() const { return m_PhiMin; }
      int phimax() const { return m_PhiMax; }

      // setters
      void setVal1( int val) { m_Val1 = val; }
      void setVal2( int val) { m_Val2 = val; }
      void setVal3( int val) { m_Val3 = val; }
      void setVal4( int val) { m_Val4 = val; }
      void setVal5( int val) { m_Val5 = val; }
      void setVal6( int val) { m_Val6 = val; }
      void setVal7( int val) { m_Val7 = val; }
      void setVal8( int val) { m_Val8 = val; }
      void setEtaMin( int val) { m_EtaMin = val; }
      void setEtaMax( int val) { m_EtaMax = val; }
      void setPhiMin( int val) { m_PhiMin = val; }
      void setPhiMax( int val) { m_PhiMax = val; }
	
      virtual void print(const std::string& indent="", unsigned int detail=1) const;

   protected:
      int m_Val[8];
      int m_Val1;
      int m_Val2;
      int m_Val3;
      int m_Val4;
      int m_Val5;
      int m_Val6;
      int m_Val7;
      int m_Val8;
      int m_PhiMin;
      int m_PhiMax;
      int m_EtaMin;
      int m_EtaMax;
      
   };

}

#endif 
  
