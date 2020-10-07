/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArNoiseCorrelationMonAlg
 * @author Margherita Spalla 
 * base on LArNoiseCorrelationMon
 *
 */

#ifndef LARMONITORING_LARCORRMON_H
#define LARMONITORING_LARCORRMON_H

//inheritance:
#include "AthenaMonitoring/AthMonitorAlgorithm.h"


//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


//STL:
#include <string>



class LArOnlineID;
class HWIdentifier;


class LArNoiseCorrelationMonAlg final: public AthMonitorAlgorithm
{
  
  
public:
  LArNoiseCorrelationMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Default destructor */
  virtual ~LArNoiseCorrelationMonAlg();
  
  
   
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  
private:
  
  /** services  */
  const LArOnlineID* m_LArOnlineIDHelper;

  /** Handle to bad-channel mask */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;

  /** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};

  /** Handle to event info */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfo", "EventInfo"};
  
  
  /**correlation histograms*/

  /** list of FEBs to monitor. FEB names are expected to be strings of the form used in 'febString' method, e.g.  'BarrelCFT00Slot02'  */

  Gaudi::Property<std::vector<std::string > >  m_FEBsToMonitor {this, "FEBsToMonitor", std::vector<std::string>(0)};
  Gaudi::Property<bool> m_plotCustomFEBSset {this, "PlotCustomFEBSset", false}; 
  Gaudi::Property<std::vector<std::string> > m_FEBlist {this, "FEBlist", std::vector<std::string>(0)};


 Gaudi::Property<bool> m_plotsOFF {this, "PlotsOFF", false}; //to avoid plotting everything when online or in case the wrong custom list is passed

  /** to avoid asking for triggers in case of a calibration run*/
 Gaudi::Property<bool> m_isCalibrationRun {this, "IsCalibrationRun", false};

  
  /**bool use to mask the bad channels*/
  Gaudi::Property<bool>        m_ignoreKnownBadChannels{this, "IgnoreBadChannels", false}; 

  /** Handle to digits */
  SG::ReadHandleKey<LArDigitContainer> m_LArDigitContainerKey{this,"LArDigitContainerKey","FREE","SG key of LArDigitContainer read from Bytestream"};
  
  /** the group array **/
  std::map<std::string,int> m_noiseCorrGroups;

  /** Declare methods used*/
  bool isGoodChannel(const HWIdentifier id,const float ped,const LArOnOffIdMapping *cabling) const;
  std::string febString(const HWIdentifier) const;


};

#endif
