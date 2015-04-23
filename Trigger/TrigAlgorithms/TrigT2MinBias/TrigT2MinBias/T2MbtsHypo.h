/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2_MBTS_HYPO_H
#define T2_MBTS_HYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT2MinBias/T2MbtsUtils.h"
#include <map>

// namespace xAOD {
//   class TrigT2MbtsBits;
// }

/** @class T2MbtsHypo
 
@author W. H. Bell <W.Bell@cern.ch>

A hypothesis algorithm to read the MBTS DSP energy and time and form a
trigger decision based on the required constraints.
*/
class T2MbtsHypo: public HLT::HypoAlgo, public T2MbtsUtils {
  public:
   T2MbtsHypo(const std::string &name, ISvcLocator* pSvcLocator);
 
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
   
   /** The number of MBTS counters required to be above threshold */
   unsigned int m_mbtsCounters;
   
   /** A flag to control if this is a coincidence requirement or not */
  bool m_coincidence;
  /** Flag extending above choice to do Or between the two sides of MBTS */   
  bool m_or; 
  /** Flag to select lowe than threshold multiplicities */
  bool m_veto;
   /** Flag to count multiplicities only on inner or outer MBTS modules */
  int m_mbtsmode;
   /** A pointer to the TrigT2MbtsBits object. */
   const xAOD::TrigT2MbtsBits *m_t2MbtsBits; 

   /** for monitoring */
   int m_selMultEBA;
   int m_selMultEBC;

};

#endif
