//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: QuirkTransportation.cxx 717143 2016-01-11 14:18:11Z jchapman $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
// 
// ------------------------------------------------------------
//  GEANT 4  include file implementation
//
// ------------------------------------------------------------
//
// This class is a process responsible for the transportation of 
// a particle, ie the geometrical propagation that encounters the 
// geometrical sub-volumes of the detectors.
//
// It is also tasked with the key role of proposing the "isotropic safety",
//   which will be used to update the post-step point's safety.
//
// =======================================================================
// Modified:   
//   20 Nov  2008, J.Apostolakis: Push safety to helper - after ComputeSafety
//    9 Nov  2007, J.Apostolakis: Flag for short steps, push safety to helper
//   19 Jan  2006, P.MoraDeFreitas: Fix for suspended tracks (StartTracking)
//   11 Aug  2004, M.Asai: Add G4VSensitiveDetector* for updating stepPoint.
//   21 June 2003, J.Apostolakis: Calling field manager with 
//                     track, to enable it to configure its accuracy
//   13 May  2003, J.Apostolakis: Zero field areas now taken into
//                     account correclty in all cases (thanks to W Pokorski).
//   29 June 2001, J.Apostolakis, D.Cote-Ahern, P.Gumplinger: 
//                     correction for spin tracking   
//   20 Febr 2001, J.Apostolakis:  update for new FieldTrack
//   22 Sept 2000, V.Grichine:     update of Kinetic Energy
// Created:  19 March 1997, J. Apostolakis
// =======================================================================

#include "G4ProductionCutsTable.hh"
#include "G4ParticleTable.hh"
#include "G4ChordFinder.hh"
#include "G4SafetyHelper.hh"
#include "G4FieldManagerStore.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4Version.hh"

#include "Quirk.h"
#include "InfracolorForce.h"
#include "HyperbolaStepper.h"
#include "QuirkTransportation.h"

class G4VSensitiveDetector;

//////////////////////////////////////////////////////////////////////////
//
// Constructor

QuirkTransportation::QuirkTransportation( G4int verboseLevel )
  : G4VProcess( G4String("QuirkTransportation"), fTransportation ),
    fParticleIsLooping( false ),
    fPreviousSftOrigin (0.,0.,0.),
    fPreviousSafety    ( 0.0 ),
    fThreshold_Warning_Energy( 100 * CLHEP::MeV ),  
    fThreshold_Important_Energy( 250 * CLHEP::MeV ), 
    fThresholdTrials( 10 ), 
    fUnimportant_Energy( 1 * CLHEP::MeV ),  // Not used
    fNoLooperTrials(0),
    fSumEnergyKilled( 0.0 ), fMaxEnergyKilled( 0.0 ), 
    fShortStepOptimisation(false),    // Old default: true (=fast short steps)
    fVerboseLevel( verboseLevel )
{
  G4TransportationManager* transportMgr ; 

  transportMgr = G4TransportationManager::GetTransportationManager() ; 

  fLinearNavigator = transportMgr->GetNavigatorForTracking() ; 

  // fGlobalFieldMgr = transportMgr->GetFieldManager() ;

  fFieldPropagator = transportMgr->GetPropagatorInField() ;

  fpSafetyHelper =   transportMgr->GetSafetyHelper();  // New 

  // Cannot determine whether a field exists here, as it would 
  //  depend on the relative order of creating the detector's 
  //  field and this process. That order is not guaranted.
  // Instead later the method DoesGlobalFieldExist() is called

  static G4TouchableHandle nullTouchableHandle;  // Points to (G4VTouchable*) 0
  fCurrentTouchableHandle = nullTouchableHandle; 

  fCandidateEndGlobalTime = 0;
}

//////////////////////////////////////////////////////////////////////////

QuirkTransportation::~QuirkTransportation()
{
  if( (fVerboseLevel > 0) && (fSumEnergyKilled > 0.0 ) ){ 
    G4cout << " QuirkTransportation: Statistics for looping particles " << G4endl;
    G4cout << "   Sum of energy of loopers killed: " <<  fSumEnergyKilled << G4endl;
    G4cout << "   Max energy of loopers killed: " <<  fMaxEnergyKilled << G4endl;
  } 
}

