/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Primary include
#include "G4AtlasTools/G4FieldManagerToolBase.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// Some geant headers erroneously expect these to be in the global namespace.
// Don't use using, so as not to trigger warnings from the namespace
// static checker.
static constexpr double perThousand = CLHEP::perThousand;
static constexpr double perMillion  = CLHEP::perMillion;

// Geant4 steppers
#include "G4AtlasRK4.hh"
#include "G4BogackiShampine23.hh"
#include "G4BogackiShampine45.hh"
#include "G4CashKarpRKF45.hh"
#include "G4ClassicalRK4.hh"
#include "G4DoLoMcPriRK34.hh"
#include "G4DormandPrince745.hh"
#include "G4DormandPrinceRK56.hh"
#include "G4DormandPrinceRK78.hh"
#include "G4FieldManager.hh"
#include "G4HelixExplicitEuler.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4HelixSimpleRunge.hh"
#include "G4NystromRK4.hh"
#include "G4RK547FEq1.hh"
#include "G4RK547FEq2.hh"
#include "G4RK547FEq3.hh"
#include "G4RKG3_Stepper.hh"
#include "G4TsitourasRK45.hh"

// Geant4 includes
#include "G4ChordFinder.hh"
#include "G4IntegrationDriver.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4VIntegrationDriver.hh"

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
  : base_class(type, name, parent)
{
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

#if G4VERSION_NUMBER >= 1040
//=============================================================================
// Create the driver with a stepper (Geant4 >= 10.4)
//=============================================================================
G4VIntegrationDriver*
G4FieldManagerToolBase::createDriverAndStepper(std::string name, G4MagneticField* field) const
{
  ATH_MSG_DEBUG("createDriverAndStepper");
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
  G4VIntegrationDriver* driver = nullptr;
  if (name=="HelixImplicitEuler") {
    G4HelixImplicitEuler* stepper = new G4HelixImplicitEuler(eqRhs);
    driver = new G4IntegrationDriver<G4HelixImplicitEuler>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="HelixSimpleRunge") {
    G4HelixSimpleRunge* stepper = new G4HelixSimpleRunge(eqRhs);
    driver = new G4IntegrationDriver<G4HelixSimpleRunge>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="HelixExplicitEuler") {
    G4HelixExplicitEuler* stepper = new G4HelixExplicitEuler(eqRhs);
    driver = new G4IntegrationDriver<G4HelixExplicitEuler>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="NystromRK4") {
    G4NystromRK4* stepper = new G4NystromRK4(eqRhs);
    driver = new G4IntegrationDriver<G4NystromRK4>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="ClassicalRK4") {
    G4ClassicalRK4* stepper = new G4ClassicalRK4(eqRhs);
    driver = new G4IntegrationDriver<G4ClassicalRK4>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="AtlasRK4") {
    G4AtlasRK4* stepper = new G4AtlasRK4(eqRhs);
    driver = new G4IntegrationDriver<G4AtlasRK4>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="BogackiShampine23") {
    G4BogackiShampine23* stepper = new G4BogackiShampine23(eqRhs);
    driver = new G4IntegrationDriver<G4BogackiShampine23>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="BogackiShampine45") {
    G4BogackiShampine45* stepper = new G4BogackiShampine45(eqRhs);
    driver = new G4IntegrationDriver<G4BogackiShampine45>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="CashKarpRKF45") {
    G4CashKarpRKF45* stepper = new G4CashKarpRKF45(eqRhs);
    driver = new G4IntegrationDriver<G4CashKarpRKF45>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="DoLoMcPriRK34") {
    G4DoLoMcPriRK34* stepper = new G4DoLoMcPriRK34(eqRhs);
    driver = new G4IntegrationDriver<G4DoLoMcPriRK34>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="DormandPrince745") {
    G4DormandPrince745* stepper = new G4DormandPrince745(eqRhs);
    driver = new G4IntegrationDriver<G4DormandPrince745>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="DormandPrinceRK56") {
    G4DormandPrinceRK56* stepper = new G4DormandPrinceRK56(eqRhs);
    driver = new G4IntegrationDriver<G4DormandPrinceRK56>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="DormandPrinceRK78") {
    G4DormandPrinceRK78* stepper = new G4DormandPrinceRK78(eqRhs);
    driver = new G4IntegrationDriver<G4DormandPrinceRK78>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="RK547FEq1") {
    G4RK547FEq1* stepper = new G4RK547FEq1(eqRhs);
    driver = new G4IntegrationDriver<G4RK547FEq1>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="RK547FEq2") {
    G4RK547FEq2* stepper = new G4RK547FEq2(eqRhs);
    driver = new G4IntegrationDriver<G4RK547FEq2>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="RK547FEq3") {
    G4RK547FEq3* stepper = new G4RK547FEq3(eqRhs);
    driver = new G4IntegrationDriver<G4RK547FEq3>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="RKG3_Stepper") {
    G4RKG3_Stepper* stepper = new G4RKG3_Stepper(eqRhs);
    driver = new G4IntegrationDriver<G4RKG3_Stepper>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else if (name=="TsitourasRK45") {
    G4TsitourasRK45* stepper = new G4TsitourasRK45(eqRhs);
    driver = new G4IntegrationDriver<G4TsitourasRK45>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  } else {
    ATH_MSG_ERROR("Stepper " << name << " not available! returning NystromRK4!");
    G4NystromRK4* stepper = new G4NystromRK4(eqRhs);
    driver = new G4IntegrationDriver<G4NystromRK4>(
        m_minStep, stepper, stepper->GetNumberOfVariables());
  }
  return driver;
}
#endif

#if G4VERSION_NUMBER < 1040
//=============================================================================
// Create the stepper (Geant4 < 10.4)
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
  else if (name=="BogackiShampine23") return new G4BogackiShampine23(eqRhs);
  else if (name=="BogackiShampine45") return new G4BogackiShampine45(eqRhs);
  else if (name=="CashKarpRKF45") return new G4CashKarpRKF45(eqRhs);
  else if (name=="DoLoMcPriRK34") return new G4DoLoMcPriRK34(eqRhs);
  else if (name=="DormandPrince745") return new G4DormandPrince745(eqRhs);
  else if (name=="DormandPrinceRK56") return new G4DormandPrinceRK56(eqRhs);
  else if (name=="DormandPrinceRK78") return new G4DormandPrinceRK78(eqRhs);
  else if (name=="RK547FEq1") return new G4RK547FEq1(eqRhs);
  else if (name=="RK547FEq2") return new G4RK547FEq2(eqRhs);
  else if (name=="RK547FEq3") return new G4RK547FEq3(eqRhs);
  else if (name=="RKG3_Stepper") return new G4RKG3_Stepper(eqRhs);
  else if (name=="TsitourasRK45") return new G4TsitourasRK45(eqRhs);
  else {
    ATH_MSG_ERROR("Stepper " << name << " not available! returning NystromRK4!");
    return new G4NystromRK4(eqRhs);
  }
}
#endif

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
