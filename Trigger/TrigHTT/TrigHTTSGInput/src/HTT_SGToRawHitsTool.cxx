/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTSGInput/HTT_SGToRawHitsTool.h"

#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTOfflineHit.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTObjects/HTTHit.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"

#include "StoreGate/DataHandle.h"

#include "IdDictDetDescr/IdDictManager.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiClusterCollection.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetSimData/SCT_SimHelper.h"
#include "InDetSimData/PixelSimHelper.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"

#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"

#include "TrkParameters/TrackParameters.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "GaudiKernel/IPartPropSvc.h"


HTT_SGToRawHitsTool::HTT_SGToRawHitsTool(const std::string& algname, const std::string &name, const IInterface *ifc) :
  base_class(algname,name,ifc),
  m_truthToTrack( "Trk::TruthToTrack/InDetTruthToTrack" ),
  m_extrapolator( "Trk::Extrapolator/AtlasExtrapolator" ),
  m_beamSpotSvc( "BeamCondSvc" , name ),
  m_maxEta(3.3),
  m_minPt(.8*CLHEP::GeV),
  m_idHelper(nullptr)
{
  declareProperty("maxEta",                   m_maxEta);
  declareProperty("minPt",                    m_minPt);
  declareProperty("pixelClustersName",        m_pixelClustersName="PixelClusters", "name of pixel collection");
  declareProperty("SCT_ClustersName",         m_sctClustersName=  "SCT_Clusters", "name of SCT collection");
  declareProperty("OfflineName",              m_offlineName = "InDetTrackParticles", "name of offline tracks collection");
  declareProperty("dumpHitsOnTracks",         m_dumpHitsOnTracks=false, "dump hits of tracks if true");
  declareProperty("dumpTruthIntersections",   m_dumpTruthIntersections=false, "dump truth intersection if true");
  declareProperty("TruthToTrackTool"        , m_truthToTrack);
  declareProperty("Extrapolator"            , m_extrapolator);
  declareProperty("BeamCondSvc"             , m_beamSpotSvc);
  declareProperty("ReadOfflineClusters",  m_readOfflineClusters=true, "flag to enable the offline cluster save");
  declareProperty("ReadTruthTracks",  m_readTruthTracks=true, "flag to enable the truth tracking save");
  declareProperty("ReadOfflineTracks",m_readOfflineTracks = true, "flag to enable the offline tracking save");
  declareProperty("UseNominalOrigin", m_UseNominalOrigin=false);
}




// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode HTT_SGToRawHitsTool::initialize(){

  ATH_MSG_DEBUG ("HTT_SGToRawHitsTool::initialize()");

  ATH_CHECK(m_truthToTrack.retrieve());

  ATH_CHECK (m_extrapolator.retrieve());

  ATH_CHECK (m_beamSpotSvc.retrieve());

  IPartPropSvc* partPropSvc = 0;
  ATH_CHECK(service("PartPropSvc", partPropSvc));
  m_particleDataTable = partPropSvc->PDT();


  const IdDictManager* idDictMgr( 0 );
  ATH_CHECK( detStore()->retrieve(idDictMgr, "IdDict") );

  // ID helpers
  m_idHelper = new AtlasDetectorID;
  if (idDictMgr->initializeHelper(*m_idHelper)) {
     ATH_MSG_ERROR ("Unable to initialize ID IdHelper" );
     return StatusCode::FAILURE;
   }

  ATH_CHECK( detStore()->retrieve(m_PIX_mgr, "Pixel") );
  ATH_CHECK( detStore()->retrieve(m_pixelId, "PixelID") );
  ATH_CHECK( detStore()->retrieve(m_SCT_mgr, "SCT") );
  ATH_CHECK( detStore()->retrieve(m_sctId,   "SCT_ID") );

  ATH_MSG_DEBUG ("Initialization complete");
  return StatusCode::SUCCESS;
}


StatusCode HTT_SGToRawHitsTool::finalize(){
  ATH_MSG_INFO ("finalized");
  if( m_idHelper ) { delete m_idHelper; }
  return StatusCode::SUCCESS;
}


/** This function get from the SG the inner detector raw hits
  and prepares them for HTT simulation */
