#ifndef TrigEgammaEmulationFastCaloHypoTool_h
#define TrigEgammaEmulationFastCaloHypoTool_h


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"


namespace Trig{


  class TrigEgammaEmulationFastCaloHypoTool: 
          public TrigEgammaEmulationBaseHypoTool,
          virtual public ITrigEgammaEmulationBaseHypoTool
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationFastCaloHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationFastCaloHypoTool(const std::string& myname);
      ~TrigEgammaEmulationFastCaloHypoTool()=default;


      /* emulate */
      virtual bool emulate(  const Trig::TrigData &input, bool &pass ) const override;

      virtual StatusCode initialize() override;

    private:

        bool decide(  const Trig::TrigData &input ) const;
        bool decide_ringer(  const Trig::TrigData &input ) const;

        int findCutIndex( float eta ) const;

        Gaudi::Property< bool >               m_acceptAll { this, "AcceptAll", false , "Accept all" };
        Gaudi::Property< bool >               m_useRinger { this, "UseRinger", false , "Use Ringer Selection" };
        Gaudi::Property<std::string>          m_pidName{this,"PidName", "", "Pid name"};
        Gaudi::Property<float>                m_emEtCut{this,"EtCut", 0.0, "Et threshold"};


        //Calorimeter electron ID  cuts
        Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for L2 calo selection:eta bins
        Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
        Gaudi::Property< std::vector<float> > m_eT2thr { this, "ET2thr", {}, "Second layer ET threshold" };
        Gaudi::Property< std::vector<float> > m_hadeTthr { this, "HADETthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_hadeT2thr { this, "HADET2thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_carcorethr { this, "CARCOREthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_caeratiothr { this, "CAERATIOthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_F1thr { this, "F1thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_WETA2thr { this, "WETA2thr", {}, "" };
        Gaudi::Property< std::vector<float> > m_WSTOTthr { this, "WSTOTthr", {}, "" };
        Gaudi::Property< std::vector<float> > m_F3thr { this, "F3thr", {}, "" };
        Gaudi::Property< float >              m_detacluster { this, "dETACLUSTERthr", 0. , "" };
        Gaudi::Property< float >              m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  





  };


}//namespace
#endif
