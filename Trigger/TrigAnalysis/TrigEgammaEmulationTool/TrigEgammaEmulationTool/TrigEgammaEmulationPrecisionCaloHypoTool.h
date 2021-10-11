/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationPrecisionCaloHypoTool_h
#define TrigEgammaEmulationPrecisionCaloHypoTool_h

#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"


namespace Trig{

  class TrigEgammaEmulationPrecisionCaloHypoTool: public TrigEgammaEmulationBaseHypoTool,
                                                virtual public ITrigEgammaEmulationBaseHypoTool
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationPrecisionCaloHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationPrecisionCaloHypoTool(const std::string& myname);
      ~TrigEgammaEmulationPrecisionCaloHypoTool()=default;

      virtual bool emulate( const TrigData &input, bool &pass) const override;
      
    private:

      bool decide( const TrigData &input, const xAOD::CaloCluster*) const;
      int findCutIndex( float eta ) const;

      Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
      Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
      Gaudi::Property< std::vector<float> > m_et2thr { this, "ET2thr" , {} , "" };  
      Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; 
      Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };

  };

}//namespace
#endif
