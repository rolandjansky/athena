/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaL2ElectronSelectorTool_H
#define TrigEgammaL2ElectronSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>

namespace Trig{
class TrigEgammaL2ElectronSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaL2ElectronSelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaL2ElectronSelectorTool(const std::string& myname);
    ~TrigEgammaL2ElectronSelectorTool();

    StatusCode initialize();
    StatusCode finalize(); 
    bool emulation( const xAOD::IParticleContainer*, bool &, const TrigInfo&);

  private:

    bool is_correct_trigInfo(const TrigInfo&);

    const xAOD::EmTauRoIContainer *m_emTauRois; 

    //good emulator parameters
    float m_etThr;
    std::string m_pidname;
    std::string m_signature;
    std::string m_str_etthr;

    // Properties:
    unsigned int m_trackalgoID; //!<  integer that assigns the tracking algorihtm used
    //tracking cut
    float  m_trackPtthr; //!< pT cut on track
    //calo-tracking cuts
    std::vector<float> m_etabin;
    std::vector<float> m_calotrackdeta; //!<  deta between calo and track
    std::vector<float> m_calotrackdphi;  //!<  dphi between calo and track
    std::vector<float> m_calotrackdeoverp_low;  //!<  E/p lower cut between calo and track
    std::vector<float> m_calotrackdeoverp_high; //!<  E/p upper cut between calo and track 
    //TRT cuts
    std::vector<float> m_trtratio; //!< cut on ratio of NTRHits/NTRStrawHits for IDScan and SiTrack tracks
    //TRTSegFinder cuts
    float  m_trackPtthrTRT; //!< pT cut on track
    std::vector<float> m_etabinTRT; //!< eta bins for TRT cuts
    std::vector<float> m_trtratioTRT; //!< cut on ratio of NTRHits/NTRStrawHits for TRTSegFinder tracks
    std::vector<float> m_calotrackdetaTRT; //!<  deta between calo and track
    std::vector<float> m_calotrackdphiTRT;  //!<  dphi between calo and track
    std::vector<float> m_calotrackdeoverp_lowTRT;  //!<  E/p lower cut between calo and track
    std::vector<float> m_calotrackdeoverp_highTRT; //!<  E/p upper cut between calo and track 
 
};

}//namespace

#endif
