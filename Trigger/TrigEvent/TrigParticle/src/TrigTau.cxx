// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigTau.cxx

Description: To be created by the last FEX algorithm.
  
Design: Must be light and easy to serialize AND to store in ESD; should 
             avoid ElementLinks and try to persistify pointers instead; 
             should avoid inheritance to be easy to serialize.

Authour: M.P. Casado - Tu Mar 01 2006

Modified:

*/

#include "TrigParticle/TrigTau.h"

// default constructor
TrigTau::TrigTau() : P4PtEtaPhiM(0,0,0,1777*CLHEP::MeV), NavigableTerminalNode(),
		     m_roiID(-1),    m_Zvtx(0), 
		      m_err_Zvtx(0), 
		     m_etCalibCluster(0), m_simpleEtFlow(0), m_nMatchedTracks(0),
		     m_cluster(NULL),  m_tracksInfo(NULL), m_tracks(NULL),
		     m_trkAvgDist(0), m_etOverPtLeadTrk(0)
{}

// constructor
TrigTau::TrigTau(int roi, 
		 float eta, float phi, float zvtx, float pt, 
		 float err_zvtx, 
		 float etCalibClusterr, float etFlowr, int nMatchedTracks,
		 const TrigTauCluster* cluster, const TrigInDetTrackCollection* tracks,
		 const TrigTauTracksInfo * tracksInfo) : 
  P4PtEtaPhiM(pt,eta,phi, 1777*CLHEP::MeV), NavigableTerminalNode(),
  m_roiID(roi),   m_Zvtx(zvtx), 
   m_err_Zvtx(err_zvtx), 
  m_etCalibCluster(etCalibClusterr), m_simpleEtFlow(etFlowr),  m_nMatchedTracks(nMatchedTracks), 
  m_cluster(cluster),  m_tracksInfo(tracksInfo),  m_tracks(tracks),
  m_trkAvgDist(0), m_etOverPtLeadTrk(0)
{}


// constructor
TrigTau::TrigTau(int roi, 
		 float eta, float phi, float zvtx, float pt, float etCalibClusterr,
		 float etFLow, int nMatchedTracks,
		 const TrigTauCluster* cluster, const TrigInDetTrackCollection* tracks,
		 const TrigTauTracksInfo * tracksInfo) :
  P4PtEtaPhiM(pt,eta,phi, 1777*CLHEP::MeV), NavigableTerminalNode(),
   m_roiID(roi),  
   m_Zvtx(zvtx), 
   m_err_Zvtx(0), 
   m_etCalibCluster(etCalibClusterr), m_simpleEtFlow(etFLow),
   m_nMatchedTracks(nMatchedTracks),  m_cluster(cluster),  
   m_tracksInfo(tracksInfo), m_tracks(tracks),
   m_trkAvgDist(0), m_etOverPtLeadTrk(0)
{}

// destructor
TrigTau::~TrigTau() {}

// accessor methods
int   TrigTau::roiId()    const { return m_roiID;    }

float TrigTau::Zvtx()     const { return m_Zvtx;     }

float TrigTau::err_Zvtx() const { return m_err_Zvtx; }

float TrigTau::etCalibCluster()   const { return m_etCalibCluster;   }

float TrigTau::simpleEtFlow() const { return m_simpleEtFlow; }

int TrigTau::nMatchedTracks()   const { return m_nMatchedTracks;   }
float TrigTau::trkAvgDist() const { return m_trkAvgDist; }
float TrigTau::etOverPtLeadTrk() const { return m_etOverPtLeadTrk; }

const TrigTauCluster* TrigTau::tauCluster() const { return m_cluster; }

const TrigInDetTrackCollection*  TrigTau::trackCollection()   const { return m_tracks;   }
const TrigTauTracksInfo*  TrigTau::tracksInfo()   const { return m_tracksInfo;   }

  /** set methods */
