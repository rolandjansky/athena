/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// *****************************************************************************
//  Filename : TileTrackMuFeature.h
//  Author   : Heuijin Lim
//  Created  : Jun. 16, 2008
//
//  DESCRIPTION:
//    Contain the muon candidates from TileCal and the combined tracks from
//    Inner-Detector.
//
//  HISTORY:
//
//  BUGS:
//
//*****************************************************************************

#ifndef TRIGMUONEVENT_TILETRACKMUFEATURE_H
#define TRIGMUONEVENT_TILETRACKMUFEATURE_H

#include <string>
#include <vector>

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"
#include "AthLinks/ElementLink.h"

// Trigger includes:
#include "TrigMuonEvent/TileMuFeatureContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"


class TileTrackMuFeature :  public P4PtEtaPhiMBase,
                            public NavigableTerminalNode,
                            virtual public INavigable4Momentum
{
 public:  

  /** Constructor */
  TileTrackMuFeature(); 

  TileTrackMuFeature (
	float PtTR_Trk,
 	float EtaTR_Trk, float PhiTR_Trk, int Typ_IDTrk,
	const ElementLink< TileMuFeatureContainer>& TileMuOutput, 
	const ElementLink< TrigInDetTrackCollection>& IDScanOutput);

  /** Destructor */
  ~TileTrackMuFeature();

  /** Copy pointer constructor */
  TileTrackMuFeature(const TileTrackMuFeature* muon_feature);

  /** Copy reference constructor */
  TileTrackMuFeature(const TileTrackMuFeature& muon_feature); 

  /** Assignement operator */ 
  TileTrackMuFeature& operator=(const TileTrackMuFeature& muon_feature);

  /** Ovveride pure virtual methods */
  double PtTR_Trk()  const { return m_PtTR_Trk;	}
  double EtaTR_Trk() const { return m_EtaTR_Trk; }
  double PhiTR_Trk() const { return m_PhiTR_Trk; }
  int	 Typ_IDTrk() const { return m_Typ_IDTrk; }

  const TileMuFeature*  TileMuOutput(void) const { return *m_TileMuOutput; }
  const TrigInDetTrack* IDScanOutput(void) const { return *m_IDScanOutput; }

  const ElementLink<TileMuFeatureContainer>&   TileMuLink(void) const { return m_TileMuOutput; }
  const ElementLink<TrigInDetTrackCollection>& IDScanLink(void) const { return m_IDScanOutput; }

  double pt(void)  const { return m_IDScanOutput ? (*m_IDScanOutput)->param()->pT():-9999.9; }
  double eta(void) const { return m_IDScanOutput ? (*m_IDScanOutput)->param()->eta():-9999.9; }
  double phi(void) const { return m_IDScanOutput ? (*m_IDScanOutput)->param()->phi0():-9999.9; }
  double m(void)   const { return -9999.9; } 

 private:

  float m_PtTR_Trk;
  float m_EtaTR_Trk;
  float m_PhiTR_Trk;
  int	m_Typ_IDTrk;

  ElementLink<TileMuFeatureContainer>	m_TileMuOutput;
  ElementLink<TrigInDetTrackCollection> m_IDScanOutput;
};

/** Helper function for printing the object */
std::string str( const TileTrackMuFeature& d );

/** Helper operator for printing the object */
MsgStream& operator<< ( MsgStream& m, const TileTrackMuFeature& d );

/** Operator comparing two CombinedMuonFeature objects for equality */
bool operator== ( const TileTrackMuFeature& a, const TileTrackMuFeature& b );

/** Operator comparing two CombinedMuonFeature objects for inequality */
inline bool operator!= ( const TileTrackMuFeature& a, const TileTrackMuFeature& b )
{
  return !( a == b );
}

/** Comparison with feedback */
void diff( const TileTrackMuFeature& a, const TileTrackMuFeature& b, std::map< std::string, double >& variableChange );

CLASS_DEF(TileTrackMuFeature,             77762301, 0)
CLASS_DEF(DataVector<TileTrackMuFeature>, 61923500, 0) 

#include "TrigMuonEvent/TileTrackMuFeatureContainer.h"
 
#endif  
