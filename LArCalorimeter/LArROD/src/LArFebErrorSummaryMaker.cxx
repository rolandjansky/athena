/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArROD/LArFebErrorSummaryMaker.h"

#include "LArRawEvent/LArFebErrorSummary.h" 
#include "LArIdentifier/LArOnlineID.h" 

#include <bitset>


StatusCode LArFebErrorSummaryMaker::initialize()
{
  ATH_MSG_DEBUG(" initialize " );

  ATH_CHECK(detStore()->retrieve( m_onlineHelper ) );

  if (m_checkAllFeb)
    {
      std::vector<HWIdentifier>::const_iterator it = m_onlineHelper->feb_begin();
      std::vector<HWIdentifier>::const_iterator it_e = m_onlineHelper->feb_end();

      for (;it!=it_e;++it){
          m_all_febs.insert((*it).get_identifier32().get_compact());
      }
      ATH_MSG_INFO(" number of FEBs from LArOnlineID  "<< m_all_febs.size()  );
    }
  else { // We should chack partition id
     if(m_partition.size() > 0) {
        unsigned length = m_partition.size();
        if(m_partition.value().find("LArgHecFcal") < length && m_partition.value().find("EB-HEC") < length) m_isHec=true;
        if(m_partition.value().find("LArgHecFcal") < length && m_partition.value().find("EB-FCAL") < length) m_isFcal=true;
        if(m_partition.value().find("LArgBarrelPS") < length)  {
           m_isEmPS = true;
           if(m_partition.value().find("EB-EMBA") < length) m_isAside = true;
           if(m_partition.value().find("EB-EMBC") < length) m_isCside = true;
        }
        if(m_partition.value().find("LArgEm") < length && m_partition.value().find("EB-EMB") < length)  {
           m_isEmb = true;
           if(m_partition.value().find("EB-EMBA") < length) m_isAside = true;
           if(m_partition.value().find("EB-EMBC") < length) m_isCside = true;
        }
        if(m_partition.value().find("LArgEm") < length && m_partition.value().find("EB-EMEC") < length)  {
           m_isEmec = true;
           if(m_partition.value().find("EB-EMECA") < length) m_isAside = true;
           if(m_partition.value().find("EB-EMECC") < length) m_isCside = true;
        }
     }
     if(m_isHec || m_isFcal || m_isEmb || m_isEmec || m_isEmPS) {
       ATH_MSG_DEBUG("m_isHec: "<<m_isHec<<" m_isFcal: "<< m_isFcal <<" m_isEmb: "<< m_isEmb <<" m_isEmec: "<< m_isEmec <<" m_isEmbPS: "<<  m_isEmPS );
     } else {
       ATH_MSG_WARNING("Wrong PartitionId property: "<<m_partition.value() );
       ATH_MSG_WARNING("Missing FEB's will be not checked " );
     }
     // Now let's build the list of FEB's according partition
     std::vector<HWIdentifier>::const_iterator it = m_onlineHelper->feb_begin();
     std::vector<HWIdentifier>::const_iterator it_e = m_onlineHelper->feb_end();

     for (;it!=it_e;++it){
        if(m_isHec && m_onlineHelper->isHECchannel(*it)) { m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
        if(m_isFcal && m_onlineHelper->isFCALchannel(*it)) { m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
        if((m_isEmb && m_onlineHelper->isEMBchannel(*it) && (!m_onlineHelper->isPS(*it))) 
              || (m_isEmec && m_onlineHelper->isEMECchannel(*it)) 
              || (m_isEmPS && m_onlineHelper->isPS(*it) && (!m_onlineHelper->isEMBchannel(*it)))  ) { 
           if(m_isAside && m_onlineHelper->pos_neg(*it) == 1) {m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
           if(m_isCside && m_onlineHelper->pos_neg(*it) == 0) {m_all_febs.insert((*it).get_identifier32().get_compact()); continue; }
        }
     }
  }
  ATH_MSG_INFO("Number of expected FEB's: "<<m_all_febs.size() );

  ATH_CHECK( m_readKey.initialize() );
  ATH_CHECK( m_writeKey.initialize() );
  ATH_CHECK( m_bfKey.initialize());


  //Set error counters to 0
  for (unsigned int i=0;i<LArFebErrorSummary::N_LArFebErrorType;++i){
    m_errors[i]=0;
  }

  ATH_MSG_INFO(" initialized "  );

  return StatusCode::SUCCESS ; 

} 


StatusCode LArFebErrorSummaryMaker::execute(const EventContext& ctx) const 
{
  ATH_MSG_DEBUG(" execute " );

  SG::ReadHandle<LArFebHeaderContainer> hdrCont{m_readKey,ctx};
  
  SG::WriteHandle<LArFebErrorSummary> febErrorSummary = SG::makeHandle(m_writeKey,ctx);
  ATH_CHECK(  febErrorSummary.record (std::make_unique<LArFebErrorSummary>()) );

  SG::ReadCondHandle<LArBadFebCont> h_bf{m_bfKey,ctx};
  const LArBadFebCont* badFebs{*h_bf};

  unsigned int nbSamplesFirst=0;
  uint32_t eventTypeFirst = 999;

  ATH_MSG_DEBUG( " LArFebHeaderContainer Size = "<< hdrCont->size()   );

  std::set<unsigned int> all_febs;
  if(m_checkAllFeb || m_isHec || m_isFcal || m_isEmb || m_isEmec || m_isEmPS){

    all_febs= m_all_febs;
    
  }
    

  for (const auto it : *hdrCont) {
      HWIdentifier febid= it->FEBId();
      unsigned int int_id =  febid.get_identifier32().get_compact();
   
      if (m_checkAllFeb || m_isHec || m_isFcal || m_isEmb || m_isEmec || m_isEmPS){
	all_febs.erase(int_id);
      }
      // ctrl3 
      // const std::vector <uint16_t> &febctrl3 = it->FebCtrl3();

      // Retrieve rodstatus from DSP header
      uint32_t rodstatus = it->RodStatus();
      // Retrieve SCA adresses
      const std::vector <uint16_t> &sca = it->SCA();
      // Eventype = 2 : transparent/raw data - 4 : Physic - 7 : calibration - 10 : pedestal
      uint32_t eventType = it->DetEventType();      

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
        if (eventType == 4 && (it->NbSamples() != 0)) nbSamplesFirst=it->NbSamples();
      }
      // Test the uniformity of number of samples and that sca # is inside [0;143] only in raw data                     
      if (eventType == 2){
	if (nbSamplesFirst!=sca.size() && sca.size()!=0) badNbOfSp = true;
	for (unsigned int i=0; i<sca.size(); ++i){
	  if (sca[i]>143) scaOutOfRange = true;
	}
      }
      if (eventType == 4 && (nbSamplesFirst != it->NbSamples()) && (it->NbSamples() != 0)) badNbOfSp = true;

      // Test that the number of samples is not zero only in raw data and results mode                                  
      if (eventType == 2 &&  sca.size() == 0) zeroSp = true;
      if (eventType == 4 &&  (it->RodResults1Size()  == 0)) zeroSp = true;


      if (!zeroSp){
	if (!it->ChecksumVerification()) checkSumErr = true; 
	if (eventType==4) {
	  int expSize1 = 0;
	  if ((it->FormatVersion()&0xF)<=11) expSize1 = it->NbSweetCells1() + 83 + (int) ceilf((it->NbSamples()+1)/2.0); // Old DSP version
	  if ((it->FormatVersion()&0xF)>=12) expSize1 = it->NbSweetCells1() + 84 + (int) ceilf((it->NbSamples()+1)/2.0); // New DSP version after 07/11 with new extra word SumE
	  if (expSize1 != it->RodResults1Size()) {
		checkSumErr = true; 
	  }
	  int nbOf32bits = (it->NbSweetCells2() * it->NbSamples() +1) / 2.0;
	  if ( nbOf32bits != it->RodResults2Size()) {
		checkSumErr = true; 
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
	const LArBadFeb febStatus = badFebs->status(febid);
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
      ATH_MSG_DEBUG( " Error for this FEB id  "<<std::hex<< int_id << " is " <<errw );
	
      if ( ! febErrorSummary->set_feb_error(int_id, errw) ) {
	
	ATH_MSG_DEBUG( " failed to insert the error into LArFebErrorSummary  "<< std::hex << febid << std::dec  );

      }

  }
  
  if (m_checkAllFeb || m_isHec || m_isFcal || m_isEmb || m_isEmec || m_isEmPS){
    const uint16_t errw = 1<< LArFebErrorSummary::MissingHeader; 
    bool warn=false;
    for (auto it : all_febs) { 
      const HWIdentifier febid =  HWIdentifier( Identifier32(it) ) ; 
      const LArBadFeb febStatus = badFebs->status(febid);
      if ( febStatus.deadReadout() || febStatus.deadAll() || febStatus.deactivatedInOKS() ) {
	ATH_MSG_DEBUG( " This FEB is not read out  0x"<< std::hex << it << std::dec  );
      }
      else {
	//Print warning only for first couple of events and if we have at least one FEB read out
	//(dont' flood log with useless message is LAr is not in the run)
	//if (this->outputLevel()<=MSG::WARNING && m_missingFebsWarns < m_warnLimit &&  hdrCont->size()>0) {
	if (msgLvl(MSG::WARNING) && m_missingFebsWarns.load() < m_warnLimit &&  hdrCont->size()>0) {
	  warn=true;
	  const std::string bec= m_onlineHelper->barrel_ec(febid)==0 ? "BARREL/" : "ENDCAP/";
	  const std::string side=m_onlineHelper->pos_neg(febid)==0 ? "C/" : "A/";
	  ATH_MSG_WARNING( "FEB [" <<bec<<side<<m_onlineHelper->feedthrough(febid)<<"/"<<m_onlineHelper->slot(febid)<<"] not read out!"  );
	}	

	febErrorSummary->set_feb_error(it,errw); 
	m_errors[LArFebErrorSummary::MissingHeader]+=1; 
      }//end else missing FEB to report 
	    
    }//end loop over febs
    if (warn) {
      if ((++m_missingFebsWarns)==m_warnLimit)
	ATH_MSG_WARNING( "No more warnings about FEBs not read out!"  );
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

  ATH_MSG_INFO(" In finalize:  Number of Errors for each type"  );

  for (unsigned int i=0;i<LArFebErrorSummary::N_LArFebErrorType;++i){
    uint16_t err = 1<<i; 
    ATH_MSG_INFO( " type, name, count = " << i << " " << LArFebErrorSummary::error_to_string(err) << " " << m_errors[i].load()  );
  }
  
  return StatusCode::SUCCESS;

}

