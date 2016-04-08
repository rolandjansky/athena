/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TrigMuonEFInfo.cxx
Package : offline/Trigger/TrigEvent/TrigMuonEvent
Author  : Sergio Grancagnolo
Created : April 2008
Note    : Trigger container AOD for EF Muon (TrigMuonEFInfo)

DESCRIPTION:

Apr.2008: An AOD containing RoI index and track parameters for each
          muon trigger step.
 *****************************************************************************/

// STL include(s):
#include <sstream>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"

TrigMuonEFInfo::TrigMuonEFInfo( unsigned short int theRoI,
                                std::unique_ptr<TrigMuonEFInfoTrackContainer> tracks)
  : m_roi (theRoI),
    m_nSegments (0),
    m_nMdtHits (0),
    m_nRpcHits (0),
    m_nTgcHits (0),
    m_nCscHits (0),
    m_etaPreviousLevel (0),
    m_phiPreviousLevel (0),
    m_trackContainer (tracks.release())
{
	// these objects have to be created to guard
	// backward compatibilty for the TPCnv package.
	// they are not stored.
	m_spectrometerTrack = new TrigMuonEFTrack();
	m_extrapolatedTrack = new TrigMuonEFTrack();
	m_combinedTrack = new TrigMuonEFCbTrack();
}


TrigMuonEFInfo::TrigMuonEFInfo( unsigned short int theRoI,
                                std::unique_ptr<TrigMuonEFInfoTrackContainer> tracks,
                                std::unique_ptr<TrigMuonEFTrack> spectrometerTrack,
                                std::unique_ptr<TrigMuonEFTrack> extrapolatedTrack,
                                std::unique_ptr<TrigMuonEFCbTrack> combinedTrack)
  : m_roi (theRoI),
    m_nSegments (0),
    m_nMdtHits (0),
    m_nRpcHits (0),
    m_nTgcHits (0),
    m_nCscHits (0),
    m_etaPreviousLevel (0),
    m_phiPreviousLevel (0),
    m_trackContainer (tracks.release()),
    m_spectrometerTrack (spectrometerTrack.release()),
    m_extrapolatedTrack (extrapolatedTrack.release()),
    m_combinedTrack (combinedTrack.release())
{
}


TrigMuonEFInfo::TrigMuonEFInfo( unsigned short int theRoI )
{

	m_roi = theRoI;
	m_nSegments = 0;
	m_nMdtHits = 0;
	m_nRpcHits = 0;
	m_nCscHits = 0;
	m_nTgcHits = 0;
	m_etaPreviousLevel = 0.;
	m_phiPreviousLevel = 0.;

	m_trackContainer = new TrigMuonEFInfoTrackContainer();

	// these objects have to be created to guard
	// backward compatibilty for the TPCnv package.
	// they are not stored.
	m_spectrometerTrack = new TrigMuonEFTrack();
	m_extrapolatedTrack = new TrigMuonEFTrack();
	m_combinedTrack = new TrigMuonEFCbTrack();
}


TrigMuonEFInfo::TrigMuonEFInfo()
{
	m_roi = nRoINumDefault;
	m_nSegments = 0;
	m_nMdtHits = 0;
	m_nRpcHits = 0;
	m_nCscHits = 0;
	m_nTgcHits = 0;
	m_etaPreviousLevel = 0.;
	m_phiPreviousLevel = 0.;

	m_trackContainer = new TrigMuonEFInfoTrackContainer();

	// these objects have to be created to guard
	// backward compatibilty for the TPCnv package.
	// they are not stored.
	m_spectrometerTrack = new TrigMuonEFTrack();
	m_extrapolatedTrack = new TrigMuonEFTrack();
	m_combinedTrack = new TrigMuonEFCbTrack();
}

