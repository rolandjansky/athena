/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBRec/CBNTAA_BeamInstrumentation.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/INTupleSvc.h"

#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBBPCRawCont.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include <vector>

CBNTAA_BeamInstrumentation::CBNTAA_BeamInstrumentation(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator)
{
  declareProperty("ContainerKey1",m_containerKey1="ScintillatorCont");
  declareProperty("ContainerKey2",m_containerKey2="BPCCont");
  declareProperty("ContainerKey3",m_containerKey3="BPCRawCont");
  declareProperty("ContainerKey4",m_containerKey4="TBPhase");
  declareProperty("ContainerKey5",m_containerKey5="TBTrigPat");
  declareProperty("BPCRAW", m_bpc_raw=false);
  declareProperty("BPCnames",m_bpc_names);
  declareProperty("Scintnames",m_scint_names);
  declareProperty("TrigArray",m_trig_array=false);
  declareProperty("TrigPattern",m_triggflag);
  declareProperty("NeverReturnFailure", m_neverReturnFailure=false);
  m_scint_adc=NULL;
  m_scint_tdc=NULL;
  m_bpc_x=NULL;
  m_bpc_y=NULL;
  m_bpc_up=NULL;
  m_bpc_down=NULL;
  m_bpc_left=NULL;
  m_bpc_right=NULL;
  m_bpc_adc_hor=NULL;
  m_bpc_adc_ver=NULL;
  m_trig_sel=NULL;
  m_bpc_ntnames=NULL;
  m_scint_ntnames=NULL;
  m_clock = 0;
  m_trig = 0;
  m_trig_id = 0;
}

CBNTAA_BeamInstrumentation::~CBNTAA_BeamInstrumentation() 
{//Clean up arrays of ntuple entries (if they have been booked)
  if (m_scint_adc)
    delete m_scint_adc;
  if (m_scint_tdc)
    delete m_scint_tdc;
  if ( m_bpc_x)
    delete  m_bpc_x;
  if ( m_bpc_y)
    delete  m_bpc_y;
  if (m_bpc_up)
    delete m_bpc_up;
  if (m_bpc_down)
    delete m_bpc_down;
  if (m_bpc_right)
    delete m_bpc_right;
  if (m_bpc_left)
    delete m_bpc_left;
  if (m_bpc_adc_ver)
    delete m_bpc_adc_ver;
  if (m_bpc_adc_hor)
    delete m_bpc_adc_hor;
  if (m_trig_sel)
     delete m_trig_sel;
}

StatusCode CBNTAA_BeamInstrumentation::CBNT_initialize()
{
  ATH_MSG_DEBUG ( "in initialize()" );

  IToolSvc* toolSvc = nullptr;
  ATH_CHECK( service( "ToolSvc",toolSvc  ) );


  const unsigned nScint=m_scint_names.size();  
  addBranch("Beam_sNames",m_scint_ntnames);
  for (unsigned i=0;i<nScint;i++) {
    const std::string ADCItemName=add_name("scint_",m_scint_names[i]);
    m_scint_ntnames->push_back(ADCItemName);
  }
  addBranch("Beam_sADC",m_scint_adc);
  addBranch("Beam_sTDC",m_scint_tdc);

  const unsigned nBPCs=m_bpc_names.size();
  addBranch("Beam_bNames",m_bpc_ntnames);
  for (unsigned i=0;i<nBPCs;i++) {
    const std::string BPCName=add_name("bpc_",m_bpc_names[i]);
    m_bpc_ntnames->push_back(BPCName);
  }
  addBranch("Beam_bpcX",m_bpc_x);
  addBranch("Beam_bpcY",m_bpc_y);
  
  // BPC RAW, set to false by default
  if(m_bpc_raw==true) {// bpc raw

      addBranch("Beam_bpcUp",m_bpc_up);
      addBranch("Beam_bpcDown",m_bpc_down);
      addBranch("Beam_bpcRight",m_bpc_right);
      addBranch("Beam_bpcLeft",m_bpc_left);
      addBranch("Beam_bpcADCHor",m_bpc_adc_hor);
      addBranch("Beam_bpcADCVer",m_bpc_adc_ver);
    
  } //bpcraw
  
  addBranch("Beam_Trigger",m_trig,"Trigger/l");
  // trigger array
  ATH_MSG_DEBUG (" m_trig_array"<<m_trig_array);
  if (m_trig_array==true) {
    
    ATH_MSG_DEBUG (" loading array ntuple");
    addBranch("Beam_NTrig",m_trig_id,"/l");

    addBranch("Beam_Trig_Array",m_trig_sel);
  }
  
  addBranch ("Beam_Clock",m_clock,"Clock/f");

  return StatusCode::SUCCESS; 
  
}

