/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaResolutionTool_H
#define TrigEgammaResolutionTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"

class TrigEgammaResolutionTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaResolutionTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaResolutionTool( const std::string& myname );
  ~TrigEgammaResolutionTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
  StatusCode toolExecute(const std::string,const TrigInfo,std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>> pairObjs);
  void setDetail(bool doDetail){ m_detailedHists = doDetail; }
  void setTP(bool tp){ m_tp = tp; }
protected:
private:

  void fillL2CaloResolution(const std::string, const xAOD::TrigEMCluster *,const xAOD::Egamma *);
  void fillHLTResolution(const std::string, const xAOD::Egamma *,const xAOD::Egamma *);
  void fillIsolationResolution(const std::string, const xAOD::Egamma *,const xAOD::Egamma *);
  void fillHLTAbsResolution(const std::string, const xAOD::Egamma *,const xAOD::Egamma *);
  void fillL1CaloResolution(const std::string, const xAOD::EmTauRoI*, const xAOD::Egamma*);
  void fillL1CaloAbsResolution(const std::string, const xAOD::EmTauRoI*, const xAOD::Egamma*);
  
  /*! Resolution methods */
  void resolutionPhoton(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj,bool);
  void resolutionElectron(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj,bool);
  void resolutionL2Photon(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionL2Electron(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
  void resolutionEFCalo(const std::string,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj);
   /*! Include more detailed histograms */
  bool m_detailedHists;
};

#endif
