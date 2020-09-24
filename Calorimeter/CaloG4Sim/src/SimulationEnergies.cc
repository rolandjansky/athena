/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SimulationEnergies.cc

// This class implements the calculations requires to categorize the
// energies deposited during the simulation. This "Monte Carlo truth"
// information is used in calibration studies and other applications.

// 12-Aug-2009 M. Leltchouk: this code update improves performance.
// Many thanks to Zachary Marshall for introducing and coding or
// suggesting most of these changes and to Gennady Pospelov and
// Vakhtang Tsulaia for supporting this code and the whole
// "machinary of calibration hits" which he greatly improved and extended.

// 07-Jan-2004 Mikhail Leltchouk with code additions and great support
//        from William Glenn Seligman (WGS).
//  Columbia University in the City of New York,
//  Nevis Labs, 136 South Broadway, Irvington, NY 10533, USA

//                ----------------------------

// 15-Jul-2004 WGS: Now use the VEscapedEnergyProcessing interface and
// EscapedEnergyRegistry from package Calorimeter/CaloG4Sim.

// 28-Nov-2005 M. Leltchouk: protection against segfaults caused by
// geometry problems when touchableHandle->GetVolume()==0

// 20-Apr-2006 M. Leltchouk: internal particle mass table is used now
// in SimulationEnergies::measurableEnergy because in recently released
// G4 8.0 particle masses may not be available when SimulationEnergies
// constructor is looking for them, see Andrea Dell'Acqua's comment below.

// 24-Apr-2007 M. Leltchouk: Particle Data Group (PDG) encoding for nuclei
// has been introduced in SimulationEnergies.cc and
// tagged CaloG4Sim-00-02-26 to be consistent with update in Geant4 8.2.

//    Particle Data Group (PDG) encoding for nuclei ( described in
//    http://pdg.lbl.gov/2006/mcdata/mc_particle_id_contents.html )
//    was introduced in Geant4 8.2 and in CaloG4Sim-00-02-26 which
//    should be used for ATLAS simulations starting from release 13.0.0.

//    CaloG4Sim-00-02-25 should be used for earlier releases where
//    PDGEncoding == 0 for nuclei.

// 18-Feb-2008 M. Leltchouk:
// Since G4AtlasApps-00-02-45 the neutrinos are killed in the 'ATLAS'
// geometries (not in Combined Test Beam geometries) by
// Simulation/G4Atlas/G4AtlasAlg/AthenaStackingAction::ClassifyNewTrack()

// To handle both cases: with or without neutrino cut, i.e. with
// from G4Svc.G4SvcConf import G4Svc
// G4Svc.KillAllNeutrinos=True  (or G4Svc.KillAllNeutrinos=False)
// the SimulationEnergies.cc code has been changed:

// 1) Now neutrino energies are accumulated in result.energy[kEscaped]
// as soon as neutrinos appear in the list of secondaries.
// 2) These escaped energies are recorded to the cell where the escaping
// track originates (i.e. where neutrinos have been born as a result of
// some particle decay) without call of
//  ProcessEscapedEnergy( thisTrackVertex, escapedEnergy ).
// 3) If a neutrino is tracked (was not killed) then the special early
// return from SimulationEnergies::Classify is used to avoid the second
// counting of the same neutrino energy when this neutrino escapes from
// World volume.

#undef DEBUG_ENERGIES

#include "CaloG4Sim/SimulationEnergies.h"
#include "CaloG4Sim/VEscapedEnergyProcessing.h"
#include "CaloG4Sim/EscapedEnergyRegistry.h"

#include "MCTruth/AtlasG4EventUserInfo.h"

#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4TrackVector.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"

// Particle definitions
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Neutron.hh"
#include "G4Proton.hh"
#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"
#include "G4Lambda.hh"
#include "G4SigmaPlus.hh"
#include "G4XiMinus.hh"
#include "G4SigmaZero.hh"
#include "G4XiZero.hh"
#include "G4OmegaMinus.hh"
#include "G4SigmaMinus.hh"
#include "G4AntiLambda.hh"
#include "G4AntiSigmaPlus.hh"
#include "G4AntiSigmaZero.hh"
#include "G4AntiXiZero.hh"
#include "G4AntiXiMinus.hh"
#include "G4AntiOmegaMinus.hh"
#include "G4AntiSigmaMinus.hh"

