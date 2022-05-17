/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArSC2Ntuple.h"
#include "LArRawEvent/LArRawSCContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "LArRawEvent/LArSCDigit.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"

LArSC2Ntuple::LArSC2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  LArDigits2Ntuple(name, pSvcLocator) {
    m_ntTitle = "SCDigits";
    m_ntpath = "/NTUPLES/FILE1/SCDIGITS";
  }

StatusCode LArSC2Ntuple::initialize() {

  ATH_MSG_DEBUG( "LArSC2Ntuple in initialize" ); 

  m_isSC = true;

  ATH_CHECK( LArDigits2Ntuple::initialize() );
  
  StatusCode sc=m_nt->addItem("latomeChannel",m_latomeChannel);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "addItem 'latomeChannel' failed" );
    return sc;
  }
  
  sc = m_nt->addItem("bcidVec",m_Nsamples, m_bcidVec);//here - > define length?
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "addItem 'bcidVec' failed" );
    return sc;
  }
  sc = m_nt->addItem("latomeSourceId",m_latomeSourceId);
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "addItem 'latomeSourceId' failed" );
    return sc;
  }
      
  // Loop over container keys
  for ( const std::string &ck : m_contKeys ){   
    if ( ck.find("SC")  == std::string::npos){	// main readout only
      if ( ck == "LArRawChannels" ){
	m_hasRawChan   = true;
	sc = m_nt->addItem("raw_energy",m_raw_energy);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR( "addItem 'raw_energy' failed" );
	  return sc;
	}
      }

    }else if ( ck == "SC_ADC_BAS" ){	// SC_ADC_BAS DigitContainer
      sc	   = m_nt->addItem("samples_ADC_BAS",m_Nsamples,m_samples_ADC_BAS);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'samples_ADC_BAS' failed" );
	return sc;
      }
 
      sc = m_nt->addItem("bcidVec_ADC_BAS",m_Nsamples, m_bcidVec_ADC_BAS);//here - > define length?
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'bcidVec_ADC_BAS' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_LATOME_HEADER" ){	// SC LATOME HEADER
      sc	   = m_nt->addItem("bcidLATOMEHEAD",m_bcidLATOMEHEAD);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'bcidLATOMEHEAD' failed" );
	return sc;
      }
      sc = m_nt->addItem("latomeidLATOMEHEAD",m_latomeidLATOMEHEAD);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'latomeidLATOMEHEAD' failed" );
	return sc;
      }
      sc = m_nt->addItem("l1idLATOMEHEAD",m_l1idLATOMEHEAD);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'l1idLATOMEHEAD' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_ET" ){ // SC_ET RawSCContainer
      sc = m_nt->addItem("energyVec_ET", m_Nsamples, m_energyVec_ET);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'energyVec_ET' failed" );
	return sc;
      }
      sc = m_nt->addItem("bcidVec_ET", m_Nsamples, m_bcidVec_ET);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'bcidVec_ET' failed" );
	return sc;
      }
      sc = m_nt->addItem("saturVec_ET", m_Nsamples, m_saturVec_ET);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'saturVec_ET' failed" );
	return sc;
      }
      
    }else if ( ck == "SC_ET_ID" ){	// SC_ET_ID RawSCContainer

      sc = m_nt->addItem("energyVec_ET_ID", m_Nsamples, m_energyVec_ET_ID);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'energyVec_ET_ID' failed" );
	return sc;
      }
      sc = m_nt->addItem("bcidVec_ET_ID", m_Nsamples, m_bcidVec_ET_ID);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'bcidVec_ET_ID' failed" );
	return sc;
      }
      sc = m_nt->addItem("saturVec_ET_ID", m_Nsamples, m_saturVec_ET_ID);
      if (sc.isFailure()) {
	ATH_MSG_ERROR( "addItem 'saturVec_ET_ID' failed" );
	return sc;
      }
    }
    
  }// end container key loop
  ATH_MSG_DEBUG("Finished container loop");

  return StatusCode::SUCCESS;
  
}

