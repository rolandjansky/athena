/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 FinderTolerances: centralised definition of all cuts and tolerances
                   applied from iPatTrackFinder 
 -------------------------------------------------------------------
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/Incident.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "iPatTrackFinder/FinderTolerances.h"
#include "iPatUtility/VertexRegion.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

FinderTolerances::FinderTolerances	(const std::string&	type,
					 const std::string&	name,
					 const IInterface*	parent)
    :   AthAlgTool		(type, name, parent),
	m_incidentSvc		("IncidentSvc", name),
	m_magFieldSvc		("MagField::AtlasFieldSvc/AtlasFieldSvc", name), 
	m_manager		(nullptr),
	m_halfField		(0.),
	m_maxPhiSlope		(0.),
	m_maxDeltaRZ		(0.),
	m_maxPrimaryDeltaEtaPix	(0.),
	m_maxPrimaryDeltaEtaSct	(0.),
	m_maxPrimaryDeltaEtaVtx	(0.),
	m_maxPrimaryDeltaPhiPix	(0.),
	m_maxPrimaryDeltaPhiSct	(0.),
	m_maxPrimaryDeltaPhiVtx	(0.),
	m_maxPrimaryImpact	(0.),
	m_maxSecondaryImpact	(0.),
	m_maxVertexRegionZ	(0.),
	m_maxVertexZ		(0.),
	m_minPt			(0.),
	m_minVertexRegionZ	(0.),
	m_minVertexZ		(0.),
	m_pixelName     	("Pixel"),
	m_printLevel		(0),
	m_vertexZWidth		(0.)
{
    declareInterface<IFinderConfiguration>(this);
    declareProperty("MagFieldSvc",	m_magFieldSvc );
}

FinderTolerances::~FinderTolerances (void)
{}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

StatusCode
FinderTolerances::initialize()
{
    // print where you are
    ATH_MSG_INFO( "FinderTolerances::initialize()" );

    if (m_incidentSvc.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve service " << m_incidentSvc );
	return StatusCode::FAILURE;
    }
    // register to the incident service:
    // 	BeginEvent needed to update field dependent tolerances
    m_incidentSvc->addListener( this, IncidentType::BeginEvent);

    if (! m_magFieldSvc.empty())
    {
	if (m_magFieldSvc.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve service " << m_magFieldSvc );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved service " << m_magFieldSvc );
	}
    }

    // retrieve the pixel GeoModel manager
    if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,m_pixelName))
    {
        ATH_MSG_FATAL( "Could not find the pixel Manager: " << m_pixelName << " !" );
        return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode
FinderTolerances::finalize()
{
    ATH_MSG_DEBUG( "FinderTolerances::finalize()" );

    return StatusCode::SUCCESS; 
}

/** handle for incident service */    
void
FinderTolerances::handle(const Incident& inc)
{
    if (inc.type() == IncidentType::BeginEvent )
    {
	// set the "fieldValue" = conversion factor from pt to radius of curvature at origin
	// this is used to obtain an approx pt for TrackCandidates in TrackFinder.
	// A nominal 0.1Tesla is assumed if the soelenoid field is off
	double fieldValue	= -0.1*Gaudi::Units::tesla*CLHEP::c_light;
	if (m_magFieldSvc->solenoidOn())
	{
	    Amg::Vector3D field;
	    Amg::Vector3D position(0.,0.,0.);
	    m_magFieldSvc->getField(&position,&field);
	    fieldValue		= -field(2)*CLHEP::c_light;
	}
	m_halfField		= 0.5*fieldValue;
	m_maxPhiSlope		= 1.2*std::abs(m_halfField/m_minPt);
	ATH_MSG_DEBUG(" halfField " << m_halfField << "   maxPhiSlope " << m_maxPhiSlope);
    }
}

void
FinderTolerances::setProperties (double			maxSecondaryImpact,
				 double			minPtValue,
				 int			printLevel,
				 const VertexRegion&	vertexRegion)
{
    m_maxSecondaryImpact= maxSecondaryImpact;
    m_minPt		= minPtValue;
    m_printLevel	= printLevel;
    m_maxDeltaRZ	= 10.0;
    m_maxVertexRegionZ	= vertexRegion.centre().z() + vertexRegion.width().z();
    m_minVertexRegionZ	= vertexRegion.centre().z() - vertexRegion.width().z();
    m_vertexZWidth	= 20.0*Gaudi::Units::mm;
    
    // cuts specific to PrimaryCandidate:
    //   settings valid for pt > 1.6 Gev - from vertex offset according to layout
    //   (Initial has increased pixel separation and relaxation as lower expected lumi)
    //   scale below this value to account for increased multiple scattering etc
    double refPt = 1.6*Gaudi::Units::GeV;
    if (m_manager->getLayout() == "Initial")
    {
	m_maxPrimaryDeltaEtaPix	= 1.3*Gaudi::Units::mm;
	m_maxPrimaryDeltaEtaSct	= 2.0*Gaudi::Units::mm;
	m_maxPrimaryDeltaPhiPix	= 0.013;
	m_maxPrimaryDeltaPhiSct	= 0.0015;
    }
    else
    {
	m_maxPrimaryDeltaEtaPix	= 0.7*Gaudi::Units::mm;
	m_maxPrimaryDeltaEtaSct	= 2.0*Gaudi::Units::mm;
	m_maxPrimaryDeltaPhiPix	= 0.006;
	m_maxPrimaryDeltaPhiSct	= 0.0015;
    }
    // tighter cuts for vertex definition
    m_maxPrimaryDeltaEtaVtx	= 1.0*Gaudi::Units::mm;
    m_maxPrimaryDeltaPhiVtx	= 0.0005;

    // scale according to minPt
    if (m_minPt < refPt)
    {
	double scale = refPt/m_minPt;
	m_maxPrimaryDeltaEtaPix	*= scale;
	m_maxPrimaryDeltaEtaSct	*= scale;
	m_maxPrimaryDeltaPhiPix	*= scale;
	m_maxPrimaryDeltaPhiSct	*= scale;
    }
    
    // impact parameter:
    // TODO: this should be configurable, plus pt, layout and luminosity dependent
    m_maxPrimaryImpact	= 1.*Gaudi::Units::mm;
    
    ATH_MSG_INFO( " layout " << m_manager->getLayout()
		  << "   maxPrimaryImpact " << m_maxPrimaryImpact << "mm  "
		  << "   maxSecondaryImpact " << m_maxSecondaryImpact << "mm" );
}

void
FinderTolerances::minPt (double value)
{
    m_minPt		= value;

    // need to include safety factor at this stage
    m_maxPhiSlope	= 1.2*std::abs(m_halfField/m_minPt);
}

void
FinderTolerances::vertexZLimits (double maxVertexZ, double minVertexZ)
{
    m_maxVertexZ	= maxVertexZ;
    m_minVertexZ	= minVertexZ;
}

