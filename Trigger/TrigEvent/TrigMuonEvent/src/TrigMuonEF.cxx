/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
Name    : TrigMuonEF.cxx
Package : offline/Trigger/TrigEvent/TrigMuonEvent
Author  : Gabriella Cataldi
Created : November 2006 
Note    : Simple Trigger AOD for EF Muon (TrigMoore)

DESCRIPTION:

Nov.2006: A simple AOD containing just track parameters and status for re-running 
          hypothesis with FEX off. 
*****************************************************************************/

// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonEF.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

TrigMuonEF::TrigMuonEF() :
  P4IPtCotThPhiM( 0., 0., 0., 0. ),
  m_muonCode(0){
}

TrigMuonEF::TrigMuonEF(double theIPt, double theCotTh, double thePhi, double theM) :
  P4IPtCotThPhiM(theIPt , theCotTh, thePhi ,theM ),
  m_muonCode(0){
}

TrigMuonEF::TrigMuonEF( const TrigMuonEF& rhs ) :
  I4Momentum(rhs),
  P4IPtCotThPhiMBase( rhs ),
  INavigable(rhs),
  IAthenaBarCode(rhs),
  INavigable4Momentum(rhs),
  P4IPtCotThPhiM( rhs ),
  NavigableTerminalNode(),
  m_muonCode( rhs.m_muonCode )
{ }

/** Assignement operator */
TrigMuonEF& TrigMuonEF::operator=( const TrigMuonEF& rhs )
{
  if ( this != &rhs ) {
    P4IPtCotThPhiM::operator=( rhs );
    m_muonCode                    = rhs.m_muonCode;
  }
  return *this;
}

/** set data member */
void TrigMuonEF::set_muonCode(int theMuonCode){
  m_muonCode = theMuonCode; 
}

/** set data member */
void TrigMuonEF::set_RoINum(std::string theRoINum){
  m_roi = theRoINum; 
}

/** set data member */
void TrigMuonEF::set_Charge(double theCharge){
  m_charge = theCharge; 
}

/** Destructor */
TrigMuonEF::~TrigMuonEF() {}

//
// I use the standard eta-phi-pt parameters in the printouts and comparisons,
// because iPt and CotTh don't tell me much on first sight. Besides, iPt values
// are usually too small for a meaningful direct comparison.
//

std::string str( const TrigMuonEF& d ) {

   std::stringstream stream;
   stream << "MuonCode: " << d.MuonCode()
          << "; RoINum: " << d.RoINum()
          << "; eta: " << d.eta()
          << "; phi: " << d.phi()
          << "; pt: " << d.pt()
          << "; charge: " << d.charge();

   return stream.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigMuonEF& d ) {

   return ( m << str( d ) );

}

bool operator== ( const TrigMuonEF& a, const TrigMuonEF& b ) {

   if( a.MuonCode() != b.MuonCode() )
      return false;
   if( std::abs( a.charge() - b.charge() ) > DELTA )
      return false;
   if( std::abs( a.pt() - b.pt() ) > DELTA )
      return false;
   if( std::abs( a.eta() - b.eta() ) > DELTA )
      return false;
   if( std::abs( a.phi() - b.phi() ) > DELTA )
      return false;
   if( std::abs( a.m() - b.m() ) > DELTA )
      return false;

   return true;
}

void diff( const TrigMuonEF& a, const TrigMuonEF& b,
           std::map< std::string, double >& variableChange ) {

   if( a.MuonCode() != b.MuonCode() ) {
      variableChange[ "MuonCode" ] = static_cast< double >( a.MuonCode() - b.MuonCode() );
   }
   if( std::abs( a.charge() - b.charge() ) > DELTA ) {
      variableChange[ "Charge" ] = a.charge() - b.charge();
   }
   if( std::abs( a.pt() - b.pt() ) > DELTA ) {
      variableChange[ "Pt" ] = a.pt() - b.pt(); 
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

   return;

}
