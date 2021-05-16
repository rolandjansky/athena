/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CBNT_BeamInstrumentation.h"

#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBBPCRawCont.h"
#include "TBEvent/TBPhase.h"
#include "TBEvent/TBTriggerPatternUnit.h"
#include <vector>

CBNT_BeamInstrumentation::CBNT_BeamInstrumentation(const std::string & name, ISvcLocator * pSvcLocator) : CBNT_TBRecBase(name, pSvcLocator)
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
  m_trig_sel = 0;
  m_clock = 0;
  m_trig = 0;
  m_trig_id = 0;
}

CBNT_BeamInstrumentation::~CBNT_BeamInstrumentation() 
{//Clean up arrays of ntuple entries (if they have been booked)
  if (m_scint_adc)
    delete[] m_scint_adc;
  if (m_scint_tdc)
    delete[] m_scint_tdc;
  if ( m_bpc_x)
    delete[]  m_bpc_x;
  if ( m_bpc_y)
    delete[]  m_bpc_y;
  if (m_bpc_up)
    delete[] m_bpc_up;
  if (m_bpc_down)
    delete[] m_bpc_down;
  if (m_bpc_right)
    delete[] m_bpc_right;
  if (m_bpc_left)
    delete[] m_bpc_left;
  if (m_bpc_adc_ver)
    delete[] m_bpc_adc_ver;
  if (m_bpc_adc_hor)
    delete[] m_bpc_adc_hor;
}

StatusCode CBNT_BeamInstrumentation::CBNT_initialize()
{
  ATH_MSG_DEBUG ( "in initialize()" );

//  NTuple::Tuple* ntuplePtr;
//  ntuplePtr = this->nt();
  const unsigned nScint=m_scint_names.size();  
  m_scint_adc=new float[nScint];
  m_scint_tdc=new float[nScint];
  for (unsigned i=0;i<nScint;i++) {
    const std::string ADCItemName=add_name("Beam/sADC_",m_scint_names[i]);
    const std::string ADCItemNameDesc = ADCItemName + "/F";
//    sc=ntuplePtr->addItem(ADCItemName,m_scint_adc[i],0,4095);
    addBranch(ADCItemName,m_scint_adc[i],ADCItemNameDesc);
    const std::string TDCItemName=add_name("Beam/sTDC_",m_scint_names[i]);
    const std::string TDCItemNameDesc=TDCItemName+"/F";
//    sc=ntuplePtr->addItem(TDCItemName,m_scint_tdc[i],0,4095);
    addBranch(TDCItemName,m_scint_tdc[i],TDCItemNameDesc);
  }

  const unsigned nBPCs=m_bpc_names.size();
  m_bpc_x=new float[nBPCs];
  m_bpc_y=new float[nBPCs];
  for (unsigned i=0;i<nBPCs;i++) {
    const std::string BPCxItemName=add_name("Beam/bpcX_",m_bpc_names[i]);
    const std::string BPCxItemNameDesc=BPCxItemName+"/F";
//    sc = ntuplePtr->addItem (BPCxItemName,m_bpc_x[i],-150,150);
    addBranch(BPCxItemName,m_bpc_x[i],BPCxItemNameDesc);
    const std::string BPCyItemName=add_name("Beam/bpcY_",m_bpc_names[i]);
    const std::string BPCyItemNameDesc=BPCyItemName+"/F";
    addBranch(BPCyItemName,m_bpc_y[i],BPCyItemNameDesc);
  }
  
  // BPC RAW, set to false by default
  if(m_bpc_raw==true) {// bpc raw

    m_bpc_up=new float[nBPCs];
    m_bpc_down=new float[nBPCs];
    m_bpc_left=new float[nBPCs];
    m_bpc_right=new float[nBPCs];
    m_bpc_adc_hor=new float[nBPCs];
    m_bpc_adc_ver=new float[nBPCs];
    
    for (unsigned i=0;i<nBPCs;i++) {
      const std::string BPCupItemName=add_name("Beam/bpcUp_",m_bpc_names[i]);
      const std::string BPCupItemNameDesc=BPCupItemName+"/F";
//      sc = ntuplePtr->addItem(BPCupItemName,m_bpc_up[i],0,2056);
      addBranch(BPCupItemName,m_bpc_up[i],BPCupItemNameDesc);
      const std::string BPCdownItemName=add_name("Beam/bpcDown_",m_bpc_names[i]);
      const std::string BPCdownItemNameDesc=BPCdownItemName+"/F";
      addBranch(BPCdownItemName,m_bpc_down[i],BPCdownItemNameDesc);
      const std::string BPCrightItemName=add_name("Beam/bpcRight_",m_bpc_names[i]);
      const std::string BPCrightItemNameDesc=BPCrightItemName+"/F";
      addBranch(BPCrightItemName,m_bpc_right[i],BPCrightItemNameDesc);
      const std::string BPCleftItemName=add_name("Beam/bpcLeft_",m_bpc_names[i]);
      const std::string BPCleftItemNameDesc=BPCleftItemName+"/F";
      addBranch(BPCleftItemName,m_bpc_left[i],BPCleftItemNameDesc);
      const std::string BPChorItemName=add_name("Beam/bpcADCHor_",m_bpc_names[i]);
      const std::string BPChorItemNameDesc=BPChorItemName+"/F";
      addBranch(BPChorItemName,m_bpc_adc_hor[i],BPChorItemNameDesc);
      const std::string BPCverItemName=add_name("Beam/bpcADCVer_",m_bpc_names[i]);
      const std::string BPCverItemNameDesc=BPCverItemName+"/F";
      addBranch(BPCverItemName,m_bpc_adc_ver[i],BPCverItemNameDesc);
    }
  } //bpcraw
  
//  sc = ntuplePtr->addItem ("Beam/Trigger",m_trig, 0, 20);
  addBranch("Beam/Trigger",m_trig,"Trigger/I");
  // trigger array
  ATH_MSG_DEBUG (" m_trig_array"<<m_trig_array);
  if (m_trig_array==true) {
    
    ATH_MSG_DEBUG (" loading array ntuple");
//    sc = ntuplePtr->addItem("Beam/NTrig",m_trig_id,0,50);
    addBranch("Beam/NTrig",m_trig_id,"Ntrig/I");

//    sc = ntuplePtr->addItem("Beam/Trig_Array",m_trig_id,m_trig_sel);
    addBranch("Beam/Trig_Array",m_trig_sel);
  }
  
//  sc = ntuplePtr->addItem ("Beam/Clock",m_clock, 0, 10000);
  addBranch ("Beam/Clock",m_clock, "Clock/F");
  return StatusCode::SUCCESS; 
  
}

