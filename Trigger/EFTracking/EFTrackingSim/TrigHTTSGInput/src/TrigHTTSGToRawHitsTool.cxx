/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigHTTObjects/HTTEventInputHeader.h"
#include "TrigHTTObjects/HTTEventInfo.h"
#include "TrigHTTObjects/HTTOfflineTrack.h"
#include "TrigHTTObjects/HTTOfflineHit.h"
#include "TrigHTTObjects/HTTTruthTrack.h"
#include "TrigHTTObjects/HTTHit.h"

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

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "InDetRIO_OnTrack/SiClusterOnTrack.h"

#include "GaudiKernel/IPartPropSvc.h"
#include "TrigHTTSGToRawHitsTool.h"

namespace {
  // A few constants for truth cuts
  const float HTT_PT_TRUTHMIN = 400.;
  const float HTT_Z_TRUTHMIN = 2300.;
}

TrigHTTSGToRawHitsTool::TrigHTTSGToRawHitsTool(const std::string& algname, const std::string& name, const IInterface* ifc) :
  base_class(algname, name, ifc)
{}

StatusCode TrigHTTSGToRawHitsTool::initialize() {

  ATH_MSG_DEBUG("TrigHTTSGToRawHitsTool::initialize()");

  ATH_CHECK(m_truthToTrack.retrieve());
  ATH_CHECK(m_extrapolator.retrieve());
  ATH_CHECK(m_beamSpotKey.initialize());


  IPartPropSvc* partPropSvc = nullptr;
  ATH_CHECK(service("PartPropSvc", partPropSvc));
  m_particleDataTable = partPropSvc->PDT();


  const IdDictManager* idDictMgr = nullptr;
  ATH_CHECK(detStore()->retrieve(idDictMgr, "IdDict"));

  // ID helpers
  m_idHelper = new AtlasDetectorID;
  if (idDictMgr->initializeHelper(*m_idHelper)) {
    ATH_MSG_ERROR("Unable to initialize ID IdHelper");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(detStore()->retrieve(m_PIX_mgr, "ITkPixel"));
  ATH_CHECK(detStore()->retrieve(m_pixelId, "PixelID"));
  ATH_CHECK(detStore()->retrieve(m_SCT_mgr, "ITkStrip"));
  ATH_CHECK(detStore()->retrieve(m_sctId, "SCT_ID"));

  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_pixelClusterContainerKey.initialize());
  ATH_CHECK(m_sctClusterContainerKey.initialize());

  ATH_CHECK(m_offlineTracksKey.initialize());
  ATH_CHECK(m_mcCollectionKey.initialize());
  ATH_CHECK(m_pixelSDOKey.initialize());
  ATH_CHECK(m_stripSDOKey.initialize());
  ATH_CHECK(m_pixelRDOKey.initialize());
  ATH_CHECK(m_stripRDOKey.initialize());

  ATH_MSG_DEBUG("Initialization complete");
  return StatusCode::SUCCESS;
}


StatusCode TrigHTTSGToRawHitsTool::finalize() {
  if (m_idHelper) { delete m_idHelper; }
  return StatusCode::SUCCESS;
}


/** This function get from the SG the inner detector raw hits
  and prepares them for HTT simulation */
