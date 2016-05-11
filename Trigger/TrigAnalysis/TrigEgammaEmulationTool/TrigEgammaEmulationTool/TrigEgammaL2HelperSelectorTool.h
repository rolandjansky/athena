/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaL2HelperSelectorTool_H
#define TrigEgammaL2HelperSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloRingerSelectorTool.h"
//#include "TrigEgammaEmulationTool/TrigEgammaL2ElectronSelectorBaseTool.h"
//#include "TrigEgammaEmulationTool/TrigEgammaL2EPhotonSelectorBaseTool.h"

#include "AsgTools/AsgTool.h"


namespace Trig{
class TrigEgammaL2HelperSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaL2HelperSelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaL2HelperSelectorTool(const std::string& myname);
    ~TrigEgammaL2HelperSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 
    
    bool emulation( const xAOD::TrigEMCluster         *, bool &, const std::string&);
    bool emulation( const xAOD::IParticleContainer*    , bool &, const std::string&);

  private:

    ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_caloSelectors;
    ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_caloRingerSelectors;
    //ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_l2ElectronSelectors
};

}//namespace

#endif
