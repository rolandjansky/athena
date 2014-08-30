/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TrigMuonEFTrack.cxx
Package : offline/Trigger/TrigEvent/TrigMuonEvent
Author  : Sergio Grancagnolo
Created : April 2008
Note    : Simple Trigger AOD for EF Muon (TrigMuonEF)

DESCRIPTION:

Apr.2008: A simple AOD containing just track parameters for re-running
          hypothesis with FEX off.
 *****************************************************************************/

// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonEFTrack.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;
// "Distance" used for 1/pT comparison (since we are in MeV)
static const double IDELTA = 0.00000001;

TrigMuonEFTrack::TrigMuonEFTrack() :
	P4IPtCotThPhiM( 0., 0., 0., 0. ),
	m_charge (CHARGE_NA),
	m_d0 (0),
	m_z0 (0),
	m_chi2 (0),
	m_chi2prob (0),
	m_posx (0),
	m_posy (0),
	m_posz (0),
	m_nMdtHitsPhi (0),
	m_nRpcHitsPhi (0),
	m_nTgcHitsPhi (0),
	m_nCscHitsPhi (0),
	m_nMdtHitsEta (0),
	m_nRpcHitsEta (0),
	m_nTgcHitsEta (0),
	m_nCscHitsEta (0),
    m_nMdtHits (0)
	{
	}

TrigMuonEFTrack::TrigMuonEFTrack(double theIPt, double theCotTh, double thePhi, double theM) :
	P4IPtCotThPhiM(theIPt, theCotTh, thePhi ,theM ),
	m_charge (CHARGE_NA),
	m_d0 (0),
	m_z0 (0),
	m_chi2 (0),
	m_chi2prob (0),
	m_posx (0),
	m_posy (0),
	m_posz (0),
	m_nMdtHitsPhi (0),
	m_nRpcHitsPhi (0),
	m_nTgcHitsPhi (0),
	m_nCscHitsPhi (0),
	m_nMdtHitsEta (0),
	m_nRpcHitsEta (0),
	m_nTgcHitsEta (0),
	m_nCscHitsEta (0),
    m_nMdtHits (0)
	{
	}

TrigMuonEFTrack::TrigMuonEFTrack( const TrigMuonEFTrack& rhs ) :
	I4Momentum(rhs),
	P4IPtCotThPhiMBase( rhs ),
	INavigable(rhs),
	IAthenaBarCode(),
	INavigable4Momentum(rhs),
	P4IPtCotThPhiM( rhs ),
	NavigableTerminalNode(),
	m_charge( rhs.m_charge ),
	m_d0 (rhs.m_d0),
	m_z0 (rhs.m_z0),
	m_chi2 (rhs.m_chi2),
	m_chi2prob (rhs.m_chi2prob),
	m_posx (rhs.m_posx),
	m_posy (rhs.m_posy),
	m_posz (rhs.m_posz),
	m_nMdtHitsPhi (rhs.m_nMdtHitsPhi),
	m_nRpcHitsPhi (rhs.m_nRpcHitsPhi),
	m_nTgcHitsPhi (rhs.m_nTgcHitsPhi),
	m_nCscHitsPhi (rhs.m_nCscHitsPhi),
	m_nMdtHitsEta (rhs.m_nMdtHitsEta),
	m_nRpcHitsEta (rhs.m_nRpcHitsEta),
	m_nTgcHitsEta (rhs.m_nTgcHitsEta),
	m_nCscHitsEta (rhs.m_nCscHitsEta),
    m_nMdtHits (0)
	{ }

/** Assignement operator */
TrigMuonEFTrack& TrigMuonEFTrack::operator=( const TrigMuonEFTrack& rhs )
{
	if ( this != &rhs ) {
		P4IPtCotThPhiM::operator=( rhs );
		m_charge = rhs.m_charge;
		m_d0 = rhs.m_d0;
		m_z0 = rhs.m_z0;
		m_chi2 = rhs.m_chi2;
		m_chi2prob = rhs.m_chi2prob;
		m_posx = rhs.m_posx;
		m_posy = rhs.m_posy;
		m_posz = rhs.m_posz;
		m_nMdtHitsPhi = rhs.m_nMdtHitsPhi;
		m_nRpcHitsPhi = rhs.m_nRpcHitsPhi;
		m_nTgcHitsPhi = rhs.m_nTgcHitsPhi;
		m_nCscHitsPhi = rhs.m_nCscHitsPhi;
		m_nMdtHitsEta = rhs.m_nMdtHitsEta;
		m_nRpcHitsEta = rhs.m_nRpcHitsEta;
		m_nTgcHitsEta = rhs.m_nTgcHitsEta;
		m_nCscHitsEta = rhs.m_nCscHitsEta;
		m_nMdtHits = rhs.m_nMdtHits;
	}
	return *this;
}

/** set data member */
void TrigMuonEFTrack::setPerigee(double iPt, double cotTh, double phi, double mass)
{
	m_iPt   = iPt;
	m_cotTh = cotTh;
	m_phi   = phi;
	m_m  = mass;
	return;
}

/** Destructor */
TrigMuonEFTrack::~TrigMuonEFTrack() {}

//////////////////////////////////////////////////////////////////
// helper operators

std::string str ( const TrigMuonEFTrack& d ) {
	std::stringstream ss;
	ss << " inverse pt: " << d.iPt()
	<< "; cot(theta): " << d.cotTh()
	<< "; phi: " << d.phi()
	<< "; mass: " << d.m()
	<< "; charge: " << d.charge()
	<< "; NCscHitsPhi: " << d.NCscHitsPhi()
	<< "; NCscHitsEta: " << d.NCscHitsEta()
	<< "; NTgcHitsPhi: " << d.NTgcHitsPhi()
	<< "; NTgcHitsEta: " << d.NTgcHitsEta()
	<< "; NRpcHitsPhi: " << d.NRpcHitsPhi()
	<< "; NRpcHitsEta: " << d.NRpcHitsEta()
	<< "; NMdtHits: " << d.NMdtHits();

	return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigMuonEFTrack& d ) {

	return ( m << str( d ) );
}

