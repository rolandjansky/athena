/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaEFPhotonSelectorTool_H
#define TrigEgammaEFPhotonSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IAsgPhotonIsEMSelector.h"

#include <vector>

namespace Trig{
class TrigEgammaEFPhotonSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  {
  ASG_TOOL_CLASS(TrigEgammaEFPhotonSelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaEFPhotonSelectorTool(const std::string& myname);
    ~TrigEgammaEFPhotonSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 

    bool emulation( const xAOD::IParticleContainer*, bool&, const Trig::Info &);

  private:

    bool ApplyPhotonPid(const xAOD::Photon *eg, const std::string pidname);

    // ToolHandles
    /* In python order will matter. Should always be tight, medium, loose
       Order no longer important since using a map */
    std::map< std::string, unsigned int > m_PidToolMap; /*! Pass a string to pick up correct selector */
    /* Photon selectors */
    ToolHandleArray<IAsgPhotonIsEMSelector>   m_photonOnlIsEMTool;
  


};

}//namespace

#endif
