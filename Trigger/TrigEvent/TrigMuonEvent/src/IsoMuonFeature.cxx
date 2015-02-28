/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************

NAME:           IsoMuonFeature.cxx
PACKAGE:        Trigger/TrigEvent/TrigMuonEvent
AUTHORS:        Stefano Giagu <stefano.giagu@cern.ch>
PURPOSE:        Keep the important output variables
                from the muon isolation trigger.
DATE:           V3.0 January 28th, 2008

******************************************************/

// STL include(s):
#include <sstream>
#include <cmath>

// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "TrigMuonEvent/IsoMuonFeature.h"

// "Distance" used by the comparison operator(s):
static const double DELTA = 0.001;

IsoMuonFeature::IsoMuonFeature() : P4PtEtaPhiMBase(), NavigableTerminalNode(),
    m_sumet01(0.0),
    m_sumet02(0.0),
    m_sumet03(0.0),
    m_sumet04(0.0),
    m_sumpt01(0.0),
    m_sumpt02(0.0),
    m_sumpt03(0.0),
    m_sumpt04(0.0),
    m_PtMuID(0.0),
    m_MaxPtID(0.0),
    m_flag(0),
    m_RoiIdMu(0),
    m_PtMu(0.0),
    m_QMu(0.0),
    m_EtaMu(0.0),
    m_PhiMu(0.0),
    m_EtInnerConeEC(0.0),
    m_EtOuterConeEC(0.0),
    m_EtInnerConeHC(0.0),
    m_EtOuterConeHC(0.0),
    m_NTracksCone(0),
    m_SumPtTracksCone(0.0),
    m_PtMuTracksCone(0.0),
    m_LAr_w(0.0),
    m_Tile_w(0.0) {}

IsoMuonFeature::~IsoMuonFeature() {}

//Assignment operator
IsoMuonFeature&
IsoMuonFeature::operator=(const IsoMuonFeature& mfeature) {
    if (this != &mfeature) {
      m_sumet01    = mfeature.getsumet01();
      m_sumet02    = mfeature.getsumet02();
      m_sumet03    = mfeature.getsumet03();
      m_sumet04    = mfeature.getsumet04();
      m_sumpt01    = mfeature.getsumpt01();
      m_sumpt02    = mfeature.getsumpt02();
      m_sumpt03    = mfeature.getsumpt03();
      m_sumpt04    = mfeature.getsumpt04();
      m_PtMuID     = mfeature.getPtMuID();
      m_MaxPtID    = mfeature.getMaxPtID();
      m_flag       = mfeature.getFlag();
      m_RoiIdMu    = mfeature.getRoiIdMu();
      m_PtMu       = mfeature.pt();
      m_QMu        = mfeature.getQMu();
      m_EtaMu      = mfeature.getEtaMu();
      m_PhiMu      = mfeature.getPhiMu();
    }
    return *this;    
}

int IsoMuonFeature::iso_strategy(void)            const {

   if (m_NTracksCone>0) return m_NTracksCone/100000;
   else {
     int a = static_cast<int>(m_flag/10000);
     return a;
   }
}

int IsoMuonFeature::iso_errorFlag(void)           const {
   int a = static_cast<int>(m_flag/10000);
   int b = static_cast<int>(m_flag-a*10000);
   return b;
}

// Copy constructor (Note that also the base class is copied)
IsoMuonFeature::IsoMuonFeature( const IsoMuonFeature* feat ) :
                 I4Momentum( *feat ),
                 INavigable( *feat ),
                 INavigable4Momentum( *feat ),
                 P4PtEtaPhiMBase ( *feat ),
                 NavigableTerminalNode ( ),
                 m_sumet01   ( feat->getsumet01() ),
                 m_sumet02   ( feat->getsumet02() ),
                 m_sumet03   ( feat->getsumet03() ),
                 m_sumet04   ( feat->getsumet04() ),
                 m_sumpt01   ( feat->getsumpt01() ),
                 m_sumpt02   ( feat->getsumpt02() ),
                 m_sumpt03   ( feat->getsumpt03() ),
                 m_sumpt04   ( feat->getsumpt04() ),
                 m_PtMuID    ( feat->getPtMuID() ),
                 m_MaxPtID   ( feat->getMaxPtID() ),
                 m_flag      ( feat->getFlag() ),
                 m_RoiIdMu   ( feat->getRoiIdMu() ),
                 m_PtMu      ( feat->pt() ),
                 m_QMu       ( feat->getQMu() ),
                 m_EtaMu     ( feat->getEtaMu() ),
                 m_PhiMu     ( feat->getPhiMu() ) {}