StatusCode TrigHTTSGToRawHitsTool::readData(HTTEventInputHeader* header, const EventContext& eventContext)
{
  m_eventHeader = header; //take the external pointer
  auto eventInfo = SG::makeHandle(m_eventInfoKey, eventContext);
  //Filled to variable / start event
  HTTEventInfo event_info;
  event_info.setRunNumber(eventInfo->runNumber());
  event_info.setEventNumber(eventInfo->eventNumber());
  event_info.setLB(eventInfo->lumiBlock());
  event_info.setBCID(eventInfo->bcid());
  event_info.setaverageInteractionsPerCrossing(eventInfo->averageInteractionsPerCrossing());
  event_info.setactualInteractionsPerCrossing(eventInfo->actualInteractionsPerCrossing());

  event_info.setextendedLevel1ID(eventInfo->extendedLevel1ID());
  event_info.setlevel1TriggerType(eventInfo->level1TriggerType());
  //  event_info.setlevel1TriggerInfo(eventInfo->level1TriggerInfo ()); // unclear if needed, TODO come back to it

  m_eventHeader->newEvent(event_info);//this also reset all variables

  HitIndexMap hitIndexMap; // keep running index event-unique to each hit
  HitIndexMap pixelClusterIndexMap;

  // get pixel and sct cluster containers

  // dump raw silicon data
  ATH_MSG_DEBUG("Dump raw silicon data");
  ATH_CHECK(readRawSilicon(hitIndexMap, pixelClusterIndexMap, eventContext));

  ATH_MSG_DEBUG("Found list of hits, size =" << m_eventHeader->nHits());
  HTTOptionalEventInfo optional;

  if (m_readOfflineClusters) {
    std::vector <HTTCluster> clusters;
    ATH_CHECK(readOfflineClusters(clusters, eventContext));
    for (auto cluster : clusters) optional.addOfflineCluster(cluster);
    ATH_MSG_DEBUG("Saved " << optional.nOfflineClusters() << " offline clusters");
  }

  if (m_readTruthTracks) {
    std::vector <HTTTruthTrack> truth;
    ATH_CHECK(readTruthTracks(truth, eventContext));
    for (const HTTTruthTrack& trk : truth) optional.addTruthTrack(trk);
    ATH_MSG_DEBUG("Saved " << optional.nTruthTracks() << " truth tracks");
  }

  std::vector <HTTOfflineTrack> offline;
  if (m_readOfflineTracks) {
    ATH_CHECK(readOfflineTracks(offline, eventContext));
    for (const HTTOfflineTrack& trk : offline) optional.addOfflineTrack(trk);
    ATH_MSG_DEBUG("Saved " << optional.nOfflineTracks() << " offline tracks");
  }


  m_eventHeader->setOptional(optional);
  ATH_MSG_DEBUG(*m_eventHeader);
  ATH_MSG_DEBUG("End of execute()");
  return StatusCode::SUCCESS;
}


