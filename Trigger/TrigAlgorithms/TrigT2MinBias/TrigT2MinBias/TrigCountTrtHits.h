/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMINBIAS_TRIGCOUNTTRTHITS_H
#define TRIGMINBIAS_TRIGCOUNTTRTHITS_H

#include <string>
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"

class TRT_ID;
class TrigTrtHitCounts;
class TrigHisto1D;

/** @class TrigCountTrtHits

@author Regina Kwee <rkwee@cern.ch>
@author Samir Ferrag <Samir.Ferrag@cern.ch>
@author Will Bell <W.Bell@cern.ch>

A feature extraction algorithm to extract the time over threshold
distributions for TRT raw data objects.
*/
class TrigCountTrtHits: public HLT::AllTEAlgo {
 public:
  
  TrigCountTrtHits(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor
  virtual ~TrigCountTrtHits(void);
  
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltBeginRun();  
  HLT::ErrorCode hltFinalize();  
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
  StatusCode fillCollections( bool, bool, std::vector<int>&, std::vector<int>& );
  
  /**
   * @brief This method overwrites the default one (doing nothing) in the algo.h class
   *
   * This is used to reset the internal caching mechanism of this algorithm
   */
  HLT::ErrorCode hltEndEvent();
  
 private:

  HLT::ErrorCode checkDetectorMask();
  Bool_t m_hltExecuteInitialisationRun; //!< Flag to run extra initialisation on the first event when xAOD::EventInfo is present

  /** A data member to retain a connection to the MsgStream
      service. */
  MsgStream m_log;
  
  /** A data member to retain a connection to the Detector Store. */
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_storeGate;

  /** For access to the TRT identifiers */
  const TRT_ID *m_trtHelper;

  ToolHandle<ITrigTRT_DriftCircleProviderTool> m_rawDataTool; 
  std::vector<int> m_trtDataErrors;
 
  /** A histograms of time over threshold for TRT RDO in the endcap C */
  TrigHisto1D *m_endcapC;

  /** A histograms of time over threshold for TRT RDO in the barrel */
  TrigHisto1D *m_barrel;

  /** A histograms of time over threshold for TRT RDO in the endcap A */
  TrigHisto1D *m_endcapA;

  /** TrigHisto input parameters */
  unsigned int m_hTotBins;
  float m_hTotMin;
  float m_hTotMax;

  /** A pointer to the TRT_RDO_Container */
  std::string m_trtRdoContainerName;
  
  /** detector mask flags */
  bool m_trt_barrel_a_side;
  bool m_trt_barrel_c_side;
  bool m_trt_endcap_a_side;
  bool m_trt_endcap_c_side;
  
  /** trigger timers
      the sum will give the total time of the main TRT part */
  TrigTimer* m_dataLoopTimer;
  TrigTimer* m_retrieveDataTimer;
  TrigTimer* m_attachFTimer;

  /** Variables used to monitor data stored in TrigTrtHitCounts */ 
  std::vector<float> m_endcapC_Tot; 
  std::vector<float> m_barrel_Tot; 
  std::vector<float> m_endcapA_Tot; 

  /** Number total number of TRT hits used for basic monitoring and debugging */
  unsigned int m_nTrtHits;
  
  bool m_useCachedResult;          //!< internal caching: true when the hltExecute will run in cached mode.
  TrigTrtHitCounts* m_trtHitCounts; //!< internal caching: SpacePoint feature from the first execution.
  HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution.
};

#endif
