/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaEFElectronSelectorTool_H
#define TrigEgammaEFElectronSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronIsEMSelector.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"

// ******* For track isolation ********************************************
//#include "xAODPrimitives/IsolationType.h"
//#include "xAODPrimitives/IsolationCorrection.h"
//#include "RecoToolInterfaces/IsolationCommon.h"
//#include "RecoToolInterfaces/ITrackIsolationTool.h"
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

    bool emulation( const xAOD::IParticleContainer*, bool&, const Trig::Info &);
  
  private:

    bool ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname, double avgmu);
    bool ApplyIsolation(const xAOD::Electron *, bool);

    // ToolHandles
    /*! Offline isEM Selectors */
    ToolHandleArray<IAsgElectronIsEMSelector>   m_electronOnlIsEMTool;
    /*! Offline LH Selectors */
    ToolHandleArray<IAsgElectronLikelihoodTool> m_electronOnlLHTool;
    /*! Online LH Selector for emulation*/ 
    ToolHandleArray<IAsgElectronLikelihoodTool>  m_electronTrigOnlLHTool;
    /*! Track Isolation Tool */ 
    //ToolHandle<xAOD::ITrackIsolationTool> m_trackIsolationTool;
   
    // Likelihood extra info 
    std::string m_lhinfo;

    //Isolation
    unsigned int m_EtConeSizes;
    unsigned int m_PtConeSizes;
    bool m_useClusETforCaloIso;
    bool m_useClusETforTrackIso;
    std::vector<float> m_RelEtConeCut;
    std::vector<float> m_EtConeCut;
    std::vector<float> m_RelPtConeCut;
    std::vector<float> m_RelPtConeCut_var;
    std::vector<float> m_PtConeCut;
    std::map<int, std::string> m_mapEtCone,m_mapPtCone;
    std::map<int, std::string> m_mapRelEtCone,m_mapRelPtCone;

};

}//namespace

#endif
