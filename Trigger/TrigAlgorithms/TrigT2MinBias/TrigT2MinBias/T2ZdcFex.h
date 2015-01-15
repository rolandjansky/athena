/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2_ZDC_FEX_H
#define T2_ZDC_FEX_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "ZdcEvent/ZdcRawChannelCollection.h"
#include "xAODTrigMinBias/TrigT2ZdcSignals.h"
#include <vector>

//#define  MY_DEBUG

class ZdcID;

/*
  
@author D. O. Damazio <Denis.Oliveira.Damazio@cern.ch>

A feature extraction algorithm to retrieve the ZDC Cells
*/
class T2ZdcFex: public HLT::AllTEAlgo {
 public:
  T2ZdcFex(const std::string &name, ISvcLocator* pSvcLocator);
  

  /**
   * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
   *
   * @param input outer vector describeds the different input TE types,
   * here we expect: 1st LVL1 energy TE; 2nd Muons;
   * inner vector provides all TE instances of the given type
   *
   * @param output the output TE type
   */
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
			    unsigned int type_out);
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  
  /**
   * @brief This method overwrites the default one (doing nothing) in the algo.h class
   *
   * This is used to reset the internal caching mechanism of this algorithm
   */
  HLT::ErrorCode hltEndEvent();
  

 private:
  
  /** A data member to retain a connection to the MsgStream
      service. */
  MsgStream m_log;
  
  /** iterators to ZdcCells. To be used when online
      are to be used.
      Used as input for TrigDataAccess::LoadCollections */
  ZdcRawChannelCollection::const_iterator m_zBegin;
  ZdcRawChannelCollection::const_iterator m_zEnd;
  ZdcRawChannelCollection::const_iterator m_zt;
  
  TrigTimer *m_timerLoadColl;
  TrigTimer *m_timerAlg;
  TrigTimer *m_timerSave;   
  
  /** Object  that provides data access in a Region of
      Interest. See TrigDataAccess for more details. */
  ToolHandle<ITrigDataAccess> m_data;
  
  /** zdc ID manipulation to get some geometry */
  const ZdcID* m_zdcID;
  

  /** Energy deposited in each detector */ 
  std::vector<float> m_triggerEnergies;
  
  /** Relative trigger times with repect to collision timing */
  std::vector<float> m_triggerTimes;

  /** number of entries per module */
  std::vector<int> m_triggerEntries;

  bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode.
  xAOD::TrigT2ZdcSignals *m_zdcSignals;    //!< internal caching: ZDC feature from the first execution.
  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution.
  int m_ZdcEnRecoOpt; // ZDC Reconstruction options  (Energies)
  int m_ZdcTimeRecoOpt; // ZDC Reconstruction options (Time)
  float m_en_threshold, m_time_threshold, m_chi_threshold;
};

#endif
