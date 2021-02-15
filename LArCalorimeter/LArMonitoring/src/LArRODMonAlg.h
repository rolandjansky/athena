/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARMONITORING_LArRODMONALG_H
#define LARMONITORING_LArRODMONALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArOFC.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "CaloConditions/CaloNoise.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "LArRecConditions/ILArBadChannelMasker.h"
#include "LArRawEvent/LArRawChannelContainer.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/ReadHandleKey.h"


class LArRawChannel;
class HWIdentifier;
class LArOnOffIdMapping;
class LArRawChannelContainer;
class LArDigitContainer;
class LArFebHeaderContainer;

#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>

class LArEM_ID;
class LArDigit;

class CaloDetDescrManager;


class LArRODMonAlg: public AthMonitorAlgorithm
{
 public:
  LArRODMonAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /** @brief Default destructor */
  virtual ~LArRODMonAlg();

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  // Called each event
  virtual StatusCode fillHistograms(const EventContext& ctx) const override final;


private:
  const LArOnlineID* m_LArOnlineIDHelper;

  enum PARTITION {EMBC=0,EMBA,EMECC,EMECA,HECC,HECA,FCALC,FCALA,N_PARTITIONS};
  const std::vector<std::string> m_PARTNAMES{"EMBC","EMBA","EMECC","EMECA","HECC","HECA","FCalC","FCalA","UNKNOWN"};

  class ERRCOUNTER {
  public:
    ERRCOUNTER() {
      clear();
    };
    void clear();
    unsigned errors_E[3];
    unsigned errors_T[3];
    unsigned errors_Q[3];
  };
  


  PARTITION getPartition(const HWIdentifier chid) const;
  const std::string & getPartitionName(const HWIdentifier chid) const;

  StatusCode compareChannels(const LArOnOffIdMapping* cabling,
                             const ILArOFC* ofcs,
                             const ILArShape* shapes,
                             const ILArHVScaleCorr* hvScaleCorrs,
                             const ILArPedestal* pedestals,
                             const LArADC2MeV* adc2mev,
                             const HWIdentifier chid, 
                             std::vector<unsigned> &errsPerFEB,
                             std::vector<ERRCOUNTER> &errcounters,
                             const LArRawChannel& rcDig, const LArRawChannel& rcBS, 
                             bool doDspTestDump, bool doCellsDump, 
		             std::ofstream &ofcfile,               
		             std::ofstream &digitsfile,            
	         	     std::ofstream &energyfile,
                             std::ofstream &dumpfile,              
                             const LArDigit* dig=nullptr) const;

  void closeDumpfiles();

  bool FebStatus_Check();

  /** @brief For a given cell, dump info that can be used by DspTest, into two txt files and one dat file */
  void DumpCellEvent(int count,                            // counter
                     ILArOFC::OFCRef_t ofc,                // vector containing OFC's
                     ILArOFC::OFCRef_t ofcb,               // vector containing OFCb's
		     ILArShape::ShapeRef_t ofch,           // vector containing OFCh's
		     ILArShape::ShapeRef_t ofcd,           // vector containing OFCd's
		     float Escale,                         // ADC2MeV factor
		     float Ramp0,                          // ramp intercept
		     float pedestal,                       // pedestal
		     const std::vector<short>* digits,     // vector containing samples (digits)
		     int E_off,                            // Energy calculated offline (i.e. fromDigits)
		     int E_on,                             // Energy calculated online (i.e. fromBytestream)
		     std::ofstream &ofcfile,               // output file containing ofc's
		     std::ofstream &digitsfile,            // output file containing digits
		     std::ofstream &energyfile,            // output file containing energies
		     const HWIdentifier chid,              // FEB HW ID
		     int event) const;                           // Event #
  
  
  /** @brief Dump a cell's information and calculated energies into a txt file */
  void DumpCellInfo(HWIdentifier chid,                    // Channel HW ID
		    const std::string &partc,
		    int gain,
		    int event, 
		    int E_off,                             // Energy calculated offline (i.e. fromDigits)  
		    int E_on,                              // Energy calculated online (i.e. fromBytestream)
		    float T_off,                             // Time calculated offline (i.e. fromDigits)  
		    float T_on,                              // Time calculated online (i.e. fromBytestream)
		    float Q_off,                             // Quality Factor calculated offline (i.e. fromDigits)  
		    float Q_on,                              // Quality Factor calculated online (i.e. fromBytestream)
		    std::ofstream &dumpfile) const;
  

