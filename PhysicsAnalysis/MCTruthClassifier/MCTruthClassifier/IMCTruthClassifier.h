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
    MODIFIED :
*/

#include "AsgTools/IAsgTool.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "xAODTruth/TruthParticle.h"

#include <memory>
#include <unordered_map>
#include <vector>

#define MCTRUTHCLASSIFIER_CONST

#ifndef GENERATIONBASE // Can not be used in Generation release
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODEgamma/PhotonFwd.h"
#include "xAODJet/Jet.h"
#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticleFwd.h"
#endif

#ifndef XAOD_ANALYSIS // Can not be used in AnalysisBase
#include <AtlasHepMC/GenParticle.h>
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) // Can only be used in Athena
#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#endif

class IMCTruthClassifier : virtual public asg::IAsgTool
{

  ASG_TOOL_INTERFACE(IMCTruthClassifier)
public:
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) // Can only be used in Athena
  typedef Trk::IParticleCaloExtensionTool::Cache Cache;
#endif
  // Additional information that can be returned by the classifier.
  // Originally, these were all held in member variables in the classifier,
  // but that prevents the classifier methods from being made const.
  // Not all of these are filled by all classifier calls; it would probably
  // be better to split this up into a hierarchy of structures.
  class Info
  {
  public:
    const xAOD::TruthParticle* genPart = nullptr;

    MCTruthPartClassifier::ParticleOutCome particleOutCome = MCTruthPartClassifier::UnknownOutCome;

    const xAOD::TruthParticle* mother = nullptr;
    long motherBarcode = 0;
    int motherPDG = 0;

    int photonMotherBarcode = 0;
    long photonMotherStatus = 0;
    int photonMotherPDG = 0;
    const xAOD::TruthParticle* photonMother = nullptr;

    const xAOD::TruthParticle* bkgElecMother = nullptr;

    std::vector<const xAOD::TruthParticle*> tauFinalStatePart;

#ifndef GENERATIONBASE /*Disable when no recostruction packages are expected*/
    float deltaRMatch = -999;
    float deltaPhi = -999;
    float probTrkToTruth = 0;
    uint8_t numOfSiHits = 0;

    std::vector<const xAOD::TruthParticle*> egPartPtr;
    std::vector<float> egPartdR;
    std::vector<std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>> egPartClas;

    std::vector<const xAOD::TrackParticle*> cnvPhotTrkPtr;
    std::vector<const xAOD::TruthParticle*> cnvPhotTrkToTruthPart;
    std::vector<MCTruthPartClassifier::ParticleType> cnvPhotPartType;
    std::vector<MCTruthPartClassifier::ParticleOrigin> cnvPhotPartOrig;
#endif

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE) /*Can only be used in Athena*/
    Cache* extrapolationCache = nullptr;
#endif
  };

  /** Virtual destructor */
  virtual ~IMCTruthClassifier(){};

  /* All get to see these*/
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    const xAOD::TruthParticle*,
    Info* info = nullptr) const = 0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> checkOrigOfBkgElec(
    const xAOD::TruthParticle*,
    Info* info = nullptr) const = 0;

  virtual const xAOD::TruthParticle* isHadronFromB(const xAOD::TruthParticle*) const = 0;

#ifndef XAOD_ANALYSIS /*This can not run in Analysis Base*/
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    HepMC::ConstGenParticlePtr,
    Info* info = nullptr) const = 0;
#endif
  //
#ifndef GENERATIONBASE /*These can not run in Generation only release*/
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    const xAOD::TrackParticle*,
    Info* info = nullptr) const = 0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    const xAOD::Electron*,
    Info* info = nullptr) const = 0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    const xAOD::Photon*,
    Info* info = nullptr) const = 0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    const xAOD::Muon*,
    Info* info = nullptr) const = 0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin> particleTruthClassifier(
    const xAOD::CaloCluster*,
    Info* info = nullptr) const = 0;
  virtual std::pair<MCTruthPartClassifier::ParticleType, MCTruthPartClassifier::ParticleOrigin>
  particleTruthClassifier(const xAOD::Jet*, bool DR, Info* info = nullptr) const = 0;

  virtual const xAOD::TruthParticle* getGenPart(const xAOD::TrackParticle*, Info* info = nullptr) const = 0;
#endif
};

#endif // MCTRUTHCLASSIFIER_IMCTRUTHCLASSIFIER_H
