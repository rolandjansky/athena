/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef TrigEgammaL1SelectorTool_H
#define TrigEgammaL1SelectorTool_H

#include "TrigEgammaEmulationTool/ITrigEgammaSelectorBaseTool.h"
#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
#include "AsgTools/AsgTool.h"

namespace Trig{
class TrigEgammaL1SelectorTool: 
  public Trig::TrigEgammaSelectorBaseTool,///TDT utilities
  virtual public Trig::ITrigEgammaSelectorBaseTool 
  
  {
  ASG_TOOL_CLASS(TrigEgammaL1SelectorTool, Trig::ITrigEgammaSelectorBaseTool)

  public:

    using TrigEgammaSelectorBaseTool::emulation;
    //****************************************************************************** 
    TrigEgammaL1SelectorTool(const std::string& myname);
    ~TrigEgammaL1SelectorTool() {};

    StatusCode initialize();
    StatusCode finalize(); 
    
    bool emulation( const xAOD::EmTauRoI*, bool &, const Trig::Info &);
    
  private:

    ///isolation for level one using linear region cuts.
    float emulationL1V(const std::string L1item, const float l1eta);
    bool isolationL1(const float min, const float offset, const float slope, const float energy, const float emE);
    bool variableEtL1(const std::string L1item, const float l1energy, const float l1eta);

    float                     m_isolMaxCut;
    std::vector<float>        m_hadCoreCutMin;
    std::vector<float>        m_hadCoreCutOff;
    std::vector<float>        m_hadCoreSlope;
    std::vector<float>        m_emIsolCutMin;
    std::vector<float>        m_emIsolCutOff;
    std::vector<float>        m_emIsolCutSlope;
    std::vector<std::string>  m_wpNames;

};

}//namespace

#endif