StatusCode CBNT_BeamInstrumentation::CBNT_execute()
{
  ATH_MSG_DEBUG ( "in execute()" );

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
      for (const TBScintillator * scint : *scintc) {
 	const std::string name = scint->getDetectorName();
	//In the initialize method we build the vectors of ntuple-items for ADCs and BPCs that
	//the same scintillators have the same index. We search the name-vector and use the index
        //for m_scint_adc and m_scint_tdc
	unsigned NtupleVectorIndex;
	for ( NtupleVectorIndex=0; NtupleVectorIndex!=nScint; NtupleVectorIndex++) 
	  if (name==m_scint_names[NtupleVectorIndex]) {
	    //	    log << MSG::DEBUG << "Scintillator '" << name << "' not found." << endmsg;
	    break;
	  }
	if (NtupleVectorIndex==nScint) 
	  continue; //Name not found for this event
	//Otherwise: Fill nutple
	m_scint_adc[NtupleVectorIndex] = scint->getSignal();
	m_scint_tdc[NtupleVectorIndex] = scint->getTimeSignal();
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
      for (const TBBPC * bpc : *bpcc) {
	std::string name = bpc->getDetectorName();
	unsigned NtupleVectorIndex;
	for ( NtupleVectorIndex=0; NtupleVectorIndex!=nBPCs; NtupleVectorIndex++) 
	  if (name==m_bpc_names[NtupleVectorIndex])
	    break;
        if (NtupleVectorIndex==nBPCs) 
	  continue; //Name not found for this event
	//Otherwise: Fill nutple
	m_bpc_x[NtupleVectorIndex] = bpc->getXPos();
	m_bpc_y[NtupleVectorIndex] = bpc->getYPos();
      } //end loop over scintillator container
    }
    
  if (m_bpc_raw==true) { //bpc raw
   TBBPCRawCont * bpccr = nullptr;
    sc = evtStore()->retrieve(bpccr, m_containerKey3);
    if (sc.isFailure()){
      ATH_MSG_ERROR ( " Cannot read TBBPCRawCont from StoreGate! key= " <<m_containerKey3);
    } else {
      const unsigned nBPCs=m_bpc_names.size();
      for (const TBBPCRaw * bpcr : *bpccr) {
	std::string name = bpcr->getDetectorName();
        unsigned NtupleVectorIndex;
        for ( NtupleVectorIndex=0; NtupleVectorIndex!=nBPCs; NtupleVectorIndex++){
	  ATH_MSG_DEBUG (" loading 1");
          if (name==m_bpc_names[NtupleVectorIndex]){
	    ATH_MSG_DEBUG (" Ntuple Vector"<<NtupleVectorIndex);
	    m_bpc_up[NtupleVectorIndex] = bpcr->getTDCUp();
	    m_bpc_down[NtupleVectorIndex] = bpcr->getTDCDown();
	    m_bpc_right[NtupleVectorIndex] = bpcr->getTDCRight();
	    m_bpc_left[NtupleVectorIndex] = bpcr->getTDCLeft();
	    m_bpc_adc_hor[NtupleVectorIndex] = bpcr->getADCHorizontal();
	    m_bpc_adc_ver[NtupleVectorIndex] = bpcr->getADCVertical();
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

StatusCode CBNT_BeamInstrumentation::CBNT_finalize()
{
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}

std::string CBNT_BeamInstrumentation::add_name(const char* base, const std::string& extension) {
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

StatusCode CBNT_BeamInstrumentation::CBNT_clear()
{
   if(m_trig_sel) m_trig_sel->clear();
   return StatusCode::SUCCESS;
}