TrigMuonEFInfo::TrigMuonEFInfo( const TrigMuonEFInfo& rhs ) :
	m_roi( rhs.m_roi ),
	m_nSegments( rhs.m_nSegments ),
	m_nMdtHits( rhs.m_nMdtHits ),
	m_nRpcHits( rhs.m_nRpcHits ),
	m_nTgcHits( rhs.m_nTgcHits ),
	m_nCscHits( rhs.m_nCscHits ),
	m_etaPreviousLevel( rhs.m_etaPreviousLevel),
	m_phiPreviousLevel( rhs.m_phiPreviousLevel)
	{

	// deep copy of m_trackContainer
	m_trackContainer = new TrigMuonEFInfoTrackContainer();
	for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = rhs.m_trackContainer->begin() ;TrackItr!=rhs.m_trackContainer->end();TrackItr++)
	{  // loop over container content
		TrigMuonEFInfoTrack* infoTrackDest = new TrigMuonEFInfoTrack(**TrackItr); // clone elements
		m_trackContainer->push_back(infoTrackDest);  // add to container

	}

	// check for legacy tracks
	m_spectrometerTrack = 0;
	m_extrapolatedTrack = 0;
	m_combinedTrack = 0;
	if (rhs.m_spectrometerTrack)
		m_spectrometerTrack = new TrigMuonEFTrack( *rhs.m_spectrometerTrack ); // legacy
	if (rhs.m_extrapolatedTrack)
		m_extrapolatedTrack = new TrigMuonEFTrack( *rhs.m_extrapolatedTrack ); // legacy
	if (rhs.m_combinedTrack)
		m_combinedTrack = new TrigMuonEFCbTrack( *rhs.m_combinedTrack ); // legacy

}

TrigMuonEFInfo& TrigMuonEFInfo::operator=( const TrigMuonEFInfo& rhs )
{

	if ( this != &rhs ) {
		m_roi = rhs.m_roi;
		m_nSegments = rhs.m_nSegments;
		m_nMdtHits = rhs.m_nMdtHits;
		m_nRpcHits = rhs.m_nRpcHits;
		m_nTgcHits = rhs.m_nTgcHits;
		m_nCscHits = rhs.m_nCscHits;
		m_etaPreviousLevel = rhs.m_etaPreviousLevel;
		m_phiPreviousLevel = rhs.m_phiPreviousLevel;

		// clean up
		delete m_trackContainer;

		// deep copy of m_trackContainer
		m_trackContainer = new TrigMuonEFInfoTrackContainer();
	    for (TrigMuonEFInfoTrackContainer::const_iterator TrackItr = rhs.m_trackContainer->begin() ;TrackItr!=rhs.m_trackContainer->end();TrackItr++)
	    {  // loop over container content
	              TrigMuonEFInfoTrack* infoTrackDest = new TrigMuonEFInfoTrack(**TrackItr); // clone elements
	              m_trackContainer->push_back(infoTrackDest);  // add to container
	    }

		// check for legacy tracks
		if (rhs.m_spectrometerTrack) {
			delete m_spectrometerTrack;
			m_spectrometerTrack = new TrigMuonEFTrack( *rhs.m_spectrometerTrack ); // legacy
		}
		if (rhs.m_extrapolatedTrack) {
			delete m_extrapolatedTrack;
			m_extrapolatedTrack = new TrigMuonEFTrack( *rhs.m_extrapolatedTrack ); // legacy
		}
		if (rhs.m_combinedTrack) {
			delete m_combinedTrack;
			m_combinedTrack = new TrigMuonEFCbTrack( *rhs.m_combinedTrack ); // legacy
		}

	}
	return *this;
}

TrigMuonEFInfo& TrigMuonEFInfo::operator=( TrigMuonEFInfo&& rhs )
{

	if ( this != &rhs ) {
		m_roi = rhs.m_roi;
		m_nSegments = rhs.m_nSegments;
		m_nMdtHits = rhs.m_nMdtHits;
		m_nRpcHits = rhs.m_nRpcHits;
		m_nTgcHits = rhs.m_nTgcHits;
		m_nCscHits = rhs.m_nCscHits;
		m_etaPreviousLevel = rhs.m_etaPreviousLevel;
		m_phiPreviousLevel = rhs.m_phiPreviousLevel;

                std::swap (m_trackContainer, rhs.m_trackContainer);
                std::swap (m_spectrometerTrack, rhs.m_spectrometerTrack);
                std::swap (m_extrapolatedTrack, rhs.m_extrapolatedTrack);
                std::swap (m_combinedTrack, rhs.m_combinedTrack);

	}
	return *this;
}

TrigMuonEFInfo::~TrigMuonEFInfo()
{
	delete m_trackContainer;
	delete m_spectrometerTrack;
	delete m_extrapolatedTrack;
	delete m_combinedTrack;
}

void TrigMuonEFInfo::setRoINum( unsigned short int theRoINum )
{
	m_roi = theRoINum;
	return;
}