IsoMuonFeature::IsoMuonFeature( const IsoMuonFeature& feat ) :
                 I4Momentum( feat ),
                 INavigable( feat ),
                 IAthenaBarCode( feat ),
                 INavigable4Momentum( feat ),
                 P4PtEtaPhiMBase ( feat ),
                 NavigableTerminalNode ( ),
                 m_sumet01   ( feat.getsumet01() ),
                 m_sumet02   ( feat.getsumet02() ),
                 m_sumet03   ( feat.getsumet03() ),
                 m_sumet04   ( feat.getsumet04() ),
                 m_sumpt01   ( feat.getsumpt01() ),
                 m_sumpt02   ( feat.getsumpt02() ),
                 m_sumpt03   ( feat.getsumpt03() ),
                 m_sumpt04   ( feat.getsumpt04() ),
                 m_PtMuID    ( feat.getPtMuID() ),
                 m_MaxPtID   ( feat.getMaxPtID() ),
                 m_flag      ( feat.getFlag() ),
                 m_RoiIdMu   ( feat.getRoiIdMu() ),
                 m_PtMu      ( feat.pt() ),
                 m_QMu       ( feat.getQMu() ),
                 m_EtaMu     ( feat.getEtaMu() ),
                 m_PhiMu     ( feat.getPhiMu() ) {}

void IsoMuonFeature::print (void) {
 std::cout << std::endl;
 std::cout << "m_sumet01:          " << m_sumet01    << std::endl;
 std::cout << "m_sumet02:          " << m_sumet02    << std::endl;
 std::cout << "m_sumet03:          " << m_sumet03    << std::endl;
 std::cout << "m_sumet04:          " << m_sumet04    << std::endl;
 std::cout << "m_sumpt01:          " << m_sumpt01    << std::endl;
 std::cout << "m_sumpt02:          " << m_sumpt02    << std::endl;
 std::cout << "m_sumpt03:          " << m_sumpt03    << std::endl;
 std::cout << "m_sumpt04:          " << m_sumpt04    << std::endl;
 std::cout << "m_PtMuID:           " << m_PtMuID     << std::endl;
 std::cout << "m_MaxPtID:          " << m_MaxPtID    << std::endl;
 std::cout << "m_flag:             " << m_flag       << std::endl;
 std::cout << "m_RoiIdMu:          " << m_RoiIdMu    << std::endl;
 std::cout << "m_PtMu:             " << m_PtMu       << std::endl;
 std::cout << "m_QMu:              " << m_QMu        << std::endl;
 std::cout << "m_EtaMu:            " << m_EtaMu      << std::endl;
 std::cout << "m_PhiMu:            " << m_PhiMu      << std::endl;
 std::cout << std::endl;
}

void IsoMuonFeature::print ( MsgStream& log ) {
 log << MSG::DEBUG  << "m_sumet01:          " << m_sumet01    << "; ";
 log << MSG::DEBUG  << "m_sumet02:          " << m_sumet02    << "; ";
 log << MSG::DEBUG  << "m_sumet03:          " << m_sumet03    << "; ";
 log << MSG::DEBUG  << "m_sumet04:          " << m_sumet04    << "; ";
 log << MSG::DEBUG  << "m_sumpt01:          " << m_sumpt01    << "; ";
 log << MSG::DEBUG  << "m_sumpt02:          " << m_sumpt02    << "; ";
 log << MSG::DEBUG  << "m_sumpt03:          " << m_sumpt03    << "; ";
 log << MSG::DEBUG  << "m_sumpt04:          " << m_sumpt04    << "; ";
 log << MSG::DEBUG  << "m_PtMuID:           " << m_PtMuID     << "; ";
 log << MSG::DEBUG  << "m_MaxPtID:          " << m_MaxPtID    << "; ";
 log << MSG::DEBUG  << "m_flag:             " << m_flag       << "; ";
 log << MSG::DEBUG  << "m_RoiIdMu:          " << m_RoiIdMu    << "; ";
 log << MSG::DEBUG  << "m_PtMu:             " << m_PtMu       << "; ";
 log << MSG::DEBUG  << "m_QMu:              " << m_QMu        << "; ";
 log << MSG::DEBUG  << "m_EtaMu:            " << m_EtaMu      << "; ";
 log << MSG::DEBUG  << "m_PhiMu:            " << m_PhiMu      << "; ";
 log << endreq;
}

//////////////////////////////////////////////////////////////////
// helper operators

std::string str( const IsoMuonFeature& d ) {
   std::stringstream ss;
   ss << "sumet01: " << d.getsumet01()
      << "; sumet02: " << d.getsumet02()
      << "; sumet03: " << d.getsumet03()
      << "; sumet04: " << d.getsumet04()
      << "; sumpt01: " << d.getsumpt01()
      << "; sumpt02: " << d.getsumpt02()
      << "; sumpt03: " << d.getsumpt03()
      << "; sumpt04: " << d.getsumpt04()
      << "; PtMuID: " << d.getPtMuID()
      << "; MaxPtID: " << d.getMaxPtID()
      << "; flag: " << d.getFlag()
      << "; RoiIdMu: " << d.getRoiIdMu()
      << "; RoiIdMu: " << d.getRoiIdMu()
      << "; PtMu: " << d.pt()
      << "; QMu: " << d.getQMu()
      << "; EtaMu: " << d.getEtaMu()
      << "; PhiMu: " << d.getPhiMu();

   return ss.str();
}

