/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArCaliWaves2Ntuple.h"
#include "CaloIdentifier/CaloGain.h"
//#include "TGraph.h"
//#include "TF1.h"

LArCaliWaves2Ntuple::LArCaliWaves2Ntuple(const std::string& name, ISvcLocator* pSvcLocator): LArWaves2Ntuple(name, pSvcLocator)
{ 
  m_keylist.clear() ;
  declareProperty("KeyList",        m_keylist);
  declareProperty("DACSaturSkip",   m_dacSaturSkip=false);
  declareProperty("SaveJitter",     m_saveJitter=false);
  declareProperty("NtupleName",     m_ntName="DELAYS"); 
  declareProperty("NtupleFile",     m_ntFile= "FILE1");
  m_dacSaturLayer0.resize(0);
  m_dacSaturLayer1.resize(0);
  m_dacSaturLayer2.resize(0);
  m_dacSaturLayer3.resize(0);
  declareProperty("DACSaturPS",    m_dacSaturLayer0);
  declareProperty("DACSaturStrips",m_dacSaturLayer1);
  declareProperty("DACSaturMiddle",m_dacSaturLayer2);
  declareProperty("DACSaturBack",  m_dacSaturLayer3);
  declareProperty("AddCorrUndo",   m_addCorrUndo=true);
  declareProperty("ApplyCorrection",m_applyCorr=false);


}

StatusCode LArCaliWaves2Ntuple::initialize() {
  m_ntTitle="Calibration Wave";
  m_ntpath=std::string("/NTUPLES/")+m_ntFile+std::string("/")+m_ntName;
  return LArWaves2Ntuple::initialize();
}

LArCaliWaves2Ntuple::~LArCaliWaves2Ntuple()
{}

StatusCode LArCaliWaves2Ntuple::stop() 
{ 
  
  // Check DACSatur jobOption consistency, in case setup default values
  if ( m_dacSaturSkip && m_dacSaturLayer0.size()<3 ) {
     ATH_MSG_WARNING( "DACSaturPS     jobOption has wrong size. Will use default." ) ;
     m_dacSaturLayer0.resize(3);
     m_dacSaturLayer0[0] = 15000 ; 
     m_dacSaturLayer0[1] = 50000 ;
     m_dacSaturLayer0[2] = 65000 ;
  }
  if ( m_dacSaturSkip && m_dacSaturLayer1.size()<3 ) {
     ATH_MSG_WARNING( "DACSaturStrips jobOption has wrong size. Will use default." ) ;
     m_dacSaturLayer1.resize(3);
     m_dacSaturLayer1[0] = 800 ; 
     m_dacSaturLayer1[1] = 8000 ;
     m_dacSaturLayer1[2] = 65000 ;
  }
  if ( m_dacSaturSkip && m_dacSaturLayer2.size()<3 ) {
     ATH_MSG_WARNING( "DACSaturMiddle jobOption has wrong size. Will use default." ) ;
     m_dacSaturLayer2.resize(3);
     m_dacSaturLayer2[0] = 1000 ; 
     m_dacSaturLayer2[1] = 10000 ;
     m_dacSaturLayer2[2] = 65000 ;
  }
  if ( m_dacSaturSkip && m_dacSaturLayer3.size()<3 ) {
     ATH_MSG_WARNING( "DACSaturBack   jobOption has wrong size. Will use default." ) ;
     m_dacSaturLayer3.resize(3);
     m_dacSaturLayer3[0] = 800 ; 
     m_dacSaturLayer3[1] = 8000 ;
     m_dacSaturLayer3[2] = 65000 ;
  }

  StatusCode sc;
  sc=m_nt->addItem("DAC",m_dac,0,64000);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'DAC' failed" );
    return StatusCode::FAILURE;
  }

  sc=m_nt->addItem("gain",m_gain,0,3);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'gain' failed" );
    return StatusCode::FAILURE;
  }

  sc=m_nt->addItem("nPulsedCalibLines",m_nPulsedCalibLines,0,4);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'nPulsedCalibLines' failed" );
    return StatusCode::FAILURE;
  }
    
  sc=m_nt->addItem("pulsedCalibLines",4,m_pulsedCalibLines);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'pulsedCalibLines' failed" );
    return StatusCode::FAILURE;
  }
  
  if (m_saveJitter) {
    sc=m_nt->addItem("Jitter",m_jitter,0.,1.);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Jitter' failed" );
      return StatusCode::FAILURE;
    } 
  }
  
  if (m_addCorrUndo) {
    sc=m_nt->addItem("corrUndo",m_corrUndo,0,1);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'corrUndo' failed" );
      return StatusCode::FAILURE;
    }
  }

  for ( unsigned k=0 ; k<m_keylist.size() ; k++ ) {
    const std::string& key = m_keylist[k] ;


    ATH_MSG_INFO( "Processing WaveContainer from StoreGate! key=" << m_keylist[k] ); 
    const LArCaliWaveContainer* caliWaveContainer;
    StatusCode sc = m_detStore->retrieve(caliWaveContainer,key);	    
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Cannot read LArCaliWaveContainer from StoreGate! key=" << key );
      return StatusCode::FAILURE;
    } else 
      ATH_MSG_INFO( "Read LArCaliWaveContainer from StoreGate! key= "  << key );
    
    if (m_applyCorr) {
      if (!caliWaveContainer->correctionsApplied()) {
	LArCaliWaveContainer* caliWaveContainer_nc=const_cast<LArCaliWaveContainer*>(caliWaveContainer);
	sc=caliWaveContainer_nc->applyCorrections();
	if (sc.isFailure()) {
	  ATH_MSG_ERROR( "Failed to apply corrections to LArCaliWaveContainer!" );
	}
	else
	  ATH_MSG_INFO( "Applied corrections to LArCaliWaveContainer" );
      }
      else {
	ATH_MSG_WARNING( "Corrections already applied. Can't apply twice!" );
      }
    }// end if applyCorr

    
    for (unsigned igain=CaloGain::LARHIGHGAIN;igain<CaloGain::LARNGAIN ;++igain){
      std::vector<HWIdentifier>::const_iterator itOnId = m_onlineId->channel_begin();
      std::vector<HWIdentifier>::const_iterator itOnIdEnd = m_onlineId->channel_end();
      for(; itOnId!=itOnIdEnd;++itOnId){
	const HWIdentifier chid = *itOnId;
	m_gain=(long)igain;
	const LArCaliWaveVec& cwv = caliWaveContainer->get(chid,igain);
	if (cwv.size()==0) continue;
	
	LArCaliWaveVec::const_iterator cwv_it=cwv.begin();
	LArCaliWaveVec::const_iterator cwv_it_e=cwv.end();
	for (;cwv_it!=cwv_it_e;++cwv_it) {
	  const LArCaliWave& wave=*cwv_it;
	  if (wave.isEmpty()) continue;
	  if (m_addCorrUndo) m_corrUndo=0;
	  bool skip=writeEntry(chid,igain,wave);
	  if (skip) continue;
	  sc=ntupleSvc()->writeRecord(m_nt);
	  if (sc!=StatusCode::SUCCESS) {
	    ATH_MSG_ERROR( "writeRecord failed" );
	    return sc;
	  }
	}//End loop over DAC 
      }//end loop over identifiers
    }//end loop over gains


    if (m_addCorrUndo) {
      for (unsigned igain=CaloGain::LARHIGHGAIN;igain<CaloGain::LARNGAIN ;++igain) {
	LArCaliWaveContainer::ConstCorrectionIt itUndo=caliWaveContainer->undoCorrBegin(igain);
	LArCaliWaveContainer::ConstCorrectionIt itUndo_e=caliWaveContainer->undoCorrEnd(igain);
	for(;itUndo!=itUndo_e;itUndo++) {
	  const HWIdentifier chid(itUndo->first);
	  const LArCaliWaveVec& cwv = itUndo->second;
	  m_gain  = (long)igain;
	  m_corrUndo = 1;
	  LArCaliWaveVec::const_iterator cwv_it=cwv.begin();
	  LArCaliWaveVec::const_iterator cwv_it_e=cwv.end();
	  for (;cwv_it!=cwv_it_e;++cwv_it) {
	    const LArCaliWave& wave=*cwv_it;
	    bool skip=writeEntry(chid,igain,wave);
	    if (skip) continue;
	    sc=ntupleSvc()->writeRecord(m_nt);
	    if (sc!=StatusCode::SUCCESS) {
	      ATH_MSG_ERROR( "writeRecord failed" );
	      return sc;
	    }
	  }//end loop over DAC
	}//end loop over corrections
      }//end loop over gain
    }//end if addUndoCorr
  }//end loop over container keys
  ATH_MSG_INFO( "LArWave2Ntuple has finished." );
  return StatusCode::SUCCESS;
} // end finalize-method.



