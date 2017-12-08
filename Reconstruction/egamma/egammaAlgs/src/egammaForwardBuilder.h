/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMAFORWARDBUILDER_H
#define EGAMMAALGS_EGAMMAFORWARDBUILDER_H

/**
  @class egammaForwardBuilder
          Algorithm which makes a egammaObjectCollection for forward electrons.
          egammaForwardBuilder, is dedicated to the reconstruction and 
          identification of electrons in the forward region of ATLAS 
          (2.5<|eta|<4.9). In contrast to the softe and egamma builders the 
          algorithm can use only the information from the calorimeters, as the 
          tracking system is limited to |eta|<2.5, and the topological 
          clusters (instead of SW clusters). The pre-selection and ID are 
          done in the same algorithm. The variables used to discriminant 
          between electron and hadrons are defined as the topo cluster moments 
          or combination of them. This is done separately in two eta bins: 
          the EMEC IW and the FCal using a cut based technic. 
          The forward electron AUTHOR is 8. 
*/

//
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "EventKernel/IParticle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"

#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "ElectronPhotonSelectorTools/IAsgForwardElectronIsEMSelector.h"

#include <string>

class egammaForwardBuilder : public AthAlgorithm
{
 public:

  /** @brief constructor*/
  egammaForwardBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief destructor*/
  ~egammaForwardBuilder();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();

  /** @brief retrieve object quality tool */
  void RetrieveObjectQualityTool();
  /** @brief retrieve 4-mom builder */
  StatusCode RetrieveEMFourMomBuilder();
  /** @brief execute object quality tool */
  StatusCode ExecObjectQualityTool(xAOD::Egamma *eg); 

 private:

  /** @brief Tool to perform object quality*/
  ToolHandle<IegammaBaseTool> m_objectqualityTool {this,
      "ObjectQualityToolName", "",
      "Name of the object quality tool (empty tool name ignored)"};

  /** @brief Tool to perform the 4-mom computation*/
  ToolHandle<IEMFourMomBuilder> m_fourMomBuilder {this,
      "FourMomBuilderTool", "EMFourMomBuilder",
      "Handle of 4-mom Builder"};

  /** @brief input topo cluster type */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_topoClusterKey {this,
      "TopoClusterName", "",
      "Name of the input cluster collection"};

  /** @brief output electron container */
  SG::WriteHandleKey<xAOD::ElectronContainer>  m_electronOutputKey {this,
      "ElectronOutputName", "",
      "Name of Electron Container to be created"};
  
  /** @brief output cluster container */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outClusterContainerKey {this,
      "ClusterContainerName", ""
      "Name of the output EM cluster container"};

  /** @brief output cluster container cell links: name taken from containter name **/
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_outClusterContainerCellLinkKey;

  /** @brief  ET cut */
  Gaudi::Property<double> m_ETcut {this, "EtCut", 5.*CLHEP::GeV, "ET cut"};

  /** @brief eta cut */
  Gaudi::Property<double> m_etacut {this, "EtaCut", 2.5, "eta cut"};

  // to measure speed of the algorithm
  IChronoStatSvc* m_timingProfile;
  
 protected:
  /** Handle to the selectors */
  ToolHandleArray<IAsgForwardElectronIsEMSelector> m_forwardelectronIsEMselectors {this,
      "forwardelectronIsEMselectors", {}, 
      "The selectors that we need to apply to the FwdElectron object"};

  Gaudi::Property<std::vector<std::string> > m_forwardelectronIsEMselectorResultNames {this,
      "forwardelectronIsEMselectorResultNames", {},
      "The selector result names"};
  
};
#endif

