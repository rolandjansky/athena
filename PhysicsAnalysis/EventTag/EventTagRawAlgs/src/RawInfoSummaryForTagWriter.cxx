/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// code to write the TAG_COMM variables that are only available from 
// RAW into a persistified object for writing to the TAG - Jamie Boyd
/////////////////////////////////////////////////////////////////////////


#include "RawInfoSummaryForTagWriter.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TrkTrackSummary/TrackSummary.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TileIdentifier/TileTBID.h"



//----------------------------------------------------------------

RawInfoSummaryForTagWriter::RawInfoSummaryForTagWriter(const std::string& name,
                                 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator)
{
  declareProperty("IDTrackKey",m_sgKeyIDtrack);
  declareProperty("CaloCellContKey", m_cellContKey="AllCalo");
  declareProperty("CaloClusterKey",m_caloClusterKey="CaloTopoCluster");
  declareProperty("LArCollTimeName", m_larCollTimeName="LArCollisionTime");
  declareProperty("doClusterSums",m_doClusterSums=true);
  declareProperty("TRT_DriftCircleContainerName", m_TRT_DriftCircleName="TRT_DriftCircles");
  declareProperty("SCT_SpacePointName",m_sctSpacePointName="SCT_SpacePoints");
  declareProperty("Pixel_SpacePointName",m_pixSpacePointName="PixelSpacePoints");
  declareProperty("TRT_PhaseName", m_trtPhaseName="");//"TRT_Phase");
  declareProperty("MBTSName",m_mbtsName="MBTSContainer");
  declareProperty("MBTSCollTimeKey",m_MBTSCollTimeKey="MBTSCollisionTime");
  declareProperty("MBTS_Threshold",  m_mbts_threshold = 40.0/222.0 );  // Value in pC
  declareProperty("OutputKey", m_RISFTKey="RawInfoSummaryForTag");
}

//----------------------------------------------------------------

RawInfoSummaryForTagWriter::~RawInfoSummaryForTagWriter()
{ 
}


//----------------------------------------------------------------