void TrigMuonEFInfo::setNSegments( unsigned short int theNSegments )
{
	m_nSegments = theNSegments;
	return;
}


void TrigMuonEFInfo::setNMdtHits( unsigned short int theNMdtHits )
{
	m_nMdtHits = theNMdtHits;
	return;
}

void TrigMuonEFInfo::setNRpcHits( unsigned short int theNRpcHits )
{
	m_nRpcHits = theNRpcHits;
	return;
}

void TrigMuonEFInfo::setNTgcHits( unsigned short int theNTgcHits )
{
	m_nTgcHits = theNTgcHits;
	return;
}

void TrigMuonEFInfo::setNCscHits( unsigned short int theNCscHits )
{
	m_nCscHits = theNCscHits;
	return;
}

void TrigMuonEFInfo::setEtaPreviousLevel( double theEtaPreviousLevel )
{
	m_etaPreviousLevel = theEtaPreviousLevel;
	return;
}

void TrigMuonEFInfo::setPhiPreviousLevel( double thePhiPreviousLevel )
{
	m_phiPreviousLevel = thePhiPreviousLevel;
	return;
}

void TrigMuonEFInfo::addTrack(TrigMuonEFInfoTrack* theTrack){

	m_trackContainer->push_back(theTrack);

	return;
}

void TrigMuonEFInfo::setSpectrometerTrack( TrigMuonEFTrack* theTrack )
{
	TrigMuonEFInfoTrack *track;
	if (!hasTrack()) {
		addTrack( new TrigMuonEFInfoTrack());
	}
	track =  (*(m_trackContainer->begin()));
	track->setSpectrometerTrack(theTrack);
	return;
}

TrigMuonEFTrack* TrigMuonEFInfo::SpectrometerTrack()
{
	if (!hasTrack() && !hasLegacyTrack()) {
		addTrack( new TrigMuonEFInfoTrack());
		return (*(m_trackContainer->begin()))->SpectrometerTrack();
	}
	else if (hasLegacyTrack()) {
		return(m_spectrometerTrack);
	}
	return (*(m_trackContainer->begin()))->SpectrometerTrack();
}

void TrigMuonEFInfo::setExtrapolatedTrack( TrigMuonEFTrack* theTrack )
{
	TrigMuonEFInfoTrack *track;
	if (!hasTrack()) {
		addTrack( new TrigMuonEFInfoTrack());
	}
	track =  (*(m_trackContainer->begin()));
	track->setExtrapolatedTrack(theTrack);
	return;
}

TrigMuonEFTrack* TrigMuonEFInfo::ExtrapolatedTrack()
{
	if (!hasTrack() && !hasLegacyTrack()) {
		addTrack( new TrigMuonEFInfoTrack());
		return (*(m_trackContainer->begin()))->ExtrapolatedTrack();
	}
	else if ( hasLegacyTrack()) {
		return(m_extrapolatedTrack);
	}
	return (*(m_trackContainer->begin()))->ExtrapolatedTrack();

}

void TrigMuonEFInfo::setCombinedTrack( TrigMuonEFCbTrack* theTrack )
{
	TrigMuonEFInfoTrack *track;
	if (!hasTrack()) {
		addTrack( new TrigMuonEFInfoTrack());
	}
	track =  (*(m_trackContainer->begin()));
	track->setCombinedTrack(theTrack);
	return;
}

TrigMuonEFCbTrack* TrigMuonEFInfo::CombinedTrack()
{
	if (!hasTrack() && !hasLegacyTrack()) {
		addTrack( new TrigMuonEFInfoTrack());
		return (*(m_trackContainer->begin()))->CombinedTrack();
	}
	else if ( hasLegacyTrack()) {
		return(m_combinedTrack);
	}
	return (*(m_trackContainer->begin()))->CombinedTrack();
}

bool TrigMuonEFInfo::hasTrack() const {
	// object, legacy tracks?
	if (!m_trackContainer) {
		return false;
	}
	// trackContainer empty?
	else if (m_trackContainer->size()>0) {
		return true;
	}
	else {
		return false;
	}
}

