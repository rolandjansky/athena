/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "StoreGate/ReadCondHandleKey.h"
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
  ToolHandle<ILArBadChannelMasker> m_badChannelMask;

  /** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};
  
private:
  
  /**correlation histograms*/
  int m_Nchan; 
  double m_chan_low,m_chan_up;
  std::string  m_hist_name;
  std::string  m_hist_summary_name1;
  std::string  m_hist_summary_name2;
  std::string m_hist_title;

  /** The FEB map*/
  std::map<HWIdentifier,std::pair<TH2F_LW*,std::pair<TH2F_LW*,TProfile_LW*> > > m_FEBhistograms; 
  std::pair<TH2F_LW*,std::pair<TH2F_LW*,TProfile_LW*> > m_histos;  //for handling histograms

  /** Summary plot*/
  TH2F_LW * m_h_summary_plot1_EMBA;
  TH2F_LW * m_h_summary_plot1_EMBC;
  TH2F_LW * m_h_summary_plot1_EMECA;
  TH2F_LW * m_h_summary_plot1_EMECC;
  TH2F_LW * m_h_summary_plot1_HECA;
  TH2F_LW * m_h_summary_plot1_HECC;
  TH2F_LW * m_h_summary_plot1_FCALA;
  TH2F_LW * m_h_summary_plot1_FCALC;

  TH2F_LW * m_h_summary_plot2_EMBA;
  TH2F_LW * m_h_summary_plot2_EMBC;
  TH2F_LW * m_h_summary_plot2_EMECA;
  TH2F_LW * m_h_summary_plot2_EMECC;
  TH2F_LW * m_h_summary_plot2_HECA;
  TH2F_LW * m_h_summary_plot2_HECC;
  TH2F_LW * m_h_summary_plot2_FCALA;
  TH2F_LW * m_h_summary_plot2_FCALC;


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
  int m_ch1,m_ch2;  

  /**other variables used in joboptions*/
  bool m_IsOnline; 
  bool m_ignoreKnownBadChannels;
  bool m_DoSummary;

  std::string m_LArDigitContainerKey;
  
  /** Private members*/
  int m_evtId;
  std::vector<std::string> m_triggers;
  int m_evtCounter;
  
  /** Declare methods used*/
  bool isGoodChannel(const HWIdentifier id,const float ped,const LArOnOffIdMapping *cabling) const;
  void fillInCorrelations();
  void bookSelectedFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookAllFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookThisFEB(HWIdentifier id,MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);

};

#endif
