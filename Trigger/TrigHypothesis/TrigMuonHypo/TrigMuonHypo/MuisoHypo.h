/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MuisoHypo.h
// PACKAGE:  Trigger/TrigHypotheis/TrigMuonHypo/MuisoHypo
//
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
//
// PURPOSE:  LVL2 Muon Isolation Hypothesis Algorithm: V3.5
// ********************************************************************
#ifndef TRIG_MUISOHYPO_H 
#define TRIG_MUISOHYPO_H

#include <string>
#include "TrigInterfaces/HypoAlgo.h"

class StoreGateSvc;

/**
 * \class MuisoHypo
 * \brief MuisoHypo is a Trigger Hypo Algorithm that retrieves the L2 IsoMuonFeature Container
 * created by TrigmuIso and then applies a subset of muon isolation selection cuts.
 * A TE will be set active if the selection cuts are fullfilled.
 *
 *
 */
class MuisoHypo: public HLT::HypoAlgo {
  enum { MaxNumberTools = 20 };  
  public:
    /** Constructor. */
    MuisoHypo(const std::string & name, ISvcLocator* pSvcLocator);
    /** Destructor. */
    ~MuisoHypo();

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
    
    /** Max Calorimetric Isolation regions 1/2/3 */
    FloatProperty   m_MaxCaloIso_1;
    FloatProperty   m_MaxCaloIso_2;
    FloatProperty   m_MaxCaloIso_3;
    /** Max ID Isolation regions 1/2/3 */
    FloatProperty   m_MaxIDIso_1;
    FloatProperty   m_MaxIDIso_2;
    FloatProperty   m_MaxIDIso_3;

    /** Max Eta regions 1/2/3 */
    FloatProperty   m_EtaRegion_1;
    FloatProperty   m_EtaRegion_2;
    FloatProperty   m_EtaRegion_3;

    /** Isolation cone sizes (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)  */
    IntegerProperty m_CaloConeSize;
    IntegerProperty m_IDConeSize;


    /** Alternative isolation algorithms (for exotic analyses) */
    BooleanProperty m_UseHollowConeCalo;
    IntegerProperty m_HollowConeCaloInner;
    IntegerProperty m_HollowConeCaloOuter;
    BooleanProperty m_UseHollowConeID;
    IntegerProperty m_HollowConeIDInner;
    IntegerProperty m_HollowConeIDOuter;
    BooleanProperty m_RemoveMaxPtTrack;
    BooleanProperty m_UseMaxPtTrackAlgo;
    BooleanProperty m_DoNotSubtractMuonPt;

    /** Apply Calo based Isolation. */
    BooleanProperty m_UseCalo;
    /** Use Absolute Calo Isolation. */
    BooleanProperty m_UseAbsCalo;
    /** Apply ID based Isolation. */
    BooleanProperty m_UseID;
    /** Use Absolute ID Isolation. */
    BooleanProperty m_UseAbsID;
    /** Use pt measured in ID as muon Pt */
    BooleanProperty m_UseIDptForMu;

    // Other members:   
    StoreGateSvc*   m_storeGate;

    /** Cut counter. */
    int   m_cutCounter;
    /** Transverse Energy sum calo */ 
    float m_SumEtCone;
    /** Transverse Momentum sum ID */ 
    float m_SumPtCone;
    /** Isolation strategy */
    int   m_Strategy;

};
#endif
