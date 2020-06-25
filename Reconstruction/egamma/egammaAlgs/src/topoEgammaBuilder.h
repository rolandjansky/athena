/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_TOPOEGAMMABUILDER_H
#define EGAMMAALGS_TOPOEGAMMABUILDER_H
/**
  @class topoEgammaBuilder 
  The main electron and photon reconstruction algorithm, based on topoclusters. 
  It attempts to recover electrons that emit brem photons by creating "superclusters."  
  Superclusters can also be used in photons to especially help converted photons. 
  This is the default egamma algorithm for central electrons and photons as of release 21.  
*/

// INCLUDE HEADER FILES:
#include <vector>

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "egammaRecEvent/egammaRecContainer.h"

#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"
#include "egammaInterfaces/IEMClusterTool.h" 
#include "egammaInterfaces/IEMShowerBuilder.h" 
#include "egammaInterfaces/IegammaOQFlagsBuilder.h" 
#include "egammaInterfaces/IegammaBaseTool.h" 
class egammaRec;

class topoEgammaBuilder : public AthReentrantAlgorithm
{
public:

    /** @brief Default constructor*/
    topoEgammaBuilder(const std::string& name, ISvcLocator* pSvcLocator);

    /** @brief initialize method*/
    StatusCode initialize() override final;
    /** @brief finalize method*/
    StatusCode finalize() override final;
    /** @brief execute method*/
    StatusCode execute(const EventContext& ctx) const override final;

private:
    /** Given an egammaRec object, a pointer to the electron container and the author, 
     * create and dress an electron, pushing it back to the container and 
     * calling the relevant tools **/

    bool getElectron(const egammaRec* egRec, xAOD::ElectronContainer *electronContainer,
            const unsigned int author, const uint8_t type) const;

    /** Given an egammaRec object, a pointer to the photon container and the author, 
     * create and dress a photon, pushing it back to the container and 
     * calling the relevant tools **/
    bool getPhoton(const egammaRec* egRec, xAOD::PhotonContainer *photonContainer,
            const unsigned int author, uint8_t type) const;


    /** @brief Do the final ambiguity **/  
    StatusCode doAmbiguityLinks(xAOD::ElectronContainer *electronContainer, 
            xAOD::PhotonContainer *photonContainer) const ;

    /** @brief Call a tool using contExecute and electrons, photon containers if given **/
    StatusCode CallTool(const EventContext& ctx,
            const ToolHandle<IegammaBaseTool>& tool, 
            xAOD::ElectronContainer *electronContainer = nullptr, 
            xAOD::PhotonContainer *photonContainer = nullptr) const;


    /** @brief Vector of tools for dressing electrons and photons **/
    ToolHandleArray<IegammaBaseTool> m_egammaTools {this,
        "egammaTools", {}, "Tools for dressing electrons and photons"};

    /** @brief Vector of tools for dressing ONLY electrons **/
    ToolHandleArray<IegammaBaseTool> m_electronTools {this,
        "ElectronTools", {}, "Tools for dressing ONLY electrons"};

    /** @brief Vector of tools for dressing ONLY photons **/
    ToolHandleArray<IegammaBaseTool> m_photonTools {this,
        "PhotonTools", {}, "Tools for dressing ONLY photons"};

    /** @brief Tool to do the final electron/photon cluster building */
    ToolHandle<IEMClusterTool> m_clusterTool {this, 
        "EMClusterTool", "egammaTools/EMClusterTool", 
        "Tool that does electron/photon final cluster building"};

    /** @brief Tool to do the final electron/photon cluster building */
    ToolHandle<IEMShowerBuilder> m_ShowerTool {this, 
        "EMShowerTool", "egammaTools/EMShowerBuilder", 
        "Tool that does electron/photon shower shape building"};

    /** @brief Tool to resolve electron/photon ambiguity */
    ToolHandle<IEGammaAmbiguityTool> m_ambiguityTool {this, 
        "AmbiguityTool", "ElectronPhotonSelectorTools/EGammaAmbiguityTool", 
        "Tool that does electron/photon ambiguity resolution"};

    /** @brief Tool to resolve electron/photon ambiguity */
    ToolHandle<IegammaOQFlagsBuilder> m_egammaOQTool {this, 
      "ObjectQualityTool", {}, 
      "Tool that adds electron/photon Object Quality info"};



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
    // Other properties.
    //
    Gaudi::Property<bool> m_doPhotons {this, "doPhotons", true, "Run the Photon reconstruction"};
    Gaudi::Property<bool> m_doElectrons {this, "doElectrons", true, "Run the Electron reconstruction"};
    bool m_doAmbiguity;
    bool m_doOQ;
};

#endif
