#ifndef TrigEgammaEmulationFastElectronHypoTool_h
#define TrigEgammaEmulationFastElectronHypoTool_h


#include "AsgTools/AsgTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationToolMT.h"
#include "TrigEgammaEmulationTool/TrigEgammaEmulationBaseHypoTool.h"
#include "TrigEgammaEmulationTool/ITrigEgammaEmulationBaseHypoTool.h"


namespace Trig{


  class TrigEgammaEmulationFastElectronHypoTool: 
          public TrigEgammaEmulationBaseHypoTool,
          virtual public ITrigEgammaEmulationBaseHypoTool
  { 
    ASG_TOOL_CLASS( TrigEgammaEmulationFastElectronHypoTool , ITrigEgammaEmulationBaseHypoTool)

    public:

      TrigEgammaEmulationFastElectronHypoTool(const std::string& myname);
      ~TrigEgammaEmulationFastElectronHypoTool()=default;

      virtual bool emulate(  const Trig::TrigData &input, bool &pass ) const override;
      

    private:

        bool decide( const Trig::TrigData &input , const xAOD::TrigElectron* electron) const;

        Gaudi::Property<bool>         m_doRinger{ this, "DoRinger", false, "apply ringer selection" };
        Gaudi::Property<std::string>  m_pidName{ this, "PidName", "", "pid name" };

        Gaudi::Property<bool>   m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
        Gaudi::Property<bool>   m_doLRT{ this, "DoLRT", false, "apply d0 cut for LRT triggers" };
        Gaudi::Property<float>  m_trackPt{ this, "TrackPt",  { float( 5.0*Gaudi::Units::GeV ) }, "Track pT requirement ( separate threshold for each electron )" };
        Gaudi::Property<float>  m_caloTrackDEta{ this,  "CaloTrackdETA", 0, "Delta Eta between the track and cluster"      }; //loose cut
        Gaudi::Property<float>  m_caloTrackDPhi{ this,  "CaloTrackdPHI", 0, "Delta Phi between track and cluster"     }; //loose cut
        Gaudi::Property<float>  m_caloTrackdEoverPLow{ this,  "CaloTrackdEoverPLow", 0, "Min E over Pt cut "};
        Gaudi::Property<float>  m_caloTrackdEoverPHigh{ this,  "CaloTrackdEoverPHigh", 0, "Max E over pT cut" };
        Gaudi::Property<float>  m_trtRatio{ this,  "TRTRatio", 0, "TRT HT ratio" };
        Gaudi::Property<float>  m_trkd0{ this,  "d0Cut", 0, "d0 cut" };

  };


}//namespace
#endif
