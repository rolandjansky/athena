/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** @class TrigTauTracksInfo

   Contains basic information about trackc collection associated with Tau RoI. 
   To be used by FEX TrigT2IDTau at L2.


   created by Olya Igonkina - Jan 16, 2008


*/

#include "TrigInDetEvent/TrigTauTracksInfo.h"

TrigTauTracksInfo::TrigTauTracksInfo() :
  P4PtEtaPhiM(0,0,0,0),
  NavigableTerminalNode(),
  m_roiID(0),
  m_nCoreTracks(0),
  m_nSlowTracks(0),
  m_nIsoTracks(0),
  m_charge(0),
  m_leadingTrackPt(0),
  m_scalarPtSumCore(0),
  m_scalarPtSumIso(0),
  m_ptBalance(0),
  m_tracks(0)
{
  m_3fastest.setPt(0);
  m_3fastest.setEta(0);
  m_3fastest.setPhi(0);
  m_3fastest.setM(0);
  
} 

TrigTauTracksInfo::TrigTauTracksInfo(const TrigTauTracksInfo & te) : 
      I4Momentum(te), 
      P4PtEtaPhiMBase(te),
      INavigable(te),
      IAthenaBarCode(te),
      INavigable4Momentum(te),
      P4PtEtaPhiM(te),
      NavigableTerminalNode(te) 
{
  m_roiID           = te.m_roiID;
  m_nCoreTracks     = te.m_nCoreTracks     ;
  m_nSlowTracks     = te.m_nSlowTracks     ;
  m_nIsoTracks      = te.m_nIsoTracks      ;
  m_charge          = te.m_charge          ;
  m_leadingTrackPt  = te.m_leadingTrackPt ;
  m_scalarPtSumCore = te.m_scalarPtSumCore ;
  m_scalarPtSumIso  = te.m_scalarPtSumIso  ;
  m_ptBalance       = te.m_ptBalance       ;

  
  m_3fastest.setPt( te.threeFastestTracks().pt());
  m_3fastest.setEta( te.threeFastestTracks().eta());
  m_3fastest.setPhi( te.threeFastestTracks().phi());
  m_3fastest.setM( te.threeFastestTracks().m());
}
TrigTauTracksInfo::~TrigTauTracksInfo(){}


void TrigTauTracksInfo::set3fastestPtEtaPhiM(float pt, float eta, float phi, float m)
{
  m_3fastest.setPt( pt);
  m_3fastest.setEta( eta);
  m_3fastest.setPhi( phi);
  m_3fastest.setM( m);
}
void TrigTauTracksInfo::set3fastestPxPyPzE(float px, float py, float pz, float e)
{
  m_3fastest.set4Mom(P4PxPyPzE(px,py,pz, e));
}


std::string str( const TrigTauTracksInfo& tau ) {

   std::stringstream stream;
   stream << "RoI ID: " << tau.roiId()
          << "; pt: " << tau.pt()
          << "; eta: " << tau.eta()
          << "; phi: " << tau.phi()
          << "; nCoreTracks: " << tau.nCoreTracks()
          << "; nSlowTracks: " << tau.nSlowTracks()
          << "; nIsoTracks: " << tau.nIsoTracks()
          << "; charge: " << tau.charge()
          << "; leadingTrackPt: " << tau.leadingTrackPt()
          << "; scalarPtSumCore: " << tau.scalarPtSumCore()
          << "; scalarPtSumIso: " << tau.scalarPtSumIso()
          << "; ptBalance: " << tau.ptBalance()
          << "; threeFastestTracksPt: " << tau.threeFastestTracks().pt()
          << "; threeFastestTracksEta: " << tau.threeFastestTracks().eta()
          << "; threeFastestTracksPhi: " << tau.threeFastestTracks().phi();


   return stream.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigTauTracksInfo& tau ) {

   return ( m << str( tau ) );

}

bool operator== ( const TrigTauTracksInfo& left, const TrigTauTracksInfo& right ) {

  static const double DELTA = 0.001;
   if( ( left.roiId() != right.roiId() ) ||
       ( left.nCoreTracks() != right.nCoreTracks() ) ||
       ( left.nSlowTracks() != right.nSlowTracks() ) ||
       ( left.nIsoTracks() != right.nIsoTracks() ) ||

       ( std::abs( left.pt() - right.pt() ) > DELTA ) ||
       ( std::abs( left.eta() - right.eta() ) > DELTA ) ||
       ( std::abs( left.phi() - right.phi() ) > DELTA ) ||

       ( std::abs( left.charge() - right.charge() ) > DELTA ) ||
       ( std::abs( left.leadingTrackPt() - right.leadingTrackPt() ) > DELTA ) ||
       ( std::abs( left.scalarPtSumCore() - right.scalarPtSumCore() ) > DELTA ) ||
       ( std::abs( left.scalarPtSumIso() - right.scalarPtSumIso() ) > DELTA ) ||
       ( std::abs( left.ptBalance() - right.ptBalance() ) > DELTA ) 
       ) {

      return false;

   } else {

      return true;

   }

}

void diff( const TrigTauTracksInfo& left, const TrigTauTracksInfo& right,
           std::map< std::string, double >& varChange ) {

  static const double DELTA = 0.001;
   if( left.roiId() != right.roiId() ) {
      varChange[ "roiId" ] = static_cast< double >( left.roiId() - right.roiId() );
   }
   if( left.nCoreTracks() != right.nCoreTracks() ) {
      varChange[ "nCoreTracks" ] = static_cast< double >( left.nCoreTracks() - right.nCoreTracks() );
   }
   if( left.nSlowTracks() != right.nSlowTracks() ) {
      varChange[ "nSlowTracks" ] = static_cast< double >( left.nSlowTracks() - right.nSlowTracks() );
   }
   if( left.nIsoTracks() != right.nIsoTracks() ) {
      varChange[ "nIsoTracks" ] = static_cast< double >( left.nIsoTracks() - right.nIsoTracks() );
   }
   if( std::abs( left.pt() - right.pt() ) > DELTA ) {
     varChange[ "pt" ] = left.pt() - right.pt();
   }
   if( std::abs( left.eta() - right.eta() ) > DELTA ) {
      varChange[ "eta" ] = left.eta() - right.eta();
   }
   if( std::abs( left.phi() - right.phi() ) > DELTA ) {
      varChange[ "phi" ] = left.phi() - right.phi();
   }

   if( std::abs( left.charge() - right.charge() ) > DELTA ) {
      varChange[ "charge" ] = left.charge() - right.charge();
   }
   if( std::abs( left.leadingTrackPt() - right.leadingTrackPt() ) > DELTA ) {
      varChange[ "leadingTrackPt" ] = left.leadingTrackPt() - right.leadingTrackPt();
   }
   if(  left.scalarPtSumCore() != right.scalarPtSumCore()  ) {
      varChange[ "scalarPtSumCore" ] = left.scalarPtSumCore() - right.scalarPtSumCore();
   }
   if(  left.scalarPtSumIso() != right.scalarPtSumIso()  ) {
      varChange[ "scalarPtSumIso" ] = left.scalarPtSumIso() - right.scalarPtSumIso();
   }
   if(  left.ptBalance() != right.ptBalance()  ) {
      varChange[ "ptBalance" ] = left.ptBalance() - right.ptBalance();
   }

   return;
}
