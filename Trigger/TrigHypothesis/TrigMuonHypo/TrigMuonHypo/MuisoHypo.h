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
// PURPOSE:  LVL2 Muon Isolation Hypothesis Algorithm: V4.0
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
    
    /** Max ID Isolation regions 1/2/3 */
    FloatProperty   m_MaxIDIso_1;
    FloatProperty   m_MaxIDIso_2;
    FloatProperty   m_MaxIDIso_3;

    /** Max Eta regions 1/2/3 */
    FloatProperty   m_EtaRegion_1;
    FloatProperty   m_EtaRegion_2;
    FloatProperty   m_EtaRegion_3;

    /** Isolation cone sizes (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)  */
    IntegerProperty m_IDConeSize;

    // Other members:   
    StoreGateSvc*   m_storeGate;

    /** Cut counter. */
    int   m_cutCounter;
    /** Transverse Momentum sum ID */ 
    float m_SumPtCone;

};
#endif