//////////////////////////////////////////////////////////////////////////
//
// Responsibilities:
//    Find whether the geometry limits the Step, and to what length
//    Calculate the new value of the safety and return it.
//    Store the final time, position and momentum.

G4double QuirkTransportation::
AlongStepGetPhysicalInteractionLength( const G4Track&  track,
                                             G4double, //  previousStepSize
                                             G4double  currentMinimumStep,
                                             G4double& currentSafety,
                                             G4GPILSelection* selection )
{
  G4double geometryStepLength; 
  fParticleIsLooping = false ;

  // Initial actions moved to  StartTrack()   
  // --------------------------------------
  // Note: in case another process changes touchable handle
  //    it will be necessary to add here (for all steps)   
  // fCurrentTouchableHandle = aTrack->GetTouchableHandle();

  // GPILSelection is set to defaule value of CandidateForSelection
  // It is a return value
  //
  *selection = CandidateForSelection ;

  // Get initial Energy/Momentum of the track
  //
  const G4DynamicParticle*    pParticle  = track.GetDynamicParticle() ;
  const G4ParticleDefinition* pParticleDef   = pParticle->GetDefinition() ;
  G4ThreeVector startMomentumDir       = pParticle->GetMomentumDirection() ;
  G4ThreeVector startPosition          = track.GetPosition() ;

  // G4double   theTime        = track.GetGlobalTime() ;

  // The Step Point safety can be limited by other geometries and/or the 
  // assumptions of any process - it's not always the geometrical safety.
  // We calculate the starting point's isotropic safety here.
  //
  G4ThreeVector OriginShift = startPosition - fPreviousSftOrigin ;
  G4double      MagSqShift  = OriginShift.mag2() ;
  if( MagSqShift >= sqr(fPreviousSafety) )
  {
     currentSafety = 0.0 ;
  }
  else
  {
     currentSafety = fPreviousSafety - std::sqrt(MagSqShift) ;
  }

  // Is the particle charged ?
  //
  G4double              particleCharge = pParticle->GetCharge() ; 

  // There is no need to locate the current volume. It is Done elsewhere:
  //   On track construction 
  //   By the tracking, after all AlongStepDoIts, in "Relocation"

  // Set up field manager
  G4FieldManager* fieldMgr = fFieldPropagator->FindAndSetFieldManager( track.GetVolume() ); 
  if (fieldMgr != 0) {
     // Message the field Manager, to configure it for this track
     fieldMgr->ConfigureForTrack( &track );
  } else {
     G4Exception("QuirkTransportation::AlongStepGetPhysicalInteractionLength", "QuirkNoFieldMgr", RunMustBeAborted, "no field manager");
  }

  // Set up stepper to calculate quirk trajectory
  const Quirk* quirkDef = dynamic_cast<const Quirk*>(pParticleDef);
  if (quirkDef == 0) {
     G4Exception("QuirkTransportation::AlongStepGetPhysicalInteractionLength", "NonQuirk", FatalErrorInArgument, "QuirkTransportation run on non-quirk particle");
  }
  HyperbolaStepper quirkStepper(
     quirkDef->GetStringIn(),
     track,
     fieldMgr->DoesFieldExist() ? fieldMgr->GetDetectorField() : 0
  );

  // Switch out chord finder with quirk chord finder
  G4ChordFinder* oldChordFinder = fieldMgr->GetChordFinder();
  G4ChordFinder quirkChordFinder(
     new G4MagInt_Driver(0.0, &quirkStepper, quirkStepper.GetNumberOfVariables())
  );
  fieldMgr->SetChordFinder(&quirkChordFinder);

  G4double       momentumMagnitude = pParticle->GetTotalMomentum() ;
  G4ThreeVector  EndUnitMomentum ;
  G4double       restMass = pParticleDef->GetPDGMass() ;

#if G4VERSION_NUMBER > 1009
  //FIXME untested!!!
  G4ChargeState chargeState(particleCharge,    // in e+ units
                            quirkDef->GetPDGSpin(),
                            0,
                            0,
                            0); //no magnetic charge?
  G4EquationOfMotion* equationOfMotion = (fFieldPropagator->GetChordFinder()->GetIntegrationDriver()->GetStepper())->GetEquationOfMotion();
  equationOfMotion->SetChargeMomentumMass( chargeState,
                                           momentumMagnitude, // in Mev/c 
                                           restMass           ) ;  
#else
  fFieldPropagator->SetChargeMomentumMass( particleCharge,    // in e+ units
                                           momentumMagnitude, // in Mev/c 
                                           restMass           ) ;  
#endif

  G4ThreeVector spin        = track.GetPolarization() ;
  G4FieldTrack  aFieldTrack = G4FieldTrack( startPosition, 
                                            track.GetMomentumDirection(),
                                            0.0, 
                                            track.GetKineticEnergy(),
                                            restMass,
                                            track.GetVelocity(),
                                            track.GetGlobalTime(), // Lab.
                                            0.0,                   // substituting step length for proper time
                                            &spin                  ) ;

  //if (currentMinimumStep <= 0) {
  //   G4cout << "QuirkTransportation: currentMinimumStep = " << currentMinimumStep << G4endl;
  //   G4Exception(
  //       "QuirkTransportation::AlongStepGetPhysicalInteractionLength",
  //       "BadMinimumStep",
  //       EventMustBeAborted,
  //       "QuirkTransportation: currentMinimumStep <= 0"
  //   );
  //}
  G4bool dbg = false; //(track.GetCurrentStepNumber() % 1000000 == 0);
  quirkStepper.SetDebug(dbg);
  if (dbg) G4cout << "QuirkTransportation: start = " << aFieldTrack.GetPosition() << G4endl;
  if (dbg) G4cout << "QuirkTransportation: currentMinimumStep = " << currentMinimumStep << G4endl;
  if (dbg) G4cout << "QuirkTransportation: maxlength = " << quirkStepper.GetMaxLength() << G4endl;
  currentMinimumStep = std::min(currentMinimumStep, quirkStepper.GetMaxLength());
  if (dbg) G4cout << "QuirkTransportation: currentMinimumStep = " << currentMinimumStep << G4endl;
  if( currentMinimumStep > 0 )
  {
     // Call field propagator to handle boundary crossings
     G4double lengthAlongCurve = fFieldPropagator->ComputeStep( aFieldTrack,
                                                                currentMinimumStep, 
                                                                currentSafety,
                                                                track.GetVolume() ) ;
     if (dbg) G4cout << "QuirkTransportation: moved " << lengthAlongCurve << G4endl;
     fGeometryLimitedStep = lengthAlongCurve < currentMinimumStep;
     if( fGeometryLimitedStep ) {
        geometryStepLength   = lengthAlongCurve ;
     } else {
        geometryStepLength   = currentMinimumStep ;
     }
     // Update stepper, string vectors with step length from field propagator
     quirkStepper.Update(aFieldTrack, aFieldTrack.GetCurveLength() == 0 && !fGeometryLimitedStep);
  }
  else
  {
     geometryStepLength   = 0.0 ;
     fGeometryLimitedStep = false ;
  }
  if (dbg) G4cout << "QuirkTransportation: moved " << aFieldTrack.GetCurveLength() << G4endl;
  if (dbg) G4cout << "QuirkTransportation: end = " << aFieldTrack.GetPosition() << " [" << aFieldTrack.GetProperTimeOfFlight() << "]" << G4endl;

  // Remember last safety origin & value.
  //
  fPreviousSftOrigin = startPosition ;
  fPreviousSafety    = currentSafety ;         
  // fpSafetyHelper->SetCurrentSafety( newSafety, startPosition);

  // Get end-of-step quantities
  geometryStepLength = aFieldTrack.GetCurveLength();
  fTransportEndPosition = aFieldTrack.GetPosition() ;
  fMomentumChanged         = true ; 
  fTransportEndMomentumDir = aFieldTrack.GetMomentumDir() ;
  fTransportEndKineticEnergy  = aFieldTrack.GetKineticEnergy() ; 
  fCandidateEndGlobalTime   = aFieldTrack.GetLabTimeOfFlight();
  fTransportEndSpin  = aFieldTrack.GetSpin();
  fParticleIsLooping = fFieldPropagator->IsParticleLooping() ;
  endpointDistance   = (fTransportEndPosition - startPosition).mag() ;

  // If we are asked to go a step length of 0, and we are on a boundary
  // then a boundary will also limit the step -> we must flag this.
  //
  if( currentMinimumStep == 0.0 ) 
  {
      if( currentSafety == 0.0 )  fGeometryLimitedStep = true ;
  }

  // Update the safety starting from the end-point,
  // if it will become negative at the end-point.
  //
  if( currentSafety < endpointDistance ) 
  {
       G4double endSafety =
            fLinearNavigator->ComputeSafety( fTransportEndPosition) ;
       currentSafety      = endSafety ;
       fPreviousSftOrigin = fTransportEndPosition ;
       fPreviousSafety    = currentSafety ; 
       fpSafetyHelper->SetCurrentSafety( currentSafety, fTransportEndPosition);

       // Because the Stepping Manager assumes it is from the start point, 
       //  add the StepLength
       //
       currentSafety     += endpointDistance ;

#ifdef G4DEBUG_TRANSPORT 
       G4cout.precision(12) ;
       G4cout << "***QuirkTransportation::AlongStepGPIL ** " << G4endl  ;
       G4cout << "  Called Navigator->ComputeSafety at " << fTransportEndPosition
              << "    and it returned safety= " << endSafety << G4endl ; 
       G4cout << "  Adding endpoint distance " << endpointDistance 
              << "   to obtain pseudo-safety= " << currentSafety << G4endl ; 
#endif
  }            

  fParticleChange.ProposeTrueStepLength(geometryStepLength) ;

  // Restore original stepper
  fieldMgr->SetChordFinder(oldChordFinder);

  return geometryStepLength ;
}

