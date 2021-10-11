/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArCoherentNoisefractionMonAlg
 * @author  Pavol Strizenec, based on codes from M. Spalla and Y. Enari
 *
*/

#ifndef LARMONITORING_LARCNFMON_H
#define LARMONITORING_LARCNFMON_H

//inheritance:
#include "AthenaMonitoring/AthMonitorAlgorithm.h"


//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


//STL:
#include <string>



class LArOnlineID;
class HWIdentifier;


class LArCoherentNoisefractionMonAlg final: public AthMonitorAlgorithm
{
  
  
public:
  LArCoherentNoisefractionMonAlg(const std::string& name, ISvcLocator* pSvcLocator);
  
  /** @brief Default destructor */
  virtual ~LArCoherentNoisefractionMonAlg();
  
  
   
  virtual StatusCode initialize() override;
  virtual StatusCode fillHistograms(const EventContext& ctx) const override;

  
private:
  
  /** services  */
  const LArOnlineID* m_LArOnlineIDHelper;

  /** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};

  /** Handle to event info */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfo", "EventInfo"};
  
  
  /**correlation histograms*/

  /** list of FEBs to monitor. FEB names are expected to be strings of the form used in 'febString' method, e.g.  'BarrelCFT00Slot02'  */

  Gaudi::Property<bool> m_plotCustomFEBSset {this, "PlotCustomFEBSset", false}; 
  Gaudi::Property<std::vector<std::string> > m_FEBlist {this, "FEBlist", std::vector<std::string>(0)};
  Gaudi::Property<std::vector<std::string> > m_groupNames {this, "ListOfGroupNames", std::vector<std::string>(0)};
  Gaudi::Property<std::vector<unsigned> > m_groupNChan {this, "GroupNchan", std::vector<unsigned>(0)};
  Gaudi::Property<std::vector<bool> > m_processGroup {this, "GroupsToMonitor", std::vector<bool>(0)};


  /** to avoid asking for triggers in case of a calibration run*/
  Gaudi::Property<bool> m_isCalibrationRun {this, "IsCalibrationRun", false};

  /** Handle to digits */
  SG::ReadHandleKey<LArDigitContainer> m_LArDigitContainerKey{this,"LArDigitContainerKey","FREE","SG key of LArDigitContainer read from Bytestream"};
  
  /** the group array **/
  std::vector<std::map<std::string,int> > m_histoGroups;

  std::string febString(const HWIdentifier) const;

  std::vector<HWIdentifier> m_febMap;

  double calc_dev(std::vector<float> *input_vector) const;
  double calc_sum_dev(std::vector<float> *input_vector) const;

};

#endif
