/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TileEvent/MBTSCollisionTime.h"

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
  m_BCMTimeDiffCol_Cut(0),
  m_BCMTimeDiffHalo_CutLo(0),
  m_BCMHiGainCut(0),
  m_BCMLowGainCut(0),
  m_MBTSBeamVeto_MultiplicityCut(0),
  m_MBTSBeamVeto_TimeCut(0),
  m_MBTSBeamVeto_ThresholdCut(0),
  m_MBTSmask(TileCell::MASK_BADCH | TileCell::MASK_OVER | TileCell::MASK_TIME),
  m_MBTSpattern(TileCell::MASK_TIME), 
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

  declareProperty("BCMTimeDiffCol_Cut",m_BCMTimeDiffCol_Cut=6.);
  declareProperty("BCMTimeDiffHalo_CutLo",m_BCMTimeDiffHalo_CutLo=6.);
  declareProperty("BCMLowGainCut",m_BCMLowGainCut=1);
  declareProperty("BCMHiGainCut",m_BCMHiGainCut=1);

  declareProperty("MBTSBeamVeto_MultiplicityCut",m_MBTSBeamVeto_MultiplicityCut=0); // >0
  declareProperty("MBTSBeamVeto_TimeCut",m_MBTSBeamVeto_TimeCut=15.);
  declareProperty("MBTSBeamVeto_ThresholdCut",m_MBTSBeamVeto_ThresholdCut=40.0/222.0);
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

  //initialise the read handle keys 
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_beamBackgroundDataKey.initialize(!m_isMC));
  ATH_CHECK(m_LUCID_rawDataContainerKey.initialize(!m_isMC));
  ATH_CHECK(m_bcmCollisionTimeKey.initialize());
  ATH_CHECK( m_sctSpacePointKey.initialize(!m_isMC) );
  ATH_CHECK( m_pixSpacePointKey.initialize(!m_isMC) );
  ATH_CHECK(m_tileCellContainerKey.initialize());
  ATH_CHECK(m_lArCollisionTimeKey.initialize(!m_isMC));
  ATH_CHECK(m_eventInfoDecorKey.initialize());
  
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
  if (!m_isMC) { // do not request in MC
    SG::ReadHandle<BeamBackgroundData> beamBackgroundDataReadHandle(m_beamBackgroundDataKey);
    
    if(!beamBackgroundDataReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to BeamBackgoundData with name: " << m_beamBackgroundDataKey.key());
    else{
      if( beamBackgroundDataReadHandle->GetNumSegment() > m_HaloNumSegment_Cut ){
	if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloMuonSegment)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloMuonSegment]);
	else m_bitcntvec[EventInfo::HaloMuonSegment]++;
      }
      if( beamBackgroundDataReadHandle->GetNumClusterShape() > m_HaloNumClusterShape_Cut ){
	if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloClusterShape)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloClusterShape]);
	else m_bitcntvec[EventInfo::HaloClusterShape]++; 
      }
      if( beamBackgroundDataReadHandle->GetNumNoTimeLoose() > m_HaloNumOneSidedLoose_Cut ){
	if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloMuonOneSided)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloMuonOneSided]);
	else m_bitcntvec[EventInfo::HaloMuonOneSided]++;
      }
      if( beamBackgroundDataReadHandle->GetNumTwoSidedNoTime() > m_HaloNumTwoSided_Cut ){
	if( eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::HaloMuonTwoSided)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::HaloMuonTwoSided]);
	else m_bitcntvec[EventInfo::HaloMuonTwoSided]++;
      }
    }
  }
    
  ///////////////////////////////////////////////////
  // LUCID: LUCIDBeamVeto
  //////////////////////////////////////////////////

  if (!m_isMC) { // do not request in MC
    SG::ReadHandle<LUCID_RawDataContainer> LUCID_rawDataContainerReadHandle(m_LUCID_rawDataContainerKey);
    
    if (!LUCID_rawDataContainerReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to LUCID_RawDataContainer with name: " << m_LUCID_rawDataContainerKey.key());
    else{
      int LUCIDcounter(0);
      for (auto LUCID_rawData : *LUCID_rawDataContainerReadHandle){
	LUCIDcounter+=LUCID_rawData->getNhitsPMTsideA();
	LUCIDcounter+=LUCID_rawData->getNhitsPMTsideC();
      }
      if ( LUCIDcounter>m_LUCIDBeamVeto_Cut ){
	if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::LUCIDBeamVeto)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::LUCIDBeamVeto]);
	else m_bitcntvec[EventInfo::LUCIDBeamVeto]++;
      }
    }
  }
    
  ///////////////////////////////////////////////////
  // BCM: BCMTimeDiffHalo, BCMTimeDiffCol, BCMBeamVeto
  //////////////////////////////////////////////////
  
  SG::ReadHandle<BcmCollisionTime> bcmCollisionTimeReadHandle(m_bcmCollisionTimeKey);

  if (!bcmCollisionTimeReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to BcmCollisionTime with name: " << m_bcmCollisionTimeKey.key());
  else{
    std::vector<float> bcmTDs = bcmCollisionTimeReadHandle->getDeltaT();
    ATH_MSG_DEBUG( " got BCMCollisionTime object getMultiLG "<<bcmCollisionTimeReadHandle->getMultiLG()<<" getMultiHG "<<bcmCollisionTimeReadHandle->getMultiHG()<<" TDs size "<<bcmTDs.size());
    float minTD=999;
    float maxTD=0;

    for(unsigned int i=0; i < bcmTDs.size(); i++){
      float td = bcmTDs.at(i);
      ATH_MSG_DEBUG( " BCMCollisionTime td "<<i<<" "<<td);
      if (fabs(td)>fabs(maxTD)) maxTD=td;
      if (fabs(td)<fabs(minTD)) minTD=td;
    }

    ATH_MSG_DEBUG(" BCMCollisionTime minDT "<<minTD<<" maxDT "<<maxTD);

    if (fabs(minTD)<  m_BCMTimeDiffCol_Cut) {
      if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::BCMTimeDiffCol)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::BCMTimeDiffCol]);
      else m_bitcntvec[EventInfo::BCMTimeDiffCol]++;
    }
    if (fabs(maxTD)>m_BCMTimeDiffHalo_CutLo){
      if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::BCMTimeDiffHalo)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::BCMTimeDiffHalo]);
      else m_bitcntvec[EventInfo::BCMTimeDiffHalo]++;
    }
    if (bcmCollisionTimeReadHandle->getMultiLG()>m_BCMLowGainCut || bcmCollisionTimeReadHandle->getMultiHG()>m_BCMHiGainCut){
      if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::BCMBeamVeto)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::BCMBeamVeto]);
      else  m_bitcntvec[EventInfo::BCMBeamVeto]++;
    }  
  }
    
  ////////////////////////////////////////////////////////////////////////////////
  // ID SP multiplicities from Raw for filling:  IDMultiplicityHuge, IDSPNonEmpty
  ///////////////////////////////////////////////////////////////////////////////
  if (!m_isMC) { // do not request in MC
    SG::ReadHandle<SpacePointContainer> sctSP{m_sctSpacePointKey};
    SG::ReadHandle<SpacePointContainer> pixSP{m_pixSpacePointKey};

    
    if (!sctSP.isValid() or !pixSP.isValid()) {
      ATH_MSG_WARNING("Invalid ReadHandle to SCT/Pix spacepoints");
    }
    else{
      int NSCTsps = 0;
      int NPIXsps = 0;
      std::for_each(sctSP->begin(),sctSP->end(),[&NSCTsps](const auto coll){if (coll) NSCTsps+=coll->size();});
      std::for_each(pixSP->begin(),pixSP->end(),[&NPIXsps](const auto coll){if (coll) NPIXsps+=coll->size();});
      

      // set IDMultiplicityHuge
      if ( (NPIXsps)>m_PixMultiplicityHuge_Cut){
	if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::PixMultiplicityHuge)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::PixMultiplicityHuge]);
	else m_bitcntvec[EventInfo::PixMultiplicityHuge]++;
      }
      
      // set PixSPNonEmpty
      if ( (NPIXsps)>m_PixSPNonEmpty_Cut ){
	if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::PixSPNonEmpty)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::PixSPNonEmpty]);
	else m_bitcntvec[EventInfo::PixSPNonEmpty]++;
      }
      
      if ( (NSCTsps)>m_SCTMultiplicityHuge_Cut){
	if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::SCTMultiplicityHuge)==false)	ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::SCTMultiplicityHuge]);
	else m_bitcntvec[EventInfo::SCTMultiplicityHuge]++;
      }
      
      // set SCTSPNonEmpty
      if ( (NSCTsps)>m_SCTSPNonEmpty_Cut ){
	if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::SCTSPNonEmpty)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::SCTSPNonEmpty]);
	else m_bitcntvec[EventInfo::SCTSPNonEmpty]++;
      }
    }
  }
  
  ////////////////
  // MBTSBeamVeto
  ///////////////

  SG::ReadHandle<TileCellContainer> tileCellContainerReadHandle(m_tileCellContainerKey);

  if (!tileCellContainerReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to TileCellContainer: " << m_tileCellContainerKey.key());
  else{
    int MBTScount(0);

    for (auto tileCellContainer : *tileCellContainerReadHandle){
      if (tileCellContainer->energy() < m_MBTSBeamVeto_ThresholdCut) continue;
      const uint8_t qbit1 = tileCellContainer->qbit1();

      if ((qbit1 & m_MBTSmask) != m_MBTSpattern) {
	ATH_MSG_DEBUG("Rejected based on quality bits");
	continue;
      }
      if (fabs(tileCellContainer->time())<m_MBTSBeamVeto_TimeCut) MBTScount++;
    }//loop on MBTS containers

    if (MBTScount>m_MBTSBeamVeto_MultiplicityCut){
      if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::MBTSBeamVeto)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::MBTSBeamVeto]);
      else  m_bitcntvec[EventInfo::MBTSBeamVeto]++;
    }
  }
    
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // LAr EC collision timing stuff (from Guillaume...) - for filling:      LArECTimeDiffHalo, LArECTimeDiffCol
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (!m_isMC) { // do not request in MC
    SG::ReadHandle<LArCollisionTime> lArCollisionTimeReadHandle(m_lArCollisionTimeKey);
    
    if (!lArCollisionTimeReadHandle.isValid()) ATH_MSG_WARNING("Invalid ReadHandle to LArCollisionTime: " << m_lArCollisionTimeKey.key());
    else{
      if (lArCollisionTimeReadHandle->ncellA() > m_LArEC_SideCut && lArCollisionTimeReadHandle->ncellC() > m_LArEC_SideCut) {
	float LArECtimeDiff =   lArCollisionTimeReadHandle->timeA()-lArCollisionTimeReadHandle->timeC();
	if (fabs(LArECtimeDiff)<m_LArECTimeDiffCol_Cut){
	  if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::LArECTimeDiffCol)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " <<  m_bitnamevec[EventInfo::LArECTimeDiffCol]);
	  else m_bitcntvec[EventInfo::LArECTimeDiffCol]++;
	}
	if (fabs(LArECtimeDiff)>m_LArECTimeDiffHalo_CutLo && fabs(LArECtimeDiff)<m_LArECTimeDiffHalo_CutHi){ 
	  if (eventInfoReadHandle->updateEventFlagBit(EventInfo::Background,EventInfo::LArECTimeDiffHalo)==false) ATH_MSG_WARNING("Failed to set EventInfo Background word bit " << m_bitnamevec[EventInfo::LArECTimeDiffHalo]);
	  else m_bitcntvec[EventInfo::LArECTimeDiffHalo]++;
	}// halo timing
      } // enough hits per side
    }
  }
  
  ////////////////////////////////////////
  // printout the final background word
  ///////////////////////////////////////
  ATH_MSG_DEBUG("Summary of background word contents:");
  ATH_MSG_DEBUG("MBTSTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffHalo));
  ATH_MSG_DEBUG("MBTSTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSTimeDiffCol));
  ATH_MSG_DEBUG("MBTSBeamVeto: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MBTSBeamVeto));
  ATH_MSG_DEBUG("LArECTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffHalo));
  ATH_MSG_DEBUG("LArECTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::LArECTimeDiffCol));
  ATH_MSG_DEBUG("PixMultiplicityHuge: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::PixMultiplicityHuge));
  ATH_MSG_DEBUG("PixSPNonEmpty: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::PixSPNonEmpty));
  ATH_MSG_DEBUG("SCTMultiplicityHuge: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTMultiplicityHuge));
  ATH_MSG_DEBUG("SCTSPNonEmpty: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::SCTSPNonEmpty));
  ATH_MSG_DEBUG("CSCTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffHalo));
  ATH_MSG_DEBUG("CSCTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::CSCTimeDiffCol));
  ATH_MSG_DEBUG("BCMTimeDiffHalo: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffHalo));
  ATH_MSG_DEBUG("BCMTimeDiffCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMTimeDiffCol));
  ATH_MSG_DEBUG("BCMBeamVeto: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::BCMBeamVeto));
  ATH_MSG_DEBUG("MuonTimingCol: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCol));
  ATH_MSG_DEBUG("MuonTimingCosmic: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::MuonTimingCosmic));
  ATH_MSG_DEBUG("LUCIDBeamVeto: "<<eventInfoReadHandle->isEventFlagBitSet(EventInfo::Background,EventInfo::LUCIDBeamVeto));
  
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------

StatusCode BackgroundWordFiller::finalize() {

  ATH_MSG_INFO("In finalize()");
  
  ATH_MSG_INFO("Job Summary for Background Word (NB. MBTS time bits not counted by this alg)");
  for(int ibit=0;ibit<EventInfo::NBackgroundWords;++ibit) ATH_MSG_INFO(" " << m_bitnamevec[ibit] << ": " <<  m_bitcntvec[ibit] << "/" << m_totalcnt << " events");
  
  return StatusCode::SUCCESS;
}

