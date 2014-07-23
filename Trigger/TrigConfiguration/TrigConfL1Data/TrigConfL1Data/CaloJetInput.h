/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_CaloJetInput
#define TrigConf_CaloJetInput

#include <iosfwd>
#include <string>
#include "TrigConfL1Data/L1DataBaseclass.h"

namespace TrigConf {
  
   class CaloJetInput : public L1DataBaseclass {
      
   public:
      
      CaloJetInput( const std::string& name,
                    unsigned int version,
                    const std::string& type, 
                    int ptcut,
                    int phimin,
                    int phimax,
                    int etamin,
                    int etamax);
      CaloJetInput();
      
      virtual ~CaloJetInput();
      
      // getters
      const std::string& type() const { return m_Type; }      
      int ptcut() const { return m_Ptcut; }
      int phimin() const { return m_PhiMin; }
      int phimax() const { return m_PhiMax; }
      int etamin() const { return m_EtaMin; }
      int etamax() const { return m_EtaMax; }

      // setters
      void setType( const std::string& type ) { m_Type = type; }
      void setPtcut( int ptcut ) { m_Ptcut = ptcut; }
      void setPhiMin( int phimin ) { m_PhiMin = phimin; }
      void setPhiMax( int phimax ) { m_PhiMax = phimax; }
      void setEtaMin( int etamin ) { m_EtaMin = etamin; }
      void setEtaMax( int etamax ) { m_EtaMax = etamax; }
	
      virtual void print(const std::string& indent="", unsigned int detail=1) const;

   private:

      std::string m_Type;
      int m_Ptcut;
      int m_PhiMin;
      int m_PhiMax;
      int m_EtaMin;
      int m_EtaMax;
      
   };

}

#endif 
  
