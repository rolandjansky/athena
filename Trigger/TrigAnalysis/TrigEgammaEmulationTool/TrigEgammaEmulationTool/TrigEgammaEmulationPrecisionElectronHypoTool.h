/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEmulationPrecisionElectronHypoTool_h
#define TrigEgammaEmulationPrecisionElectronHypoTool_h


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"


namespace Trig{


  class TrigEgammaEmulationPrecisionElectronHypoTool: public TrigEgammaEmulationBaseHypoTool,
                                                      virtual public ITrigEgammaEmulationBaseHypoTool
  { 

    ASG_TOOL_CLASS( TrigEgammaEmulationPrecisionElectronHypoTool , ITrigEgammaEmulationBaseHypoTool )

    public:

      TrigEgammaEmulationPrecisionElectronHypoTool(const std::string& myname);
      ~TrigEgammaEmulationPrecisionElectronHypoTool()=default;


      virtual bool emulate( const TrigData &input, bool &pass) const override;

      virtual StatusCode initialize() override;

    private:

      bool decide(   const Trig::TrigData &input, const xAOD::Electron *el) const;
      int findCutIndex( float eta ) const;

      Gaudi::Property< std::string >  m_pidName { this, "PidName", "" };
      Gaudi::Property< float >        m_detacluster { this, "dETACLUSTERthr", 0. , "" };
      Gaudi::Property< float >        m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
      Gaudi::Property< float >        m_RelPtConeCut { this, "RelPtConeCut", -999., "Track isolation cut" };
      Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for PRECISION electron selection:eta bins
      Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
      Gaudi::Property<float>                m_d0{ this,  "d0Cut", -1, "d0 cut" };

      Gaudi::Property<bool>                m_acceptAll{ this,  "AcceptAll", false, "Accept all events" };





  };


}//namespace
#endif
