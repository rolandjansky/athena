/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMCTruthClassifierDev.h, (c) ATLAS Detector software 2008
///////////////////////////////////////////////////////////////////

#ifndef MCTRUTHCLASSIFIER_IMCTRUTHCLASSIFIER_H
#define MCTRUTHCLASSIFIER_IMCTRUTHCLASSIFIER_H

/** @class IegammaMCTruthclassifier
  Interface for the PhysicsAnalysis/MCTruthClassifier/MCTruthclassifier
  @author Frederic Derue derue@lpnhe.in2p3.fr
CREATED : 01/09/2008
MODIFIED : 02/07/2020 [Sukanya Sinha (sukanya.sinha@cern.ch)]
*/

#include "AsgTools/IAsgTool.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include <vector>

// EDM Fwd includes

#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODMuon/Muon.h"
#include "xAODJet/Jet.h"

#ifndef XAOD_ANALYSIS
namespace HepMC {
 class GenParticle;
}
#endif

        
class IMCTruthClassifier : virtual public asg::IAsgTool {
     
  ASG_TOOL_INTERFACE(IMCTruthClassifier)
 public:
  /** Virtual destructor */
  virtual ~IMCTruthClassifier(){};  
  virtual StatusCode initialize() = 0;
  virtual StatusCode execute() = 0;
  virtual StatusCode finalize() = 0;

  //interface to  GenParticle, leave it only for Athena
#ifndef XAOD_ANALYSIS
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const HepMC::GenParticle *)= 0; 
#endif
  //
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::TruthParticle *)= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::TrackParticle *)= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Electron* )= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Photon* )= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Muon* )= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::CaloCluster* )=0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Jet*, bool DR )= 0;

  /// \brief main function used in \ref MCTruthClassifier returning the value from defOrigofParticle to \ref TruthClassificationDecorator 
  virtual unsigned int classify(const xAOD::TruthParticle *) = 0; 

  /// \brief function used in \ref MCTruthClassifier classifying truth particles with HepMC status 1 & 2 
  virtual unsigned int defOrigOfParticle(const xAOD::TruthParticle*) = 0;

  virtual float getProbTrktoTruth() = 0;

  virtual MCTruthPartClassifier::ParticleOutCome  getParticleOutCome() = 0;

  virtual const xAOD::TruthParticle* getGenPart(const xAOD::TrackParticle *) = 0;
  virtual const xAOD::TruthParticle* getGenPart() = 0;

  virtual const xAOD::TruthParticle* getMother() = 0;
  virtual int   getMotherPDG() = 0;
  virtual int   getMotherBarcode() = 0;
  virtual const xAOD::TruthParticle* getPhotonMother() = 0;
  virtual int   getPhotonMotherPDG() = 0;
  virtual int   getPhotonMotherBarcode() = 0;
  virtual  std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>
                         checkOrigOfBkgElec(const xAOD::TruthParticle*) = 0;
  virtual  const xAOD::TruthParticle* getBkgElecMother() = 0;

  virtual std::vector<const xAOD::TruthParticle*>* getTauFinalState() = 0;

  virtual float getdeltaRMatch() = 0;
  virtual float getdeltaPhiMatch() = 0;
  virtual uint8_t   getNumOfSiHits() = 0;

  virtual std::vector<const xAOD::TruthParticle*>  getEGPartPntr() = 0;
  virtual std::vector<float> getEGPartdR() = 0;
  virtual std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> >  getEGPartClas() = 0;

  virtual std::vector<const xAOD::TrackParticle*>             getCnvPhotTrkPtr() = 0;
  virtual std::vector<const xAOD::TruthParticle*>             getCnvPhotTrkToTruthPart() = 0;
  virtual std::vector<MCTruthPartClassifier::ParticleType>    getCnvPhotPartType() = 0;
  virtual std::vector<MCTruthPartClassifier::ParticleOrigin>  getCnvPhotPartOrig() = 0;

};

#endif // MCTRUTHCLASSIFIER_IMCTRUTHCLASSIFIER_H 
