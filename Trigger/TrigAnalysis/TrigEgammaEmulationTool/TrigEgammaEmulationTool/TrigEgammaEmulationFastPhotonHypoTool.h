#ifndef TrigEgammaEmulationFastPhotonHypoTool_h
#define TrigEgammaEmulationFastPhotonHypoTool_h


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"


namespace Trig{


  class TrigEgammaEmulationFastPhotonHypoTool: 
          public TrigEgammaEmulationBaseHypoTool,
          virtual public ITrigEgammaEmulationBaseHypoTool
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationFastPhotonHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationFastPhotonHypoTool(const std::string& myname);
      ~TrigEgammaEmulationFastPhotonHypoTool()=default;

      virtual bool emulate(  const Trig::TrigData &input, bool &pass ) const override;


    private:
        int findCutIndex( float eta ) const;
        bool decide(  const Trig::TrigData &input ) const;
        
        Gaudi::Property<bool>                 m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
        Gaudi::Property<float>                m_detacluster { this, "dETACLUSTERthr", 0. , "" };
        Gaudi::Property<float>                m_dphicluster { this, "dPHICLUSTERthr", 0. , "" }; 
        Gaudi::Property< std::vector<float> > m_eTthr       { this, "ETthr"         , {} , "" };  
        Gaudi::Property< std::vector<float> > m_eT2thr { this, "ET2thr", {}, "Second layer ET threshold" };
        Gaudi::Property< std::vector<float> > m_hadeTthr { this, "HADETthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_hadeT2thr { this, "HADET2thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_carcorethr { this, "CARCOREthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_caeratiothr { this, "CAERATIOthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_F1thr { this, "F1thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_WETA2thr { this, "WETA2thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_WSTOTthr { this, "WSTOTthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_F3thr { this, "F3thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; 


  };


}//namespace
#endif