StatusCode TrigHTTSGToRawHitsTool::readOfflineTracks(std::vector<HTTOfflineTrack>& offline, const EventContext& eventContext)
{
  auto offlineTracksHandle = SG::makeHandle(m_offlineTracksKey, eventContext);
  ATH_MSG_DEBUG("read Offline tracks, size= " << offlineTracksHandle->size());

  int iTrk = -1;
  for (const xAOD::TrackParticle* trackParticle : *offlineTracksHandle) {
    iTrk++;
    HTTOfflineTrack tmpOfflineTrack;
    tmpOfflineTrack.setQOverPt(trackParticle->pt() > 0 ? trackParticle->charge() / trackParticle->pt() : 0);
    tmpOfflineTrack.setEta(trackParticle->eta());
    tmpOfflineTrack.setPhi(trackParticle->phi());

    const DataVector<const Trk::TrackStateOnSurface>* trackStates = trackParticle->track()->trackStateOnSurfaces();
    if (trackStates == nullptr) {
      ATH_MSG_ERROR("missing trackStatesOnSurface");
      return StatusCode::FAILURE;
    }
    for (const Trk::TrackStateOnSurface* tsos : *trackStates) {
      if (tsos == nullptr) continue;
      if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {
        const Trk::MeasurementBase* measurement = tsos->measurementOnTrack();
        if (tsos->trackParameters() != nullptr &&
          tsos->trackParameters()->associatedSurface().associatedDetectorElement() != nullptr &&
          tsos->trackParameters()->associatedSurface().associatedDetectorElement()->identify() != 0
          ) {
          const Trk::RIO_OnTrack* hit = dynamic_cast <const Trk::RIO_OnTrack*>(measurement);
          const Identifier& hitId = hit->identify();
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
    offline.push_back(tmpOfflineTrack);
  }//end of loop over tracks


  return StatusCode::SUCCESS;
}



// dump silicon channels with geant matching information.
StatusCode
TrigHTTSGToRawHitsTool::readRawSilicon(HitIndexMap& hitIndexMap, HitIndexMap& pixelClusterIndexMap, const EventContext& eventContext) // const cannot make variables push back to DataInput
{
  ATH_MSG_DEBUG("read silicon hits");
  unsigned int hitIndex = 0u;

  ATH_CHECK(readPixelSimulation(hitIndexMap, hitIndex, eventContext));
  ATH_CHECK(readStripSimulation(hitIndexMap, hitIndex, eventContext));
  ATH_CHECK(dumpPixelClusters(pixelClusterIndexMap, eventContext));

  return StatusCode::SUCCESS;
}


StatusCode
TrigHTTSGToRawHitsTool::readPixelSimulation(HitIndexMap& hitIndexMap, unsigned int& hitIndex, const EventContext& eventContext) {

  auto pixelSDOHandle = SG::makeHandle(m_pixelSDOKey, eventContext);
  auto pixelRDOHandle = SG::makeHandle(m_pixelRDOKey, eventContext);

  ATH_MSG_DEBUG("Found Pixel SDO Map");

  for (const InDetRawDataCollection<PixelRDORawData>* pixel_rdoCollection : *pixelRDOHandle) {
    if (pixel_rdoCollection == nullptr) { continue; }
    // loop on all RDOs
    for (const PixelRDORawData* pixelRawData : *pixel_rdoCollection) {
      Identifier rdoId = pixelRawData->identify();
      // get the det element from the det element collection
      const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId); assert(sielement);

      Amg::Vector2D LocalPos = sielement->rawLocalPositionOfCell(rdoId);
      Amg::Vector3D globalPos = sielement->globalPosition(LocalPos);
      InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier(rdoId);

      // update map between pixel identifier and event-unique hit index.
      // ganged pixels (nCells==2) get two entries.
      hitIndexMap[rdoId] = hitIndex;
      const int nCells = sielement->numberOfConnectedCells(cellID);
      if (nCells == 2) {
        const InDetDD::SiCellId tmpCell = sielement->connectedCell(cellID, 1);
        const Identifier tmpId = sielement->identifierFromCellId(tmpCell);
        hitIndexMap[tmpId] = hitIndex; // add second entry for ganged pixel ID
      }
      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel.
      HepMC::ConstGenParticlePtr bestParent = nullptr;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if (!m_pixelSDOKey.empty()) {
        InDetSimDataCollection::const_iterator iter(pixelSDOHandle->find(rdoId));
        if (nCells > 1 && iter == pixelSDOHandle->end()) {
          InDetDD::SiReadoutCellId SiRC(m_pixelId->phi_index(rdoId), m_pixelId->eta_index(rdoId));
          for (int ii = 0; ii < nCells && iter == pixelSDOHandle->end(); ++ii) {
            iter = pixelSDOHandle->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC, ii)));
          }
        } // end search for correct ganged pixel
        // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
        if (iter != pixelSDOHandle->end()) getTruthInformation(iter, parentMask, bestExtcode, bestParent);
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
      else if (barrel_ec == 2)
        tmpSGhit.setDetectorZone(DetectorZone::posEndcap);
      else if (barrel_ec == -2)
        tmpSGhit.setDetectorZone(DetectorZone::negEndcap);

      tmpSGhit.setLayerDisk(m_pixelId->layer_disk(rdoId));
      tmpSGhit.setPhiModule(m_pixelId->phi_module(rdoId));
      tmpSGhit.setEtaModule(m_pixelId->eta_module(rdoId));
      tmpSGhit.setPhiIndex(m_pixelId->phi_index(rdoId));
      tmpSGhit.setEtaIndex(m_pixelId->eta_index(rdoId));
      tmpSGhit.setEtaWidth(0);
      tmpSGhit.setPhiWidth(0);
      tmpSGhit.setX(globalPos[Amg::x]);
      tmpSGhit.setY(globalPos[Amg::y]);
      tmpSGhit.setZ(globalPos[Amg::z]);
      tmpSGhit.setToT(pixelRawData->getToT());
      index_type index, position;
      bestExtcode.eventIndex(index, position);
      if (bestParent)
        tmpSGhit.setEventIndex(index);
      else
        tmpSGhit.setEventIndex(std::numeric_limits<long>::max());
      tmpSGhit.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
      tmpSGhit.setBarcodePt(static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)));
      tmpSGhit.setParentageMask(parentMask.to_ulong());

      // Add truth
      HTTMultiTruth mt;
      HTTMultiTruth::Barcode uniquecode(tmpSGhit.getEventIndex(), tmpSGhit.getBarcode());
      mt.maximize(uniquecode, tmpSGhit.getBarcodePt());
      tmpSGhit.setTruth(mt);

      m_eventHeader->addHit(tmpSGhit);
    } // end for each RDO in the collection
  } // for each pixel RDO collection

  return StatusCode::SUCCESS;
}

