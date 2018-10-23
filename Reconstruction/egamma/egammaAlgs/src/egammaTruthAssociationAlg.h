/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAALGS_EGAMMATRUTHASSOCIATIONALG_H
#define EGAMMAALGS_EGAMMATRUTHASSOCIATIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "StoreGate/WriteDecorHandle.h"

#include <string>

/**
   @class egammaTruthAssociationAlg
   Decorate egamma object with truth information during reconstruction
   Creates egammaTruth collection with links to reco objects
    
   @author B. Lenzi
*/

class egammaTruthAssociationAlg : public AthAlgorithm {

public:

  typedef Trk::IParticleCaloExtensionTool::Cache Cache;

  /** @brief constructor */
  egammaTruthAssociationAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief destructor */
  virtual ~egammaTruthAssociationAlg();

  /** @brief initialize method */
  virtual StatusCode initialize() override final;
  /** @brief execute on container */
  virtual StatusCode execute() override final;
  /** @brief finalize method */
  virtual StatusCode finalize() override final;
  
private:
 
  struct MCTruthInfo_t {
    MCTruthPartClassifier::ParticleType first;
    MCTruthPartClassifier::ParticleOrigin second;
    const xAOD::TruthParticle* genPart;
  };
  
  /// A function that initializes the decor handles, but also checks the naming convention
  template<class T> StatusCode initializeDecorKeys(SG::WriteDecorHandleKeyArray<T>& keys);
    
  /** @brief helper class to contain write decoration handles */
  template<class T> struct writeDecorHandles {
    writeDecorHandles(const SG::WriteDecorHandleKeyArray<T>& keys); // constructor

    SG::WriteDecorHandle<T, ElementLink<xAOD::TruthParticleContainer> > el;
    SG::WriteDecorHandle<T, int > type;
    SG::WriteDecorHandle<T, int > origin;

    // any of the handles should function as a read handle, choice is arbitrary
    SG::ReadHandle<T>& readHandle() { return type; };

  };
  
    
  /** @brief Loop over elements in the reco container, decorate them with truth info and
   * decorate the truth particles with links to the reco ones (reco<typeName>Link) **/
  template<class T, class L> StatusCode match(const xAOD::TruthParticleContainer& truthParticles,
					      const SG::WriteDecorHandleKeyArray<T>& hkeys,
					      const SG::AuxElement::Accessor<L>& linkAccess,
					      xAOD::TruthParticleContainer& egammaTruthContainer) ;

  /** @brief return the result of MCTruthClassifier::particleTruthClassifier
    * or do a second pass for electrons based on the cluster to find true photons **/
  template<class T> MCTruthInfo_t particleTruthClassifier(const T*, Cache *) ;
     
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
  
