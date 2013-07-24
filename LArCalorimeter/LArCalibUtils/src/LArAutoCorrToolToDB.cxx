/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 
 NAME:     LArAutoCorrToolToDB.cxx
 PACKAGE:  offline/LArCalorimeter/LArCalibUtils
 
 AUTHORS:  G.Unal
 CREATED:  Feb. 24, 2010 
    
 PURPOSE:  write LArAutoCorr in Database from LArAutoCorrTotalTool output
             
********************************************************************/

// Include files
#include "LArCalibUtils/LArAutoCorrToolToDB.h"
#include "LArRawConditions/LArAutoCorrComplete.h"
#include "CaloIdentifier/CaloGain.h"


LArAutoCorrToolToDB::LArAutoCorrToolToDB(const std::string& name, ISvcLocator* pSvcLocator) 
  : Algorithm(name, pSvcLocator),
    m_storeGateSvc(0),
    m_detStore(0),
    m_onlineHelper(0),
    m_autocorrTool("LArAutoCorrTotalTool")
{
   declareProperty("AutoCorrTool",m_autocorrTool);
   declareProperty("GroupingType",    m_groupingType="ExtendedFeedThrough");
   declareProperty("AutoCorrKey",   m_acContName="LArAutoCorr");
}


LArAutoCorrToolToDB::~LArAutoCorrToolToDB()
{}

//----------------------------------------------------------------------------
StatusCode LArAutoCorrToolToDB::initialize()
{
  // StoreGate service
  MsgStream log(msgSvc(), name());
  StatusCode sc = service("StoreGateSvc", m_storeGateSvc);
  if (sc.isFailure())
    {
      log << MSG::FATAL << " StoreGate service not found " << std::endl;
      sc = StatusCode::FAILURE;
      return sc;
    }
  
  // Get DetectorStore service
  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure())
    {
      log << MSG::FATAL << " DetectorStore service not found " << std::endl;
      sc = StatusCode::FAILURE;
      return sc;
    }
  
  sc = m_detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_autocorrTool.retrieve();
  if (sc.isFailure()) {
      log << MSG::ERROR << "Cannot retrieve LArAutoCorrTotalTool " << endreq;
      return sc;
  }

  return StatusCode::SUCCESS;

}

// ---------------------------------------
StatusCode LArAutoCorrToolToDB::finalize()
{
  return StatusCode::SUCCESS;
}

// ---------------------------------------
StatusCode LArAutoCorrToolToDB::execute()
{  
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode LArAutoCorrToolToDB::stop() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> stop()" << endreq;

  LArAutoCorrComplete* larAutoCorrComplete = new LArAutoCorrComplete();
  // Initialize LArAutoCorrComplete 
  StatusCode sc=larAutoCorrComplete->setGroupingType(m_groupingType,log);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to set groupingType for LArAutoCorrComplete object" << endreq;
    return sc;
  }
  sc=larAutoCorrComplete->initialize(); 
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed initialize LArAutoCorrComplete object" << endreq;
    return sc;
  }


  //Loop over gains
  for (unsigned igain=0;igain<(int)CaloGain::LARNGAIN;igain++) {
    CaloGain::CaloGain gain=(CaloGain::CaloGain)igain;
    //Loop over cells
    std::vector<HWIdentifier>::const_iterator it   =m_onlineHelper->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e =m_onlineHelper->channel_end();
    unsigned nSkipped=0;
    unsigned nDone=0;
    for(;it!=it_e;++it) {

      HWIdentifier chid = (*it);
     
      const std::vector<double>  AutoCorr = m_autocorrTool->autoCorrTotal(chid,igain,-1);
      // Not to process channels not existing in input elec. autocorr. DB
      if(AutoCorr.size() < 2 || (AutoCorr[0]==0. && AutoCorr[1]==0.)) {
	++nSkipped;
	continue;
      }
      for(unsigned ii=0; ii<AutoCorr.size(); ++ii) std::cout<<AutoCorr[ii]<<" : ";
      std::cout<<std::endl;
      const std::vector<double> rmsSampl =  m_autocorrTool->samplRMS(chid,igain,-1);
      unsigned int nsamples_AC = (1+((int)(sqrt(1+8*AutoCorr.size()))))/2;

      std::vector<float> cov;
      unsigned int ntot = nsamples_AC*(nsamples_AC+1) / 2;
      cov.resize(ntot,0.);

      unsigned int k=0;
      for (unsigned i=0;i<nsamples_AC;i++) {  
         for (unsigned j=i;j<nsamples_AC;j++,k++) {
          double AC;
          if (i==j) {
             AC=1.;
          }
          else {
             int i1=std::min(i,j);
             int i2=std::max(i,j);
             int index = i1*nsamples_AC - i1*(i1+1)/2 -(i1+1) + i2; 
             AC=AutoCorr[index];
          }
          AC = AC*rmsSampl[i]*rmsSampl[j];
          cov[k] = AC;
        }
      }

      larAutoCorrComplete->set(chid,gain,cov);
      ++nDone;
    }//end loop over all cells
    log << MSG::INFO << "Gain " << gain << ": " << nDone << " channeles done, " << nSkipped  << " channels skipped (no Elec Noise AC in input)" << endreq;
  }//end loop over gains

  // Record LArAutoCorrComplete
  sc = m_detStore->record(larAutoCorrComplete,m_acContName);
  if (sc != StatusCode::SUCCESS) { 
    log << MSG::ERROR  << " Cannot store LArAutoCorrComplete in TDS "<< endreq;
    delete larAutoCorrComplete;
    return sc;
  }
  else
    log << MSG::INFO << "Recorded LArAutCorrComplete object with key " << m_acContName << endreq;
  // Make symlink
  sc = m_detStore->symLink(larAutoCorrComplete, (ILArAutoCorr*)larAutoCorrComplete);
  if (sc != StatusCode::SUCCESS)  {
    log << MSG::ERROR  << " Cannot make link for Data Object " << endreq;
    return sc;
  }
   
  return StatusCode::SUCCESS;
}








