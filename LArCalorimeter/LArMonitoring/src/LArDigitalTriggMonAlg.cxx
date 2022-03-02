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
//           Yesenia Hernadez (yesenia@cern.ch)
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
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
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
    m_LArEM_IDHelper(0),
    m_SCID_helper(0)
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

  /** Get LAr Online SC Id Helper*/
  if ( detStore()->retrieve( m_SCID_helper, "CaloCell_SuperCell_ID" ).isSuccess() ) {
    ATH_MSG_DEBUG("connected non-tool: CaloCell_SuperCell_ID" );    
  } else {
    return StatusCode::FAILURE;    
  }

  ATH_CHECK(m_digitContainerKey.initialize());  
  ATH_CHECK(m_rawSCContainerKey.initialize());  
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_cablingKey.initialize());

  return AthMonitorAlgorithm::initialize();
}

StatusCode LArDigitalTriggMonAlg::fillHistograms(const EventContext& ctx) const
{
  
  ATH_MSG_DEBUG("in fillHists()" );

  //monitored variables

  auto MNsamples = Monitored::Scalar<int>("MNsamples",-1);
  auto MSCChannel = Monitored::Scalar<int>("MSCChannel",-1);
  auto MlatomeSourceId = Monitored::Scalar<int>("MlatomeSourceId",-1);
  auto Mmaxpos = Monitored::Scalar<int>("Mmaxpos",-1);
  auto Mpartition = Monitored::Scalar<int>("Mpartition",-1);
  auto Msampos = Monitored::Scalar<int>("Msampos",-1);
  auto MADC = Monitored::Scalar<int>("MADC",-1);
  auto MADC_0 = Monitored::Scalar<int>("MADC_0",-1);
  auto MSCeT = Monitored::Scalar<float>("MSCeT",0.0);
  auto MSCphi = Monitored::Scalar<float>("MSCphi",0.0);
  auto MSCeta = Monitored::Scalar<float>("MSCeta",0.0);
  auto MSCsatur = Monitored::Scalar<float>("MSCsatur",0.0);
  auto MlatomeSourceIdBIN = Monitored::Scalar<int>("MlatomeSourceIdBIN",1);
  auto MlatomeSourceIdBIN_subdet = Monitored::Scalar<int>("MlatomeSourceIdBIN_subdet",1);


  /**EventID is a part of EventInfo, search event informations:*/
  SG::ReadHandle<xAOD::EventInfo> thisEvent{m_eventInfoKey,ctx};

  const std::vector<unsigned> streamsThisEvent=LArMon::trigStreamMatching(m_streams,thisEvent->streamTags());
  
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;

  SG::ReadHandle<LArDigitContainer> hLArDigitContainer{m_digitContainerKey,ctx}; //"SC"
  SG::ReadHandle<LArRawSCContainer > hSCetContainer{m_rawSCContainerKey,ctx}; //"SC_ET"

  ATH_MSG_DEBUG("hLArDigitContainer.isValid() " << hLArDigitContainer.isValid());
  ATH_MSG_DEBUG("hSCetContainer.isValid() " << hSCetContainer.isValid());

  const LArDigitContainer*  pLArDigitContainer= nullptr;
  const LArRawSCContainer*  pSCetContainer= nullptr;
  StatusCode sc=evtStore()->retrieve(pLArDigitContainer,"SC");
  sc=evtStore()->retrieve(pSCetContainer,"SC_ET");

  ATH_MSG_DEBUG(" pLArDigitContainer: " << pLArDigitContainer <<  " pSCetContainer: " << pSCetContainer );
  ATH_MSG_DEBUG(" pLArDigitContainer->size(): " << pLArDigitContainer->size() <<  " pSCetContainer->size(): " << pSCetContainer->size() );

  if (pLArDigitContainer && pLArDigitContainer->size()>0 && hLArDigitContainer.isValid()){

    /** Define iterators to loop over Digits containers*/
    LArDigitContainer::const_iterator itDig = hLArDigitContainer->begin(); 
    LArDigitContainer::const_iterator itDig_e= hLArDigitContainer->end(); 
    const LArDigit* pLArDigit;

    std::string layer;

    /** Loop over digits*/
    for ( ; itDig!=itDig_e;++itDig) {
      pLArDigit = *itDig;
      unsigned int trueNSamples = pLArDigit->nsamples();
      MNsamples = trueNSamples;
      
      fill(m_scMonGroupName, MNsamples);
      
      HWIdentifier id = pLArDigit->hardwareID(); //gives online ID
      // will be used in next iteration
      //int channelHWID = m_LArOnlineIDHelper->channel(id);
      
      const Identifier offlineID=cabling->cnvToIdentifier(id);
      const CaloSuperCellDetDescrManager* ddman = nullptr;
      ATH_CHECK( detStore()->retrieve (ddman) );

      const CaloDetDescrElement* caloDetElement = ddman->get_element(offlineID);
      
      if(caloDetElement == 0 ){
	ATH_MSG_ERROR( "Cannot retrieve caloDetElement" );
        continue;
      } 
      
      float etaSC = caloDetElement->eta_raw();
      float phiSC = caloDetElement->phi_raw();
      
      int calosample=caloDetElement->getSampling();
      unsigned iLyrNS=m_caloSamplingToLyrNS.at(calosample);

      const unsigned whichSide=(etaSC>0) ? 0 : 1; //Value >0 means A-side 
      unsigned iLyr=iLyrNS*2+whichSide;
      auto layerName=m_layerNames[iLyr];
      
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
	MSCChannel = scdigi->Channel();
	fill(m_scMonGroupName, MSCChannel);
	
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
      
      /** max sample per layer*/
      if(layerName.find("P")!= std::string::npos || layerName.find("0")!= std::string::npos){  
        layer = "0";
      }
      else if(layerName.find("1")!= std::string::npos){ 
	layer = "1";
      }
      else if(layerName.find("2")!= std::string::npos){ 
	layer = "2";
      }
      else{  
	layer = "3";
      }

      auto MlatomeSourceIdBIN_layer = Monitored::Scalar<float>("MlatomeSourceIdBIN_"+layer,MlatomeSourceIdBIN);
      auto Mmaxpos_layer = Monitored::Scalar<float>("Mmaxpos_"+layer,Mmaxpos);
      auto eta_digi = Monitored::Scalar<float>("eta_digi_"+layerName,etaSC);
      auto phi_digi = Monitored::Scalar<float>("phi_digi_"+layerName,phiSC);
      auto badbit_eta_digi = Monitored::Scalar<float>("badQualBit_eta_"+layerName,etaSC);
      auto badbit_phi_digi = Monitored::Scalar<float>("badQualBit_phi_"+layerName,phiSC);
      
      bool isBadQualityBit=false;

      //samples
      for(unsigned i=0; i<trueNSamples;++i) {
	Msampos=i+1;
	MADC=pLArDigit->samples().at(i);
	
	if(i==0)
          MADC_0=pLArDigit->samples().at(i);
	
	fill(m_scMonGroupName, Msampos, MADC, MlatomeSourceIdBIN);

	if(MADC==-1){
	  isBadQualityBit=true;
	  fill(m_scMonGroupName, Msampos, MlatomeSourceIdBIN);
	}
      }

      /** bad quality bit coverage plot*/
      if(isBadQualityBit) 
	fill(m_scMonGroupName, badbit_eta_digi, badbit_phi_digi);

      /** max sample vs latome per layer*/
      fill(m_scMonGroupName, Mmaxpos_layer, Mpartition, MlatomeSourceIdBIN_layer);

      /** max sample vs latome per layer if adc[i] - adc[0] > 15*/
      if(pLArDigit->samples().at(Mmaxpos-1) - MADC_0 > 15) {
	auto MlatomeSourceIdBIN_layer_cut = Monitored::Scalar<float>("MlatomeSourceIdBIN_"+layer+"_cut",MlatomeSourceIdBIN);
	auto Mmaxpos_layer_cut = Monitored::Scalar<float>("Mmaxpos_"+layer+"_cut",Mmaxpos);
	fill(m_scMonGroupName, Mmaxpos_layer_cut, Mpartition, MlatomeSourceIdBIN_layer_cut);
	fill(m_scMonGroupName, eta_digi, phi_digi);
      }

      
    }/** End of loop on LArDigit*/
    
    
  } // pLArDigitContainer

  if (pSCetContainer && pSCetContainer->size()>0 && hSCetContainer.isValid()){

    LArRawSCContainer::const_iterator itSC = hSCetContainer->begin();
    LArRawSCContainer::const_iterator itSC_e = hSCetContainer->end();
    const LArRawSC* rawSC = 0;
    
    /** Loop over SCs*/
    for ( ; itSC!=itSC_e;++itSC) {
      rawSC = *itSC;
      MSCChannel = rawSC->chan();
      
      if (rawSC->energies().size()>0){
	int energy  = rawSC->energies().at(0);
	if (rawSC->energies().size()>1)
	  energy  = rawSC->energies().at(1);

	if ( energy & (1<<17)) energy=energy-std::pow(2,18);

	MSCeT = energy;
      }
      
      if (rawSC->satur().size()>0){
	MSCsatur = rawSC->satur().at(0);      
      }
      
      MlatomeSourceIdBIN=getXbinFromSourceID(rawSC->SourceId());  
      
      fill(m_scMonGroupName, MSCChannel, MSCeT, MSCsatur, MlatomeSourceIdBIN);
      
      if (MSCChannel<10){
	ATH_MSG_DEBUG("--- from  etcontainer MSCeT  = " << MSCeT);
	ATH_MSG_DEBUG("    |______ --------- MSCChannel = "<< MSCChannel);
	ATH_MSG_DEBUG("    |______ --------- MSCsatur = "<< MSCsatur);
	ATH_MSG_DEBUG("    |______ --------- MlatomeSourceIdBIN = "<< MlatomeSourceIdBIN);
	ATH_MSG_DEBUG("    |______ --------- rawSC->SourceId() = "<< rawSC->SourceId());
      }

      ////////////////// make coverage plots
      HWIdentifier id = rawSC->hardwareID(); //gives online ID
      //      /**skip disconnected channels:*/ ?
      //      if(!cabling->isOnlineConnected(id)) continue;

      const Identifier offlineID=cabling->cnvToIdentifier(id); //converts online to offline ID
      Identifier32 Ofl32 =offlineID.get_identifier32();
      Identifier32 Onl32 =id.get_identifier32();
	

      //        m_SCID_helper
      //https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/LArCalorimeter/LArMonTools/src/LArCoverage.cxx
      // Get Physical Coordinates
      const CaloSuperCellDetDescrManager* ddman = nullptr;
      
      ATH_CHECK( detStore()->retrieve (ddman) );

      float etaSC = 0; float phiSC = 0.;
      const CaloDetDescrElement* caloDetElement = ddman->get_element(offlineID);
      if(caloDetElement == 0 ){
	
	ATH_MSG_ERROR( "Cannot retrieve (eta,phi) coordinates for raw channels" );
	ATH_MSG_ERROR( "  ==============> etaSC, phiSC: " << etaSC << " ," << phiSC << std::hex << ";  offlineID = "<< offlineID << "; Ofl32 compact= "<< Ofl32.get_compact()<< "; online ID =" << id << "; Onl32 = " << Onl32.get_compact()  << "; rawSC->SourceId() = " << rawSC->SourceId());
	continue; 
      }
      etaSC = caloDetElement->eta_raw();
      phiSC = caloDetElement->phi_raw();
      
      int calosample=caloDetElement->getSampling();
      unsigned iLyrNS=m_caloSamplingToLyrNS.at(calosample);
      
      const unsigned side=(etaSC>0) ? 0 : 1; //Value >0 means A-side                                                                                              
      unsigned iLyr=iLyrNS*2+side;
      
      MSCeta = etaSC; MSCphi = phiSC;
      
      auto layerName=m_layerNames[iLyr];

      auto LMSCphi = Monitored::Scalar<float>("superCellPhi_"+layerName,phiSC);
      auto LMSCeta = Monitored::Scalar<float>("superCellEta_"+layerName,etaSC);
      auto LMSCet = Monitored::Scalar<float>("superCellEt_"+layerName,MSCeT);

      fill(m_scMonGroupName, MSCeta, MSCphi);
      fill(m_scMonGroupName, LMSCeta, LMSCphi);
      fill(m_scMonGroupName, LMSCeta, LMSCphi, LMSCet);

      if (MSCeT>1000) { //make some conditions for filling
	auto LMSCphi_EtCut = Monitored::Scalar<float>("superCellPhi_EtCut_"+layerName,phiSC);
	auto LMSCeta_EtCut = Monitored::Scalar<float>("superCellEta_EtCut_"+layerName,etaSC);
	auto LMSCet_EtCut = Monitored::Scalar<float>("superCellEt_EtCut_"+layerName,MSCeT);

	fill(m_scMonGroupName, LMSCeta_EtCut, LMSCphi_EtCut);
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
  } else if (m_LArOnlineIDHelper->isEMECchannel(id)) {
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

  std::stringstream ss2;
  std::string phiBin = sourceIDhex.substr(sourceIDhex.size()-1);
  ss2 << phiBin;
  int value;
  ss2 >> std::hex >> value; //value is in hex

  binx = detStartingBin + value;
  if (binx>m_NLatomeBins){
    ATH_MSG_WARNING("something wrong with binning, filling overflowbin");
    binx=m_NLatomeBins;
  }

  return binx;
}