StatusCode
TrigHTTSGToRawHitsTool::readStripSimulation(HitIndexMap& hitIndexMap, unsigned int& hitIndex, const EventContext& eventContext) {

  auto stripSDOHandle = SG::makeHandle(m_stripSDOKey, eventContext);
  ATH_MSG_DEBUG("Found SCT SDO Map");
  auto stripRDOHandle = SG::makeHandle(m_stripRDOKey, eventContext);
  for (const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection : *stripRDOHandle) {
    if (SCT_Collection == nullptr) { continue; }
    for (const SCT_RDORawData* sctRawData : *SCT_Collection) {
      const Identifier rdoId = sctRawData->identify();
      // get the det element from the det element collection
      const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
      Amg::Vector2D LocalPos = sielement->rawLocalPositionOfCell(rdoId);
      std::pair<Amg::Vector3D, Amg::Vector3D> endsOfStrip = sielement->endsOfStrip(LocalPos);

      hitIndexMap[rdoId] = hitIndex;
      ++hitIndex;
      // if there is simulation truth available, try to retrieve the
      // "most likely" barcode for this strip.
      HepMC::ConstGenParticlePtr bestParent = nullptr;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if (!m_stripSDOKey.empty()) {
        InDetSimDataCollection::const_iterator iter(stripSDOHandle->find(rdoId));
        // if SDO found for this strip, associate the particle
        if (iter != stripSDOHandle->end()) getTruthInformation(iter, parentMask, bestExtcode, bestParent);
      } // end if sct truth available
      // push back the hit information  to DataInput for HitList , copy from RawInput.cxx

      HTTHit tmpSGhit;
      tmpSGhit.setHitType(HitType::unmapped);
      tmpSGhit.setDetType(SiliconTech::strip);
      tmpSGhit.setIdentifierHash(sielement->identifyHash());

      int barrel_ec = m_sctId->barrel_ec(rdoId);
      if (barrel_ec == 0)
        tmpSGhit.setDetectorZone(DetectorZone::barrel);
      else if (barrel_ec == 2)
        tmpSGhit.setDetectorZone(DetectorZone::posEndcap);
      else if (barrel_ec == -2)
        tmpSGhit.setDetectorZone(DetectorZone::negEndcap);

      tmpSGhit.setLayerDisk(m_sctId->layer_disk(rdoId));
      tmpSGhit.setPhiModule(m_sctId->phi_module(rdoId));
      tmpSGhit.setEtaModule(m_sctId->eta_module(rdoId));
      tmpSGhit.setPhiIndex(m_sctId->strip(rdoId));
      tmpSGhit.setEtaIndex(m_sctId->row(rdoId));
      tmpSGhit.setSide(m_sctId->side(rdoId));
      tmpSGhit.setEtaWidth(sctRawData->getGroupSize());
      tmpSGhit.setPhiWidth(0);
      index_type index, position;
      bestExtcode.eventIndex(index, position);
      if (bestParent)
        tmpSGhit.setEventIndex(index);
      else
        tmpSGhit.setEventIndex(std::numeric_limits<long>::max());

      tmpSGhit.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
      tmpSGhit.setBarcodePt(static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)));
      tmpSGhit.setParentageMask(parentMask.to_ulong());
      tmpSGhit.setX(0.5 * (endsOfStrip.first.x() + endsOfStrip.second.x()));
      tmpSGhit.setY(0.5 * (endsOfStrip.first.y() + endsOfStrip.second.y()));
      tmpSGhit.setZ(0.5 * (endsOfStrip.first.z() + endsOfStrip.second.z()));

      // Add truth
      HTTMultiTruth mt;
      HTTMultiTruth::Barcode uniquecode(tmpSGhit.getEventIndex(), tmpSGhit.getBarcode());
      mt.maximize(uniquecode, tmpSGhit.getBarcodePt());
      tmpSGhit.setTruth(mt);

      m_eventHeader->addHit(tmpSGhit);
    } // end for each RDO in the strip collection
  } // end for each strip RDO collection
  // dump all RDO's and SDO's for a given event, for debugging purposes

  return StatusCode::SUCCESS;
}