//////////////////////////////////////////////////////////////////////////
//
//   Initialize ParticleChange  (by setting all its members equal
//                               to corresponding members in G4Track)

G4VParticleChange* QuirkTransportation::AlongStepDoIt( const G4Track& track,
                                                       const G4Step&  /*stepData*/ )
{
  static G4int noCalls=0;

  noCalls++;

  fParticleChange.Initialize(track) ;

  //  Code for specific process 
  //
  fParticleChange.ProposePosition(fTransportEndPosition) ;
  fParticleChange.ProposeMomentumDirection(fTransportEndMomentumDir) ;
  fParticleChange.ProposeEnergy(fTransportEndKineticEnergy) ;
  fParticleChange.SetMomentumChanged(fMomentumChanged) ;

  fParticleChange.ProposePolarization(fTransportEndSpin);
  
  // Calculate  Lab Time of Flight (ONLY if field Equations used it!)
     // G4double endTime   = fCandidateEndGlobalTime;
     // G4double delta_time = endTime - startTime;

  G4double startTime = track.GetGlobalTime() ;
  G4double deltaTime = fCandidateEndGlobalTime - startTime ;
  fParticleChange.ProposeGlobalTime( fCandidateEndGlobalTime ) ;

  // Now Correct by Lorentz factor to get "proper" deltaTime
  
  G4double  restMass       = track.GetDynamicParticle()->GetMass() ;
  G4double deltaProperTime = deltaTime*( restMass/track.GetTotalEnergy() ) ;

  fParticleChange.ProposeProperTime(track.GetProperTime() + deltaProperTime) ;
  //fParticleChange. ProposeTrueStepLength( track.GetStepLength() ) ;

  // If the particle is caught looping or is stuck (in very difficult
  // boundaries) in a magnetic field (doing many steps) 
  //   THEN this kills it ...
  //
  if ( fParticleIsLooping )
  {
      G4double endEnergy= fTransportEndKineticEnergy;

      if( (endEnergy < fThreshold_Important_Energy) 
	  || (fNoLooperTrials >= fThresholdTrials ) ){
	// Kill the looping particle 
	//
	fParticleChange.ProposeTrackStatus( fStopAndKill )  ;

        // 'Bare' statistics
        fSumEnergyKilled += endEnergy; 
	if( endEnergy > fMaxEnergyKilled) { fMaxEnergyKilled= endEnergy; }

#ifdef G4VERBOSE
	if( (fVerboseLevel > 1) || 
	    ( endEnergy > fThreshold_Warning_Energy )  ) { 
	  G4cout << " QuirkTransportation is killing track that is looping or stuck "
		 << G4endl
		 << "   This track has " << track.GetKineticEnergy() / CLHEP::MeV
		 << " MeV energy." << G4endl;
	  G4cout << "   Number of trials = " << fNoLooperTrials 
		 << "   No of calls to AlongStepDoIt = " << noCalls 
		 << G4endl;
	}
#endif
	fNoLooperTrials=0; 
      }
      else{
	fNoLooperTrials ++; 
#ifdef G4VERBOSE
	if( (fVerboseLevel > 2) ){
	  G4cout << "   QuirkTransportation::AlongStepDoIt(): Particle looping -  "
		 << "   Number of trials = " << fNoLooperTrials 
		 << "   No of calls to  = " << noCalls 
		 << G4endl;
	}
#endif
      }
  }else{
      fNoLooperTrials=0; 
  }

  // Another (sometimes better way) is to use a user-limit maximum Step size
  // to alleviate this problem .. 

  // Introduce smooth curved trajectories to particle-change
  //
  fParticleChange.SetPointerToVectorOfAuxiliaryPoints
    (fFieldPropagator->GimmeTrajectoryVectorAndForgetIt() );

  return &fParticleChange ;
}