// For special escaped energy processing.
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4TouchableHandle.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

#include "CxxUtils/AthUnlikelyMacros.h"

#include <vector>
#include <string>
#include <mutex>

static std::once_flag warning1OnceFlag;
static std::once_flag warning2OnceFlag;
static std::once_flag warning3OnceFlag;
static std::once_flag warning4OnceFlag;

namespace CaloG4
{

  SimulationEnergies::SimulationEnergies()
  {}

  SimulationEnergies::~SimulationEnergies()
  {}


  // The "simple" call, intended for calibration calculators:
  void SimulationEnergies::Energies( const G4Step* a_step,
                                     std::vector<G4double>& energies ) const
  {
    // Call the detailed classification.  Process any escaped energy.
    ClassifyResult_t category = Classify( a_step, true );

    // Extract the values we need from the result.  Note that it's at
    // this point we decide which of the available-energy calculations
    // is to be used in a calibration hit.
    if(!energies.empty()) energies.clear();

    energies.push_back( category.energy[SimulationEnergies::kEm] );
    energies.push_back( category.energy[SimulationEnergies::kNonEm] );
    energies.push_back( category.energy[SimulationEnergies::kInvisible0] );
    energies.push_back( category.energy[SimulationEnergies::kEscaped] );

    // Update the global step information to say that we've dealt with this
    AtlasG4EventUserInfo* atlasG4EvtUserInfo = dynamic_cast<AtlasG4EventUserInfo*>(
        G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
    if ( atlasG4EvtUserInfo ) {
      // Update the step info
      atlasG4EvtUserInfo->SetLastProcessedBarcode( a_step->GetTrack()->GetTrackID() );
      atlasG4EvtUserInfo->SetLastProcessedStep( a_step->GetTrack()->GetCurrentStepNumber() );
    }
  }


  // Detailed calculation.

  // -- OUTPUT (energy in MeV, time in nanosecond):
  //  1)  result.energy[kEm]    - visible energy for electromagnetic scale
  //  2)  result.energy[kNonEm] - visible energy for hadronic scale
  //  3)  result.energy[kInvisible0] - invisible energy (Version 0) | one of these
  //  3a) result.energy[kInvisible1] - invisible energy (Version 1) | 3 versions should
  //  3b) result.energy[kInvisible2] - invisible energy (Version 2) | be choosen for a hit
  //  4)  result.energy[kEscaped] - escaped energy
  //  5)  result.time - "midstep" time delay compare to the light
  //                     travel from the point (0,0,0) in World
  //                     coordinates to the "midstep" point.
  //
  // For some studies, it's useful to have this method not process the
  // escaped energy.  If a_processEscaped==false, then the escaped
  // energy will be not be routed to some other volume's hits.

  SimulationEnergies::ClassifyResult_t
  SimulationEnergies::Classify(const G4Step* step, const G4bool a_processEscaped) const
  {
    // Initialize our result to zero.

    ClassifyResult_t result;
    for (unsigned int i = 0; i != kNumberOfEnergyCategories; i++ ) {
      result.energy[ (eEnergyCategory) i ] = 0;
    }

    G4Track* pTrack = step->GetTrack();
    G4ParticleDefinition* particle = pTrack->GetDefinition();

    // If it is a step of a neutrino tracking:
    if (ParticleIsNeutrino(particle)) {
      return result;
    }

    G4TrackStatus status = pTrack->GetTrackStatus();
    G4double dEStepVisible = step->GetTotalEnergyDeposit();
    G4int processSubTypeValue=0;
    if ( step->GetPostStepPoint()->GetProcessDefinedStep() != nullptr ) {
      //from G4VProcess.hh
      processSubTypeValue =
        step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType();
    }

    G4double EkinPreStep = step->GetPreStepPoint()->GetKineticEnergy();
    const G4DynamicParticle* dynParticle = pTrack->GetDynamicParticle();
    G4double EkinPostStep = pTrack->GetKineticEnergy();

    // Start of calculation of invisible energy for current step.
    if ( status == fStopAndKill ){ // StopAndKill stepping particle at PostStep
      G4double incomingEkin = EkinPreStep - dEStepVisible;
      G4double incomingEtot = dynParticle->GetMass() + incomingEkin;

      result.energy[kInvisible0]  =
        measurableEnergy(particle,
                         particle->GetPDGEncoding(),
                         incomingEtot,
                         incomingEkin);
    }
    else if (status == fAlive || status == fStopButAlive) {
      // Alive stepping particle at PostStep
      result.energy[kInvisible0] = EkinPreStep - EkinPostStep - dEStepVisible;
    }

    G4SteppingManager* steppingManager =
      G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager();

    // Copy internal variables from the G4SteppingManager.
    G4TrackVector* fSecondary = steppingManager->GetfSecondary();
    G4int fN2ndariesAtRestDoIt = steppingManager->GetfN2ndariesAtRestDoIt();
    G4int fN2ndariesAlongStepDoIt = steppingManager->GetfN2ndariesAlongStepDoIt();
    G4int fN2ndariesPostStepDoIt = steppingManager->GetfN2ndariesPostStepDoIt();

    G4int tN2ndariesTot = fN2ndariesAtRestDoIt +
                          fN2ndariesAlongStepDoIt +
                          fN2ndariesPostStepDoIt;

    // loop through secondary particles which were added at current step
    // to the list of all secondaries of current track:
    G4int loopStart = (*fSecondary).size() - tN2ndariesTot;
    size_t loopEnd  = (*fSecondary).size();
    if (loopStart < 0) {
      loopEnd = loopEnd - loopStart;
      loopStart = 0;
    }

    G4ParticleDefinition *secondaryID;
    G4double totalEofSecondary=0, kinEofSecondary=0, measurEofSecondary=0;

    for(size_t lp1=loopStart; lp1<loopEnd; lp1++) {

      //----- extract information about each new secondary particle:
      secondaryID = (*fSecondary)[lp1]->GetDefinition();
      totalEofSecondary = (*fSecondary)[lp1]->GetTotalEnergy();

      //----- use this information:
      if (ParticleIsNeutrino(secondaryID)) {
        result.energy[kInvisible0] -= totalEofSecondary;
        result.energy[kEscaped] += totalEofSecondary;
      }
      else {
        //----- extract further information about each new secondary particle:
        kinEofSecondary = (*fSecondary)[lp1]->GetKineticEnergy();
        measurEofSecondary = measurableEnergy(secondaryID,
                                              secondaryID->GetPDGEncoding(),
                                              totalEofSecondary,
                                              kinEofSecondary);
        result.energy[kInvisible0] -= measurEofSecondary;
      }
    }

#ifdef DEBUG_ENERGIES
    // For debugging:
    G4bool allOK = true;
#endif

    if ( pTrack->GetNextVolume() == nullptr )
    {
      // this particle escaped World volume at this step
      // checking for abnormal case
      if ( status != fStopAndKill ) {
        G4cerr << "SimulationEnergies::Classify particle "
               << particle->GetParticleName()
               << " escaped World volume with status=" << status << G4endl;
      }

      G4double escapedEnergy =
        measurableEnergy(particle,
                         particle->GetPDGEncoding(),
                         dynParticle->GetTotalEnergy(),
                         EkinPostStep);

      result.energy[kInvisible0] -= escapedEnergy;

      if ( a_processEscaped ) {
          auto fakeStep = CreateFakeStep(pTrack, escapedEnergy);
#ifdef DEBUG_ENERGIES
        allOK =
#endif
          ProcessEscapedEnergy( fakeStep.get() );
      }
      else {
        result.energy[kEscaped] += escapedEnergy;
      }
    }

    // END of calculation of Invisible and Escaped energy for current step

    // Subdivision of visible energy for current step
    if (particle == G4Electron::Definition() ||
        particle == G4Positron::Definition() ||
        processSubTypeValue == 12)
    {
      result.energy[kEm] = dEStepVisible;
    }
    else {
      result.energy[kNonEm] = dEStepVisible;
    }

#ifdef DEBUG_ENERGIES
    if ( ! allOK ) {
      std::cout << "SimulationEnergies::Classify - additional info: "
                << std::endl
                << "particle name=" << particle->GetParticleName()
                << "   volume="
                << step->GetPreStepPoint()->GetPhysicalVolume()->GetName()
                << " status=" << status
                // 12-Aug-2009  << " process=" << processDefinedStepName
                << " process SubType=" << processSubTypeValue
                << std::endl;
    }
#endif
    return result;
  }

  G4double SimulationEnergies::
  measurableEnergyV2(const G4ParticleDefinition* particleDef,
                     G4int PDGEncoding,
                     G4double totalEnergy,
                     G4double kineticEnergy) const
  {
    // 15-Dec-2003 Mikhail Leltchouk: inspired by FORTRAN Function PrMeasE
    // used by Michael Kuhlen and Peter Loch with Geant3 since 1991.
    G4double measurableEnergy = totalEnergy;

    if (particleDef == G4Electron::Definition() ||
        particleDef == G4Proton::Definition() ||
        particleDef == G4Neutron::Definition() ||
        PDGEncoding == 1000010020 || PDGEncoding == 1000010030 ||
        PDGEncoding == 1000020040 || PDGEncoding == 1000020030) {
      measurableEnergy = kineticEnergy;
    }
    else if (particleDef == G4Positron::Definition()) {
      measurableEnergy = 2.* totalEnergy - kineticEnergy;
    }
    else if (PDGEncoding > 1000010019) { //for nuclei
      measurableEnergy = kineticEnergy;
    }
    else if (particleDef == G4Lambda::Definition() ||
             particleDef == G4SigmaPlus::Definition() ||
             particleDef == G4SigmaZero::Definition() ||
             particleDef == G4SigmaMinus::Definition() ||
             particleDef == G4XiMinus::Definition() ||
             particleDef == G4XiZero::Definition() ||
             particleDef == G4OmegaMinus::Definition()) {
      measurableEnergy = kineticEnergy;
    }

    else if (particleDef == G4AntiNeutron::Definition()) {
      measurableEnergy = 2.* totalEnergy - kineticEnergy;
    }
    else if (particleDef == G4AntiProton::Definition() ||
             particleDef == G4AntiLambda::Definition() ||
             particleDef == G4AntiSigmaPlus::Definition() ||
             particleDef == G4AntiSigmaZero::Definition() ||
             // Need AntiSigmacPlus and Zero as well?
             particleDef == G4AntiSigmaMinus::Definition() ||
             particleDef == G4AntiXiZero::Definition() ||
             // Need AntiXicMinus and Zero as well?
             particleDef == G4AntiXiMinus::Definition() ||
             particleDef == G4AntiOmegaMinus::Definition()) {
      measurableEnergy = 2.* totalEnergy - kineticEnergy;
    }

    if (measurableEnergy < -0.0001) {
      G4cerr << "Calibration Hit: NEGATIVE measurableEnergyV2=" << measurableEnergy
             << " < -0.0001 MeV for " << particleDef->GetParticleName() << G4endl;
      measurableEnergy = 0.;
    }

    //for tests:
    //measurableEnergy = kineticEnergy;

    return measurableEnergy;
  }


  // 5-Dec-2003 Mikhail Leltchouk: extended version of FORTRAN Function PrMeasE
  // used by Michael Kuhlen and Peter Loch with Geant3 since 1991.
  //
  // Purpose: returns energy measurable in a calorimeter.
  //
  // Rest masses of stable particles do not give rise to a signal in
  // the calorimeter. The measurable energy is obtained by subtracting
  // the rest mass from the total energy for these particles and those
  // particles which may decay into them. For antiparticles the rest
  // mass has to be added due to annihilation processes.
  //
  // Input: particleDef   - Geant4 particle definition
  //        PDGEncoding   - Particle Data Group (PDG) particle number
  //        totalEnergy   - particle total energy
  //        kineticEnergy - particle kinetic energy
  //
  // Output: measurableEnergy - energy measurable in a calorimeter
  G4double SimulationEnergies::measurableEnergy(const G4ParticleDefinition* particleDef,
                                                G4int PDGEncoding,
                                                G4double totalEnergy,
                                                G4double kineticEnergy) const
  {
    const G4double electronMass = G4Electron::Definition()->GetPDGMass();
    const G4double protonMass = G4Proton::Definition()->GetPDGMass();
    const G4double neutronMass = G4Neutron::Definition()->GetPDGMass();

#ifdef DEBUG_ENERGIES
    G4cout << "SimulationEnergies initialization: " << G4endl;
    G4cout << ">>>> electronMass=" << electronMass << G4endl;
    G4cout << ">>>> protonMass=" << protonMass << G4endl;
    G4cout << ">>>> neutronMass=" << neutronMass << G4endl;
#endif

    G4double correctionForMass = 0.;
    if (particleDef == G4Electron::Definition()){
      correctionForMass = - electronMass;
    }
    else if (particleDef == G4Positron::Definition()){
      correctionForMass = + electronMass;
    }
    else if (particleDef == G4Proton::Definition()){
      correctionForMass = - protonMass;
    }
    else if (particleDef == G4Neutron::Definition()){
      correctionForMass = - neutronMass;
    }
    else if (PDGEncoding > 1000010019 ){ //for nuclei
      return kineticEnergy;
    }
    else if (particleDef == G4Lambda::Definition() ||
             particleDef == G4SigmaPlus::Definition() ||
             particleDef == G4SigmaZero::Definition() ||
             particleDef == G4XiMinus::Definition() ||
             particleDef == G4XiZero::Definition() ||
             particleDef == G4OmegaMinus::Definition()) {
      correctionForMass = - protonMass;
    }
    else if (particleDef == G4SigmaMinus::Definition()) { // Need Sigma Minus?
      correctionForMass = - neutronMass;
    }

    else if (particleDef == G4AntiNeutron::Definition()) {
      correctionForMass = + neutronMass;
    }
    else if (particleDef == G4AntiProton::Definition() ||
             particleDef == G4AntiLambda::Definition() ||
             particleDef == G4AntiSigmaZero::Definition() ||
             // Need AntiSigmacPlus and Zero as well?
             particleDef == G4AntiSigmaPlus::Definition() ||
             particleDef == G4AntiXiZero::Definition() ||
             // Need AntiXicMinus and Zero as well?
             particleDef == G4AntiXiMinus::Definition() ||
             particleDef == G4AntiOmegaMinus::Definition()) {
      correctionForMass = + protonMass;
    }
    else if (particleDef == G4AntiSigmaMinus::Definition()) {
      correctionForMass = + neutronMass;
    }

    G4double measurableEnergy = totalEnergy + correctionForMass;

    if (measurableEnergy < -0.0001) {
      G4cerr << "Calibration Hit: NEGATIVE measurableEnergy="
             << measurableEnergy << " < -0.0001 MeV for "
             << particleDef->GetParticleName() << G4endl;
      measurableEnergy = 0.;
    }
    return measurableEnergy;
  }


  G4bool SimulationEnergies::ProcessEscapedEnergy( G4Step* fakeStep ) const
  {
    // Escaped energy requires special processing.  The energy was not
    // deposited in the current G4Step, but at the beginning of the
    // particle's track.  For example, if a neutrino escapes the
    // detector, the energy should be recorded at the point where the
    // neutrino was created, not at the point where it escaped the
    // detector.

    // Different detectors have different procedures for handling
    // escaped energy.  We use the EscapedEnergyRegistry class to
    // route the request for special processing to the appropriate
    // procedure.

    // Choose which VEscapedEnergyProcessing routine we'll use based
    // on the volume name.

    CaloG4::EscapedEnergyRegistry* registry =
      CaloG4::EscapedEnergyRegistry::GetInstance();
    CaloG4::VEscapedEnergyProcessing* eep;

    G4TouchableHandle touchableHandle = fakeStep->GetPreStepPoint()->GetTouchableHandle();
    if (touchableHandle->GetHistoryDepth()) {
      // normal case: volume name exists
      G4String volumeName = touchableHandle->GetVolume()->GetName();
      eep = registry->GetProcessing( volumeName );

      if ( eep != nullptr ) {
        // Call the appropriate routine.
        return eep->Process( fakeStep );
      }
      // If we reach here, we're in a volume that has no escaped energy
      // procedure (probably neither Tile nor LAr).

      // I don't know about Tile, but in LAr there is a default
      // procedure for non-sensitive volumes.  Let's use that (for now).

      eep = registry->GetProcessing( "LAr::" );
      if (ATH_LIKELY(eep)) {
        return eep->Process( fakeStep );
      }

      // If we get here, the registry was never initialized for LAr.
      std::call_once(warning1OnceFlag, [](){
          G4cout << "SimulationEnergies::ProcessEscapedEnergy - " << G4endl
                 << "   WARNING! CaloG4::EscapedEnergyRegistry was never initialized for 'LArG4::'" << G4endl
                 << "   and LArG4Sim is the package with the code that handles CalibrationHits" << G4endl
                 << "   in non-sensitive volumes.  Not all energies deposited in this simulation" << G4endl
                 << "   will be recorded." << G4endl;
        });

      eep = registry->GetProcessing( "Tile::" );
      if (ATH_LIKELY(eep)) {
        std::call_once(warning3OnceFlag, [](){
            G4cout << "  Using TileGeoG4CalibSD for escaped energy processing," << G4endl
                   << "  since LAr SD is not available" << G4endl;
            });
        return eep->Process( fakeStep );
      } else {
        std::call_once(warning3OnceFlag, [](){
            G4cout << "  WARNING! TileGeoG4CalibSD was never initialized as well," << G4endl
                   << "  escaped energies will be definitely lost" << G4endl;
            });
        return false;
      }
    }
    else {
      // If we reach here, we're in an area with geometry problem
      // There is a default procedure for non-sensitive volumes in LAr.
      //  Let's use that (for now).

      eep = registry->GetProcessing( "LAr::" );
      if (ATH_LIKELY(eep)) {
        return eep->Process( fakeStep );
      }

      // If we get here, the registry was never initialized for LAr.
      std::call_once(warning2OnceFlag, [](){
          G4cout << "SimulationEnergies::ProcessEscapedEnergy - " << G4endl
                 <<"   WARNING! touchableHandle->GetVolume()==0  geometry problem ?  and also" << G4endl
                 << "   WARNING! CaloG4::EscapedEnergyRegistry was never initialized for 'LArG4::'" << G4endl
                 << "   and LArG4Sim is the package with the code that handles CalibrationHits" << G4endl
                 << "   in non-sensitive volumes.  Not all energies deposited in this simulation" << G4endl
                 << "   will be recorded." << G4endl;
        });

      eep = registry->GetProcessing( "Tile::" );
      if (ATH_LIKELY(eep)) {
        std::call_once(warning4OnceFlag, [](){
            G4cout << "  Using TileGeoG4CalibSD for escaped energy processing," << G4endl
                   << "  since LAr SD is not available" << G4endl;
            });
        return eep->Process( fakeStep );
      } else {
        std::call_once(warning4OnceFlag, [](){
            G4cout << "  WARNING! TileGeoG4CalibSD was never initialized as well," << G4endl
                   << "  escaped energies will be definitely lost" << G4endl;
            });
        return false;
      }
    }
  }

  inline G4bool SimulationEnergies::ParticleIsNeutrino( G4ParticleDefinition* particle ) const
  {
    return (particle == G4NeutrinoE::Definition() || particle == G4AntiNeutrinoE::Definition() || // nu_e,   anti_nu_e
            particle == G4NeutrinoMu::Definition() || particle == G4AntiNeutrinoMu::Definition() || // nu_mu,  anti_nu_mu
            particle == G4NeutrinoTau::Definition() || particle == G4AntiNeutrinoTau::Definition());   // nu_tau, anti_nu_tau
  }

std::unique_ptr<G4Step> SimulationEnergies::CreateFakeStep(G4Track* a_track, G4double a_energy) const
{
  const G4ThreeVector& a_point = a_track->GetVertexPosition();

  // Locate the G4TouchableHandle associated with the volume
  // containing "a_point".
  G4TouchableHandle   a_touchableHandle = new G4TouchableHistory();
  G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking()->LocateGlobalPointAndUpdateTouchable(a_point,
                                                                   a_touchableHandle(),
                                                                   false);

  auto fakeStep = std::make_unique<G4Step>();
  G4StepPoint*        fakePreStepPoint  = fakeStep->GetPreStepPoint();
  G4StepPoint*        fakePostStepPoint = fakeStep->GetPostStepPoint();
  // Set the touchable volume at PreStepPoint:
  fakePreStepPoint->SetTouchableHandle(a_touchableHandle);
  fakePreStepPoint->SetPosition(a_point);
  fakePreStepPoint->SetGlobalTime(0.);
  // Most of the calculators expect a PostStepPoint as well.  For
  // now, try setting this to be the same as the PreStepPoint.
  fakePostStepPoint->SetTouchableHandle(a_touchableHandle);
  fakePostStepPoint->SetPosition(a_point);
  fakePostStepPoint->SetGlobalTime(0.);
  // The total energy deposit in the step is actually irrelevant.
  fakeStep->SetTotalEnergyDeposit(a_energy);
  fakeStep->SetTrack(a_track);
  return fakeStep;
}


} // namespace LArG4
