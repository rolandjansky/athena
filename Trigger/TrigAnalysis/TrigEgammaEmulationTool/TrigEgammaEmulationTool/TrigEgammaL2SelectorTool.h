/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaL2SelectorTool_H
#define TrigEgammaL2SelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloRingerSelectorTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaL2ElectronSelectorTool.h"
//#include "TrigEgammaEmulationTool/TrigEgammaL2EPhotonSelectorBaseTool.h"

#include "AsgTools/AsgTool.h"


namespace Trig{
class TrigEgammaL2SelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaL2SelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaL2SelectorTool(const std::string& myname);
    ~TrigEgammaL2SelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 
    
    bool emulation( const xAOD::TrigEMCluster         *, bool &, const Trig::Info&);
    bool emulation( const xAOD::IParticleContainer    *, bool &, const Trig::Info&);

  private:

    ToolHandle<Trig::ITrigEgammaSelectorBaseTool>      m_electronSelector;
    ToolHandle<Trig::ITrigEgammaSelectorBaseTool>      m_photonSelector;
    ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_caloCutIDSelectors;
    ToolHandleArray<Trig::ITrigEgammaSelectorBaseTool> m_caloRingerSelectors;

};

}//namespace

#endif
