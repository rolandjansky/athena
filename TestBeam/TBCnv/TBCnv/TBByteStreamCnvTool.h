//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TESTBEAM_BYTESTREAMTOOL_H
#define TESTBEAM_BYTESTREAMTOOL_H

#include <stdint.h>
#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"
#include "ByteStreamData/RawEvent.h" 
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "ByteStreamData/RawEvent.h" 
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h" 
#include "TBEvent/TBTDC.h"
#include "TBEvent/TBBeamDetector.h"
#include "TBEvent/TBBPCRawCont.h"
#include "TBEvent/TBMWPCRawCont.h"
#include "TBEvent/TBScintillatorRawCont.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include "TBEvent/TBTailCatcherRaw.h"
#include "TBEvent/TBADCRawCont.h"
#include "TBEvent/TBTDCRawCont.h"
#include "TBEvent/TBEventInfo.h"
#include "TBEvent/TBLArDigitContainer.h"
#include "TBEvent/TBLArCalibDigitContainer.h"
#include "LArCabling/LArCablingLegacyService.h"

#include "CaloIdentifier/CaloGain.h"

#include "eformat/SourceIdentifier.h"

class ByteStreamCnvSvc;
class IROBDataProviderSvc; 
class LArOnlineID;

/** An AlgTool class to provide conversion 
    ByteStream <-> TestBeam Instrumentation Data
    * created Feb 2004 Walter Lampl
  */


        
class TBByteStreamCnvTool: public AthAlgTool {
public:

  /** constructor
  */
  TBByteStreamCnvTool( const std::string& type, const std::string& name,
		       const IInterface* parent ) ;

  static const InterfaceID& interfaceID( ) ;

  virtual StatusCode initialize();
  virtual StatusCode finalize();



  StatusCode WriteFragment();
  StatusCode ReadFragment(int unrec_code);
  StatusCode ReadFragment(TBTDC*& tdc,std::string key);
  StatusCode ReadFragment(TBTDCRawCont*& tdcrawCont,std::string key);
  StatusCode ReadFragment(TBADCRawCont*& adcrawCont,std::string key);
  StatusCode ReadFragment(TBBPCRawCont*& bpcrawCont,std::string key);
  StatusCode ReadFragment(TBMWPCRawCont*& mwpcrawCont,std::string key);
  StatusCode ReadFragment(TBTriggerPatternUnit*& trigpat,std::string key);
  StatusCode ReadFragment(TBScintillatorRawCont*& scintrawCont,std::string key);
  StatusCode ReadFragment(TBTailCatcherRaw*& tailcatchraw,std::string key); 
  StatusCode ReadFragment(TBEventInfo*& tbeventinfo,std::string key); 
  StatusCode ReadFragment(TBLArDigitContainer*& tblardigitcont,std::string key); 
  StatusCode ReadFragment(TBLArCalibDigitContainer*& tblarcalibdigitcont,std::string key); 

  StatusCode BuildRODBlock(std::vector<uint32_t>* theRODBlock); 
  StatusCode GetRODBlock(eformat::SubDetector subdet_id, 
			 eformat::SubDetector subdet_rod_id=m_DontCheckRodSubDetID);

  StatusCode H6BuildObjects(int unrec_code); 
  StatusCode H6RecordObjects(int unrec_code); 
  StatusCode H8BuildObjects(int unrec_code);


  

private: 

  static const eformat::SubDetector m_DontCheckRodSubDetID; //==0xff

  ByteStreamCnvSvc* m_ByteStreamEventAccess;
  IROBDataProviderSvc *m_rdpSvc;
  EventID m_lastEventID;
  eformat::SubDetector m_subdet_id;
  std::vector<uint32_t> m_rodBlock; //For reading only : data fragment
  //StoreGateSvc* m_detectorStore;
  const LArOnlineID* m_onlineHelper;
  
  // event fragments for writing
  std::vector<uint32_t> * m_theRodBlock;
  OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment * m_theROB;

  bool m_H6run;
  bool m_H8run;
  bool m_force_Hchoice;
  bool m_dump;
  std::vector<std::string> m_keys;
  int m_subdet_key;

  // H6 SubFragment navigation :
  int m_subfrag_id;
  int m_subfrag_size;
  int m_subfrag_firstdata;

  bool NextSubFrag(){
    m_subfrag_firstdata+=m_subfrag_size;
    if(m_subfrag_firstdata<int(m_rodBlock.size())){
      m_subfrag_size=m_rodBlock[m_subfrag_firstdata-2];
      m_subfrag_id=m_rodBlock[m_subfrag_firstdata-1];
      return true;
    }else return false;
  }
  
  LArCablingLegacyService *m_larCablingSvc;
  
  // pointers to raw objects :
  TBTDC * m_tbtdc;
  TBTDCRawCont* m_tdcrawCont;
  TBADCRawCont* m_adcrawCont;
  TBBPCRawCont * m_bpcrawCont;
  TBMWPCRawCont * m_mwpcrawCont;
  TBTriggerPatternUnit * m_trigpat;
  TBScintillatorRawCont * m_scintrawCont;
  TBTailCatcherRaw * m_tailcatchraw;
  TBEventInfo * m_eventinfo;
  TBLArDigitContainer * m_tblardigitcont[4];
  TBLArCalibDigitContainer * m_tblarcalibdigitcont[4];
  

  // MiniROD members
  std::vector<int> m_boards;
  std::vector<int> m_samples;
  std::vector<int> m_gains;
  std::vector<int> m_febgain;
  std::vector<int> m_firstsamples;
  std::vector<short> m_arrayofsample[128][4]; // 128 channels, 4 possible gain mode
  CaloGain::CaloGain m_arrayofgain[128][4];  // 128 channels. 


  CaloGain::CaloGain getCaloGain(int gain){
    switch(gain){
    case 0:
      return CaloGain::LARLOWGAIN;
    case 1:
      return CaloGain::LARMEDIUMGAIN;
    case 2:
      return CaloGain::LARHIGHGAIN;
    default:
      return CaloGain::UNKNOWNGAIN;
    }
  }



  // Calibration runs :
  bool m_isCalib;
  unsigned char m_calib_pattern[16];
  uint16_t m_calib_dac;
  uint16_t m_calib_delay;
  bool m_calib_error; //m_calib_isPulsed;


  // Run - Event Info : 
  
  int m_ev_number;
  unsigned int m_run_num;
  float m_beam_moment;
  std::string m_beam_part;
  float m_cryoX;
  float m_cryoAngle;
  float m_tableY;
  std::string m_summary_path;



  // H8 trigger word
  unsigned int m_h8_triggword;

  // Word manipulation :
  unsigned short firstword(unsigned int w){
    return 65535 & w;
  }
  unsigned short secondword(unsigned int w){
    return w >> 16;
  }

  unsigned short firstnbit(int n,unsigned short w){ 
    unsigned short tmp=0;
    for(unsigned short i=0;i<n;i++) tmp = tmp | (1<<i);
    return (w & tmp);
  }

  bool testbit(int n,unsigned short w){
    return (w & (1<<n));
  }

};

#endif






