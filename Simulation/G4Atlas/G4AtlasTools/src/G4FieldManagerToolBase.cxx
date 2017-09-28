/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Primary include
#include "G4AtlasTools/G4FieldManagerToolBase.h"

// Geant4 includes
#include "G4FieldManager.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4NystromRK4.hh"
#include "G4ClassicalRK4.hh"
#include "G4AtlasRK4.hh"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : G4FieldManagerToolBase
//
// 2015-11-17: Andrea Dell'Acqua
//-----------------------------------------------------------------------------


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4FieldManagerToolBase::G4FieldManagerToolBase(const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent)
  : base_class(type, name, parent),
    m_fieldSvc("G4FieldSvc", name),
    m_fieldOn(true),
    m_integratorStepper("AtlasRK4"),
    m_equationOfMotion("", this), //private toolhandle
    m_minEps(-1.0),
    m_maxEps(-1.0),
    m_deltaChord(-1.0),
    m_deltaOneStep(-1.0),
    m_deltaIntersection(-1.0),
    m_maxStep(-1.0) // (1.*CLHEP::m)
{
  // Declare common configurable properties
  declareProperty( "FieldSvc", m_fieldSvc, "Service providing a G4MagneticField");
  declareProperty( "FieldOn", m_fieldOn, "Toggles field on/off");
  declareProperty( "IntegratorStepper", m_integratorStepper, "Integrator stepper name" );
  declareProperty( "EquationOfMotion", m_equationOfMotion, "");
  declareProperty( "DeltaChord", m_deltaChord, "Missing distance for the chord finder" );
  declareProperty( "DeltaOneStep", m_deltaOneStep, "Delta(one-step)" );
  declareProperty( "DeltaIntersection", m_deltaIntersection, "Accuracy for boundary intersection" );
  declareProperty( "MinimumEpsilonStep", m_minEps, "Minimum epsilon (see G4 documentation)" );
  declareProperty( "MaximumEpsilonStep", m_maxEps, "Maximum epsilon (see G4 documentation)" );
  declareProperty( "MaximumStep", m_maxStep, "Maximum step length in field (see G4 documentation)" );
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode G4FieldManagerToolBase::initialize( )
{
  ATH_MSG_DEBUG("G4FieldManagerToolBase::initialize()");

  // Skip field svc retrieval if field disabled.
  if(m_fieldOn) {
    ATH_CHECK( m_fieldSvc.retrieve() );
  }
  else {
    ATH_MSG_DEBUG("Field disabled. Not retrieving G4 field svc.");
  }
  if (!m_equationOfMotion.empty())
    {
      ATH_CHECK(m_equationOfMotion.retrieve());
    }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Create the stepper
//=============================================================================
G4MagIntegratorStepper*
G4FieldManagerToolBase::getStepper(std::string name, G4MagneticField* field) const
{
  ATH_MSG_DEBUG("getStepper");
  G4Mag_EqRhs* eqRhs(nullptr);
  if (!m_equationOfMotion.empty())
    {
      eqRhs = m_equationOfMotion->makeEquationOfMotion(field);
      ATH_MSG_INFO("Configuring alternative equation of motion using " <<
                   m_equationOfMotion.name() );
    }
  else
    {
      ATH_MSG_VERBOSE("Using G4Mag_UsualEqRhs as the equation of motion.");
      eqRhs = new G4Mag_UsualEqRhs(field);
    }
  // @TODO Add some way of confirming that the choices of Equation of
  // motion and stepper are compatible.
  // @TODO consider moving the stepper choice into a tool as well?
  // Only worthwhile if we envisage adding more stepper choices in the
  // future.
  if (name=="HelixImplicitEuler") return new G4HelixImplicitEuler(eqRhs);
  else if (name=="HelixSimpleRunge") return new G4HelixSimpleRunge(eqRhs);
  else if (name=="HelixExplicitEuler") return new G4HelixExplicitEuler(eqRhs);
  else if (name=="NystromRK4") return new G4NystromRK4(eqRhs);
  else if (name=="ClassicalRK4") return new G4ClassicalRK4(eqRhs);
  else if (name=="AtlasRK4") return new G4AtlasRK4(eqRhs);
  else {
    ATH_MSG_ERROR("Stepper " << name << " not available! returning NystromRK4!");
    return new G4NystromRK4(eqRhs);
  }
}

//=============================================================================
// Create the stepper
//=============================================================================
StatusCode G4FieldManagerToolBase::
setFieldParameters(G4FieldManager* fieldMgr) const
{
  ATH_MSG_DEBUG("setFieldParameters");
  if (fieldMgr) {
    if (m_deltaChord>0) fieldMgr->GetChordFinder()->SetDeltaChord(m_deltaChord);
    if (m_deltaOneStep>0) fieldMgr->SetDeltaOneStep(m_deltaOneStep);
    if (m_deltaIntersection>0) fieldMgr->SetDeltaIntersection(m_deltaIntersection);
    if (m_minEps>0) fieldMgr->SetMinimumEpsilonStep(m_minEps);
    if (m_maxEps>0) fieldMgr->SetMaximumEpsilonStep(m_maxEps);
  }
  else {
    ATH_MSG_ERROR("setFieldParameters received NULL field mgr!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