  /** @brief The egamma cluster decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::CaloClusterContainer> m_clusterDecKeys {this,
      "ClusterContainerName", "",
      "Name of the egamma cluster container"};
  
  /** @brief The electron container decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer> m_electronDecKeys {this,
      "ElectronContainerName", "",
      "Name of the input electron container"};  

  /** @brief The fwd electron container decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::ElectronContainer> m_fwdElectronDecKeys {this,
      "FwdElectronContainerName", "",
      "Name of the input fwd electron container"};    
 
  /** @brief The photon container decor handle key array **/
  SG::WriteDecorHandleKeyArray<xAOD::PhotonContainer> m_photonDecKeys {this,
      "PhotonContainerName", "",
      "Name of the input photon container"};     
  
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

//// The templated functions


// ==========================================================================
template<class T> 
StatusCode 
egammaTruthAssociationAlg::initializeDecorKeys(SG::WriteDecorHandleKeyArray<T>& keys)
{
  if (keys.size() == 0) {
    return StatusCode::SUCCESS;
  } else if (keys.size() == 1) {
    // This should be the default, with just one key defined
    // that's just the base name. It should not have any . in it;
    std::string baseName = keys[0].key();
    if (baseName.find('.') != std::string::npos) {
      ATH_MSG_FATAL("Only the input container name is expeted, without extensions. Given: " << baseName);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("The base container name is " << baseName);
    keys[0] = baseName + ".truthParticleLink";
    keys.emplace_back(baseName + ".truthType");
    keys.emplace_back(baseName + ".truthOrigin");
    ATH_CHECK(keys.initialize());
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_FATAL("Must specify only the base container name as a string");
    return StatusCode::FAILURE;
  }
}

// ==========================================================================   
// constructor
template<class T> 
egammaTruthAssociationAlg::writeDecorHandles<T>::writeDecorHandles(const SG::WriteDecorHandleKeyArray<T>& hkeys) :
  el(hkeys.at(0)),
  type(hkeys.at(1)),
  origin(hkeys.at(2))
{
}


// ==========================================================================   
template<class T> egammaTruthAssociationAlg::MCTruthInfo_t 
egammaTruthAssociationAlg::particleTruthClassifier(const T* particle, Cache *extrapolationCache) {
  MCTruthInfo_t info;
  IMCTruthClassifier::Info mcinfo;
  mcinfo.extrapolationCache = extrapolationCache;
  auto ret = m_mcTruthClassifier->particleTruthClassifier(particle, &mcinfo);
  info.genPart = mcinfo.genPart;
  info.first = ret.first;
  info.second = ret.second;
  return info;
}
/** Template specialisation for electrons: 
 * second pass based on the cluster to find true photons **/
template<> egammaTruthAssociationAlg::MCTruthInfo_t 
egammaTruthAssociationAlg::particleTruthClassifier<xAOD::Electron>(const xAOD::Electron* electron, Cache *extrapolationCache) {
  MCTruthInfo_t info;
  IMCTruthClassifier::Info mcinfo;
  mcinfo.extrapolationCache = extrapolationCache;
  auto ret = m_mcTruthClassifier->particleTruthClassifier(electron, &mcinfo);
  if (ret.first == MCTruthPartClassifier::Unknown &&
      !xAOD::EgammaHelpers::isFwdElectron(electron) && electron->caloCluster()){
    ATH_MSG_DEBUG("Trying cluster-based truth classification for electron");
    ret = m_mcTruthClassifier->particleTruthClassifier( electron->caloCluster(), &mcinfo);
  }
  info.genPart = mcinfo.genPart;
  info.first = ret.first;
  info.second = ret.second;
  return info;
}

// ==========================================================================   
template<class T, class L> 
StatusCode egammaTruthAssociationAlg::match(const xAOD::TruthParticleContainer& truthParticles,
					    const SG::WriteDecorHandleKeyArray<T>& hkeys,
					    const SG::AuxElement::Accessor<L>& linkAccess,
					    xAOD::TruthParticleContainer& egammaTruthContainer) {

  writeDecorHandles<T> decoHandles(hkeys);

  //Extrapolation Cache
  Cache extrapolationCache{};

  for (auto particle : *decoHandles.readHandle()){

    MCTruthInfo_t info = particleTruthClassifier(particle, &extrapolationCache);

    const xAOD::TruthParticle* truthParticle = info.genPart;
    if (truthParticle) {
      ElementLink<xAOD::TruthParticleContainer> link(truthParticle, truthParticles);
      ATH_MSG_DEBUG("Decorating object with link to truth, index = " << link.index());
      decoHandles.el(*particle) = link;
    } else {
      decoHandles.el(*particle) = ElementLink<xAOD::TruthParticleContainer>();
    }
    decoHandles.el(*particle).toPersistent();
    ATH_MSG_DEBUG("truthType = " << info.first << " truthOrigin = " << info.second);
    decoHandles.type(*particle) = static_cast<int>( info.first );
    decoHandles.origin(*particle) = static_cast<int>( info.second );

    // Decorate the corresponding truth particle with the link to the reco
    if (m_doEgammaTruthContainer) {
      const xAOD::TruthParticle *truth = xAOD::TruthHelpers::getTruthParticle(*particle);
      if (truth) {
	xAOD::TruthParticle *truthEgamma = getEgammaTruthParticle(truth, egammaTruthContainer);
	if (truthEgamma) {
	  // we found a truthEgamma object we should annotate if this is the best link
	  bool annotateLink = true; // by default we annotate
	  const auto link = linkAccess(*truthEgamma); // what already exists
	  if (link.isValid()) {
	    auto oldPart = *link;
	    if (oldPart && truthEgamma->e() > 0 &&
		std::abs( oldPart->e()/truthEgamma->e()-1 ) < std::abs( particle->e()/truthEgamma->e()-1 ) ){
	      ATH_MSG_DEBUG(truthEgamma << ": " << " already set to a better matched particle: " << particle);
	      annotateLink = false;
	    }
	  }

	  if (annotateLink) {
	    L link(particle, *decoHandles.readHandle());
	    linkAccess(*truthEgamma) = link;
	    linkAccess(*truthEgamma).toPersistent(); 
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}


#endif // EGAMMAALGS_EGAMMATRUTHASSOCIATIONALG_H