bool TrigMuonEFInfo::hasLegacyTrack() const {

	if ( m_spectrometerTrack==0
			|| m_extrapolatedTrack==0
			|| m_combinedTrack==0) {
		return false;
	}
	else if (
			m_spectrometerTrack->charge()!=m_spectrometerTrack->CHARGE_NA ||
			m_extrapolatedTrack->charge()!=m_extrapolatedTrack->CHARGE_NA ||
			m_combinedTrack->charge()!=m_combinedTrack->CHARGE_NA ||
			m_spectrometerTrack->m()!=0. || // legacy, charge wasn't stored until p2
			m_extrapolatedTrack->m()!=0. || // of TrigMuonEFTrackCnv
			m_combinedTrack->m()!=0.

	) {
		return true;
	}
	else {
		return false;
	}
}

// legacy method, should not be used anymore
bool TrigMuonEFInfo::hasSpectrometerTrack() const {
	return (hasLegacyTrack()||hasTrack());
}

// legacy method, should not be used anymore
bool TrigMuonEFInfo::hasCombinedTrack() const {
	return (hasLegacyTrack()||hasTrack());
}

// legacy method, should not be used anymore
bool TrigMuonEFInfo::hasExtrapolatedTrack() const {
	return (hasLegacyTrack()||hasTrack());
}

//////////////////////////////////////////////////////////////////
// helper functions

//////////////////////////////////////////////////////////////////
// helper operators

std::string str ( const TrigMuonEFInfo& d ) {
	std::stringstream ss;
	ss << "RoI index: "   << d.RoINum()
	<< "; NSegments: " << d.NSegments()
	<< "; NMdtHits: " << d.NMdtHits()
	<< "; NRpcHits: " << d.NRpcHits()
	<< "; NTgcHits: " << d.NTgcHits()
	<< "; NCscHits: " << d.NCscHits()
	<< "; hasTrack: " << d.hasTrack() << std::endl;

	if (d.hasTrack()) {

		// find match in containerB
                for (const TrigMuonEFInfoTrack* dd : *d.TrackContainer()) {
			if (dd) {

			ss << "; muon type: " << dd->MuonType();

			if (dd->hasSpectrometerTrack())
				ss << "; spectrometer track: " << dd->SpectrometerTrack()
				<< " : " << *(dd->SpectrometerTrack());

			if (dd->hasExtrapolatedTrack())
				ss << "; extrapolated track: " << dd->ExtrapolatedTrack()
				<< " : " << *(dd->ExtrapolatedTrack());

			if (dd->hasCombinedTrack())
				ss << "; combined track: " << dd->CombinedTrack()
				<< " : " << *(dd->CombinedTrack());
			}
		}
	}
	return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigMuonEFInfo& d ) {

	return ( m << str( d ) );
}

bool operator== ( const TrigMuonEFInfo& a, const TrigMuonEFInfo& b ) {

	if ( a.RoINum() != b.RoINum() )
		return false;
	if ( a.hasTrack() != b.hasTrack() )
		return false;
	if (a.hasTrack() && b.hasTrack() ) {
		// simple check for the number of entries in container:
		if (
				( a.TrackContainer()->begin() - a.TrackContainer()->end())
				!= ( b.TrackContainer()->begin() - b.TrackContainer()->end())
		)
		{
			return false;
		}
		// sort container?
		// unfortunately sort() of DataVector just sorts by pointer value,
		// so really useless here....
		// policy: make a copy of container B (tmpContB) and loop over container A
		// for every match rm element from tmpContB.
		// at the end check whether tmpContB is empty -> condition for equality

		// make a view copy of container B to rm later on elements which matched to
		// elements of container A so we don't modify container B
		TrigMuonEFInfoTrackContainer *tmpContB = new TrigMuonEFInfoTrackContainer(SG::VIEW_ELEMENTS);
		*tmpContB = *(b.TrackContainer());
		for (
				TrigMuonEFInfoTrackContainer::const_iterator TrkItrA = a.TrackContainer()->begin();
				TrkItrA != a.TrackContainer()->end();
				TrkItrA++
		)
		{
			// find match in containerB
			TrigMuonEFInfoTrackContainer::iterator TrkItrB = tmpContB->begin();
			while ( TrkItrB != tmpContB->end() ) {
				if (*(*TrkItrB)==*(*TrkItrA)) {
					// if match rm element from tmp container and break loop
					tmpContB->erase(TrkItrB);
					break;
				}
				TrkItrB++;
			}
		}

		// if all elements were matched tmp container must be empty
		bool result = tmpContB->empty();
		// delete tmp container
		delete tmpContB;
		// check result
		if(!result) return false;
	}
	if (a.hasLegacyTrack() && b.hasLegacyTrack()) {
		if( !(*( a.legacySpectrometerTrack() ) == *( b.legacySpectrometerTrack() ) ) )
			return false;
		if( !(*( a.legacyExtrapolatedTrack() ) == *( b.legacyExtrapolatedTrack() ) ) )
			return false;
		if( !(*( a.legacyCombinedTrack() ) == *( b.legacyCombinedTrack() ) ) )
			return false;

	}
	return true;
}

