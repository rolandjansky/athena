/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    bool emulation( const xAOD::IParticleContainer*, bool &, const Trig::Info&);

  private:

    void setTrackPt( float );
    unsigned int getTrackAlgoID( const std::string &);

    const xAOD::EmTauRoIContainer *m_emTauRois; 
    //good emulator parameters
    float m_etThr;
    //tracking cut
    float  m_trackPtthr; //!< pT cut on track
    //calo-tracking cuts
    float m_calotrackdeta; //!<  deta between calo and track
    float m_calotrackdphi;  //!<  dphi between calo and track
    float m_calotrackdeoverp_low;  //!<  E/p lower cut between calo and track
    float m_calotrackdeoverp_high; //!<  E/p upper cut between calo and track 
    //TRT cuts
    float  m_trtratio; //!< cut on ratio of NTRHits/NTRStrawHits for IDScan and SiTrack tracks
 
};

}//namespace

#endif
