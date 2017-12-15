// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFCBTRACK_H
#define TRIGMUONEVENT_TRIGMUONEFCBTRACK_H

// STL include(s):
#include <map>
#include <string>

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"
#include "AthLinks/ElementLink.h"
#include "Particle/TrackParticleContainer.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonEFTrack.h"

// Forward declaration(s):
class MsgStream;
namespace Rec {
  class TrackParticle;
}

class TrigMuonEFCbTrack : public TrigMuonEFTrack {

public: 
   /** Default constructor */
   TrigMuonEFCbTrack( );
   /** Default constructor */
   TrigMuonEFCbTrack( double theIPt, double theCotTh, double thePhi, double theM );

   /** Copy constructor */
   TrigMuonEFCbTrack( const TrigMuonEFCbTrack& rhs );
   TrigMuonEFCbTrack( const TrigMuonEFTrack& rhs );

   /** Assignement operator */
   TrigMuonEFCbTrack& operator=( const TrigMuonEFCbTrack& rhs );

   /** Destructor */
   ~TrigMuonEFCbTrack();

   /* set methods */
   void setMatchChi2( double theMatchChi2 );
   void setIDTrack(const Rec::TrackParticleContainer* cont, unsigned int itrk);
   void setIDTrackElementLink(const ElementLink<Rec::TrackParticleContainer>& link);

   /** Get associated ID track */
   const Rec::TrackParticle* getIDTrackParticle() const;
   const ElementLink<Rec::TrackParticleContainer>& getIDTrackParticleLink() const {return m_idTrackLink;}

   /* get methods */
   double matchChi2() const { return m_matchChi2; }

	unsigned short int NIdSctHits() const
	{
		return m_nIdSctHits;
	}

	void setNIdSctHits(unsigned short int nIdSctHits)
	{
		this->m_nIdSctHits = nIdSctHits;
	}

	unsigned short int NIdPixelHits() const
	{
		return m_nIdPixelHits;
	}

	void setNIdPixelHits(unsigned short int nIdPixelHits)
	{
		this->m_nIdPixelHits = nIdPixelHits;
	}

	unsigned short int NTrtHits() const
	{
		return m_nTrtHits;
	}

	void setNTrtHits(unsigned short int nTrtHits)
	{
		this->m_nTrtHits = nTrtHits;
	}



private:
   double m_matchChi2;
   unsigned short int m_nIdSctHits;
   unsigned short int m_nIdPixelHits;
   unsigned short int m_nTrtHits;

  /// Link to the associated ID track
  ElementLink<Rec::TrackParticleContainer> m_idTrackLink;
};

/// Helper function for printing the object
std::string str( const TrigMuonEFCbTrack& d );
/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigMuonEFCbTrack& d );

/// Operator comparing two TrigMuonEFCbTrack objects for equality
bool operator== ( const TrigMuonEFCbTrack& a, const TrigMuonEFCbTrack& b );
/// Operator comparing two TrigMuonEFCbTrack objects for inequality
inline bool operator!= ( const TrigMuonEFCbTrack& a, const TrigMuonEFCbTrack& b ) {
   return !( a == b );
}

/// Comparison with feedback
void diff( const TrigMuonEFCbTrack& a, const TrigMuonEFCbTrack& b,
           std::map< std::string, double >& variableChange );

CLASS_DEF( TrigMuonEFCbTrack , 56071859, 1 )

#endif // TRIGMUONEVENT_TRIGMUONEFCBTRACK_H
