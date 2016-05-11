/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    bool emulation( const xAOD::TrigEMCluster*, bool &, const std::string&);

  private:

    bool is_correct_trigger(const std::string&);
    bool emulationL2( const xAOD::TrigEMCluster*, bool &);

    const xAOD::EmTauRoIContainer *m_emTauRois; 

    //good emulator parameters
    std::string m_pidname;
    std::string m_signature;
    std::string m_str_etthr;
 
    //Calorimeter electron ID  cuts
    std::vector<float> m_etabin;
    std::vector<float> m_eTthr;
    std::vector<float> m_eT2thr;
    std::vector<float> m_hadeTthr;
    std::vector<float> m_hadeT2thr;
    std::vector<float> m_carcorethr;
    std::vector<float> m_caeratiothr;
    std::vector<float> m_F1thr;
    std::vector<float> m_WETA2thr;
    std::vector<float> m_WSTOTthr;
    std::vector<float> m_F3thr;
    float m_detacluster;
    float m_dphicluster;
};

}//namespace

#endif
