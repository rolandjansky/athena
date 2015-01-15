/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2ZDCHYPO_H
#define T2ZDCHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"

#include <map>

#define ZDC_MOD 4
//#define MY_DEBUG
/** @class T2ZdcHypo
 
@author Antonio Sidoti <antonio.sidoti@roma1.infn.it> 
dato che Will Bell ha fatto solo danni


A hypothesis algorithm to read the ZDC energy and time and form a
trigger decision based on the required constraints.
*/
class T2ZdcHypo: public HLT::HypoAlgo {
  public:
   T2ZdcHypo(const std::string &name, ISvcLocator* pSvcLocator);
 
   HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, 
			     bool& pass);
   HLT::ErrorCode hltInitialize();
   HLT::ErrorCode hltFinalize();

 private:

   /** A data member to retain a connection to the MsgStream
       service. */
   MsgStream m_log;
   
   /** A flag to allow this Hypo to be disabled. */
   bool m_acceptAll;
   
   
   /** Flags to control the logic of the selection */
  int m_timeLogic;
  int m_energyLogic;
  int m_multiLogic;


  /** Monitored variables */
  int m_selMult_A, m_selMult_C;
  float m_selMeanTime_A, m_selMeanTime_C;
  float m_selSumEn_A, m_selSumEn_C;
  float m_seltimeDiff_A_C;

   /** A pointer to the TrigT2ZdcSignals object. */
   const xAOD::TrigT2ZdcSignals *m_t2ZdcSignals; 


   /** Energy threshold in pC */
  //   float m_threshold;
   
   
   
   /** Offsets with respect to the global offset of all counters. */
   std::vector<float> m_timeOffset;
  
  /** Energy Pedestals */
   std::vector<float> m_EnergyPed;

  /** Energy Calibrations */
   std::vector<float> m_EnergyCal;
  
  /** cut on Energy and Multiplicities */
  
  std::vector<float> m_SumEn;
  std::vector<float> m_Mult;
  
   /** A time cut in ns.  Values <= 0 disable the time cut */
   float m_timeCut;
  
   /** Time difference from A to C side */
   float m_timeDiff_A_C;
   
  /** Hit multiplicity after energy and optional time cut. */ 
  std::pair<int, int> m_mult;
  
  /** Mean time after energy and optional time cut. */ 
  std::pair<float, float> m_meanTime;
  
  /** Energy sum after energy and optional time cut. */ 
  std::pair<float, float> m_sumEn;
  /** cut on time_difference of modules */
  float m_timeModuleCut;
    
  int calculateSumMean();

};

#endif
