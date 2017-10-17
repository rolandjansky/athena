/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_egammaTruthAssociationAlg_H
#define EGAMMAREC_egammaTruthAssociationAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandle.h"

#include <memory>
#include <array>

/**
   @class egammaTruthAssociationAlg
   Decorate egamma object with truth information during reconstruction
   Creates egammaTruth collection with links to reco objects
    
   @author B. Lenzi
*/

class egammaTruthAssociationAlg : public AthAlgorithm {

public:
  
  /** @brief constructor */
  egammaTruthAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief destructor */
  virtual ~egammaTruthAssociationAlg();

  /** @brief initialize method */
  virtual StatusCode initialize();
  /** @brief execute on container */
  virtual StatusCode execute();
  /** @brief finalize method */
  virtual StatusCode finalize();
  
private:
 
  struct MCTruthInfo_t {
    MCTruthPartClassifier::ParticleType first;
    MCTruthPartClassifier::ParticleOrigin second;
    const xAOD::TruthParticle* genPart;
  };
  
  /** @brief helper class to contain write docoration handle keys */
  template<class T> struct writeDecorHandleKeys {
    StatusCode initializeDecorKeys(const std::string &name); // note, not constructor

    std::array<SG::WriteDecorHandleKey<T>, 3> keys;
  };
    
  /** @brief helper class to contain write decoration handles */
  template<class T> struct writeDecorHandles {
    writeDecorHandles(const writeDecorHandleKeys<T>& keys); // constructor

    SG::WriteDecorHandle<T, ElementLink<xAOD::TruthParticleContainer> > el;
    SG::WriteDecorHandle<T, int > type;
    SG::WriteDecorHandle<T, int > origin;

    // any of the handles should function as a read handle, choice is arbitrary
    SG::ReadHandle<T>& readHandle() { return type; };

  };
  
    
  /** @brief Loop over elements in the reco container, decorate them with truth info and
   * decorate the truth particles with links to the reco ones (reco<typeName>Link) **/
  template<class T, class L> StatusCode match(const xAOD::TruthParticleContainer& truthParticles,
					      const egammaTruthAssociationAlg::writeDecorHandleKeys<T>& hkeys,
					      const SG::AuxElement::Accessor<L>& linkAccess,
					      xAOD::TruthParticleContainer& egammaTruthContainer) ;

  /** @brief return the result of MCTruthClassifier::particleTruthClassifier
    * or do a second pass for electrons based on the cluster to find true photons **/
  template<class T> MCTruthInfo_t particleTruthClassifier(const T*) ;
     
  /** @brief Create a copy a truth particle, add it to the new container and decorate it
    *  with a link to the original particle **/
  void getNewTruthParticle(xAOD::TruthParticleContainer& egammaTruthContainer,
			   const xAOD::TruthParticle *truth, 
                           const xAOD::TruthParticleContainer *oldContainer) ;

  /** @brief Return true if the truth particle is a prompt electron or photon **/  
  bool isPromptEgammaParticle(const xAOD::TruthParticle *truth) ;

  
  /** @brief Return the truth particle in the egamma truth container that corresponds
    * to the given truth particle (egammaTruthContainer is non-const reference because 
    * returning non-const pointer) **/ 
  xAOD::TruthParticle* getEgammaTruthParticle(const xAOD::TruthParticle *truth, 
					      xAOD::TruthParticleContainer& egammaTruthContainer) const;

  
  /** @brief Create egamma truth container? **/
  Gaudi::Property<bool> m_doEgammaTruthContainer {this,
      "CreateEgammaTruthContainer", true,
      "Create egammaTruthContainer?"};
  
  /** @brief Match fwd electrons? **/
  Gaudi::Property<bool> m_matchForwardElectrons {this,
      "MatchForwardElectrons", true,
      "Match forward electrons?"};

  /** @brief Match clusters? **/
  Gaudi::Property<bool> m_matchClusters {this, "MatchClusters", false,
      "Match clusters?"};
  
  /** @brief Name of the egamma cluster container **/
  Gaudi::Property<std::string> m_clusterContainerName {this,
      "ClusterContainerName", "", 
      "Name of the egamma cluster container"};
  writeDecorHandleKeys<xAOD::CaloClusterContainer> m_clusterDecKeys;
    
  /** @brief Name of the input electron container **/
  Gaudi::Property<std::string> m_electronContainerName {this,
      "ElectronContainerName", "",
      "Name of the input electron container"};
  writeDecorHandleKeys<xAOD::ElectronContainer> m_electronDecKeys;  

  /** @brief Name of the input electron container **/
  Gaudi::Property<std::string> m_fwdElectronContainerName {this,
      "FwdElectronContainerName", "",
      "Name of the input fwd electron container"};
  writeDecorHandleKeys<xAOD::ElectronContainer> m_fwdElectronDecKeys;  
 
  /** @brief Name of the input photon container **/
  Gaudi::Property<std::string> m_photonContainerName {this,
      "PhotonContainerName", "",
      "Name of the input photon container"};  
  writeDecorHandleKeys<xAOD::PhotonContainer> m_photonDecKeys;  

  /** @brief Name of the truth event container **/
  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventContainerKey {this,
      "TruthEventContainerName", "", 
      "Name of the truth event container"};

  /** @brief Name of the truth particle container **/
  SG::ReadHandleKey<xAOD::TruthParticleContainer>  m_truthParticleContainerKey {this,
      "TruthParticleContainerName", "", 
      "Name of the truth particle container"};

  /** @brief Name of the output egamma truth container **/
  SG::WriteHandleKey<xAOD::TruthParticleContainer> m_egammaTruthParticleContainerKey {this,
      "EgammaTruthContainerName", "",
      "Name of the output egamma truth particle container"};
  
  /** @brief Minimum Pt to enter egamma truth particle container **/
  Gaudi::Property<float> m_minPt {this, "MinPtEgammaTruth", 10, 
      "Minimum Pt to enter egamma truth particle container"};

  /** @brief Minimum Pt for FSR to enter egamma truth particle container **/
  Gaudi::Property<float> m_minPtFSR {this, "MinPtEgammaTruthFSR", 1e3, 
      "Minimum Pt for FSR to enter egamma truth particle container"};

  /** Barcode offset for G4 particles **/
  Gaudi::Property<int> m_barcodeOffset {this, "SimBarcodeOffset", 200e3,
      "Barcode offset for G4 particles"};
  
  /** @brief MCTruthClassifier **/
  ToolHandle<IMCTruthClassifier> m_mcTruthClassifier {this,
      "MCTruthClassifier", "EMMCTruthClassifier", "Handle of MCTruthClassifier"};
  
};

#endif // EGAMMAREC_egammaTruthAssociationAlg_H 
