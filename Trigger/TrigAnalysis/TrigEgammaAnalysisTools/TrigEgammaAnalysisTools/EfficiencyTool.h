/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EfficiencyTool_H
#define EfficiencyTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"

class EfficiencyTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(EfficiencyTool, ITrigEgammaAnalysisBaseTool)

public:

  EfficiencyTool( const std::string& myname );
  ~EfficiencyTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
  StatusCode toolExecute(const std::string,const TrigInfo,std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>> pairObjs);
  void setDetail(bool doDetail){ m_detailedHists = doDetail; }
  void setTP(bool tp){ m_tp = tp; }
protected:
private:
  void fillEfficiency(const std::string,bool,const float,const std::string,const xAOD::Egamma *,bool fill2D=true);
  void inefficiency(const std::string,const unsigned int, const unsigned int,const float,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void fillInefficiency(const std::string,const xAOD::Electron *,const xAOD::Photon *,const xAOD::CaloCluster *,const xAOD::TrackParticle *); 

  void analyseIsEM(const xAOD::Electron *,const std::string,const std::bitset<4>);
  void analyseIsEMLH(const xAOD::Electron *,const std::string/*,const std::bitset<4>*/);
  /*! Include more detailed histograms */
  bool m_detailedHists;
};

#endif
