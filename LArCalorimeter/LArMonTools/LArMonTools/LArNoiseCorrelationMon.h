/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * TBD: update
 * @class LArNoiseCorrelationMon
 * @author Louis Helary 
 *
 */

#ifndef LARCORRMON_H
#define LARCORRMON_H

//inheritance:
#include "AthenaMonitoring/ManagedMonitorToolBase.h"


//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "LArRecConditions/ILArBadChannelMasker.h"



//STL:
#include <string>



class LArOnlineID;
class HWIdentifier;
class LArOnlineIDStrHelper;
class LArCablingService;
class TProfile2D_LW;
class TProfile_LW;
class TH2F_LW;

class LArNoiseCorrelationMon: public ManagedMonitorToolBase
{
  
  
public:
  LArNoiseCorrelationMon(const std::string& type, 
	      const std::string& name,
	      const IInterface* parent);
  
  /** @brief Default destructor */
  virtual ~LArNoiseCorrelationMon();
  
  
   
  StatusCode initialize();
  StatusCode finalize();
  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode procHistograms();
  
protected:
  
  
  /** services  */
  LArOnlineIDStrHelper* m_strHelper;
  const LArOnlineID* m_LArOnlineIDHelper;
  
  /** Handle to LArCablingService */
  ToolHandle<LArCablingService> m_LArCablingService; 
  
  /** Handle to bad-channel mask */
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;
  
  /** Handle to pedestal */
  const DataHandle<ILArPedestal>  m_larPedestal;
  
  
private:
  
  /**correlation histograms*/
  int Nchan; 
  double chan_low,chan_up;
  std::string  hist_name;
  std::string hist_title;

  /** The FEB map*/
  std::map<HWIdentifier,std::pair<TH2F_LW*,std::pair<TH2F_LW*,TProfile_LW*> > > m_FEBhistograms; 
  std::pair<TH2F_LW*,std::pair<TH2F_LW*,TProfile_LW*> > m_histos;  //for handling histograms

  /** list of FEBs to monitor. FEB names are expected to be of the type  LARONLINEID defined in the package atlas/LArCalorimeter/LArMonTools/LArMonTools/LArOnlineIDStrHelper.h  e.g. 'BarrelCFT00Slot02'  */
  std::vector<std::string> m_FEBsToMonitor; 

  /** to avoid publishing tons of histograms online, in case it's a problem*/
  bool m_publishAllFebsOnline;

  /** to avoid asking for triggers in case of a calibration run*/
  bool m_isCalibrationRun;
  
  /**declare identifier*/
  HWIdentifier m_febID;
  int m_ch1,m_ch2;  

  /**declaration variables used in joboptions*/
  bool m_IsOnline; 
  bool m_ignoreKnownBadChannels;

  std::string m_LArDigitContainerKey;
  std::string m_larPedestalKey;
  
  /** Private members*/
  int m_evtId;
  std::vector<std::string> m_triggers;
  
  /** Declare methods used*/
  bool isGoodChannel(HWIdentifier id,float ped);
  void bookSelectedFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookAllFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookThisFEB(HWIdentifier id,MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);

};

#endif
