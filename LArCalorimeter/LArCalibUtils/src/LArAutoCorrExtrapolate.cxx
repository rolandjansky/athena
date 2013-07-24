/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorrExtrapolate.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  G.Unal
 CREATED:  Sept 2008
  
 PURPOSE:  extrapolate autocorr from High gain from random to Low /Medium Gain
           (from medium to low for HEC)

  HISTORY:
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArAutoCorrExtrapolate.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "LArElecCalib/ILArRamp.h"

#include "LArRawConditions/LArAutoCorrComplete.h"
#include "LArRawConditions/LArPedestalComplete.h"
#include "LArRecConditions/ILArBadChanTool.h"

#include "CaloIdentifier/CaloGain.h"


LArAutoCorrExtrapolate::LArAutoCorrExtrapolate(const std::string& name, ISvcLocator* pSvcLocator) 
  : Algorithm(name, pSvcLocator),
    m_detStore(0),
    m_onlineId(0),
    m_badChannelTool("LArBadChanTool")
{
  m_useBad=true;
  declareProperty("KeyOutput",    m_keyoutput="LArAutoCorr");
  declareProperty("keyPedestal",  m_keyPedestal="Pedestal");
  declareProperty("keyAutoInput", m_keyinput="LArAutoCorrElec");
  declareProperty("keyPedInput",  m_keyPedInput="PedestalElec");
  declareProperty("keyRamp",      m_keyRamp="LArRamp");
  declareProperty("nSamples",     m_Nsamples=5);
  declareProperty("BadChannelTool",m_badChannelTool);
}


LArAutoCorrExtrapolate::~LArAutoCorrExtrapolate()
{}

