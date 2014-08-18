/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
MODIFIED :
*/

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include <vector>

// EDM includes
#include "xAODTruth/TruthParticle.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODJet/Jet.h"

namespace HepMC {
 class GenParticle;
}

static const InterfaceID IID_IMCTruthClassifier("IMCTruthClassifier", 1, 0);
        
class IMCTruthClassifier : virtual public IAlgTool {
     
 public:
  /** Virtual destructor */
  virtual ~IMCTruthClassifier(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID();
  
  virtual StatusCode initialize() = 0;
  virtual StatusCode execute() = 0;
  virtual StatusCode finalize() = 0;

  //interface to  GenParticle
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const HepMC::GenParticle *)= 0; 
  //
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::TruthParticle *)= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::TrackParticle *)= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Electron* )= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Photon* )= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Muon* )= 0; 
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::CaloCluster* )=0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>  particleTruthClassifier(const xAOD::Jet*, bool DR )= 0;

  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> trackClassifier(const xAOD::TrackParticle *) = 0;   

  virtual float getProbTrktoTruth() = 0;


  virtual MCTruthPartClassifier::ParticleOutCome  getParticleOutCome() = 0;

  //virtual const xAOD::TruthParticle* getTruthParticle(const HepMC::GenParticle*) = 0;

  //virtual const HepMC::GenParticle* getGenPart(const xAOD::TruthParticle*) = 0;
  // virtual const HepMC::GenParticle* getGenPart(const xAOD::TrackParticle *) = 0;
  //virtual const HepMC::GenParticle* getGenPart() = 0;
  virtual const xAOD::TruthParticle* getGenPart(const xAOD::TrackParticle *) = 0;
  virtual const xAOD::TruthParticle* getGenPart() = 0;

  //virtual const HepMC::GenParticle* getMother() = 0;
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
  virtual void  setdeltaRMatchCut(float) = 0;
  virtual float getdeltaPhiMatch() = 0;
  virtual void  setdeltaPhiMatchCut(float )= 0;
  virtual void  setNumOfSiHitsCut(uint8_t ) = 0;
  virtual uint8_t   getNumOfSiHits() = 0;
  virtual void  setPhtdRMatchToTrCut(float) = 0;
  virtual void  setFwrdEledRMatchToTrCut(float ) = 0;

  virtual std::vector<const xAOD::TruthParticle*>  getEGPartPntr() = 0;
  virtual std::vector<float> getEGPartdR() = 0;
  virtual std::vector<std::pair<MCTruthPartClassifier::ParticleType,MCTruthPartClassifier::ParticleOrigin> >  getEGPartClas() = 0;



  virtual std::vector<const xAOD::TrackParticle*>             getCnvPhotTrkPtr() = 0;
  virtual std::vector<const xAOD::TruthParticle*>             getCnvPhotTrkToTruthPart() = 0;
  virtual std::vector<MCTruthPartClassifier::ParticleType>    getCnvPhotPartType() = 0;
  virtual std::vector<MCTruthPartClassifier::ParticleOrigin>  getCnvPhotPartOrig() = 0;

};

inline const InterfaceID& IMCTruthClassifier::interfaceID()
{
  return IID_IMCTruthClassifier;
}

#endif // MCTRUTHCLASSIFIER_IMCTRUTHCLASSIFIER_H 