StatusCode HTT_SGToRawHitsTool::readData(HTTEventInputHeader* header)
{
  m_eventHeader=header; //take the external pointer

  const EventInfo* eventInfo(0);
  ATH_CHECK(evtStore()->retrieve(eventInfo));

  const EventID* eventID( eventInfo->event_ID() );

  //Filled to variable / start event
  HTTEventInfo event_info;
  event_info.setRunNumber(eventID->run_number());
  event_info.setEventNumber(eventID->event_number());
  event_info.setLB(eventID->lumi_block());
  event_info.setBCID(eventID->bunch_crossing_id());
  event_info.setaverageInteractionsPerCrossing(eventInfo->averageInteractionsPerCrossing());
  event_info.setactualInteractionsPerCrossing(eventInfo->actualInteractionsPerCrossing());

  const TriggerInfo *triggerInfo(eventInfo->trigger_info());
  event_info.setextendedLevel1ID(triggerInfo->extendedLevel1ID() );
  event_info.setlevel1TriggerType(triggerInfo->level1TriggerType () );
  event_info.setlevel1TriggerInfo(triggerInfo->level1TriggerInfo ());

  m_eventHeader->newEvent(event_info);//this also reset all varaibles

  HitIndexMap hitIndexMap; // keep running index event-unique to each hit
  HitIndexMap pixelClusterIndexMap;

  // get pixel and sct cluster containers
  if( evtStore()->retrieve(m_pixelContainer, m_pixelClustersName).isFailure() ) {
    ATH_MSG_WARNING ( "unable to retrieve the PixelCluster container " << m_pixelClustersName);
  }
  if( evtStore()->retrieve(m_sctContainer, m_sctClustersName).isFailure() ) {
   ATH_MSG_WARNING ( "unable to retrieve the SCT_Cluster container " << m_sctClustersName);
  }

  // dump raw silicon data
  ATH_MSG_INFO ("Dump raw silicon data");
  ATH_CHECK( read_raw_silicon( hitIndexMap, pixelClusterIndexMap ) );

  ATH_MSG_INFO ("Found list of hits, size ="<< m_eventHeader->nHits());
  HTTOptionalEventInfo optional;

  if (m_readOfflineClusters){
    ATH_CHECK( read_offline_clusters(optional));
    ATH_MSG_INFO ("Saved "<<optional.nOfflineClusters()<<" offline clusters");
  }

  if (m_readTruthTracks){
    if (read_truth_tracks(optional) != StatusCode::SUCCESS){
      ATH_MSG_ERROR ("Error in reading truth tracks");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Saved "<<optional.nTruthTracks()<<" truth tracks");
  }

  if (m_readOfflineTracks){
    if (read_offline_tracks(optional)!= StatusCode::SUCCESS){
      ATH_MSG_ERROR ("Error in reading offline tracks");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ("Saved "<<optional.nOfflineTracks()<<" offline tracks");
  }


  m_eventHeader->setOptional(optional);
  ATH_MSG_DEBUG ( *m_eventHeader);
  ATH_MSG_DEBUG ( "End of execute()");
  return StatusCode::SUCCESS;
}


StatusCode HTT_SGToRawHitsTool::read_offline_tracks(HTTOptionalEventInfo& optional)
{
  const xAOD::TrackParticleContainer *offlineTracks = nullptr;
  if(evtStore()->retrieve(offlineTracks,m_offlineName).isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve Offline Tracks " << m_offlineName);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO ("read Offline tracks, size= "<< offlineTracks->size());
  auto track_it   = offlineTracks->begin();
  auto last_track = offlineTracks->end();
  optional.reserveTrack(offlineTracks->size());
  for (int iTrk=0 ; track_it!= last_track; track_it++, iTrk++){
    auto track = (*track_it)->track();
    HTTOfflineTrack tmpOfflineTrack;
    tmpOfflineTrack.setPt((*track_it)->pt()*(*track_it)->charge());
    tmpOfflineTrack.setEta((*track_it)->eta());
    tmpOfflineTrack.setPhi((*track_it)->phi());

    const DataVector<const Trk::TrackStateOnSurface>* trackStates=track->trackStateOnSurfaces();
    if(!trackStates)    {
      ATH_MSG_ERROR("trackStatesOnSurface troubles");
      return StatusCode::FAILURE;
    }
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it=trackStates->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end=trackStates->end();
    if (!(*it)) {
      ATH_MSG_WARNING("TrackStateOnSurface == Null" << std::endl);
      continue;
    }
    for (; it!=it_end; it++) {
      const Trk::TrackStateOnSurface* tsos=(*it);
      if (tsos == 0) continue;
      if ((*it)->type(Trk::TrackStateOnSurface::Measurement) ){
	const Trk::MeasurementBase *measurement = (*it)->measurementOnTrack();
	if(  (*it)->trackParameters() !=0 &&
	     (*it)->trackParameters()->associatedSurface().associatedDetectorElement() != nullptr &&
	     (*it)->trackParameters()->associatedSurface().associatedDetectorElement()->identify() !=0
	     ){
	  const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
	  const Identifier & hitId = hit->identify();
	  HTTOfflineHit tmpOfflineHit;
	  if (m_idHelper->is_pixel(hitId)) {
	    tmpOfflineHit.setIsPixel(true);
	    tmpOfflineHit.setIsBarrel(m_pixelId->is_barrel(hitId));

	    const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(hitId);
	    tmpOfflineHit.setClusterID(sielement->identifyHash());
	    tmpOfflineHit.setTrackNumber(iTrk);
	    tmpOfflineHit.setLayer(m_pixelId->layer_disk(hitId));
	    tmpOfflineHit.setLocX((float)measurement->localParameters()[Trk::locX]);
	    tmpOfflineHit.setLocY((float)measurement->localParameters()[Trk::locY]);
	  }
	  else if (m_idHelper->is_sct(hitId)) {
	    tmpOfflineHit.setIsPixel(false);
	    tmpOfflineHit.setIsBarrel(m_sctId->is_barrel(hitId));
	    const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(hitId);
	    tmpOfflineHit.setClusterID(sielement->identifyHash());
	    tmpOfflineHit.setTrackNumber(iTrk);
	    tmpOfflineHit.setLayer(m_pixelId->layer_disk(hitId));
	    tmpOfflineHit.setLocX(((float)measurement->localParameters()[Trk::locX]));
	    tmpOfflineHit.setLocY(-99999.9);
	  }
	  tmpOfflineTrack.addHit(tmpOfflineHit);
	}
      }
    }
    optional.addOfflineTrack(tmpOfflineTrack);
  }//end of loop over tracks


 return StatusCode::SUCCESS;
}



// dump silicon channels with geant matching information.
StatusCode
HTT_SGToRawHitsTool::read_raw_silicon( HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap ) // const cannot make variables push back to DataInput
{
  ATH_MSG_INFO ("read silicon hits");
  unsigned int hitIndex = 0u;

  if (ReadPixelSimulation(hitIndexMap, hitIndex).isFailure()) {
    ATH_MSG_ERROR("Failure in ReadPixelSimulation");
    return StatusCode::FAILURE;
  }

  if (ReadStripSimulation(hitIndexMap, hitIndex).isFailure()) {
    ATH_MSG_ERROR("Failure in StripSimulation");
    return StatusCode::FAILURE;
  }

  if (DumpPixelClusters(pixelClusterIndexMap).isFailure()) {
    ATH_MSG_ERROR("Failure in DumpPixelClusters");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode
HTT_SGToRawHitsTool::ReadPixelSimulation( HitIndexMap& hitIndexMap, unsigned int &hitIndex ) {
  const DataHandle<PixelRDO_Container> pixel_rdocontainer_iter;
  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = evtStore()->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if (!have_pixel_sdo) {
    ATH_MSG_WARNING ( "Missing Pixel SDO Map");
  }

  ATH_MSG_INFO ("Found Pixel SDO Map");

  if( evtStore()->retrieve(pixel_rdocontainer_iter, "PixelRDOs").isSuccess()  ) {
    m_eventHeader->reserveHits(pixel_rdocontainer_iter->size());
    pixel_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( PixelRDO_Container::const_iterator iColl=pixel_rdocontainer_iter->begin(), fColl=pixel_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection(*iColl);
      if( !pixel_rdoCollection ) { continue; }
      // loop on all RDOs
      for( DataVector<PixelRDORawData>::const_iterator iRDO=pixel_rdoCollection->begin(), fRDO=pixel_rdoCollection->end(); iRDO!=fRDO; ++iRDO ) {
        Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert( sielement);

	Amg::Vector2D LocalPos = sielement->localPositionOfCell(rdoId);
	Amg::Vector3D globalPos = sielement->globalPosition(LocalPos);

	InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier (rdoId);

        // update map between pixel identifier and event-unique hit index.
        // ganged pixels (nCells==2) get two entries.
        hitIndexMap[rdoId] = hitIndex;
        const int nCells = sielement->numberOfConnectedCells(cellID);
        if( nCells==2 ) {
          const InDetDD::SiCellId tmpCell = sielement->connectedCell(cellID,1);
          const Identifier tmpId = sielement->identifierFromCellId(tmpCell);
          hitIndexMap[tmpId] = hitIndex; // add second entry for ganged pixel ID
        }
        // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel.
        const HepMC::GenParticle* bestParent = 0;
	TrigHTTInputUtils::ParentBitmask parentMask;
        HepMcParticleLink::ExtendedBarCode bestExtcode;
        if( have_pixel_sdo && pixelSimDataMap ) {
          InDetSimDataCollection::const_iterator iter( pixelSimDataMap->find(rdoId) );
          if( nCells>1 && iter==pixelSimDataMap->end() ) {
	    InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(rdoId), m_pixelId->eta_index(rdoId) );
            for( int ii=0; ii<nCells && iter==pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
	  if( iter!=pixelSimDataMap->end() ) GetTruthInformation(iter, parentMask, bestExtcode, bestParent);
        } // end if pixel truth available
        ++hitIndex;

        // push back the hit information  to DataInput for HitList
	HTTHit tmpSGhit;
        tmpSGhit.setHitType(HitType::unmapped);
        tmpSGhit.setDetType(SiliconTech::pixel);
        tmpSGhit.setIdentifierHash(sielement->identifyHash());

	int barrel_ec = m_pixelId->barrel_ec(rdoId);
	if (barrel_ec == 0)
          tmpSGhit.setDetectorZone(DetectorZone::barrel);
	else if (barrel_ec == 2 || barrel_ec == -2)
          tmpSGhit.setDetectorZone(DetectorZone::endcap);

        tmpSGhit.setLayerDisk( m_pixelId->layer_disk(rdoId));
        tmpSGhit.setPhiModule(m_pixelId->phi_module(rdoId));
        tmpSGhit.setEtaModule(m_pixelId->eta_module(rdoId));
        tmpSGhit.setPhiIndex(m_pixelId->phi_index(rdoId));
        tmpSGhit.setEtaIndex(m_pixelId->eta_index(rdoId));
        tmpSGhit.setEtaWidth(0);
        tmpSGhit.setPhiWidth(0);
	tmpSGhit.setX(globalPos[Amg::x]);
	tmpSGhit.setY(globalPos[Amg::y]);
	tmpSGhit.setZ(globalPos[Amg::z]);
        tmpSGhit.setToT((*iRDO)->getToT());
        tmpSGhit.setEventIndex((long)(bestParent ? bestExtcode.eventIndex() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcodePt( static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)) );
        tmpSGhit.setParentageMask(parentMask.to_ulong());

	// Add truth
	HTTMultiTruth mt;
	HTTMultiTruth::Barcode uniquecode(tmpSGhit.getEventIndex(),tmpSGhit.getBarcode());
	mt.maximize(uniquecode,tmpSGhit.getBarcodePt());
	tmpSGhit.setTruth(mt);

	m_eventHeader->addHit(tmpSGhit);
      } // end for each RDO in the collection
    } // for each pixel RDO collection
  } // dump raw pixel data

  return StatusCode::SUCCESS;
}

StatusCode
HTT_SGToRawHitsTool::ReadStripSimulation( HitIndexMap& hitIndexMap, unsigned int &hitIndex) {

  const InDetSimDataCollection* sctSimDataMap(0);
  const bool have_sct_sdo = evtStore()->retrieve(sctSimDataMap, "SCT_SDO_Map").isSuccess();
  if (!have_sct_sdo) {
    ATH_MSG_WARNING ( "Missing SCT SDO Map");
  }
  ATH_MSG_INFO ("Found SCT SDO Map");

  const DataHandle<SCT_RDO_Container> sct_rdocontainer_iter;
  if( evtStore()->retrieve(sct_rdocontainer_iter, "SCT_RDOs").isSuccess() ) {
    sct_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( SCT_RDO_Container::const_iterator iColl=sct_rdocontainer_iter->begin(), fColl=sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
      if( !SCT_Collection ) { continue; }
      for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
        const Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
	Amg::Vector2D LocalPos = sielement->localPositionOfCell(rdoId);
	std::pair<Amg::Vector3D, Amg::Vector3D> endsOfStrip = sielement->endsOfStrip(LocalPos);

        hitIndexMap[rdoId] = hitIndex;
        ++hitIndex;
        // if there is simulation truth available, try to retrieve the
        // "most likely" barcode for this strip.
        const HepMC::GenParticle* bestParent = 0;
	TrigHTTInputUtils::ParentBitmask parentMask;
        HepMcParticleLink::ExtendedBarCode bestExtcode;
        if( have_sct_sdo && sctSimDataMap ) {
          InDetSimDataCollection::const_iterator iter( sctSimDataMap->find(rdoId) );
          // if SDO found for this strip, associate the particle
          if( iter!=sctSimDataMap->end() ) GetTruthInformation(iter, parentMask, bestExtcode, bestParent);
        } // end if sct truth available
        // push back the hit information  to DataInput for HitList , copy from RawInput.cxx
	
	HTTHit tmpSGhit;
        tmpSGhit.setHitType(HitType::unmapped);
        tmpSGhit.setDetType(SiliconTech::strip);
        tmpSGhit.setIdentifierHash(sielement->identifyHash());
	
	int barrel_ec = m_sctId->barrel_ec(rdoId);
	if (barrel_ec == 0)
          tmpSGhit.setDetectorZone(DetectorZone::barrel);
	else if (barrel_ec == 2 || barrel_ec == -2)
          tmpSGhit.setDetectorZone(DetectorZone::endcap);
	
        tmpSGhit.setLayerDisk(m_sctId->layer_disk(rdoId));
        tmpSGhit.setPhiModule(m_sctId->phi_module(rdoId));
        tmpSGhit.setEtaModule(m_sctId->eta_module(rdoId));
        tmpSGhit.setPhiIndex(m_sctId->strip(rdoId));
        tmpSGhit.setEtaIndex(m_sctId->row(rdoId));
	tmpSGhit.setSide(m_sctId->side(rdoId));
        tmpSGhit.setEtaWidth((*iRDO)->getGroupSize());
        tmpSGhit.setPhiWidth(0);
        tmpSGhit.setEventIndex((long)(bestParent ? bestExtcode.eventIndex() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
        tmpSGhit.setBarcodePt( static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)) );
        tmpSGhit.setParentageMask(parentMask.to_ulong());
	tmpSGhit.setX(0.5*(endsOfStrip.first.x() + endsOfStrip.second.x()));
	tmpSGhit.setY(0.5*(endsOfStrip.first.y() + endsOfStrip.second.y()));
	tmpSGhit.setZ(0.5*(endsOfStrip.first.z() + endsOfStrip.second.z()));

	// Add truth
	HTTMultiTruth mt;
	HTTMultiTruth::Barcode uniquecode(tmpSGhit.getEventIndex(),tmpSGhit.getBarcode());
	mt.maximize(uniquecode,tmpSGhit.getBarcodePt());
	tmpSGhit.setTruth(mt);

	m_eventHeader->addHit(tmpSGhit);
      } // end for each RDO in the strip collection
    } // end for each strip RDO collection
    // dump all RDO's and SDO's for a given event, for debugging purposes
  } // end dump raw SCT data

  return StatusCode::SUCCESS;
}


StatusCode
HTT_SGToRawHitsTool::DumpPixelClusters(HitIndexMap& pixelClusterIndexMap ) {
  unsigned int pixelClusterIndex=0;
  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = evtStore()->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if (!have_pixel_sdo) {
    ATH_MSG_WARNING ( "Missing Pixel SDO Map");
  }

  // Dump pixel clusters. They're in m_pixelContainer
  m_pixelContainer->clID(); // anything to dereference the DataHandle
  for( InDet::SiClusterContainer::const_iterator iColl=m_pixelContainer->begin(), fColl=m_pixelContainer->end(); iColl!=fColl; ++iColl ) {
    const InDet::SiClusterCollection* pixelClusterCollection(*iColl);
    if( !pixelClusterCollection ) {
      ATH_MSG_DEBUG ( "pixelClusterCollection not available!");
      continue;
    }

    for( DataVector<InDet::SiCluster>::const_iterator iCluster=pixelClusterCollection->begin(), fCluster=pixelClusterCollection->end(); iCluster!=fCluster; ++iCluster ) {
      Identifier theId = (*iCluster)->identify();

      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      const HepMC::GenParticle* bestParent = 0;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if( have_pixel_sdo && pixelSimDataMap ) {
        for( std::vector<Identifier>::const_iterator rdoIter = (*iCluster)->rdoList().begin();
            rdoIter != (*iCluster)->rdoList().end(); rdoIter++ ) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(*rdoIter);
          assert( sielement);
	  InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier (*rdoIter);

	  const int nCells = sielement->numberOfConnectedCells(cellID);
          InDetSimDataCollection::const_iterator iter( pixelSimDataMap->find(*rdoIter) );
          // this might be the ganged pixel copy.
          if( nCells>1 && iter==pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(*rdoIter), m_pixelId->eta_index(*rdoIter) );
            for( int ii=0; ii<nCells && iter==pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if( iter!=pixelSimDataMap->end() ) GetTruthInformation(iter, parentMask, bestExtcode, bestParent);
	} // if we have pixel sdo's available
      }
      pixelClusterIndexMap[theId] = pixelClusterIndex;
      pixelClusterIndex++;
    } // End loop over pixel clusters
  } // End loop over pixel cluster collection

  return StatusCode::SUCCESS;
}

StatusCode
HTT_SGToRawHitsTool::read_offline_clusters(HTTOptionalEventInfo& optional)
{

  //Lets do the Pixel clusters first
  //Loopover the pixel clusters and convert them into a HTTCluster for storage
  // Dump pixel clusters. They're in m_pixelContainer

  const InDetSimDataCollection* pixelSimDataMap(0);
  const bool have_pixel_sdo = evtStore()->retrieve(pixelSimDataMap, "PixelSDO_Map").isSuccess();
  if (!have_pixel_sdo) {
    ATH_MSG_WARNING ( "Missing Pixel SDO Map");
  }

   m_pixelContainer->clID(); // anything to dereference the DataHandle
   optional.reserveOffClust(m_pixelContainer->size());
  for( InDet::SiClusterContainer::const_iterator iColl=m_pixelContainer->begin(), fColl=m_pixelContainer->end(); iColl!=fColl; ++iColl ) {
    const InDet::SiClusterCollection* pixelClusterCollection(*iColl);
    if( !pixelClusterCollection ) {
      ATH_MSG_DEBUG ( "pixelClusterCollection not available!");
      continue;
    }
    const int size = pixelClusterCollection->size();
    ATH_MSG_DEBUG ( "PixelClusterCollection found with " << size << " clusters");
    for( DataVector<InDet::SiCluster>::const_iterator iCluster=pixelClusterCollection->begin(), fCluster=pixelClusterCollection->end(); iCluster!=fCluster; ++iCluster ) {

      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      const HepMC::GenParticle* bestParent = 0;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if( have_pixel_sdo && pixelSimDataMap ) {
        for( std::vector<Identifier>::const_iterator rdoIter = (*iCluster)->rdoList().begin();
            rdoIter != (*iCluster)->rdoList().end(); rdoIter++ ) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(*rdoIter);
          assert( sielement);
	  InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier (*rdoIter);
          const int nCells = sielement->numberOfConnectedCells(cellID);
          InDetSimDataCollection::const_iterator iter( pixelSimDataMap->find(*rdoIter) );
          // this might be the ganged pixel copy.
          if( nCells>1 && iter==pixelSimDataMap->end() ) {
            InDetDD::SiReadoutCellId SiRC( m_pixelId->phi_index(*rdoIter), m_pixelId->eta_index(*rdoIter) );
            for( int ii=0; ii<nCells && iter==pixelSimDataMap->end(); ++ii ) {
              iter = pixelSimDataMap->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC,ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if( iter!=pixelSimDataMap->end() ) GetTruthInformation(iter, parentMask, bestExtcode, bestParent);
	} // if we have pixel sdo's available
      }

      Identifier theID = (*iCluster)->identify();
      //cluster object to be written out
      HTTCluster clusterOut;
      //Rawhit object to represent the cluster
      HTTHit clusterEquiv;
      //Lets get the information of this pixel cluster
      const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(theID);
      assert( sielement);
      const InDetDD::SiLocalPosition localPos = sielement->localPositionOfCell(theID);
      const Amg::Vector3D globalPos( sielement->globalPosition(localPos) );
      clusterEquiv.setHitType(HitType::clustered);
      clusterEquiv.setX(globalPos.x());
      clusterEquiv.setY(globalPos.y());
      clusterEquiv.setZ(globalPos.z());
      clusterEquiv.setDetType(SiliconTech::pixel);
      clusterEquiv.setIdentifierHash(sielement->identifyHash());

      int barrel_ec = m_pixelId->barrel_ec(theID);
      if (barrel_ec == 0)
          clusterEquiv.setDetectorZone(DetectorZone::barrel);
      else if (barrel_ec == 2 || barrel_ec == -2)
          clusterEquiv.setDetectorZone(DetectorZone::endcap);

      clusterEquiv.setLayerDisk(m_pixelId->layer_disk(theID));
      clusterEquiv.setPhiModule(m_pixelId->phi_module(theID));
      clusterEquiv.setEtaModule(m_pixelId->eta_module(theID));
      clusterEquiv.setPhiIndex(m_pixelId->phi_index(theID));
      clusterEquiv.setEtaIndex(m_pixelId->eta_index(theID));

      clusterEquiv.setPhiWidth((*iCluster)->width().colRow()[1]);
      clusterEquiv.setEtaWidth((*iCluster)->width().colRow()[0]);
      //Save the truth here as the MultiTruth object is only transient
      clusterEquiv.setEventIndex((long)(bestParent ? bestExtcode.eventIndex() : std::numeric_limits<long>::max()));
      clusterEquiv.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
      clusterEquiv.setBarcodePt( static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)) );
      clusterEquiv.setParentageMask(parentMask.to_ulong());
      clusterOut.setClusterEquiv(clusterEquiv);
      optional.addOfflineCluster(clusterOut);
    }
  }

  //Now lets do the strip clusters
  //Loopover the pixel clusters and convert them into a HTTCluster for storage
  // Dump pixel clusters. They're in m_pixelContainer
  const InDetSimDataCollection* sctSimDataMap(0);
  const bool have_sct_sdo = evtStore()->retrieve(sctSimDataMap, "SCT_SDO_Map").isSuccess();
  if (!have_sct_sdo) {
    ATH_MSG_WARNING ( "Missing SCT SDO Map");
  }
  ATH_MSG_INFO ("Found SCT SDO Map");

  const DataHandle<SCT_RDO_Container> sct_rdocontainer_iter;
  if( evtStore()->retrieve(sct_rdocontainer_iter, "SCT_RDOs").isSuccess() ) {
    sct_rdocontainer_iter->clID(); // anything to dereference the DataHandle
    for( SCT_RDO_Container::const_iterator iColl=sct_rdocontainer_iter->begin(), fColl=sct_rdocontainer_iter->end(); iColl!=fColl; ++iColl ) {
      const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection(*iColl);
      if( !SCT_Collection ) { continue; }
      for( DataVector<SCT_RDORawData>::const_iterator iRDO=SCT_Collection->begin(), fRDO=SCT_Collection->end(); iRDO!=fRDO; ++iRDO ) {
        const Identifier rdoId = (*iRDO)->identify();
        // get the det element from the det element collection
        const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
        const InDetDD::SiDetectorDesign& design = dynamic_cast<const InDetDD::SiDetectorDesign&>(sielement->design());
        const InDetDD::SiLocalPosition localPos = design.localPositionOfCell(m_sctId->strip(rdoId));
        const Amg::Vector3D gPos = sielement->globalPosition(localPos);
        // if there is simulation truth available, try to retrieve the
        // "most likely" barcode for this strip.
        const HepMC::GenParticle* bestParent = 0;
	TrigHTTInputUtils::ParentBitmask parentMask;
        HepMcParticleLink::ExtendedBarCode bestExtcode;
        if( have_sct_sdo && sctSimDataMap ) {
          InDetSimDataCollection::const_iterator iter( sctSimDataMap->find(rdoId) );
          // if SDO found for this pixel, associate the particle
          if( iter!=sctSimDataMap->end() ) GetTruthInformation(iter, parentMask, bestExtcode, bestParent);
        } // end if sct truth available
	
        // push back the hit information  to DataInput for HitList , copy from RawInput.cxx
	HTTCluster clusterOut;
	HTTHit clusterEquiv;
        clusterEquiv.setHitType(HitType::clustered);
        clusterEquiv.setX(gPos.x());
        clusterEquiv.setY(gPos.y());
        clusterEquiv.setZ(gPos.z());
        clusterEquiv.setDetType(SiliconTech::strip);
        clusterEquiv.setIdentifierHash(sielement->identifyHash());

	int barrel_ec = m_sctId->barrel_ec(rdoId);
	if (barrel_ec == 0)
	  clusterEquiv.setDetectorZone(DetectorZone::barrel);
	else if (barrel_ec == 2 || barrel_ec == -2)
	  clusterEquiv.setDetectorZone(DetectorZone::endcap);
	
        clusterEquiv.setLayerDisk(m_sctId->layer_disk(rdoId));
        clusterEquiv.setPhiModule(m_sctId->phi_module(rdoId));
        clusterEquiv.setEtaModule(m_sctId->eta_module(rdoId));
        clusterEquiv.setPhiIndex(m_sctId->strip(rdoId));
        clusterEquiv.setEtaIndex(m_sctId->row(rdoId));
        clusterEquiv.setSide(m_sctId->side(rdoId));
	//I think this is the strip "cluster" width
        clusterEquiv.setPhiWidth((*iRDO)->getGroupSize());
	//Save the truth here as the MultiTruth object is only transient
        clusterEquiv.setEventIndex((long)(bestParent ? bestExtcode.eventIndex() : std::numeric_limits<long>::max()));
        clusterEquiv.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
        clusterEquiv.setBarcodePt( static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)) );
        clusterEquiv.setParentageMask(parentMask.to_ulong());
	clusterOut.setClusterEquiv(clusterEquiv);
	optional.addOfflineCluster(clusterOut);
      } // end for each RDO in the strip collection
    } // end for each strip RDO collection
    // dump all RDO's and SDO's for a given event, for debugging purposes
  } // end dump raw SCT data


  return StatusCode::SUCCESS;
}

StatusCode
HTT_SGToRawHitsTool::read_truth_tracks(HTTOptionalEventInfo& optional)
{

  // retrieve truth tracks from athena
  const McEventCollection* SimTracks = 0;
  if( evtStore()->retrieve(SimTracks,"TruthEvent").isFailure() ) {
    std::string key = "G4Truth";
    if( evtStore()->retrieve(SimTracks,key).isFailure() ) {
      key = "";
      if( evtStore()->retrieve(SimTracks,key).isFailure() ) {
        ATH_MSG_ERROR ( "could not find the McEventCollection truth tracks");
        return StatusCode::FAILURE;
      }
    }
  }

  ATH_MSG_INFO ("Dump truth tracks, size "<<SimTracks->size()<<" "<< (SimTracks ? SimTracks->size() : 0u));


  // dump each truth track
  optional.reserveOffClust(SimTracks->size());
  for( unsigned int ievt=0;  ievt<SimTracks->size(); ++ievt ) {

    const HepMC::GenEvent* genEvent = SimTracks->at( ievt );
    // retrieve the primary interaction vertex here. for now, use the dummy origin.
    HepGeom::Point3D<double>  primaryVtx(0.,0.,0.);
    // the event should have signal process vertex unless it was generated as single particles.
    // if it exists, use it for the primary vertex.
    if( genEvent->signal_process_vertex() ) {
       primaryVtx.set( genEvent->signal_process_vertex()->point3d().x(),
                       genEvent->signal_process_vertex()->point3d().y(),
                       genEvent->signal_process_vertex()->point3d().z() );
       ATH_MSG_DEBUG ("using signal process vertex for eventIndex " << ievt << ":"
                      << primaryVtx.x() << "\t" << primaryVtx.y()  << "\t" << primaryVtx.z());
    }

    for( HepMC::GenEvent::particle_const_iterator it=genEvent->particles_begin(), ft=genEvent->particles_end(); it!=ft; ++it ) {
       const HepMC::GenParticle* const particle( *it );
       const int pdgcode = particle->pdg_id();
       // reject generated particles without a production vertex.
       if( !particle->production_vertex() ) {
          continue;
       }
       // reject neutral or unstable particles
       const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgcode));
       if( !pd ) {
          continue;
       }
       float charge = pd->charge();
       if (pdgcode < 0) charge *= -1.; // since we took absolute value above

       if( std::abs(charge)<0.5 ) {
          continue;
       }
       if( particle->status()%1000!=1 ) {
          continue;
       }

       // truth-to-track tool
       const Amg::Vector3D momentum( particle->momentum().px(), particle->momentum().py(), particle->momentum().pz());
       const Amg::Vector3D position( particle->production_vertex()->position().x(), particle->production_vertex()->position().y(), particle->production_vertex()->position().z());
       const Trk::CurvilinearParameters cParameters( position, momentum, charge);

       Trk::PerigeeSurface persf;
       if (m_UseNominalOrigin) {
          Amg::Vector3D    origin(0,0,0);
          persf = Trk::PerigeeSurface(origin);
       }
       else {
          persf = m_beamSpotSvc->beamPos();
       }

      const Trk::TrackParameters* tP = m_extrapolator->extrapolate(cParameters, persf, Trk::anyDirection, false);
      const double track_truth_d0 = tP ? tP->parameters()[Trk::d0] : 999.;
      const double track_truth_phi = tP ? tP->parameters()[Trk::phi] : 999.;
      const double track_truth_p = (tP && fabs(tP->parameters()[Trk::qOverP]) > 1.e-8) ?
        tP->charge()/tP->parameters()[Trk::qOverP] : 10E7;
      const double track_truth_x0 = tP ? tP->position().x() : 999.;
      const double track_truth_y0 = tP ? tP->position().y() : 999.;
      const double track_truth_z0 = tP ? tP->parameters()[Trk::z0] : 999.;
      const double track_truth_q = tP ? tP->charge() : 0.;
      const double track_truth_sinphi = tP ? std::sin(tP->parameters()[Trk::phi]) : -1.;
      const double track_truth_cosphi = tP ? std::cos(tP->parameters()[Trk::phi]) : -1.;
      const double track_truth_sintheta = tP ? std::sin(tP->parameters()[Trk::theta]) : -1.;
      const double track_truth_costheta = tP ? std::cos(tP->parameters()[Trk::theta]) : -1.;
      double truth_d0corr = track_truth_d0-( primaryVtx.y()*cos(track_truth_phi)-primaryVtx.x()*sin(track_truth_phi) );
      double truth_zvertex = 0.;
      delete tP;

      const HepGeom::Point3D<double> startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
      // categorize particle (prompt, secondary, etc.) based on InDetPerformanceRTT/detector paper criteria.
      bool isPrimary = true;
      if( std::abs(truth_d0corr)>2. ) { isPrimary=false;}
      if( particle->barcode()>10000000 || particle->barcode()==0 ) { isPrimary=false;}

      if( isPrimary && particle->production_vertex() ) {
        const HepGeom::Point3D<double> startVertex(particle->production_vertex()->point3d().x(), particle->production_vertex()->point3d().y(), particle->production_vertex()->point3d().z());
        if( std::abs(startVertex.z() - truth_zvertex)>100. ) { isPrimary=false;}
        if( particle->end_vertex() ) {
          HepGeom::Point3D<double> endVertex(particle->end_vertex()->point3d().x(), particle->end_vertex()->point3d().y(), particle->end_vertex()->point3d().z());
          if( endVertex.perp() < HTT_PT_TRUTHMIN && std::abs(endVertex.z()) < HTT_Z_TRUTHMIN ) { isPrimary=false; }
        }
      } else {
        isPrimary = false;
      }

      HepMcParticleLink::ExtendedBarCode extBarcode2( particle->barcode(), ievt );

      HTTTruthTrack tmpSGTrack;
      tmpSGTrack.setX(track_truth_x0);
      tmpSGTrack.setY(track_truth_y0);
      tmpSGTrack.setZ(track_truth_z0);
      tmpSGTrack.setD0(track_truth_d0);
      tmpSGTrack.setVtxZ(primaryVtx.z());
      tmpSGTrack.setQ(track_truth_q);
      tmpSGTrack.setPX(track_truth_p*(track_truth_cosphi*track_truth_sintheta));
      tmpSGTrack.setPY(track_truth_p*(track_truth_sinphi*track_truth_sintheta));
      tmpSGTrack.setPZ(track_truth_p*track_truth_costheta);
      tmpSGTrack.setPDGCode(pdgcode);
      tmpSGTrack.setBarcode(extBarcode2.barcode());
      tmpSGTrack.setEventIndex(extBarcode2.eventIndex());
      optional.addTruthTrack(tmpSGTrack);
    } // end for each GenParticle in this GenEvent
  } // end for each GenEvent


  return StatusCode::SUCCESS;
}