  SG::ReadHandleKey<LArRawChannelContainer> m_channelKey_fromBytestream{this,"LArRawChannelKey_fromBytestream","LArRawChannels","SG key of LArRawChannels produced by teh DSP"};
  SG::ReadHandleKey<LArRawChannelContainer> m_channelKey_fromDigits{this,"LArRawChannelKey_fromDigits","LArRawChannels_FromDigits","SG key of LArRawChannels produced offline"};

  SG::ReadHandleKey<LArDigitContainer> m_digitContainerKey{this,"LArDigitContainerKey","FREE","SG key of LArDigitContainer read from Bytestream"};
  SG::ReadHandleKey<LArFebHeaderContainer> m_headerContainerKey{this,"LArFebHeaderKey","LArFebHeader","SG key of LArFebHeader"};


  SG::ReadCondHandleKey<ILArOFC>         m_keyOFC{this,"KeyOFC","LArOFC","SG key of LArOFC CDO"};
  SG::ReadCondHandleKey<ILArShape>       m_keyShape{this,"KeyShape","LArShape","SG key of LArShape CDO"};
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_keyHVScaleCorr{this,"KeyHVScaleCorr","LArHVScaleCorr","SG key of LArHVScaleCorr CDO"};
  SG::ReadCondHandleKey<ILArPedestal>    m_keyPedestal{this,"LArPedestalKey","LArPedestal","SG key of LArPedestal CDO"};

  SG::ReadCondHandleKey<LArADC2MeV> m_adc2mevKey{this,"LArADC2MeVKey","LArADC2MeV","SG Key of the LArADC2MeV CDO"};

  ToolHandle<ILArBadChannelMasker> m_badChannelMask;

  SG::ReadCondHandleKey<CaloNoise> m_noiseCDOKey{this,"CaloNoiseKey","totalNoise","SG Key of CaloNoise data object"};
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping CDO"};

  //declaration variables used in joboptions
  Gaudi::Property<std::string> m_MonGroupName  {this, "MonGroup", "RODMon"};
  Gaudi::Property<std::vector<std::string> > m_SubDetNames  {this, "LArRODSubDetNames", {} };
  Gaudi::Property<std::vector<std::string> > m_partitions {this, "LArRODPartitionNames", {} };
  Gaudi::Property<std::vector<int> > m_Nslots {this, "LArRODNslots", {} };

  // Output files names
  Gaudi::Property<std::string>  m_DigitsFileName{this, "DigitsFileName","digits.txt","Digits dump output filename"};
  Gaudi::Property<std::string>  m_EnergyFileName{this,"EnergyFileName","energy.txt"," Energies dump output filename"};
  Gaudi::Property<std::string>  m_AiFileName{this,"AiFileName","Calib_ai.dat","dump output filename"};
  Gaudi::Property<std::string>  m_DumpCellsFileName{this,"DumpCellsFileName","dumpCells.txt","Cells dump output filename"};


  Gaudi::Property<bool> m_doDspTestDump{this, "DoDspTestDump", false, "dsp dump switch"};
  Gaudi::Property<bool> m_doCellsDump{this, "DoCellsDump", false, "cell dump switch"};
  Gaudi::Property<bool> m_doCheckSum{this, "DoCheckSum", true, "checksum test switch"};
  Gaudi::Property<bool> m_doRodStatus{this, "DoRodStatus", true, "ROD status test switch"};
  Gaudi::Property<bool> m_printEnergyErrors{this, "PrintEnergyErrors", true, "energy errors printing"};
  Gaudi::Property<bool> m_removeNoiseBursts{this, "RemoveNoiseBursts", true, "removing events with noise bursts"};

