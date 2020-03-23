/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaEFCaloSelectorTool_H
#define TrigEgammaEFCaloSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include <vector>

namespace Trig{
class TrigEgammaEFCaloSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaEFCaloSelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:
    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaEFCaloSelectorTool(const std::string& myname);
    ~TrigEgammaEFCaloSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 

    bool emulation( const xAOD::IParticleContainer*, bool&, const Trig::Info &);


  private:

    bool ApplyCaloPid(const xAOD::Egamma *, const std::string, double);
    bool ApplyCaloPid(const xAOD::Egamma *eg, const std::string pidname){
      return ApplyCaloPid(eg,pidname,-99);
    }

    // ToolHandles
    // In python order will matter. Should always be tight, medium, loose
    // Order no longer important since using a map
    ///*! Offline isEM Selectors */
    ToolHandleArray<IAsgElectronIsEMSelector>   m_electronCaloIsEMTool;
    /*! Offline LH Selectors */
    ToolHandleArray<IAsgElectronLikelihoodTool> m_electronCaloLHTool; 

    std::map< std::string, unsigned int > m_PidToolMap; /*! Pass a string to pick up correct selector */
    //ToolHandle<IEMFourMomBuilder> m_fourMomBuilder;
    //ToolHandle<IEMShowerBuilder> m_showerBuilder;
    
    std::string m_lhinfo;

};

}//namespace

#endif
