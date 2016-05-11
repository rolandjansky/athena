/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaEFElectronSelectorTool_H
#define TrigEgammaEFElectronSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "ElectronPhotonSelectorTools/IAsgElectronIsEMSelector.h"
#include "ElectronPhotonSelectorTools/IAsgElectronLikelihoodTool.h"

// ******* For track isolation ********************************************
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "RecoToolInterfaces/IsolationCommon.h"
#include "RecoToolInterfaces/ITrackIsolationTool.h"
//#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"
// ************************************************************************

#include <vector>

namespace Trig{
class TrigEgammaEFElectronSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaEFElectronSelectorTool,  Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaEFElectronSelectorTool(const std::string& myname);
    ~TrigEgammaEFElectronSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 

    bool emulation( const xAOD::IParticleContainer*, bool&, const std::string&);
  
  private:

    bool ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname);
    bool ApplyOnlElectronPid(const xAOD::Electron *eg, const std::string pidname);
    bool emulatePidSelection(const HLT::TriggerElement *te, const std::string pidname);
    bool emulateIsolation(const HLT::TriggerElement *te);

    // ToolHandles
    // In python order will matter. Should always be tight, medium, loose
    // Order no longer important since using a map
    ///*! Offline isEM Selectors */
    ToolHandleArray<IAsgElectronIsEMSelector>   m_electronOnlIsEMTool;
    /*! Offline LH Selectors */
    ToolHandleArray<IAsgElectronLikelihoodTool> m_electronOnlLHTool;
    /*! Online LH Selector for emulation*/ 
    ToolHandleArray<IAsgElectronLikelihoodTool>  m_electronTrigOnlLHTool;
    /*! Track Isolation Tool */ 
    ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
    
    std::map< std::string, unsigned int > m_PidToolMap; /*! Pass a string to pick up correct selector */

};

}//namespace

#endif
