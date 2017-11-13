/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_TRIGTOPOEGAMMABUILDER_H
#define EGAMMAREC_TRIGTOPOEGAMMABUILDER_H
/**
  @class TrigTopoEgammaBuilder

  Conversion of offline  Reconstruction/egamma/egammaRec/egammaRec/topoEgammaBuilder to use in trigger
  
          top-Algorithm which creates an egammaObjectCollection.
          It retrieves data objects from TDS, and calls the subalgorithms to
	  make the pieces in egamma classs. The subclass should inherit from
	  TrigTopoEgammaBuilder, and implement the method build(egamma*)

         It is seeded from the electromagnetic (EM) calorimeters and starts from clusters reconstructed in the calorimeters by the SlidingWindowFinder (https://twiki.cern.ch/twiki/bin/view/Atlas/SlidingWindowClustering#Sliding_Window_Clustering). For each cluster, with ET(seed)>3GeV in the cluster container the best inner detector track is searched for within a given E/p range. The complexity of the extrapolation (coordinate systems, etc) is hidden in ExtrapolateToCaloTool. It then builds identification variables
      - The electromagnetic shower shape variables are calculated in the EMShowerBuilder tool with respect to these hot cells;
      - Some variables combining inner detector and electromagnetic calorimeter information, like E/p, are build;
      - In order to minimize amount of fakes, in particular in jets, pre-selection cuts are applied in egammaSelectSETool;
      - Discriminating variables are build and used in the EMPIDBuilder tool based on shower shapes in the electromagnetic calorimeter and information from the inner detector;
      - Possible matching to a conversion object is done through the tool EMConversionBuilder;
      - Bremsstrahlung recovery information is obtained through the tools EMBremsstrahlungBuilder and EMTrkRefitter;
      - the 4-momentum is build in EMFourMomBuilder.
      - apply photon recovery
      - apply photon post-processing to recalculate correctly
        information for these recovered photons
*/

// INCLUDE HEADER FILES:
#include <vector>

// INCLUDE HEADER FILES:
#include "GaudiKernel/Algorithm.h"
#include "TrigInterfaces/FexAlgo.h"

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
//class IEMBremCollectionBuilder;
//class IEMVertexBuilder;

class egammaRec;
class StoreGateSvc;

//Supercluster tools.
class IegammaTopoClusterCopier;
class IelectronSuperClusterBuilder;
class IphotonSuperClusterBuilder;

class TrigTopoEgammaBuilder : public HLT::FexAlgo
{
 public:

  /** @brief Default constructor*/
  TrigTopoEgammaBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Destructor*/
  ~TrigTopoEgammaBuilder();

  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  HLT::ErrorCode hltExecute( const HLT::TriggerElement* inputTE,
                             HLT::TriggerElement* outputTE );

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
  StatusCode RetrieveEGammaTopoClusterCopier();
  /** @brief retrieve EMAmbiguityTool **/
  StatusCode RetrieveAmbiguityTool();
  /** @brief retrieve EMTrackMatchBuilder **/
  StatusCode RetrieveEMTrackMatchBuilder();
  /** @brief retrieve EMConversionBuilder **/
  StatusCode RetrieveEMConversionBuilder();
//  /** @brief retrieve 4-mom builder **/
//  StatusCode RetrieveBremCollectionBuilder();
//  /** @brief retrieve BremVertexBuilder **/
//  StatusCode RetrieveVertexBuilder();
  /** @brief Name of the electron output collection*/
  std::string  m_electronOutputName;
  /** @brief Name of the photon output collection */
  std::string  m_photonOutputName;
  /** @brief Name of the topo cluster input collection */
  std::string  m_inputTopoClusterContainerName;

  /** @brief Name of the TrackParticle input collection */
  std::string  m_inputTrackParticleContainerName;
  /** @brief Name of the Vertex input collection */
  std::string  m_inputVertexContainerName;

  //
  // The tools
  //
  /** @brief ToolHandles for the Topo tools*/
  ToolHandle<IegammaTopoClusterCopier>      m_egammaTopoClusterCopier;
  ToolHandle<IelectronSuperClusterBuilder>  m_electronSuperClusterBuilder;
  ToolHandle<IphotonSuperClusterBuilder>  m_photonSuperClusterBuilder;
  /** @brief Tool to resolve electron/photon ambiguity */
  ToolHandle<IEGammaAmbiguityTool>             m_ambiguityTool;
  /** @brief Tool to perform track matching*/
  ToolHandle<IEMTrackMatchBuilder>             m_trackMatchBuilder;
  /** @brief Tool to retrieve the conversions*/
  ToolHandle<IEMConversionBuilder>             m_conversionBuilder;
//  /** @brief Pointer to the BremCollectionBuilder tool*/
//  ToolHandle<IEMBremCollectionBuilder>         m_BremCollectionBuilderTool;
//  /** @brief Pointer to the VertexBuilder*/
//  ToolHandle<IEMVertexBuilder>                 m_vertexBuilder;
  //
  // All booleans
  //
//  /** @brief Boolean to do Brem collection building */
//  bool         m_doBremCollection;
//  /** @brief Boolean to do Vertex collection building */
//  bool         m_doVertexCollection;
  /** @brief private member flag to do the TrackMatching (and conversion building)*/
  bool         m_doTrackMatching;
  /** @brief private member flag to do the conversion building and matching */
  bool         m_doConversions;
  //
  // Other properties.
  //
  // others:
  IChronoStatSvc* m_timingProfile;
};

#endif