StatusCode CBNTAA_BeamInstrumentation::CBNT_execute()
{
  const TBScintillatorCont * scintc = nullptr;
  StatusCode sc = evtStore()->retrieve(scintc,m_containerKey1);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR ( " Cannot read TBScintillatorCont from StoreGate! key= " << m_containerKey1 );
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    {
      const unsigned nScint=m_scint_names.size();
      ATH_MSG_DEBUG ( "Filling for "<<nScint<<" scintilators");
//      m_scint_adc->reserve(nScint);
//      m_scint_tdc->reserve(nScint);
      m_scint_adc->resize(nScint);
      m_scint_tdc->resize(nScint);
      TBScintillatorCont::const_iterator it_scint = scintc->begin();
      TBScintillatorCont::const_iterator last_scint = scintc->end();
      for(;it_scint!=last_scint;it_scint++) {
	const TBScintillator * scint = (*it_scint);
 	const std::string name = scint->getDetectorName();
	//In the initialize method we build the vectors of ntuple-items for ADCs and BPCs that
	//the same scintillators have the same index. We search the name-vector and use the index
        //for m_scint_adc and m_scint_tdc
	unsigned NtupleVectorIndex;
	for ( NtupleVectorIndex=0; NtupleVectorIndex!=nScint; NtupleVectorIndex++) 
	  if (name==m_scint_names[NtupleVectorIndex]) {
	    //	    log << MSG::DEBUG << "Scintillator '" << name << "' not found." << endreq;
	    break;
	  }
	if (NtupleVectorIndex==nScint) {
          ATH_MSG_DEBUG ( "Not found a scint name: "<<name);
	  continue; //Name not found for this event
        }
	//Otherwise: Fill nutple
	(*m_scint_adc)[NtupleVectorIndex] = scint->getSignal();
	(*m_scint_tdc)[NtupleVectorIndex] = scint->getTimeSignal();
      } //end loop over scintillator-container
    }
  
  const TBBPCCont * bpcc = nullptr;
  sc = evtStore()->retrieve(bpcc,m_containerKey2);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR ( " Cannot read TBBPCCont from StoreGate! key= " << m_containerKey2 );
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    {
      const unsigned nBPCs=m_bpc_names.size();
      m_bpc_x->reserve(nBPCs);
      m_bpc_y->reserve(nBPCs);
      TBBPCCont::const_iterator it_bpc = bpcc->begin();
      TBBPCCont::const_iterator last_bpc = bpcc->end();
      for(;it_bpc!=last_bpc;it_bpc++) {
        const TBBPC * bpc = (*it_bpc);
	std::string name = bpc->getDetectorName();
	unsigned NtupleVectorIndex;
	for ( NtupleVectorIndex=0; NtupleVectorIndex!=nBPCs; NtupleVectorIndex++) 
	  if (name==m_bpc_names[NtupleVectorIndex])
	    break;
        if (NtupleVectorIndex==nBPCs) 
	  continue; //Name not found for this event
	//Otherwise: Fill nutple
	(*m_bpc_x)[NtupleVectorIndex] = bpc->getXPos();
	(*m_bpc_y)[NtupleVectorIndex] = bpc->getYPos();
      } //end loop over scintillator container
    }
    
  if (m_bpc_raw==true) { //bpc raw
   TBBPCRawCont * bpccr = nullptr;
   sc = evtStore()->retrieve(bpccr, m_containerKey3);
    if (sc.isFailure()){
      ATH_MSG_ERROR ( " Cannot read TBBPCRawCont from StoreGate! key= " <<m_containerKey3);
    } else {
      const unsigned nBPCs=m_bpc_names.size();
      m_bpc_up->reserve(nBPCs);
      m_bpc_down->reserve(nBPCs);
      m_bpc_right->reserve(nBPCs);
      m_bpc_left->reserve(nBPCs);
      m_bpc_adc_hor->reserve(nBPCs);
      m_bpc_adc_ver->reserve(nBPCs);   
      TBBPCRawCont::const_iterator it_bpcr = bpccr->begin();
      TBBPCRawCont::const_iterator last_bpcr = bpccr->end();      
      for(;it_bpcr!=last_bpcr;it_bpcr++) {
        const TBBPCRaw * bpcr = (*it_bpcr);
	std::string name = bpcr->getDetectorName();
        unsigned NtupleVectorIndex;
        for ( NtupleVectorIndex=0; NtupleVectorIndex!=nBPCs; NtupleVectorIndex++){
	  ATH_MSG_DEBUG (" loading 1");
          if (name==m_bpc_names[NtupleVectorIndex]){
	    ATH_MSG_DEBUG (" Ntuple Vector"<<NtupleVectorIndex);
	    (*m_bpc_up)[NtupleVectorIndex] = bpcr->getTDCUp();
	    (*m_bpc_down)[NtupleVectorIndex] = bpcr->getTDCDown();
	    (*m_bpc_right)[NtupleVectorIndex] = bpcr->getTDCRight();
	    (*m_bpc_left)[NtupleVectorIndex] = bpcr->getTDCLeft();
	    (*m_bpc_adc_hor)[NtupleVectorIndex] = bpcr->getADCHorizontal();
	    (*m_bpc_adc_ver)[NtupleVectorIndex] = bpcr->getADCVertical();
	  }
	}
      }    
    }
  } //bpcraw
  
  const TBPhase * phase = nullptr;
  sc = evtStore()->retrieve(phase,m_containerKey4);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR ( " Cannot read TBPhase from StoreGate! key= " << m_containerKey4 );
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    { 
      m_clock = phase->getPhase();
    }
  
  const TBTriggerPatternUnit * trig = nullptr;
  sc = evtStore()->retrieve(trig,m_containerKey5);
  if (sc.isFailure()) 
    {
      ATH_MSG_ERROR ( " Cannot read TBTriggerPatternUnit from StoreGate! key= " << m_containerKey5 );
      if (m_neverReturnFailure) {
	return StatusCode::SUCCESS;
      } else {
	return StatusCode::FAILURE;
      }
    }
  else
    { 
      m_trig = trig->getTriggerWord();
    }

  
  //Trigger Array, returns array of triggers with corresponding 0 or 1 for each event

  if (m_trig_array==true) {
    unsigned int word =trig->getTriggerWord();
  
    // Checking if objects in triggflag triggered
    ATH_MSG_DEBUG(m_triggflag.size());
    m_trig_sel->reserve(m_triggflag.size()+1); 
    for(int unsigned i=0;i<m_triggflag.size()+1;i++){
      bool triggbol=0;
      m_trig_id=i;
      triggbol=((word & m_triggflag[i])!=0);
      if(triggbol){
	(*m_trig_sel)[i]=1;
      }
    }
  }

  return StatusCode::SUCCESS;
}  

