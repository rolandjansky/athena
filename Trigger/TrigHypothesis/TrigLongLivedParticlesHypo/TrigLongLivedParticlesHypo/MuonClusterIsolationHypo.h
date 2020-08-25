/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MuonClusterIsolationHypo.h
// PACKAGE:  Trigger/TrigHypotheis/TrigMuonHypo
//
// AUTHORS:   S. Giagu <stefano.giagu@cern.ch>
//            A.Policicchio <antonio.policicchio@cern.ch>
//
// PURPOSE:  LVL2 Muon Cluster Hypothesis Algorithm: V1.0
// ********************************************************************

#ifndef TRIG_MUCLUISOHYPO_H 
#define TRIG_MUCLUISOHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;

/*
 * \class MuonClusterHypo
 * \brief MuonClusterIsolationHypo is a Trigger Hypo Algorithm that retrieves the L1 Muon RoIs and then 
 *        requires that these L1 RoIs form a cluster with number of RoI larger than 3, with no Jets with Log(H/E)<0.5 in the cluster cone and no SITRACKs around the cluster direction. 
 *        A TE will be set active if the selection cuts are fullfilled.
*/

class MuonClusterIsolationHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    /** Constructor. */
    MuonClusterIsolationHypo(const std::string & name, ISvcLocator* pSvcLocator);
    /** Destructor. */
    ~MuonClusterIsolationHypo();

    /** HLT method to initialize. */
    HLT::ErrorCode hltInitialize();    
    /** HLT method to execute. */ 
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass); 
    /** HLT method to finalize. */
    HLT::ErrorCode hltFinalize();

  private:

    // Properties
    /** Accept all events flag. */
    BooleanProperty m_acceptAll;
    /*do isolation*/
    BooleanProperty m_doIsolation;
    /** Min number of RoIs in the cluster. */   
    IntegerProperty m_nRoIEndCap;
    IntegerProperty m_nRoIBarrel;
    /** Eta of the cluster. */   
    DoubleProperty m_etaMid;
    DoubleProperty m_etaMax;
    /** Number of Jets with Log(H/E)<0.5 in the cluster. */   
    IntegerProperty m_nJet;
    /** Number of tracks in ID */   
    IntegerProperty m_nTrk;



  /** Numbers of Roi in cluster */
  int m_CluNum;
  /** Number of Jet with Log(h/e)<0.5 **/
  int m_NumJet;
  /** Cut on Tracks */
  int m_NumTrk;


    // Other members:   
    StoreGateSvc*   m_storeGate;
    
    /** Cut counter. */
    int             m_cutCounter;

};
#endif