  // Ranges and precisions (this comes from hardware definitions)
  Gaudi::Property<int> m_range_E_0{this, "ERange0",    8192, "Energy ranges (in MeV)"};
  Gaudi::Property<int> m_range_E_1{this, "ERange1",   65536, "Energy ranges (in MeV)"};
  Gaudi::Property<int> m_range_E_2{this, "ERange2",  524288, "Energy ranges (in MeV)"};
  Gaudi::Property<int> m_range_E_3{this, "ERange3", 4194304, "Energy ranges (in MeV)"};

  Gaudi::Property<int> m_range_T_0{this, "TRange0",  1000, "Time ranges (in ps)"};
  Gaudi::Property<int> m_range_T_1{this, "TRange1",  5000, "Time ranges (in ps)"};
  Gaudi::Property<int> m_range_T_2{this, "TRange2", 25000, "Time ranges (in ps)"};
  Gaudi::Property<int> m_range_T_3{this, "TRange3", 50000, "Time ranges (in ps)"};

  Gaudi::Property<int> m_range_Q_0{this, "QRange0", 65536, "Only 1 range for now, and Q < 2^16 anyway"};
  Gaudi::Property<int> m_range_Q_1{this, "QRange1", 65536, "Only 1 range for now"};
  Gaudi::Property<int> m_range_Q_2{this, "QRange2", 65536, "Only 1 range for now"};
  Gaudi::Property<int> m_range_Q_3{this, "QRange3", 65536, "Only 1 range for now"};

  Gaudi::Property<int> m_precision_E_0{this, "PrecisionERange0",   1, "Expected precision for E calculation (in MeV)"};
  Gaudi::Property<int> m_precision_E_1{this, "PrecisionERange1",   8, "Expected precision for E calculation (in MeV)"};
  Gaudi::Property<int> m_precision_E_2{this, "PrecisionERange2",  64, "Expected precision for E calculation (in MeV)"};
  Gaudi::Property<int> m_precision_E_3{this, "PrecisionERange3", 512, "Expected precision for E calculation (in MeV)"};
  Gaudi::Property<int> m_precision_E_max{this, "PrecisionERangeMax", 8192, "Expected precision for E calculation (in MeV)"};

  Gaudi::Property<int> m_precision_T_0{this, "PrecisionTRange0", 340, "Expected precision for T calculation (in ps)"};
  Gaudi::Property<int> m_precision_T_1{this, "PrecisionTRange1", 340, "Expected precision for T calculation (in ps)"};
  Gaudi::Property<int> m_precision_T_2{this, "PrecisionTRange2", 340, "Expected precision for T calculation (in ps)"};
  Gaudi::Property<int> m_precision_T_3{this, "PrecisionTRange3", 340, "Expected precision for T calculation (in ps)"};
  Gaudi::Property<int> m_precision_T_max{this, "PrecisionTRangeMax", 340, "Expected precision for T calculation (in ps)"};

  Gaudi::Property<int> m_precision_Q_0{this, "PrecisionQRange0", 3, "Expected precision for Q calculation"};
  Gaudi::Property<int> m_precision_Q_1{this, "PrecisionQRange1", 3, "Expected precision for Q calculation"};
  Gaudi::Property<int> m_precision_Q_2{this, "PrecisionQRange2", 3, "Expected precision for Q calculation"};
  Gaudi::Property<int> m_precision_Q_3{this, "PrecisionQRange3", 3, "Expected precision for Q calculation"};
  Gaudi::Property<int> m_precision_Q_max{this, "PrecisionQRangeMax", 3, "Expected precision for Q calculation"};

