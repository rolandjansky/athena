//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DIPHOTONANALYSIS_H
#define DIPHOTONANALYSIS_H

#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "PhotonAnalysisUtils/PAUnamespace.h"
#include "PhotonAnalysisUtils/PAUDiPhotonContainer.h"
#include "PhotonAnalysisUtils/IPAUprimaryVertexFinder.h"
#include "PhotonAnalysisUtils/IPAUhggFitter.h"
#include "PhotonAnalysisUtils/PAUphotonCorrectionTool.h"
#include "egammaInterfaces/IegammaBaseTool.h"

#include "D3PDMakerUtils/SGKeyResolver.h"
#include "AthContainers/ConstDataVector.h"


class DiphotonAnalysis : public AthAlgorithm {
  
 public:
  
  DiphotonAnalysis(const std::string& name, ISvcLocator* pSvcLocator);
  ~DiphotonAnalysis() {};
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();


private:
  StatusCode PAUTwoGamma() ;
  void collectTruthPhoton(const PAU::egammaTruth* truePart) ;
  float PAUInvMass(const std::vector<float>& E, const std::vector<float>& eta, const std::vector<float>& phi) const ;
  void fillHggQuantities( const PAUphotonCorrectionTool::corrected_set_t & corr , int accuracy, PAU::DiPhoton* dp ) ;
  
  ToolHandle<IPAUprimaryVertexFinder>  m_PrimaryVertex;
  ToolHandle<IPAUhggFitter>            m_PAUhggFitter ;
  ToolHandle<IPAUphotonCorrectionTool> m_PhotonCorrTool ;
  std::string m_photonContainerName;
  std::string m_clusterContainerName;
  std::string m_VxPrimaryCandidateName;

  float m_dipho_PtCut;

  PAU::DiPhotonContainer* m_DiPhotonContainer;
  ConstDataVector<VxContainer>* m_DiPhotonVertexContainer;

  D3PD::SGKeyResolver* m_resolver;

  ToolHandle<IegammaBaseTool> m_egammaOQFlagsBuilder;
  bool m_isData;

};

#endif // HGG_ANALYSIS_SKELETON_NEW_H

