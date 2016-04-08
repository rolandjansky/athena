/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DetectorGeometrySvc.h"

#include "G4VUserDetectorConstruction.hh"

// For additional materials that are required
#include "G4NistManager.hh"
#include "G4Material.hh"

DetectorGeometrySvc::DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),
    m_detTool(""),
    m_detConstruction(""),
    m_regionCreators(),
    m_parallelWorlds(),
    m_activateParallelWorlds(false)
{
  declareProperty( "World" , m_detTool , "Tool handle of the top-of-the-tree detector geometry tool" );
  declareProperty( "DetectorConstruction" , m_detConstruction , "Tool handle of the DetectorConstruction" );
  declareProperty( "RegionCreators" , m_regionCreators , "Tools to define G4 physics regions" );
  declareProperty( "ParallelWorlds" , m_parallelWorlds , "Tools to define G4 parallel worlds" );
  declareProperty( "ActivateParallelWorlds",m_activateParallelWorlds,"Toggle on/off the G4 parallel geometry system");
  ATH_MSG_DEBUG( "DetectorGeometrySvc being created!" );
}

DetectorGeometrySvc::~DetectorGeometrySvc()
{
}

StatusCode DetectorGeometrySvc::initialize(){
  // First build additional volumes that might be required for geometry
  //  Some of the tools will take action on initialize(), so the materials
  //  should be available prior to their retrieval
  BuildExtraMaterials();

  // go through all tools and retrieve them
  //  This fires initialize() for each of those tools

  ATH_MSG_DEBUG( "Setting up a DetectorConstruction " << m_detConstruction.name() );
  CHECK(m_detConstruction.retrieve());

  //CHECK(m_notifierSvc.retrieve()); // TESTING possibly not required.

  ATH_MSG_DEBUG( "Initializing World detectors in " << name() );
  CHECK( m_detTool.retrieve() );

  ATH_MSG_DEBUG( "Detectors " << m_detTool.name() <<" being set as World" );
  m_detTool->SetAsWorld();
  m_detTool->Build();

  ATH_MSG_DEBUG( "Setting World in the DetectorConstruction " );
  m_detConstruction->SetWorld(m_detTool->GetWorldVolume());

  ATH_MSG_DEBUG( "Setting up G4 physics regions" );
  for (auto it: m_regionCreators)
    CHECK( it.retrieve() );

  if (m_activateParallelWorlds)
    {
      ATH_MSG_DEBUG( "Setting up G4 parallel worlds" );
      for (auto it: m_parallelWorlds)
        {
          CHECK( it.retrieve() );
          m_parallelWorldNames.push_back(it.name());
          m_detConstruction->RegisterParallelWorld(it->GetParallelWorld());
        }
    }

  ATH_MSG_DEBUG( "DetectorGeometrySvc initialized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode DetectorGeometrySvc::finalize(){
  ATH_MSG_DEBUG( "DetectorGeometrySvc being ifinalized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode DetectorGeometrySvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IDetectorGeometrySvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IDetectorGeometrySvc*>(this);
  } else {
    // Interface is not directly available : try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
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

void DetectorGeometrySvc::BuildExtraMaterials()
{
  G4NistManager* man = G4NistManager::Instance();
  G4Element* N   = man->FindOrBuildElement("N");
  G4Element* O   = man->FindOrBuildElement("O");
  G4Element* Ar  = man->FindOrBuildElement("Ar");
  G4Element* H   = man->FindOrBuildElement("H");
  G4Element* Si  = man->FindOrBuildElement("Si");
  G4Element* Ca  = man->FindOrBuildElement("Ca");
  G4Element* Al  = man->FindOrBuildElement("Al");
  G4Element* Fe  = man->FindOrBuildElement("Fe");
  G4Element* C   = man->FindOrBuildElement("C");

  // Constructor of G4Material inserts it into a table for later use
  G4Material *air = new G4Material("Air", 0.001214*CLHEP::g/CLHEP::cm3,4);
  air->AddElement( N, 0.7494);
  air->AddElement( O, 0.2369);
  air->AddElement(Ar, 0.0129);
  air->AddElement( H, 0.0008);

  G4Material *vacuum = new G4Material("Vacuum", 0.000001*CLHEP::g/CLHEP::cm3,1);
  vacuum->AddMaterial(air, 1.0);

  G4Material *material_concrete = new G4Material("Concrete",2.3*CLHEP::g/CLHEP::cm3,6);
  material_concrete->AddElement(Si,0.227915);
  material_concrete->AddElement(O,0.605410);
  material_concrete->AddElement(H,0.099720);
  material_concrete->AddElement(Ca,0.049860);
  material_concrete->AddElement(Al,0.014245);
  material_concrete->AddElement(Fe,0.002850);

  G4Material *material_scintillator = new G4Material("Scintillator",1.032*CLHEP::g/CLHEP::cm3,2);
  material_scintillator->AddElement(C,0.5);
  material_scintillator->AddElement(H,0.5);

  G4Material *material_mylar = new G4Material("Mylar",1.39*CLHEP::g/CLHEP::cm3,3);
  material_mylar->AddElement(H,4./11.);
  material_mylar->AddElement(C,5./11.);
  material_mylar->AddElement(O,2./11.);

  G4Material *material_rock = new G4Material("Rock",2.33*CLHEP::g/CLHEP::cm3,1);
  material_rock->AddElement(Si,1);
}
