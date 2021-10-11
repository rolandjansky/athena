/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "DetectorGeometrySvc.h"

#include "G4VUserDetectorConstruction.hh"

// For additional materials that are required
#include "G4NistManager.hh"
#include "G4Material.hh"

DetectorGeometrySvc::DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class(name,pSvcLocator)
{
  ATH_MSG_DEBUG( "DetectorGeometrySvc being created!" );
}

StatusCode DetectorGeometrySvc::initialize(){

  // go through all tools and retrieve them
  //  This fires initialize() for each of those tools
  ATH_MSG_DEBUG( "Setting up a DetectorConstruction " << m_detConstruction.name() );
  ATH_CHECK(m_detConstruction.retrieve());

  ATH_MSG_DEBUG( "DetectorGeometrySvc initialized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode DetectorGeometrySvc::finalize(){
  ATH_MSG_DEBUG( "DetectorGeometrySvc being finalized!!!" );
  // // Code for G4Field debugging
  // ATH_MSG_INFO("There are " << G4FieldManagerStore::GetInstance()->size() << " G4FieldManagers in the G4FieldManagerStore.");
  // for(auto fieldMgr: *G4FieldManagerStore::GetInstance())
  //   {
  //     G4LogicalVolumeStore *g4lvs = G4LogicalVolumeStore::GetInstance();
  //     unsigned int numberOfVolumes = 0;
  //     for (const auto log_vol : *g4lvs)
  //       {
  //         if(fieldMgr== log_vol->GetFieldManager())
  //           {
  //             ++numberOfVolumes;
  //           }
  //       }
  //     ATH_MSG_INFO("Number of volumes = " << numberOfVolumes << ", GetDeltaIntersection = " << fieldMgr->GetDeltaIntersection() << ", GetDeltaOneStep() = " << fieldMgr->GetDeltaOneStep() << ", GetMinimumEpsilonStep() = " << fieldMgr->GetMinimumEpsilonStep() << ", GetMaximumEpsilonStep() = " << fieldMgr->GetMaximumEpsilonStep() << ", GetDeltaChord() = " << fieldMgr->GetChordFinder()->GetDeltaChord() );
  //     //fieldMgr->GetChordFinder()->PrintStatistics();
  //     G4MagInt_Driver* temp(fieldMgr->GetChordFinder()->GetIntegrationDriver());
  //     ATH_MSG_INFO("GetHmin() = " << temp->GetHmin() << ", GetSafety() = " << temp->GetSafety() << ",  GetPshrnk() = " << temp->GetPshrnk() << ",  GetPgrow() = " << temp->GetPgrow() << ",  GetErrcon() = " << temp->GetErrcon());
  //     const G4MagIntegratorStepper* stepper(temp->GetStepper());
  //     std::string stepperName("Unknown");
  //     if (dynamic_cast<const G4HelixImplicitEuler*>(stepper)!=nullptr) stepperName = "HelixImplicitEuler";
  //     else if (dynamic_cast<const G4HelixSimpleRunge*>(stepper)!=nullptr) stepperName="HelixSimpleRunge";
  //     else if (dynamic_cast<const G4HelixExplicitEuler*>(stepper)!=nullptr) stepperName="HelixExplicitEuler";
  //     else if (dynamic_cast<const G4NystromRK4*>(stepper)!=nullptr) stepperName="NystromRK4";
  //     else if (dynamic_cast<const G4ClassicalRK4*>(stepper)!=nullptr) stepperName="ClassicalRK4";
  //     else if (dynamic_cast<const G4AtlasRK4*>(stepper)!=nullptr) stepperName="AtlasRK4";
  //     ATH_MSG_INFO("Stepper properties: GetNumberOfVariables() = " << stepper->GetNumberOfVariables() << ", GetNumberOfStateVariables() = " << stepper->GetNumberOfStateVariables() << ", IntegratorOrder() = " << stepper->IntegratorOrder() << ", Stepper Type = " << stepperName);
  //   }
  return StatusCode::SUCCESS;
}

G4VUserDetectorConstruction* DetectorGeometrySvc::GetDetectorConstruction()
{
  return m_detConstruction->GetDetectorConstruction();
}

std::vector<std::string>& DetectorGeometrySvc::GetParallelWorldNames()
{
  return m_detConstruction->GetParallelWorldNames();
}

