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
#include "LArCabling/LArCablingService.h"
#include "LArElecCalib/ILArRamp.h"

#include "LArRawConditions/LArAutoCorrComplete.h"
#include "LArRawConditions/LArPedestalComplete.h"
#include "LArRecConditions/ILArBadChanTool.h"

#include "CaloIdentifier/CaloGain.h"


LArAutoCorrExtrapolate::LArAutoCorrExtrapolate(const std::string& name, ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
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
  ATH_MSG_INFO ( ">>> Initialize" );
  
  if (m_badChannelTool.retrieve().isFailure()) {
    ATH_MSG_WARNING ( "No tool for bad channel" );
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
  ATH_MSG_INFO ( ">>> stop()" );

  ATH_CHECK( detStore()->retrieve(m_onlineId, "LArOnlineID") );
  LArCablingService * larCabling = nullptr;
  ATH_CHECK( toolSvc()->retrieveTool("LArCablingService",larCabling) );

  LArAutoCorrComplete* larAutoCorrComplete  = nullptr;
  ATH_CHECK( detStore()->retrieve(larAutoCorrComplete,m_keyoutput) );
  ATH_MSG_INFO ( "  Got computed Autocorrelation from physics " );

  const LArAutoCorrComplete* larAutoCorrElec = nullptr;
  ATH_CHECK( detStore()->retrieve(larAutoCorrElec,m_keyinput) );
  ATH_MSG_INFO ( " Got electronics noise autocorrelation from database " );

  const LArPedestalComplete* larPedestalElec = nullptr;
  ATH_CHECK( detStore()->retrieve(larPedestalElec,m_keyPedInput) );
  ATH_MSG_INFO ( " Got pedestal from database " );

  const ILArRamp* larRamp = nullptr;
  ATH_CHECK( detStore()->retrieve(larRamp,m_keyRamp) );
  ATH_MSG_INFO ( " Got ramp from database " );

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

      ATH_MSG_VERBOSE ( " hwid,gain0,gain1,gain2 " << m_onlineId->channel_name(hwid) << " " << gain0 << " " << gain1 << " " << gain2 );

// get AutoCorr  for highest gain, physics event
      ILArAutoCorr::AutoCorrRef_t corr=larAutoCorrComplete->autoCorr(hwid,gain0);
      ATH_MSG_VERBOSE ( " corr.size() " << corr.size() );

// get electronics noise + autocorrelation from pedestal run for highest gain
      float sigma0_elec;
      if (larPedestalElec->pedestalRMS(hwid,gain0)>=(1.0+LArElecCalib::ERRORCODE)) 
              sigma0_elec = larPedestalElec->pedestalRMS(hwid,gain0);
      else {
        ATH_MSG_WARNING ( " Cannot not find elec pedestal RMS " << m_onlineId->channel_name(hwid) );
        continue;
      }
      ILArAutoCorr::AutoCorrRef_t corr0_elec = larAutoCorrElec->autoCorr(hwid,gain0);
      ATH_MSG_VERBOSE ( " sigma0_elec " << sigma0_elec << "  corr0_ele.size() " << corr0_elec.size() );
      if (corr0_elec.size()==0) {
        ATH_MSG_WARNING ( " cannot find elec autocorr " << m_onlineId->channel_name(hwid) );
        continue;
      }

      {
        msg() << MSG::VERBOSE << " corr0_elec "; 
        unsigned int ii=0;
        for (;ii<corr0_elec.size();ii++)
          msg() << MSG::VERBOSE << corr0_elec[ii] << " "; 
        msg() << MSG::VERBOSE << endmsg;
      }

      float sigma0_elec2 = sigma0_elec*sigma0_elec;

// in case of some channels with missing data, try to recover using electronics autocorrelation only
      std::vector<float> corr2;
      bool recovered=false;
      if (corr.size()==0) {
           HWIdentifier febId = m_onlineId->feb_Id(hwid);
           if (m_useBad && m_badChannelTool->febMissing(febId)) {
             ATH_MSG_DEBUG ( " Known missing Feb, no physics autocorr  " << m_onlineId->channel_name(hwid) << "... use electronics from db " );
           } else {
             ATH_MSG_WARNING ( " No physics autocorr for channel " << m_onlineId->channel_name(hwid) << " .. use electronics from db" );
           }
           if (corr0_elec.size()==0) {
             ATH_MSG_WARNING ( " No physics and no  electronics autocorrelation available " << m_onlineId->channel_name(hwid) );
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
             ATH_MSG_WARNING ( " cannot find elec autocorr " << m_onlineId->channel_name(hwid) );
             continue;
           }
           {
             msg() << MSG::VERBOSE << " corr_elec ";
             unsigned int ii=0;
             for (;ii<corr_elec.size();ii++)
               msg() << MSG::VERBOSE << corr_elec[ii] << " ";
             msg() << MSG::VERBOSE << endmsg;
           }


           // get electronic noise in ADC counts
           float sigma_elec;
           if (larPedestalElec->pedestalRMS(hwid,gain) >= (1.0+LArElecCalib::ERRORCODE)) 
               sigma_elec = larPedestalElec->pedestalRMS(hwid,gain);
           else {
             ATH_MSG_WARNING ( " cannot find elec noise RMS " << m_onlineId->channel_name(hwid) );
             continue;
           }
           ATH_MSG_VERBOSE ( " sigma_elec " << sigma_elec << "  corr_elec.size() " << corr_elec.size() );
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
           ATH_MSG_VERBOSE ( " gain ratio " << ratio );

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
             msg() << MSG::VERBOSE << " old corr ";
             if (corr.size()>=((unsigned int)(m_Nsamples*(m_Nsamples+1)/2))) {
               unsigned k=0;
               for (int i=0;i<m_Nsamples;i++) {
                 for (int j=i;j<m_Nsamples;j++,k++)
                   msg() << MSG::VERBOSE <<  corr[k] << " ";
                 msg() << MSG::VERBOSE << endmsg;
               }
            }
           }
           {
             msg() << MSG::VERBOSE << " new corr "; 
             unsigned k=0;
             for (int i=0;i<m_Nsamples;i++) {
               for (int j=i;j<m_Nsamples;j++,k++)
                 msg() << MSG::VERBOSE <<  new_corr[k] << " "; 
               msg() << MSG::VERBOSE << endmsg;
             }
           }

// save new extrapolation 
           larAutoCorrComplete->set(hwid,gain,new_corr);

          
      }    // loop over gains

   }      // loop over channels
      
  return StatusCode::SUCCESS;
}
