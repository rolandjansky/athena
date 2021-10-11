/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_XAODEGAMMABUILDER_H
#define EGAMMAALGS_XAODEGAMMABUILDER_H
/**
  @class xAODEgammaBuilder
  The last step of the electron and photon reconstruction algorithm. The
  algorithm builds physics objects (xAOD::Electron, xAOD::Photon) from the
  egammaRec containers built by the previous algorithms. In the step of the
  chain electron and photon reconstruction are merged again and the ambiguity
  resolution is applied.

  Input collections:
  - InputElectronRecCollectionName: default=ElectronSuperRecCollection.
   A container of egammaRec for electrons
  - InputPhotonRecCollectionName: default=PhotonSuperRecCollection.
   A container of egammaRec for photons

  Output collections:
  - ElectronOutputName: default=ElectronContainer. A container of xAOD::Electron.
  - PhotonOutputName: default=PhotonContainer. A container of xAOD::Photon

  If the algorithm runs on both electrons and photons, which is the default
  (see doPhotons and doElectrons properties), the ambiguity resolution is applied.
  If there are two egammaRec, one coming from the electron input container,
  one coming from the photon input container, with the same eta and phi
  of the hottest cells then the ambiguity tool decides if to save
  only one or both. The author and the ambiguityType are set accordingly to that
  decision. If both particles are saved a link between them is saved (ambiguityLink).
  All the properties of electrons are set:
  - cluster links
  - track links
  - charge
  - TrackCaloMatchValues for all the layers, e.g. xAOD::EgammaParameters::deltaEta0,
   xAOD::EgammaParameters::deltaPhi0, xAOD::EgammaParameters::deltaPhiRescaled0
  - xAOD::EgammaParameters::deltaPhiFromLastMeasurement

  All the properties of photons are set:
  - cluster links
  - vertex links
  - xAOD::EgammaParameters::convMatchDeltaEta1,
   xAOD::EgammaParameters::convMatchDeltaPhi1

  Several tools are then applied to the particle:
  - Shower shapes are computed thanks to the tool configured by
  EMShowerTool property, implementing the interface IEMShowerBuilder,
  by default EMShowerBuilder.
  - Object quality is filled by the tool specified by the property
  ObjectQualityTool (no default). If the property is not set
  object quality is skipped.
  - finalize the cluster with the tool specified by the property
  EMClusterTool (default=EMClusterTool), for example applying energy
  calibration
  - tools specified by the properties egammaTools, ElectronTools,
  PhotonTools are executed (default: none)
*/

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"
#include "egammaInterfaces/IEMClusterTool.h"
#include "egammaInterfaces/IEMShowerBuilder.h"
#include "egammaInterfaces/IegammaBaseTool.h"
#include "egammaInterfaces/IegammaOQFlagsBuilder.h"
#include "egammaRecEvent/egammaRecContainer.h"
#include "egammaUtils/electronPearShapeAlignmentCorrection.h"

#include <memory>

class egammaRec;

class xAODEgammaBuilder : public AthReentrantAlgorithm
{
public:

    xAODEgammaBuilder(const std::string& name, ISvcLocator* pSvcLocator);

    StatusCode initialize() override final;
    StatusCode finalize() override final;
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
    static StatusCode doAmbiguityLinks(const EventContext& ctx,
                                xAOD::ElectronContainer* electronContainer,
                                xAOD::PhotonContainer* photonContainer) ;

    /** @brief Call a tool using contExecute and electrons, photon containers if
     * given **/
    StatusCode CallTool(const EventContext& ctx,
                        const ToolHandle<IegammaBaseTool>& tool,
                        xAOD::ElectronContainer* electronContainer = nullptr,
                        xAOD::PhotonContainer* photonContainer = nullptr) const;

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

    /** @brief Tool to compute shower shapes */
    ToolHandle<IEMShowerBuilder> m_ShowerTool {this,
        "EMShowerTool", "egammaTools/EMShowerBuilder",
        "Tool that does electron/photon shower shape building"};

    /** @brief Tool to resolve electron/photon ambiguity */
    ToolHandle<IEGammaAmbiguityTool> m_ambiguityTool {this,
        "AmbiguityTool", "EGammaAmbiguityTool/egammaambiguitytool",
        "Tool that does electron/photon ambiguity resolution"};

    /** @brief Tool to add electron/photon Object Quality info */
    ToolHandle<IegammaOQFlagsBuilder> m_egammaOQTool {this,
      "ObjectQualityTool", {},
      "Tool that adds electron/photon Object Quality info"};

    // Read/Write handlers

    /** @brief Name of the electron output collection*/
    SG::WriteHandleKey<xAOD::ElectronContainer> m_electronOutputKey {this,
        "ElectronOutputName", "ElectronContainer",
        "Name of Electron Container to be created"};

    /** @brief Name of the photon output collection */
    SG::WriteHandleKey<xAOD::PhotonContainer> m_photonOutputKey {this,
        "PhotonOutputName", "PhotonContainer",
        "Name of Photon Container to be created"};

    /** @brief Name of input super cluster electron egammaRec container */
    SG::ReadHandleKey<EgammaRecContainer> m_electronClusterRecContainerKey {this,
        "InputElectronRecCollectionName",
        "ElectronSuperRecCollection",
        "Input container for electron  Super Cluster  egammaRec objects"};

    /** @brief Name of input super cluster photon egammaRec container */
    SG::ReadHandleKey<EgammaRecContainer> m_photonClusterRecContainerKey {this,
        "InputPhotonRecCollectionName",
        "PhotonSuperRecCollection",
        "Input container for electron  Super Cluster  egammaRec objects"};

    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloDetDescrMgrKey {
        this,
        "CaloDetDescrManager",
        "CaloDetDescrManager",
        "SG Key for CaloDetDescrManager in the Condition Store"
    };

    /** @brief Name of the dummy electron output collection*/
    SG::WriteHandleKey<xAOD::ElectronContainer> m_dummyElectronOutputKey {this,
        "DummyElectronOutputName", "",
        "Name of Dummy Electron Container to be created"};

    //
    // Other properties.
    /** @brief Option to do truth*/
    Gaudi::Property<bool> m_isTruth {this, "isTruth", false, "is truth"};
    Gaudi::Property<bool> m_doPhotons {this, "doPhotons", true, "Run the Photon reconstruction"};
    Gaudi::Property<bool> m_doElectrons {this, "doElectrons", true, "Run the Electron reconstruction"};
    std::unique_ptr<electronPearShapeAlignmentCorrection> m_deltaEta1Pear;
    bool m_doAmbiguity{};
    bool m_doOQ{};
    bool m_doDummyElectrons = false;
};

#endif
