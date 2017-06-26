/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMABUILDER_H
#define EGAMMAALGS_EGAMMABUILDER_H
/**
  @class egammaBuilder
          top-Algorithm which creates an egammaObjectCollection. 
          It retrieves data objects from TDS, and calls the subalgorithms to 
	  make the pieces in egamma classs. The subclass should inherit from 
	  egammaBuilder, and implement the method build(egamma*)

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

class IegammaBaseTool;
class IEGammaAmbiguityTool;
class IEMTrackMatchBuilder;
class IEMConversionBuilder;

class egammaRec;
class StoreGateSvc;

class egammaBuilder : public AthAlgorithm
{
 public:

  /** @brief Default constructor*/
  egammaBuilder(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Destructor*/
  ~egammaBuilder();

  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief finalize method*/
  StatusCode finalize();
  /** @brief execute method*/
  StatusCode execute();
  
  /** Given an egammaRec object, a pointer to the electron container and the author, 
    * create and dress an electron, pushing it back to the container and 
    * calling the relevant tools **/
  bool getElectron(const egammaRec*, xAOD::ElectronContainer*, unsigned int);

  /** Given an egammaRec object, a pointer to the photon container and the author, 
    * create and dress a photon, pushing it back to the container and 
    * calling the relevant tools **/
  bool getPhoton(const egammaRec*, xAOD::PhotonContainer*, unsigned int);
  
  /** Add topo-seeded photons (that do not overlap with other clusters) 
    * to the photon container **/
  StatusCode addTopoSeededPhotons(xAOD::PhotonContainer *photonContainer,
                                  const xAOD::CaloClusterContainer *clusters);

  /** Return true if refCluster overlaps with any cluster in the collection 
    * Used to select topo-seeded clusters **/
  bool clustersOverlap(const xAOD::CaloCluster *refCluster, 
                       const xAOD::CaloClusterContainer *clusters);


 private:


  /** @brief Vector of tools for dressing electrons and photons **/
  ToolHandleArray<IegammaBaseTool> m_egammaTools;
  
  /** @brief Vector of tools for dressing ONLY electrons **/
  ToolHandleArray<IegammaBaseTool> m_electronTools;

  /** @brief Vector of tools for dressing ONLY photons **/
  ToolHandleArray<IegammaBaseTool> m_photonTools;

  /** @brief Retrieve each tool in the given vector **/
  StatusCode RetrieveTools(ToolHandleArray<IegammaBaseTool>& tools);
  
  /** @brief Call a tool using contExecute and electrons, photon containers if given **/
  StatusCode CallTool(ToolHandle<IegammaBaseTool>& tool, 
                      xAOD::ElectronContainer *electronContainer = 0, 
                      xAOD::PhotonContainer *photonContainer = 0);

  /** @brief retrieve EGammaAmbiguityTool **/
  StatusCode RetrieveAmbiguityTool();
  /** @brief retrieve EMTrackMatchBuilder **/
  StatusCode RetrieveEMTrackMatchBuilder();
  /** @brief retrieve EMConversionBuilder **/
  StatusCode RetrieveEMConversionBuilder();
  
  /** @brief Name of the electron output collection*/
  SG::WriteHandleKey<xAOD::ElectronContainer> m_electronOutputKey;
  /** @brief Name of the photon output collection */
  SG::WriteHandleKey<xAOD::PhotonContainer> m_photonOutputKey;

  /** @brief Name of the cluster intput collection */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClusterContainerKey;

  /** @brief Name of the topo-seeded cluster collection */
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_topoSeededClusterContainerKey;

  /** @brief Name of egammaRec container */
  SG::WriteHandleKey<EgammaRecContainer> m_egammaRecContainerKey;

  //
  // The tools
  //
  // subalgorithm pointers cached in initialize:
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
  /** @brief private member flag to do the conversion matching */
  bool         m_doConversions;
  /** @brief add topo-seeded photons */
  bool         m_doTopoSeededPhotons;
  //
  // Other properties.
  //
  /** @brief Discard clusters with energy less than this after corrections. */
  float        m_clusterEnergyCut;
  
  // @brief Minimum deltaEta to check if clusters overlap
  float m_minDeltaEta;
  
  // @brief Minimum deltaPhi to check if clusters overlap
  float m_minDeltaPhi;  
  
  // @brief Minimum transverse energy to accept topo-seeded clusters
  float m_minEtTopo;

  // @brief Maximum transverse energy to accept topo-seeded clusters
  float m_maxEtTopo;
  
  // others:
  bool            m_dump ;
  IChronoStatSvc* m_timingProfile;

};

#endif
