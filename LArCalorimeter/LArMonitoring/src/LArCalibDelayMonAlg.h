/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCalibDelayMonAlg_H
#define LArCalibDelayMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArFebHeader.h"
#include "xAODEventInfo/EventInfo.h"

#include "CaloIdentifier/CaloGain.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "LArElecCalib/ILArPedestal.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"

#include <string>
#include <vector>

class LArCalibDelayMonAlg: public AthMonitorAlgorithm
{
 public:
  LArCalibDelayMonAlg(const std::string& name,ISvcLocator* pSvcLocator );		      

  /** @brief Default destructor */
  virtual ~LArCalibDelayMonAlg();

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override;


  // Called each event
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  // Handle to digits 
  SG::ReadHandleKey<LArCalibDigitContainer> m_calibDigitContainerKey{this,"LArCalibDigitContainerKey","","SG key of LArCalibDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArAccumulatedDigitContainer> m_accDigitContainerKey{this,"LArAccumulatedDigitContainerKey","","SG key of LArAccumulatedDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArAccumulatedCalibDigitContainer> m_accCalibDigitContainerKey{this,"LArAccumulatedCalibDigitContainerKey","","SG key of LArAccumulatedCalibDigitContainer read from Bytestream"};
  
  SG::ReadHandleKey<LArFebHeaderContainer> m_hdrContKey{this, "LArFebHeaderKey", "LArFebHeader"};
  
  // MonGroup(s) name
  Gaudi::Property<std::string> m_MonGroupName {this,"LArDelayGroupName","LArDelayMonGroup"};
  
  // Properties
  Gaudi::Property<std::vector<std::string> > m_partitions {this, "PartitionNames", {} };
  Gaudi::Property<std::vector<std::string> > m_SubDetNames{this, "SubDetNames", {} };
  
//Histogram grouping (part)
  std::vector<std::map<std::string,int> > m_histoGroups;
  
  unsigned int returnPartition(int be,int pn,int ft,int sl) const;
  
    // Id helper
  const LArOnlineID* m_onlineHelper;
  const LArEM_ID*    m_EMHelper;
  const LArHEC_ID*   m_HECHelper;
  const LArFCAL_ID*  m_FCALHelper;
    
};

#endif

