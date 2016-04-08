/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
// code to write the TAG_COMM variables that are only available from 
// RAW into a persistified object for writing to the TAG - Jamie Boyd
/////////////////////////////////////////////////////////////////////////

#include "TagEvent/RawInfoSummaryForTag.h"

#include "StoreGate/StoreGateSvc.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "CommissionEvent/ComTime.h" 
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "InDetPrepRawData/TRT_DriftCircleContainer.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "TileEvent/TileCellContainer.h"
#include "TileEvent/MBTSCollisionTime.h"

#include "EventTagRawAlgs/RawInfoSummaryForTagWriter.h"
#include "TileIdentifier/TileTBID.h"

#include <algorithm>

#include "LArRecEvent/LArCollisionTime.h"

//----------------------------------------------------------------

RawInfoSummaryForTagWriter::RawInfoSummaryForTagWriter(const std::string& name,
                                 ISvcLocator* pSvcLocator) :
  AthAlgorithm(name,pSvcLocator),
  m_doTileMuonFitter(true),
  m_fieldServiceHandle("AtlasFieldSvc",name)
{
  declareProperty("IDTrackKey",m_sgKeyIDtrack);
  declareProperty("doClusterSums",m_doClusterSums=true);
  declareProperty("TRT_DriftCircleContainerName", m_TRT_DriftCircleName="TRT_DriftCircles");
  declareProperty("MBTS_Threshold",  m_mbts_threshold = 40.0/222.0 );  // Value in pC
  declareProperty("MagFieldSvc"        , m_fieldServiceHandle);
}

//----------------------------------------------------------------

RawInfoSummaryForTagWriter::~RawInfoSummaryForTagWriter()
{ 
}


//----------------------------------------------------------------

StatusCode RawInfoSummaryForTagWriter::initialize()
{

  ATH_MSG_INFO("In initialize()");

  StatusCode sc;

  sc = service( "StoreGateSvc", m_storeGate);
  if( sc.isFailure() ) {
    ATH_MSG_FATAL("Unable to locate the eventStore");
    return sc;
  } 

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !");
    return StatusCode::FAILURE;
  }

  // Retrieve TileTBID helper from det store
  // (The MBTS was added to the Test Beam (TB) list.)
  if(detStore->retrieve(m_tileTBID).isFailure()) {
    ATH_MSG_ERROR ("Unable to retrieve TileTBID helper from DetectorStore");
    return StatusCode::FAILURE;
  }
  
  ATH_CHECK(m_fieldServiceHandle.retrieve());

  return StatusCode::SUCCESS;  
}

//----------------------------------------------------------------

