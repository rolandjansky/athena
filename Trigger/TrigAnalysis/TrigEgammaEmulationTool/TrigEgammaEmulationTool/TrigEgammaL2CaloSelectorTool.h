/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TrigEgammaL2CaloSelectorTool_H
#define TrigEgammaL2CaloSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>

namespace Trig{
class TrigEgammaL2CaloSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaL2CaloSelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaL2CaloSelectorTool(const std::string& myname);
    ~TrigEgammaL2CaloSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 
    bool emulation( const xAOD::TrigEMCluster*, bool &, const Trig::Info &);

  private:

    bool emulationL2( const xAOD::TrigEMCluster*, bool &);
    void setEtThr( float et ){m_eTthr=et;};

    const xAOD::EmTauRoIContainer *m_emTauRois; 

    //good emulator parameters
    std::string m_pidname;
    std::string m_signature;
    std::string m_str_etthr;
 
    //Calorimeter electron ID  cuts
    std::vector<float> m_etabin;
    std::vector<float> m_hadeTthr;
    std::vector<float> m_carcorethr;
    std::vector<float> m_caeratiothr;
    
    float m_hadeT2thr;
    float m_eTthr;
    float m_eT2thr;
    float m_F1thr;
    float m_WETA2thr;
    float m_WSTOTthr;
    float m_F3thr;
    float m_detacluster;
    float m_dphicluster;
};

}//namespace

#endif
