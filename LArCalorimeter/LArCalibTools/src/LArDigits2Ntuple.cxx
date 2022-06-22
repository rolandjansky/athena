/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibTools/LArDigits2Ntuple.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "Identifier/HWIdentifier.h"
#include "LArRawEvent/LArSCDigit.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"

LArDigits2Ntuple::LArDigits2Ntuple(const std::string& name, ISvcLocator* pSvcLocator):
  LArCond2NtupleBase(name, pSvcLocator),
  m_ipass(0),
  m_event(0)
{
  m_ntTitle = "LArDigits";
  m_ntpath  = "/NTUPLES/FILE1/LARDIGITS";  
}

LArDigits2Ntuple::~LArDigits2Ntuple() 
{}


StatusCode LArDigits2Ntuple::initialize()
{
  ATH_MSG_DEBUG( "in initialize" ); 

  ATH_MSG_DEBUG(" IS it SC?? " << m_isSC );
  
  ATH_CHECK( LArCond2NtupleBase::initialize() );
  
  StatusCode sc = m_nt->addItem("IEvent",m_IEvent);
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
  
  sc = m_nt->addItem("Gain",m_gain,0,4);
  if (sc!=StatusCode::SUCCESS) {
    ATH_MSG_ERROR( "addItem 'Gain' failed" );
    return sc;
  }
  
  if(m_fillBCID){
    sc = m_nt->addItem("BCID",m_bcid);
    if (sc!=StatusCode::SUCCESS) {
      ATH_MSG_ERROR( "addItem 'BCID' failed" );
      return sc;
    }
  }

  if(!m_isSC){
    sc = m_nt->addItem("ELVL1Id",m_ELVL1Id);
    if (sc!=StatusCode::SUCCESS) {
       ATH_MSG_ERROR( "addItem 'ELVL1Id' failed" );
       return sc;
    }
  }

  ATH_CHECK(m_contKey.initialize(m_contKey.key().size()) );
  ATH_CHECK(m_evtInfoKey.initialize() );
  ATH_CHECK(m_LArFebHeaderContainerKey.initialize(!m_isSC) );

  m_ipass	   = 0;
  m_event	   = 0;
  
  return StatusCode::SUCCESS;
  
}

StatusCode LArDigits2Ntuple::execute()
{

  if(m_contKey.key().empty()) return StatusCode::SUCCESS;

  StatusCode	sc;
  
  ATH_MSG_DEBUG( "LArDigits2Ntuple in execute" ); 
  m_event++;
  unsigned long long thisevent;
  unsigned long	thisbcid	   = 0;
  unsigned long	thisELVL1Id	   = 0;

  SG::ReadHandle<xAOD::EventInfo>evt (m_evtInfoKey);
  thisevent	   = evt->eventNumber();

  // Get BCID from FEB header
  if ( !m_isSC ){ // we are not processing SC data, Feb header could be accessed
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

  SG::ReadHandle<LArDigitContainer> hdlDigit(m_contKey);
  if(!hdlDigit.isValid()) {
    ATH_MSG_WARNING( "Unable to retrieve LArDigitContainer with key " << m_contKey << " from DetectorStore. " );
    return StatusCode::SUCCESS;
  } else
    ATH_MSG_DEBUG( "Got LArDigitContainer with key " << m_contKey.key() );

  const LArDigitContainer DigitContainer   = *hdlDigit;

  if(!hdlDigit.cptr()) {
     ATH_MSG_WARNING( "No digits in this event ?");
     return StatusCode::SUCCESS;
  }

  unsigned cellCounter=0;
  for( const LArDigit *digi : DigitContainer ){

    if(m_fillBCID) m_bcid	   = thisbcid; 
    m_IEvent	   = thisevent;

    unsigned int trueMaxSample	   = digi->nsamples();

    if (!m_isSC){
      m_gain	   = digi->gain();
      m_ELVL1Id	   = thisELVL1Id; 
      if(m_gain < CaloGain::INVALIDGAIN || m_gain > CaloGain::LARNGAIN) m_gain  = CaloGain::LARNGAIN;
    }
    if(trueMaxSample>m_Nsamples){
      if(!m_ipass){
        ATH_MSG_WARNING( "The number of digi samples in data is larger than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << m_Nsamples << " will be available in the ntuple " );
        m_ipass   = 1;
      }
      trueMaxSample   = m_Nsamples;
    }
    else if(trueMaxSample<m_Nsamples){
      if(!m_ipass){
        ATH_MSG_WARNING( "The number of digi samples in data is lower than the one specified by JO: " << trueMaxSample << " > " << m_Nsamples << " --> only " << trueMaxSample << " will be available in the ntuple " );
        m_ipass   = 1;
      }
    }
    m_ntNsamples   = trueMaxSample;

    fillFromIdentifier(digi->hardwareID());      

    if(m_FTlist.size() > 0) {	// should do a selection
      if(std::find(std::begin(m_FTlist), std::end(m_FTlist), m_FT)  == std::end(m_FTlist)) {	// is our FT in list ?
        continue;
      }
    }

    for(unsigned i =	0; i<trueMaxSample;++i) m_samples[i]	   = digi->samples().at(i);


    ATH_CHECK( ntupleSvc()->writeRecord(m_nt) );
    cellCounter++;
  }// over cells 
   
  ATH_MSG_DEBUG( "LArDigits2Ntuple has finished." );
  return StatusCode::SUCCESS;
}// end finalize-method.
