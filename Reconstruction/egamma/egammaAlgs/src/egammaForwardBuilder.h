/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/EventContext.h"

#include "EventKernel/IParticle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloClusterCellLinkContainer.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"

#include "egammaInterfaces/IegammaOQFlagsBuilder.h" 
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IEMFourMomBuilder.h"
#include "EgammaAnalysisInterfaces/IAsgForwardElectronIsEMSelector.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <string>

class egammaForwardBuilder : public AthReentrantAlgorithm
{
 public:

  /** @brief constructor*/
  egammaForwardBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief destructor*/
  ~egammaForwardBuilder();

  /** @brief initialize method*/
  virtual StatusCode initialize() override final;
  /** @brief finalize method*/
  virtual StatusCode finalize() override final;
  /** @brief execute method*/
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode ExecObjectQualityTool(const EventContext &ctx, xAOD::Egamma *eg) const; 

  /** @brief Tool to perform object quality*/
  ToolHandle<IegammaOQFlagsBuilder> m_objectQualityTool {this,
      "ObjectQualityTool", "",
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
  Gaudi::Property<double> m_ETcut {this, "EtCut", 5.*Gaudi::Units::GeV, "ET cut"};

  /** @brief eta cut */
  Gaudi::Property<double> m_etacut {this, "EtaCut", 2.5, "eta cut"};

 protected:
  /** Handle to the selectors */
  ToolHandleArray<IAsgForwardElectronIsEMSelector> m_forwardElectronIsEMSelectors {this,
      "forwardelectronIsEMselectors", {}, 
      "The selectors that we need to apply to the FwdElectron object"};

  Gaudi::Property<std::vector<std::string> > m_forwardElectronIsEMSelectorResultNames {this,
      "forwardelectronIsEMselectorResultNames", {},
      "The selector result names"};
  
};
#endif

