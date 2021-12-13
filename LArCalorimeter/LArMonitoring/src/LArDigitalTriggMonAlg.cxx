/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArDigitalTriggMonAlg.cxx
// PACKAGE:  LArMonitoring
//
// AUTHOR:   Pavol Strizenec (pavol@mail.cern.ch)
//           Ljiljana Morvaj (ljiljana.morvaj@cern.ch)
//           Based on LAtDigitMon tool by L. Hellary and LArOddCellsMonTool.cxx  by Benjamin Trocme
// 
// Monitor a few things in the LArDigit...
//
//	1) Check that the highest value of the LArDigit is contained in an interval. 
//         If it is not the case increment 3 histograms for each subdetector:
//	      a) Out of range histograms
//	      b) The average histograms: give the average value of the highest digit sample 
//	      c) Channel VS FEB histograms: gives wich slot on wich FEB has his highest digit sample ou of the range
//	2) Check if a digits samples are in saturation. If it's the case increment the saturation histograms. 
//
// Available cuts in the jo file:
//
//   a) SampleRangeLow-SampleRangeUp: range to check the digit sample.
//   b) ADCcut : To select Digits Samples with signal.
//   c) ADCsature: lowest value to check if a Digit sample is in saturation.
// ********************************************************************

#include "LArDigitalTriggMonAlg.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//Histograms
//LAr infos:
#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigit.h"

#include "LArRawEvent/LArSCDigit.h"
#include "LArRawEvent/LArLATOMEHeaderContainer.h"

#include "LArTrigStreamMatching.h"

//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

