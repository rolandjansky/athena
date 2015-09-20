/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4PVPlacement.hh"

// Geant4 includes used in functions

DetectorGeometryBase::DetectorGeometryBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),m_subDetTools(this),m_detGeoSvc(0),theParent(0),m_detectorName(name),m_isWorld(0)
{
	ATH_MSG_INFO( "Detector Geometry tool being created: " << name );
	
	// re-initialize m_detectorName in order to take the real detector name rather than the path to it
	size_t ipos=m_detectorName.find_last_of(".");
	size_t length=m_detectorName.size();
	if (ipos<length) m_detectorName=m_detectorName.substr(ipos+1,length-ipos-1);
    ATH_MSG_INFO( "m_detectorName default value reset to "<<m_detectorName);
	
	StatusCode sc=service("DetectorGeometrySvc",m_detGeoSvc);
	if (sc.isFailure()) ATH_MSG_ERROR( "Could not retrieve DetectorGeometrySvc" );

    declareProperty( "SubDetectors" , m_subDetTools , "Tool handle array of all subdetector tools" );
	declareProperty( "DetectorName" , m_detectorName , "Detector name (same as the Tool name if not set");
}

// Athena method, called at initialization time
StatusCode DetectorGeometryBase::initialize()
{
	if (m_detectorName.empty()) m_detectorName=name();
	ATH_MSG_INFO( "Detector Geometry tool being initialized: " << name() <<" "<<m_detectorName ); 

  //  This fires initialize() for each of those tools
    if (m_subDetTools.size())
	{
      ATH_MSG_INFO( "Initializing list of " << m_subDetTools.size() << " detectors in " << name() );
      CHECK( m_subDetTools.retrieve() );
	}
	
  	return StatusCode::SUCCESS;
}
 
void DetectorGeometryBase::Build() 
{
	ATH_MSG_INFO( "Detector Geometry tool: BUILD() being called for " << name() );
	SetEnvelope();
	m_detGeoSvc->SetCurrentDetectorName(m_detectorName);
	BuildGeometry();
	PositionInParent();
	BuildSubDetectors();
} 

void DetectorGeometryBase::BuildGeometry() 
{
	ATH_MSG_INFO( "This is DetectorGeometryBase::BuildGeometry(): anything going wrong?");
}

void DetectorGeometryBase::PositionInParent()
{
	ATH_MSG_INFO( "This is DetectorGeometryBase::PositionInParent()");
	if (m_isWorld)
	{
		// check that the detector is built
		if (envelope.IsBuilt())
		{
			G4VPhysicalVolume* physWorld= new G4PVPlacement(0,G4ThreeVector(),
					envelope.theEnvelope,m_detectorName+"-World",0,false,0,false);
			envelope.thePositionedVolume=physWorld;
		}
	}
	else
	{
		// check that there is a parent
		if (!theParent) ATH_MSG_ERROR("Parent not set!!!!!!!!!! "<<m_detectorName);
		if (theParent)
		{
			if (theParent->GetEnvelope().IsBuilt())
			{
				// still to do: implement a possible rotation/displacement
				
				G4VPhysicalVolume* physVol= new G4PVPlacement(0,G4ThreeVector(),
					envelope.theEnvelope,m_detectorName,theParent->GetEnvelope().theEnvelope,false,0,false);
				envelope.thePositionedVolume=physVol;
			}
		}
	}
}

void DetectorGeometryBase::BuildSubDetectors()
{
    ATH_MSG_INFO("This is DetectorGeometryBase::BuildSubDetectors() "<<m_detectorName);
	for (auto itr: m_subDetTools)
	{
		ATH_MSG_INFO("   positioning "<<itr->GetDetectorName());
		itr->SetParent(this);
		itr->Build();
	}
}
  
void DetectorGeometryBase::SetEnvelope()
{
}
  
void DetectorGeometryBase::ResetEnvelope()
{
}

void DetectorGeometryBase::SetDetectorName(const std::string s)
{
	m_detectorName=s;
}
  
std::string DetectorGeometryBase::GetDetectorName() const
{
	return m_detectorName;
}

void DetectorGeometryBase::SetAsWorld()
{
	m_isWorld=true;
}

void DetectorGeometryBase::SetParent(IDetectorGeometryTool* p)
{
	theParent=p;
}

Envelope& DetectorGeometryBase::GetEnvelope()
{
	return envelope;
}

G4VPhysicalVolume* DetectorGeometryBase::GetWorldVolume()
{
	if (m_isWorld)
		return envelope.thePositionedVolume;
	else
	{
		ATH_MSG_ERROR("trying to get World from a DetectorTool which World is not!");
		return 0;
	}
}

StatusCode
DetectorGeometryBase::queryInterface(const InterfaceID& riid, void** ppvIf) 
{
  if ( riid == IDetectorGeometryTool::interfaceID() ) 
  {
    *ppvIf = (IDetectorGeometryTool*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return AlgTool::queryInterface( riid, ppvIf );
}
