/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************

NAME:           TrigMuonClusterFeature.cxx
PACKAGE:        Trigger/TrigEvent/TrigMuonEvent
AUTHORS:        Antonio Policicchio <Antonio.Poliicchio@cern.ch>
AUTHORS:        Stefano Giagu <stefano.giagu@cern.ch>
PURPOSE:        Keep the important output variables
                from the muon ROI cluster trigger
DATE:           V1.0 January 13th, 2009

******************************************************/
// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/TrigMuonClusterFeature.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

TrigMuonClusterFeature::TrigMuonClusterFeature() : P4PtEtaPhiMBase(), NavigableTerminalNode(),
m_eta(0.),
m_phi(0.),
m_nroi(0),
m_njet(0),
m_ntrk(0)
{}

TrigMuonClusterFeature::TrigMuonClusterFeature	(float eta, float phi, int nroi, int njet, int ntrk) : P4PtEtaPhiMBase(), NavigableTerminalNode(),
  m_eta (eta),
  m_phi (phi),
  m_nroi (nroi),
  m_njet (njet),
  m_ntrk (ntrk)
{}
  
TrigMuonClusterFeature::~TrigMuonClusterFeature() {}

//Assignment operator
TrigMuonClusterFeature&
TrigMuonClusterFeature::operator=(const TrigMuonClusterFeature& mfeature) {
  if (this != &mfeature) {
    I4Momentum::operator=( mfeature );
    INavigable::operator=( mfeature );
    INavigable4Momentum::operator=( mfeature );
    P4PtEtaPhiMBase::operator= ( mfeature );
    NavigableTerminalNode::operator= ( mfeature );
    m_eta = mfeature.getEta();
    m_phi = mfeature.getPhi();
    m_nroi = mfeature.getNRoi();
    m_njet = mfeature.getNJet();
    m_ntrk = mfeature.getNTRK();
  }
  return *this;
}

// Copy constructor (Note that also the base class is copied)
TrigMuonClusterFeature::TrigMuonClusterFeature( const TrigMuonClusterFeature* feat ) :
  I4Momentum( *feat ),
  INavigable( *feat ),
  INavigable4Momentum( *feat ),
  P4PtEtaPhiMBase ( *feat ),
  NavigableTerminalNode ( *feat ),
  m_eta( feat->getEta() ),
  m_phi( feat->getPhi() ),
  m_nroi( feat->getNRoi() ),
  m_njet( feat->getNJet() ),
  m_ntrk( feat->getNTRK() )
{}


TrigMuonClusterFeature::TrigMuonClusterFeature( const TrigMuonClusterFeature& feat ) :
  I4Momentum( feat ),
  INavigable( feat ),
  IAthenaBarCode( feat ),
  INavigable4Momentum( feat ),
  P4PtEtaPhiMBase ( feat ),
  NavigableTerminalNode ( feat ),
  m_eta    ( feat.getEta()),
  m_phi    ( feat.getPhi()),
  m_nroi( feat.getNRoi() ),
  m_njet( feat.getNJet() ),
  m_ntrk( feat.getNTRK() ) {}

//////////////////////////////////////////////////////////////////
// helper operators

std::string str ( const TrigMuonClusterFeature& d ) {
  std::stringstream ss;
  ss << "  Eta:     " << d.getEta()
     << "; Phi:     " << d.getPhi()
     << "; NRoI:    " << d.getNRoi()
     << "; NJET:    " << d.getNJet()
     << "; NTRK:    " << d.getNTRK();
  return ss.str();
} 
MsgStream& operator<< ( MsgStream& m, const TrigMuonClusterFeature& d ) {
  return (m << str(d));
}

bool operator== ( const TrigMuonClusterFeature& a, const TrigMuonClusterFeature& b ) {
  if ( std::abs(a.getEta() - b.getEta()) > DELTA )    return false;
  if ( std::abs(a.getPhi() - b.getPhi()) > DELTA )    return false;
  if ( a.getNRoi()   != b.getNRoi() )   return false;
  if ( a.getNJet()   != b.getNJet() )   return false;
  if ( a.getNTRK()   != b.getNTRK() )   return false;
  return true;
}
 
void diff( const TrigMuonClusterFeature& a, const TrigMuonClusterFeature& b,
           std::map<std::string, double>& variableChange ) {

   if( std::abs( a.getEta() - b.getEta() ) > DELTA ) {
      variableChange[ "Eta" ] = a.getEta() - b.getEta();
   }
   if( std::abs( a.getPhi() - b.getPhi() ) > DELTA ) {
      variableChange[ "Phi" ] = a.getPhi() - b.getPhi();
   }
   if( a.getNRoi() != b.getNRoi() ) {
      variableChange[ "NRoI" ] = static_cast< double >( a.getNRoi() - b.getNRoi() );
   }
   if( a.getNJet() != b.getNJet() ) {
      variableChange[ "NJet" ] = static_cast< double >( a.getNJet() - b.getNJet() );
   }
   if( a.getNTRK() != b.getNTRK() ) {
      variableChange[ "NTRK" ] = static_cast< double >( a.getNTRK() - b.getNTRK() );
   }

   return;
}


