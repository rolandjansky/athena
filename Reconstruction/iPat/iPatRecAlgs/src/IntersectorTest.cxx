/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IntersectorTest Algorithm

 useful for optimising the speed and precision of any Intersector flavour
 
 ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iostream>
#include <iomanip>
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "iPatInterfaces/IiPatFitter.h"
#include "iPatRecAlgs/IntersectorTest.h"
#include "iPatTrack/HitList.h"
#include "iPatTrack/Track.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

IntersectorTest::IntersectorTest(const std::string& name, ISvcLocator* pSvcLocator)
    :	AthAlgorithm		(name, pSvcLocator),
	m_intersector		("Trk::RungeKuttaIntersector/RKIntersector"),
	// m_intersector	("Trk::StraightLineIntersector/StraightLineIntersector"),
	m_preciseIntersector	("Trk::RungeKuttaIntersector/PreciseIntersector"),
	m_trackFitter		("TrackFitter/TrackFitter"),
	m_cotThetaMax		(0.),
	m_etaScale		(0.),
	m_fixedPt		(false),
	m_maxOffset		(0.),
	m_maxRotation		(0.),
	m_zLimit		(0.),
	m_backwardsSurface     	(0),
	m_barrelSurface     	(0),
	m_forwardsSurface     	(0),
	m_perigeeSurface     	(0),
	m_momentum		(5000.*Gaudi::Units::GeV),
	m_region		(""),
	m_tracksName		("iPatTracks"),
	m_tracks		(0),
	m_failCount		(0),
	m_offsetCount		(0),
	m_rotationCount		(0),
	m_trackCount		(0)
{
    declareProperty("Intersector",		m_intersector);
    declareProperty("PreciseIntersector",	m_preciseIntersector);
    declareProperty("TrackFitter",		m_trackFitter);
    declareProperty("Momentum",			m_momentum);
    declareProperty("Region",			m_region);
    declareProperty("TracksName",		m_tracksName);
}

//--------------------------------------------------------------------------
IntersectorTest::~IntersectorTest(void)
{}

//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>