//////////////////////////////////////////////////////////////////////////
//
//  This ensures that the PostStep action is always called,
//  so that it can do the relocation if it is needed.
// 

G4double QuirkTransportation::
PostStepGetPhysicalInteractionLength( const G4Track&,
                                            G4double, // previousStepSize
                                            G4ForceCondition* pForceCond )
{ 
  *pForceCond = Forced ; 
  return DBL_MAX ;  // was kInfinity ; but convention now is DBL_MAX
}

/////////////////////////////////////////////////////////////////////////////
//

G4VParticleChange* QuirkTransportation::PostStepDoIt( const G4Track& track,
                                                   const G4Step& )
{
   G4TouchableHandle retCurrentTouchable ;   // The one to return
   G4bool isLastStep= false; 

  // Initialize ParticleChange  (by setting all its members equal
  //                             to corresponding members in G4Track)
  // fParticleChange.Initialize(track) ;  // To initialise TouchableChange

  fParticleChange.ProposeTrackStatus(track.GetTrackStatus()) ;

  // If the Step was determined by the volume boundary,
  // logically relocate the particle

  if(fGeometryLimitedStep)
  {  
    // fCurrentTouchable will now become the previous touchable, 
    // and what was the previous will be freed.
    // (Needed because the preStepPoint can point to the previous touchable)

    fLinearNavigator->SetGeometricallyLimitedStep() ;
    fLinearNavigator->
    LocateGlobalPointAndUpdateTouchableHandle( track.GetPosition(),
                                               track.GetMomentumDirection(),
                                               fCurrentTouchableHandle,
                                               true                      ) ;
    // Check whether the particle is out of the world volume 
    // If so it has exited and must be killed.
    //
    if( fCurrentTouchableHandle->GetVolume() == 0 )
    {
       fParticleChange.ProposeTrackStatus( fStopAndKill ) ;
       G4cout << "QuirkTransportation: number of steps = " << track.GetCurrentStepNumber() << G4endl;
    }
    retCurrentTouchable = fCurrentTouchableHandle ;
    fParticleChange.SetTouchableHandle( fCurrentTouchableHandle ) ;

    // Update the Step flag which identifies the Last Step in a volume
    isLastStep =  fLinearNavigator->ExitedMotherVolume() 
                       | fLinearNavigator->EnteredDaughterVolume() ;

#ifdef G4DEBUG_TRANSPORT
    //  Checking first implementation of flagging Last Step in Volume
    G4bool exiting =  fLinearNavigator->ExitedMotherVolume();
    G4bool entering = fLinearNavigator->EnteredDaughterVolume();
    if( ! (exiting || entering) ) { 
    G4cout << " Transport> :  Proposed isLastStep= " << isLastStep 
           << " Exiting "  << fLinearNavigator->ExitedMotherVolume()          
	   << " Entering " << fLinearNavigator->EnteredDaughterVolume() 
	   << G4endl;
    } 
#endif
  }
  else                 // fGeometryLimitedStep  is false
  {                    
    // This serves only to move the Navigator's location
    //
    fLinearNavigator->LocateGlobalPointWithinVolume( track.GetPosition() ) ;

    // The value of the track's current Touchable is retained. 
    // (and it must be correct because we must use it below to
    // overwrite the (unset) one in particle change)
    //  It must be fCurrentTouchable too ??
    //
    fParticleChange.SetTouchableHandle( track.GetTouchableHandle() ) ;
    retCurrentTouchable = track.GetTouchableHandle() ;

    isLastStep= false;
#ifdef G4DEBUG_TRANSPORT
    //  Checking first implementation of flagging Last Step in Volume
    G4cout << " Transport> Proposed isLastStep= " << isLastStep
	   << " Geometry did not limit step. " << G4endl;
#endif
  }         // endif ( fGeometryLimitedStep ) 

  fParticleChange.ProposeLastStepInVolume(isLastStep);    

  const G4VPhysicalVolume* pNewVol = retCurrentTouchable->GetVolume() ;
  const G4Material* pNewMaterial   = 0 ;
  const G4VSensitiveDetector* pNewSensitiveDetector   = 0 ;
                                                                                       
  if( pNewVol != 0 )
  {
    pNewMaterial= pNewVol->GetLogicalVolume()->GetMaterial();
    pNewSensitiveDetector= pNewVol->GetLogicalVolume()->GetSensitiveDetector();
  }

  // ( <const_cast> pNewMaterial ) ;
  // ( <const_cast> pNewSensitiveDetector) ;

  fParticleChange.SetMaterialInTouchable( (G4Material *) pNewMaterial ) ;
  fParticleChange.SetSensitiveDetectorInTouchable( (G4VSensitiveDetector *) pNewSensitiveDetector ) ;

  const G4MaterialCutsCouple* pNewMaterialCutsCouple = 0;
  if( pNewVol != 0 )
  {
    pNewMaterialCutsCouple=pNewVol->GetLogicalVolume()->GetMaterialCutsCouple();
  }

  if( pNewVol!=0 && pNewMaterialCutsCouple!=0 && pNewMaterialCutsCouple->GetMaterial()!=pNewMaterial )
  {
    // for parametrized volume
    //
    pNewMaterialCutsCouple =
      G4ProductionCutsTable::GetProductionCutsTable()
                             ->GetMaterialCutsCouple(pNewMaterial,
                               pNewMaterialCutsCouple->GetProductionCuts());
  }
  fParticleChange.SetMaterialCutsCoupleInTouchable( pNewMaterialCutsCouple );

  // temporarily until Get/Set Material of ParticleChange, 
  // and StepPoint can be made const. 
  // Set the touchable in ParticleChange
  // this must always be done because the particle change always
  // uses this value to overwrite the current touchable pointer.
  //
  fParticleChange.SetTouchableHandle(retCurrentTouchable) ;

  return &fParticleChange ;
}