StatusCode LArAutoCorrExtrapolate::initialize()
{
  StatusCode sc;
  
  // Use the message service
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Initialize" << endreq;
  
  // Get DetectorStore service
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) 
    {
      log << MSG::FATAL << " DetectorStore service not found " << std::endl;
      sc = StatusCode::FAILURE; 
      return sc;
    }

  if (m_badChannelTool.retrieve().isFailure()) {
      log << MSG::WARNING
             << "No tool for bad channel"
             << endreq; 
      m_useBad=false;
  }

  
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArAutoCorrExtrapolate::execute()
{
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode LArAutoCorrExtrapolate::stop()
{
  StatusCode sc;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> stop()" << endreq;

  sc = m_detStore->retrieve(m_onlineId, "LArOnlineID");
  if (!sc.isSuccess() || !m_onlineId) {
     log << MSG::ERROR << " cannot retrieve LArOnlineID " << endreq;
     return StatusCode::FAILURE;
  }

  LArCablingService * larCabling;
  sc = toolSvc()->retrieveTool("LArCablingService",larCabling);
  if (!sc.isSuccess() || !larCabling) {
      log << MSG::ERROR << " cannot retrieve LArCabling service " << endreq;
      return StatusCode::FAILURE;
  }


// retrieve physics autocorrcomplete
  LArAutoCorrComplete* larAutoCorrComplete;
  sc=m_detStore->retrieve(larAutoCorrComplete,m_keyoutput);
  if (!sc.isSuccess() || !larAutoCorrComplete) {
    log << MSG::ERROR << " cannot retrieve autocorr " << m_keyoutput << endreq; 
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << "  Got computed Autocorrelation from physics " << endreq;

// retrieve autocorr for electronics only
  const LArAutoCorrComplete* larAutoCorrElec;
  sc = m_detStore->retrieve(larAutoCorrElec,m_keyinput);
  if (!sc.isSuccess() || !larAutoCorrElec) {
    log << MSG::ERROR << " cannot retrieve Electronics AutoCorr " << m_keyinput << endreq; 
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << " Got electronics noise autocorrelation from database " << endreq;

// retrieve noise / pedestal for electronics only
  const LArPedestalComplete* larPedestalElec;
  sc = m_detStore->retrieve(larPedestalElec,m_keyPedInput);
  if (!sc.isSuccess() || !larPedestalElec) {
    log << MSG::ERROR << " cannot retrieve Electronics Pedestal  " << m_keyPedInput << endreq;   
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << " Got pedestal from database " << endreq;

// retrieve electronics gain (ramp)
  const ILArRamp* larRamp;
  sc = m_detStore->retrieve(larRamp,m_keyRamp);
  if (!sc.isSuccess() || !larRamp) {
    log << MSG::ERROR << " cannot retrieve ramp  " << m_keyRamp << endreq;   
    return StatusCode::FAILURE;
  }
  log << MSG::INFO << " Got ramp from database " << endreq;

// loop over channels
   std::vector<HWIdentifier>::const_iterator it = m_onlineId->channel_begin();
   std::vector<HWIdentifier>::const_iterator it_e = m_onlineId->channel_end();
   for (;it!=it_e;it++) {
      HWIdentifier hwid = (*it);

// skip disconnected channels

      if (!(larCabling->isOnlineConnected(hwid))) continue; 

// if HEC  , correct low gain, otherwise correct Medium and Low
      CaloGain::CaloGain gain0 = CaloGain::LARHIGHGAIN;
      CaloGain::CaloGain gain1 = CaloGain::LARMEDIUMGAIN;
      if (m_onlineId->isHECchannel(hwid)) {
         gain0 = CaloGain::LARMEDIUMGAIN;
         gain1 = CaloGain::LARLOWGAIN;
      }
      CaloGain::CaloGain gain2 = CaloGain::LARLOWGAIN;

      log << MSG::VERBOSE << " hwid,gain0,gain1,gain2 " << m_onlineId->channel_name(hwid) << " " << gain0 << " " << gain1 << " " << gain2 << endreq;

// get AutoCorr  for highest gain, physics event
      ILArAutoCorr::AutoCorrRef_t corr=larAutoCorrComplete->autoCorr(hwid,gain0);
      log << MSG::VERBOSE << " corr.size() " << corr.size() << endreq;

// get electronics noise + autocorrelation from pedestal run for highest gain
      float sigma0_elec;
      if (larPedestalElec->pedestalRMS(hwid,gain0)>=(1.0+LArElecCalib::ERRORCODE)) 
              sigma0_elec = larPedestalElec->pedestalRMS(hwid,gain0);
      else {
          log <<MSG::WARNING << " Cannot not find elec pedestal RMS " << m_onlineId->channel_name(hwid) << endreq;
          continue;
      }
      ILArAutoCorr::AutoCorrRef_t corr0_elec = larAutoCorrElec->autoCorr(hwid,gain0);
      log << MSG::VERBOSE << " sigma0_elec " << sigma0_elec << "  corr0_ele.size() " << corr0_elec.size() << endreq;
      if (corr0_elec.size()==0) {
        log << MSG::WARNING << " cannot find elec autocorr " << m_onlineId->channel_name(hwid) << endreq;
        continue;
      }

      {
       log << MSG::VERBOSE << " corr0_elec "; 
       unsigned int ii=0;
       for (;ii<corr0_elec.size();ii++) log << MSG::VERBOSE << corr0_elec[ii] << " "; 
       log << MSG::VERBOSE << endreq;
      }

      float sigma0_elec2 = sigma0_elec*sigma0_elec;

// in case of some channels with missing data, try to recover using electronics autocorrelation only
      std::vector<float> corr2;
      bool recovered=false;
      if (corr.size()==0) {
           HWIdentifier febId = m_onlineId->feb_Id(hwid);
           if (m_useBad && m_badChannelTool->febMissing(febId)) {
             log << MSG::DEBUG << " Known missing Feb, no physics autocorr  " << m_onlineId->channel_name(hwid) << "... use electronics from db " << endreq;
           } else {
             log << MSG::WARNING << " No physics autocorr for channel " << m_onlineId->channel_name(hwid) << " .. use electronics from db" << endreq;
           }
           if (corr0_elec.size()==0) {
              log << MSG::WARNING << " No physics and no  electronics autocorrelation available " << m_onlineId->channel_name(hwid) << endreq;
              continue;
           }
           int n = m_Nsamples;
           if (corr0_elec.size()<(unsigned int)(n-1)) n = corr0_elec.size()+1;
           unsigned int size = n*(n+1)/2;
           std::vector<float> corr2;
           corr2.resize(size,0.);
           unsigned k=0;
           for (int i=0;i<n;i++) {
             for (int j=i;j<n;j++,k++) {
               if (i==j) corr2[k]=sigma0_elec2;
               else      corr2[k]=corr0_elec[j-i-1]*sigma0_elec2;
             }
           }
           larAutoCorrComplete->set(hwid,gain0,corr2);
           recovered=true;
      } 
           

// loop over gains to correct
      for (int gain=gain1;gain<=gain2;gain++) {

//  for Presampler skip lowgain
           if (m_onlineId->isEMBPS(hwid) && gain==CaloGain::LARLOWGAIN) continue;

           // get electronics noise autocorrelation
           ILArAutoCorr::AutoCorrRef_t corr_elec = larAutoCorrElec->autoCorr(hwid,gain);
           if (corr_elec.size()==0) {
             log <<MSG::WARNING << " cannot find elec autocorr " << m_onlineId->channel_name(hwid) << endreq;
             continue;
           }
           {
            log << MSG::VERBOSE << " corr_elec ";
            unsigned int ii=0;
            for (;ii<corr_elec.size();ii++) log << MSG::VERBOSE << corr_elec[ii] << " ";
            log << MSG::VERBOSE << endreq;
           }


           // get electronic noise in ADC counts
           float sigma_elec;
           if (larPedestalElec->pedestalRMS(hwid,gain) >= (1.0+LArElecCalib::ERRORCODE)) 
               sigma_elec = larPedestalElec->pedestalRMS(hwid,gain);
           else {
             log << MSG::WARNING << " cannot find elec noise RMS " << m_onlineId->channel_name(hwid) << endreq;
             continue;
           }
           log << MSG::VERBOSE << " sigma_elec " << sigma_elec << "  corr_elec.size() " << corr_elec.size() << endreq;
           float sigma_elec2 = sigma_elec*sigma_elec;

 // extrapolate to high gain using ramp
           float ratio=0.1;
           if (gain0==CaloGain::LARHIGHGAIN && gain==CaloGain::LARLOWGAIN) ratio=0.01;
	   const ILArRamp::RampRef_t rampcoeff=larRamp->ADC2DAC(hwid, gain);
	   const ILArRamp::RampRef_t rampcoeff0=larRamp->ADC2DAC(hwid, gain0);
           if (rampcoeff.size()>1 && rampcoeff0.size()>1) {
              float ramp1 = rampcoeff[1];
              float ramp0 = rampcoeff0[1];
              if (ramp1>0.01 && ramp0>0.01 && ramp1<1e4 && ramp0<1e4) ratio = ramp0/ramp1;
           }
           //float ratio2=ratio*ratio;
           log << MSG::VERBOSE << " gain ratio " << ratio << endreq; 

// compute new autocorrelation

           std::vector<float> new_corr;
           if (!recovered) new_corr.reserve(corr.size());
           else            new_corr.reserve(corr2.size());

           unsigned int k=0;
           for (int i=0;i<m_Nsamples;i++) {
              for (int j=i;j<m_Nsamples;j++,k++) {
                float rij_0=0.;
                if (!recovered)  {
                  if (k<corr.size()) rij_0 = corr[k];
                }
                else {
                  if (k<corr2.size()) rij_0 = corr2[k];
                }

                float corre0 = 0. ;
                float corre  = 0. ;
                if (i==j) {
                    corre0=1.;
                    corre=1.;
                } 
                else {
                     unsigned int index = j-i-1;
                     if (index<corr0_elec.size()) corre0=corr0_elec[index];
                     if (index<corr_elec.size()) corre=corr_elec[index];
                }
                float rij_tot = rij_0*ratio - sigma0_elec2*corre0*ratio + sigma_elec2*corre;
                new_corr.push_back(rij_tot);
              }
           } 

           {
            log << MSG::VERBOSE << " old corr ";
            if (corr.size()>=((unsigned int)(m_Nsamples*(m_Nsamples+1)/2))) {
            unsigned k=0;
            for (int i=0;i<m_Nsamples;i++) {
              for (int j=i;j<m_Nsamples;j++,k++) log << MSG::VERBOSE <<  corr[k] << " ";
              log << MSG::VERBOSE << endreq;
            }
            }
           }
           {
            log << MSG::VERBOSE << " new corr "; 
            unsigned k=0;
            for (int i=0;i<m_Nsamples;i++) {
              for (int j=i;j<m_Nsamples;j++,k++) log << MSG::VERBOSE <<  new_corr[k] << " "; 
              log << MSG::VERBOSE << endreq;
            }
           }

// save new extrapolation 
           larAutoCorrComplete->set(hwid,gain,new_corr);

          
      }    // loop over gains

   }      // loop over channels
      
  return StatusCode::SUCCESS;
}