StatusCode RawInfoSummaryForTagWriter::initialize()
{

  ATH_MSG_INFO("In initialize()");

  // Retrieve TileTBID helper from det store
  // (The MBTS was added to the Test Beam (TB) list.)
  if(detStore()->retrieve(m_tileTBID).isFailure()) {
    ATH_MSG_ERROR ("Unable to retrieve TileTBID helper from DetectorStore");
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK(m_sgKeyIDtrack.initialize());
  ATH_CHECK(m_cellContKey.initialize());
  if(m_doClusterSums)  {
    ATH_CHECK(m_caloClusterKey.initialize());
  }
  //ATH_CHECK(m_sgKeyTileMuonFitterNew.initialize());
  ATH_CHECK(m_TRT_DriftCircleName.initialize());
  ATH_CHECK(m_TRT_DriftCircleName.initialize());
  ATH_CHECK(m_MBTSCollTimeKey.initialize());
  ATH_CHECK(m_sctSpacePointName.initialize());
  ATH_CHECK(m_pixSpacePointName.initialize());
  ATH_CHECK(m_mbtsName.initialize());
  ATH_CHECK(m_larCollTimeName.initialize());
  if (!m_trtPhaseName.key().empty()) {
    ATH_CHECK(m_trtPhaseName.initialize());
  }
  ATH_CHECK(m_bcmRDOName.initialize(!m_bcmRDOName.key().empty()));
  ATH_CHECK(m_RISFTKey.initialize());
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------

StatusCode RawInfoSummaryForTagWriter::execute()
{
  ////////////////////////////////////////////////////////////////////////////////

  ATH_MSG_DEBUG("Executing " << name());
 
  EventContext ctx = Gaudi::Hive::currentContext();

  ATH_MSG_DEBUG("making RawInfoSummaryForTag object ");
  SG::WriteHandle<RawInfoSummaryForTag> RISFTobject(m_RISFTKey, ctx);
  ATH_CHECK(RISFTobject.record(std::make_unique<RawInfoSummaryForTag>()));

   ///////////////////////////////////////////////////////////////
   // Now get the number of entries in various containers...

   std::vector<unsigned int> nIDTracks;
   std::vector<bool> haveIDTrack;


   typedef std::pair<const Trk::Track*, double> trk_double;
   std::vector<trk_double> trkptvec;
   std::vector<trk_double>::const_iterator trkdoubleit;
   std::vector<double> ptvec;       
 
   // first ID tracks

   // input Track Collection
   int totNPixHits(0),totNSCTHits(0),totNTRTHits(0);
   
   MagField::AtlasFieldCache    fieldCache;
   // Get field cache object
   SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
   const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  
   if (fieldCondObj == nullptr) {
     ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
     return StatusCode::FAILURE;
   }
   fieldCondObj->getInitializedCache (fieldCache);

   SG::ReadHandle<TrackCollection> tracks{m_sgKeyIDtrack, ctx};
   for (const Trk::Track* track : *tracks) {

     const Trk::TrackSummary* summary = track->trackSummary();
     if (!summary) continue;       
     int nPixelHits  =  summary->get(Trk::numberOfPixelHits);
     int nTRTHits    =  summary->get(Trk::numberOfTRTHits);
     int nSCTHits    =  summary->get(Trk::numberOfSCTHits);
     if (nPixelHits>0) totNPixHits++;
     if (nSCTHits>0) totNSCTHits++;
     if (nTRTHits>0) totNTRTHits++;

     if(fieldCache.solenoidOn()){
       const Trk::Perigee* perigee = track->perigeeParameters(); 
       if(!perigee) continue;
       if(perigee->parameters()[Trk::qOverP]!=0){
	 double pt = fabs(sin(perigee->parameters()[Trk::theta])/perigee->parameters()[Trk::qOverP]);
	 ATH_MSG_VERBOSE( "ID track with parameters =" );
	 ATH_MSG_VERBOSE( "     d0 = " <<  perigee->parameters()[Trk::d0] );
	 ATH_MSG_VERBOSE( "     z0 = " <<   perigee->parameters()[Trk::z0] );
	 ATH_MSG_VERBOSE( "     phi0 = " << perigee->parameters()[Trk::phi0] );
	 ATH_MSG_VERBOSE( "     theta = " << perigee->parameters()[Trk::theta] );
	 ATH_MSG_VERBOSE( "     qoverP = " <<  perigee->parameters()[Trk::qOverP] );
	 ATH_MSG_VERBOSE( "     pt = " <<  pt );
	 trkptvec.push_back(trk_double(track,pt));
       } 
     }
     else {
       // if not field, take the tracks with maximum number of hits
       double tothits=  (double)(nPixelHits+nTRTHits+nSCTHits);
       trkptvec.push_back(trk_double(track,tothits));
     }
   }//end loop over tracks

   if(trkptvec.size()==0) {
     RISFTobject->setTrackLead1_d0(0.);
     RISFTobject->setTrackLead1_z0(0.);
     RISFTobject->setTrackLead1_phi0(0.);
     RISFTobject->setTrackLead1_theta(0.);
     RISFTobject->setTrackLead1_qoverp(0.);
     RISFTobject->setTrackLead2_d0(0.);
     RISFTobject->setTrackLead2_z0(0.);
     RISFTobject->setTrackLead2_phi0(0.);
     RISFTobject->setTrackLead2_theta(0.);
     RISFTobject->setTrackLead2_qoverp(0.);

   }
   else if(trkptvec.size()==1)  { 
     trkdoubleit = trkptvec.begin();
     trk_double mypair = (*trkdoubleit);
     const Trk::Track *track = mypair.first;

     RISFTobject->setTrackLead1_d0(track->perigeeParameters()->parameters()[Trk::d0]);
     RISFTobject->setTrackLead1_z0(track->perigeeParameters()->parameters()[Trk::z0]);
     RISFTobject->setTrackLead1_phi0(track->perigeeParameters()->parameters()[Trk::phi0]);
     RISFTobject->setTrackLead1_theta(track->perigeeParameters()->parameters()[Trk::theta]);
     RISFTobject->setTrackLead1_qoverp(track->perigeeParameters()->parameters()[Trk::qOverP]);

     RISFTobject->setTrackLead2_d0(0.);
     RISFTobject->setTrackLead2_z0(0.);
     RISFTobject->setTrackLead2_phi0(0.);
     RISFTobject->setTrackLead2_theta(0.);
     RISFTobject->setTrackLead2_qoverp(0.);

   }
   else {
     // find leading pts
     for(trkdoubleit=trkptvec.begin(); trkdoubleit!=trkptvec.end(); ++trkdoubleit) {
       ptvec.push_back((*trkdoubleit).second);       
     }

     sort(ptvec.begin(),ptvec.end());           

     double ptlead1 = *(ptvec.end()-1);
     double ptlead2 = *(ptvec.end()-2);
	 
     for(trkdoubleit=trkptvec.begin(); trkdoubleit!=trkptvec.end(); ++trkdoubleit){
       const Trk::Track *track = (*trkdoubleit).first;
       const Trk::Perigee* perigee = track->perigeeParameters();       
	   
       if((*trkdoubleit).second==ptlead1){

	 RISFTobject->setTrackLead1_d0(perigee->parameters()[Trk::d0]);
	 RISFTobject->setTrackLead1_z0(perigee->parameters()[Trk::z0]);
	 RISFTobject->setTrackLead1_phi0(perigee->parameters()[Trk::phi0]);
	 RISFTobject->setTrackLead1_theta(perigee->parameters()[Trk::theta]);
	 RISFTobject->setTrackLead1_qoverp(perigee->parameters()[Trk::qOverP]);

       }
	   
       if((*trkdoubleit).second==ptlead2){

	 RISFTobject->setTrackLead2_d0(perigee->parameters()[Trk::d0]);
	 RISFTobject->setTrackLead2_z0(perigee->parameters()[Trk::z0]);
	 RISFTobject->setTrackLead2_phi0(perigee->parameters()[Trk::phi0]);
	 RISFTobject->setTrackLead2_theta(perigee->parameters()[Trk::theta]);
	 RISFTobject->setTrackLead2_qoverp(perigee->parameters()[Trk::qOverP]);
	 
       }
     }
     trkptvec.clear();
     ptvec.clear();
   }

   RISFTobject->setPixelTracks(totNPixHits);
   RISFTobject->setSCTTracks(totNSCTHits);
   RISFTobject->setTRTTracks(totNTRTHits);


   ////////////////////////////////////////////   
   //  these need to be in the correct order!!!
  ///////////////////////////////////
    // total Energy sums
    float totClusterEne(0.),totCellEne(0.),CellEx(0.),CellEy(0.),CellMissEt(0.),CellMissEtPhi(0.);
    float totCellEneEMB(0.),totCellEneEMEC(0.),totCellEneHEC(0.),totCellEneFCAL(0.),totCellEneTile(0.);
    //
    SG::ReadHandle<CaloCellContainer> cell_container{m_cellContKey, ctx};
    
    for (const CaloCell* cell : *cell_container) {
      float energy = cell->energy();
      totCellEne += energy;
      CellEx += energy*(cell->cosPhi())*(cell->sinTh());
      CellEy += energy*(cell->sinPhi())*(cell->sinTh());

      // sum per subcalo
      unsigned int iCaloNum = static_cast<unsigned int>(cell->caloDDE()->getSubCalo()); 
      if (iCaloNum==CaloCell_ID::LAREM) {
	int inum = cell->caloDDE()->getSampling();
	if (inum<CaloSampling::PreSamplerE) totCellEneEMB += energy;
	else  totCellEneEMEC += energy;
      }
      if (iCaloNum==CaloCell_ID::LARHEC) totCellEneHEC+= energy;
      if (iCaloNum==CaloCell_ID::LARFCAL) totCellEneFCAL+= energy;
      if (iCaloNum==CaloCell_ID::TILE) totCellEneTile+= energy;
    }//end loop over cells

    CellMissEt = sqrt(CellEx*CellEx+CellEy*CellEy);
    CellMissEtPhi = atan2f(-1.*CellEy,-1.*CellEx);
    

    float clusEt1(0.),clusEta1(-999.),clusPhi1(-999.),clusE1(0.);

    if(m_doClusterSums){
      SG::ReadHandle< xAOD::CaloClusterContainer> cluster_container(m_caloClusterKey, ctx);
      for (const auto* cluster : *cluster_container) {
	const float energy = cluster->e();
	totClusterEne += energy;
	if (std::fabs(energy) > clusE1) {
	  clusE1=energy;
	  clusEta1= cluster->eta();
	  clusPhi1= cluster->phi();
	  clusEt1 = cluster->et();
	}
      }
    }// if doClusterSum
    
    
    RISFTobject->setCellEnergySum(totCellEne);
    RISFTobject->setCellEnergySumEMB(totCellEneEMB);
    RISFTobject->setCellEnergySumEMEC(totCellEneEMEC);
    RISFTobject->setCellEnergySumHEC(totCellEneHEC);
    RISFTobject->setCellEnergySumFCAL(totCellEneFCAL);
    RISFTobject->setCellEnergySumTile(totCellEneTile);
    RISFTobject->setClusterEnergySum(totClusterEne);
    RISFTobject->setCellMET(CellMissEt);
    RISFTobject->setCellMETPhi(CellMissEtPhi);

    RISFTobject->setTopoClEt1(clusEt1);
    RISFTobject->setTopoClEta1(clusEta1);
    RISFTobject->setTopoClPhi1(clusPhi1);


    ///////////////////////////////
    // hit multiplicities...
    unsigned int  nDCs(0), nDCsAll(0);
    // TRT
    SG::ReadHandle<InDet::TRT_DriftCircleContainer> TRTcontainer(m_TRT_DriftCircleName, ctx);
    for(InDet::TRT_DriftCircleContainer::const_iterator it=TRTcontainer->begin(); it!=TRTcontainer->end(); it++) {
      const InDet::TRT_DriftCircleCollection *colNext=&(**it);
      if (!colNext) continue;
	  
      // loop over Clusters
      DataVector<InDet::TRT_DriftCircle>::const_iterator p_DC;
      for(p_DC=colNext->begin(); p_DC!=colNext->end(); ++p_DC) {
	//	    ATH_MSG_DEBUG("TRT DriftCircle ID=" << m_trtID->show_to_string( (*p_DC)->identify()));
	//	    Identifier DCId = (*p_DC)->identify();
	const InDet::TRT_DriftCircle& DC = **p_DC;
	nDCsAll++;
	if (DC.highLevel())  nDCs++; 
      }
    }
    // SCT SPs
    unsigned int spsize=0;
    
    SG::ReadHandle<SpacePointContainer> SCT_spcontainer(m_sctSpacePointName, ctx);
    for(SpacePointContainer::const_iterator it=SCT_spcontainer->begin(); it!=SCT_spcontainer->end(); ++it) {
      const SpacePointCollection *colNext=&(**it);
      if (!colNext) continue;
      spsize+=colNext->size();
    }
    
     
      // Pixel SPs
     unsigned int pspsize=0;
     SG::ReadHandle<SpacePointContainer>  P_spcontainer(m_pixSpacePointName, ctx);
     for(SpacePointContainer::const_iterator it=P_spcontainer->begin(); it!=P_spcontainer->end(); ++it) {
       const SpacePointCollection *colNext=&(**it);
       if (!colNext) continue;
       pspsize+=colNext->size();
     }

    RISFTobject->setNsctSPs(spsize);
    RISFTobject->setNtrtHtDCs(nDCs);
    RISFTobject->setNtrtDCs(nDCsAll);
    RISFTobject->setNpixSPs(pspsize);

   //////////////////////////////////////
   // BCM tag bit

   unsigned int bcmHit=0;
   
   if (!m_bcmRDOName.key().empty()) {
       SG::ReadHandle<BCM_RDO_Container> bcmRDO(m_bcmRDOName, ctx);
    int num_collect = bcmRDO->size();
    if ( num_collect != 16 ){
       ATH_MSG_WARNING (" Number of collections: " << num_collect);
    }

    BCM_RDO_Container::const_iterator chan_itr = bcmRDO->begin();
    BCM_RDO_Container::const_iterator chan_itr_end = bcmRDO->end();  
    for (; chan_itr != chan_itr_end; chan_itr++) {
       // Loop over all BCM hits in this collection
       BCM_RDO_Collection::const_iterator bcm_itr = (*chan_itr)->begin();
       BCM_RDO_Collection::const_iterator bcm_itr_end = (*chan_itr)->end();

       for (; bcm_itr != bcm_itr_end; bcm_itr++) {
        if ((*bcm_itr)->getPulse1Width() != 0 || (*bcm_itr)->getPulse2Width() != 0) {
	  bcmHit++;
        }
       } // end of loop over raw data
    } // end of loop over collections
   }
   RISFTobject->setBCMHit(bcmHit);



   ///////////////////////////////////////////////////////
   // MBTS word - copied from: 

   SG::ReadHandle<TileCellContainer> tileCellCnt(m_mbtsName, ctx);
   uint32_t ibit, bit_pos = 0;
   float charge = 0;
   uint32_t mbtsBits = 0;
   // Discriminate the signals
   TileCellContainer::const_iterator itr = tileCellCnt->begin();
   TileCellContainer::const_iterator itr_end = tileCellCnt->end();
   for(; itr != itr_end; ++itr) {
     charge = (*itr)->energy();
     ATH_MSG_DEBUG( "Energy =" << charge << "pC");
     if(charge > m_mbts_threshold) {
       Identifier id=(*itr)->ID();
       // cache type, module and channel
       // MBTS Id type is  "side"  +/- 1
       int type_id = m_tileTBID->type(id);
       // MBTS Id module is "phi"  0-7
       int module_id = m_tileTBID->module(id);
       // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
       int channel_id = m_tileTBID->channel(id);

       // Catch errors
       if( abs(type_id) != 1 ){
	 ATH_MSG_WARNING ("MBTS identifier type is out of range");
	 continue;
       }
       if( channel_id < 0 || channel_id > 1 ){
	 ATH_MSG_WARNING ("MBTS identifier channel is out of range");
	 continue;
       }
       if( module_id < 0 || module_id > 7 ){
	 ATH_MSG_WARNING ("MBTS identifier module is out of range");
	 continue;
       }      
       bit_pos = 0; // The position of the bit
       if(type_id == -1) {
	 bit_pos += 16;
       }
       bit_pos += channel_id*8;
       bit_pos += module_id;
       ibit = 1; // The mbts bit for this TileCell
       ibit <<= bit_pos;
       mbtsBits += ibit;
     }
   }

   RISFTobject->setMBTSword(mbtsBits);

   SG::ReadHandle<MBTSCollisionTime> mbtsTime{m_MBTSCollTimeKey, ctx};
   float timeDiff=-999.;
   float timeSum=-999.;
   int MBTS_SideCut(2);
   if (mbtsTime->ncellA()>MBTS_SideCut && mbtsTime->ncellC()>MBTS_SideCut){
     timeDiff=mbtsTime->time();
     timeSum =mbtsTime->timeA() + mbtsTime->timeC();
   }
   RISFTobject->setMBTStimeDiff(timeDiff);
   RISFTobject->setMBTStimeAvg(timeSum/2.);

   ////////////////////////////////////////////////////
   // LAr EC collision timing stuff (from Guillaume...)

   float LArECtimeDiff=-999.;
   float LArECtimeSum=-999.;
   SG::ReadHandle<LArCollisionTime> tps(m_larCollTimeName, ctx);
   const int nMin=2;
   if (tps->ncellA() > nMin && tps->ncellC() > nMin){
     LArECtimeDiff =   tps->timeA()-tps->timeC();
     LArECtimeSum =   tps->timeA()+tps->timeC();
   } 

   RISFTobject->setLArECtimeDiff(LArECtimeDiff);
   RISFTobject->setLArECtimeAvg(LArECtimeSum/2.);
   //////////////////////////////////
   // adding in TRT Event Phase
   // taken from: 
   // https://twiki.cern.ch/twiki/bin/view/Atlas/TRTEventPhase#Accessing_TRT_time_from_ESD_file
   float myTRTtime=0;
   if (!m_trtPhaseName.key().empty()) {
     SG::ReadHandle<ComTime> theComTime(m_trtPhaseName, ctx);
     myTRTtime = theComTime->getTime();
   }
   RISFTobject->setTrtEventPhase(myTRTtime);
   
   return StatusCode::SUCCESS;
  }

//----------------------------------------------------------------

StatusCode RawInfoSummaryForTagWriter::finalize() {

  ATH_MSG_INFO ( "In finalize()");

  return StatusCode::SUCCESS;

}

