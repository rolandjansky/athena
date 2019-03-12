/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibHvTool.h
 * Header file for the SCT_CalibHvTool class
 * @author Shaun Roe
 **/

#ifndef SCT_CalibHvTool_h
#define SCT_CalibHvTool_h

//Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

//Inner detector includes
#include "InDetIdentifier/SCT_ID.h"

//local includes
#include "SCT_CalibAlgs/ISCT_CalibHistoTool.h"
#include "SCT_ConditionsTools/ISCT_DCSConditionsTool.h" //template parameter

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

//STL includes
#include <string>
#include <vector>
#include <queue>
#include <utility> //for 'pair'

//fwd declarations
class StatusCode;
class ISvcLocator;
class IdentifierHash;
class EventInfo;

class SCT_CalibHvTool : public extends<AthAlgTool, ISCT_CalibHistoTool>
{

 public:
  //@name Tool methods, reimplemented
  //@{
  SCT_CalibHvTool(const std::string&, const std::string&, const IInterface*);
  virtual ~SCT_CalibHvTool() = default;
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  //@}
  //@name ISCT_CalibHistoTool methods, reimplemented
  //@{
  virtual bool book();
  virtual bool fill(const bool fromData=false);
  virtual bool fillFromData();
  //@}

 private:
  enum {n_chipPerSide = 6, n_chipPerModule = 12, n_stripPerChip = 128, n_etaInBarrel = 12,
        n_phiBinsB0 = 32, n_phiBinsB1 = 40, n_phiBinsB2 = 48, n_phiBinsB3 = 56,
        n_phiBinsECShort = 40, n_phiBinsECMiddle = 40, n_phiBinsECOuter = 52, n_elements=8176,
        n_BSErrorType = 15, firstBSErrorType = 0, lastBSErrorType = 14
  };

  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<StoreGateSvc> m_evtStore;
  ToolHandle<ISCT_DCSConditionsTool> m_DCSConditionsTool{this, "SCT_DCSConditionsTool", "SCT_DCSConditionsTool", "Tool to retrieve SCT DCS information"};

  const SCT_ID* m_pSCTHelper;
  SCT_ID::const_id_iterator m_waferItrBegin;
  SCT_ID::const_id_iterator m_waferItrEnd;
  typedef std::vector<int> VecInt;
  VecInt* m_sct_waferHash;
  VecInt* m_sct_numHitsInWafer;

  //private use in this class
  int m_maxq;
  // For HV trips
  std::vector<std::queue<int>> m_phvtripQueue;
  std::queue<int> m_prevLBN;
  VecInt m_phvtripRunningTotalInt;
  VecInt m_phvtripProcessedEventsInt;
  VecInt m_phvtripHasItTripped;
  VecInt m_phvtripHasItTripped_prev;
  //
  int m_phvtripPrevTime;
  int m_phvtripFirstTime;
  double m_absolutetriplimit;
  double m_relativetriplimit;
  int m_tq[100];

  std::vector<std::vector<std::pair<int, int>>> m_summarytrips;
  std::vector<std::vector<std::pair<int, int>>> m_summarytripslb;
  const EventInfo* m_evt;
  bool m_outputLowHits;
  int m_lowHitCut;
  ///retrieve a tool and report if it failed
  template<class T>
    bool retrievedTool(T & tool, const std::string & toolName) {
    if (tool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve " << toolName);
      return false;
    }
    return true;
  }

};
#endif