StatusCode
TrigHTTSGToRawHitsTool::dumpPixelClusters(HitIndexMap& pixelClusterIndexMap, const EventContext& eventContext) {
  unsigned int pixelClusterIndex = 0;
  auto pixelSDOHandle = SG::makeHandle(m_pixelSDOKey, eventContext);
  auto pixelClusterContainerHandle = SG::makeHandle(m_pixelClusterContainerKey, eventContext);
  // Dump pixel clusters. They're in m_pixelContainer
  for (const InDet::SiClusterCollection* pixelClusterCollection : *pixelClusterContainerHandle) {
    if (pixelClusterCollection == nullptr) {
      ATH_MSG_DEBUG("pixelClusterCollection not available!");
      continue;
    }

    for (const InDet::SiCluster* cluster : *pixelClusterCollection) {
      Identifier theId = cluster->identify();
      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      HepMC::ConstGenParticlePtr bestParent = nullptr;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if (!m_pixelSDOKey.empty()) {
        for (const Identifier& rdoId : cluster->rdoList()) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId);
          assert(sielement);
          InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier(rdoId);

          const int nCells = sielement->numberOfConnectedCells(cellID);
          InDetSimDataCollection::const_iterator iter(pixelSDOHandle->find(rdoId));
          // this might be the ganged pixel copy.
          if (nCells > 1 && iter == pixelSDOHandle->end()) {
            InDetDD::SiReadoutCellId SiRC(m_pixelId->phi_index(rdoId), m_pixelId->eta_index(rdoId));
            for (int ii = 0; ii < nCells && iter == pixelSDOHandle->end(); ++ii) {
              iter = pixelSDOHandle->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC, ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if (iter != pixelSDOHandle->end()) getTruthInformation(iter, parentMask, bestExtcode, bestParent);
        } // if we have pixel sdo's available
      }
      pixelClusterIndexMap[theId] = pixelClusterIndex;
      pixelClusterIndex++;
    } // End loop over pixel clusters
  } // End loop over pixel cluster collection

  return StatusCode::SUCCESS;
}

