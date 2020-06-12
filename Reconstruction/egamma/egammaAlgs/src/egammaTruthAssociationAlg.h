/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMATRUTHASSOCIATIONALG_H
#define EGAMMAALGS_EGAMMATRUTHASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ToolHandle.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "StoreGate/WriteHandleKey.h"

#include <string>

/**
   @class egammaTruthAssociationAlg
   Decorate egamma object with truth information during reconstruction
   Creates egammaTruth collection with links to reco objects

   @author B. Lenzi J. Mitrevski C. Anastopoulos
*/

class egammaTruthAssociationAlg : public AthAlgorithm
{

public:
  typedef Trk::IParticleCaloExtensionTool::Cache Cache;

  /** @brief constructor */
  egammaTruthAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief destructor */
  virtual ~egammaTruthAssociationAlg();

  /** @brief initialize method */
  virtual StatusCode initialize() override final;
  /** @brief finalize method */
  virtual StatusCode finalize() override final;

  /** @brief execute on container */
  virtual StatusCode execute() override final
  {
    return execute_r(Algorithm::getContext());
  }
  // This will become the normal execute when
  // inheriting from AthReentrantAlgorithm
  StatusCode execute_r(const EventContext& ctx) const;

private:
  struct MCTruthInfo_t
  {
    MCTruthPartClassifier::ParticleType first;
    MCTruthPartClassifier::ParticleOrigin second;
    const xAOD::TruthParticle* genPart;
  };

  /// A function that initializes the decor handles, but also checks the naming
  /// convention
  template<class T>
  StatusCode initializeDecorKeys(SG::WriteDecorHandleKeyArray<T>& keys,
                                 const std::string& name);

  /** @brief helper class to contain write decoration handles */
  template<class T>
  struct writeDecorHandles
  {
    writeDecorHandles(
      const SG::WriteDecorHandleKeyArray<T>& keys); // constructor

    SG::WriteDecorHandle<T, ElementLink<xAOD::TruthParticleContainer>> el;
    SG::WriteDecorHandle<T, int> type;
    SG::WriteDecorHandle<T, int> origin;

    // any of the handles should function as a read handle, choice is arbitrary
    SG::ReadHandle<T>& readHandle() { return type; };
  };

  /** @brief Loop over elements in the reco container, decorate them with truth
   * info and decorate the truth particles with links to the reco ones
   * (reco<typeName>Link) **/
  template<class T, class L>
  StatusCode match(const xAOD::TruthParticleContainer& truthParticles,
                   const SG::WriteDecorHandleKeyArray<T>& hkeys,
                   const SG::AuxElement::Accessor<L>& linkAccess,
                   xAOD::TruthParticleContainer* egammaTruthContainer) const;

  /** @brief return the result of MCTruthClassifier::particleTruthClassifier
   * or do a second pass for electrons based on the cluster to find true photons
   * **/
  template<class T>
  MCTruthInfo_t particleTruthClassifier(const T*, Cache*) const;

  /** @brief Create a copy a truth particle, add it to the new container and
   * decorate it with a link to the original particle **/
  void getNewTruthParticle(xAOD::TruthParticleContainer& egammaTruthContainer,
                           const xAOD::TruthParticle* truth,
                           const xAOD::TruthParticleContainer* oldContainer) const;

  /** @brief Return true if the truth particle is a prompt electron or photon
   * **/
  bool isPromptEgammaParticle(const xAOD::TruthParticle* truth) const;

  /** @brief Return the truth particle in the egamma truth container that
   * corresponds to the given truth particle (egammaTruthContainer is non-const
   * reference because returning non-const pointer) **/
  xAOD::TruthParticle* getEgammaTruthParticle(
    const xAOD::TruthParticle* truth,
    xAOD::TruthParticleContainer& egammaTruthContainer) const;

  /** @brief Create egamma truth container? **/
  Gaudi::Property<bool> m_doEgammaTruthContainer{
    this,
    "CreateEgammaTruthContainer",
    true,
    "Create egammaTruthContainer?"
  };

  /** @brief Match electrons? **/
  Gaudi::Property<bool> m_matchElectrons{ this,
                                          "MatchElectrons",
                                          true,
                                          "Match (central) electrons?" };

