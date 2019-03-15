/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigEgammaDistTool_H
#define TrigEgammaDistTool_H

#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"

class TrigEgammaDistTool
: public TrigEgammaAnalysisBaseTool,
    virtual public ITrigEgammaAnalysisBaseTool {
        ASG_TOOL_CLASS(TrigEgammaDistTool, ITrigEgammaAnalysisBaseTool)

public:

  TrigEgammaDistTool( const std::string& myname );
  ~TrigEgammaDistTool() {};

  StatusCode childInitialize();
  StatusCode childBook();
  StatusCode childExecute();
  StatusCode childFinalize();
  StatusCode toolExecute(const std::string,const TrigInfo,std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>> pairObjs);
  void setDetail(bool doDetail){ m_detailedHists = doDetail; }
  void setTP(bool tp){ m_tp = tp; }
protected:
private:
  /*! fill kinematic histograms, et,eta,phi,lumi and efficiency */
  void fillShowerShapes(const std::string, const xAOD::Egamma *); // Online and Offline fillers
  void fillTracking(const std::string, const xAOD::Electron *); // Online and Offline fillers
  void fillEFCalo(const std::string,const xAOD::CaloCluster *);
  void fillL2Electron(const std::string,const xAOD::TrigElectron *);
  void fillL2Calo(const std::string,const xAOD::TrigEMCluster *);
  void fillRingerShapes(const std::string,const xAOD::TrigRingerRings *);
  void fillL1Calo(const std::string,const xAOD::EmTauRoI *);
   /*! Include more detailed histograms */
  bool m_detailedHists;
};

#endif