StatusCode CBNTAA_BeamInstrumentation::CBNT_clear()
{
  if (m_scint_adc)
     m_scint_adc->clear();
  if (m_scint_tdc)
     m_scint_tdc->clear();
  if ( m_bpc_x)
      m_bpc_x->clear();
  if ( m_bpc_y)
      m_bpc_y->clear();
  if (m_bpc_up)
     m_bpc_up->clear();
  if (m_bpc_down)
     m_bpc_down->clear();
  if (m_bpc_right)
     m_bpc_right->clear();
  if (m_bpc_left)
     m_bpc_left->clear();
  if (m_bpc_adc_ver)
     m_bpc_adc_ver->clear();
  if (m_bpc_adc_hor)
     m_bpc_adc_hor->clear();
  if (m_trig_sel)
     m_trig_sel->clear();

  return StatusCode::SUCCESS;
}

StatusCode CBNTAA_BeamInstrumentation::CBNT_finalize()
{
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}

std::string CBNTAA_BeamInstrumentation::add_name(const char* base, const std::string extension) {
  std::string retval(base);
  for (unsigned i=0;i<extension.size();i++) {
    const char& ch=extension[i];
    if (ch=='=' || ch==':' || ch=='/')
      continue; //Inore these characters
    else if (ch=='-')
      retval.append("m");//replace by letter m
    else if (ch=='+')
      retval.append("p");//replace by letter p
    else
      retval.append(1,ch);
  }
  return retval;
}