MsgStream& operator<< ( MsgStream& m, const IsoMuonFeature& d ) {

   return ( m << str( d ) );
}

bool operator== ( const IsoMuonFeature& a, const IsoMuonFeature& b ) {

   if( std::abs( a.getsumet01() - b.getsumet01() ) > DELTA )
      return false;
   if( std::abs( a.getsumet02() - b.getsumet02() ) > DELTA )
      return false;
   if( std::abs( a.getsumet03() - b.getsumet03() ) > DELTA )
      return false;
   if( std::abs( a.getsumet04() - b.getsumet04() ) > DELTA )
      return false;
   if( std::abs( a.getsumpt01() - b.getsumpt01() ) > DELTA )
      return false;
   if( std::abs( a.getsumpt02() - b.getsumpt02() ) > DELTA )
      return false;
   if( std::abs( a.getsumpt03() - b.getsumpt03() ) > DELTA )
      return false;
   if( std::abs( a.getsumpt04() - b.getsumpt04() ) > DELTA )
      return false;
   if( std::abs( a.getPtMuID() - b.getPtMuID() ) > DELTA )
      return false;
   if( std::abs( a.getMaxPtID() - b.getMaxPtID() ) > DELTA )
      return false;
   if( a.getFlag() != b.getFlag() )
      return false;
   if( a.getRoiIdMu() != b.getRoiIdMu() )
      return false;
   if( std::abs( a.pt() - b.pt() ) > DELTA )
      return false;
   if( std::abs( a.getQMu() - b.getQMu() ) > DELTA )
      return false;
   if( std::abs( a.getEtaMu() - b.getEtaMu() ) > DELTA )
      return false;
   if( std::abs( a.getPhiMu() - b.getPhiMu() ) > DELTA )
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
void diff( const IsoMuonFeature& a, const IsoMuonFeature& b,
           std::map< std::string, double >& variableChange ) {

   if( std::abs( a.getsumet01() - b.getsumet01() ) > DELTA ) {
      variableChange[ "sumet01" ] = a.getsumet01() - b.getsumet01();
   }
   if( std::abs( a.getsumet02() - b.getsumet02() ) > DELTA ) {
      variableChange[ "sumet02" ] = a.getsumet02() - b.getsumet02();
   }
   if( std::abs( a.getsumet03() - b.getsumet03() ) > DELTA ) {
      variableChange[ "sumet03" ] = a.getsumet03() - b.getsumet03();
   }
   if( std::abs( a.getsumet04() - b.getsumet04() ) > DELTA ) {
      variableChange[ "sumet04" ] = a.getsumet04() - b.getsumet04();
   }
   if( std::abs( a.getsumpt01() - b.getsumpt01() ) > DELTA ) {
      variableChange[ "sumpt01" ] = a.getsumpt01() - b.getsumpt01();
   }
   if( std::abs( a.getsumet02() - b.getsumet02() ) > DELTA ) {
      variableChange[ "sumet02" ] = a.getsumet02() - b.getsumpt02();
   }
   if( std::abs( a.getsumpt03() - b.getsumpt03() ) > DELTA ) {
      variableChange[ "sumpt03" ] = a.getsumpt03() - b.getsumpt03();
   }
   if( std::abs( a.getsumpt04() - b.getsumpt04() ) > DELTA ) {
      variableChange[ "sumpt04" ] = a.getsumpt04() - b.getsumpt04();
   }
   if( a.getFlag() != b.getFlag() ) {
      variableChange[ "flag" ] = static_cast< double >( a.getFlag() -
                                                        b.getFlag() );
   }
   if( a.getRoiIdMu() != b.getRoiIdMu() ) {
      variableChange[ "RoiIdMu" ] = static_cast< double >( a.getRoiIdMu() -
                                                           b.getRoiIdMu() );
   }
   if( std::abs( a.pt() - b.pt() ) > DELTA ) {
      variableChange[ "PtMu" ] = a.pt() - b.pt();
   }
   if( std::abs( a.getQMu() - b.getQMu() ) > DELTA ) {
      variableChange[ "QMu" ] = a.getQMu() - b.getQMu();
   }
   if( std::abs( a.getEtaMu() - b.getEtaMu() ) > DELTA ) {
      variableChange[ "EtaMu" ] = a.getEtaMu() - b.getEtaMu();
   }
   if( std::abs( a.getPhiMu() - b.getPhiMu() ) > DELTA ) {
      variableChange[ "PhiMu" ] = a.getPhiMu() - b.getPhiMu();
   }

   return;
}
 
// End of file IsoMuonFeature.cxx