/*---------------------------------------------------------*/
LArDigitalTriggMonAlg::LArDigitalTriggMonAlg(const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_LArOnlineIDHelper(0),
    m_LArEM_IDHelper(0)
{	


}

/*---------------------------------------------------------*/
LArDigitalTriggMonAlg::~LArDigitalTriggMonAlg()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArDigitalTriggMonAlg::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArDigitalTriggMonAlg" );
  
  /** Get LAr Online Id Helper*/
  if ( detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ).isSuccess() ) {
    ATH_MSG_DEBUG("connected non-tool: LArOnlineID" );    
  } else {
    return StatusCode::FAILURE;    
  }
  
  // Get LArEM Id Helper, not used now...
  if ( detStore()->retrieve( m_LArEM_IDHelper, "LArEM_ID" ).isSuccess() ) {
    ATH_MSG_DEBUG("connected non-tool: LArEM_ID" );
  } else {
    ATH_MSG_FATAL( "unable to connect non-tool: LArEM_ID" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_digitContainerKey.initialize());  
  ATH_CHECK(m_digitContainerKey1.initialize());  
  ATH_CHECK(m_rawSCContainerKey.initialize());  
  ATH_CHECK(m_rawSCContainerKey1.initialize());  
  ATH_CHECK(m_eventInfoKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode LArDigitalTriggMonAlg::fillHistograms(const EventContext& ctx) const
{
  
  ATH_MSG_DEBUG("in fillHists()" );
  
  //monitored variables

  auto MNsamples = Monitored::Scalar<int>("MNsamples",-1);
  auto MlatomeChannel = Monitored::Scalar<int>("MlatomeChannel",-1);
  auto MSCChannel = Monitored::Scalar<int>("MSCChannel",-1);
  auto MlatomeSourceId = Monitored::Scalar<int>("MlatomeSourceId",-1);
  auto Mmaxpos = Monitored::Scalar<int>("Mmaxpos",-1);
  auto Mpartition = Monitored::Scalar<int>("Mpartition",-1);
  auto Msampos = Monitored::Scalar<int>("Msampos",-1);
  auto MADC = Monitored::Scalar<int>("MADC",-1);
  auto MSCeT = Monitored::Scalar<float>("MSCeT",0.0);
  auto MSCsatur = Monitored::Scalar<float>("MSCsatur",0.0);
  auto MlatomeSourceIdBIN = Monitored::Scalar<int>("MlatomeSourceIdBIN",1);
  auto MlatomeSourceIdBIN_subdet = Monitored::Scalar<int>("MlatomeSourceIdBIN_subdet",1);


  /**EventID is a part of EventInfo, search event informations:*/
  SG::ReadHandle<xAOD::EventInfo> thisEvent{m_eventInfoKey,ctx};

  const std::vector<unsigned> streamsThisEvent=LArMon::trigStreamMatching(m_streams,thisEvent->streamTags());
  
  SG::ReadHandle<LArDigitContainer> hLArDigitContainer{m_digitContainerKey,ctx}; //"SC"
  SG::ReadHandle<LArDigitContainer> hLArDigitContainer1{m_digitContainerKey1,ctx};

  SG::ReadHandle<LArRawSCContainer > hSCetContainer{m_rawSCContainerKey,ctx}; //"SC_ET"
  SG::ReadHandle<LArRawSCContainer > hSCetContainer1{m_rawSCContainerKey1,ctx};

  ATH_MSG_DEBUG("hLArDigitContainer->size() " << hLArDigitContainer->size());
  ATH_MSG_DEBUG("hLArDigitContainer1->size() " << hLArDigitContainer1->size());
  ATH_MSG_DEBUG("hSCetContainer->size() " << hSCetContainer->size());
  ATH_MSG_DEBUG("hSCetContainer1->size() " << hSCetContainer1->size());

  ATH_MSG_DEBUG("hLArDigitContainer.isValid() " << hLArDigitContainer.isValid());
  ATH_MSG_DEBUG("hLArDigitContainer1.isValid() " << hLArDigitContainer1.isValid());
  ATH_MSG_DEBUG("hSCetContainer.isValid() " << hSCetContainer.isValid());
  ATH_MSG_DEBUG("hSCetContainer1.isValid() " << hSCetContainer1.isValid());

  const LArDigitContainer*  pLArDigitContainer= nullptr;
  const LArDigitContainer*  pLArDigitContainer1= nullptr;
  const LArRawSCContainer*  pSCetContainer= nullptr;
  const LArRawSCContainer*  pSCetContainer1= nullptr;
  StatusCode sc=evtStore()->retrieve(pLArDigitContainer,"SC");
  sc=evtStore()->retrieve(pLArDigitContainer1,"SC_ADC_BAS");
  sc=evtStore()->retrieve(pSCetContainer,"SC_ET");
  sc=evtStore()->retrieve(pSCetContainer1,"SC_ET_ID");

  ATH_MSG_DEBUG(" pLArDigitContainer: "<< pLArDigitContainer<< " pLArDigitContainer1: "<< pLArDigitContainer1<< " pSCetContainer: "<< pSCetContainer<< " pSCetContainer1: "<< pSCetContainer1);
  ATH_MSG_DEBUG(" pLArDigitContainer->size(): "<< pLArDigitContainer->size()<< " pLArDigitContainer1->size(): "<< pLArDigitContainer1->size()<< " pSCetContainer->size(): "<< pSCetContainer->size()<< " pSCetContainer1->size(): "<< pSCetContainer1->size());


  if (pLArDigitContainer && pLArDigitContainer->size()>0 && hLArDigitContainer.isValid()){
    /** Define iterators to loop over Digits containers*/
    LArDigitContainer::const_iterator itDig = hLArDigitContainer->begin(); 
    LArDigitContainer::const_iterator itDig_e= hLArDigitContainer->end(); 
    const LArDigit* pLArDigit;
    
    /** Loop over digits*/
    for ( ; itDig!=itDig_e;++itDig) {
      pLArDigit = *itDig;
      unsigned int trueNSamples = pLArDigit->nsamples();
      MNsamples = trueNSamples;
      
      fill(m_scMonGroupName, MNsamples);
      
      HWIdentifier id = pLArDigit->hardwareID();
      // will be used in next iteration
      //int channelHWID = m_LArOnlineIDHelper->channel(id);
      
      /** Determine to which partition this channel belongs to*/
      int side = m_LArOnlineIDHelper->pos_neg(id);
      const int ThisPartition=WhatPartition(id,side);
      //    std::string spart = m_partitions[ThisPartition];
      Mpartition = ThisPartition;
      
      /** Check the gain -  SCs have only 1 gain*/
      // will be used in next iteration
      //int gain = pLArDigit->gain();
      
      const LArSCDigit* scdigi = dynamic_cast<const LArSCDigit*>(pLArDigit);
      if(!scdigi){ ATH_MSG_DEBUG(" CAN'T CAST ");
      }else{
	MlatomeChannel = scdigi->Channel();
	fill(m_scMonGroupName, MlatomeChannel);
	
	
	MlatomeSourceId = scdigi->SourceId();
	MlatomeSourceIdBIN=getXbinFromSourceID(MlatomeSourceId);   
	fill(m_scMonGroupName, MlatomeSourceId);   
      }
      
      /** Retrieve samples*/
      const std::vector<short>* digito = &pLArDigit->samples();
      
      /**retrieve the max sample digit ie digitot.back().*/
      std::vector<short>::const_iterator maxSam = std::max_element(digito->begin(), digito->end());
      int thismaxPos=(maxSam-digito->begin());
      Mmaxpos=thismaxPos+1; //count samples [1,5]
      fill(m_scMonGroupName, Mmaxpos, Mpartition, MlatomeSourceIdBIN);
      
      //samples
      for(unsigned i=0; i<trueNSamples;++i) {
	Msampos=i+1;
	MADC=pLArDigit->samples().at(i);
	
	fill(m_scMonGroupName, Msampos, MADC, MlatomeSourceIdBIN);
      }
      
    }/** End of loop on LArDigit*/
    
    
  } // pLArDigitContainer


  if (pSCetContainer && pSCetContainer->size()>0){
    LArRawSCContainer::const_iterator itSC = hSCetContainer->begin();
    LArRawSCContainer::const_iterator itSC_e = hSCetContainer->end();
    const LArRawSC* rawSC = 0;
    
    /** Loop over SCs*/
    for ( ; itSC!=itSC_e;++itSC) {
      rawSC = *itSC;
      MSCChannel = rawSC->chan();
      
      if (rawSC->energies().size()>0){
	MSCeT =   rawSC->energies().at(0);      
      }
      
      if (rawSC->satur().size()>0){
	MSCsatur = rawSC->satur().at(0);      
      }
      
      MlatomeSourceIdBIN=getXbinFromSourceID(rawSC->SourceId());  
      
      fill(m_scMonGroupName, MSCChannel, MSCeT, MSCsatur, MlatomeSourceIdBIN);
      
      if (MSCChannel<10){
	ATH_MSG_DEBUG("--- from  etcontainer rawSC->energies()  = " << MSCeT);
	ATH_MSG_DEBUG("    |______ --------- MSCChannel = "<< MSCChannel);
	ATH_MSG_DEBUG("    |______ --------- MSCsatur = "<< MSCsatur);
	ATH_MSG_DEBUG("    |______ --------- MlatomeSourceIdBIN = "<< MlatomeSourceIdBIN);
	ATH_MSG_DEBUG("    |______ --------- rawSC->SourceId() = "<< rawSC->SourceId());
      }
    }//end loop over SCs
    if (rawSC && (rawSC->energies().size() != 1 || rawSC->satur().size() !=1 ))
      ATH_MSG_WARNING("GAH rawSC->energies().size() = " <<rawSC->energies().size() <<" ,  why didn't I get only 1 SC with 1 energy? or is the saturation? ");
    
  } // if (pSCetContainer){
  
  
  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
/** Say which partition is a channel*/

int LArDigitalTriggMonAlg::WhatPartition(HWIdentifier id, int side) const
{
  
  if (m_LArOnlineIDHelper->isEmBarrelOnline(id)) {
    if(side==0) return 0;
    else return 1;
  } else if (m_LArOnlineIDHelper-> isEMECchannel(id)) {
    if(side==0) return 2;
    else return 3;
  } else if (m_LArOnlineIDHelper->isHECchannel(id)) {
    if(side==0) return 4;
    else return 5;
  } else {
    if(side==0) return 6;
    else return 7;
  }
}

  
  
int LArDigitalTriggMonAlg::getXbinFromSourceID(int sourceID) const
{

  std::map <std::string, std::pair<std::string, int> > LatomeDetBinMapping = {
    {"0x48",{"FCALC",1}},
    {"0x4c",{"EMEC/HECC",3}},
    {"0x44",{"EMECC",11}},
    {"0x4a",{"EMB/EMECC",27}},
    {"0x42",{"EMBC",43}},
    {"0x41",{"EMBA",59}},
    {"0x49",{"EMB/EMECA",75}},
    {"0x43",{"EMECA",91}},
    {"0x4b",{"EMEC/HECA",107}},
    {"0x47",{"FCALA",115}}
  };
  

  //  int NLatomeBins = 117;

  int binx=m_NLatomeBins;
  std::string detName="";
  int detStartingBin=m_NLatomeBins;

  std::stringstream ss;
  ss<< std::hex << sourceID; // int decimal_value
  std::string sourceIDhex ( ss.str() );

  std::string detID=sourceIDhex.substr(0,2);
  //make sure we got the right string
  if (detID=="0x"){
    detID=sourceIDhex.substr(0,4);
  }

  for( auto& pair:LatomeDetBinMapping){

    if (pair.first.find(detID)!=std::string::npos ){
      detName=pair.second.first;
      detStartingBin=pair.second.second;
    }
  }

  //  std::cout<<"sourceIDhex: "<<sourceIDhex<<", detID "<<detID<<", detName "<< detName<< ", det starting Bin "<<detStartingBin<<std::endl;
  
  std::stringstream ss2;
  std::string phiBin = sourceIDhex.substr(sourceIDhex.size()-1);
  ss2 << phiBin;
  int value;
  ss2 >> std::hex >> value;
  
  binx = detStartingBin + value;
  if (binx>m_NLatomeBins){
    ATH_MSG_WARNING("something wrong with binning, filling overflowbin");
    binx=m_NLatomeBins;
  }
  return binx;
}
