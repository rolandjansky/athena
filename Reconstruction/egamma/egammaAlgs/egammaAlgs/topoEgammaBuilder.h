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

#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

class IegammaBaseTool;
class IEGammaAmbiguityTool;
class IEMTrackMatchBuilder;
class IEMConversionBuilder;

class egammaRec;
class StoreGateSvc;

//Supercluster tools.
class IelectronSuperClusterBuilder;
class IphotonSuperClusterBuilder;

class topoEgammaBuilder : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  topoEgammaBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Destructor*/
  ~topoEgammaBuilder();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();
  
 private:

  /** @brief Vector of tools for dressing electrons and photons **/
  ToolHandleArray<IegammaBaseTool> m_egammaTools;
  
  /** @brief Vector of tools for dressing ONLY electrons **/
  ToolHandleArray<IegammaBaseTool> m_electronTools;

  /** @brief Vector of tools for dressing ONLY photons **/
  ToolHandleArray<IegammaBaseTool> m_photonTools;

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

  /** @brief  Supercluster-specific stuff **/
  StatusCode RetrieveElectronSuperClusterBuilder();
  StatusCode RetrievePhotonSuperClusterBuilder();
  /** @brief retrieve EMAmbiguityTool **/
  StatusCode RetrieveAmbiguityTool();
  /** @brief retrieve EMTrackMatchBuilder **/
  StatusCode RetrieveEMTrackMatchBuilder();
  /** @brief retrieve EMConversionBuilder **/
  StatusCode RetrieveEMConversionBuilder();
  /** @brief Name of the electron output collection*/
  std::string  m_electronOutputName;
  /** @brief Name of the photon output collection */
  std::string  m_photonOutputName;
  /** @brief Name of the topo cluster input collection */
  std::string  m_inputTopoClusterContainerName;
  /** @brief Name of egammaRec container */
  std::string  m_egammaRecContainerName;
  /** @brief Name of input super cluster electron egammaRec container */
  std::string  m_electronSuperClusterRecContainerName;
  /** @brief Name of input super cluster photon egammaRec container */
  std::string  m_photonSuperClusterRecContainerName;
  //
  // The tools
  //
  /** @brief ToolHandles for the Topo tools*/
  ToolHandle<IelectronSuperClusterBuilder>  m_electronSuperClusterBuilder;
  ToolHandle<IphotonSuperClusterBuilder>  m_photonSuperClusterBuilder;
  /** @brief Tool to resolve electron/photon ambiguity */
  ToolHandle<IEGammaAmbiguityTool>             m_ambiguityTool;
  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder>             m_trackMatchBuilder;
  /** @brief Tool to retrieve the conversions*/
  ToolHandle<IEMConversionBuilder>             m_conversionBuilder;
  //
  // All booleans
  //
  /** @brief private member flag to do the TrackMatching (and conversion building)*/
  bool         m_doTrackMatching;
  /** @brief private member flag to do the conversion building and matching */
  bool         m_doConversions;
  //
  // Other properties.
  //
  // others:
  bool            m_dump ;
  IChronoStatSvc* m_timingProfile;
};

#endif
