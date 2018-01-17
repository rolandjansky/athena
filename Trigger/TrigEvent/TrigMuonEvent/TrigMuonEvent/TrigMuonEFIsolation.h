// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFISOLATION_H
#define TRIGMUONEVENT_TRIGMUONEFISOLATION_H
/**
 * @class TrigMuonEFIsolation
 *
 * EDM class for holding results of EF muon isolation calculation.
 *
 * @author Mark Owen
 */

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"
#include "AthLinks/ElementLink.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

class TrigMuonEFInfoTrack;

class TrigMuonEFIsolation {

 public:

  /// Default constructor
  TrigMuonEFIsolation();

  /// Constructor to fill track isolation variables
  TrigMuonEFIsolation(const float trkptcone02, const float trkptcone03);

  /// Copy constructor from reference
  TrigMuonEFIsolation(const TrigMuonEFIsolation& iso);
  
  /// Copy constructor from pointer
  TrigMuonEFIsolation(const TrigMuonEFIsolation* iso);

  /// Assignment operator
  TrigMuonEFIsolation& operator=(const TrigMuonEFIsolation& rhs);

  /// Destructor
  ~TrigMuonEFIsolation() { }

  /// set the link to the EF muon we calculated the isolation for
  void setEFMuonInfoLink(const TrigMuonEFInfoContainer* infocont, const int pos);
  void setEFMuonInfoTrackLink(const int itrk);

  /// set the element link to the TrigMuonEFInfo object we are using
  void setEFMuonInfoElementLink(const ElementLink<TrigMuonEFInfoContainer> link);

  // sum of pT of tracks in 0.2 cone
  float sumTrkPtCone02() const {return m_sumTrkPtCone02;}
  // sum of pT of tracks in 0.3 cone
  float sumTrkPtCone03() const {return m_sumTrkPtCone03;}
  
  // sum of calorimeter Et in 0.1 cone
  float sumEtCone01() const {return m_sumEtCone01;}
  // sum of calorimeter Et in 0.2 cone
  float sumEtCone02() const {return m_sumEtCone02;}
  // sum of calorimeter Et in 0.3 cone
  float sumEtCone03() const {return m_sumEtCone03;}
  // sum of calorimeter Et in 0.4 cone
  float sumEtCone04() const {return m_sumEtCone04;}

  /// Position of the muon in the TrigMuonEFInfoTrack container
  int trackPosition() const {return m_trkPos;}

  /// Access the muon we calculated the isolation for
  const TrigMuonEFInfoTrack* getEFMuonInfoTrack() const;

  /// Access the muon info object we calculated the isolation for
  const TrigMuonEFInfo* getMuonInfo() const;
  const ElementLink<TrigMuonEFInfoContainer>& getMuonInfoLink() const;


  /// set sum of track Pt in 0.2 cone
  void setSumTrkPtCone02(const double ptcone02) {m_sumTrkPtCone02=ptcone02;}
  /// set sum of track Pt in 0.3 cone
  void setSumTrkPtCone03(const double ptcone03) {m_sumTrkPtCone03=ptcone03;}

  /// set sum of calorimeter Et in 0.1 cone
  void setSumEtCone01(const double etcone01) {m_sumEtCone01=etcone01;}
  /// set sum of calorimeter Et in 0.2 cone
  void setSumEtCone02(const double etcone02) {m_sumEtCone02=etcone02;}
  /// set sum of calorimeter Et in 0.3 cone
  void setSumEtCone03(const double etcone03) {m_sumEtCone03=etcone03;}
  /// set sum of calorimeter Et in 0.4 cone
  void setSumEtCone04(const double etcone04) {m_sumEtCone04=etcone04;}

 private:
  
  // sum of pT of tracks in 0.2 cone
  float m_sumTrkPtCone02;
  // sum of pT of tracks in 0.3 cone
  float m_sumTrkPtCone03;

  // sum of ET in calorimeter in 0.1 cone
  float m_sumEtCone01;

  // sum of ET in calorimeter in 0.2 cone
  float m_sumEtCone02;

  // sum of ET in calorimeter in 0.3 cone
  float m_sumEtCone03;

  // sum of ET in calorimeter in 0.4 cone
  float m_sumEtCone04;

  /// Position of the TrigMuonEFInfoTrack within the TrigMuonEFInfo object
  int m_trkPos;

  /// Element link to TrigMuonEFInfo
  ElementLink<TrigMuonEFInfoContainer> m_muonEFLink;

};//class TrigMuonEFIsolation

/// Operator comparing two TrigMuonEFIsolation objects for equality
bool operator== ( const TrigMuonEFIsolation& a, const TrigMuonEFIsolation& b );
/// Operator comparing two TrigMuonEFIsolation objects for inequality
inline bool operator!= ( const TrigMuonEFIsolation& a, const TrigMuonEFIsolation& b ) {
   return !( a == b );
}

CLASS_DEF( TrigMuonEFIsolation , 100542549 , 1 )

#endif // TRIGMUONEVENT_TRIGMUONEFISOLATION_H__