StatusCode
IntersectorTest::initialize()
{
    // print where you are
    ATH_MSG_INFO( "IntersectorTest::initialize()" );

    // get the Tools
    if (m_intersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_intersector );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_intersector );
    
    if (m_preciseIntersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_preciseIntersector );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_preciseIntersector );

    if (m_trackFitter.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackFitter );
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "Retrieved tool " << m_trackFitter );

    if (m_tracksName == "")
    {
	ATH_MSG_FATAL( "No name set for iPat Tracks" );
	return StatusCode::FAILURE;
    }

    // create cylinder with end plates according to test region
    // also set momentum-dependent precision requirements
    // 	full Atlas volume
    m_etaScale		= 1.;
    double rlimit	= 12.*Gaudi::Units::meter;
    double zlimit	= 24.*Gaudi::Units::meter;
    // use a different momentum dependence for offset and rotation requirements
    // offset limited by final short step configuration at high momentum
    //                by turning angle/step at low momentum
    double scaleOff	= 1. +  500.*Gaudi::Units::GeV/m_momentum;
    double scaleRot	= 1. +  1000.*Gaudi::Units::GeV/m_momentum;
    m_maxOffset		= scaleOff*1.*Gaudi::Units::nanometer;	//150, .15
    m_maxRotation	= scaleRot*0.001*Gaudi::Units::nanometer;	// i.e. urad
    if (m_momentum < 3.*Gaudi::Units::GeV)	m_fixedPt = true;
    
    if (m_region == "silicon tracker")
    {
	m_fixedPt	= true;
	rlimit		= 0.6*Gaudi::Units::meter;
	zlimit		= 2.8*Gaudi::Units::meter;
	m_maxOffset	= scaleOff*10.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.02*Gaudi::Units::nanometer; // urad
    }
    ////////   review Z0 - increase if poss. Ditto Z1. Then check if R1 can increase
    else if (m_region == "silicon barrel")  // AKA indetR0/1,Z1 etc
    {
	// m_etaScale	= 0.5;
	m_fixedPt	= true;
	rlimit		= 1.1*Gaudi::Units::meter;	//1.1*Gaudi::Units::meter;
	zlimit		= 3.0*Gaudi::Units::meter;	//3.0*Gaudi::Units::meter;
	m_maxOffset	= scaleOff*10.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.02*Gaudi::Units::nanometer; // urad
    }
    else if (m_region == "indet")
    {
	rlimit		= 1.1*Gaudi::Units::meter;
	zlimit		= 3.0*Gaudi::Units::meter;
	// floor is at 1 or 2 TeV for offset !!!!!!!!!
	m_maxOffset	= scaleOff*16.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.04*Gaudi::Units::nanometer; // urad
    }
    else if (m_region == "indet barrel")
    {
	m_etaScale	= 0.5;
	m_fixedPt	= true;
	rlimit		= 1.1*Gaudi::Units::meter;
	zlimit		= 1.1*Gaudi::Units::meter;
	m_maxOffset	= scaleOff*80.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.05*Gaudi::Units::nanometer; // urad
    }
    else if (m_region == "solenoid barrel")
    {
	m_etaScale	= 0.5;
	m_fixedPt	= true;
	rlimit		= 1.3*Gaudi::Units::meter;
	zlimit		= 1.3*Gaudi::Units::meter;
	m_maxOffset	= scaleOff*200.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.10*Gaudi::Units::nanometer; // urad
    }
    else if (m_region == "solenoid + calo endcap")
    {
	rlimit		= 1.3*Gaudi::Units::meter;
	zlimit		= 8.0*Gaudi::Units::meter;
    }
    else if (m_region == "calo")
    {
	rlimit		= 3.8*Gaudi::Units::meter;
	zlimit		= 6.0*Gaudi::Units::meter;
    }
    else if (m_region == "muon entry barrel")
    {
	m_etaScale	= 0.5;
	m_fixedPt	= true;
	rlimit		= 4.2*Gaudi::Units::meter;
	zlimit		= 6.8*Gaudi::Units::meter;
	m_maxOffset	= scaleOff*1000.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.10*Gaudi::Units::nanometer; // urad
    }
    else if (m_region == "muon barrel")
    {
	m_etaScale	= 0.5;
	m_fixedPt	= true;
	rlimit		= 12.0*Gaudi::Units::meter;
	zlimit		= 9.0*Gaudi::Units::meter;
	m_maxOffset	= scaleOff*4000.*Gaudi::Units::nanometer;
	m_maxRotation	= scaleRot*0.25*Gaudi::Units::nanometer; // urad
    }

    m_cotThetaMax	= zlimit/rlimit;
    // CLHEP::HepRotation rotation;
    // HepGeom::Transform3D* transformB	=
    // 	new HepGeom::Transform3D(rotation,CLHEP::Hep3Vector(0.,0.,-zlimit));
    // m_backwardsSurface			= new Trk::DiscSurface(transformB, 0., rlimit);
    // HepGeom::Transform3D* transformF	=
    // 	new HepGeom::Transform3D(rotation,CLHEP::Hep3Vector(0.,0.,zlimit));
    // m_barrelSurface			= new Trk::CylinderSurface(transformF, rlimit, zlimit);
    Amg::Transform3D* transformB	= new Amg::Transform3D;
    (*transformB)			= Amg::Translation3D(0.,0.,-zlimit);
     m_backwardsSurface			= new Trk::DiscSurface(transformB, 0., rlimit);
    m_barrelSurface			= new Trk::CylinderSurface(rlimit, zlimit);
    Amg::Transform3D* transformF	= new Amg::Transform3D;
    (*transformF)			= Amg::Translation3D(0.,0.,+zlimit);
    m_forwardsSurface			= new Trk::DiscSurface(transformF, 0., rlimit);
    m_perigeeSurface			= new Trk::PerigeeSurface(Amg::Vector3D(0.,0.,0.));
    m_zLimit				= zlimit;

    ATH_MSG_INFO( " test intersections for " << std::setiosflags(std::ios::fixed|std::ios::right)
		  << std::setw(7)	<< std::setprecision(1) << m_momentum/Gaudi::Units::GeV
		  << "GeV requiring "	<< std::setw(7)	<< std::setprecision(1) 
		  << m_maxOffset/Gaudi::Units::nanometer	<< "nm and "
		  << std::setw(5)	<< std::setprecision(2)
		  << m_maxRotation/Gaudi::Units::nanometer	<< "urad precision."
		  << " Using a bounded cylinder with radius "
		  << std::setw(5)	<< std::setprecision(2) << rlimit/Gaudi::Units::meter << "m and "
		  << std::setw(5)	<< std::setprecision(2) << zlimit/Gaudi::Units::meter
		  << "m halflength");
    
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------
    
