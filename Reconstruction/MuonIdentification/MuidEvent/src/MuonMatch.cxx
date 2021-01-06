/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class  MuonMatch

   Store association between tracks forming combined muon
   and information on the match quality
	 
   @author Alan.Poppleton@cern.ch
*/
///////////////////////////////////////////////////////////////////////////////

#include "MuidEvent/MuonMatch.h"
#include "Particle/TrackParticle.h"
#include "TrkTrack/Track.h"
#include "VxVertex/RecVertex.h"

namespace Rec
{

// combined track constructor (inDet track matched to extrapolated track)
MuonMatch::MuonMatch (const Trk::Track*			combinedTrack,
		      const Trk::Track*			extrapolatedTrack,
		      const Trk::Track*			indetTrack,
		      const Trk::Track*			spectrometerTrack,
		      FieldIntegral			fieldIntegral,
		      double   				innerMatchChi2,
		      int				innerMatchDoF,
		      double   				innerMatchProb,
		      double				momentumBalanceSignificance,
		      double   				outerMatchChi2,
		      int				outerMatchDoF,
		      double   				outerMatchProb,
		      ScatteringAngleSignificance	scatteringAngleSignificance)
    :	m_combinedTrack				(combinedTrack),
	m_extrapolatedRefit			(0),
	m_extrapolatedTrack			(extrapolatedTrack),
	m_indetParticle				(0),
	m_indetTrack				(indetTrack),
	m_spectrometerParticle			(0),
	m_spectrometerTrack			(spectrometerTrack),
	m_vertex				(0),
	m_bestMatch				(false),
	m_fieldIntegral				(fieldIntegral),
	m_innerMatchChi2			(innerMatchChi2),
	m_innerMatchDoF				(innerMatchDoF),
	m_innerMatchProb			(innerMatchProb),
	m_matchType				(Unmatched),
	m_momentumBalanceSignificance		(momentumBalanceSignificance),
	m_outerMatchChi2			(outerMatchChi2),
	m_outerMatchDoF				(outerMatchDoF),
	m_outerMatchProb			(outerMatchProb),
	m_scatteringAngleSignificance		(scatteringAngleSignificance),
	m_looseMatch				(false),
	m_simpleMatch				(false)
{}

// combined track constructor (vertexAssociated track)
MuonMatch::MuonMatch (const Trk::Track*			combinedTrack,
		      const TrackParticle*		indetParticle,
		      const TrackParticle*		spectrometerParticle,
		      const Trk::RecVertex*		vertex,
		      FieldIntegral			fieldIntegral,
		      ScatteringAngleSignificance	scatteringAngleSignificance)
    :	m_combinedTrack				(combinedTrack),
	m_extrapolatedRefit			(0),
	m_extrapolatedTrack			(0),
	m_indetParticle				(indetParticle),
	m_indetTrack				(0),
	m_spectrometerParticle			(spectrometerParticle),
	m_spectrometerTrack			(0),
	m_vertex				(vertex),
	m_bestMatch				(false),
	m_fieldIntegral				(fieldIntegral),
	m_innerMatchChi2			(0.),
	m_innerMatchDoF				(0),
	m_innerMatchProb			(0.),
	m_matchType				(Vertex),
	m_momentumBalanceSignificance		(0.),
	m_outerMatchChi2			(0.),
	m_outerMatchDoF				(0),
	m_outerMatchProb			(0.),
	m_scatteringAngleSignificance		(scatteringAngleSignificance),
	m_looseMatch				(false),
	m_simpleMatch				(false)
{
    if (indetParticle)		m_indetTrack		= indetParticle->originalTrack();
    if (spectrometerParticle)	m_spectrometerTrack	= spectrometerParticle->originalTrack();
}

// copy and steal constructor
MuonMatch::MuonMatch (MuonMatch& match)
    :	m_combinedTrack				(match.m_combinedTrack),
	m_extrapolatedRefit			(match.m_extrapolatedRefit),
	m_extrapolatedTrack			(match.m_extrapolatedTrack),
	m_indetParticle                         (match.m_indetParticle),
	m_indetTrack				(match.m_indetTrack),
	m_spectrometerParticle                  (match.m_spectrometerParticle),
	m_spectrometerTrack			(match.m_spectrometerTrack),
	m_vertex				(match.m_vertex),
	m_bestMatch				(match.m_bestMatch),
	m_fieldIntegral				(match.m_fieldIntegral),
	m_innerMatchChi2			(match.m_innerMatchChi2),
	m_innerMatchDoF				(match.m_innerMatchDoF),
	m_innerMatchProb			(match.m_innerMatchProb),
	m_matchType				(match.m_matchType),
	m_momentumBalanceSignificance		(match.m_momentumBalanceSignificance),
	m_outerMatchChi2			(match.m_outerMatchChi2),
	m_outerMatchDoF				(match.m_outerMatchDoF),
	m_outerMatchProb			(match.m_outerMatchProb),
	m_scatteringAngleSignificance		(match.m_scatteringAngleSignificance),
	m_looseMatch				(match.m_looseMatch),
	m_simpleMatch				(match.m_simpleMatch)
{
    // steal combined track
    if (match.m_combinedTrack)		match.m_combinedTrack		= 0;
    if (match.m_extrapolatedRefit)	match.m_extrapolatedRefit	= 0;
}

// assign and steal operator
MuonMatch& MuonMatch::operator= (MuonMatch& match)
{
    if (this!=&match){
	m_extrapolatedTrack			=match.m_extrapolatedTrack;
	m_indetParticle                         =match.m_indetParticle;
	m_indetTrack				=match.m_indetTrack;
	m_spectrometerParticle                  =match.m_spectrometerParticle;
	m_spectrometerTrack			=match.m_spectrometerTrack;
	m_vertex				=match.m_vertex;

	m_bestMatch				=match.m_bestMatch;
	m_fieldIntegral				=match.m_fieldIntegral;
	m_innerMatchChi2			=match.m_innerMatchChi2;
	m_innerMatchDoF				=match.m_innerMatchDoF;
	m_innerMatchProb			=match.m_innerMatchProb;
	m_matchType				=match.m_matchType;
	m_momentumBalanceSignificance		=match.m_momentumBalanceSignificance;
	m_outerMatchChi2			=match.m_outerMatchChi2;
	m_outerMatchDoF				=match.m_outerMatchDoF;
	m_outerMatchProb			=match.m_outerMatchProb;
	m_scatteringAngleSignificance		=match.m_scatteringAngleSignificance;
	m_looseMatch				=match.m_looseMatch;
	m_simpleMatch				=match.m_simpleMatch;

	if(m_combinedTrack)			delete m_combinedTrack;
	m_combinedTrack				=match.m_combinedTrack;
	if (match.m_combinedTrack)		match.m_combinedTrack		= 0;

	if(m_extrapolatedRefit)			delete m_extrapolatedRefit;
	m_extrapolatedRefit			=match.m_extrapolatedRefit;
	if (match.m_extrapolatedRefit)		match.m_extrapolatedRefit	= 0;
    }
    return *this;
}

MuonMatch::~MuonMatch (void)
{
    delete m_combinedTrack;
    delete m_extrapolatedRefit;
}

void
MuonMatch::extrapolatedRefit (const Trk::Track* refittedTrack)
{
    const Trk::Track* oldTrack = m_extrapolatedRefit;
    m_extrapolatedRefit = refittedTrack;
    delete oldTrack;
}
 
void
MuonMatch::extrapolatedTrack (const Trk::Track* extrapolatedTrack)
{
    m_extrapolatedTrack	= extrapolatedTrack;
}
 
Trk::Track*
MuonMatch::stealCombinedTrack (void)
{
    // give up ownership of combined track
    const Trk::Track* combinedTrack = m_combinedTrack;
    m_combinedTrack = 0;
    return const_cast<Trk::Track*>(combinedTrack);
}

Trk::Track*
MuonMatch::stealExtrapolatedRefit (void)
{
    // give up ownership of combined track
    const Trk::Track* extrapolatedRefit = m_extrapolatedRefit;
    m_extrapolatedRefit = 0;
    return const_cast<Trk::Track*>(extrapolatedRefit);
}
 
void
MuonMatch::fieldIntegral (FieldIntegral value)
{ m_fieldIntegral = value; }
 
void
MuonMatch::matchType (MatchType type)
{ m_matchType = type; }  
   
void
MuonMatch::momentumBalanceSignificance (double value)
{ m_momentumBalanceSignificance = value; }
  
void
MuonMatch::scatteringAngleSignificance (ScatteringAngleSignificance value)
{ m_scatteringAngleSignificance = value; }
    
}	// end of namespace
 
