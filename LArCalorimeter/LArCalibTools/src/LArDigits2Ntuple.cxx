/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArDigits2Ntuple.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawSCContainer.h"
#include "Identifier/HWIdentifier.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArSCDigit.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"

LArDigits2Ntuple::LArDigits2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  LArCond2NtupleBase(name, pSvcLocator),
  m_ipass(0),
  m_event(0), m_FTlist(0), m_fillBCID(false)
{
  declareProperty("ContainerKeys",m_contKeys);
  declareProperty("NSamples",m_Nsamples=32);
  declareProperty("FTlist",m_FTlist);
  declareProperty("FillBCID",m_fillBCID);
  declareProperty("OverwriteEventNumber",m_overwriteEventNumber=false);
  m_ntTitle = "LArDigits";
  m_ntpath  = "/NTUPLES/FILE1/LARDIGITS";  
}

LArDigits2Ntuple::~LArDigits2Ntuple() 
{}


StatusCode LArDigits2Ntuple::initialize()
{
  ATH_MSG_INFO( "in initialize" ); 

  m_isSCFlag = ( std::any_of(m_contKeys.begin(), m_contKeys.end(),[&](std::string const &s) {return s.find("SC") != std::string::npos;} ) );

  ATH_MSG_DEBUG(" IS it SC?? " << m_isSCFlag );
  
  StatusCode sc = LArCond2NtupleBase::initialize();
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "Base init failed" );
    return StatusCode::FAILURE;
  }
  
  sc = m_nt->addItem("IEvent",m_IEvent);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'IEvent' failed" );
    return sc;
  }
  
  sc = m_nt->addItem("samples",m_Nsamples,m_samples);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'samples' failed" );
    return sc;
  }
   
  sc = m_nt->addItem("Nsamples",m_ntNsamples,0,32);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Nsamples' failed" );
    return sc;
  }
  
  if(m_fillBCID){
    sc = m_nt->addItem("BCID",m_bcid);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'BCID' failed" );
      return sc;
    }
  }

  if (m_isSCFlag){
    sc=m_nt->addItem("latomeChannel",m_latomeChannel);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'latomeChannel' failed" );
      return sc;
    }
  }else{
    sc = m_nt->addItem("ELVL1Id",m_ELVL1Id);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'ELVL1Id' failed" );
      return sc;
    }

    sc = m_nt->addItem("Gain",m_gain,-1,3);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'Gain' failed" );
      return sc;
    }
  }
  
  // Loop over container keys
  for ( std::string ck : m_contKeys ){   
    if ( ck.find("SC")  == std::string::npos){	// main readout only
      if ( ck == "LArRawChannels" ){
	m_hasRawChan   = true;
	sc = m_nt->addItem("raw_energy",m_raw_energy);
	if (sc!=StatusCode::SUCCESS) {
	  ATH_MSG_ERROR( "addItem 'raw_energy' failed" );
	  return sc;
	}
      }

    }else if ( ck == "SC" ){	// SC DigitContainer
      sc = m_nt->addItem("bcidVec",m_Nsamples, m_bcidVec);//here - > define length?
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'bcidVec' failed" );
	return sc;
      }
      sc = m_nt->addItem("latomeSourceId",m_latomeSourceId);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'latomeSourceId' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_ADC_BAS" ){	// SC_ADC_BAS DigitContainer
      sc	   = m_nt->addItem("samples_ADC_BAS",m_Nsamples,m_samples_ADC_BAS);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'samples_ADC_BAS' failed" );
	return sc;
      }
 
      sc = m_nt->addItem("bcidVec_ADC_BAS",m_Nsamples, m_bcidVec_ADC_BAS);//here - > define length?
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'bcidVec_ADC_BAS' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_LATOME_HEADER" ){	// SC LATOME HEADER
      sc	   = m_nt->addItem("bcidLATOMEHEAD",m_bcidLATOMEHEAD);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'bcidLATOMEHEAD' failed" );
	return sc;
      }
      sc = m_nt->addItem("latomeidLATOMEHEAD",m_latomeidLATOMEHEAD);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'latomeidLATOMEHEAD' failed" );
	return sc;
      }
      sc = m_nt->addItem("l1idLATOMEHEAD",m_l1idLATOMEHEAD);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'l1idLATOMEHEAD' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_ET" ){ // SC_ET RawSCContainer
      sc = m_nt->addItem("energyVec_ET", m_Nsamples, m_energyVec_ET);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'energyVec_ET' failed" );
	return sc;
      }
      sc = m_nt->addItem("bcidVec_ET", m_Nsamples, m_bcidVec_ET);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'bcidVec_ET' failed" );
	return sc;
      }
      sc = m_nt->addItem("saturVec_ET", m_Nsamples, m_saturVec_ET);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'saturVec_ET' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_ET_ID" ){	// SC_ET_ID RawSCContainer

      sc = m_nt->addItem("energyVec_ET_ID", m_Nsamples, m_energyVec_ET_ID);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'energyVec_ET_ID' failed" );
	return sc;
      }
      sc = m_nt->addItem("bcidVec_ET_ID", m_Nsamples, m_bcidVec_ET_ID);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'bcidVec_ET_ID' failed" );
	return sc;
      }
      sc = m_nt->addItem("saturVec_ET_ID", m_Nsamples, m_saturVec_ET_ID);
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_ERROR( "addItem 'saturVec_ET_ID' failed" );
	return sc;
      }
    }
    
  }// end container key loop
  ATH_MSG_WARNING("Finished container loop");

  ATH_CHECK(m_evtInfoKey.initialize() );

  if( !m_isSCFlag ) ATH_CHECK( m_LArFebHeaderContainerKey.initialize() );
  
  ATH_MSG_WARNING("Checked containerkeys");
  m_ipass	   = 0;
  m_event	   = 0;
  
  return StatusCode::SUCCESS;
  
}