void diff( const TrigMuonEFInfo& a, const TrigMuonEFInfo& b,
		std::map<std::string, double>& variableChange ) {

	if( a.RoINum() != b.RoINum() ) {
		variableChange[ "RoINum" ] = static_cast< double >( a.RoINum() - b.RoINum() );
	}
	if( a.NSegments() != b.NSegments() ) {
		variableChange[ "NSegments" ] = static_cast< double >( a.NSegments() -
				b.NSegments() );
	}
	if( a.NMdtHits() != b.NMdtHits() ) {
		variableChange[ "NMdtHits" ] = static_cast< double >( a.NMdtHits() -
				b.NMdtHits() );
	}
	if( a.NRpcHits() != b.NRpcHits() ) {
		variableChange[ "NRpcHits" ] = static_cast< double >( a.NRpcHits() -
				b.NRpcHits() );
	}
	if( a.NCscHits() != b.NCscHits() ) {
		variableChange[ "NCscHits" ] = static_cast< double >( a.NCscHits() -
				b.NCscHits() );
	}
	if( a.NTgcHits() != b.NTgcHits() ) {
		variableChange[ "NTgcHits" ] = static_cast< double >( a.NTgcHits() -
				b.NTgcHits() );
	}
	if( a.EtaPreviousLevel() != b.EtaPreviousLevel() ) {
		variableChange[ "EtaPreviousLevel" ] = static_cast< double >( a.EtaPreviousLevel() -
				b.EtaPreviousLevel() );
	}
	if( a.PhiPreviousLevel() != b.PhiPreviousLevel() ) {
		variableChange[ "PhiPreviousLevel" ] = static_cast< double >( a.PhiPreviousLevel() -
				b.PhiPreviousLevel() );
	}
	// simple check on number of tracks in container
	if (
			(a.hasTrack() && b.hasTrack())
			&&
			(a.TrackContainer()->size() != b.TrackContainer()->size())
	) {
		variableChange[ "NTracks" ] = static_cast< double >(a.TrackContainer()->size() -
				b.TrackContainer()->size() );
	}
	// make a view copy of container B to rm later on elements which matched to
	// elements of container A so we don't modify container B
	TrigMuonEFInfoTrackContainer *tmpContB = new TrigMuonEFInfoTrackContainer(SG::VIEW_ELEMENTS);
	*tmpContB = *(b.TrackContainer());

	for (
			TrigMuonEFInfoTrackContainer::const_iterator TrkItrA = a.TrackContainer()->begin();
			TrkItrA != a.TrackContainer()->end();
			TrkItrA++
	)
	{
		// find match in containerB
		TrigMuonEFInfoTrackContainer::iterator TrkItrB = tmpContB->begin();
		while ( TrkItrB != tmpContB->end() ) {
			
			if (*(*TrkItrB)==*(*TrkItrA)) {
				// if match rm element from tmp container, diff, and break loop
				diff( *( (*TrkItrA)->SpectrometerTrack() ), *( (*TrkItrB)->SpectrometerTrack() ), variableChange );
				diff( *( (*TrkItrA)->ExtrapolatedTrack() ), *( (*TrkItrB)->ExtrapolatedTrack() ), variableChange );
				diff( *( (*TrkItrA)->CombinedTrack() ), *( (*TrkItrB)->CombinedTrack() ), variableChange );
				tmpContB->erase(TrkItrB);
				break;
			}
			TrkItrB++;
		}
	}
	// clean up
	delete tmpContB;

	if (a.hasLegacyTrack() && b.hasLegacyTrack()) {
		diff( *( a.legacySpectrometerTrack() ), *( b.legacySpectrometerTrack() ), variableChange );
		diff( *( a.legacyExtrapolatedTrack() ), *( b.legacyExtrapolatedTrack() ), variableChange );
		diff( *( a.legacyCombinedTrack() ), *( b.legacyCombinedTrack() ), variableChange );
	}

	return;
}
