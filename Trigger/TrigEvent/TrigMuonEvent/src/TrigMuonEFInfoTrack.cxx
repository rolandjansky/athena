/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TrigMuonEFInfoTrack.cxx
Package : offline/Trigger/TrigEvent/TrigMuonEvent
Author  : Alexander Oh
Created : Mar 2009
Note    : Trigger container AOD for EF Muon (TrigMuonEFInfoTrack)

DESCRIPTION:

Mar 2009: Changed structure. TrigMuonEFInfoTrack is introduced to hold
          Track related data. Allows multiple muons per roi.
 *****************************************************************************/

// STL include(s):
#include <sstream>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonEFInfoTrack.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

TrigMuonEFInfoTrack::TrigMuonEFInfoTrack()
{
	m_spectrometerTrack = new TrigMuonEFTrack();
	m_extrapolatedTrack = new TrigMuonEFTrack();
	m_combinedTrack = new TrigMuonEFCbTrack();
	m_muonType = MuonTypeEFMuon;
}

TrigMuonEFInfoTrack::TrigMuonEFInfoTrack( const TrigMuonEFInfoTrack& rhs ) :

	m_spectrometerTrack( new TrigMuonEFTrack( *rhs.m_spectrometerTrack )),
	m_extrapolatedTrack( new TrigMuonEFTrack(*rhs.m_extrapolatedTrack )),
	m_combinedTrack( new TrigMuonEFCbTrack( *rhs.m_combinedTrack )),
	m_muonType( rhs.m_muonType )
	{ }

TrigMuonEFInfoTrack& TrigMuonEFInfoTrack::operator=( const TrigMuonEFInfoTrack& rhs )
{
	if ( this != &rhs ) {
		delete m_spectrometerTrack;
		delete m_extrapolatedTrack;
		delete m_combinedTrack;

		m_spectrometerTrack = new TrigMuonEFTrack( *rhs.m_spectrometerTrack );
		m_extrapolatedTrack = new TrigMuonEFTrack( *rhs.m_extrapolatedTrack );
		m_combinedTrack = new TrigMuonEFCbTrack( *rhs.m_combinedTrack );
		m_muonType = rhs.m_muonType;
	}
	return *this;
}

TrigMuonEFInfoTrack::~TrigMuonEFInfoTrack()
{
	delete m_spectrometerTrack;
	delete m_extrapolatedTrack;
	delete m_combinedTrack;
}


void TrigMuonEFInfoTrack::setSpectrometerTrack( TrigMuonEFTrack* track )
{
  *m_spectrometerTrack = *track;
}

void TrigMuonEFInfoTrack::setExtrapolatedTrack( TrigMuonEFTrack* track )
{
  *m_extrapolatedTrack = *track;
}

void TrigMuonEFInfoTrack::setCombinedTrack( TrigMuonEFCbTrack* track )
{
  *m_combinedTrack = *track;
}

void TrigMuonEFInfoTrack::setSpectrometerTrack( std::unique_ptr<TrigMuonEFTrack> track )
{
  delete m_spectrometerTrack;
  m_spectrometerTrack = track.release();
}

void TrigMuonEFInfoTrack::setExtrapolatedTrack( std::unique_ptr<TrigMuonEFTrack> track )
{
  delete m_extrapolatedTrack;
  m_extrapolatedTrack = track.release();
}

void TrigMuonEFInfoTrack::setCombinedTrack( std::unique_ptr<TrigMuonEFCbTrack> track )
{
  delete m_combinedTrack;
  m_combinedTrack = track.release();
}

bool TrigMuonEFInfoTrack::hasSpectrometerTrack() const {
	if (m_spectrometerTrack!=0 && m_spectrometerTrack->charge()!=m_spectrometerTrack->CHARGE_NA) {
		return true;
	}
	else {
		return false;
	}

}

bool TrigMuonEFInfoTrack::hasCombinedTrack() const {
	if (m_combinedTrack!=0 && m_combinedTrack->charge()!=m_combinedTrack->CHARGE_NA) {
		return true;
	}
	else {
		return false;
	}

}

bool TrigMuonEFInfoTrack::hasExtrapolatedTrack() const {
	if (m_extrapolatedTrack!=0 && m_extrapolatedTrack->charge()!=m_extrapolatedTrack->CHARGE_NA) {
		return true;
	}
	else {
		return false;
	}

}


//////////////////////////////////////////////////////////////////
// helper operators

std::string str ( const TrigMuonEFInfoTrack& d ) {
	std::stringstream ss;
	ss
	<< "; muon type: " << d.MuonType()
	<< "; spectrometer track: " << d.SpectrometerTrack()
	<< "; extrapolated track: " << d.ExtrapolatedTrack()
	<< "; combined track: " << d.CombinedTrack()
	<< "\nspectrometer track: " << *(d.SpectrometerTrack())
	<< "\nextrapolated track: " << *(d.ExtrapolatedTrack())
	<< "\ncombined track: " << *(d.CombinedTrack());
	return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigMuonEFInfoTrack& d ) {
	return ( m << str( d ) );
}

bool operator== ( const TrigMuonEFInfoTrack& a, const TrigMuonEFInfoTrack& b ) {

	if ( *(a.SpectrometerTrack()) != *(b.SpectrometerTrack()))
		return false;
	if ( *(a.ExtrapolatedTrack()) != *(b.ExtrapolatedTrack()))
		return false;
	if ( *(a.CombinedTrack()) != *(b.CombinedTrack()))
			return false;
	if ( a.MuonType() != b.MuonType())
			return false;

	return true;
}

void diff( const TrigMuonEFInfoTrack& a, const TrigMuonEFInfoTrack& b,
		std::map<std::string, double>& variableChange ) {
	diff( *( a.SpectrometerTrack() ), *( b.SpectrometerTrack() ), variableChange );
	diff( *( a.ExtrapolatedTrack() ), *( b.ExtrapolatedTrack() ), variableChange );
	diff( *( a.CombinedTrack() ), *( b.CombinedTrack() ), variableChange );
   if( a.MuonType() != b.MuonType() ) {
      variableChange["MuonType"] = a.MuonType() - b.MuonType();
   }
	return;
}
