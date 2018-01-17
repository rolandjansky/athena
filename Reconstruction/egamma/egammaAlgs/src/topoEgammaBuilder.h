/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_TOPOEGAMMABUILDER_H
#define EGAMMAALGS_TOPOEGAMMABUILDER_H
/**
  @class topoEgammaBuilder 

  The main electron and photon reconstruction algorithm, based on topoclusters. It attempts to recover electrons that emit brem photons by creating "superclusters."  Superclusters can also be used in photons to especially help converted photons. This is the default egamma algorithm for central electrons and photons as of release 21.  

*/

// INCLUDE HEADER FILES:
#include <vector>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"
#include "egammaInterfaces/IegammaBaseTool.h" 

class egammaRec;

class topoEgammaBuilder : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  topoEgammaBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();
  
 private:

  /** @brief Vector of tools for dressing electrons and photons **/
  ToolHandleArray<IegammaBaseTool> m_egammaTools {this,
      "egammaTools", {}, "Tools for dressing electrons and photons"};
  
  /** @brief Vector of tools for dressing ONLY electrons **/
  ToolHandleArray<IegammaBaseTool> m_electronTools {this,
      "ElectronTools", {}, "Tools for dressing ONLY electrons"};

  /** @brief Vector of tools for dressing ONLY photons **/
  ToolHandleArray<IegammaBaseTool> m_photonTools {this,
      "PhotonTools", {}, "Tools for dressing ONLY photons"};

  /** @brief Retrieve each tool in the given vector **/
  StatusCode RetrieveTools(ToolHandleArray<IegammaBaseTool>& tools);

  /** Given an egammaRec object, a pointer to the electron container and the author, 
   * create and dress an electron, pushing it back to the container and 
   * calling the relevant tools **/
  
  bool getElectron(const egammaRec* egRec, xAOD::ElectronContainer *electronContainer,
		   const unsigned int author, const uint8_t type);

  /** Given an egammaRec object, a pointer to the photon container and the author, 
   * create and dress a photon, pushing it back to the container and 
   * calling the relevant tools **/
  bool getPhoton(const egammaRec* egRec, xAOD::PhotonContainer *photonContainer,
		 const unsigned int author, uint8_t type);
  

  /** @brief Do the final ambiguity **/  
  StatusCode doAmbiguityLinks(xAOD::ElectronContainer *electronContainer, 
			      xAOD::PhotonContainer *photonContainer);

  /** @brief Call a tool using contExecute and electrons, photon containers if given **/
  StatusCode CallTool(ToolHandle<IegammaBaseTool>& tool, 
                      xAOD::ElectronContainer *electronContainer = 0, 
                      xAOD::PhotonContainer *photonContainer = 0);

  /** @brief retrieve EMAmbiguityTool **/
  StatusCode RetrieveAmbiguityTool();

  /** @brief Name of the electron output collection*/
  SG::WriteHandleKey<xAOD::ElectronContainer> m_electronOutputKey {this,
      "ElectronOutputName", "ElectronContainer", 
      "Name of Electron Container to be created"};

  /** @brief Name of the photon output collection */
  SG::WriteHandleKey<xAOD::PhotonContainer> m_photonOutputKey {this,
      "PhotonOutputName", "PhotonContainer",
      "Name of Photon Container to be created"};

  /** @brief Name of input super cluster electron egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_electronSuperClusterRecContainerKey {this,
      "SuperElectronRecCollectionName", 
      "ElectronSuperRecCollection",
      "Input container for electron  Super Cluster  egammaRec objects"};

  /** @brief Name of input super cluster photon egammaRec container */
  SG::ReadHandleKey<EgammaRecContainer> m_photonSuperClusterRecContainerKey {this,
      "SuperPhotonRecCollectionName",
      "PhotonSuperRecCollection",
      "Input container for electron  Super Cluster  egammaRec objects"};
  //
  // The tools
  //
  /** @brief Tool to resolve electron/photon ambiguity */
  ToolHandle<IEGammaAmbiguityTool> m_ambiguityTool {this, 
      "AmbiguityTool", "EGammaAmbiguityTool", 
      "Tool that does electron/photon ambiguity resolution"};

  //
  // Other properties.
  //
  // others:
  IChronoStatSvc* m_timingProfile;
};

#endif