  /*
  int m_history_size;
  int m_history_granularity;
  int m_hsize;
  std::vector<bool> m_hdone;
  */

  Gaudi::Property<bool> m_skipKnownProblematicChannels{this, "SkipKnownProblematicChannels", false, "skipping known problems?"};
  Gaudi::Property<bool> m_skipNullPed{this, "SkipNullPed", false, "skipping no pedestal channels ?"};
  Gaudi::Property<bool> m_skipNullQT{this, "SkipNullQT", false, "skipping no quality channe4ls ?"};

  Gaudi::Property<float> m_unit_online{this, "TimeOFCUnitOnline", 1.};
  Gaudi::Property<float> m_unit_offline{this,"TimeOFCUnitOffline", 1.};
  Gaudi::Property<float> m_timeOffset{this, "TimeOffset", 0.};
  Gaudi::Property<short> m_adc_th{this, "ADCthreshold", 50, "Minimal number of ADC amplitude among samples required to compare online/offline"};
  Gaudi::Property<float> m_peakTime_cut{this, "peakTimeCut", 5., "Cut on abs(peak time) to compare online/offline (all quantities)"};
  /*float m_nb_lb;*/

  const float m_BC; // value of 1 bunch-crossing = 25ns

  /* Histogram grouping (part) */
  std::map<std::string,int> m_histoGroups;

  Gaudi::Property<std::vector<std::string> > m_streams{this, "Streams", {} };

  Gaudi::Property<unsigned> m_max_dump{this, "MaxEvDump", 100, "max number of events to dump"};
  mutable std::atomic<unsigned> m_ndump;

  mutable std::atomic<unsigned>  m_counter;
  mutable std::atomic<unsigned> m_eventsCounter;
};


inline 
LArRODMonAlg::PARTITION LArRODMonAlg::getPartition(const HWIdentifier chid) const{
  const int side=m_LArOnlineIDHelper->pos_neg(chid);
  if (m_LArOnlineIDHelper->isEMBchannel(chid)) {
    if (side==0)
      return EMBC;
    else
      return EMBA;
  }
  if (m_LArOnlineIDHelper->isEMECchannel(chid)) {
     if (side==0)
       return EMECC;
     else
       return EMECA;
  }
  if (m_LArOnlineIDHelper->isHECchannel(chid)){
    if (side==0)
      return HECC;
    else
      return HECA;
  }
  if (m_LArOnlineIDHelper->isFCALchannel(chid)) {
    if (side==0)
      return FCALC;
    else
      return FCALA;
  }

  ATH_MSG_FATAL( "Channel 0x "<< std::hex << chid.get_identifier32().get_compact() << std::dec << " neither EMB nor EMEC nor HEC nor FCAL???" );
  return N_PARTITIONS;
}

inline 
const std::string & LArRODMonAlg::getPartitionName(const HWIdentifier chid) const{
  const int side=m_LArOnlineIDHelper->pos_neg(chid);
  if (m_LArOnlineIDHelper->isEMBchannel(chid)) {
    if (side==0)
      return m_PARTNAMES[0];
    else
      return m_PARTNAMES[1];
  }
  if (m_LArOnlineIDHelper->isEMECchannel(chid)) {
     if (side==0)
       return m_PARTNAMES[2];
     else
       return m_PARTNAMES[3];
  }
  if (m_LArOnlineIDHelper->isHECchannel(chid)){
    if (side==0)
      return m_PARTNAMES[4];
    else
      return m_PARTNAMES[5];
  }
  if (m_LArOnlineIDHelper->isFCALchannel(chid)) {
    if (side==0)
      return m_PARTNAMES[6];
    else
      return m_PARTNAMES[7];
  }

  ATH_MSG_FATAL( "Channel 0x "<< std::hex << chid.get_identifier32().get_compact() << std::dec << " neither EMB nor EMEC nor HEC nor FCAL???" );
  return m_PARTNAMES[8];
}


#endif
