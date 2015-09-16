/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DetectorGeometrySvc.h"

#include "G4VUserDetectorConstruction.hh"

#include "PVNotifier.h"
#include "LVNotifier.h"

DetectorGeometrySvc::DetectorGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator )
  : AthService(name,pSvcLocator),m_detTool(this),m_detConstruction(this),m_regionCreators(this),m_parallelWorlds(this),
    lvNotifier(0),pvNotifier(0)
{
    	declareProperty( "World" , m_detTool , "Tool handle of the top-of-the-tree detector geometry tool" );
	declareProperty( "DetectorConstruction" , m_detConstruction , "Tool handle of the DetectorConstruction" );
	declareProperty( "RegionCreators" , m_regionCreators , "Tools to define G4 physics regions" );
	declareProperty( "ParallelWorlds" , m_parallelWorlds , "Tools to define G4 parallel worlds" );
	declareProperty( "ActivateLVNotifier",m_activateLVNotifier,"Toggle on/off the G4 LV notifier");
	declareProperty( "ActivatePVNotifier",m_activatePVNotifier,"Toggle on/off the G4 PV notifier");
	declareProperty( "ActivateParallelWorlds",m_activateParallelWorlds,"Toggle on/off the G4 parallel geometry system");
	ATH_MSG_INFO( "DetectorGeometrySvc being created!" );
}

DetectorGeometrySvc::~DetectorGeometrySvc()
{
	if (lvNotifier) delete lvNotifier;
	if (pvNotifier) delete pvNotifier;
}

StatusCode DetectorGeometrySvc::initialize(){
  // go through all tools and retrieve them
  //  This fires initialize() for each of those tools
  
  ATH_MSG_INFO( "Setting up a DetectorConstruction " << name() );
  CHECK(m_detConstruction.retrieve());
  
  if (m_activateLVNotifier || m_activatePVNotifier)
  {
  	 ATH_MSG_INFO( "Notifiers being instantiated " );
	 if (m_activateLVNotifier) lvNotifier=new LVNotifier(this);
	 if (m_activatePVNotifier) pvNotifier=new PVNotifier(this);
  }
  
  ATH_MSG_INFO( "Initializing World  detectors in " << name() );
  CHECK( m_detTool.retrieve() );
  
  ATH_MSG_INFO( "Detectors " << m_detTool.name() <<" being set as World" );
  m_detTool->SetAsWorld();
  m_detTool->Build();
  
  ATH_MSG_INFO( "Setting World in the DetectorConstruction " );  
  m_detConstruction->SetWorld(m_detTool->GetWorldVolume()); 
  
  ATH_MSG_INFO( "Setting up G4 physics regions" );
  for (auto it: m_regionCreators)
  	CHECK( it.retrieve() );
	
  if (m_activateParallelWorlds)
  {
  	ATH_MSG_INFO( "Setting up G4 parallel worlds" );
	for (auto it: m_parallelWorlds)
	{
		CHECK( it.retrieve() );
		m_parallelWorldNames.push_back(it.name());
		m_detConstruction->RegisterParallelWorld(it->GetParallelWorld());
	}
  }	
  
  ATH_MSG_INFO( "DetectorGeometrySvc initialized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode DetectorGeometrySvc::finalize(){
  ATH_MSG_INFO( "DetectorGeometrySvc being ifinalized!!!" );
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

