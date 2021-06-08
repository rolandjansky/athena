/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArDigitMon
 * @author Louis Helary 
 *
 */

#ifndef LARMONTOOLS_LARDIGITMON_H
#define LARMONTOOLS_LARDIGITMON_H

//inheritance:
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

//Cannot forward declare this:
//#include "CaloIdentifier/CaloGain.h"

//LAr services:
#include "LArElecCalib/ILArPedestal.h"
#include "LArRecConditions/LArBadChannelMask.h"
#include "LArRecConditions/LArBadChannelCont.h"

//STL:
#include <string>

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

//Events infos:
#include "xAODEventInfo/EventInfo.h"
#include "LArRecEvent/LArNoisyROSummary.h"

class LArDigitContainer;
class LArOnOffIdMapping;

class LArEM_ID;
class LArOnlineID;
class HWIdentifier;
class LArOnlineIDStrHelper;
class LArOnOffIdMapping;
class TProfile2D_LW;
class TProfile_LW;

class LWHist1D;
class LWHist2D;
class TH1F_LW;
class TH1I_LW;
class TH2F_LW;
class TH2I_LW;

class LArDigitMon: public ManagedMonitorToolBase
{
  
  
public:
  LArDigitMon(const std::string& type, 
	      const std::string& name,
	      const IInterface* parent);
  
  /** @brief Default destructor */
  virtual ~LArDigitMon();
  
  
  
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode bookHistograms();
  StatusCode fillHistograms();
  StatusCode procHistograms();
  
protected:
  
  
  /** services*/
  LArOnlineIDStrHelper* m_strbisHelper;
  LArOnlineIDStrHelper* m_strHelper;
  const LArOnlineID* m_LArOnlineIDHelper;
  const LArEM_ID* m_LArEM_IDHelper;