StatusCode
TrigHTTSGToRawHitsTool::readOfflineClusters(std::vector <HTTCluster>& clusters, const EventContext& eventContext)
{

  //Lets do the Pixel clusters first
  //Loopover the pixel clusters and convert them into a HTTCluster for storage
  // Dump pixel clusters. They're in m_pixelContainer
  auto pixelSDOHandle = SG::makeHandle(m_pixelSDOKey, eventContext);
  auto pixelClusterContainerHandler = SG::makeHandle(m_pixelClusterContainerKey, eventContext);
  for (const InDet::SiClusterCollection* pixelClusterCollection : *pixelClusterContainerHandler) {
    if (pixelClusterCollection == nullptr) {
      ATH_MSG_DEBUG("pixelClusterCollection not available!");
      continue;
    }
    const int size = pixelClusterCollection->size();
    ATH_MSG_DEBUG("PixelClusterCollection found with " << size << " clusters");
    for (const InDet::SiCluster* cluster : *pixelClusterCollection) {

      // if there is simulation truth available, try to retrieve the "most likely" barcode for this pixel cluster.
      HepMC::ConstGenParticlePtr bestParent = nullptr;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if (!m_pixelSDOKey.empty()) {
        for (const Identifier& rdoId : cluster->rdoList()) {
          const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(rdoId);
          assert(sielement);
          InDetDD::SiCellId cellID = sielement->cellIdFromIdentifier(rdoId);
          const int nCells = sielement->numberOfConnectedCells(cellID);
          InDetSimDataCollection::const_iterator iter(pixelSDOHandle->find(rdoId));
          // this might be the ganged pixel copy.
          if (nCells > 1 && iter == pixelSDOHandle->end()) {
            InDetDD::SiReadoutCellId SiRC(m_pixelId->phi_index(rdoId), m_pixelId->eta_index(rdoId));
            for (int ii = 0; ii < nCells && iter == pixelSDOHandle->end(); ++ii) {
              iter = pixelSDOHandle->find(sielement->identifierFromCellId(sielement->design().connectedCell(SiRC, ii)));
            }
          } // end search for correct ganged pixel
          // if SDO found for this pixel, associate the particle. otherwise leave unassociated.
          if (iter != pixelSDOHandle->end()) getTruthInformation(iter, parentMask, bestExtcode, bestParent);
        } // if we have pixel sdo's available
      }

      Identifier theID = cluster->identify();
      //cluster object to be written out
      HTTCluster clusterOut;
      //Rawhit object to represent the cluster
      HTTHit clusterEquiv;
      //Lets get the information of this pixel cluster
      const InDetDD::SiDetectorElement* sielement = m_PIX_mgr->getDetectorElement(theID);
      assert(sielement);
      const InDetDD::SiLocalPosition localPos = sielement->rawLocalPositionOfCell(theID);
      const Amg::Vector3D globalPos(sielement->globalPosition(localPos));
      clusterEquiv.setHitType(HitType::clustered);
      clusterEquiv.setX(globalPos.x());
      clusterEquiv.setY(globalPos.y());
      clusterEquiv.setZ(globalPos.z());
      clusterEquiv.setDetType(SiliconTech::pixel);
      clusterEquiv.setIdentifierHash(sielement->identifyHash());

      int barrel_ec = m_pixelId->barrel_ec(theID);
      if (barrel_ec == 0)
        clusterEquiv.setDetectorZone(DetectorZone::barrel);
      else if (barrel_ec == 2)
        clusterEquiv.setDetectorZone(DetectorZone::posEndcap);
      else if (barrel_ec == -2)
        clusterEquiv.setDetectorZone(DetectorZone::negEndcap);

      clusterEquiv.setLayerDisk(m_pixelId->layer_disk(theID));
      clusterEquiv.setPhiModule(m_pixelId->phi_module(theID));
      clusterEquiv.setEtaModule(m_pixelId->eta_module(theID));
      clusterEquiv.setPhiIndex(m_pixelId->phi_index(theID));
      clusterEquiv.setEtaIndex(m_pixelId->eta_index(theID));

      clusterEquiv.setPhiWidth(cluster->width().colRow()[1]);
      clusterEquiv.setEtaWidth(cluster->width().colRow()[0]);
      //Save the truth here as the MultiTruth object is only transient
      index_type index, position;
      bestExtcode.eventIndex(index, position);
      if (bestParent)
        clusterEquiv.setEventIndex(index);
      else
        clusterEquiv.setEventIndex(std::numeric_limits<long>::max());
      clusterEquiv.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
      clusterEquiv.setBarcodePt(static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)));
      clusterEquiv.setParentageMask(parentMask.to_ulong());
      clusterOut.setClusterEquiv(clusterEquiv);
      clusters.push_back(clusterOut);
    }
  }

  //Now lets do the strip clusters
  //Loopover the pixel clusters and convert them into a HTTCluster for storage
  // Dump pixel clusters. They're in m_pixelContainer
  auto stripSDOHandle = SG::makeHandle(m_stripSDOKey, eventContext);
  ATH_MSG_DEBUG("Found SCT SDO Map");
  auto stripRDOHandle = SG::makeHandle(m_stripRDOKey, eventContext);

  for (const InDetRawDataCollection<SCT_RDORawData>* SCT_Collection : *stripRDOHandle) {
    if (SCT_Collection == nullptr) { continue; }
    for (const SCT_RDORawData* sctRawData : *SCT_Collection) {
      const Identifier rdoId = sctRawData->identify();
      // get the det element from the det element collection
      const InDetDD::SiDetectorElement* sielement = m_SCT_mgr->getDetectorElement(rdoId);
      const InDetDD::SiDetectorDesign& design = dynamic_cast<const InDetDD::SiDetectorDesign&>(sielement->design());
      const InDetDD::SiLocalPosition localPos = design.localPositionOfCell(m_sctId->strip(rdoId));
      const Amg::Vector3D gPos = sielement->globalPosition(localPos);
      // if there is simulation truth available, try to retrieve the
      // "most likely" barcode for this strip.
      HepMC::ConstGenParticlePtr bestParent = nullptr;
      TrigHTTInputUtils::ParentBitmask parentMask;
      HepMcParticleLink::ExtendedBarCode bestExtcode;
      if (!m_stripSDOKey.empty()) {
        InDetSimDataCollection::const_iterator iter(stripSDOHandle->find(rdoId));
        // if SDO found for this pixel, associate the particle
        if (iter != stripSDOHandle->end()) getTruthInformation(iter, parentMask, bestExtcode, bestParent);
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
      else if (barrel_ec == 2)
        clusterEquiv.setDetectorZone(DetectorZone::posEndcap);
      else if (barrel_ec == -2)
        clusterEquiv.setDetectorZone(DetectorZone::negEndcap);

      clusterEquiv.setLayerDisk(m_sctId->layer_disk(rdoId));
      clusterEquiv.setPhiModule(m_sctId->phi_module(rdoId));
      clusterEquiv.setEtaModule(m_sctId->eta_module(rdoId));
      clusterEquiv.setPhiIndex(m_sctId->strip(rdoId));
      clusterEquiv.setEtaIndex(m_sctId->row(rdoId));
      clusterEquiv.setSide(m_sctId->side(rdoId));
      //I think this is the strip "cluster" width
      clusterEquiv.setPhiWidth(sctRawData->getGroupSize());
      //Save the truth here as the MultiTruth object is only transient
      index_type index, position;
      bestExtcode.eventIndex(index, position);
      if (bestParent)
        clusterEquiv.setEventIndex(index);
      else
        clusterEquiv.setEventIndex(std::numeric_limits<long>::max());


      clusterEquiv.setBarcode((long)(bestParent ? bestExtcode.barcode() : std::numeric_limits<long>::max()));
      clusterEquiv.setBarcodePt(static_cast<unsigned long>(std::ceil(bestParent ? bestParent->momentum().perp() : 0.)));
      clusterEquiv.setParentageMask(parentMask.to_ulong());
      clusterOut.setClusterEquiv(clusterEquiv);
      clusters.push_back(clusterOut);
    } // end for each RDO in the strip collection
  } // end for each strip RDO collection
  // dump all RDO's and SDO's for a given event, for debugging purposes

  return StatusCode::SUCCESS;
}