// New method takes over the responsibility to reset the state of QuirkTransportation
//   object at the start of a new track or the resumption of a suspended track. 

void 
QuirkTransportation::StartTracking(G4Track* aTrack)
{
  G4VProcess::StartTracking(aTrack);

// The actions here are those that were taken in AlongStepGPIL
//   when track.GetCurrentStepNumber()==1

  // reset safety value and center
  //
  fPreviousSafety    = 0.0 ; 
  fPreviousSftOrigin = G4ThreeVector(0.,0.,0.) ;
  
  // reset looping counter -- for motion in field
  fNoLooperTrials= 0; 
  // Must clear this state .. else it depends on last track's value
  //  --> a better solution would set this from state of suspended track TODO ? 
  // Was if( aTrack->GetCurrentStepNumber()==1 ) { .. }

  // ChordFinder reset internal state
  //
  fFieldPropagator->ClearPropagatorState();   
    // Resets all state of field propagator class (ONLY)
    //  including safety values (in case of overlaps and to wipe for first track).

  // G4ChordFinder* chordF= fFieldPropagator->GetChordFinder();
  // if( chordF ) chordF->ResetStepEstimate();

  // Make sure to clear the chord finders of all fields (ie managers)
  static G4FieldManagerStore* fieldMgrStore= G4FieldManagerStore::GetInstance();
  fieldMgrStore->ClearAllChordFindersState(); 

  // Update the current touchable handle  (from the track's)
  //
  fCurrentTouchableHandle = aTrack->GetTouchableHandle();

  // Initialize infracolor string
  const Quirk* quirkDef = dynamic_cast<const Quirk*>(aTrack->GetParticleDefinition());
  if (quirkDef == 0) {
     G4Exception("QuirkTransportation::StartTracking", "NonQuirk", FatalErrorInArgument, "QuirkTransportation run on non-quirk particle");
  }
  quirkDef->GetStringIn().StartTracking(aTrack);
}

