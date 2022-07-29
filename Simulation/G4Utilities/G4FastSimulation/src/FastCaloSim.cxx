/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "FastCaloSim.h"

// FastCaloSim includes
#include "ISF_FastCaloSimEvent/TFCSSimulationState.h"
#include "ISF_FastCaloSimEvent/TFCSTruthState.h"
#include "ISF_FastCaloSimEvent/TFCSExtrapolationState.h"

// Random generator includes
#include "AthenaKernel/RNGWrapper.h"
// Athena check macros
#include "AthenaBaseComps/AthCheckMacros.h"

// Geant4 includes
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"

// HepMCHelpers include
#include "TruthUtils/HepMCHelpers.h"

// G4 sensitive detector includes
#include "G4SDManager.hh"
#include "ISF_FastCaloSimParametrization/CaloCellContainerSD.h"

#undef FCS_DEBUG


FastCaloSim::FastCaloSim(const std::string& name,
ServiceHandle<IAthRNGSvc> rndmGenSvc, Gaudi::Property<std::string> randomEngineName,
PublicToolHandle<IFastCaloSimCaloExtrapolation> FastCaloSimCaloExtrapolation, ServiceHandle<ISF::IFastCaloSimParamSvc> FastCaloSimSvc,
Gaudi::Property<std::string> CaloCellContainerSDName,
FastCaloSimTool * FastCaloSimTool)

: G4VFastSimulationModel(name),
  m_rndmGenSvc(rndmGenSvc), m_randomEngineName(randomEngineName),
  m_FastCaloSimCaloExtrapolation(FastCaloSimCaloExtrapolation), m_FastCaloSimSvc(FastCaloSimSvc),
  m_CaloCellContainerSDName(CaloCellContainerSDName),
  m_FastCaloSimTool(FastCaloSimTool)
{
}

void FastCaloSim::StartOfAthenaEvent(const EventContext& ctx ){
  
  m_rngWrapper = m_rndmGenSvc->getEngine(m_FastCaloSimTool, m_randomEngineName);
  m_rngWrapper->setSeed( m_randomEngineName, ctx );

  return;
}

G4bool FastCaloSim::IsApplicable(const G4ParticleDefinition& particleType)
{   
  // Check whether we can simulate the particle with FastCaloSim
  bool isPhoton   = &particleType == G4Gamma::GammaDefinition();
  bool isElectron = &particleType == G4Electron::ElectronDefinition();
  bool isPositron = &particleType == G4Positron::PositronDefinition();
  bool isHadron   = MC::PID::isHadron(particleType.GetPDGEncoding());

  // FastCaloSim is applicable if it is photon, electron, positron or any hadron
  bool IsApplicable = isPhoton || isElectron || isPositron || isHadron;

  #ifdef FCS_DEBUG
    const std::string pName = particleType.GetParticleName();
    G4cout<< "[FastCaloSim::IsApplicable] Got " << pName <<G4endl;
    if(IsApplicable) G4cout<<"[FastCaloSim::IsApplicable] APPLICABLE"<<G4endl;
    else G4cout<<"[FastCaloSim::IsApplicable] NOT APPLICABLE"<<G4endl;
  #endif


  return IsApplicable;
}

