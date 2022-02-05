/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArNoiseCorrelationMon
 * @author  Pavol Strizenec, based on codes from M. Spalla and Y. Enari
 *
 */

#ifndef LARCOHMON_H
#define LARCOHMON_H

//inheritance:
#include "AthenaMonitoring/ManagedMonitorToolBase.h"


//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"


//STL:
#include <string>



class LArOnlineID;
class HWIdentifier;
class LArOnlineIDStrHelper;
class TH2F_LW;
class TH1F_LW;

class LArCoherentNoisefractionMon: public ManagedMonitorToolBase
{
  
  
public:
  LArCoherentNoisefractionMon(const std::string& type, 
	      const std::string& name,
	      const IInterface* parent);
  
  /** @brief Default destructor */
  virtual ~LArCoherentNoisefractionMon();
  
  
   
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode bookHistograms() override;
  virtual StatusCode fillHistograms() override;
  virtual StatusCode procHistograms() override;
  
protected:
  
  
  /** services  */
  LArOnlineIDStrHelper* m_strHelper;
  const LArOnlineID* m_LArOnlineIDHelper;

  /** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};
  
private:
  
  /**correlation histograms*/
  int m_Nchan; 
  double m_chan_low,m_chan_up;
  std::string  m_hist_name;
  std::string m_hist_title;

  /** The intermediate histos*/
  std::map<std::pair<HWIdentifier,unsigned>, std::pair<TH1F_LW*,TH1F_LW*> > m_FEBhistograms; 
  /** per event arrays to compute sigma **/
  std::map<std::pair<HWIdentifier,unsigned>, std::vector<float> > m_div; 

  /** Summary histos, vector of 2D histograms FT:slot*/
  std::vector<TH2F_LW *> m_h_summary_plot_EMBA;
  std::vector<TH2F_LW *> m_h_summary_plot_EMBC;
  std::vector<TH2F_LW *> m_h_summary_plot_EMECA;
  std::vector<TH2F_LW *> m_h_summary_plot_EMECC;
  std::vector<TH2F_LW *> m_h_summary_plot_HECA;
  std::vector<TH2F_LW *> m_h_summary_plot_HECC;
  std::vector<TH2F_LW *> m_h_summary_plot_FCALA;
  std::vector<TH2F_LW *> m_h_summary_plot_FCALC;

  /** list of FEBs to monitor. FEB names are expected to be of the type  LARONLINEID defined in the package atlas/LArCalorimeter/LArMonTools/LArMonTools/LArOnlineIDStrHelper.h  e.g. 'BarrelCFT00Slot02'  */
  std::vector<std::string> m_FEBsToMonitor; 

  /** list of groups of the channels to monitor on each FEB - empty means all**/
  std::vector<std::string> m_groupNamesToMonitor;
  std::vector<bool> m_processGroup;
  unsigned int m_nGroups;

  /** to avoid asking for triggers in case of a calibration run*/
  bool m_isCalibrationRun;
  
  /**declare identifier*/
  HWIdentifier m_febID;

  /**other variables used in joboptions*/
  bool m_IsOnline; 

  std::string m_LArDigitContainerKey;
  
  /** Private members*/
  int m_evtId;
  std::vector<std::string> m_triggers;
  int m_evtCounter;
  
  /** Declare methods used*/
  void fillInCNF();
  void bookSelectedFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookAllFEBs(MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  void bookThisFEB(HWIdentifier id, unsigned group, MonGroup& grEMBA,MonGroup& grEMBC,MonGroup& grEMECA,MonGroup& grEMECC,MonGroup& grHECA,MonGroup& grHECC,MonGroup& grFCALA,MonGroup& grFCALC);
  double calc_dev(std::vector<float> *input_vector) const;
  double calc_sum_dev(std::vector<float> *input_vector) const;
};

#endif