StatusCode
TrigHTTSGToRawHitsTool::readTruthTracks(std::vector <HTTTruthTrack>& truth, const EventContext& eventContext)
{
  auto simTracksHandle = SG::makeHandle(m_mcCollectionKey, eventContext);
  ATH_MSG_DEBUG("Dump truth tracks, size " << simTracksHandle->size());

  // dump each truth track
  for (unsigned int ievt = 0; ievt < simTracksHandle->size(); ++ievt) {

    const HepMC::GenEvent* genEvent = simTracksHandle->at(ievt);
    // retrieve the primary interaction vertex here. for now, use the dummy origin.
    HepGeom::Point3D<double>  primaryVtx(0., 0., 0.);
    // the event should have signal process vertex unless it was generated as single particles.
    // if it exists, use it for the primary vertex.
    HepMC::ConstGenVertexPtr spv = HepMC::signal_process_vertex(genEvent);
    if (spv) {
        primaryVtx.set(spv->position().x(),
                       spv->position().y(),
                       spv->position().z());
      ATH_MSG_DEBUG("using signal process vertex for eventIndex " << ievt << ":"
        << primaryVtx.x() << "\t" << primaryVtx.y() << "\t" << primaryVtx.z());
    }

    //    for (HepMC::GenEvent::particle_const_iterator it = genEvent->particles_begin(), ft = genEvent->particles_end(); it != ft; ++it) {
    for (auto it = HepMC::begin(*genEvent), ft = HepMC::end(*genEvent); it != ft; ++it) {
      HepMC::ConstGenParticlePtr particle = *it;

      const int pdgcode = particle->pdg_id();
      // reject generated particles without a production vertex.
      if (particle->production_vertex() == nullptr) {
        continue;
      }
      // reject neutral or unstable particles
      const HepPDT::ParticleData* pd = m_particleDataTable->particle(abs(pdgcode));
      if (pd == nullptr) {
        continue;
      }
      float charge = pd->charge();
      if (pdgcode < 0) charge *= -1.; // since we took absolute value above

      if (std::abs(charge) < 0.5) {
        continue;
      }
      if (particle->status() % 1000 != 1) {
        continue;
      }

      // truth-to-track tool
      const Amg::Vector3D momentum(particle->momentum().px(), particle->momentum().py(), particle->momentum().pz());
      const Amg::Vector3D position(particle->production_vertex()->position().x(), particle->production_vertex()->position().y(), particle->production_vertex()->position().z());
      const Trk::CurvilinearParameters cParameters(position, momentum, charge);

      Trk::PerigeeSurface persf;
      if (m_UseNominalOrigin) {
        Amg::Vector3D    origin(0, 0, 0);
        persf = Trk::PerigeeSurface(origin);
      }
      else {
        SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle{ m_beamSpotKey, eventContext };
        Trk::PerigeeSurface persf(beamSpotHandle->beamPos());
      }

      const Trk::TrackParameters* tP = m_extrapolator->extrapolate(eventContext, cParameters, persf, Trk::anyDirection, false).release();
      const double track_truth_d0 = tP ? tP->parameters()[Trk::d0] : 999.;
      const double track_truth_phi = tP ? tP->parameters()[Trk::phi] : 999.;
      const double track_truth_p = (tP && fabs(tP->parameters()[Trk::qOverP]) > 1.e-8) ?
        tP->charge() / tP->parameters()[Trk::qOverP] : 10E7;
      const double track_truth_x0 = tP ? tP->position().x() : 999.;
      const double track_truth_y0 = tP ? tP->position().y() : 999.;
      const double track_truth_z0 = tP ? tP->parameters()[Trk::z0] : 999.;
      const double track_truth_q = tP ? tP->charge() : 0.;
      const double track_truth_sinphi = tP ? std::sin(tP->parameters()[Trk::phi]) : -1.;
      const double track_truth_cosphi = tP ? std::cos(tP->parameters()[Trk::phi]) : -1.;
      const double track_truth_sintheta = tP ? std::sin(tP->parameters()[Trk::theta]) : -1.;
      const double track_truth_costheta = tP ? std::cos(tP->parameters()[Trk::theta]) : -1.;
      double truth_d0corr = track_truth_d0 - (primaryVtx.y() * cos(track_truth_phi) - primaryVtx.x() * sin(track_truth_phi));
      double truth_zvertex = 0.;
      const HepGeom::Point3D<double> startVertex(particle->production_vertex()->position().x(), particle->production_vertex()->position().y(), particle->production_vertex()->position().z());
      // categorize particle (prompt, secondary, etc.) based on InDetPerformanceRTT/detector paper criteria.
      bool isPrimary = true;
      if (std::abs(truth_d0corr) > 2.) { isPrimary = false; }
      if (HepMC::barcode(particle) > m_simBarcodeOffset || HepMC::barcode(particle) == 0) { isPrimary = false; }

      if (isPrimary && particle->production_vertex()) {
        const HepGeom::Point3D<double> startVertex(particle->production_vertex()->position().x(), particle->production_vertex()->position().y(), particle->production_vertex()->position().z());
        if (std::abs(startVertex.z() - truth_zvertex) > 100.) { isPrimary = false; }
        if (particle->end_vertex()) {
          HepGeom::Point3D<double> endVertex(particle->end_vertex()->position().x(), particle->end_vertex()->position().y(), particle->end_vertex()->position().z());
          if (endVertex.perp() < HTT_PT_TRUTHMIN && std::abs(endVertex.z()) < HTT_Z_TRUTHMIN) { isPrimary = false; }
        }
      }
      else {
        isPrimary = false;
      }

      HepMcParticleLink::ExtendedBarCode extBarcode2(HepMC::barcode(particle), ievt);

      HTTTruthTrack tmpSGTrack;
      tmpSGTrack.setVtxX(track_truth_x0);
      tmpSGTrack.setVtxY(track_truth_y0);
      tmpSGTrack.setVtxZ(track_truth_z0);
      tmpSGTrack.setD0(track_truth_d0);
      tmpSGTrack.setVtxZ(primaryVtx.z());
      tmpSGTrack.setQ(track_truth_q);
      tmpSGTrack.setPX(track_truth_p * (track_truth_cosphi * track_truth_sintheta));
      tmpSGTrack.setPY(track_truth_p * (track_truth_sinphi * track_truth_sintheta));
      tmpSGTrack.setPZ(track_truth_p * track_truth_costheta);
      tmpSGTrack.setPDGCode(pdgcode);
      tmpSGTrack.setBarcode(extBarcode2.barcode());
      index_type index2, position2;
      extBarcode2.eventIndex(index2, position2);
      tmpSGTrack.setEventIndex(index2);

      truth.push_back(tmpSGTrack);
    } // end for each GenParticle in this GenEvent
  } // end for each GenEvent


  return StatusCode::SUCCESS;
}

