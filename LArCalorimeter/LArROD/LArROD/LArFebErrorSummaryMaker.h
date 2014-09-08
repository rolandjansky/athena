/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARROD_LARFEBERRORSUMMARYMAKER
#define LARROD_LARFEBERRORSUMMARYMAKER


/********************************************************************

NAME:     LArFebSummaryMaker
          Algorithm that makes LArFebSummary 

********************************************************************/

#include <AthenaBaseComps/AthAlgorithm.h>
#include <GaudiKernel/ServiceHandle.h>
#include <GaudiKernel/ToolHandle.h>

#include "LArRawEvent/LArFebHeaderContainer.h"

#include <vector>
#include <set>

class LArOnlineID       ; 
class StoreGateSvc ; 
class ILArBadChanTool;

class LArFebErrorSummaryMaker : public AthAlgorithm
{

 public:

  LArFebErrorSummaryMaker(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~LArFebErrorSummaryMaker();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  bool masked (unsigned int hid, const std::vector<unsigned int>& v_feb) const; 

  const LArOnlineID* m_onlineHelper ; 

  int m_nwarns;
  int m_warnLimit;

  int m_missingFebsWarns;

  std::vector<int> m_errors; 

  // ignore these FEBs for these errors
  std::vector<unsigned int> m_knownEvtId;
  std::vector<unsigned int> m_knownSCACStatus;
  std::vector<unsigned int> m_knownZeroSample;

  bool m_checkAllFeb; 
  std::string m_partition;
  std::set<unsigned int> m_all_febs ; 

  ToolHandle<ILArBadChanTool> m_badChannelTool;

};
#endif