StatusCode LArSC2Ntuple::execute()
{

  StatusCode	sc;
  
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ATH_MSG_DEBUG( "LArSC2Ntuple in execute" ); 
  m_sc2event++;
  unsigned long long thisevent = 0;
  unsigned long	thisbcid       = 0;

  SG::ReadHandle<xAOD::EventInfo>evt (m_evtInfoKey, ctx);
  thisevent	   = evt->eventNumber();

  // This should be used for main readout later, once TDAQ fill event headers also in calib. runs properly
  thisbcid	   = evt->bcid();
  //
  /// set it here once and no need to set at each SC/cell
  bool hasDigitContainer=true;
  SG::ReadHandle<LArDigitContainer> hdlDigit(m_contKey, ctx);
  if(!hdlDigit.isValid()) {
    ATH_MSG_WARNING( "Unable to retrieve LArDigitContainer with key " << m_contKey << " from DetectorStore. " );
    hasDigitContainer=false;
  } else
    ATH_MSG_DEBUG( "Got LArDigitContainer with key " << m_contKey.key() );

  const LArDigitContainer DigitContainer   = *hdlDigit;

  const LArDigitContainer*	DigitContainer_next	   = nullptr;
  const LArRawSCContainer*	etcontainer	   = nullptr;
  const LArRawSCContainer*	etcontainer_next	   = nullptr;
  const LArRawChannelContainer*	RawChannelContainer   = nullptr;
  const LArLATOMEHeaderContainer*headcontainer	   = nullptr;
  std::map<unsigned int, const LArLATOMEHeader*> LATOMEHeadMap;
  std::map<HWIdentifier, const LArRawChannel*>	rawChannelMap; 

  if ((std::find(m_contKeys.begin(), m_contKeys.end(), "LArRawChannels")	  != m_contKeys.end()) ){
    sc	   = evtStore()->retrieve(RawChannelContainer,"LArRawChannels");  
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve LArRawChannelContainer with key LArRawChannels from DetectorStore. " );
    } 
    else
      ATH_MSG_DEBUG( "Got LArRawChannelContainer with key LArRawChannels" );
  }

  if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_ADC_BAS")  != m_contKeys.end()) ){
    sc	   = evtStore()->retrieve(DigitContainer_next,"SC_ADC_BAS");  
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve LArDigitContainer with key SC_ADC_BAS from DetectorStore. " );
    } 
    else 
      ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_ADC_BAS " );
  }
  
  if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_ET")  != m_contKeys.end()) ){
    sc	   = evtStore()->retrieve(etcontainer,"SC_ET");  
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve LArRawSCContainer with key SC_ET from DetectorStore. " );
    } 
    else
      ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_ET " );
  }
  
  if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_ET_ID")  != m_contKeys.end()) ){
    sc	   = evtStore()->retrieve(etcontainer_next,"SC_ET_ID");  
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "Unable to retrieve LArRawSCContainer with key SC_ET_ID from DetectorStore. " );
    } 
    else
      ATH_MSG_DEBUG( "Got LArDigitContainer with key SC_ET_ID" );
  }
  
  if ((std::find(m_contKeys.begin(), m_contKeys.end(), "SC_LATOME_HEADER")	  != m_contKeys.end()) ){
    sc	   = evtStore()->retrieve(headcontainer,"SC_LATOME_HEADER");  
    if (sc.isFailure()) {
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
  if(m_hasRawChan){
    for (const LArRawChannel& raw : *RawChannelContainer) {
       rawChannelMap.try_emplace( raw.channelID(), &raw );
    }
  }

  /// set container pointers to nullptr if size is 0 (avoid checking again the size in many places)
  if( DigitContainer_next && DigitContainer_next->size()  == 0 ) DigitContainer_next = nullptr;

  if( etcontainer && etcontainer->size()	  == 0 ) etcontainer = nullptr;

  if( etcontainer_next && etcontainer_next->size()	  == 0 ) etcontainer_next = nullptr;
  
  int cellsno   = 0;
  if (hasDigitContainer && DigitContainer.size() >0) cellsno	 = DigitContainer.size();
  if (DigitContainer_next){
    if ( cellsno == 0 ){ 
      cellsno	   = DigitContainer_next->size();
    }else	if(DigitContainer_next->size()	  != (unsigned)cellsno ){ ATH_MSG_ERROR(" NOOOOOOO! Different number of entries in DigitContainer_next"<< cellsno << " " << DigitContainer_next->size() );
    }
  }
  if (etcontainer){
    if ( cellsno == 0 ){ 
      cellsno	   = etcontainer->size();
    }else	if(etcontainer->size()  != (unsigned)cellsno ){ ATH_MSG_ERROR(" NOOOOOOO! Different number of entries in etcontainer"<< cellsno << " " << etcontainer->size() );
    }
  }
  if (etcontainer_next){
    if ( cellsno == 0 ){ 
      cellsno	   = etcontainer_next->size();
    }else	if(etcontainer_next->size()  != (unsigned)cellsno ){ ATH_MSG_ERROR(" NOOOOOOO! Different number of entries in etcontainer_next"<< cellsno << " " << etcontainer_next->size() );
    }
  }
  unsigned	cellCounter	   = 0;
  for( int c    = 0;c<cellsno;++c ){
    if(m_fillBCID) m_bcid	   = thisbcid; 
    m_IEvent	   = thisevent;
    if(m_overwriteEventNumber) m_IEvent   = m_sc2event;
    if( hasDigitContainer ){

      const LArDigit* digi   = DigitContainer.at(c);     
      // ======================

      if(m_FTlist.size() > 0) {	// should do a selection
	if(std::find(std::begin(m_FTlist), std::end(m_FTlist), m_FT)  == std::end(m_FTlist)) {	// is our FT in list ?
	  continue;
	}
      }

      unsigned int trueMaxSample	   = digi->nsamples();
      m_ntNsamples   = trueMaxSample;

      if(trueMaxSample>m_Nsamples){
	if(!m_ipass){
	  ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
	  m_ipass   = 1;
	}
	trueMaxSample   = m_Nsamples;
      }

      fillFromIdentifier(digi->hardwareID());      

      for(unsigned i =	0; i<trueMaxSample;++i) m_samples[i]	   = digi->samples().at(i);

      const LArSCDigit*	scdigi   = dynamic_cast<const LArSCDigit*>(digi);
      if(!scdigi){ 
         ATH_MSG_DEBUG(" Can't cast digi to LArSCDigit*");
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
    

      if( RawChannelContainer ){
	const LArRawChannel*	rawchanmap	   = rawChannelMap[digi->hardwareID()];
	if(rawchanmap){
	  m_raw_energy   = rawchanmap->energy();
	}
      }
    }
    


    // DigitContainer 1 -> SC_ADC_BAS
    if( DigitContainer_next ){
      
      const LArDigit* digi = DigitContainer_next->at(c);

      unsigned int trueMaxSample = digi->nsamples();
      m_ntNsamples = trueMaxSample;
    
      if(trueMaxSample>m_Nsamples){
        if(!m_ipass){
          ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
          m_ipass=1;
        }
        trueMaxSample = m_Nsamples;
      }

      if( !hasDigitContainer){ //// already filled in DigitContainer?
        fillFromIdentifier(digi->hardwareID());
      }
         
     for(unsigned i =	0; i<trueMaxSample;++i) m_samples_ADC_BAS[i]   = digi->samples().at(i);

     const LArSCDigit*	scdigi   = dynamic_cast<const LArSCDigit*>(digi);
     if(!scdigi){ ATH_MSG_DEBUG(" Can't cast digi to LArSCDigit*");
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
    

      if( RawChannelContainer ){
	const LArRawChannel*	rawchanmap	   = rawChannelMap[digi->hardwareID()];
	if(rawchanmap){
	  m_raw_energy   = rawchanmap->energy();
	}
      }
    }
    


    // DigitContainer 1 -> SC_ADC_BAS
    if( DigitContainer_next ){
      
      const LArDigit* digi = DigitContainer_next->at(c);

      unsigned int trueMaxSample = digi->nsamples();
      m_ntNsamples = trueMaxSample;
    
      if(trueMaxSample>m_Nsamples){
        if(!m_ipass){
          ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
          m_ipass=1;
        }
        trueMaxSample = m_Nsamples;
      }

      if( !hasDigitContainer){ //// already filled in DigitContainer?
        fillFromIdentifier(digi->hardwareID());
      }
         
     for(unsigned i =	0; i<trueMaxSample;++i) m_samples_ADC_BAS[i]   = digi->samples().at(i);

     const LArSCDigit*	scdigi   = dynamic_cast<const LArSCDigit*>(digi);
     if(!scdigi){ ATH_MSG_DEBUG(" Can't cast digi to LArSCDigit*");
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
    

      if( RawChannelContainer ){
	const LArRawChannel*	rawchanmap	   = rawChannelMap[digi->hardwareID()];
	if(rawchanmap){
	  m_raw_energy   = rawchanmap->energy();
	}
      }
    }
    


    // DigitContainer 1 -> SC_ADC_BAS
    if( DigitContainer_next ){
      
      const LArDigit* digi = DigitContainer_next->at(c);

      unsigned int trueMaxSample = digi->nsamples();
      m_ntNsamples = trueMaxSample;
    
      if(trueMaxSample>m_Nsamples){
        if(!m_ipass){
          ATH_MSG_WARNING( "The number of samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
          m_ipass=1;
        }
        trueMaxSample = m_Nsamples;
      }

      if( !hasDigitContainer){ //// already filled in DigitContainer?
        fillFromIdentifier(digi->hardwareID());
      }
         
     for(unsigned i =	0; i<trueMaxSample;++i) m_samples_ADC_BAS[i]   = digi->samples().at(i);

     const LArSCDigit*	scdigi   = dynamic_cast<const LArSCDigit*>(digi);
     if(!scdigi){ ATH_MSG_DEBUG(" Can't cast digi to LArSCDigit*");
     }else{
       if ( !hasDigitContainer){
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
       
      if ( !hasDigitContainer && !DigitContainer_next ){
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
    // etcontainer_next -> SC_ET_ID
    if( etcontainer_next ){
      const LArRawSC*rawSC   = etcontainer_next->at(c);

      if ( !hasDigitContainer && !DigitContainer_next && !etcontainer ){
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
   
  ATH_MSG_DEBUG( "LArSC2Ntuple has finished." );
  return StatusCode::SUCCESS;
}// end finalize-method.
