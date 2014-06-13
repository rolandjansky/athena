/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidVertexCombiner
// FIXME::::
//  AlgTool to back-track all tracks in an input spectrometer collection to 
//  the IP.
//  Takes into account the calorimeter energy deposit and Coulomb scattering.
//  The return collection has a one-to-one correspondence to the input.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include <iomanip>
#include "CLHEP/Units/SystemOfUnits.h"
#include "MuidCombinedTools/MuidVertexCombiner.h"
#include "MuidEvent/MuidTrackContainer.h"
#include "MuidEvent/MuonMatch.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "muonEvent/MuonContainer.h"
#include "TrkTrack/TrackInfo.h"
#include "VxVertex/VxContainer.h"

namespace Rec
{

MuidVertexCombiner::MuidVertexCombiner (const std::string&	type,
					const std::string&	name, 
					const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_trackBuilder		("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
	m_trackQuery		("Rec::MuonTrackQuery/MuonTrackQuery"),
	m_badFitChi2		(2.5),				// used to keep statistics
	m_maxChi2Increase	(1.0),				// reject fit if vertex is outlier (adds 2 DOF)
	m_maxCovZZ		(100.*CLHEP::mm*CLHEP::mm),	// select well-defined vx from covariance 
	m_minEta		(2.4),				// select high eta muons
	m_countBadFit		(0),
	m_countMS		(0),
	m_countVC		(0)
{
    declareInterface<IMuidVertexCombiner>(this);
    
    // configure property values
    declareProperty("TrackBuilder",	m_trackBuilder);
    declareProperty("TrackQuery",	m_trackQuery);
    declareProperty("BadFitChi2",	m_badFitChi2);
    declareProperty("MaxChi2Increase",	m_maxChi2Increase);
    declareProperty("MaxCovZZ",		m_maxCovZZ);
    declareProperty("MinEta",		m_minEta);
}

MuidVertexCombiner::~MuidVertexCombiner()
{}

StatusCode
MuidVertexCombiner::initialize()
{
    ATH_MSG_INFO( "MuidVertexCombiner::initialize() - package version " << PACKAGE_VERSION );

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
MuidVertexCombiner::finalize()
{
    double badFit = 0.;
    if (m_countVC) badFit = static_cast<double>(100*m_countBadFit)/static_cast<double>(m_countVC);
    
    ATH_MSG_INFO( "finalized with " << m_countVC << " out of " << m_countMS
		  << " selected high-eta MuonSpectrometer tracks having a successful vertex association."
		  << std::setiosflags(std::ios::fixed)
		  << std::setw(6) << std::setprecision(1) << badFit << "% have a bad fit" );
    
    return StatusCode::SUCCESS;
}

std::vector<MuonMatch*>*
MuidVertexCombiner::execute(TrackCollection&		extrapolatedTracks,
			    const MuidTrackContainer*	muonMatches,
			    const VxContainer&		primaryVertices) const
{
    // empty container to store eventual vertexAssociated MuonMatches
    std::vector<MuonMatch*>* vertexMatches = new std::vector<MuonMatch*>;
    vertexMatches->reserve(extrapolatedTracks.size());

    // check there are vertices, create primary vertex spanning the whole collection
    ATH_MSG_VERBOSE( "the vertex container has size " << primaryVertices.size() );
    if (! primaryVertices.size()) return vertexMatches;

    bool debugMsgPrimary		= true;
    Trk::RecVertex* primaryVertex	= 0;
    for (VxContainer::const_iterator v = primaryVertices.begin();
	 v != primaryVertices.end();
	 ++v )
    {
	// skip badly defined vertices
	const Trk::RecVertex& recVertex = (**v).recVertex();
	if (recVertex.covariancePosition()(Trk::z,Trk::z) > m_maxCovZZ) continue;

	//FIXME? take first vertex for now. Maybe the best decision anyway...
	if (! primaryVertex) primaryVertex	= new Trk::RecVertex(recVertex);
	
	if (msgLvl(MSG::DEBUG))
	{ 		
	    Amg::Vector3D position = recVertex.position();
	    if (primaryVertex && debugMsgPrimary)
	    {
		debugMsgPrimary	= false;
		ATH_MSG_DEBUG( "Primary vertex at x, y, z, r:" << std::setiosflags(std::ios::fixed)
			       << std::setw(7) << std::setprecision(1) << position.x() << ", "
			       << std::setw(7) << std::setprecision(1) << position.y() << ", "
			       << std::setw(7) << std::setprecision(1) << position.z() << ", "
			       << std::setw(7) << std::setprecision(3) << position.perp()
			       << "  covZZ " << std::setw(7) << std::setprecision(3)
			       << sqrt(recVertex.covariancePosition()(Trk::z,Trk::z)));
	    }
	    else
	    {
		ATH_MSG_VERBOSE( "Primary vertex at x, y, z, r:" << std::setiosflags(std::ios::fixed)
				 << std::setw(7) << std::setprecision(1) << position.x() << ", "
				 << std::setw(7) << std::setprecision(1) << position.y() << ", "
				 << std::setw(7) << std::setprecision(1) << position.z() << ", "
				 << std::setw(7) << std::setprecision(3) << position.perp()
				 << "  covZZ " << std::setw(7) << std::setprecision(3)
				 << sqrt(recVertex.covariancePosition()(Trk::z,Trk::z)));
	    }	
	}
    }
    if (! primaryVertex) return vertexMatches;

    // select tracks for vertex constrained fit
    int num = 1;
    for (TrackCollection::iterator t = extrapolatedTracks.begin(); 
	 t != extrapolatedTracks.end();
	 ++t, ++num)
    {	
	// skip if already matched to an indet track
	const Trk::Track* extrapolatedTrack	= *t;
	if (muonMatches)
	{
	    bool isCombined	= false;
	    for (MuidTrackContainer::const_iterator mm = muonMatches->begin(); 
		 mm != muonMatches->end();
		 ++mm)
	    {
		if (extrapolatedTrack == (**mm).extrapolatedTrack())
		{
		    isCombined = true;
		    break;
		}
	    }
	    if (isCombined)
	    {
		ATH_MSG_VERBOSE( "extrapolated track # " << num
				 << ": skip as track matched to indet");
		continue;
	    }
	}	
		
	// select high eta extrapolated tracks with fitted curvature
	// and sufficient phi measurements (i.e. no more than 1 pseudo meas)
	if (! m_trackQuery->isExtrapolated(**t)
	    || m_trackQuery->numberPseudoMeasurements(**t) > 1
	    || (**t).info().trackProperties(Trk::TrackInfo::StraightTrack)
	    || fabs((**t).perigeeParameters()->eta()) < m_minEta)
	{
	    if (msgLvl(MSG::VERBOSE))
	    {
		if (! m_trackQuery->isExtrapolated(**t))
		{
		    ATH_MSG_VERBOSE( "extrapolated track # " << num
				     << ": skip as failed extrapolation ");
		    continue;
		}
		if (m_trackQuery->numberPseudoMeasurements(**t) > 1)
		{
		    ATH_MSG_VERBOSE( "extrapolated track # " << num
				     << ": skip as too many pseudo ");
		    continue;
		}
		if ((**t).info().trackProperties(Trk::TrackInfo::StraightTrack))
		{
		    ATH_MSG_VERBOSE( "extrapolated track # " << num
				     << ": skip as straight track ");
		    continue;
		}
		if (fabs((**t).perigeeParameters()->eta()) < m_minEta)
		{
		    ATH_MSG_VERBOSE( "extrapolated track # " << num
				     << ": skip as not at high eta ");
		    continue;
		}
	    }
	    
	    continue;
	}

	// try refit with primary vertex
	++m_countMS;

	ATH_MSG_DEBUG( "extrapolated track # " << num
		       << ":  create match with vertex-constrained refit" );
	Trk::Track* track = m_trackBuilder->standaloneFit(**t, primaryVertex);
	if (! track)
	{
	    ATH_MSG_VERBOSE( " association rejected as fails refit ");
	    continue;
	}
	
	// make quality cut wrt original extrapolated track
	double deltaChi2	= 99999.;
	if (track->fitQuality() && track->fitQuality()->numberDoF())
	    deltaChi2	= track->fitQuality()->chiSquared()/track->fitQuality()->doubleNumberDoF() -
			  (**t).fitQuality()->chiSquared()/(**t).fitQuality()->doubleNumberDoF();
	if (deltaChi2 > m_maxChi2Increase)
	{
	    ATH_MSG_VERBOSE( " association rejected due to chi2 increase of vx-constrained fit"
			     << "  vx chi2: "
			     << track->fitQuality()->chiSquared()/track->fitQuality()->doubleNumberDoF()
			     << "  sa chi2: "
			     << (**t).fitQuality()->chiSquared()/(**t).fitQuality()->doubleNumberDoF());
	    delete track;
	    continue;
	}
	
	// accepted match
	++m_countVC;
	track->info().setParticleHypothesis(Trk::muon);
	track->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidVertexAssociator);
	Rec::MuonMatch* muonMatch	= new Rec::MuonMatch(0,*t,0,0);
	muonMatch->extrapolatedRefit(track);
	vertexMatches->push_back(muonMatch);

	// monitor tracks with poor fit quality
	if (track->fitQuality()->chiSquared() > m_badFitChi2*track->fitQuality()->doubleNumberDoF())
	{
	    ++m_countBadFit;
	    ATH_MSG_DEBUG( " bad fit quality: chi2 " << track->fitQuality()->chiSquared() /
			   track->fitQuality()->doubleNumberDoF() );
	}
    }

    // delete local copy of primary vertex
    delete primaryVertex;
    return vertexMatches;
}
 
}	// end of namespace
   
