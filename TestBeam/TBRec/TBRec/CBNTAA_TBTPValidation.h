/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CBNTAA_TBTPVALIDATION_H
#define CBNTAA_TBTPVALIDATION_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CBNT_TBRecBase.h"
#include <fstream>
#include <string>
#include <vector>

class StoreGateSvc;

/** @class CBNTAA_TBTPValidation CBNTAA_TBTPValidation.h "TBRec/CBNTAA_TBTPValidation.h"
ntpl-dumper for all persistified TB classes.
@author Iftach Sadeh
*/

class CBNTAA_TBTPValidation : public CBNT_TBRecBase
{
 public:
  CBNTAA_TBTPValidation(const std::string & name, ISvcLocator * pSvcLocator);

  ~CBNTAA_TBTPValidation();

  //standart algorithm methods
  virtual StatusCode CBNT_initialize();
  virtual StatusCode CBNT_execute();
  virtual StatusCode CBNT_finalize();

 private:

  bool m_neverReturnFailure;   // controls when StatusCode::FAILURE can be issued in execute()

  // TBADCRawCont
	std::vector<unsigned int> *m_adc;
  std::vector< std::string > *m_tbDetectorName_TBADCRawCont;
  std::vector< bool > *m_overflow_TBADCRawCont;
  std::string m_containerKey1;
  // TBTDCRawCont
  std::vector<unsigned int> *m_tdc_raw;
	std::vector<bool> *m_underThreshold_raw;
  std::vector< std::string > *m_tbDetectorName_TBTDCRawCont;
  std::vector< bool > *m_overflow_TBTDCRawCont;
	std::string m_containerKey2;
  // TBPhase
  std::vector<float> *m_phase;
  std::vector<short> *m_phaseind;
  std::vector<float> *m_dTtoWAC;
	std::string m_containerKey3;
  // TBEventInfo
  std::vector<int> *m_ev_number;
  std::vector<int> *m_ev_clock;
  std::vector<int> *m_ev_type;
  std::vector<unsigned int> *m_run_num;
  std::vector<float> *m_beam_moment;
  std::vector<std::string> *m_beam_part;
  std::vector<float> *m_cryoX;
  std::vector<float> *m_cryoAngle;
  std::vector<float> *m_tableY;
	std::string m_containerKey4;
  // TBBPCCont
  typedef double signal_type_TBBPCCont;
  std::vector< signal_type_TBBPCCont > *m_xPos,*m_yPos;
  std::vector< signal_type_TBBPCCont > *m_xErr,*m_yErr;
  std::vector< signal_type_TBBPCCont > *m_xPulse, *m_yPulse;
  std::vector< short > *m_hitnumber;
  std::vector< bool > *m_xPosOverflow,   *m_yPosOverflow;
  std::vector< bool > *m_xPulseOverflow, *m_yPulseOverflow;
  std::vector< bool > *m_overflowSetFlag;
  std::vector< std::string > *m_tbDetectorName_TBBPCCont;
  std::vector< bool > *m_overflow_TBBPCCont;
	std::string m_containerKey5;
  // TBLArDigitContainer
  std::vector<unsigned int> *m_channelID;
  std::vector<unsigned char> *m_gain;
  std::vector<unsigned short> *m_nSamples;
  std::vector<unsigned short> *m_samples;
	std::string m_containerKey6;
  // TBScintillatorCont
  std::vector< float > *m_signal_scint, *m_time_signal_scint;
  std::vector< bool > *m_signal_overflow_scint, *m_time_overflow_scint;
  std::vector< std::string > *m_tbDetectorName_TBScintillatorCont;
  std::vector< bool > *m_overflow_TBScintillatorCont;
	std::string m_containerKey7;
  // TBTailCatcher
  std::vector< double > *m_signals;  
  std::vector< float > *m_signal_tCatch, *m_time_signal_tCatch;
  std::vector< bool > *m_signal_overflow_tCatch, *m_time_overflow_tCatch; 
  std::vector< std::string > *m_tbDetectorName_TBTailCatcher;
  std::vector< bool > *m_overflow_TBTailCatcher;
	std::string m_containerKey8;
  // TBTDC
  std::vector< int > *m_tdc_TBTDC;
  std::vector< int > *m_tdcmin_TBTDC;
  std::vector< float > *m_scale_TBTDC;
  std::vector< int > *m_phase_TBTDC;
	std::string m_containerKey9;
  // TBTrack
  std::vector< int > *m_hitNumberU, *m_hitNumberV;
  std::vector<double > *m_residualu, *m_residualv;
  std::vector<double > *m_chi2, *m_chi2u, *m_chi2v;
  std::vector<double > *m_angle;
  std::vector<double > *m_uslope,*m_vslope;
  std::vector<double > *m_uintercept, *m_vintercept;
  std::vector< double > *m_cryou,*m_cryov,*m_cryow;    
	std::string m_containerKey10;
  // TBTriggerPatternUnit
  std::vector< unsigned int > *m_triggerWord;
  std::vector<bool> *m_triggers;
	std::string m_containerKey11;
  // TBMWPCCont
  std::vector < std::vector<float> > *m_cPos;
  std::vector < std::vector<float> > *m_cErr;
  std::vector < bool > *m_isX;
  std::vector < std::vector<float> > *m_clusterSize_c; 
  std::vector< std::string > *m_tbDetectorName_TBMWPCCont;
  std::vector< bool > *m_overflow_TBMWPCCont;
	std::string m_containerKey12;
  // {vector of vector of unsigned} instead of {vector of vector of bool}
  std::vector < std::vector<unsigned> > *m_cPosOverflow;


  //Private function to build names for ntuple entries. Concatinates strings
  //but omitts characters that are problematic for root like -+:*/
  std::string add_name(const char* base, const std::string extension);

  StatusCode CBNT_clear();
};

#endif