G4bool FastCaloSim::ModelTrigger(const G4FastTrack& fastTrack)
{

  #ifdef FCS_DEBUG
    G4cout<<"[FastCaloSim::ModelTrigger] Got particle with "                                                      <<"\n"
                                    <<" pdg=" <<fastTrack.GetPrimaryTrack() -> GetDefinition()->GetPDGEncoding()  <<"\n"
                                    <<" Ekin="<<fastTrack.GetPrimaryTrack() -> GetKineticEnergy()                 <<"\n"
                                    <<" p="   <<fastTrack.GetPrimaryTrack() -> GetMomentum().mag()                <<"\n"
                                    <<" x="   <<fastTrack.GetPrimaryTrack() -> GetPosition().x()                  <<"\n"
                                    <<" y="   <<fastTrack.GetPrimaryTrack() -> GetPosition().y()                  <<"\n"
                                    <<" z="   <<fastTrack.GetPrimaryTrack() -> GetPosition().z()                  <<"\n"
                                    <<" r="   <<fastTrack.GetPrimaryTrack() -> GetPosition().perp()               <<"\n"
                                    <<" eta=" <<fastTrack.GetPrimaryTrack() -> GetMomentum().eta()                <<"\n"
                                    <<" phi=" <<fastTrack.GetPrimaryTrack() -> GetMomentum().phi()                <<"\n"
                                    <<G4endl;
  #endif


  // Simulate particles below 50 keV with Geant4 to have same config as ISF implementation 
  if (fastTrack.GetPrimaryTrack() -> GetKineticEnergy() < 0.05) {
      #ifdef FCS_DEBUG
        G4cout<<"[FastCaloSim::ModelTrigger] Particle below 50 keV threshold. Passing to G4. "<<G4endl;
      #endif
    return false;
  }

  // Check if triggered particle is really on the ID-Calo (parametrization) boundary
  if (!passedIDCaloBoundary(fastTrack)) {
    #ifdef FCS_DEBUG
      G4cout<<"[FastCaloSim::ModelTrigger] Particle failed passedIDCaloBoundary z="<<fastTrack.GetPrimaryTrack() -> GetPosition().z()<<" r="<<fastTrack.GetPrimaryTrack() -> GetPosition().perp()<<G4endl;
    #endif
    return false;
  }

  // Set minimum kinetic energy of pions and other hadrons required to be passed to FastCaloSim
  float minEkinPions = 200;
  float minEkinOtherHadrons = 400;

  // Get particle definition 
  const G4ParticleDefinition * G4Particle = fastTrack.GetPrimaryTrack() -> GetDefinition();
  // Get particle kinetic energy
  const float Ekin = fastTrack.GetPrimaryTrack() -> GetKineticEnergy();
  
  // Check particle type
  bool isPhoton    = G4Particle == G4Gamma::Definition();
  bool isElectron  = G4Particle == G4Electron::Definition();
  bool isPositron  = G4Particle == G4Positron::Definition();
  bool isPionPlus  = G4Particle == G4PionPlus::Definition();
  bool isPionMinus = G4Particle == G4PionMinus::Definition();

  // Pass all photons, electrons and positrons to FastCaloSim
  if (isPhoton || isElectron || isPositron){
    #ifdef FCS_DEBUG
      G4cout<<"[FastCaloSim::ModelTrigger] Model triggered"<<G4endl;
    #endif
    return true;
  }

  // Require minimum kinetic energy of pions needed to be passed to FastCaloSim
  if (isPionPlus || isPionMinus){
    bool passMinEkinPions = Ekin > minEkinPions;
    
    #ifdef FCS_DEBUG
      if(passMinEkinPions) G4cout<<"[FastCaloSim::ModelTrigger] Model triggered"<<G4endl;
      else G4cout<<"[FastCaloSim::ModelTrigger] Pion with Ekin="<<Ekin<<"below the minimum "<<minEkinPions<<" MeV threshold. Model not triggered."<<G4endl;
    #endif

    return passMinEkinPions;
  
  }

  // Require minimum kinetic energy of other hadrons needed to be passed to FastCaloSim
  bool passMinEkinOtherHadrons = Ekin > minEkinOtherHadrons;

  #ifdef FCS_DEBUG
    if(passMinEkinOtherHadrons) G4cout<<"[FastCaloSim::ModelTrigger] Model triggered"<<G4endl;
    else G4cout<<"[FastCaloSim::ModelTrigger] Other hadron with Ekin="<<Ekin<<"below the minimum "<<minEkinOtherHadrons<<" MeV threshold. Model not triggered."<<G4endl;
  #endif

  return passMinEkinOtherHadrons;
}

