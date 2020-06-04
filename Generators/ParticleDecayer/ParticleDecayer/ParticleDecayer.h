/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_PARTICLEDECAYER_H
#define GENERATORMODULES_PARTICLEDECAYER_H

#include "GeneratorModules/GenModule.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "AtlasHepMC/IO_BaseClass.h"
#include "AtlasHepMC/Polarization.h"
#include "AtlasHepMC/Flow.h"
#include "HepPDT/ParticleDataTable.hh"


#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Boost.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "AthenaKernel/errorcheck.h"



class ParticleDecayer: public GenModule {

 public:
  
  ParticleDecayer(const std::string& name, ISvcLocator* pSvcLocator);

  ~ParticleDecayer();
  
  StatusCode genInitialize();
  StatusCode fillEvt(HepMC::GenEvent*);
  
 private:

  //name of the MC truth container  
  std::string m_truthParticleContainerName;
  // LeptonJet type: 1 = one dark photon per LeptonJet, 2 = two dark pjotons per LeptonJet
  int m_LJType;
  //new PDG ID of the scalar 
  int m_scalarPDGID;
  //new mass of the scalar in MeV
  double m_scalarMass;
  //mass of the dark photon (MeV)
  double m_particleMass;
  //PDG ID of the dark photon 
  int m_particleID;
  //lifetime of the dark photon
  double m_particleLifeTime; 
  //new PDG_ID of thedark photon 
  int m_particlePDGID;
  //polarization of the dark photon
  int m_particlePolarization; 
  //opposite polarization switch in case of two dark photons
  bool m_oppositePolarization;
  ////selection of the decay mode of the dark photon 
  //BR of dark photon decay to electrons
  double m_BRElectron;
  //BR of dark photon decay to muons 
  double m_BRMuon;
  //BR of dark photon decay to pions 
  double m_BRPion;

  bool   m_doUniformDecay;
  bool   m_doExponentialDecay;
  bool   m_expDecayDoVariableLifetime;
  double m_expDecayFractionToKeep;
  bool   m_expDecayDoTruncateLongDecays;

  double m_barrelRadius;
  double m_endCapDistance;
  double m_thetaEndCapBarrel;

  HepPDT::ParticleDataTable* m_particleTable;

  double     getParticleMass(int pdgID); //retrieve tha particle mass given the PDG ID 
  void       addParticle   (HepMC::GenVertexPtr, int pdg, HepMC::FourVector, int statusCode); //add particles to the evgen file

  double rnd_ExpLifetime(double ct);
  double rnd_DoubleRange(double a, double b);
  double cosgen(int itype);

  StatusCode DFTwoBodyDecay( HepMC::GenParticlePtr, int );
  StatusCode setDecayPosition ( HepMC::GenParticlePtr, HepMC::GenEvent*, bool doScalarDecay=false );
  StatusCode changeMass( HepMC::GenParticlePtr, double );
  StatusCode getDecayProducts( CLHEP::HepLorentzVector, double, std::vector<CLHEP::HepLorentzVector>&, int decayType = 0);

  //four momentum of the dark photon
  CLHEP::HepLorentzVector m_boostLV;
  //production vertex of the dark photon/scalar
  CLHEP::HepLorentzVector m_posLV;  
  //decay vertex of the first dark photon
  CLHEP::HepLorentzVector m_posLV1;  
  //decay vertex of the second dark photon
  CLHEP::HepLorentzVector m_posLV2;  
  //counter
  int m_eventCounter;  
};

#endif
