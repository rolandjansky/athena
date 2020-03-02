/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBPEDMONALG_H
#define LARCALIBPEDMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "StoreGate/ReadHandleKey.h"
#include "LArRawEvent/LArCalibDigitContainer.h"
#include "LArRawEvent/LArAccumulatedDigitContainer.h"
#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"

#include <string>




class LArCalibPedMonAlg: public AthMonitorAlgorithm
{
 public:
  LArCalibPedMonAlg(const std::string& name,ISvcLocator* pSvcLocator );		      

  /** @brief Default destructor */
  virtual ~LArCalibPedMonAlg();

  /** @brief Overwrite dummy method from AlgTool */
  virtual StatusCode initialize() override;


  /** Called each event */
  virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

 private:

  // keys to access info
  SG::ReadHandleKey<LArCalibDigitContainer> m_calibDigitContainerKey{this,"LArCalibDigitContainerKey","","SG key of LArCalibDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArAccumulatedDigitContainer> m_accDigitContainerKey{this,"LArAccumulatedDigitContainerKey","","SG key of LArAccumulatedDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArAccumulatedCalibDigitContainer> m_accCalibDigitContainerKey{this,"LArAccumulatedCalibDigitContainerKey","","SG key of LArAccumulatedCalibDigitContainer read from Bytestream"};

  // Properties
  //MonGroup(s) name
  Gaudi::Property<std::string> m_MonGroupName {this,"LArPedGroupName","LArPedMonGroup"};

};

#endif