  //** Handle to cabling */
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this, "CablingKey", "LArOnOffIdMap","Cabling key"};
  
  /** Handle to bad-channel mask */
  LArBadChannelMask m_bcMask;
  SG::ReadCondHandleKey<LArBadChannelCont> m_bcContKey {this, "BadChanKey", "LArBadChannel", "SG key for LArBadChan object"};
  Gaudi::Property<std::vector<std::string> > m_problemsToMask{this,"ProblemsToMask",{}, "Bad-Channel categories to mask"}; 
  
  
  /** Handle to pedestal */
  SG::ReadCondHandleKey<ILArPedestal> m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};
  
  SG::ReadHandleKey<LArNoisyROSummary> m_noisyROSummaryKey{this,"NoisyROSumKey","LArNoisyROSummary","SG key of LArNoisyROSummary object"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","SG Key of EventInfo object"};

  SG::ReadHandleKey<LArDigitContainer> m_digitContainerKey{this,"LArDigitContainerKey","FREE","SG key of LArDigitContainer read from Bytestream"};


  
private:
  
  /**declaration histo summary*/
  TH2F_LW* m_summary;
  TH2F_LW* m_summaryGain;
  
  
  /** Define the monitoring histograms used for each partitions of the LArCalorimeter*/
  struct partition {
  partition():
    m_NullDigit(0),
      m_SatDigit(0),
      m_SatDigitLow(0),
      m_OutDigit(0),
      m_NullDigitChan(0),
      m_SatDigitChan(0),
      m_SatDigitChanLow(0),
      m_OutDigitChan(0),
      m_Temp_NullDigitChan(0),
      m_Temp_SatDigitChan(0),
      m_Temp_SatDigitChanLow(0),
      m_Temp_OutDigitChan(0),
      m_PNullDigit(0),
      m_PSatDigit(0),
      m_PSatDigitLow(0),
      m_POutDigit(0),
      m_AverDig(0),
      m_AverTempPos_PerStream(0),
      m_EnTime(0),
      m_SignShape(0),
      m_MaxVsTime(0),
      m_TriggerType(0),
      sumpos(0)
    {
    }; 

    TH2I_LW* m_NullDigit;
    TH2I_LW* m_SatDigit;
    TH2I_LW* m_SatDigitLow;
    TH2I_LW* m_OutDigit;
    
    //define histograms at channel levels
    TH2F_LW* m_NullDigitChan;
    TH2F_LW* m_SatDigitChan;
    TH2F_LW* m_SatDigitChanLow;
    TH2F_LW* m_OutDigitChan;
    
    //Declare temporary histograms to be used online...
    TH2I_LW* m_Temp_NullDigitChan;
    TH2I_LW* m_Temp_SatDigitChan;
    TH2I_LW* m_Temp_SatDigitChanLow;
    TH2I_LW* m_Temp_OutDigitChan;
    
    TProfile2D_LW* m_PNullDigit;
    TProfile2D_LW* m_PSatDigit;
    TProfile2D_LW* m_PSatDigitLow;
    TProfile2D_LW* m_POutDigit;
    TProfile2D_LW* m_AverDig;
    
    TH2I_LW* m_AverTempPos_PerStream;
    
    TH2F_LW* m_EnTime;
    
    TProfile_LW* m_SignShape;
    TProfile_LW* m_MaxVsTime;
    TProfile_LW* m_TriggerType;
    
    std::string name;
    int sumpos;
    
  };
  
  /** Now define the 8 partitions of the LArCalorimeter*/
  partition m_BarrelA;
  partition m_BarrelC;
  partition m_EmecA; 
  partition m_EmecC;  
  partition m_HecA;
  partition m_HecC;  
  partition m_FcalA;
  partition m_FcalC;
  
  
  /**declare identifier*/
  HWIdentifier m_feedthroughID;
  int m_slot;
  int m_feedthrough;
  int m_febHash;
  int m_channel;
  
  
  /**declaration variables used in joboptions*/
  int m_SampleRangeLow;
  int m_SampleRangeUp;
  int m_ADCsatureCut;
  int m_SigmaCut;
  int m_ExpectedSampleMax;
  int m_SampleNumberFromDB;
  int m_NumberBadFebs;
  bool m_ignoreKnownBadChannels;
  bool m_ComputeHistError;
  bool m_PercComputed;
  bool m_IsOnline; 
  double m_TreshOut;
  double m_TreshNull;
  double m_TreshSat;
  
  //Added for Stream aware:
  std::vector<std::string> m_streams;
  std::vector<unsigned> m_streamsThisEvent;
  
  /** Private members*/
  int m_Samplenbr;
  int m_eventsCounter;
  int m_evtId;
  
  
  /** Declare methods used*/
  void BookPartitions(partition& sub, const std::string& hTitle,MonGroup& ShiftGroup,MonGroup& ExpertGroup, MonGroup& ExpertGroupEff);
  void HistTitle(LWHist2D* hist,partition& sub);
  void HistTitle(TProfile2D_LW* hist,partition& sub);
  void HistTitleSum(LWHist2D* hist);
  void FillSaturation(partition& sub);
  void FillSaturationLow(partition& sub);
  void FillOutOfRange(partition& sub);
  void FillAverMaxDig(partition& sub, int& i, float nrj, unsigned int& m_l1Trig,unsigned int& lumiblock);
  void FillSumary(partition& sub);
  void OutHistTitle(partition& sub); 
  void FillSignShape(partition& sub, int& i,float nrj,float sample_max);
  void FillNullHisto(partition& sub);
  void ScalePartition(partition& sub);
  void EndOfRun(partition& sub);
  void DeleteHist(partition& sub);
  void ScaleHisto(LWHist2D * h,int& events);
  void ComputeError(LWHist2D* hist,int& events);
  partition& WhatPartition(HWIdentifier id); 
  int GetNumberCells(TProfile2D_LW* hist1,double treshold);
  double GetMeanVal(LWHist2D* hist1);
  
  void ScaleOnlinePartition(partition& sub);
  void DumpHisto(LWHist2D* hist1,TProfile2D_LW* hist2);
  void DumpOnlineHisto(LWHist2D* hist1,LWHist2D* hist2);
  
  /** control string name types for histo names and titles*/
  // LArOnlineIDStrHelper::NameType m_histoNameType; // dws << never used
  // LArOnlineIDStrHelper::NameType m_histoTitleType; // dws << never used
};

#endif