void HTT_SGToRawHitsTool::GetTruthInformation(InDetSimDataCollection::const_iterator &iter, TrigHTTInputUtils::ParentBitmask &parentMask, HepMcParticleLink::ExtendedBarCode &bestExtcode, const HepMC::GenParticle* bestParent) {
  const InDetSimData& sdo( iter->second );
  const std::vector<InDetSimData::Deposit>& deposits( sdo.getdeposits() );
  for( std::vector<InDetSimData::Deposit>::const_iterator iDep=deposits.begin(), fDep=deposits.end(); iDep!=fDep; ++iDep ) {
    const HepMcParticleLink& particleLink( iDep->first );
    // RDO's without SDO's are delta rays or detector noise.
    if( !particleLink.isValid() ) { continue; }
    const HepMC::GenParticle* particle( particleLink );
    const float genEta=particle->momentum().pseudoRapidity();
    const float genPt=particle->momentum().perp(); // MeV
    // reject unstable particles
    if( particle->status()%1000!=1 ) { continue; }
    // reject secondaries and low pT (<400 MeV) pileup
    if( particle->barcode()>10000000 || particle->barcode()==0 ) { continue; }
    // reject far forward particles
    if( fabs(genEta)>m_maxEta ) { continue; }
    // "bestParent" is the highest pt particle
    if( !bestParent || bestParent->momentum().perp()<genPt ) {
      bestParent = particle;
      bestExtcode = HepMcParticleLink::ExtendedBarCode( particleLink.barcode() , particleLink.eventIndex() );
    }
    parentMask |= TrigHTTInputUtils::construct_truth_bitmap( particle );
    // check SDO
  } // end for each contributing particle
}