StatusCode RawInfoSummaryForTagWriter::execute()
{
  ////////////////////////////////////////////////////////////////////////////////
  // first retrieve the event info, runnumber, event number, lumi block number...
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG("Executing " << name());
 

  ATH_MSG_DEBUG("making RawInfoSummaryForTag object ");
  RawInfoSummaryForTag* RISFTobject = new RawInfoSummaryForTag();


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
   const TrackCollection *tracks   = 0;
   sc = m_storeGate->retrieve(tracks, m_sgKeyIDtrack);

   if(sc.isFailure()){
     ATH_MSG_WARNING (" could not open Track collection : " <<  m_sgKeyIDtrack);
     ATH_MSG_WARNING (" will skip this collection!!");

     RISFTobject->setPixelTracks(0);
     RISFTobject->setSCTTracks(0);
     RISFTobject->setTRTTracks(0);

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
   else{
     // loop through the tracks;
     TrackCollection::const_iterator trksItr  = tracks->begin();
     TrackCollection::const_iterator trksItrE = tracks->end();
     
     for (; trksItr != trksItrE; ++trksItr) {     
       const Trk::Track* track = *trksItr;

       const Trk::TrackSummary* summary = track->trackSummary();
       if (!summary) continue;       
       int nPixelHits  =  summary->get(Trk::numberOfPixelHits);
       int nTRTHits    =  summary->get(Trk::numberOfTRTHits);
       int nSCTHits    =  summary->get(Trk::numberOfSCTHits);
       if (nPixelHits>0) totNPixHits++;
       if (nSCTHits>0) totNSCTHits++;
       if (nTRTHits>0) totNTRTHits++;

       if(m_fieldServiceHandle->solenoidOn()){
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
     }

     if(trkptvec.size()==0)
       {

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
     else if(trkptvec.size()==1) 
       { 
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
     else 
       {
	 // find leading pts
	 for(trkdoubleit=trkptvec.begin(); trkdoubleit!=trkptvec.end(); ++trkdoubleit)
	   ptvec.push_back((*trkdoubleit).second);       
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
   }

   ////////////////////////////////////////////   
   //  these need to be in the correct order!!!
  ///////////////////////////////////
    // total Energy sums
    float totClusterEne(0.),totCellEne(0.),CellEx(0.),CellEy(0.),CellMissEt(0.),CellMissEtPhi(0.);
    float totCellEneEMB(0.),totCellEneEMEC(0.),totCellEneHEC(0.),totCellEneFCAL(0.),totCellEneTile(0.);
    //
    const CaloCellContainer* cell_container;
    std::string m_cellsContName = "AllCalo";

    //Retrieve Cell collection from SG
    if ( m_storeGate->contains<CaloCellContainer>(m_cellsContName)){
      sc = m_storeGate->retrieve(cell_container, m_cellsContName);
    
      CaloCellContainer::const_iterator iCell = cell_container->begin();
      CaloCellContainer::const_iterator lastCell  = cell_container->end();
      
      for( ; iCell != lastCell; ++iCell) {
        
        //perhaps put a if ( (*iCell)->energy > threshold ) {
        const CaloCell* cell = (*iCell);
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
      }
    }
    CellMissEt = sqrt(CellEx*CellEx+CellEy*CellEy);
    CellMissEtPhi = atan2f(-1.*CellEy,-1.*CellEx);


    float clusEt1(0.),clusEta1(-999.),clusPhi1(-999.),clusE1(0.);

    // clusters crashes if Tile=False so protect with a JO switch
    if(m_doClusterSums){
      const xAOD::CaloClusterContainer* cluster_container;
      std::string m_clustersContName="CaloTopoCluster";
      //Retrieve Cluster collection from SG
      if ( m_storeGate->contains<xAOD::CaloClusterContainer>(m_clustersContName)){
        sc = m_storeGate->retrieve(cluster_container, m_clustersContName);
        xAOD::CaloClusterContainer::const_iterator iCluster = cluster_container->begin();
        xAOD::CaloClusterContainer::const_iterator lastCluster  = cluster_container->end();
        for( ; iCluster != lastCluster; ++iCluster) {
          float energy = (*iCluster)->e();
          totClusterEne += energy;
          if (std::fabs(energy) > clusE1) {
            clusE1=energy;
            clusEta1=(*iCluster)->eta();
            clusPhi1=(*iCluster)->phi();
            clusEt1 = (*iCluster)->et();
          }
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
      const InDet::TRT_DriftCircleContainer* TRTcontainer;
      if (m_storeGate->contains<InDet::TRT_DriftCircleContainer>( m_TRT_DriftCircleName)){
      sc = m_storeGate->retrieve(TRTcontainer, m_TRT_DriftCircleName);
      if (sc.isFailure()  || !TRTcontainer){
	ATH_MSG_WARNING("TRT DriftCircle container not found");
      }
      else{ 
	// loop over clusters collections
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
      }
      }
      // SCT SPs
     unsigned int spsize=0;

     const SpacePointContainer* SCT_spcontainer;
     std::string sct_sp_key("SCT_SpacePoints");
     if (m_storeGate->contains<SpacePointContainer>( sct_sp_key)){
       sc = m_storeGate->retrieve(SCT_spcontainer, sct_sp_key);
       if (sc.isFailure()  || !SCT_spcontainer){
	 ATH_MSG_WARNING ("SCT SP container not found");
       }
       else{ 
	 for(SpacePointContainer::const_iterator it=SCT_spcontainer->begin(); it!=SCT_spcontainer->end(); ++it) {
	   const SpacePointCollection *colNext=&(**it);
	   if (!colNext) continue;
	   spsize+=colNext->size();
	}
       }
     }
      // Pixel SPs
     unsigned int pspsize=0;

     const SpacePointContainer* P_spcontainer;
     std::string p_sp_key("PixelSpacePoints");
     if (m_storeGate->contains<SpacePointContainer>( p_sp_key)){
       sc = m_storeGate->retrieve(P_spcontainer, p_sp_key);
       if (sc.isFailure()  || !P_spcontainer){
	 ATH_MSG_WARNING ("Pixel SP container not found");
       }
       else{ 
	 for(SpacePointContainer::const_iterator it=P_spcontainer->begin(); it!=P_spcontainer->end(); ++it) {
	   const SpacePointCollection *colNext=&(**it);
	   if (!colNext) continue;
	   pspsize+=colNext->size();
	}
       }
     }
    RISFTobject->setNsctSPs(spsize);
    RISFTobject->setNtrtHtDCs(nDCs);
    RISFTobject->setNtrtDCs(nDCsAll);
    RISFTobject->setNpixSPs(pspsize);

   //////////////////////////////////////
   // BCM tag bit

   unsigned int bcmHit=0;

   const BCM_RDO_Container *m_bcmRDO=0;
   if (StatusCode::SUCCESS!=m_storeGate->retrieve(m_bcmRDO,"BCM_RDOs")) {
     ATH_MSG_WARNING ("Cannot find BCM RDO! ");
   }   else {
     int num_collect = m_bcmRDO->size();
     if ( num_collect != 16 ){
       ATH_MSG_WARNING (" Number of collections: " << num_collect);
     }
   
     BCM_RDO_Container::const_iterator chan_itr = m_bcmRDO->begin();
     BCM_RDO_Container::const_iterator chan_itr_end = m_bcmRDO->end();
     
     
     for (; chan_itr != chan_itr_end; chan_itr++) {
       
       // Loop over all BCM hits in this collection
       BCM_RDO_Collection::const_iterator bcm_itr = (*chan_itr)->begin();
       BCM_RDO_Collection::const_iterator bcm_itr_end = (*chan_itr)->end();
       
       for (; bcm_itr != bcm_itr_end; bcm_itr++) {
         if ((*bcm_itr)->getPulse1Width() != 0 || (*bcm_itr)->getPulse2Width() != 0) {
	   ///                       PLEASE TAG THE EVENT!!!
	   bcmHit++;
         }
       } // end of loop over raw data
     } // end of loop over collections
   }
   RISFTobject->setBCMHit(bcmHit);

     ///////////////////////////////////////////////////////
     // MBTS word - copied from: 
     // http://alxr.usatlas.bnl.gov/lxr/source/atlas/TriggerAlgorithms/TrigT2MinBias/src/T2MbtsFex.cxx
     m_mbtsContainerName = "MBTSContainer"; // The default when no key is found.
     const TileCellContainer *tileCellCnt = 0;
     uint32_t mbtsBits = 0;
     sc=m_storeGate->retrieve(tileCellCnt, m_mbtsContainerName);
     if( sc.isFailure()  || !tileCellCnt ) {
       ATH_MSG_WARNING ("Error retrieving " << m_mbtsContainerName);
     }
     else 
     {
       uint32_t ibit, bit_pos = 0;
       float m_charge = 0;
       // Discriminate the signals
       TileCellContainer::const_iterator itr = tileCellCnt->begin();
       TileCellContainer::const_iterator itr_end = tileCellCnt->end();
       for(; itr != itr_end; ++itr) {
	 m_charge = (*itr)->energy();
	 ATH_MSG_DEBUG( "Energy =" << m_charge << "pC");
	 if(m_charge > m_mbts_threshold) {
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
     }
     RISFTobject->setMBTSword(mbtsBits);

     const MBTSCollisionTime * mbtsTime;
     sc = m_storeGate->retrieve(mbtsTime,"MBTSCollisionTime");
     float timeDiff=-999.;
     float timeSum=-999.;
     int m_MBTS_SideCut(2);
     if (!sc.isFailure()) {
       if (mbtsTime->ncellA()>m_MBTS_SideCut && mbtsTime->ncellC()>m_MBTS_SideCut){
    	   timeDiff=mbtsTime->time();
       	   timeSum =mbtsTime->timeA() + mbtsTime->timeC();}
     }
     RISFTobject->setMBTStimeDiff(timeDiff);
     RISFTobject->setMBTStimeAvg(timeSum/2.);

     ////////////////////////////////////////////////////
     // LAr EC collision timing stuff (from Guillaume...)

     float LArECtimeDiff=-999.;
     float LArECtimeSum=-999.;
     const  LArCollisionTime* tps;
     sc = m_storeGate->retrieve(tps,"LArCollisionTime");
     if (!sc.isFailure()) {
       const       int nMin=2;
       if (tps->ncellA() > nMin && tps->ncellC() > nMin){
    	   LArECtimeDiff =   tps->timeA()-tps->timeC();
    	   LArECtimeSum =   tps->timeA()+tps->timeC();
       }
     } 

     RISFTobject->setLArECtimeDiff(LArECtimeDiff);
     RISFTobject->setLArECtimeAvg(LArECtimeSum/2.);
     //////////////////////////////////
     // adding in TRT Event Phase
     // taken from: 
     // https://twiki.cern.ch/twiki/bin/view/Atlas/TRTEventPhase#Accessing_TRT_time_from_ESD_file
     float myTRTtime = 0.;
     const ComTime* theComTime;
     if (m_storeGate->contains<ComTime>("TRT_Phase")) {
	sc = m_storeGate->retrieve(theComTime, "TRT_Phase");
	if (sc.isFailure()) {
	    //	   msg(MSG::ERROR) << "ComTime object not found with name TRT_Phase !!!" );
	} else {
	     myTRTtime = theComTime->getTime();
	}
      }

RISFTobject->setTrtEventPhase(myTRTtime);

//ATH_MSG_VERBOSE((*RISFTobject));

   // store the object in SG
   sc = m_storeGate->record(RISFTobject, "RawInfoSummaryForTag");
   if (sc.isFailure())
     {
       ATH_MSG_WARNING("Could not record RawInfoSummaryForTag object.");
       return (StatusCode::FAILURE);
     }



   return StatusCode::SUCCESS;
  }

//----------------------------------------------------------------

StatusCode RawInfoSummaryForTagWriter::finalize() {

  ATH_MSG_INFO ( "In finalize()");

  return StatusCode::SUCCESS;

}