bool operator== ( const TrigMuonEFTrack& a, const TrigMuonEFTrack& b ) {

	if( std::abs( a.charge() - b.charge() ) > DELTA )
		return false;
	if( std::abs( a.iPt() - b.iPt() ) > IDELTA )
		return false;
	if( std::abs( a.eta() - b.eta() ) > DELTA )
		return false;
	if( std::abs( a.phi() - b.phi() ) > DELTA )
		return false;
	if( std::abs( a.m() - b.m() ) > DELTA )
		return false;
   if( std::abs( a.d0() - b.d0() ) > DELTA )
		return false;
   if( std::abs( a.z0() - b.z0() ) > DELTA )
		return false;
   if( std::abs( a.chi2() - b.chi2() ) > DELTA )
		return false;
   if( std::abs( a.chi2prob() - b.chi2prob() ) > DELTA )
		return false;
   if( std::abs( a.posX() - b.posX() ) > DELTA )
		return false;
   if( std::abs( a.posY() - b.posY() ) > DELTA )
		return false;
   if( std::abs( a.posZ() - b.posZ() ) > DELTA )
		return false;
   if( a.NRpcHitsPhi() != b.NRpcHitsPhi() )
      return false;
   if( a.NTgcHitsPhi() != b.NTgcHitsPhi() )
      return false;
   if( a.NCscHitsPhi() != b.NCscHitsPhi() )
      return false;
   if( a.NRpcHitsEta() != b.NRpcHitsEta() )
      return false;
   if( a.NTgcHitsEta() != b.NTgcHitsEta() )
      return false;
   if( a.NCscHitsEta() != b.NCscHitsEta() )
      return false;
   if( a.NMdtHits() != b.NMdtHits() )
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
void diff( const TrigMuonEFTrack& a, const TrigMuonEFTrack& b,
		std::map< std::string, double >& variableChange ) {

	if( std::abs( a.charge() - b.charge() ) > DELTA ) {
		variableChange[ "Charge" ] = a.charge() - b.charge();
	}
	if( std::abs( a.iPt() - b.iPt() ) > IDELTA ) {
		variableChange[ "iPt" ] = a.iPt() - b.iPt();
	}
	if( std::abs( a.eta() - b.eta() ) > DELTA ) {
		variableChange[ "Eta" ] = a.eta() - b.eta();
	}
	if( std::abs( a.phi() - b.phi() ) > DELTA ) {
		variableChange[ "Phi" ] = a.phi() - b.phi();
	}
	if( std::abs( a.m() - b.m() ) > DELTA ) {
		variableChange[ "Mass" ] = a.m() - b.m();
	}
   if( std::abs( a.d0() - b.d0() ) > DELTA ) {
		variableChange[ "d0" ] = a.d0() - b.d0();
   }
   if( std::abs( a.z0() - b.z0() ) > DELTA ) {
		variableChange[ "z0" ] = a.z0() - b.z0();
   }
   if( std::abs( a.chi2() - b.chi2() ) > DELTA ) {
		variableChange[ "chi2" ] = a.chi2() - b.chi2();
   }
   if( std::abs( a.chi2prob() - b.chi2prob() ) > DELTA ) {
		variableChange[ "chi2prob" ] = a.chi2prob() - b.chi2prob();
   }
   if( std::abs( a.posX() - b.posX() ) > DELTA ) {
		variableChange[ "posX" ] = a.posX() - b.posX();
   }
   if( std::abs( a.posY() - b.posY() ) > DELTA ) {
		variableChange[ "posY" ] = a.posY() - b.posY();
   }
   if( std::abs( a.posZ() - b.posZ() ) > DELTA ) {
		variableChange[ "posZ" ] = a.posZ() - b.posZ();
   }
   if( a.NRpcHitsPhi() != b.NRpcHitsPhi() ) {
		variableChange[ "NRpcHitsPhi" ] = static_cast< double >( a.NRpcHitsPhi() -
                                                               b.NRpcHitsPhi() );
   }
   if( a.NTgcHitsPhi() != b.NTgcHitsPhi() ) {
		variableChange[ "NTgcHitsPhi" ] = static_cast< double >( a.NTgcHitsPhi() -
                                                               b.NTgcHitsPhi() );
   }
   if( a.NCscHitsPhi() != b.NCscHitsPhi() ) {
		variableChange[ "NCscHitsPhi" ] = static_cast< double >( a.NCscHitsPhi() -
                                                               b.NCscHitsPhi() );
   }
   if( a.NRpcHitsEta() != b.NRpcHitsEta() ) {
		variableChange[ "NRpcHitsEta" ] = static_cast< double >( a.NRpcHitsEta() -
                                                               b.NRpcHitsEta() );
   }
   if( a.NTgcHitsEta() != b.NTgcHitsEta() ) {
		variableChange[ "NTgcHitsEta" ] = static_cast< double >( a.NTgcHitsEta() -
                                                               b.NTgcHitsEta() );
   }
   if( a.NCscHitsEta() != b.NCscHitsEta() ) {
		variableChange[ "NCscHitsEta" ] = static_cast< double >( a.NCscHitsEta() -
                                                               b.NCscHitsEta() );
   }
   if( a.NMdtHits() != b.NMdtHits() ) {
		variableChange[ "NMdtHits" ] = static_cast< double >( a.NMdtHits() -
                                                            b.NMdtHits() );
   }

	return;
}
