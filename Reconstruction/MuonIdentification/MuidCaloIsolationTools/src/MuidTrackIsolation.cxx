/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidTrackIsolation
//  AlgTool for estimating the number, total charged momentum and most
//  energetic inner detector tracks in a cone surrounding a muon
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include "GaudiKernel/SystemOfUnits.h"
#include "MuidCaloIsolationTools/MuidTrackIsolation.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

namespace Rec
{

MuidTrackIsolation::MuidTrackIsolation (const std::string&type,
					const std::string&name, 
					const IInterface*parent)
    :	AthAlgTool		(type, name, parent),
	m_etaSafetyFactor	(0.1),
	m_inDetTracksLocation	("Tracks"),
	m_intersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_minPt			(1.0*Gaudi::Units::GeV),
	m_trackCone		(0.2),
	m_trackExtrapolation	(false)
{
    declareInterface<IMuidTrackIsolation>(this);
    declareProperty("MinPt",			m_minPt);
    declareProperty("RungeKuttaIntersector",	m_intersector);
    declareProperty("InDetTracksLocation",     	m_inDetTracksLocation);
    declareProperty("TrackCone",		m_trackCone);
    declareProperty("TrackExtrapolation",	m_trackExtrapolation);
}

MuidTrackIsolation::~MuidTrackIsolation (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
MuidTrackIsolation::initialize()
{
    ATH_MSG_INFO( "MuidTrackIsolation::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_intersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_intersector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_intersector );
    }

    // create the calo barrel surfaces (cylinder) and 2 endcap discs)
    double radius	= 2.0*Gaudi::Units::meter;
    double halfLength	= 4.0*Gaudi::Units::meter;
    m_caloCylinder	= new Trk::CylinderSurface(new Amg::Transform3D, radius, halfLength);

    // the corresponding max barrel cotTheta
    m_barrelCotTheta	= halfLength/radius;

    // and the forward/backward endcap disks
    Amg::Transform3D discRotation;
    Amg::Transform3D* transform;
    Amg::Vector3D forwardDiscPosition(0.,0.,halfLength);
    transform		= new Amg::Transform3D(discRotation);
    *transform          *= Amg::Translation3D(forwardDiscPosition);
    m_caloForwardDisc	= new Trk::DiscSurface(transform, 0., radius);
    Amg::Vector3D backwardDiscPosition(0.,0.,-halfLength);
    transform		= new Amg::Transform3D(discRotation);
    *transform          *= Amg::Translation3D(backwardDiscPosition);
    m_caloBackwardDisc	= new Trk::DiscSurface(transform, 0., radius);

    return StatusCode::SUCCESS;
}

StatusCode
MuidTrackIsolation::finalize()
{
    ATH_MSG_INFO( "Finalizing MuidTrackIsolation" );

    delete m_caloBackwardDisc;
    delete m_caloCylinder;
    delete m_caloForwardDisc;

    return StatusCode::SUCCESS;
}

std::pair<int,double>
MuidTrackIsolation::trackIsolation(double eta, double phi) const
{
    // debug input quantities
    if (msgLvl(MSG::DEBUG))
    {
	msg(MSG::DEBUG) << " MuidTrackIsolation:: " << std::setiosflags(std::ios::fixed);
	if (m_trackExtrapolation)
	{
	    msg() << "applied after extrapolation to calo, ";
	}
	else
	{
	    msg() << "applied at perigee, ";
	}
	msg() << " for muon at calo with eta,phi "
	      << std::setw(8) << std::setprecision(3) << eta
	      << std::setw(8) << std::setprecision(3) << phi;
	if (msgLvl(MSG::VERBOSE)) msg() << endmsg << MSG::VERBOSE;
    }

    // set initial state
    m_maxP = 0.;
    std::pair<int,double> isolation = std::make_pair(0,0.);

    // retrieve track collection
    const TrackCollection* inDetTracks = 0;
    if (! evtStore()->contains<TrackCollection>(m_inDetTracksLocation))
    {
	ATH_MSG_DEBUG( " no ID Track container at location  " << m_inDetTracksLocation );
	return isolation;
    }

    if (StatusCode::SUCCESS != evtStore()->retrieve(inDetTracks,m_inDetTracksLocation))
    {
	ATH_MSG_WARNING( " Could not retrieve ID Track container from "	<< m_inDetTracksLocation );
	return isolation;
    }

    // evaluate isolation according to configuration
    if (m_trackExtrapolation)
    {
	isolation = trackExtrapolated(inDetTracks,eta,phi);
    }
    else
    {
	isolation = trackVertex(inDetTracks,eta,phi);
    }

    // debug result
    if (msgLvl(MSG::DEBUG))
    {
	msg() << std::endl << " Found "<< isolation.first
	      << std::setiosflags(std::ios::fixed)
	      << " InDet tracks with total momentum "
	      << std::setw(8) << std::setprecision(1) << isolation.second/Gaudi::Units::GeV <<" Gaudi::Units::GeV "
	      << "and maximum momentum "
	      << std::setw(8) << std::setprecision(1) << m_maxP/Gaudi::Units::GeV
	      << endmsg;
    }
    
    return isolation;
}

std::pair<int,double>
MuidTrackIsolation::trackVertex(const TrackCollection* inDetTracks, double eta, double phi) const
{
    // set initial state
    m_maxP		= 0.;
    double sumP		= 0.;
    int numberTracks	= 0;

    // choose tracks in cone
    for (TrackCollection::const_iterator id = inDetTracks->begin() ;
         id != inDetTracks->end();
         ++id)
    { 
	const Trk::Perigee& perigee = *(**id).perigeeParameters();
	if ((**id).info().trackProperties(Trk::TrackInfo::StraightTrack)
	    || perigee.pT() < m_minPt)						continue;
	
	double inDetPhi	= perigee.parameters()[Trk::phi];
	double inDetEta	= perigee.eta();

	double diffEta = fabs(eta-inDetEta);
	double diffPhi = fabs(phi-inDetPhi);
	if (diffPhi > M_PI) diffPhi = 2.*M_PI - diffPhi;

	if (msgLvl(MSG::VERBOSE)) 
	    msg(MSG::VERBOSE) << std::endl << std::setiosflags(std::ios::fixed)
			      << " Id track: momentum "
			      << std::setw(8) << std::setprecision(1)
			      << perigee.momentum().mag()/Gaudi::Units::GeV
			      << "  with perigee eta and difference "
			      << std::setw(8) << std::setprecision(3) << perigee.eta()
			      << std::setw(8) << std::setprecision(3) << diffEta
			      << "  and same for phi "
			      << std::setw(8) << std::setprecision(3) << perigee.parameters()[Trk::phi]
			      << std::setw(8) << std::setprecision(3) << diffPhi;
    
	if ((diffPhi*diffPhi + diffEta*diffEta) > m_trackCone*m_trackCone)	continue;
	++numberTracks;
	double p = perigee.momentum().mag();
	if (p > m_maxP) m_maxP = p;
	sumP += p;

	if (msgLvl(MSG::VERBOSE)) msg() << "  inside cone, track#" << std::setw(3) << numberTracks;
    }

    return std::make_pair(numberTracks,sumP);
}

std::pair<int,double>
MuidTrackIsolation::trackExtrapolated(const TrackCollection* inDetTracks, double eta, double phi) const
{
    // set initial state
    m_maxP		= 0.;
    double sumP		= 0.;
    int numberTracks	= 0;

    // extrapolate close in eta tracks to calorimeter surface
    for (TrackCollection::const_iterator id = inDetTracks->begin() ;
         id != inDetTracks->end();
         ++id)
    { 
	const Trk::Perigee& perigee = *(**id).perigeeParameters();
	if ((**id).info().trackProperties(Trk::TrackInfo::StraightTrack)
	    || perigee.pT() < m_minPt)					continue;
	
	double inDetEta	= perigee.eta();
	if (fabs(eta-inDetEta) > m_trackCone + m_etaSafetyFactor)	continue;

	// track has sufficient momentum and is close in eta:
	// find intersection at calo surface
	double qOverP	= perigee.parameters()[Trk::qOverP];
	double cotTheta	= 1/tan(perigee.parameters()[Trk::theta]);
	Amg::Vector3D direction(cos(perigee.parameters()[Trk::phi]),sin(perigee.parameters()[Trk::phi]),cotTheta);
	direction /= direction.mag();
	const Trk::TrackSurfaceIntersection* idIntersection   =
	    new Trk::TrackSurfaceIntersection(perigee.position(),direction,0.);
	const Trk::Surface* surface = m_caloCylinder;
	if (cotTheta > m_barrelCotTheta)
	{
	    surface = m_caloForwardDisc;
	}
	else if (cotTheta < -m_barrelCotTheta)
	{
	    surface = m_caloBackwardDisc;
	}	
	const Trk::TrackSurfaceIntersection* caloIntersection =
	    m_intersector->intersectSurface(*surface,idIntersection,qOverP);

	// no intersection - should never happen !
	if (! caloIntersection)
	{
	    ATH_MSG_DEBUG( " track didn't find intersection !!! "
			   << std::setiosflags(std::ios::fixed)
			   << " Id track: momentum "	<< std::setw(8) << std::setprecision(1)
			   << perigee.momentum().mag()/Gaudi::Units::GeV
			   << "  with initial eta "	<< std::setw(8) << std::setprecision(3)
			   << perigee.eta()
			   << "  and phi "		<< std::setw(8) << std::setprecision(3)
			   << perigee.parameters()[Trk::phi] );

	    delete idIntersection;
	    continue;
	}

	double diffEta = eta - caloIntersection->position().eta();
	double diffPhi = fabs(phi-caloIntersection->position().phi());
	if (diffPhi > M_PI) diffPhi = 2.*M_PI - diffPhi;

	if (msgLvl(MSG::VERBOSE))
	{
	    msg() << std::endl << std::setiosflags(std::ios::fixed)
		  << " Id track: momentum "
		  << std::setw(8) << std::setprecision(1)<< perigee.momentum().mag()/Gaudi::Units::GeV
		  << "  with initial,extrapolated and calo difference for eta "
		  << std::setw(8) << std::setprecision(3) << perigee.eta()
		  << std::setw(8) << std::setprecision(3) << caloIntersection->position().eta()
		  << std::setw(8) << std::setprecision(3) << diffEta
		  << "  and phi "
		  << std::setw(8) << std::setprecision(3) << perigee.parameters()[Trk::phi]
		  << std::setw(8) << std::setprecision(3) << caloIntersection->position().phi()
		  << std::setw(8) << std::setprecision(3) << diffPhi;
	}

	// check if inside cone
	if ((diffPhi*diffPhi + diffEta*diffEta) < m_trackCone*m_trackCone)
	{
	    ++numberTracks;
	    double p = perigee.momentum().mag();
	    if (p > m_maxP) m_maxP = p;
	    sumP += p;
	    
	    if (msgLvl(MSG::VERBOSE)) msg() << "  inside cone, track#" << std::setw(3) << numberTracks;
	}
	delete caloIntersection;
	delete idIntersection;
    }
    if (msgLvl(MSG::VERBOSE)) msg() << endmsg;

    return std::make_pair(numberTracks,sumP);
}

}	// end of namespace