  /** @brief Match photons? **/
  Gaudi::Property<bool> m_matchPhotons{ this,
                                        "MatchPhotons",
                                        true,
                                        "Match photons?" };

  /** @brief Match fwd electrons? **/
  Gaudi::Property<bool> m_matchForwardElectrons{ this,
                                                 "MatchForwardElectrons",
                                                 true,
                                                 "Match forward electrons?" };

  /** @brief Match clusters? **/
  Gaudi::Property<bool> m_matchClusters{ this,
                                         "MatchClusters",
                                         false,
                                         "Match clusters?" };

  /** @brief The egamma cluster decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::CaloClusterContainer> m_clusterDecKeys{
    this,
    "DoNotSet_ClusterContainerName",
    {},
    "Do not set; configuration via the string property"
  };

  /** @brief The egamma cluster name property used to initialize the
   * WriteDecorHandleKeyArray **/
  Gaudi::Property<std::string> m_clusterDecName{
    this,
    "ClusterContainerName",
    "",
    "Name of the egamma cluster container"
  };

  /** @brief The electron container decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer> m_electronDecKeys{
    this,
    "DoNotSet_ElectronContainerName",
    {},
    "Do not set; configuration via the string property"
  };

  /** @brief The electron container name property used to initialize the
   * WriteDecorHandleKeyArray **/
  Gaudi::Property<std::string> m_electronDecName{
    this,
    "ElectronContainerName",
    "",
    "Name of the input electron container"
  };

  /** @brief The fwd electron container decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer> m_fwdElectronDecKeys{
    this,
    "DoNotSet_FwdElectronContainerName",
    {},
    "Do not set; configuration via the string property"
  };

  /** @brief The fwd electron name property used to initialize the
   * WriteDecorHandleKeyArray **/
  Gaudi::Property<std::string> m_fwdElectronDecName{
    this,
    "FwdElectronContainerName",
    "",
    "Name of the input fwd electron container"
  };

  /** @brief The photon container decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::PhotonContainer> m_photonDecKeys{
    this,
    "DoNotSet_PhotonContainerName",
    {},
    "Do not set; configuration via the string property"
  };

  /** @brief The photon container name property used to initialize the
   * WriteDecorHandleKeyArray **/
  Gaudi::Property<std::string> m_photonDecName{
    this,
    "PhotonContainerName",
    "",
    "Name of the input photon container"
  };

  /** @brief Name of the truth event container **/
  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventContainerKey{
    this,
    "TruthEventContainerName",
    "",
    "Name of the truth event container"
  };

  /** @brief Name of the truth particle container **/
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainerKey{
    this,
    "TruthParticleContainerName",
    "",
    "Name of the truth particle container"
  };

  /** @brief Name of the output egamma truth container **/
  SG::WriteHandleKey<xAOD::TruthParticleContainer>
    m_egammaTruthParticleContainerKey{
      this,
      "EgammaTruthContainerName",
      "",
      "Name of the output egamma truth particle container"
    };

  /** @brief Minimum Pt to enter egamma truth particle container **/
  Gaudi::Property<float> m_minPt{
    this,
    "MinPtEgammaTruth",
    10,
    "Minimum Pt to enter egamma truth particle container"
  };

  /** @brief Minimum Pt for FSR to enter egamma truth particle container **/
  Gaudi::Property<float> m_minPtFSR{
    this,
    "MinPtEgammaTruthFSR",
    1e3,
    "Minimum Pt for FSR to enter egamma truth particle container"
  };

  /** Barcode offset for G4 particles **/
  Gaudi::Property<int> m_barcodeOffset{ this,
                                        "SimBarcodeOffset",
                                        200e3,
                                        "Barcode offset for G4 particles" };

  /** @brief MCTruthClassifier **/
  ToolHandle<IMCTruthClassifier> m_mcTruthClassifier{
    this,
    "MCTruthClassifier",
    "EMMCTruthClassifier",
    "Handle of MCTruthClassifier"
  };
};

#endif // EGAMMAALGS_EGAMMATRUTHASSOCIATIONALG_H
