/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArCalibDelayMonAlg_H
#define LArCalibDelayMonAlg_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"


#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include <string>
#include <vector>
#include <map>

class LArOnlineID;


class LArCalibDelayMonAlg: public AthMonitorAlgorithm
{
 public:
  LArCalibDelayMonAlg(const std::string& name,ISvcLocator* pSvcLocator );		      

  /** @brief Default destructor */
  virtual ~LArCalibDelayMonAlg() = default;

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override;


  // Called each event
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  // Handle to digits 
  SG::ReadHandleKey<LArAccumulatedCalibDigitContainer> m_accCalibDigitContainerKey{this,"LArAccumulatedCalibDigitContainerKey","","SG key of LArAccumulatedCalibDigitContainer read from Bytestream"};
  
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
};

#endif

