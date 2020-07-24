/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:     TrigEFMissingETHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigMissingETHypo
 *
 * AUTHOR:   Kyle Cranmer
 * CREATED:  Oct 10, 2006
 *
 *  $Id: TrigEFMissingETHypo.h,v 1.13 2009-01-22 14:53:54 casadei Exp $
 *********************************************************************/
#ifndef TRIGEFMISSINGETHYPO_H
#define TRIGEFMISSINGETHYPO_H


#include "TrigInterfaces/HypoAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

class StoreGateSvc;
class TriggerElement;

/**
    @class TrigEFMissingETHypo

    This is the MET EF and LVL2 hypothesis algorithm.

    Trigger signature setup files can pass 4 variables:
     - double MEtCut is the threshold for missing Et.
     - double SumEtCut is the threshold for the Et sum.
     - double CutType is a flag:
        * CutType < -1.5 results in an OR of MEt and SumEt threshold cuts
        * -1.5 < CutType < -0.5 results in an AND of the two cuts
        * -.5 < CutType <-0.5   results in rejecting ALL events
        * -.5 < CutType <-0.5   results in rejecting ALL events
        *  .5 < CutType         results in a cut on Significance
    - bool doMuonCorrection is a flag that enables/disables the muon correction

    The inputs from the FEX algorithms are currently MET, METx, METy, and
    SumET. Eventually also SigmaMET.

    @author Kyle Cranmer, Diego Casadei

*/

class TrigEFMissingETHypo : public HLT::HypoAlgo {

 public:

  TrigEFMissingETHypo(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi Algorithm constructor

  HLT::ErrorCode hltInitialize(); //!< hlt initialize, setting some counters to 0
  HLT::ErrorCode hltFinalize();   //!< hlt finalize, doing essentially nothing

    /**
     * @brief implementation of the abstract hltExecute method in HLT::Hypo
     *
     * @param outputTE the output TE holding the attached features
     * @param pass     result of this hypothesis algorithm
     */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool & pass);


 private:

  std::string  m_featureLabel; //!< label for the MET feature in the HLT Navigation

  // configurables:
  double m_MEtCut;   //!< cut value for the MissingEt [MeV]
  double m_SumEtCut; //!< cut value for the SumEt [MeV]
  double m_SigCut;   //!< cut value for the Sig

  /** cut type < -1.5 => MissingEt OR SumEt;
   ** -1.5 < cut type < -0.5 => MissingEt AND SumEt;
   ** -0.5 < cut type < 0.5  => Reject All Events
   ** cut type > 0.5 => Significance
   **/
  double m_CutType;


  // Configurables for Significance cut 

  double m_significanceOffset;
  double m_significanceSlope;
  double m_significanceQuadr;
    
  double m_xsMETmin;
  double m_xsSETmin;

  double m_xsMETok;
  double m_xsSETok;

  /** Extended use: look only at the central region.
   ** central == 0 (default): do nothing special
   ** central < 0: subtract FCAL before comparing to the thresholds
   ** central == x > 0: require FCAL_MET < x/GeV (in addition to thr. comparison)
   **/
  double m_central;

  /** Extended use: veto on the central region.
   ** forward == 0 (default): do nothing special
   ** forward == x < 0: require central_MET < |x| GeV
   ** forward == x > 0: require caloCentral_MET < x GeV AND muon_MET < x GeV
   ** NB: if forward != 0 other thresholds ignored
   **/
  double m_forward;

  bool m_doMuonCorrection; //!< switch on/off muon correction
  bool m_doEMScaleTC;        //!< switch on/off the use of EM scale information (in topo. clusters)
  bool m_doOnlyCalcCentralMET; //!< calculate MET only with central contributions

  bool m_doLArH11off; //! LAr H11 off or not 
  bool m_doLArH12off; //! LAr H12 off or not
  bool m_doMETphicut; //! switch on/off MET phi cut for LAr H11/H12

  bool m_monitorMeasuredEnergies;  // Heavy Ion Monitoring
  
  bool  m_doL1L2FEBTest;        // L1=L2 vs FEB MET test
  double m_L1L2FEBTolerance;    // L1=L2 vs FEB tolerance

  /** Extended use: veto based on the event status flag.
   ** bitMask == 0 (default): do nothing special
   ** To avoid problems due to the implicit int to long int
   ** conversion in Python, the actual configurables are the
   ** lowest and highest 16-bits (never converted to long),
   ** i.e. bitMaskComp and bitMaskGlob.  They will be put
   ** together into bitMask = glob<<16 | comp at initialization
   **/
  unsigned int m_bitMask;
  unsigned int m_bitMaskComp;
  unsigned int m_bitMaskGlob;

  int m_accepted;   //!< number of accepted events
  int m_rejected;   //!< number of rejected events
  int m_errors;     //!< number of errors

  // monitored variables
  float  m_ex_log;     //!< Monitoring: signed log10 of Missing Ex [GeV]
  float  m_ey_log;     //!< Monitoring: signed log10 of Missing Ey [GeV]
  float  m_ez_log;     //!< Monitoring: signed log10 of Missing Ez [GeV]
  float  m_et_log;     //!< Monitoring: log10 of Missing Et [GeV]
  float  m_sumet_log;  //!< Monitoring: log10 of SumEt [GeV]
  float  m_ex_lin;     //!< Monitoring: Missing Ex [GeV]
  float  m_ey_lin;     //!< Monitoring: Missing Ey [GeV]
  float  m_ez_lin;     //!< Monitoring: Missing Ez [GeV]
  float  m_et_lin;     //!< Monitoring: Missing Et [GeV]
  float  m_sumet_lin;  //!< Monitoring: SumEt [GeV]
  float  m_xs;         //!< Monitoring: EF XS [GeV^1/2]
  float  m_xs2;         //!< Monitoring: EF XS [GeV^1/2]
  float  m_phi;        //!< Monitoring: MET phi [rad]

  /** Monitoring: -1: error;   0: event rejected;
   **  1: MET passed; 2: SumET (but not MET) passed; [cut type < -1.5]
   **  3: both passed [-1.5 < cut type < -0.5]
   **  4: forward event
   **/
  int    m_cutCounter;

  // Timing:
  TrigTimer*  m_totTime;

};
#endif
