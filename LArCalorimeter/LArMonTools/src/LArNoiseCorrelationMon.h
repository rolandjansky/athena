/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArNoiseCorrelationMon
 * @author Margherita Spalla 
 *
 */

#ifndef LARCORRMON_H
#define LARCORRMON_H

//inheritance:
#include "AthenaMonitoring/ManagedMonitorToolBase.h"


//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


//STL:
#include <string>



class LArOnlineID;
class HWIdentifier;
class LArOnlineIDStrHelper;
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
  
  
   
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode procHistograms() override;
  
protected:
  
  
  /** services  */
  LArOnlineIDStrHelper* m_strHelper;
  const LArOnlineID* m_LArOnlineIDHelper;

  /** Handle to bad-channel mask */
  LArBadChannelMask m_bcMask;
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{}, "Bad-Channel categories to mask"}; 
  
  /** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};

  /** Handle to event info */
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo"};
  
private:
  
  /**correlation histograms*/
  int m_Nchan = 0; 
  double m_chan_low = 0.0,m_chan_up = 0.0;
  std::string  m_hist_name;
  std::string  m_hist_summary_name1;
  std::string  m_hist_summary_name2;
  std::string m_hist_title;

  /** The FEB map*/
  std::map<HWIdentifier,std::pair<TH2F_LW*,std::pair<TH2F_LW*,TProfile_LW*> > > m_FEBhistograms; 
  std::pair<TH2F_LW*,std::pair<TH2F_LW*,TProfile_LW*> > m_histos;  //for handling histograms

  /** Summary plot*/
  TH2F_LW * m_h_summary_plot1_EMBA = nullptr;
  TH2F_LW * m_h_summary_plot1_EMBC = nullptr;
  TH2F_LW * m_h_summary_plot1_EMECA = nullptr;
  TH2F_LW * m_h_summary_plot1_EMECC = nullptr;
  TH2F_LW * m_h_summary_plot1_HECA = nullptr;
  TH2F_LW * m_h_summary_plot1_HECC = nullptr;
  TH2F_LW * m_h_summary_plot1_FCALA = nullptr;
  TH2F_LW * m_h_summary_plot1_FCALC = nullptr;

  TH2F_LW * m_h_summary_plot2_EMBA = nullptr;
  TH2F_LW * m_h_summary_plot2_EMBC = nullptr;
  TH2F_LW * m_h_summary_plot2_EMECA = nullptr;
  TH2F_LW * m_h_summary_plot2_EMECC = nullptr;
  TH2F_LW * m_h_summary_plot2_HECA = nullptr;
  TH2F_LW * m_h_summary_plot2_HECC = nullptr;
  TH2F_LW * m_h_summary_plot2_FCALA = nullptr;
  TH2F_LW * m_h_summary_plot2_FCALC = nullptr;


  /** list of FEBs to monitor. FEB names are expected to be of the type  LARONLINEID defined in the package atlas/LArCalorimeter/LArMonTools/LArMonTools/LArOnlineIDStrHelper.h  e.g. 'BarrelCFT00Slot02'  */
  std::vector<std::string> m_FEBsToMonitor; 

  /** to publish partial sums (offline only) if needed*/
  bool m_publishPartialSums;

  /** to avoid publishing tons of histograms online, in case it's a problem*/
  bool m_publishAllFebsOnline;

  /** to avoid asking for triggers in case of a calibration run*/
  bool m_isCalibrationRun;
  
  /**declare identifier*/
  HWIdentifier m_febID;
  int m_ch1 = 0,m_ch2 = 0;  

  /**other variables used in joboptions*/
  bool m_IsOnline; 
  bool m_ignoreKnownBadChannels;
  bool m_DoSummary;

  std::string m_LArDigitContainerKey;
  
  /** Private members*/
  int m_evtId = 0;
  std::vector<std::string> m_triggers;
  int m_evtCounter = 0;
  
  /** Declare methods used*/
  bool isGoodChannel(const HWIdentifier id,const float ped,const LArOnOffIdMapping *cabling, const LArBadChannelCont* bcCont) const;
  void fillInCorrelations();
  void bookSelectedFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookAllFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookThisFEB(HWIdentifier id,MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);

};

#endif
