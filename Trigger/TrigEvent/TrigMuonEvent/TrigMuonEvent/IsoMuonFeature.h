// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************

NAME:		IsoMuonFeature.h
PACKAGE:	Trigger/TrigEvent/TrigMuonEvent
AUTHORS:	Stefano Giagu <stefano.giagu@cern.ch>
PURPOSE:	Keep the important output variables
		from the muon isolation trigger.
DATE:		V4.0 May 5th, 2008

******************************************************/
#ifndef TRIGMUONEVENT_ISO_MUON_FEATURE_H
#define TRIGMUONEVENT_ISO_MUON_FEATURE_H

// STL include(s):
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"

// Forward declaration(s):
class MsgStream;

/** Class with isolation variables for TrigmuIso algorithm. 
    Includes infos from calorimeters and inner tracker. */
class IsoMuonFeature : public P4PtEtaPhiMBase,
                       public NavigableTerminalNode,
                       virtual public INavigable4Momentum {
  
public:
 
  /** Constructor. */ 
  IsoMuonFeature();

  /** Destructor. */
  ~IsoMuonFeature ();

  /** Copy pointer constructor. */
  IsoMuonFeature ( const IsoMuonFeature* mfeature ) ;
  /** Copy reference constructor. */
  IsoMuonFeature ( const IsoMuonFeature& mfeature ) ;
  /** Assignement operator. */
  IsoMuonFeature& operator=(const IsoMuonFeature& mfeature);

  // Print out variables for Debug
  /** Prints out cluster variables to std::cout. */
  void print ( void );
  /** Prints out cluster variables to MsgStream. */
  void print ( MsgStream& log );

  //Ovveride pure virtual methods
  double pt(void)     const {return fabs(m_PtMu); }
  double ptq(void)    const {return fabs(m_QMu)>0?m_PtMu*m_QMu:m_PtMu; }
  double charge(void) const {return fabs(m_QMu)>0?m_QMu:(m_PtMu>0?1.0:-1.0); }
  double eta(void)    const {return m_EtaMu; }
  double phi(void)    const {return m_PhiMu; }
  double m(void)      const {return 105.658367;}

  //Setters
  /** Set calorimetric infos. */
  inline void setCaloInfos(float e01, float e02, float e03, float e04) {
    m_sumet01 = e01;
    m_sumet02 = e02;
    m_sumet03 = e03;
    m_sumet04 = e04;
  };
  /** Set Tracks infos. */
  inline void setTrackInfos(float p01, float p02, float p03, float p04, float pmu, float pmax) {
    m_sumpt01 = p01;
    m_sumpt02 = p02;
    m_sumpt03 = p03;
    m_sumpt04 = p04;
    m_PtMuID  = pmu;
    m_MaxPtID = pmax;
  };
  /** Set Muon Roi */
  inline void setRoiMu(int id, float pt, float eta, float phi, int fl) {
    m_RoiIdMu = id;
    m_PtMu    = fabs(pt);
    if      (pt > 0) m_QMu =  1.0;
    else if (pt < 0) m_QMu = -1.0;
    else             m_QMu =  0.0;
    m_EtaMu   = eta;
    m_PhiMu   = phi;
    m_flag    = fl;
  };
  inline void setRoiMu(int id, float pt, float eta, float phi, float charge, int fl) {
    m_RoiIdMu = id;
    m_PtMu    = fabs(pt);
    m_QMu     = charge;
    m_EtaMu   = eta;
    m_PhiMu   = phi;
    m_flag    = fl;
  };

  //Getters
  /** Get Transverse Energy sum in cone. */
  inline float getsumet01 () const { return fabs(m_EtInnerConeEC+m_EtInnerConeHC)>0?m_EtInnerConeEC+m_EtInnerConeHC:m_sumet01; };
  inline float getsumet02 () const { return fabs((m_EtOuterConeEC+m_EtOuterConeHC)-(m_EtInnerConeEC+m_EtInnerConeHC))>0?((m_EtOuterConeEC+m_EtOuterConeHC)-(m_EtInnerConeEC+m_EtInnerConeHC)):m_sumet02; };
  inline float getsumet03 () const { return fabs((m_EtOuterConeEC+m_EtOuterConeHC)-(m_EtInnerConeEC+m_EtInnerConeHC))>0?((m_EtOuterConeEC+m_EtOuterConeHC)-(m_EtInnerConeEC+m_EtInnerConeHC)):m_sumet03; };
  inline float getsumet04 () const { return fabs((m_EtOuterConeEC+m_EtOuterConeHC)-(m_EtInnerConeEC+m_EtInnerConeHC))>0?((m_EtOuterConeEC+m_EtOuterConeHC)-(m_EtInnerConeEC+m_EtInnerConeHC)):m_sumet04; };

  /** Get Transverse Pt sum in cone. */
  inline float getsumpt01 () const { return m_SumPtTracksCone>0?0.0:m_sumpt01; };
  inline float getsumpt02 () const { return m_SumPtTracksCone>0?m_SumPtTracksCone:m_sumpt02; };
  inline float getsumpt03 () const { return m_SumPtTracksCone>0?m_SumPtTracksCone:m_sumpt03; };
  inline float getsumpt04 () const { return m_SumPtTracksCone>0?m_SumPtTracksCone:m_sumpt04; };

  /** Get Pt of charged track associated to muon */
  inline float getPtMuID()  const { return m_PtMuTracksCone>0?m_PtMuTracksCone:m_PtMuID; };
  /** Get Pt of max pt ID track in cone (muon excluded) */
  inline float getMaxPtID() const { return m_MaxPtID; };

  inline int getFlag()  const { return m_flag; };

  /** Get Isolation strategy (1->CombinedMuon seeded, 2->muFast Muon seeded, 3->L1 muon seeded) */
  int   iso_strategy(void)   const;
  /** Get Error Flag */
  int   iso_errorFlag(void)   const;

  /** Get Muon RoI Id*/
  inline int getRoiIdMu() const { return m_RoiIdMu; };
  /** Get Muon Pt. */
  inline float getPtMu()  const { return fabs(m_QMu)>0?m_PtMu*m_QMu:m_PtMu; };
  /** Get Muon Q. */
  inline float getQMu()  const  { return fabs(m_QMu)>0?m_QMu:(m_PtMu>0?1.0:-1.0); };
  /** Get Muon Eta. */
  inline float getEtaMu() const { return m_EtaMu; };
  /** Get Muon Phi. */
  inline float getPhiMu() const { return m_PhiMu; };

  //Obsolete (for backward compatibility)
  inline float getEtInnerConeEC   () const { return fabs(m_EtInnerConeEC)>0?m_EtInnerConeEC:0.0; };
  inline float getEtOuterConeEC   () const { return fabs(m_EtOuterConeEC)>0?m_EtOuterConeEC:m_sumet02; };
  inline float getEtInnerConeHC   () const { return fabs(m_EtInnerConeHC)>0?m_EtInnerConeHC:0.0; };
  inline float getEtOuterConeHC   () const { return fabs(m_EtOuterConeHC)>0?m_EtOuterConeHC:0.0; };
  inline int   getNTracksCone()      const { return m_NTracksCone>0?m_NTracksCone%100000:0; };
  inline float getSumPtTracksCone()  const { return m_SumPtTracksCone>0?m_SumPtTracksCone:m_sumpt02; };
  inline float getPtMuTracksCone()   const { return m_PtMuTracksCone>0?m_PtMuTracksCone:m_PtMuID; };
  inline float getLArWeight()        const { return m_LAr_w; }
  inline float getTileWeight()       const { return m_Tile_w; }

private:
  friend class IsoMuonFeatureCnv_p1;
  friend class IsoMuonFeatureCnv_p2;
  friend class IsoMuonFeatureCnv_p3;

  /** Calo based isolation */
  float m_sumet01;
  float m_sumet02;
  float m_sumet03;
  float m_sumet04;

  /** Track based isolation */
  float m_sumpt01;
  float m_sumpt02;
  float m_sumpt03;
  float m_sumpt04;

  /** Pt of charged track associated to the muon. */
  float m_PtMuID;
  /** pt of highest pt track in cone (excluded the one associated to the muon) */
  float m_MaxPtID;

  int   m_flag;

  // Muon info 
  /** ROI ID */
  int   m_RoiIdMu;
  /** Muon Pt */
  float m_PtMu;
  /** Muon Charge */
  float m_QMu;
  /** Muon Eta */
  float m_EtaMu;
  /** Muon Phi */
  float m_PhiMu;

  //obsolete
  float m_EtInnerConeEC;
  float m_EtOuterConeEC;
  float m_EtInnerConeHC;
  float m_EtOuterConeHC;
  int   m_NTracksCone;
  float m_SumPtTracksCone;
  float m_PtMuTracksCone;
  float m_LAr_w;
  float m_Tile_w;
        
};

/// Helper function for printing the object
std::string str( const IsoMuonFeature& d );
/// Helper operator for printing the object
MsgStream& operator<< ( MsgStream& m, const IsoMuonFeature& d );

/// Operator comparing two IsoMuonFeature objects for equality
bool operator== ( const IsoMuonFeature& a, const IsoMuonFeature& b );
/// Operator comparing two IsoMuonFeature objects for inequality
inline bool operator!= ( const IsoMuonFeature& a, const IsoMuonFeature& b ) {
  return !( a == b );
}

/// Comparison with feedback
void diff( const IsoMuonFeature& a, const IsoMuonFeature& b,
           std::map< std::string, double >& variableChange ); 

CLASS_DEF( IsoMuonFeature , 110010498 , 1 )
CLASS_DEF( DataVector<IsoMuonFeature> , 247013301 , 1 )

#include "TrigMuonEvent/IsoMuonFeatureContainer.h"

#endif // TRIGMUONEVENT_ISO_MUON_FEATURE_H
