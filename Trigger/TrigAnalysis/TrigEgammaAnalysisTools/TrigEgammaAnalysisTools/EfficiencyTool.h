/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  void setEmulation(bool doEmu){ m_doEmulation = doEmu;}

private:

protected:
  void fillEfficiency(const std::string,bool,const float,const std::string,const xAOD::Egamma *,bool fill2D=true);
  void inefficiency(const std::string&,const std::string,const float,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj,
                    const asg::AcceptData& acceptData);
  void fillInefficiency(const std::string&,const std::string,const xAOD::Electron *,const xAOD::Photon *,const xAOD::CaloCluster *,const xAOD::TrackParticle *,
                        const asg::AcceptData& acceptData); 
  bool analyseIsEM(const xAOD::Electron *,const std::string);
  bool analyseIsEMLH(const xAOD::Electron *,const std::string/*,const std::bitset<4>*/);
   /*! Include more detailed histograms */
  bool m_detailedHists;
  bool m_doEmulation;

};

#endif