StatusCode
IntersectorTest::execute()
{
    // ATH_MSG_INFO( "entered execution" );

    // Get hold of Tracks from TDS
    if (StatusCode::SUCCESS != evtStore()->retrieve(m_tracks, m_tracksName))
    {
	ATH_MSG_WARNING( "IntersectorTest::Could not retrieve TrackContainer" );
	return StatusCode::SUCCESS;
    }

    for (std::vector<Track*>::const_iterator t = m_tracks->begin();
	 t != m_tracks->end();
	 ++t)
    {
	Amg::Vector3D position0  ((**t).perigee_parameters().position());

	// offset +-10cm along beam axis
	Amg::Vector3D position1	=  position0;
	position1[2]		+= 100.*Gaudi::Units::mm;
	Amg::Vector3D position2	=  position0;
	position2[2]		-= 100.*Gaudi::Units::mm;

	double qOverP	= 1./m_momentum;
	if ((**t).perigee_parameters().qOverP() < 0.)  qOverP = -qOverP;

	double sinTheta	= (**t).perigee_parameters().direction().perp();
	double cosPhi	= (**t).perigee_parameters().direction().x()/sinTheta;
	double sinPhi	= (**t).perigee_parameters().direction().y()/sinTheta;
	double eta	= m_etaScale*(**t).perigee_parameters().direction().eta();
	double theta	= 2.*std::atan(std::exp(-eta));
	double cosTheta	= std::cos(theta);
	sinTheta	= std::sqrt(1. - cosTheta*cosTheta);
	Amg::Vector3D direction0 (cosPhi*sinTheta, sinPhi*sinTheta, cosTheta);
	
	// opposite direction
	Amg::Vector3D direction1 (-cosPhi*sinTheta, -sinPhi*sinTheta, -cosTheta);

	// eta +/- 0.5 to cover up to eta=3.0
	theta		= 2.*std::atan(std::exp(-eta + m_etaScale*0.5));
	cosTheta	= std::cos(theta);
	sinTheta	= std::sqrt(1. - cosTheta*cosTheta);
	Amg::Vector3D direction2 (cosPhi*sinTheta, sinPhi*sinTheta, cosTheta);
	theta		= 2.*std::atan(std::exp(-eta - m_etaScale*0.5));
	cosTheta	= std::cos(theta);
	sinTheta	= std::sqrt(1. - cosTheta*cosTheta);
	Amg::Vector3D direction3 (cosPhi*sinTheta, sinPhi*sinTheta, cosTheta);

	const Trk::TrackSurfaceIntersection* intersection0 =
	    new Trk::TrackSurfaceIntersection(position0, direction0, 0.);
	compareWithPrecise (intersection0, qOverP);

	const Trk::TrackSurfaceIntersection* intersection1 =
	    new Trk::TrackSurfaceIntersection(position1, direction0, 0.);
	compareWithPrecise (intersection1, qOverP);

	const Trk::TrackSurfaceIntersection* intersection2 =
	    new Trk::TrackSurfaceIntersection(position2, direction0, 0.);
	compareWithPrecise (intersection2, qOverP);
	
	const Trk::TrackSurfaceIntersection* intersection3 =
	    new Trk::TrackSurfaceIntersection(position0, direction1, 0.);
	compareWithPrecise (intersection3, qOverP);

	const Trk::TrackSurfaceIntersection* intersection4 =
	    new Trk::TrackSurfaceIntersection(position0, direction2, 0.);
	compareWithPrecise (intersection4, qOverP);

	const Trk::TrackSurfaceIntersection* intersection5 =
	    new Trk::TrackSurfaceIntersection(position0, direction3, 0.);
	compareWithPrecise (intersection5, qOverP);

    }

    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------

StatusCode
IntersectorTest::finalize()
{
    delete m_backwardsSurface;
    delete m_barrelSurface;
    delete m_forwardsSurface;
    delete m_perigeeSurface;
    std::string		fixed	= " with pt fixed.   ";
    if (! m_fixedPt)	fixed	= " fixed momentum.  ";
    ATH_MSG_INFO( m_region << fixed << m_trackCount << " intersections tested with " << m_failCount
		  << " failures. For "	<< std::setiosflags(std::ios::fixed|std::ios::right)
		  << std::setw(7)	<< std::setprecision(1) << m_momentum/Gaudi::Units::GeV
		  << "GeV require "	<< std::setw(7)	<< std::setprecision(1) 
		  << m_maxOffset/Gaudi::Units::nanometer	<< "nm and "
		  << std::setw(5)	<< std::setprecision(2)
		  << m_maxRotation/Gaudi::Units::nanometer	<< "urad precision giving "
		  << std::setw(4)	<< m_offsetCount	<< " imprecise offset and "
		  << std::setw(4)	<< m_rotationCount	<< " imprecise rotation");
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------------

void
IntersectorTest::compareWithPrecise(const Trk::TrackSurfaceIntersection*	intersection,
				    double					qOverP)
{
    // just in case
    if (! intersection)
    {
	++m_failCount;
	ATH_MSG_WARNING(" compareWithPrecise: invalid argument" );
	return;
    }
    
    // put on perigee surface
     const Trk::TrackSurfaceIntersection* originalIntersection =
	 m_intersector->intersectSurface(*m_perigeeSurface,
					 intersection,
					 qOverP);
     if (! originalIntersection)
    {
	++m_failCount;
	ATH_MSG_WARNING(" compareWithPrecise: fails to intersect perigee" );
	return;
    }
    
    // for debug
    double eta		= intersection->direction().eta();
    double theta	= intersection->direction().theta();
    double phi		= intersection->direction().phi();

    // fixed pt option for solenoid
    if (m_fixedPt)	qOverP *= std::sin(theta);
    
    // precise and standard extrapolation
    // const Trk::TrackSurfaceIntersection* originalIntersection =
    // 	new Trk::TrackSurfaceIntersection(*intersection);
    const Trk::TrackSurfaceIntersection* preciseIntersection =
	new Trk::TrackSurfaceIntersection(*intersection);
    double cotTheta	= intersection->direction().z()/intersection->direction().perp();
    if (cotTheta < -m_cotThetaMax)
    {
	preciseIntersection	= m_preciseIntersector->intersectSurface(*m_backwardsSurface,
									 preciseIntersection,
									 qOverP);
	intersection		= m_intersector->intersectSurface(*m_backwardsSurface,
								  intersection,
								  qOverP);
	// reset cache
	const Trk::TrackSurfaceIntersection* temp =
	    m_intersector->intersectSurface(*m_backwardsSurface,
					    intersection,
					    0.);
	delete temp;
    }
    else if (cotTheta > m_cotThetaMax)
    {
	preciseIntersection	= m_preciseIntersector->intersectSurface(*m_forwardsSurface,
									 preciseIntersection,
									 qOverP);
	intersection		= m_intersector->intersectSurface(*m_forwardsSurface,
								  intersection,
								  qOverP);
	// reset cache
	const Trk::TrackSurfaceIntersection* temp =
	    m_intersector->intersectSurface(*m_forwardsSurface,
					    intersection,
					    0.);
	delete temp;
    }
    else
    {
	preciseIntersection	= m_preciseIntersector->intersectSurface(*m_barrelSurface,
									 preciseIntersection,
									 qOverP);
	
	if (preciseIntersection && m_zLimit > std::abs(preciseIntersection->position().z()))
	{
	    intersection	= m_intersector->intersectSurface(*m_barrelSurface,
								  intersection,
								  qOverP);
	    // reset cache
	    const Trk::TrackSurfaceIntersection* temp =
		m_intersector->intersectSurface(*m_barrelSurface,
						intersection,
						0.);
	    delete temp;
	}
	else if (cotTheta < 0.)
	{
	    delete preciseIntersection;
	    preciseIntersection	=
		m_preciseIntersector->intersectSurface(*m_backwardsSurface,
						       new Trk::TrackSurfaceIntersection(*intersection),
						       qOverP);
	    intersection	= m_intersector->intersectSurface(*m_backwardsSurface,
								  intersection,
								  qOverP);
	    // reset cache
	    const Trk::TrackSurfaceIntersection* temp =
		m_intersector->intersectSurface(*m_backwardsSurface,
						intersection,
						0.);
	    delete temp;
	}
	else
	{
	    delete preciseIntersection;
	    preciseIntersection	=
		m_preciseIntersector->intersectSurface(*m_forwardsSurface,
						       new Trk::TrackSurfaceIntersection(*intersection),
						       qOverP);
	    intersection	= m_intersector->intersectSurface(*m_forwardsSurface,
								  intersection,
								  qOverP);
	    // reset cache
	    const Trk::TrackSurfaceIntersection* temp =
		m_intersector->intersectSurface(*m_forwardsSurface,
						intersection,
						0.);
	    delete temp;
	} 
    }

    // track back again
    const Trk::TrackSurfaceIntersection* backIntersection =
     	( (intersection) ? new Trk::TrackSurfaceIntersection(*intersection) : nullptr );

    // accumulate statistics
    ++m_trackCount;
    double pathLength	= 0.;
    if (intersection && preciseIntersection)
    {
	double offset	= (intersection->direction().cross
			   (intersection->position() - preciseIntersection->position())).mag();
	double rotation	= (intersection->direction().cross(preciseIntersection->direction())).mag();
	double r	= preciseIntersection->position().perp();
	double phiP	= preciseIntersection->position().phi();
	double z	= preciseIntersection->position().z();

	// TODO compare pathLengths
	pathLength	= intersection->pathlength();
	double delta	= preciseIntersection->pathlength() - pathLength;
	if (std::abs(delta) > m_maxOffset)
	    ATH_MSG_INFO( " pathlength problem: eta " << std::setiosflags(std::ios::fixed|std::ios::right)
			  << std::setw(6)  << std::setprecision(3) << eta
			  << "   r,phi,z "
			  << std::setw(8) << std::setprecision(1) << r	<< ", "
			  << std::setw(7)  << std::setprecision(4) << phiP	<< ", "
			  << std::setw(7) << std::setprecision(1) << z
			  << "   phi, sin(theta) "
			  << std::setw(7)  << std::setprecision(4) << phi	<< ", "
			  << std::setw(6)  << std::setprecision(4) << std::sin(theta)
			  << "   path "<< std::setw(9)  << std::setprecision(6) << pathLength
			  << " delta "<< std::setw(9)  << std::setprecision(6) << delta);
	
	delete intersection;
	delete preciseIntersection;
	if (offset > m_maxOffset || rotation > m_maxRotation)
	{
	    if (offset > m_maxOffset)
	    {
		++m_offsetCount;
	    }
	    else
	    {
		++m_rotationCount;
	    }
	    
	    ATH_MSG_INFO( " NOT precise: eta " << std::setiosflags(std::ios::fixed|std::ios::right)
			  << std::setw(6)  << std::setprecision(3) << eta
			  << "   r,phi,z "
			  << std::setw(8) << std::setprecision(1) << r	<< ", "
			  << std::setw(7)  << std::setprecision(4) << phiP	<< ", "
			  << std::setw(7) << std::setprecision(1) << z
			  << "   phi, sin(theta) "
			  << std::setw(7)  << std::setprecision(4) << phi	<< ", "
			  << std::setw(6)  << std::setprecision(4) << std::sin(theta)
			  << "   offset "<< std::setw(9)  << std::setprecision(6) << offset
			  << "  rotation "<< std::setw(9)  << std::setprecision(6) << rotation);
	}
    }
    else
    {
	++m_failCount;
	if (intersection)		ATH_MSG_WARNING(" missing precise solution" );
	if (preciseIntersection)	ATH_MSG_WARNING(" missing RungeKutta solution" );
	delete intersection;
	delete preciseIntersection;
    }

    // check tracking back
    backIntersection	= (backIntersection ? m_intersector->intersectSurface(*m_perigeeSurface,backIntersection, qOverP) : nullptr);

    if (backIntersection)
    {
	double offset	=
	    (originalIntersection->direction().cross(
		originalIntersection->position() - backIntersection->position())).mag();
	double rotation	=
	    (originalIntersection->direction().cross(backIntersection->direction())).mag();
	delete originalIntersection;
	delete backIntersection;
	if (offset > m_maxOffset + pathLength*m_maxRotation || rotation > 2.*m_maxRotation)
	{
	    if (offset > m_maxOffset + pathLength*m_maxRotation)
	    {
		++m_offsetCount;
	    }
	    else
	    {
		++m_rotationCount;
	    }
	    
	    ATH_MSG_INFO( " NOT precise (backtrack): eta " << std::setiosflags(std::ios::fixed|std::ios::right)
			  << std::setw(6)  << std::setprecision(3) << eta
			  << "   phi, sin(theta) "
			  << std::setw(7)  << std::setprecision(4) << phi	<< ", "
			  << std::setw(6)  << std::setprecision(4) << std::sin(theta)
			  << "   offset "<< std::setw(9)  << std::setprecision(6) << offset
			  << "  rotation "<< std::setw(9)  << std::setprecision(6) << rotation);
	}
    }
    else
    {
	++m_failCount;
	ATH_MSG_WARNING(" missing backTrack solution" );
	delete originalIntersection;
    }
    

    // // track perigee parameters
    // std::cout << " count " << m_trackCount;
    // (**t).perigee_parameters().print();
    // std::cout << std::endl;

}

   
