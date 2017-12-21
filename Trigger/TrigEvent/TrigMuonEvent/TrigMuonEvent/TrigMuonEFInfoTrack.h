// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFINFOTRACK_H
#define TRIGMUONEVENT_TRIGMUONEFINFOTRACK_H

// STL include(s):
#include <map>
#include <string>

#include "AthenaKernel/CLASS_DEF.h"

// Forward declaration(s):
class MsgStream;
class TrigMuonEFTrack;
class TrigMuonEFCbTrack;

class TrigMuonEFInfoTrack
{
public:
	/* muon types */
	static const int MuonTypeEFMuon=1;
	static const int MuGirlMuon=2;
	static const int MuGirlMuonTag=3;

	/** Default constructor */
	TrigMuonEFInfoTrack();
	/** Copy constructor */
	TrigMuonEFInfoTrack(const TrigMuonEFInfoTrack & rhs);
	/** Assignement operator */
	TrigMuonEFInfoTrack & operator =(const TrigMuonEFInfoTrack & rhs);
	/** Destructor */
	~TrigMuonEFInfoTrack();

	bool hasSpectrometerTrack() const;
	bool hasExtrapolatedTrack() const;
	bool hasCombinedTrack() const;

	void setSpectrometerTrack( TrigMuonEFTrack* track );
	void setExtrapolatedTrack( TrigMuonEFTrack* track );
	void setCombinedTrack( TrigMuonEFCbTrack* track );

        void setSpectrometerTrack( std::unique_ptr<TrigMuonEFTrack> track );
        void setExtrapolatedTrack( std::unique_ptr<TrigMuonEFTrack> track );
        void setCombinedTrack( std::unique_ptr<TrigMuonEFCbTrack> track );

	TrigMuonEFTrack* SpectrometerTrack() const { return m_spectrometerTrack; }
	TrigMuonEFTrack* ExtrapolatedTrack() const { return m_extrapolatedTrack; }
	TrigMuonEFCbTrack* CombinedTrack()   const { return m_combinedTrack; }

	unsigned short int MuonType() const
	{
		return m_muonType;
	}

	void setMuonType(unsigned short int muonType)
	{
		this->m_muonType = muonType;
	}

private:

	TrigMuonEFTrack* m_spectrometerTrack;
	TrigMuonEFTrack* m_extrapolatedTrack;
	TrigMuonEFCbTrack* m_combinedTrack;
	unsigned short int m_muonType;

};

std::string str( const TrigMuonEFInfoTrack& d );                      //<! printing helper
MsgStream& operator<< ( MsgStream& m, const TrigMuonEFInfoTrack& d ); //<! printing helper (wraps above)

bool operator== ( const TrigMuonEFInfoTrack& a, const TrigMuonEFInfoTrack& b );

inline bool operator!= ( const TrigMuonEFInfoTrack& a, const TrigMuonEFInfoTrack& b ) {
   return !( a == b );
}

/** @brief comparison with feedback
 * Function compares two objects and returns "semi verbose" output
 * in the form of map where there are variable names and differences
 * between two objects
 * @param variableChange - map to record the differences
 * In case of collections (or objects when the size may be different) that information can also be returned in variableChange
 */
void diff( const TrigMuonEFInfoTrack& a, const TrigMuonEFInfoTrack& b, std::map<std::string, double>& variableChange );

CLASS_DEF( TrigMuonEFInfoTrack , 154078292 , 1 )

#endif // TRIGMUONEVENT_TRIGMUONEFINFOTRACK_H
