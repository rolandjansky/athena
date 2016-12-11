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
#include <memory>
class IMCTruthClassifier;

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
  
  /** @brief Loop over elements in the reco container, decorate them with truth info and
    * decorate the truth particles with links to the reco ones (reco<typeName>Link) **/
  template<class T> StatusCode match(std::string containerName, 
				     const std::string& typeName) ;


  /** @brief return the result of MCTruthClassifier::particleTruthClassifier
    * or do a second pass for electrons based on the cluster to find true photons **/
  template<class T> MCTruthInfo_t particleTruthClassifier(const T*) ;
   
  /** @brief Decorate IParticle (cluster or egamma) object with truth information **/
  StatusCode decorateWithTruthInfo(xAOD::IParticle*, const MCTruthInfo_t&);
  
  /** @brief Decorate truth object with link to reco as recoNameLink **/
  template<class T> bool decorateWithRecoLink(T* part, const DataVector<T>* container, 
					      const std::string& name) ;
  
  /** @brief Create a copy a truth particle, add it to the new container and decorate it
    *  with a link to the original particle **/
  void getNewTruthParticle(const xAOD::TruthParticle *truth, 
                           const xAOD::TruthParticleContainer *oldContainer) ;

  /** @brief Return true if the truth particle is a prompt electron or photon **/  
  bool isPromptEgammaParticle(const xAOD::TruthParticle *truth) ;

  
  /** @brief Return the truth particle in the egamma truth container that corresponds
    * to the given truth particle **/ 
  xAOD::TruthParticle* getEgammaTruthParticle(const xAOD::TruthParticle *truth) const;
  
  /** @brief Create egamma truth container? **/
  bool m_doEgammaTruthContainer;
  
  /** @brief Match fwd electrons? **/
  bool m_matchForwardElectrons;

  /** @brief Match clusters? **/
  bool m_matchClusters;
  
  /** @brief Name of the egamma cluster container **/
  std::string m_clusterContainerName;
    
  /** @brief Name of the input electron container **/
  std::string m_electronContainerName;

  /** @brief Name of the input electron container **/
  std::string m_fwdElectronContainerName;
  
  /** @brief Name of the input photon container **/
  std::string m_photonContainerName;  

  /** @brief Name of the truth event container **/
  std::string m_truthEventContainerName;

  /** @brief Name of the truth particle container **/
  std::string m_truthParticleContainerName;

  /** @brief Name of the output egamma truth container **/
  std::string m_egammaTruthParticleContainerName;
  
  /** @brief Minimum Pt to enter egamma truth particle container **/
  float m_minPt;

  /** @brief Minimum Pt for FSR to enter egamma truth particle container **/
  float m_minPtFSR;

  /** Barcode offset for G4 particles **/
  int m_barcodeOffset;
  
  /** @brief MCTruthClassifier **/
  ToolHandle<IMCTruthClassifier>   m_mcTruthClassifier;
  
  std::unique_ptr<xAOD::TruthParticleContainer> m_egammaTruthContainer; 
  
};

#endif // EGAMMAREC_egammaTruthAssociationAlg_H 
