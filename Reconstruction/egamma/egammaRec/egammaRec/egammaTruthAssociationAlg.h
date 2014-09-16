/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAREC_egammaTruthAssociationAlg_H
#define EGAMMAREC_egammaTruthAssociationAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MCTruthClassifier/MCTruthClassifierDefs.h"

#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/PhotonContainer.h"

#include "xAODTruth/TruthParticleContainer.h"

class IMCTruthClassifier;
class StoreGateSvc;

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
 
  typedef std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> MCTruthInfo_t;
   
  /** @brief Decorate egamma object with truth information **/
  bool decorateTruth(xAOD::Egamma*, MCTruthInfo_t&);

  /** @brief Decorate truth object with link to reco electron **/  
  bool decorateReco(xAOD::Electron*, const xAOD::ElectronContainer*);

  /** @brief Decorate truth object with link to reco photon **/
  bool decorateReco(xAOD::Photon*, const xAOD::PhotonContainer*);
  
  /** @brief Create a copy a truth particle, add it to the new container and decorate it
    *  with a link to the original particle **/
  void getNewTruthParticle(const xAOD::TruthParticle *truth, 
                           const xAOD::TruthParticleContainer *oldContainer,
                           xAOD::TruthParticleContainer *newContainer);

  /** @brief Return true if the truth particle is a prompt electron or photon **/  
  bool isPromptEgammaParticle(const xAOD::TruthParticle *truth);
  
  /** @brief Return the truth particle in the egamma truth container that corresponds
    * to the given truth particle **/ 
  xAOD::TruthParticle* getEgammaTruthParticle(const xAOD::TruthParticle *truth);
  
  /** @brief Create egamma truth container? **/
  bool m_doEgammaTruthContainer;
  
  /** @brief Use forward electrons? **/
  bool m_useForwardElectrons;
    
  /** @brief Name of the input electron container **/
  std::string m_electronContainerName;

  /** @brief Name of the input electron container **/
  std::string m_fwdElectronContainerName;
  
  /** @brief Name of the input photon container **/
  std::string m_photonContainerName;  

  /** @brief Name of the truth container **/
  std::string m_truthParticleContainerName;

  /** @brief Name of the output egamma truth container **/
  std::string m_egammaTruthParticleContainerName;
  
  /** @brief Minimum Pt to enter egamma truth particle container **/
  float m_minPt;
  
  /** @brief MCTruthClassifier **/
  ToolHandle<IMCTruthClassifier>   m_mcTruthClassifier;
  
  StoreGateSvc*   m_storeGate;
  
  xAOD::TruthParticleContainer *m_egammaTruthContainer; 
  
};

#endif // EGAMMAREC_egammaTruthAssociationAlg_H 
