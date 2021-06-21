/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationPrecisionPhotonHypoTool_h
#define TrigEgammaEmulationPrecisionPhotonHypoTool_h


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationInfo.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"



namespace Trig{


  class TrigEgammaEmulationPrecisionPhotonHypoTool: public TrigEgammaEmulationBaseHypoTool,
                                                    virtual public ITrigEgammaEmulationBaseHypoTool
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationPrecisionPhotonHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationPrecisionPhotonHypoTool(const std::string& myname);
      ~TrigEgammaEmulationPrecisionPhotonHypoTool()=default;

      virtual bool emulate( const TrigData &input, bool &pass) const override;
      
    
    private:

      bool decide(   const TrigData &input, const xAOD::Photon *ph ) const;
      
      int findCutIndex( float eta ) const;

      Gaudi::Property< std::string >  m_pidName { this, "PidName", "" };
      Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
      Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };
      Gaudi::Property< float > m_RelEtConeCut { this, "RelEtConeCut", -999. , "Calo isolation cut" };
      Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; 
      Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };

  };


}//namespace
#endif