void TrigHTTSGToRawHitsTool::getTruthInformation(InDetSimDataCollection::const_iterator& iter,
  TrigHTTInputUtils::ParentBitmask& parentMask,
  HepMcParticleLink::ExtendedBarCode& bestExtcode,
  HepMC::ConstGenParticlePtr bestParent) {

  const InDetSimData& sdo(iter->second);
  const std::vector<InDetSimData::Deposit>& deposits(sdo.getdeposits());
  for (const InDetSimData::Deposit& dep : deposits) {

    const HepMcParticleLink& particleLink(dep.first);
    // RDO's without SDO's are delta rays or detector noise.
    if (!particleLink.isValid()) { continue; }
    const float genEta = particleLink->momentum().pseudoRapidity();
    const float genPt = particleLink->momentum().perp(); // MeV
    // reject unstable particles
    if (particleLink->status() % 1000 != 1) { continue; }
    // reject secondaries and low pT (<400 MeV) pileup
    if (HepMC::barcode(particleLink.cptr()) > m_simBarcodeOffset || HepMC::barcode(particleLink.cptr()) == 0) { continue; }
    // reject far forward particles
    if (fabs(genEta) > m_maxEta) { continue; }
    // "bestParent" is the highest pt particle
    if (bestParent == nullptr || bestParent->momentum().perp() < genPt) {
      bestParent = particleLink.cptr();
      bestExtcode = HepMcParticleLink::ExtendedBarCode(particleLink.barcode(), particleLink.eventIndex());
    }
#ifdef HEPMC3
    parentMask |= TrigHTTInputUtils::construct_truth_bitmap(std::shared_ptr<const HepMC3::GenParticle>(particleLink.cptr()));
#else
    parentMask |= TrigHTTInputUtils::construct_truth_bitmap(particleLink.cptr());
#endif
    // check SDO
  } // end for each contributing particle
}