void TrigTau::setRoiId(int roiId){m_roiID = roiId;}
void TrigTau::setZvtx(float zvtx){m_Zvtx = zvtx;}
void TrigTau::setErr_Zvtx(float err_zvtx){m_err_Zvtx = err_zvtx;}
void TrigTau::setEtCalibCluster(float etCalibCluster){m_etCalibCluster = etCalibCluster ;}
void TrigTau::setSimpleEtFlow(float etFlow){m_simpleEtFlow = etFlow; }
void TrigTau::setNMatchedTracks(int nMatchedTracks){m_nMatchedTracks = nMatchedTracks;}
void TrigTau::setTauCluster(const TrigTauCluster* tauCluster){m_cluster = tauCluster ;}
void TrigTau::setTrackCollection(const TrigInDetTrackCollection* trackColl){m_tracks =
trackColl ;}
void TrigTau::setTracksInfo(const TrigTauTracksInfo* tracksInfo){m_tracksInfo =
tracksInfo ;}
void TrigTau::setTrkAvgDist(float trkAvgDist){m_trkAvgDist = trkAvgDist; }
void TrigTau::setEtOverPtLeadTrk(float etOverPtLeadTrk){m_etOverPtLeadTrk = etOverPtLeadTrk; }


std::string str( const TrigTau& tau ) {

   std::stringstream stream;
   stream << "RoI ID: " << tau.roiId()
          << "; pt: " << tau.pt()
          << "; eta: " << tau.eta()
          << "; phi: " << tau.phi()
          << "; etCalibCluster: " << tau.etCalibCluster()
          << "; simpleEtFlow: " << tau.simpleEtFlow()
          << "; nMatchedTracks: " << tau.nMatchedTracks();
   //          << "; Zvtx: " << tau.Zvtx()
   //       << "; err_Zvtx: " << tau.err_Zvtx();
   stream << "; trkAvgDist: "<< tau.trkAvgDist()
	  << "; etOverPtLeadTrk: "<<tau.etOverPtLeadTrk();

   return stream.str();
}

MsgStream& operator<< ( MsgStream& m, const TrigTau& tau ) {

   return ( m << str( tau ) );

}

bool operator== ( const TrigTau& left, const TrigTau& right ) {

  static const double DELTA = 0.001;
   if( ( left.roiId() != right.roiId() ) ||
       ( std::abs( left.pt() - right.pt() ) > DELTA ) ||
       ( std::abs( left.eta() - right.eta() ) > DELTA ) ||
       ( std::abs( left.phi() - right.phi() ) > DELTA ) ||
       ( std::abs( left.etCalibCluster() - right.etCalibCluster() ) > DELTA ) ||
       ( std::abs( left.simpleEtFlow() - right.simpleEtFlow() ) > DELTA ) ||
       ( left.nMatchedTracks() != right.nMatchedTracks() ) ||
       (  std::abs( left.trkAvgDist() - right.trkAvgDist() ) > DELTA ) ||
       (  std::abs( left.etOverPtLeadTrk() - right.etOverPtLeadTrk() ) > DELTA ) 
       ) {

      return false;

   } else {

      return true;

   }

}

void diff( const TrigTau& left, const TrigTau& right,
           std::map< std::string, double >& varChange ) {

  static const double DELTA = 0.001;
   if( left.roiId() != right.roiId() ) {
      varChange[ "roiId" ] = static_cast< double >( left.roiId() - right.roiId() );
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

   if( std::abs( left.etCalibCluster() - right.etCalibCluster() ) > DELTA ) {
      varChange[ "etCalibCluster" ] = left.etCalibCluster() - right.etCalibCluster();
   }
   if( std::abs( left.simpleEtFlow() - right.simpleEtFlow() ) > DELTA ) {
      varChange[ "simpleEtFlow" ] = left.simpleEtFlow() - right.simpleEtFlow();
   }
   if(  left.nMatchedTracks() != right.nMatchedTracks()  ) {
      varChange[ "nMatchedTracks" ] = left.nMatchedTracks() - right.nMatchedTracks();
   }
   if(  left.trkAvgDist() != right.trkAvgDist()  ) {
      varChange[ "trkAvgDist" ] = left.trkAvgDist() - right.trkAvgDist();
   }
   if(  left.etOverPtLeadTrk() != right.etOverPtLeadTrk()  ) {
      varChange[ "etOverPtLeadTrk" ] = left.etOverPtLeadTrk() - right.etOverPtLeadTrk();
   }

   return;
}
