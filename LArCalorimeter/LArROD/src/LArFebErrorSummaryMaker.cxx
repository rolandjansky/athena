/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

********************************************************************/

#include "LArROD/LArFebErrorSummaryMaker.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "StoreGate/StoreGateSvc.h" 
#include "LArRawEvent/LArFebErrorSummary.h" 
#include "LArIdentifier/LArOnlineID.h" 
#include "LArRecConditions/ILArBadChanTool.h"
#include "LArRecConditions/LArBadFeb.h"

#include <bitset>


bool isHec, isFcal, isEmb, isEmec, isEmPS, isAside, isCside;
/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArFebErrorSummaryMaker::LArFebErrorSummaryMaker(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),m_onlineHelper(NULL),m_nwarns(0),m_missingFebsWarns(0),
  m_checkAllFeb(true), m_partition(""), m_badChannelTool("")
{
  declareProperty("warnLimit", m_warnLimit=10);

  declareProperty("CheckAllFEB",m_checkAllFeb);
  declareProperty("PartitionId", m_partition); //Should contain DAQ partition (+ eventually the EventBuilder)

  declareProperty("MaskFebEvtId",m_knownEvtId);
  declareProperty("MaskFebScacStatus",m_knownSCACStatus);
  declareProperty("MaskFebZeroSample",m_knownZeroSample);
  declareProperty("BadChannelTool",m_badChannelTool,"Bad channel tool to get info on Feb errors to ignore from database");
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArFebErrorSummaryMaker::~LArFebErrorSummaryMaker()
{  }



StatusCode LArFebErrorSummaryMaker::initialize()
{

  MsgStream  log(messageService(),name());

  log <<MSG::DEBUG<<" initialize "<<endreq; 


  StoreGateSvc* detStore; 
  StatusCode sc = service("DetectorStore", detStore ); 
  if(!sc.isSuccess() ) {
       log << MSG::ERROR << " Can't retrieve DetectorStore " << endreq;
       return sc;
  }


  sc = detStore->retrieve( m_onlineHelper ) ; 
  if(sc!=::StatusCode::SUCCESS) {
   log<<MSG::ERROR<<" Failed to LArOnlineID" << endreq; 
   return sc; 
  }

  m_errors.resize(LArFebErrorSummary::N_LArFebErrorType,0 ); 

  isHec = isFcal = isEmb = isEmec = isEmPS = isAside = isCside = false;

  if (m_checkAllFeb)
    {
      std::vector<HWIdentifier>::const_iterator it = m_onlineHelper->feb_begin();
      std::vector<HWIdentifier>::const_iterator it_e = m_onlineHelper->feb_end();

      for (;it!=it_e;++it){
          m_all_febs.insert((*it).get_identifier32().get_compact());
      }
      log<<MSG::INFO<<" number of FEBs from LArOnlineID  "<< m_all_febs.size() << endreq; 
    }
  else { // We should chack partition id
     if(m_partition.size() > 0) {
        unsigned length = m_partition.size();
        if(m_partition.find("LArgHecFcal") < length && m_partition.find("EB-HEC") < length) isHec=true;
        if(m_partition.find("LArgHecFcal") < length && m_partition.find("EB-FCAL") < length) isFcal=true;
        if(m_partition.find("LArgBarrelPS") < length)  {
           isEmPS = true;
           if(m_partition.find("EB-EMBA") < length) isAside = true;
           if(m_partition.find("EB-EMBC") < length) isCside = true;
        }
        if(m_partition.find("LArgEm") < length && m_partition.find("EB-EMB") < length)  {
           isEmb = true;
           if(m_partition.find("EB-EMBA") < length) isAside = true;
           if(m_partition.find("EB-EMBC") < length) isCside = true;
        }
        if(m_partition.find("LArgEm") < length && m_partition.find("EB-EMEC") < length)  {
           isEmec = true;
           if(m_partition.find("EB-EMECA") < length) isAside = true;
           if(m_partition.find("EB-EMECC") < length) isCside = true;
        }
     }
     if(isHec || isFcal || isEmb || isEmec || isEmPS) {
        log<<MSG::DEBUG<<"isHec: "<<isHec<<" isFcal: "<< isFcal <<" isEmb: "<< isEmb <<" isEmec: "<< isEmec <<" isEmbPS: "<<  isEmPS<<endreq;
     } else {
        log<<MSG::WARNING<<"Wrong PartitionId property: "<<m_partition<<endreq;
        log<<MSG::WARNING<<"Missing FEB's will be not checked "<<endreq;
     }
     // Now let's build the list of FEB's according partition
     std::vector<HWIdentifier>::const_iterator it = m_onlineHelper->feb_begin();
     std::vector<HWIdentifier>::const_iterator it_e = m_onlineHelper->feb_end();

     for (;it!=it_e;++it){
        if(isHec && m_onlineHelper->isHECchannel(*it)) { m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
        if(isFcal && m_onlineHelper->isFCALchannel(*it)) { m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
        if((isEmb && m_onlineHelper->isEMBchannel(*it) && (!m_onlineHelper->isPS(*it))) 
              || (isEmec && m_onlineHelper->isEMECchannel(*it)) 
              || (isEmPS && m_onlineHelper->isPS(*it) && (!m_onlineHelper->isEMBchannel(*it)))  ) { 
           if(isAside && m_onlineHelper->pos_neg(*it) == 1) {m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
           if(isCside && m_onlineHelper->pos_neg(*it) == 0) {m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
        }
     }
  }
  log<<MSG::INFO<<"Number of expected FEB's: "<<m_all_febs.size()<<endreq;

  log<<MSG::INFO<<" initialized " << endreq; 

  if (!m_badChannelTool.empty()) {
     sc = m_badChannelTool.retrieve();
     if (sc.isFailure()) {
       log << MSG::ERROR << " Failed to retrieve LArBadChanTool" << endreq;
       return sc;
     }
  }

  return StatusCode::SUCCESS ; 

} 


StatusCode LArFebErrorSummaryMaker::execute()
{
  MsgStream  log(messageService(),name());

  log <<MSG::DEBUG<<" execute "<<endreq; 

  const LArFebHeaderContainer* hdrCont;
  StatusCode sc = evtStore()->retrieve(hdrCont);
  if (sc.isFailure() || !hdrCont) {
    if (m_nwarns < m_warnLimit) 
      {
	m_nwarns++;
	log << MSG::WARNING << "No LArFebHeaderContainer found in TDS, LArFebErrorSummary not created " << endreq; 
      }
    return StatusCode::SUCCESS;
  }
  

  LArFebErrorSummary* febErrorSummary = new LArFebErrorSummary() ;

  sc = evtStore()->record(febErrorSummary,"LArFebErrorSummary");

  if ( sc.isFailure() ) {
	log << MSG::ERROR << "Can not record LArFebErrorSummary, new LArFebErrorSummary not created " << endreq; 
	delete febErrorSummary; 
	return StatusCode::SUCCESS;
  }

  sc = evtStore()->setConst(febErrorSummary);
  if ( sc.isFailure() ) {
	log << MSG::ERROR << "Can not set const for LArFebErrorSummary" << endreq; 
	// return sc;
  }
  unsigned int nbSamplesFirst=0;
  uint32_t eventTypeFirst = 999;

  log << MSG::DEBUG << " LArFebHeaderContainer Size = "<< hdrCont->size()  << endreq; 

  LArFebHeaderContainer::const_iterator it = hdrCont->begin(); 
  LArFebHeaderContainer::const_iterator it_e = hdrCont->end(); 

  std::set<unsigned int> all_febs;
  if(m_checkAllFeb || isHec || isFcal || isEmb || isEmec || isEmPS){

    all_febs= m_all_febs;
    
  }
    

  for ( ; it!=it_e;++it) {

      HWIdentifier febid=(*it)->FEBId();
      unsigned int int_id =  febid.get_identifier32().get_compact();
   
      if (m_checkAllFeb || isHec || isFcal || isEmb || isEmec || isEmPS){
	all_febs.erase(int_id);
      }
      // ctrl3 
      // const std::vector <uint16_t> &febctrl3 = (*it)->FebCtrl3();

      // Retrieve rodstatus from DSP header
      uint32_t rodstatus = (*it)->RodStatus();
      // Retrieve SCA adresses
      const std::vector <uint16_t> &sca = (*it)->SCA();
      // Eventype = 2 : transparent/raw data - 4 : Physic - 7 : calibration - 10 : pedestal
      uint32_t eventType = (*it)->DetEventType();      

      // Check the type consistency among different FEBs
      bool typeMism = false;
      // The event type is not yet determined(1st FEB) - Take this as reference
      if (eventTypeFirst == 999) eventTypeFirst = eventType;
      if (eventType != eventTypeFirst) typeMism = true;

      std::bitset<32> rodstatusbits(rodstatus);
      
      bool scaOutOfRange = false;
      bool badNbOfSp = false;
      bool zeroSp = false;
      bool checkSumErr=false; 

      // Extract the number of samples for the first non zero samples block                                             
      // This allow to allow to always plot the number of samples in the histo                                          
      if (nbSamplesFirst == 0){
        // Raw data / transparent : Nb of samples is here determined with size of raw data vector                       
        if (eventType == 2 && (sca.size() != 0)) nbSamplesFirst=sca.size();
        // Physic : Nb of samples is here determined with method of LArFEBMonHeader                                     
        if (eventType == 4 && ((*it)->NbSamples() != 0)) nbSamplesFirst=(*it)->NbSamples();
      }
      // Test the uniformity of number of samples and that sca # is inside [0;143] only in raw data                     
      if (eventType == 2){
	if (nbSamplesFirst!=sca.size() && sca.size()!=0) badNbOfSp = true;
	for (unsigned int i=0; i<sca.size(); ++i){
	  if (sca[i]>143) scaOutOfRange = true;
	}
      }
      if (eventType == 4 && (nbSamplesFirst != (*it)->NbSamples()) && ((*it)->NbSamples() != 0)) badNbOfSp = true;

      // Test that the number of samples is not zero only in raw data and results mode                                  
      if (eventType == 2 &&  sca.size() == 0) zeroSp = true;
      if (eventType == 4 &&  ((*it)->RodResults1Size()  == 0)) zeroSp = true;


      if (!zeroSp){
	if (!(*it)->ChecksumVerification()) checkSumErr = true; 
	if (eventType==4) {
	  int expSize1 = 0;
	  if (((*it)->FormatVersion()&0xF)<=11) expSize1 = (*it)->NbSweetCells1() + 83 + (int) ceilf(((*it)->NbSamples()+1)/2.0); // Old DSP version
	  if (((*it)->FormatVersion()&0xF)>=12) expSize1 = (*it)->NbSweetCells1() + 84 + (int) ceilf(((*it)->NbSamples()+1)/2.0); // New DSP version after 07/11 with new extra word SumE
	  //	  log << MSG::ERROR << "Version  " << ((*it)->FormatVersion()&0xF) << " Expected size " << expSize1 << " " << (*it)->RodResults1Size() << endreq; 
	  if (expSize1 != (*it)->RodResults1Size()) {
		checkSumErr = true; 
	//	log << MSG::ERROR << "Version 1  " << ((*it)->FormatVersion()&0xF) << " Expected size " << expSize1 << " " << (*it)->RodResults1Size() << endreq;
	  }
	  int nbOf32bits = ((*it)->NbSweetCells2() * (*it)->NbSamples() +1) / 2.0;
	  if ( nbOf32bits != (*it)->RodResults2Size()) {
		checkSumErr = true; 
	//	log << MSG::ERROR << "Version 2  " << nbOf32bits << " " << (*it)->RodResults2Size() << endreq;
	  }
	}
      }

      

      uint16_t errw=0; 

      /* enum LArFebErrorType{
	 Parity,BCID,SampleHeader,EVTID,ScacStatus,ScaOutOfRange,
	 GainMismatch,TypeMismatch,NumOfSamples,EmptyDataBlock,DspBlockSize,CheckSum, BadGain, N_LArFebErrorType
	 } ;
      */

      if (rodstatusbits[6])
	errw = errw | (1<< LArFebErrorSummary::Parity);
	
      if( rodstatusbits[2] | rodstatusbits[7])
	errw = errw | (1<< LArFebErrorSummary::BCID);
	
      if(rodstatusbits[3] | rodstatusbits[8])
	errw = errw | (1<< LArFebErrorSummary::SampleHeader);
	
      if( ( rodstatusbits[1] | rodstatusbits[9] ) && ! masked(int_id, m_knownEvtId) )
	errw = errw | (1<< LArFebErrorSummary::EVTID);

      if( ( rodstatusbits[4] | rodstatusbits[11] | rodstatusbits[12]) && ! masked(int_id, m_knownSCACStatus) )
	errw = errw | (1<< LArFebErrorSummary::ScacStatus);

      if (scaOutOfRange)
	errw = errw | (1<< LArFebErrorSummary::ScaOutOfRange);

      if (rodstatusbits[5])
	errw = errw | (1<< LArFebErrorSummary::GainMismatch);

      if (rodstatusbits[24])
	errw = errw | (1<< LArFebErrorSummary::BadGain);

      if (typeMism) 
	errw = errw | (1<< LArFebErrorSummary::TypeMismatch);

      if (badNbOfSp)
	errw = errw | (1<< LArFebErrorSummary::NumOfSamples);

      if (zeroSp && ! masked(int_id,m_knownZeroSample) )
	errw = errw | (1<< LArFebErrorSummary::EmptyDataBlock);

      if (checkSumErr)
	//if (zeroSp)
	errw = errw | (1<< LArFebErrorSummary::CheckSum);


      if (errw!=0){

	// check which errors should be ignored for this feb
	if (!m_badChannelTool.empty()) {
	  const LArBadFeb febStatus = m_badChannelTool->febStatus(febid);
	  unsigned int err_toignore = febStatus.ignoreErrors();
	  if (err_toignore >0) {
	    uint16_t erri = (uint16_t) (err_toignore);
	    errw = errw & ( ~erri); 
	  }
	}


	for (unsigned int i=0;i<LArFebErrorSummary::N_LArFebErrorType;++i){
	  if ( errw & (1<<i)){
	    m_errors[i]+=1; 
	    }
	}
	log<<MSG::DEBUG<< " Error for this FEB id  "<<std::hex<< int_id << " is " <<errw<<endreq;
	
	if ( ! febErrorSummary->set_feb_error(int_id, errw) ) {
	
	  log<<MSG::DEBUG<< " failed to insert the error into LArFebErrorSummary  "<< std::hex << febid << std::dec << endreq;

	}

      }


    }
  
  if (m_checkAllFeb || isHec || isFcal || isEmb || isEmec || isEmPS){
    const uint16_t errw = 1<< LArFebErrorSummary::MissingHeader; 
    bool warn=false;
    std::set<unsigned int>::const_iterator it =  all_febs.begin();
    std::set<unsigned int>::const_iterator it_e =  all_febs.end();
    for(;it!=it_e;++it){
    
      const HWIdentifier febid =  HWIdentifier( Identifier32(*it) ) ; 
      const LArBadFeb febStatus = m_badChannelTool->febStatus(febid);
      if ( febStatus.deadReadout() || febStatus.deadAll() || febStatus.deactivatedInOKS() ) {
	log<<MSG::DEBUG<< " This FEB is not read out  "<< std::hex << *it << std::dec << endreq;
      }
      else {
	//Print warning only for first couple of events and if we have at least one FEB read out
	//(dont' flood log with useless message is LAr is not in the run)
	//if (this->outputLevel()<=MSG::WARNING && m_missingFebsWarns < m_warnLimit &&  hdrCont->size()>0) {
	if (log.level()<=MSG::WARNING && m_missingFebsWarns < m_warnLimit &&  hdrCont->size()>0) {
	  warn=true;
	  const std::string bec= m_onlineHelper->barrel_ec(febid)==0 ? "BARREL/" : "ENDCAP/";
	  const std::string side=m_onlineHelper->pos_neg(febid)==0 ? "C/" : "A/";
	  log<<MSG::WARNING << "FEB [" <<bec<<side<<m_onlineHelper->feedthrough(febid)<<"/"<<m_onlineHelper->slot(febid)<<"] not read out!" << endreq;
	}	

	febErrorSummary->set_feb_error((*it),errw); 
	m_errors[LArFebErrorSummary::MissingHeader]+=1; 
      }//end else missing FEB to report 
	    
    }//end loop over febs
    if (warn) {
      ++m_missingFebsWarns;
      if (m_missingFebsWarns==m_warnLimit)
	log << MSG::WARNING << "No more warnings about FEBs not read out!" << endreq;
    }
  }//end if checkCompletness
  
  return StatusCode::SUCCESS;

}


bool LArFebErrorSummaryMaker::masked (unsigned int hid, const std::vector<unsigned int>& v_feb) const {

  bool is_masked = false ;

  if (v_feb.size()==0 ) return false ; 

  std::vector<unsigned int>::const_iterator it = find(v_feb.begin(),v_feb.end(), hid); 
  if ( it != v_feb.end() ){
    // masked 
    is_masked  = true  ; 
  }

  return is_masked; 

}

StatusCode LArFebErrorSummaryMaker::finalize()
{

  MsgStream  log(messageService(),name());
  
  log<<MSG::INFO<<" In finalize:  Number of Errors for each type" <<endreq; 

  for (unsigned int i=0;i<LArFebErrorSummary::N_LArFebErrorType;++i){
    uint16_t err = 1<<i; 
    log<< MSG::INFO << " type, name, count = " << i << " " << LArFebErrorSummary::error_to_string(err) << " " << m_errors[i] << endreq; 
  }
  
  return StatusCode::SUCCESS;

}

