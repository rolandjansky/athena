/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUTRACKSINFO_H
#define TRIGTAUTRACKSINFO_H

/** @class TrigTauTracksInfo

   Contains basic information about trackc collection associated with Tau RoI. 
   To be used by FEX TrigT2IDTau at L2.


   created by Olya Igonkina - Jan 16, 2008


*/
#include "AthenaKernel/CLASS_DEF.h"
#include "FourMom/P4PtEtaPhiM.h"
#include "FourMom/P4PxPyPzE.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"



class TrigTauTracksInfo : public P4PtEtaPhiM,  public NavigableTerminalNode ,
			  virtual public INavigable4Momentum   {
public:
  /** Constructor */
  TrigTauTracksInfo();
  /** Copy Constructor */  
  TrigTauTracksInfo(const TrigTauTracksInfo & te);

  /** Descrtuctor */
  ~TrigTauTracksInfo();

  /** getters */
  int roiId() const {return m_roiID;};

  int nCoreTracks() const {return m_nCoreTracks;}; 
  int nSlowTracks() const{return m_nSlowTracks;};
  int nIsoTracks() const{return m_nIsoTracks;};

  float charge() const{return m_charge;};
  float leadingTrackPt() const { return m_leadingTrackPt; };
  float sumPtRatio() const{return m_scalarPtSumIso/m_scalarPtSumCore;};
  float scalarPtSumCore() const{return m_scalarPtSumCore;};
  float scalarPtSumIso() const{return m_scalarPtSumIso;};
  float ptBalance() const{return m_ptBalance;};

  const P4PtEtaPhiM & threeFastestTracks() const {return m_3fastest; } ;
  const  TrigInDetTrackCollection*  trackCollection()   const {return m_tracks;};


  /** setters */
  void setRoiId(int id)  {m_roiID = id;};
  void setNCoreTracks( int nCoreTrk) {m_nCoreTracks = nCoreTrk; } ;
  void setNSlowTracks(int nSlowTrk)  {m_nSlowTracks = nSlowTrk; }  ;
  void setNIsoTracks( int nIsoTrk )  {m_nIsoTracks  = nIsoTrk;  };

  void setCharge( float charge) {m_charge = charge; };
  void setLeadingTrackPt (float pt ) {m_leadingTrackPt = pt;}
  void setScalarPtSumCore(float sumPtCore) {m_scalarPtSumCore = sumPtCore; };
  void setScalarPtSumIso(float sumPtIso)   {m_scalarPtSumIso = sumPtIso; };
  void setPtBalance(float ptBal)           {m_ptBalance = ptBal;} ;

  void set3fastestPtEtaPhiM(float pt, float eta, float phi, float m);
  void set3fastestPxPyPzE(float px, float py, float pz, float e);
  void setTrackCollection(const TrigInDetTrackCollection* trackColl) {m_tracks = trackColl;};

  
private:
  /** RoI id */
  int m_roiID;
  /** number of tracks in the core region */
  int m_nCoreTracks;
  /** number of slow tracks in the core region */
  int m_nSlowTracks;
  /** number of tracks in the isolation region */
  int m_nIsoTracks;

  /** charge of tracks in the core region.  */
  float m_charge; // use float in case we ever will shift to weighted pt charge
  /** leading track Pt */
  float m_leadingTrackPt;
  /** scalar sum of pts in core area */  
  float m_scalarPtSumCore;
  /** scalar sum of pts in isolation area */  
  float m_scalarPtSumIso;
  /** variable determining a difference between slow and leading tracks */
  float m_ptBalance;

  P4PtEtaPhiM m_3fastest; 

  const TrigInDetTrackCollection*  m_tracks;


};

/// Helper function for printing the object
std::string str( const TrigTauTracksInfo& tau );

/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigTauTracksInfo& tau );

/// Operator comparing two TrigTauTracksInfo objects for equality
bool operator== ( const TrigTauTracksInfo& left, const TrigTauTracksInfo& right );
/// Operator comparing two TrigTauTracksInfo objects for inequality
inline bool operator!= ( const TrigTauTracksInfo& left, const TrigTauTracksInfo& right ) {
   return !( left == right );
}
	
/// Comparison with feedback
void diff( const TrigTauTracksInfo& left, const TrigTauTracksInfo& right,
           std::map< std::string, double >& varChange );



CLASS_DEF( TrigTauTracksInfo , 239760293 , 1 )

#include "TrigInDetEvent/TrigTauTracksInfoCollection.h"
#endif