bool LArCaliWaves2Ntuple::writeEntry(const HWIdentifier chid, const unsigned gain, const LArCaliWave& wave) {
  //call fill method of base-class 
  fillWave(chid,wave);
  m_dac = wave.getDAC();
  int DACSatur = 100000;
  if (m_dacSaturSkip) {
    switch (m_layer) {
    case 0:
      DACSatur = m_dacSaturLayer0[gain];
      break;
    case 1:
      DACSatur = m_dacSaturLayer1[gain];
      break;
    case 2:
      DACSatur = m_dacSaturLayer2[gain];
      break;
    case 3:
      DACSatur = m_dacSaturLayer3[gain];
      break;
    }	  
    if (m_dac > DACSatur ) return true;
  }
    

  /// HEC calibration lines
  if ( !m_isSC ) {
  const std::vector<HWIdentifier>& calibLineV = ((LArCablingService*)m_larCablingSvc)->calibSlotLine(chid);
  if ( calibLineV.size()>0 ) {
    ATH_MSG_DEBUG( "wave.getIsPulsedInt() " << wave.getIsPulsedInt()<<" : "<< calibLineV.size());
    for(int i=0;i<4;i++)
      m_pulsedCalibLines[i] = NOT_VALID;
    std::vector<HWIdentifier>::const_iterator calibLineIt = calibLineV.begin();
    unsigned iCalibLine=0;
    m_nPulsedCalibLines=0;
    for(calibLineIt = calibLineV.begin(); calibLineIt != calibLineV.end();++calibLineIt) {
      if ( (wave.getIsPulsedInt()>>iCalibLine) & 1 ){
	m_pulsedCalibLines[iCalibLine] = m_onlineId->channel(*calibLineIt);
	if (m_pulsedCalibLines[iCalibLine]>=0) m_calibLine = m_onlineId->channel(*calibLineIt);
	m_nPulsedCalibLines++;
      }
      iCalibLine++;
    }
    ATH_MSG_DEBUG( "m_pulsedCalibLines: "<<m_pulsedCalibLines[0]<<"/"<<m_pulsedCalibLines[1]<<"/"<<m_pulsedCalibLines[2]<<"/"<<m_pulsedCalibLines[3]);
  }else { 
    m_nPulsedCalibLines = 0 ;
    m_pulsedCalibLines[0] = NOT_VALID;
  }
  }else { 
    m_nPulsedCalibLines = 0 ;
    m_pulsedCalibLines[0] = NOT_VALID;
  }

  // Jitter
  if ( m_saveJitter ) {
    m_jitter = m_waveHelper.getJitter(wave);
  }
 
  return false;
}