StatusCode LArDigits2Ntuple::execute()
{

  StatusCode	sc;
  
  ATH_MSG_INFO( "in execute" ); 
  m_event++;
  unsigned long long thisevent;
  unsigned long	thisbcid	   = 0;
  unsigned long	thisELVL1Id	   = 0;

  SG::ReadHandle<xAOD::EventInfo>evt (m_evtInfoKey);
  thisevent	   = evt->eventNumber();

  // Get BCID from FEB header
  if ( !m_isSCFlag ){ // we are not processing SC data, Feb header could be accessed
    SG::ReadHandle<LArFebHeaderContainer> hdrCont(m_LArFebHeaderContainerKey);
    if (! hdrCont.isValid()) {
      ATH_MSG_WARNING( "No LArFEB container found in TDS" );
    }
    else {
      ATH_MSG_DEBUG( "LArFEB container found");
      if(m_fillBCID) thisbcid	   = (*hdrCont->begin())->BCId() ;
      ATH_MSG_DEBUG( "BCID FROM FEB HEADER " << thisbcid );
      thisELVL1Id   = (*hdrCont->begin())->ELVL1Id();
      ATH_MSG_DEBUG( "NSAMPLES FROM FEB HEADER " << (*hdrCont->begin())->NbSamples() );
    }
  }else{
    // This should be used for main readout later, once TDAQ fill event headers also in calib. runs properly
    thisbcid	   = evt->bcid();
  }
  /// set it here once and no need to set at each SC/cell
  const LArDigitContainer*	DigitContainer	   = nullptr;
  sc   = evtStore()->retrieve(DigitContainer,m_contKeys.at(0));  
  if (sc       != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Unable to retrieve LArDigitContainer with key " << m_contKeys.at(0) << " from DetectorStore. " );
  } 
  else
    ATH_MSG_DEBUG( "Got LArDigitContainer with key " << m_contKeys.at(0) );

  const LArDigitContainer*	DigitContainer1	   = nullptr;
  const LArRawSCContainer*	etcontainer	   = nullptr;
  const LArRawSCContainer*	etcontainer1	   = nullptr;
  const LArRawChannelContainer*	RawChannelContainer   = nullptr;
  const LArLATOMEHeaderContainer*headcontainer	   = nullptr;
  std::map<unsigned int, const LArLATOMEHeader*> LATOMEHeadMap;
  std::map<HWIdentifier, const LArRawChannel*>	rawChannelMap; 

  if ((std::find(m_contKeys.begin(), m_contKeys.end(), "LArRawChannels")	  != m_contKeys.end()) ){
    sc	   = evtStore()->retrieve(RawChannelContainer,"LArRawChannels");  
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_WARNING( "Unable to retrieve LArRawChannelContainer with key 'LArRawChannels' from DetectorStore. " );
    } 
  }
  if (m_isSCFlag){
    if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_ADC_BAS")  != m_contKeys.end()) ){
      sc	   = evtStore()->retrieve(DigitContainer1,"SC_ADC_BAS");  
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING( "Unable to retrieve LArDigitContainer with key SC_ADC_BAS from DetectorStore. " );
      } 
      else 
	ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_ADC_BAS " );
    }
    
    if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_ET")  != m_contKeys.end()) ){
      sc	   = evtStore()->retrieve(etcontainer,"SC_ET");  
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING( "Unable to retrieve LArRawSCContainer with key SC_ET from DetectorStore. " );
      } 
      else
	ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_ET " );
    }
    
    if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_ET_ID")  != m_contKeys.end()) ){
      sc	   = evtStore()->retrieve(etcontainer1,"SC_ET_ID");  
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING( "Unable to retrieve LArRawSCContainer with key SC_ET_ID from DetectorStore. " );
      } 
      else
	ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_ET_ID" );
    }
    
    if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_LATOME_HEADER")	  != m_contKeys.end()) ){
      sc	   = evtStore()->retrieve(headcontainer,"SC_LATOME_HEADER");  
      if (sc!=StatusCode::SUCCESS) {
	ATH_MSG_WARNING( "Unable to retrieve LArLATOMEHeaderContainer with key SC_LATOME_HEADER from DetectorStore. " );
      } 
      else
	ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_LATOME_HEADER " ); 
    }
    
    if (headcontainer){// loop through header container and fill map
      for (const LArLATOMEHeader* hit : *headcontainer) {
	LATOMEHeadMap.try_emplace ( hit->SourceId(), hit );
      }
    }
  }else if(m_hasRawChan){
    for (const LArRawChannel& raw : *RawChannelContainer) {
      rawChannelMap.try_emplace( raw.channelID(), &raw );
    }
  }// end if m_isSC
  

  /// set container pointers to 0 if size is 0 (avoid checking again the size in many places)
  if( DigitContainer && DigitContainer->size()	  == 0 ) DigitContainer = nullptr;

  if( DigitContainer1 && DigitContainer1->size()  == 0 ) DigitContainer1 = nullptr;

  if( etcontainer && etcontainer->size()	  == 0 ) etcontainer = nullptr;

  if( etcontainer1 && etcontainer1->size()	  == 0 ) etcontainer1 = nullptr;
  
  int cellsno   = 0;
  if (DigitContainer) cellsno	   = DigitContainer->size();
  if (DigitContainer1){
    if ( cellsno == 0 ){ 
      cellsno	   = DigitContainer1->size();
    }else	if(DigitContainer1->size()	  != (unsigned)cellsno ){ ATH_MSG_ERROR(" NOOOOOOO! Different number of entries "<< cellsno << " " << DigitContainer1->size() );
    }
  }
  if (etcontainer){
    if ( cellsno == 0 ){ 
      cellsno	   = etcontainer->size();
    }else	if(etcontainer->size()  != (unsigned)cellsno ){ ATH_MSG_ERROR(" NOOOOOOO! Different number of entries "<< cellsno << " " << etcontainer->size() );
    }
  }
  if (etcontainer1){
    if ( cellsno == 0 ){ 
      cellsno	   = etcontainer1->size();
    }else	if(etcontainer1->size()  != (unsigned)cellsno ){ ATH_MSG_ERROR(" NOOOOOOO! Different number of entries "<< cellsno << " " << etcontainer1->size() );
    }
  }
  unsigned	cellCounter	   = 0;
  for( int c    = 0;c<cellsno;++c ){
    if(m_fillBCID) m_bcid	   = thisbcid; 
    m_IEvent	   = thisevent;
    if(m_overwriteEventNumber) m_IEvent   = m_event;
    if( DigitContainer ){

      const LArDigit* digi   = DigitContainer->at(c);     
      // ======================

      unsigned int trueMaxSample	   = digi->nsamples();
      m_ntNsamples   = trueMaxSample;

      if (!m_isSC){
	m_gain	   = digi->gain();
	m_ELVL1Id	   = thisELVL1Id; 
	if(m_gain < CaloGain::INVALIDGAIN || m_gain > CaloGain::LARNGAIN) m_gain  = CaloGain::LARNGAIN;
      }
      if(trueMaxSample>m_Nsamples){
	if(!m_ipass){
	  ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	  m_ipass   = 1;
	}
	trueMaxSample   = m_Nsamples;
      }
      fillFromIdentifier(digi->hardwareID());      
      if(m_FTlist.size() > 0) {	// should do a selection
	if(std::find(std::begin(m_FTlist), std::end(m_FTlist), m_FT)  == std::end(m_FTlist)) {	// is our FT in list ?
	  continue;
	}
      }
      for(unsigned i =	0; i<trueMaxSample;++i) m_samples[i]	   = digi->samples().at(i);

      if (m_isSC){// LArSCDigit stuff
	const LArSCDigit*	scdigi   = dynamic_cast<const LArSCDigit*>(digi);
	if(!scdigi){ ATH_MSG_DEBUG(" CAN'T CAST ");
	}else{
	  if (headcontainer){
	    const LArLATOMEHeader*headmap   = LATOMEHeadMap[scdigi->SourceId()];
	    if(headmap){
	      m_bcidLATOMEHEAD   = headmap->BCId();
	      m_latomeidLATOMEHEAD   = headmap->LatomeId();
	      m_l1idLATOMEHEAD   = headmap->L1Id();
	    }
	  }   
	  m_latomeChannel	   = scdigi->Channel();
	  for( unsigned i = 0; i<scdigi->BCId().size();++i){
	    m_bcidVec[i]	   = scdigi->BCId().at(i);
	  }	 
	  m_latomeSourceId	   = scdigi->SourceId();
	}
      }
    
      if( RawChannelContainer ){
	const LArRawChannel*	rawchanmap	   = rawChannelMap[digi->hardwareID()];
	if(rawchanmap){
	  m_raw_energy   = rawchanmap->energy();
	}
      }
    }
    


    // DigitContainer 1 -> SC_ADC_BAS
    if( DigitContainer1 ){
      
      const LArDigit* digi = DigitContainer1->at(c);

      unsigned int trueMaxSample = digi->nsamples();
      m_ntNsamples = trueMaxSample;
    
      if(trueMaxSample>m_Nsamples){
        if(!m_ipass){
          ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
          m_ipass=1;
        }
        trueMaxSample = m_Nsamples;
      }

      if( !DigitContainer){ //// already filled in DigitContainer?
        fillFromIdentifier(digi->hardwareID());
      }
         
     for(unsigned i =	0; i<trueMaxSample;++i) m_samples_ADC_BAS[i]   = digi->samples().at(i);

     const LArSCDigit*	scdigi   = dynamic_cast<const LArSCDigit*>(digi);
     if(!scdigi){ ATH_MSG_DEBUG(" CAN'T CAST ");
     }else{
       if ( !DigitContainer){
         if (headcontainer){
           const LArLATOMEHeader*headmap   = LATOMEHeadMap[scdigi->SourceId()];
           if(headmap){
             m_bcidLATOMEHEAD	   = headmap->BCId();
             m_latomeidLATOMEHEAD   = headmap->LatomeId();
             m_l1idLATOMEHEAD	   = headmap->L1Id();
           }
         }
         m_latomeChannel	   = scdigi->Channel();
         m_latomeSourceId	   = scdigi->SourceId();
       }

       for( unsigned i = 0; i<scdigi->BCId().size();++i){
         m_bcidVec_ADC_BAS[i]	   = scdigi->BCId().at(i);
       }
     }
    }

    // etcontainer -> SC_ET
    if( etcontainer ){
      const LArRawSC*rawSC   = etcontainer->at(c);
       
      if ( !DigitContainer && !DigitContainer1 ){
	m_latomeChannel	   = rawSC->chan();
	if (headcontainer){
	  const LArLATOMEHeader*headmap   = LATOMEHeadMap[rawSC->SourceId()];
	  if(headmap){
	    m_bcidLATOMEHEAD	   = headmap->BCId();
	    m_latomeidLATOMEHEAD   = headmap->LatomeId();
	    m_l1idLATOMEHEAD	   = headmap->L1Id();
	  }
	}
      }
      for( unsigned i=0; i<rawSC->bcids().size();++i){	// just use the vector directly?
	m_bcidVec_ET[i]	   = rawSC->bcids().at(i);
      }
      for( unsigned i=0; i<rawSC->energies().size();++i){	// just use the vector directly?
	m_energyVec_ET[i]	   = rawSC->energies().at(i);
      }
      for( unsigned i = 0; i<rawSC->satur().size();++i){	// just use the vector directly?
	m_saturVec_ET[i]	   = rawSC->satur().at(i);
      }

    }
    // etcontainer1 -> SC_ET_ID
    if( etcontainer1 ){
      const LArRawSC*rawSC   = etcontainer1->at(c);

      if ( !DigitContainer && !DigitContainer1 && !etcontainer ){
	m_latomeChannel	   = rawSC->chan();
	if (headcontainer){
	  const LArLATOMEHeader*headmap   = LATOMEHeadMap[rawSC->SourceId()];
	  if(headmap){
	    m_bcidLATOMEHEAD	   = headmap->BCId();
	    m_latomeidLATOMEHEAD   = headmap->LatomeId();
	    m_l1idLATOMEHEAD	   = headmap->L1Id();
	  }
	}
      }
      for( unsigned i=0; i<rawSC->bcids().size();++i){	// just use the vector directly?
	m_bcidVec_ET_ID[i]	   = rawSC->bcids()[i];
      }
      for( unsigned i=0; i<rawSC->energies().size();++i){	// just use the vector directly?
	m_energyVec_ET_ID[i]	   = rawSC->energies()[i];
      }
      for( unsigned i = 0; i<rawSC->satur().size();++i){	// just use the vector directly?
	m_saturVec_ET_ID[i]	   = rawSC->satur()[i];
      }
    }

    sc   = ntupleSvc()->writeRecord(m_nt);
    if (sc != StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "writeRecord failed" );
      return sc;
    }
    cellCounter++;
  }// over cells 
   
  ATH_MSG_DEBUG( "LArDigits2Ntuple has finished." );
  return StatusCode::SUCCESS;
}// end finalize-method.
