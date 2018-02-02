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
// $Id: QuirkTransportation.h 583415 2014-02-14 16:44:26Z jblack $
// GEANT4 tag $Name: geant4-09-04-patch-01 $
//
// 
// ------------------------------------------------------------
//        GEANT 4  include file implementation
// ------------------------------------------------------------
//
// Class description:
//
// G4Transportation is a process responsible for the transportation of 
// a particle, i.e. the geometrical propagation encountering the 
// geometrical sub-volumes of the detectors.
// It is also tasked with part of updating the "safety".

// =======================================================================
// Created:  19 March 1997, J. Apostolakis
// =======================================================================
#ifndef QUIRKTRANSPORTATION_H
#define QUIRKTRANSPORTATION_H

#include "G4VProcess.hh"
#include "G4FieldManager.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleChangeForTransport.hh"
class G4SafetyHelper; 

class QuirkTransportation : public G4VProcess 
{
  // Concrete class that does the geometrical transport 

  public:  // with description

     QuirkTransportation( G4int verbosityLevel= 1);
     ~QuirkTransportation(); 

     G4double      AlongStepGetPhysicalInteractionLength(
                             const G4Track& track,
                                   G4double  previousStepSize,
                                   G4double  currentMinimumStep, 
                                   G4double& currentSafety,
                                   G4GPILSelection* selection
                            );

     G4VParticleChange* AlongStepDoIt(
                             const G4Track& track,
                             const G4Step& stepData
                            );

     G4VParticleChange* PostStepDoIt(
                             const G4Track& track,
                             const G4Step&  stepData
                            );
       // Responsible for the relocation.

     G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& ,
                             G4double   previousStepSize,
                             G4ForceCondition* pForceCond
                            );
       // Forces the PostStepDoIt action to be called, 
       // but does not limit the step.

     G4PropagatorInField* GetPropagatorInField();
     void SetPropagatorInField( G4PropagatorInField* pFieldPropagator);
       // Access/set the assistant class that Propagate in a Field.

     inline void   SetVerboseLevel( G4int verboseLevel );
     inline G4int  GetVerboseLevel() const;
       // Level of warnings regarding eg energy conservation
       // in field integration.

     inline G4double GetThresholdWarningEnergy() const; 
     inline G4double GetThresholdImportantEnergy() const; 
     inline G4int GetThresholdTrials() const; 

     inline void SetThresholdWarningEnergy( G4double newEnWarn ); 
     inline void SetThresholdImportantEnergy( G4double newEnImp ); 
     inline void SetThresholdTrials(G4int newMaxTrials ); 

     // Get/Set parameters for killing loopers: 
     //   Above 'important' energy a 'looping' particle in field will 
     //   *NOT* be abandoned, except after fThresholdTrials attempts.
     // Below Warning energy, no verbosity for looping particles is issued

     inline G4double GetMaxEnergyKilled() const; 
     inline G4double GetSumEnergyKilled() const;
     inline void ResetKilledStatistics( G4int report = 1);      
     // Statistics for tracks killed (currently due to looping in field)

     inline void EnableShortStepOptimisation(G4bool optimise=true); 
     // Whether short steps < safety will avoid to call Navigator (if field=0)

  public:  // without description

     G4double AtRestGetPhysicalInteractionLength(
                             const G4Track& ,
                             G4ForceCondition* 
                            ) { return -1.0; };
       // No operation in  AtRestDoIt.

     G4VParticleChange* AtRestDoIt(
                             const G4Track& ,
                             const G4Step&
                            ) {return 0;};
       // No operation in  AtRestDoIt.

  void StartTracking(G4Track* aTrack);
       // Reset state for new (potentially resumed) track 

  protected:

     G4bool               DoesGlobalFieldExist();
       // Checks whether a field exists for the "global" field manager.

  private:

     G4Navigator*         m_linearNavigator;
     G4PropagatorInField* m_fieldPropagator;
       // The Propagators used to transport the particle

     // G4FieldManager*      fGlobalFieldMgr;     // Used MagneticField CC
       // Field Manager for the whole Detector

     G4ThreeVector        m_transportEndPosition;
     G4ThreeVector        m_transportEndMomentumDir;
     G4double             m_transportEndKineticEnergy;
     G4ThreeVector        m_transportEndSpin;
     G4bool               m_momentumChanged;
     G4bool               m_energyChanged;
     G4double             m_candidateEndGlobalTime;
       // The particle's state after this Step, Store for DoIt

     G4bool               m_particleIsLooping;

     G4TouchableHandle    m_currentTouchableHandle;
     
     // G4bool         fFieldExists;
       // Whether a magnetic field exists ...
       // A data member for this is problematic: it is useful only if it
       // can be initialised and updated -- and a scheme is not yet possible.

     G4bool m_geometryLimitedStep;
       // Flag to determine whether a boundary was reached.

     G4ThreeVector  m_previousSftOrigin;
     G4double       m_previousSafety; 
       // Remember last safety origin & value.

     G4ParticleChangeForTransport m_particleChange;
       // New ParticleChange

     G4double m_endpointDistance;

  // Thresholds for looping particles: 
  // 
     G4double m_threshold_Warning_Energy;     //  Warn above this energy
     G4double m_threshold_Important_Energy;   //  Hesitate above this
     G4int    m_thresholdTrials;              //    for this no of trials
       // Above 'important' energy a 'looping' particle in field will 
       //   *NOT* be abandoned, except after fThresholdTrials attempts.
     G4double m_unimportant_Energy;
       //  Below this energy, no verbosity for looping particles is issued

  // Counter for steps in which particle reports 'looping',
  //   if it is above 'Important' Energy 
     G4int    m_noLooperTrials; 
  // Statistics for tracks abandoned
     G4double m_sumEnergyKilled;
     G4double m_maxEnergyKilled;

  // Whether to avoid calling G4Navigator for short step ( < safety)
  //   If using it, the safety estimate for endpoint will likely be smaller.
     G4bool   m_shortStepOptimisation; 

     G4SafetyHelper* m_safetyHelper;  // To pass it the safety value obtained

  // Verbosity 
     G4int    m_verboseLevel;
       // Verbosity level for warnings
       // eg about energy non-conservation in magnetic field.
};

#include "QuirkTransportation.icc"

#endif  
