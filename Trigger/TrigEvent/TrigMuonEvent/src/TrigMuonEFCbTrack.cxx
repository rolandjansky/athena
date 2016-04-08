/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TrigMuonEFCbTrack.cxx
Package : offline/Trigger/TrigEvent/TrigMuonEvent
Author  : Sergio Grancagnolo
Created : November 2008
Note    : Simple Trigger AOD for EF Muon (TrigMuonEF)

DESCRIPTION:

Nov.2008: A TrigMuonEFTrack overloading that contains the match chi2.
*****************************************************************************/

// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"
#include "Particle/TrackParticleContainer.h"
#include "Particle/TrackParticle.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

TrigMuonEFCbTrack::TrigMuonEFCbTrack() :
	TrigMuonEFTrack( 0., 0., 0., 0. ),
	m_matchChi2(-999.),
	m_nIdSctHits (0),
	m_nIdPixelHits(0),
	m_nTrtHits(0),
	m_idTrackLink()
{
}

TrigMuonEFCbTrack::TrigMuonEFCbTrack(double theIPt, double theCotTh, double thePhi, double theM) :
	TrigMuonEFTrack(theIPt, theCotTh, thePhi ,theM ),
	m_matchChi2(-999.),
	m_nIdSctHits (0),
	m_nIdPixelHits(0),
	m_nTrtHits(0),
	m_idTrackLink()
{
}

TrigMuonEFCbTrack::TrigMuonEFCbTrack( const TrigMuonEFCbTrack& rhs ) :
  I4Momentum(rhs),
  P4IPtCotThPhiMBase( rhs ),
  INavigable(rhs),
  IAthenaBarCode(),
  INavigable4Momentum(rhs),
  TrigMuonEFTrack(rhs),
  m_matchChi2( rhs.m_matchChi2 ),
  m_nIdSctHits (rhs.m_nIdSctHits),
  m_nIdPixelHits(rhs.m_nIdPixelHits),
  m_nTrtHits(rhs.m_nTrtHits),
  m_idTrackLink(rhs.m_idTrackLink)
{ }

TrigMuonEFCbTrack::TrigMuonEFCbTrack( const TrigMuonEFTrack& rhs ) :
  I4Momentum(rhs),
  P4IPtCotThPhiMBase( rhs ),
  INavigable(rhs),
  IAthenaBarCode(),
  INavigable4Momentum(rhs),
  TrigMuonEFTrack(rhs),
  m_matchChi2(-999.),
  m_nIdSctHits (0),
  m_nIdPixelHits(0),
  m_nTrtHits(0),
  m_idTrackLink()
  { }

/** Assignement operator */
TrigMuonEFCbTrack& TrigMuonEFCbTrack::operator=( const TrigMuonEFCbTrack& rhs )
{
  if ( this != &rhs ) {
    TrigMuonEFTrack::operator=( rhs );
    m_matchChi2 = rhs.m_matchChi2;
    m_idTrackLink = rhs.m_idTrackLink;
  }
  return *this;
}

/** set data member */
void TrigMuonEFCbTrack::setMatchChi2(double theMatchChi2){
  m_matchChi2 = theMatchChi2;
}

/** Destructor */
TrigMuonEFCbTrack::~TrigMuonEFCbTrack() {}

/** 
 * Get associated ID track 
 */
const Rec::TrackParticle* TrigMuonEFCbTrack::getIDTrackParticle() const {
  if(!m_idTrackLink.isValid()) return 0; //possible since old data has no link
  return *(m_idTrackLink);
}

/** 
 * Set associated ID track.
 *
 * The container (cont) should be the container the ID track is in
 * and itrk is the index which specifies the position of the track
 * within the container.
 */
void TrigMuonEFCbTrack::setIDTrack(const Rec::TrackParticleContainer* cont, unsigned int itrk) {
  m_idTrackLink.toIndexedElement(*cont, itrk);
}

/** 
 * Set associated ID track.
 *
 * Just pass directly an element link pointing to the correct ID track.
 */
void TrigMuonEFCbTrack::setIDTrackElementLink(const ElementLink<Rec::TrackParticleContainer>& link) {
  m_idTrackLink = link;
}

//////////////////////////////////////////////////////////////////
// helper operators

/**
 * This function uses the function str(TrigMuonEFTrack&) to print
 * the "base class part" of the object.
 */
std::string str( const TrigMuonEFCbTrack& d ) {
   std::stringstream ss;
   ss << str( ( TrigMuonEFTrack& ) d )
      << "; chi2 match: " << d.matchChi2();

   return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigMuonEFCbTrack& d ) {

   return ( m << str( d ) );
}

/**
 * This operator uses the operator operator==(TrigMuonEFTrack&,TrigMuonEFTrack&)
 * to compare the "base class parts" of the objects.
 */
bool operator== ( const TrigMuonEFCbTrack& a, const TrigMuonEFCbTrack& b ) {

   if( ( TrigMuonEFTrack& ) a != ( TrigMuonEFTrack& ) b )
      return false;
   if( std::abs( a.matchChi2() - b.matchChi2() ) > DELTA )
      return false;

   return true;
}

/**
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are varaible names and relative differences
 * between two obejcts
 *
 * @param variableChange Map to record the differences
 * In case of collections (or objects when the size may be different) that
 * information can also be returned in varaibleChange
 */
void diff( const TrigMuonEFCbTrack& a, const TrigMuonEFCbTrack& b,
           std::map< std::string, double >& variableChange ) {

   // Call the function operating on the base class:
   diff( ( TrigMuonEFTrack& ) a, ( TrigMuonEFTrack& ) b, variableChange );

   if( std::abs( a.matchChi2() - b.matchChi2() ) > DELTA ) {
      variableChange[ "MatchChi2" ] = a.matchChi2() - b.matchChi2();
   }

   return;
}
