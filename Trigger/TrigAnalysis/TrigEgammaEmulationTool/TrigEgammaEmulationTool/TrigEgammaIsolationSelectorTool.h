/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaIsolationSelectorTool_H
#define TrigEgammaIsolationSelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"

namespace Trig{
class TrigEgammaIsolationSelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  
  {
  ASG_TOOL_CLASS(TrigEgammaIsolationSelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaIsolationSelectorTool(const std::string& myname);
    ~TrigEgammaIsolationSelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 
    
    //bool emulation( const xAOD::Electron*, bool &, const Trig::Info &);
    //bool emulation( const xAOD::Photon*  , bool &, const Trig::Info &);
    bool emulation(const xAOD::IParticle*  , bool &, const Trig::Info &);
    
  private:
    
    //Isolation
    unsigned int m_EtConeSizes;
    unsigned int m_PtConeSizes;
    bool m_useClusETforCaloIso;
    bool m_useClusETforTrackIso;
    std::vector<float> m_RelEtConeCut;
    std::vector<float> m_EtConeCut;
    std::vector<float> m_RelPtConeCut;
    //std::vector<float> m_RelPtConeCut_var;
    std::vector<float> m_PtConeCut;
    std::map<int, std::string> m_mapEtCone,m_mapPtCone;
    std::map<int, std::string> m_mapRelEtCone,m_mapRelPtCone;

};

}//namespace

#endif
