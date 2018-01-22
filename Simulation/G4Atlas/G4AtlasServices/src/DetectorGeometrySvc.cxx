/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "DetectorGeometrySvc.h"

#include "G4VUserDetectorConstruction.hh"

// For additional materials that are required
#include "G4NistManager.hh"
#include "G4Material.hh"

// // Includes for field configuration debugging
// #include "G4FieldManagerStore.hh"
// #include "G4ChordFinder.hh"
// #include "G4LogicalVolumeStore.hh"
// #include "G4MagIntegratorDriver.hh"
// #include "G4AtlasRK4.hh"
// #include "G4HelixImplicitEuler.hh"
// #include "G4HelixSimpleRunge.hh"
// #include "G4HelixExplicitEuler.hh"
// #include "G4NystromRK4.hh"
// #include "G4ClassicalRK4.hh"

DetectorGeometrySvc::DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class(name,pSvcLocator),
    m_detTool("",this),
    m_detConstruction("",this),
    m_regionCreators(this),
    m_parallelWorlds(this),
    m_configurationTools(this),
    m_fieldManagers(this),
    m_activateParallelWorlds(false)
{
  declareProperty( "World" , m_detTool , "Tool handle of the top-of-the-tree detector geometry tool" );
  declareProperty( "DetectorConstruction" , m_detConstruction , "Tool handle of the DetectorConstruction" );
  declareProperty( "RegionCreators" , m_regionCreators , "Tools to define G4 physics regions" );
  declareProperty( "ParallelWorlds" , m_parallelWorlds , "Tools to define G4 parallel worlds" );
  declareProperty( "ActivateParallelWorlds",m_activateParallelWorlds,"Toggle on/off the G4 parallel geometry system");
  declareProperty( "FieldManagers",m_fieldManagers,"field managers used");
  declareProperty( "GeometryConfigurationTools",m_configurationTools,"Tools for geometry configuration");
  ATH_MSG_DEBUG( "DetectorGeometrySvc being created!" );
}

DetectorGeometrySvc::~DetectorGeometrySvc()
{
}

StatusCode DetectorGeometrySvc::initialize(){

  // go through all tools and retrieve them
  //  This fires initialize() for each of those tools
  ATH_MSG_DEBUG( "Initializing Geometry configuration tools "  );
  for (auto it: m_configurationTools)
  {
    ATH_CHECK( it.retrieve() );
    ATH_CHECK( it->preGeometryConfigure() );
  }

  ATH_MSG_DEBUG( "Setting up a DetectorConstruction " << m_detConstruction.name() );
  ATH_CHECK(m_detConstruction.retrieve());

  ATH_MSG_DEBUG( "Initializing World detectors in " << name() );
  ATH_CHECK( m_detTool.retrieve() );

  ATH_MSG_DEBUG( "Detectors " << m_detTool.name() <<" being set as World" );
  m_detTool->SetAsWorld();
  m_detTool->Build();

  ATH_MSG_DEBUG( "Setting World in the DetectorConstruction " );
  m_detConstruction->SetWorld(m_detTool->GetWorldVolume());

  ATH_MSG_DEBUG( "Setting up G4 physics regions" );
  for (auto& it: m_regionCreators)
    {
      ATH_CHECK( it.retrieve() );
    }

  if (m_activateParallelWorlds)
    {
      ATH_MSG_DEBUG( "Setting up G4 parallel worlds" );
      for (auto& it: m_parallelWorlds)
        {
          ATH_CHECK( it.retrieve() );
          m_parallelWorldNames.push_back(it.name());
          m_detConstruction->RegisterParallelWorld(it->GetParallelWorld());
        }
    }

  ATH_MSG_DEBUG( "Running geometry post-configuration tools" );
  for (auto it: m_configurationTools)
    ATH_CHECK( it->postGeometryConfigure() );

  ATH_MSG_DEBUG( "Setting up field managers" );
  ATH_CHECK( m_fieldManagers.retrieve() );
  ATH_CHECK( initializeFields() );

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

//-----------------------------------------------------------------------------
// Setup the magnetic field managers for configured volumes
//-----------------------------------------------------------------------------
StatusCode DetectorGeometrySvc::initializeFields()
{
  for (auto& fm : m_fieldManagers) {
    ATH_CHECK( fm->initializeField() );
  }
  return StatusCode::SUCCESS;
}

G4VUserDetectorConstruction* DetectorGeometrySvc::GetDetectorConstruction()
{
  return m_detConstruction->GetDetectorConstruction();
}

std::vector<std::string>& DetectorGeometrySvc::GetParallelWorldNames()
{
  return m_parallelWorldNames;
}

