/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Algorihtm to fill the background word that is stored in the EventInfo 

background word contains:
=========================
MBTSTimeDiffHalo, MBTSTimeDiffCol,  // these bits are set in MBTSTimeDiffEventInfoAlg due to ordering reasons....
MBTSBeamVeto
LArECTimeDiffHalo, LArECTimeDiffCol 
PixMultiplicityHuge, PixSPNonEmpty,
SCTMultiplicityHuge, SCTSPNonEmpty
CSCTimeDiffHalo, CSCTimeDiffCol
BCMTimeDiffHalo, BCMTimeDiffCol, BCMBeamVeto
MuonTimingCol, MuonTimingCosmic,
LUCIDBeamVeto

Jamie Boyd 7/2/2010

added for 2012 in addition to above:
====================================
HaloMuonSegment, HaloClusterShape,
HaloMuonOneSided, HaloMuonTwoSided,
HaloTileClusterPattern,
BeamGasPixel,
CosmicStandAlone, CosmicStandAloneTight,
CosmicCombined, CosmicCombinedTight,
BkgdResvBit1, BkgdResvBit2,
BkgdResvBit3, BkgdResvBit4,
BkgdResvBit5,

Mark Tibbetts 6/3/2012

*/

#include "RecBackgroundAlgs/BackgroundWordFiller.h"
#include "RecBackgroundEvent/BeamBackgroundData.h"
#include "TagEvent/RawInfoSummaryForTag.h"
#include "TileEvent/MBTSCollisionTime.h"
#include "LArRecEvent/LArCollisionTime.h"
#include "TileEvent/TileContainer.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "BCM_CollisionTime/BcmCollisionTime.h"

//----------------------------------------------------------------

using xAOD::EventInfo;

