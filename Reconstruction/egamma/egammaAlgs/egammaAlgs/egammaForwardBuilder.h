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

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/ElectronContainer.h"

#include <string>

class egamma;
class IegammaBaseTool;
class StoreGateSvc;
class IEMFourMomBuilder;
class IAsgSelectionTool;
class IAsgForwardElectronIsEMSelector;
class ILumiBlockMuTool;
class CaloClusterCellLink;


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

  /** @brief Name of tool to perform object quality*/
  std::string  m_ObjectQualityToolName;
  /** @brief Tool to perform object quality*/
  ToolHandle<IegammaBaseTool> m_objectqualityTool;

  /** @brief Tool to perform the 4-mom computation*/
  ToolHandle<IEMFourMomBuilder> m_fourMomBuilder;

  /** @brief input topo cluster type */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_topoClusterKey;

  /** @brief output electron container */
  SG::WriteHandleKey<xAOD::ElectronContainer>  m_electronOutputKey;
  
  /** @brief output cluster container */
  SG::WriteHandleKey<xAOD::CaloClusterContainer> m_outClusterContainerKey;
  /** @brief output cluster container cell links: name taken from containter name **/
  SG::WriteHandleKey<CaloClusterCellLinkContainer> m_outClusterContainerCellLinkKey;

  /** @brief  ET cut */
  double m_ETcut;
  /** @brief eta cut */
  double m_etacut;

  // to measure speed of the algorithm
  IChronoStatSvc* m_timingProfile;
  
 protected:
  /** Handle to the selectors */
  ToolHandleArray<IAsgForwardElectronIsEMSelector> m_forwardelectronIsEMselectors;
  std::vector<std::string> m_forwardelectronIsEMselectorResultNames;
  
  
};
#endif