void FastCaloSim::DoIt(const G4FastTrack& fastTrack, G4FastStep& fastStep)
{

  TFCSSimulationState simState(*m_rngWrapper);
  TFCSTruthState truthState;
  TFCSExtrapolationState extrapolState;
  
  // Get Geant4 primary track
  const G4Track * G4PrimaryTrack = fastTrack.GetPrimaryTrack(); 
  // Get Geant4 particle definition
  const G4ParticleDefinition * G4Particle = G4PrimaryTrack -> GetDefinition();
  // Get Geant4 particle pdgID
  signed int pdgID = G4Particle -> GetPDGEncoding();

  // Do not simulate particles below 10 MeV
  if(G4PrimaryTrack -> GetKineticEnergy() < 10){
    #ifdef FCS_DEBUG
      G4cout<<"[FastCaloSim::DoIt] Skipping particle with Ekin: " << G4PrimaryTrack -> GetKineticEnergy() <<" MeV. Below the 10 MeV threshold"<<G4endl;
    #endif 
    fastStep.KillPrimaryTrack();
    return;
  }
  // Decide on which FastCaloSim parametrization to use (electron, photon or pion)
  if(G4Particle == G4Electron::Definition() ||  G4Particle == G4Positron::Definition() || G4Particle == G4Gamma::Definition())
  { 
    // Use egamma parametrization for simulation of electrons and photons
    truthState.set_pdgid(pdgID);
  }
  else{
    // Use pion parametrization for simulation of all hadrons
    truthState.set_pdgid(G4PionPlus::Definition() -> GetPDGEncoding());
  }

  // Set the kinematics of the FastCaloSim truth state
  truthState.SetPtEtaPhiM(G4PrimaryTrack -> GetMomentum().perp(), 
                          G4PrimaryTrack -> GetMomentum().eta(), 
                          G4PrimaryTrack -> GetMomentum().phi(), 
                          G4Particle -> GetPDGMass());
  
  // Set the vertex of the FastCaloSim truth state
  truthState.set_vertex(G4PrimaryTrack -> GetPosition().x(), 
                        G4PrimaryTrack -> GetPosition().y(), 
                        G4PrimaryTrack -> GetPosition().z());


  /* For anti protons and anti-neutrons the kinetic energy should be
  calculated as Ekin = E() + M() instead of E() - M() this is 
  achieved by setting an Ekin offset of 2*M() to the truth state */
  if(pdgID == -2212 || pdgID == -2112) truthState.set_Ekin_off(2 * G4Particle -> GetPDGMass());
  
  // Perform the FastCaloSim extrapolation to the calo ID boundary and all other calo layers
  m_FastCaloSimCaloExtrapolation->extrapolate(extrapolState, &truthState);

  // Do not simulate further if extrapolation to ID - Calo boundary fails
  if(extrapolState.CaloSurface_eta() == -999){
    #ifdef FCS_DEBUG
      G4cout<<"[FastCaloSim::DoIt] Killing particle as extrapolation failed"<<G4endl;
    #endif
    fastStep.KillPrimaryTrack();
    return;
  }
  // Perform the actual FastCaloSim simulation
  if(m_FastCaloSimSvc->simulate(simState, &truthState, &extrapolState).isFailure()){
    G4Exception("FastCaloSimSvc", "FailedSimulationCall", FatalException, "FastCaloSimSvc: Simulation call failed.");
    abort(); 
  }

  #ifdef FCS_DEBUG
    G4cout<<"[FastCaloSim::DoIt] Energy returned: " << simState.E() << G4endl;
    G4cout<<"[FastCaloSim::DoIt] Energy fraction for layer: " << G4endl;
    for (int s = 0; s < 24; s++) G4cout<<"[FastCaloSim::DoIt]   Sampling " << s << " energy " << simState.E(s) << G4endl;
  #endif

  // Retrieve the associated CaloCellContainer sensitive detector
  CaloCellContainerSD * caloCellContainerSD = getCaloCellContainerSD();
  // Record the cells 
  caloCellContainerSD->recordCells(simState);

  // Clean up the auxiliar info from the simulation state
  simState.DoAuxInfoCleanup();

  // kill the primary track
  fastStep.KillPrimaryTrack();
  fastStep.SetPrimaryTrackPathLength(0.0);
}