BackgroundWordFiller::BackgroundWordFiller(const std::string& name,
                                 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_HaloNumSegment_Cut(0),
  m_HaloNumClusterShape_Cut(0),
  m_HaloNumOneSidedLoose_Cut(0),
  m_HaloNumTwoSided_Cut(0),
  m_PixMultiplicityHuge_Cut(0),
  m_PixSPNonEmpty_Cut(0),
  m_SCTMultiplicityHuge_Cut(0),
  m_SCTSPNonEmpty_Cut(0),
  m_LUCIDBeamVeto_Cut(0),
  m_MBTS_SideCut(0),
  m_BCMTimeDiffCol_Cut(0),
  m_BCMTimeDiffHalo_CutLo(0),
  m_BCMHiGainCut(0),
  m_BCMLowGainCut(0),
  m_MBTSBeamVeto_MultiplicityCut(0),
  m_MBTSBeamVeto_TimeCut(0),
  m_MBTSBeamVeto_ThresholdCut(0),
  m_MBTSmask(TileCell::MASK_BADCH | TileCell::MASK_OVER | TileCell::MASK_TIME),
  m_MBTSpattern(TileCell::MASK_TIME), 
  m_mbtsContainerName(""),
  m_LArEC_SideCut(0),
  m_LArECTimeDiffCol_Cut(0),
  m_LArECTimeDiffHalo_CutLo(0),
  m_LArECTimeDiffHalo_CutHi(0),
  m_totalcnt(0),
  m_bitnamevec{{"MBTSTimeDiffHalo", "MBTSTimeDiffCol",
		 "LArECTimeDiffHalo", "LArECTimeDiffCol", 
		 "PixMultiplicityHuge", "PixSPNonEmpty",
		 "SCTMultiplicityHuge", "SCTSPNonEmpty",
		 "CSCTimeDiffHalo", "CSCTimeDiffCol",
		 "BCMTimeDiffHalo", "BCMTimeDiffCol", 
		 "MuonTimingCol", "MuonTimingCosmic",
		 "MBTSBeamVeto",
		 "BCMBeamVeto",
		 "LUCIDBeamVeto",
		 "HaloMuonSegment", "HaloClusterShape",
		 "HaloMuonOneSided", "HaloMuonTwoSided",
		 "HaloTileClusterPattern",
		 "BeamGasPixel",
		 "CosmicStandAlone", "CosmicStandAloneTight",
		 "CosmicCombined", "CosmicCombinedTight",
		 "BkgdResvBit1", "BkgdResvBit2",
		 "BkgdResvBit3", "BkgdResvBit4",
		 "BkgdResvBit5"}}

{
  // ID
  declareProperty("HaloNumSegment_Cut",m_HaloNumSegment_Cut=0);
  declareProperty("HaloNumClusterShape_Cut",m_HaloNumClusterShape_Cut=0);
  declareProperty("HaloNumOneSidedLoose_Cut",m_HaloNumOneSidedLoose_Cut=0);
  declareProperty("HaloNumTwoSided_Cut",m_HaloNumTwoSided_Cut=0);
  declareProperty("PixMultiplicityHuge_Cut",m_PixMultiplicityHuge_Cut=100000);
  declareProperty("PixSPNonEmpty_Cut",m_PixSPNonEmpty_Cut=10);
  declareProperty("SCTMultiplicityHuge_Cut",m_SCTMultiplicityHuge_Cut=100000);
  declareProperty("SCTSPNonEmpty_Cut",m_SCTSPNonEmpty_Cut=10);

  // LUCID
  declareProperty("LUCIDBeamVeto_Cut",m_LUCIDBeamVeto_Cut=0); // >0
  // MBTS
  declareProperty("MBTS_SideCut",m_MBTS_SideCut=4);

  declareProperty("BCMTimeDiffCol_Cut",m_BCMTimeDiffCol_Cut=6.);
  declareProperty("BCMTimeDiffHalo_CutLo",m_BCMTimeDiffHalo_CutLo=6.);
  declareProperty("BCMLowGainCut",m_BCMLowGainCut=1);
  declareProperty("BCMHiGainCut",m_BCMHiGainCut=1);

  declareProperty("MBTSBeamVeto_MultiplicityCut",m_MBTSBeamVeto_MultiplicityCut=0); // >0
  declareProperty("MBTSBeamVeto_TimeCut",m_MBTSBeamVeto_TimeCut=15.);
  declareProperty("MBTSBeamVeto_ThresholdCut",m_MBTSBeamVeto_ThresholdCut=40.0/222.0);
  declareProperty("MBTSContainer",m_mbtsContainerName="MBTSContainer");
  // LAr
  declareProperty("LArEC_SideCut",m_LArEC_SideCut=1);
  declareProperty("LArECTimeDiffCol_Cut",m_LArECTimeDiffCol_Cut=10.);
  declareProperty("LArECTimeDiffHalo_CutLo",m_LArECTimeDiffHalo_CutLo=10.);
  declareProperty("LArECTimeDiffHalo_CutHi",m_LArECTimeDiffHalo_CutHi=30.);


  //Reset counter-array
  m_bitcntvec.fill(0);
}

//----------------------------------------------------------------

BackgroundWordFiller::~BackgroundWordFiller()
{ 
}


//----------------------------------------------------------------

StatusCode BackgroundWordFiller::initialize() {

  //initialise the read handle key to the EventInfo object
  ATH_CHECK( m_eventInfoKey.initialize() );
  
  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------

StatusCode BackgroundWordFiller::execute() {

   ///////////////////////// 
   // get the EventInfo
   /////////////////////////

  SG::ReadHandle<xAOD::EventInfo> eventInfoReadHandle(m_eventInfoKey); 
  
   
  m_totalcnt++;

   ///////////////////////
   // NOW SET THE BITS!!
   ///////////////////////

    ///////////////////////////////////////////////////
   // Halo Identification
   //////////////////////////////////////////////////  

   if( evtStore()->contains<BeamBackgroundData>("BeamBackgroundData") ){

     const BeamBackgroundData* beamBackgroundData;

     if (evtStore()->retrieve(beamBackgroundData, "BeamBackgroundData").isFailure() )
       msg(MSG::WARNING) << "  Could not retrieve BeamBackgroundData." << endmsg;
     else {     
       if( beamBackgroundData->GetNumSegment() > m_HaloNumSegment_Cut ){
	 if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloMuonSegment)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloMuonSegment] << endmsg;
	 else
	   m_bitcntvec[EventInfo::HaloMuonSegment]++;
       }
       if( beamBackgroundData->GetNumClusterShape() > m_HaloNumClusterShape_Cut ){
	 if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloClusterShape)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloClusterShape] << endmsg;
	 else
	   m_bitcntvec[EventInfo::HaloClusterShape]++; 
       }
       if( beamBackgroundData->GetNumNoTimeLoose() > m_HaloNumOneSidedLoose_Cut ){
	 if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloMuonOneSided)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloMuonOneSided] << endmsg;
	 else
	   m_bitcntvec[EventInfo::HaloMuonOneSided]++;
       }
       if( beamBackgroundData->GetNumTwoSidedNoTime() > m_HaloNumTwoSided_Cut ){
	 if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloMuonTwoSided)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloMuonTwoSided] << endmsg;
	 else
	   m_bitcntvec[EventInfo::HaloMuonTwoSided]++;
       }
     }// get BeamBackgroudData
       
   }// beambackground SG veto

   ///////////////////////////////////////////////////
   // LUCID: LUCIDBeamVeto
   //////////////////////////////////////////////////

   if( evtStore()->contains<LUCID_RawDataContainer>("Lucid_RawData") ){ 

     const LUCID_RawDataContainer* LUCID_RawDataContainer;
     if (evtStore()->retrieve(LUCID_RawDataContainer, "Lucid_RawData").isFailure() ) {
       msg(MSG::WARNING) << "  Could not retrieve Lucid_RawData" << endmsg;
     }
     else {
       LUCID_RawDataContainer::const_iterator LUCID_RawData_itr = LUCID_RawDataContainer->begin();
       LUCID_RawDataContainer::const_iterator LUCID_RawData_end = LUCID_RawDataContainer->end();
       int LUCIDcounter(0);
       for (; LUCID_RawData_itr != LUCID_RawData_end; LUCID_RawData_itr++) {
	 LUCIDcounter+=(*LUCID_RawData_itr)->getNhitsPMTsideA();
	 LUCIDcounter+=(*LUCID_RawData_itr)->getNhitsPMTsideC();
       }
       if ( LUCIDcounter>m_LUCIDBeamVeto_Cut ){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::LUCIDBeamVeto)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::LUCIDBeamVeto] << endmsg;
	 else
	   m_bitcntvec[EventInfo::LUCIDBeamVeto]++;
       }
     }// retrieved LUCID stuff
   }// LUCID SG veto

   ///////////////////////////////////////////////////
   // BCM: BCMTimeDiffHalo, BCMTimeDiffCol, BCMBeamVeto
   //////////////////////////////////////////////////

   if( evtStore()->contains<BcmCollisionTime>("BcmCollisionTime") ){
     
     const BcmCollisionTime* BCMct;
     if (evtStore()->retrieve(BCMct).isFailure() ) {
       msg(MSG::WARNING) << "  Could not retrieve BCMCollisionTime" << endmsg;
     }
     else {
       std::vector<float> bcmTDs = BCMct->getDeltaT();
       ATH_MSG_DEBUG( " got BCMCollisionTime object getMultiLG "<<BCMct->getMultiLG()<<" getMultiHG "<<BCMct->getMultiHG()<<" TDs size "<<bcmTDs.size());
       float minTD=999;
       float maxTD=0;
       for(unsigned int i=0; i < bcmTDs.size(); i++){
	 float td = bcmTDs.at(i);
	 ATH_MSG_DEBUG( " BCMCollisionTime td "<<i<<" "<<td);
	 if (fabs(td)>fabs(maxTD)) maxTD=td;
	 if (fabs(td)<fabs(minTD)) minTD=td;
       }
       ATH_MSG_DEBUG( " BCMCollisionTime minDT "<<minTD<<" maxDT "<<maxTD);
       // set time diff bits
       if (fabs(minTD)<  m_BCMTimeDiffCol_Cut) {
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::BCMTimeDiffCol)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::BCMTimeDiffCol] << endmsg;
	 else
	   m_bitcntvec[EventInfo::BCMTimeDiffCol]++;
       }
       if (fabs(maxTD)>m_BCMTimeDiffHalo_CutLo){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::BCMTimeDiffHalo)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::BCMTimeDiffHalo] << endmsg;
	 else
	   m_bitcntvec[EventInfo::BCMTimeDiffHalo]++;
       }
       if (BCMct->getMultiLG()>m_BCMLowGainCut || BCMct->getMultiHG()>m_BCMHiGainCut){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::BCMBeamVeto)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::BCMBeamVeto] << endmsg;
	 else
	   m_bitcntvec[EventInfo::BCMBeamVeto]++;
       }  
       
     }
   }//BCM SG veto


   ////////////////////////////////////////////////////////////////////////////////
   // ID SP multiplicities from Raw for filling:  IDMultiplicityHuge, IDSPNonEmpty
   ///////////////////////////////////////////////////////////////////////////////
   if( evtStore()->contains<RawInfoSummaryForTag>("RawInfoSummaryForTag") ){
       

     const RawInfoSummaryForTag* rawinfo=0;

     if ( evtStore()->retrieve( rawinfo ).isFailure()) {
       msg(MSG::WARNING) << "Cannot get raw summary info from storegate" << endmsg;
     }   else {
       int NSCTsps = rawinfo->getNsctSPs();
       int NPIXsps = rawinfo->getNpixSPs();
       //   int NTRTdcs = rawinfo->getNtrtDCs();
       //   int NTRTdcsHT = rawinfo->getNtrtHtDCs();
       
       // set IDMultiplicityHuge
       if ( (NPIXsps)>m_PixMultiplicityHuge_Cut){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::PixMultiplicityHuge)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::PixMultiplicityHuge] << endmsg;
	 else
	   m_bitcntvec[EventInfo::PixMultiplicityHuge]++;
       }
       
       // set PixSPNonEmpty
       if ( (NPIXsps)>m_PixSPNonEmpty_Cut ){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::PixSPNonEmpty)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::PixSPNonEmpty] << endmsg;
	 else
	   m_bitcntvec[EventInfo::PixSPNonEmpty]++;
       }
       
       if ( (NSCTsps)>m_SCTMultiplicityHuge_Cut){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::SCTMultiplicityHuge)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::SCTMultiplicityHuge] << endmsg;
	 else
	   m_bitcntvec[EventInfo::SCTMultiplicityHuge]++;
       }
       
       // set SCTSPNonEmpty
       if ( (NSCTsps)>m_SCTSPNonEmpty_Cut ){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::SCTSPNonEmpty)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::SCTSPNonEmpty] << endmsg;
	 else
	   m_bitcntvec[EventInfo::SCTSPNonEmpty]++;
       }

       
     } // retrieved RawInfoSummaryForTag object...
   }//indet SG veto

   /*
   ////////////////////////////////////////////////////////////////////////////////////////
   // MBTS Time Diff stuff - for filling:  MBTSTimeDiffHalo, MBTSTimeDiffCol, MBTSBeamVeto
   ////////////////////////////////////////////////////////////////////////////////////////
   const MBTSCollisionTime * mbtsTime;
   sc = evtStore()->retrieve(mbtsTime,"MBTSCollisionTime");
   if (!sc.isFailure()) {
     if (mbtsTime->ncellA()>m_MBTS_SideCut && mbtsTime->ncellC()>m_MBTS_SideCut){
       if (fabs(mbtsTime->time())<m_MBTSTimeDiffCol_Cut ){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::MBTSTimeDiffCol)==false) {
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word!" << endmsg;
	 }
       }
       if (fabs(mbtsTime->time())<m_MBTSTimeDiffHalo_CutHi && fabs(mbtsTime->time())>m_MBTSTimeDiffHalo_CutLo ){ 
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::MBTSTimeDiffHalo)==false) {
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word!" << endmsg;
	 }
       } // halo timing
     } // enough hits per side
   }   else {
     msg(MSG::WARNING) << "Failed to retrieve MBTSCollisionTime object" << endmsg;
   }
   */
   ////////////////
   // MBTSBeamVeto

   if( evtStore()->contains<TileCellContainer>(m_mbtsContainerName) ){

     const TileCellContainer *tileCellCnt = 0;
     int MBTScount(0);
     if (evtStore()->retrieve(tileCellCnt, m_mbtsContainerName).isFailure()) {
       msg(MSG::WARNING) << "Error retrieving " << m_mbtsContainerName << endmsg;
     } else {
       TileCellContainer::const_iterator itr = tileCellCnt->begin();
       TileCellContainer::const_iterator itr_end = tileCellCnt->end();
       for(; itr != itr_end; ++itr) {
	 if ((*itr)->energy()<m_MBTSBeamVeto_ThresholdCut) continue;
	 const uint8_t qbit1=(*itr)->qbit1(); 
	 if ((qbit1 & m_MBTSmask) != m_MBTSpattern) {
	   ATH_MSG_DEBUG("Rejected based on quality bits");
	   continue;
	 }
	 if (fabs((*itr)->time())<m_MBTSBeamVeto_TimeCut) MBTScount++;
       }
       if (MBTScount>m_MBTSBeamVeto_MultiplicityCut){
	 if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::MBTSBeamVeto)==false)
	   msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::MBTSBeamVeto] << endmsg;
	 else
	   m_bitcntvec[EventInfo::MBTSBeamVeto]++;
       }
     } // retrieved MBTS info
   }//MBTS SG veto

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // LAr EC collision timing stuff (from Guillaume...) - for filling:      LArECTimeDiffHalo, LArECTimeDiffCol
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
   
   if( evtStore()->contains<LArCollisionTime>("LArCollisionTime") ){
     const  LArCollisionTime* tps;
     if(evtStore()->retrieve(tps,"LArCollisionTime").isFailure()) {
       msg(MSG::WARNING) << "Failed to retrieve LArCollisionTime object" << endmsg;
     }
     else {
       if (tps->ncellA() > m_LArEC_SideCut && tps->ncellC() > m_LArEC_SideCut) {
	 float LArECtimeDiff =   tps->timeA()-tps->timeC();
	 if (fabs(LArECtimeDiff)<m_LArECTimeDiffCol_Cut){
	   if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::LArECTimeDiffCol)==false)
	     msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " <<  m_bitnamevec[EventInfo::LArECTimeDiffCol] << endmsg;
	   else
	     m_bitcntvec[EventInfo::LArECTimeDiffCol]++;
	 }
	 if (fabs(LArECtimeDiff)>m_LArECTimeDiffHalo_CutLo && fabs(LArECtimeDiff)<m_LArECTimeDiffHalo_CutHi){ 
	   if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::LArECTimeDiffHalo)==false)
	     msg(MSG::WARNING) << "Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::LArECTimeDiffHalo]  << endmsg;
	   else
	     m_bitcntvec[EventInfo::LArECTimeDiffHalo]++;
	 }// halo timing
       } // enough hits per side
     } 
       
   } //LAr time SG veto
   
   ////////////////////////////////////////
   // printout the final background word
   ///////////////////////////////////////
   if (msgLvl(MSG::DEBUG)) {
     msg(MSG::DEBUG) << "Summary of background word contents:"<< endmsg;
     msg(MSG::DEBUG) << "MBTSTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffHalo)<< endmsg;
     msg(MSG::DEBUG) << "MBTSTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffCol)<< endmsg;
     msg(MSG::DEBUG) << "MBTSBeamVeto: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSBeamVeto)<< endmsg;
     msg(MSG::DEBUG) << "LArECTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffHalo)<< endmsg;
     msg(MSG::DEBUG) << "LArECTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffCol)<< endmsg;
     msg(MSG::DEBUG) << "PixMultiplicityHuge: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::PixMultiplicityHuge)<< endmsg;
     msg(MSG::DEBUG) << "PixSPNonEmpty: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::PixSPNonEmpty)<< endmsg;
     msg(MSG::DEBUG) << "SCTMultiplicityHuge: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTMultiplicityHuge)<< endmsg;
     msg(MSG::DEBUG) << "SCTSPNonEmpty: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTSPNonEmpty)<< endmsg;
     msg(MSG::DEBUG) << "CSCTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffHalo)<< endmsg;
     msg(MSG::DEBUG) << "CSCTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffCol)<< endmsg;
     msg(MSG::DEBUG) << "BCMTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffHalo)<< endmsg;
     msg(MSG::DEBUG) << "BCMTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffCol)<< endmsg;
     msg(MSG::DEBUG) << "BCMBeamVeto: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMBeamVeto)<< endmsg;
     msg(MSG::DEBUG) << "MuonTimingCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCol)<< endmsg;
     msg(MSG::DEBUG) << "MuonTimingCosmic: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCosmic)<< endmsg;
     msg(MSG::DEBUG) << "LUCIDBeamVeto: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::LUCIDBeamVeto)<< endmsg;
   }
   return StatusCode::SUCCESS;
}

//----------------------------------------------------------------

StatusCode BackgroundWordFiller::finalize() {


  msg(MSG::INFO) << "In finalize()" << endmsg;
  
  msg( MSG::INFO) << "Job Summary for Background Word (NB. MBTS time bits not counted by this alg)" << endmsg;
  for(int ibit=0;ibit<EventInfo::NBackgroundWords;++ibit)
    msg(MSG::INFO) << " " << m_bitnamevec[ibit] << ": " <<  m_bitcntvec[ibit] << "/" << m_totalcnt << " events" <<endmsg;
  
  return StatusCode::SUCCESS;
  
}

