/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef TRIGMUON_TRIGMUONCLUSTERFEATURE_H
#define TRIGMUON_TRIGMUONCLUSTERFEATURE_H
/*******************************************************

NAME:		TrigMuonClusterFeature.h
PACKAGE:	Trigger/TrigEvent/TrigMuonEvent
AUTHORS:	Antonio Policicchio <Antonio.Policicchio@cern.ch>
                Stefano Giagu <stefano.giagu@cern.ch>
PURPOSE:	Keep the important output variables
		from the muon ROI cluster trigger.
DATE:		V1.0 Jan 13th , 2009 

******************************************************/
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"

// Forward declaration(s):
class MsgStream;

class TrigMuonClusterFeature : public P4PtEtaPhiMBase,
		               public NavigableTerminalNode,
		               virtual public INavigable4Momentum {

 public:

  /** Constructors */
  TrigMuonClusterFeature();
  TrigMuonClusterFeature( float eta, float phi, int nroi, int njet, int ntrk ); 

  /** Destructor */
  ~TrigMuonClusterFeature ();

  /** Copy pointer constructor. */
  TrigMuonClusterFeature ( const TrigMuonClusterFeature* mfeature ) ;
  /** Copy reference constructor. */
  TrigMuonClusterFeature ( const TrigMuonClusterFeature& mfeature ) ;
  /** Assignement operator. */
  TrigMuonClusterFeature& operator=(const TrigMuonClusterFeature& mfeature);

  //Ovveride pure virtual methods
  double pt(void)  const { return 0.0; }
  double eta(void) const { return m_eta; }
  double phi(void) const { return m_phi; }
  double m(void)   const { return 0.0; }

  inline int getNRoi() const { return m_nroi ;};
  inline int getNTRK() const { return m_ntrk ;};
  inline int getNJet() const { return m_njet ;};
  inline float getEta() const { return m_eta ; };
  inline float getPhi() const { return m_phi ; };

  private:
  float m_eta;
  float m_phi;
  int m_nroi;
  int m_njet;
  int m_ntrk;

};
/// Helper function for printing the object
std::string str( const TrigMuonClusterFeature& d );
/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const TrigMuonClusterFeature& d );
/// Operator comparing two TrigMuonClusterFeature objects for equality
bool operator== ( const TrigMuonClusterFeature& a, const TrigMuonClusterFeature& b ); 
/// Operator comparing two TrigMuonClusterFeature objects for inequality
inline bool operator!= ( const TrigMuonClusterFeature& a, const TrigMuonClusterFeature& b ) { return !(a==b); }

/** @brief comparison with feedback
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are varaibel names and differences
 * between two obejcts
 * @param variableChange - map to record the differences
 * In case of collections (or objects when the size may be different) that information can also be returned in varaibleChange
 */
void diff( const TrigMuonClusterFeature& a, const TrigMuonClusterFeature& b, std::map<std::string, double>& variableChange ); 

CLASS_DEF( TrigMuonClusterFeature , 233283613 , 1 )

CLASS_DEF( DataVector<TrigMuonClusterFeature> , 118369108 , 1 )

#endif