CaloCellContainerSD * FastCaloSim::getCaloCellContainerSD(){
  
  G4SDManager *sdm = G4SDManager::GetSDMpointer();
  G4VSensitiveDetector * vsd = sdm->FindSensitiveDetector((G4String)m_CaloCellContainerSDName);

  if (!vsd){
    G4Exception("FastCaloSimSvc", "FailedFindSensitiveDetector", FatalException, "FastCaloSimSvc: Failed getting CaloCellContainer SD.");
    abort();
  }
  // Cast G4VSensitiveDetector to CaloCellContainerSD
  CaloCellContainerSD * caloCellContainerSD = dynamic_cast<CaloCellContainerSD*>(vsd);
  
  if (!caloCellContainerSD){
    G4Exception("FastCaloSimSvc", "FailedCastSensitiveDetector", FatalException, "FastCaloSimSvc: Failed casting G4VSensitiveDetector.");
    abort();
  }
  return caloCellContainerSD;
}


G4bool FastCaloSim::passedIDCaloBoundary(const G4FastTrack& fastTrack){


  /* Method checks if particle has crossed the ID-Calo boundary, defined using three cylinders with pairs of r/z values 
  We also perform a directional check to make sure that the particle does not originate from any backscatter from the MS or CALO */

  // NOTE:
  // For the inner beam pipe section, innerBeamPipeZ = 4185 corresponds to original AFII boundary, while 
  // innerBeamPipeZ = 4587 corresponds to the CALO::CALO boundary which should be used instead as there 
  // is no triggering volume at this point and we will trigger slightly later than the AFII boundary, so 
  // passedIDCaloBoundary would fail and we would simulate this part with Geant4

  // Barrel
  const float barrelR = 1148;
  const float barrelZ = 3549.5;
  // Inner beam pipe section
  const float innerBeamPipeR = 120;
  const float innerBeamPipeZ = 4587;
  // Outer beam pipe section
  const float outerBeamPipeR = 41;
  const float outerBeamPipeZ = 6783;

  // Get particle position
  const G4ThreeVector particlePosition = fastTrack.GetPrimaryTrack() -> GetPosition();
  // Get r and z values of position
  const float r = particlePosition.perp();
  const float z = particlePosition.z();
  // Get direction of particle 
  const G4ThreeVector particleDirection = fastTrack.GetPrimaryTrack() -> GetMomentum();
  // Construct the helper line to decide if particle comes from ID or is backscatter from CALO
  const G4ThreeVector helperLine = particlePosition + particleDirection;

  // Set 5cm trigger tolerance, i.e. the 'width' of the trigger boundary
  // be careful not to set this too low, else Geant4 might overjump your trigger boundary and simulate everything with G4
  const float triggerTolerance = 50;

  // Barrel boundary (horizontal surfaces)
  if (std::abs(z) <= barrelZ + triggerTolerance) {
    if (r >= barrelR && r < barrelR + triggerTolerance) return helperLine.perp() >= barrelR;
  }
  // Beam pipe boundary (horizontal surfaces)
  if (std::abs(z) >= barrelZ && std::abs(z) <= innerBeamPipeZ + triggerTolerance){
    if (r >= innerBeamPipeR && r < innerBeamPipeR + triggerTolerance) return helperLine.perp() >= innerBeamPipeR;
  }
  if (std::abs(z) >= innerBeamPipeZ && std::abs(z) <= outerBeamPipeZ){
    if (r >= outerBeamPipeR && r < outerBeamPipeR + triggerTolerance) return helperLine.perp() >= outerBeamPipeR;
  }
  // Barrel boundary (vertical surfaces)
  if (r >= outerBeamPipeR && r<= innerBeamPipeR){
    if (std::abs(z) >= innerBeamPipeZ && std::abs(z) < innerBeamPipeZ + triggerTolerance) return std::abs(helperLine.z()) >= innerBeamPipeZ;
  }
  // Beam pipe boundary (vertical surfaces)
  if (r >= innerBeamPipeR && r <= barrelR){
    if (std::abs(z) >= barrelZ && std::abs(z) < barrelZ + triggerTolerance) return std::abs(helperLine.z()) >= barrelZ;
  }
  
  return false;

}

