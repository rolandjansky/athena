/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidBackTracker
//  AlgTool to back-track all tracks in an input spectrometer collection to 
//  the IP.
//  Takes into account the calorimeter energy deposit and Coulomb scattering.
//  The return collection has a one-to-one correspondence to the input.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "CLHEP/Units/SystemOfUnits.h"
#include "MuidCombinedTools/MuidBackTracker.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "TrkTrack/TrackInfo.h"

namespace Rec
{

MuidBackTracker::MuidBackTracker (const std::string&	type,
				  const std::string&	name, 
				  const IInterface*	parent)
    :	AthAlgTool			(type, name, parent),
	m_trackBuilder			("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
	m_trackQuery			("Rec::MuonTrackQuery/MuonTrackQuery"),
	m_countBadFit			(0),
	m_countBadFitBarrel		(0),
	m_countBadFitEndcap		(0),
	m_countBadFitNearBarrelCoil	(0),
	m_countBadFitNearEndcapCoil	(0),
	m_countMS			(0),
	m_countSA			(0)
{
    declareInterface<IMuidBackTracker>(this);
    
    // configure property values
    declareProperty("TrackBuilder",	m_trackBuilder);
    declareProperty("TrackQuery",      	m_trackQuery);
    declareProperty("BadFitChi2",	m_badFitChi2 = 2.5);
}

MuidBackTracker::~MuidBackTracker()
{}

StatusCode
MuidBackTracker::initialize()
{
    ATH_MSG_INFO( "MuidBackTracker::initialize() - package version " << PACKAGE_VERSION );

    // get the Tools
    if (m_trackBuilder.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackBuilder );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackBuilder );
    }
    if (m_trackQuery.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuery );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackQuery );
    }
 
    return StatusCode::SUCCESS;
}

StatusCode
MuidBackTracker::finalize()
{
    double badFit = 0.;
    if (m_countSA) badFit = static_cast<double>(100*m_countBadFit)/static_cast<double>(m_countSA);
    
    ATH_MSG_INFO( "finalized with " << m_countSA << " out of " << m_countMS
		  << " MuonSpectrometer tracks having a successful standalone fit. Of these:"
		  << std::setiosflags(std::ios::fixed)
		  << std::setw(6) << std::setprecision(1) << badFit << "% have a bad fit" );
    ATH_MSG_INFO( "        Barrel: " << m_countBadFitBarrel
		  << " with bad fit quality, of which "
		  << m_countBadFitNearBarrelCoil << " pass near a toroid coil" );
    ATH_MSG_INFO( "        Endcap: " << m_countBadFitEndcap
		  << " with bad fit quality, of which "
		  << m_countBadFitNearEndcapCoil << " pass near a toroid coil" );
    
    return StatusCode::SUCCESS;
}

TrackCollection*
MuidBackTracker::execute(const TrackCollection& spectrometerTracks) const
{
    ATH_MSG_VERBOSE( " execute: begin" );
    
    // empty container to store eventual standalone MuidTracks
    TrackCollection* standaloneTracks = new TrackCollection;

    for (TrackCollection::const_iterator t = spectrometerTracks.begin();
	 t != spectrometerTracks.end();
	 ++t)
    {
	++m_countMS;
	Trk::Track* standaloneTrack		= 0;
		
	// ignore badly constructed spectrometer tracks -
	// otherwise there's a downstream problem because the corresponding particle doesn't exist
	const Trk::Perigee* standaloneTrackPerigee =
	    dynamic_cast<const Trk::Perigee*>((**t).perigeeParameters());
	if (! standaloneTrackPerigee || !standaloneTrackPerigee->covariance())
	{
	   ATH_MSG_WARNING( " skip spectrometer track without Perigee and associated cov matrix" );
	}
	else
	{
	    const Trk::Track* outgoingTrack	= m_trackQuery->outgoingTrack(**t);
	    if (outgoingTrack) standaloneTrack	= m_trackBuilder->standaloneFit(*outgoingTrack);
	    if (outgoingTrack != *t) delete outgoingTrack;
	}
	
	if (standaloneTrack)
	{
	    ++m_countSA;
	    standaloneTrack->info().setParticleHypothesis(Trk::muon);
	    standaloneTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidStandAlone);
	    standaloneTracks->push_back(standaloneTrack);
	    
	    // monitor tracks with poor fit quality
	    const Trk::FitQuality* fitQuality = standaloneTrack->fitQuality();
	    if (fitQuality
		&& fitQuality->numberDoF() > 0
		&& fitQuality->chiSquared() > m_badFitChi2*fitQuality->doubleNumberDoF())
	    {
		++m_countBadFit;
		
		ATH_MSG_DEBUG( " bad fit: chi2 "
			       << fitQuality->chiSquared()/fitQuality->doubleNumberDoF() );

		for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
			 s = (**t).trackStateOnSurfaces()->begin();
		     s != (**t).trackStateOnSurfaces()->end();
		     ++s)
		{
		    if (! (**s).trackParameters()) continue;
		    if ((**s).trackParameters()->position().perp() < 5.*CLHEP::meter
			&& fabs((**s).trackParameters()->position().z()) < 10.*CLHEP::meter) continue;
		    
		    double phi = (**s).trackParameters()->position().phi();
		    if (phi < 0.) phi += 2.*M_PI;
		    double period	=  M_PI/4.;
		    int n		=  static_cast<int>(phi/period);
		    phi			-= static_cast<double>(n)*period;
		    if (phi > 0.5*period) phi = period - phi;
			
		    // barrel or endcap
		    if ((**s).trackParameters()->position().perp() > 5.*CLHEP::meter)
		    {
			++m_countBadFitBarrel;
			if (phi > 0.3) ++m_countBadFitNearBarrelCoil;
			
			ATH_MSG_DEBUG( "  Phi offset from barrel coil " << 0.5*period - phi );
			break;
		    }
		    else
		    {
			++m_countBadFitEndcap;
			if (phi < 0.1) ++m_countBadFitNearEndcapCoil;
			
			ATH_MSG_DEBUG( "  Phi offset from endcap coil " << phi );
			break;
		    }
		}
	    }
	}
	else
	{
	    standaloneTracks->push_back(new Trk::Track(**t));
	    ATH_MSG_VERBOSE( "  extrap fails: copy spectrometer track " );
	}

	// FIXME: provide debug/verbose o/p cf the old SA algorithm
    }

    return standaloneTracks;
}
 
}	// end of namespace
   
