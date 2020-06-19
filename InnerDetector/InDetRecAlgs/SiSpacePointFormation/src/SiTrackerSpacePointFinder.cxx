/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
-------------------
ATLAS Collaboration
***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "SiSpacePointFormation/SiTrackerSpacePointFinder.h"

// For processing clusters
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// Space point Classes,
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"


// general Atlas classes
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaMonitoringKernel/Monitored.h"

namespace InDet {

//------------------------------------------------------------------------
  SiTrackerSpacePointFinder::SiTrackerSpacePointFinder(const std::string& name,
      ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{ 
  declareProperty("ProcessPixels", m_selectPixels=true);
  declareProperty("ProcessSCTs", m_selectSCTs=true);
  declareProperty("ProcessOverlaps", m_overlap=true, "process overlaps of SCT wafers.");
  declareProperty("AllClusters", m_allClusters=false, "process all clusters without limits.");
  declareProperty("OverlapLimitOpposite", m_overlapLimitOpposite=2.8, "overlap limit for opposite-neighbour.");
  declareProperty("OverlapLimitPhi", m_overlapLimitPhi=5.64, "overlap limit for phi-neighbours.");
  declareProperty("OverlapLimitEtaMin", m_overlapLimitEtaMin=1.68, "low overlap limit for eta-neighbours.");
  declareProperty("OverlapLimitEtaMax", m_overlapLimitEtaMax=3.0, "high overlap limit for eta-neighbours.");
  declareProperty("OverrideBeamSpot", m_overrideBS=false);
  declareProperty("VertexX", m_xVertex=0.);
  declareProperty("VertexY", m_yVertex=0.);
  declareProperty("VertexZ", m_zVertex=0.);

  declareProperty("SpacePointCacheSCT", m_SpacePointCache_SCTKey="");
  declareProperty("SpacePointCachePix", m_SpacePointCache_PixKey="");
}

//-----------------------------------------------------------------------
StatusCode SiTrackerSpacePointFinder::initialize()
{
  ATH_MSG_DEBUG( "SiTrackerSpacePointFinder::initialize()" );

  // Check that clusters, space points and ids have names
  if (m_selectSCTs && m_Sct_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Sct_clcontainerKey.initialize(m_selectSCTs) );

  if (m_selectPixels && m_Pixel_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "Pixels selected and no name set for Pixel clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Pixel_clcontainerKey.initialize(m_selectPixels) );

  if (m_selectSCTs && m_SpacePointContainer_SCTKey.key().empty()){
    ATH_MSG_FATAL( "No name set for SCT space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainer_SCTKey.initialize(m_selectSCTs) );

  if (m_selectPixels && m_SpacePointContainerPixelKey.key().empty()){
    ATH_MSG_FATAL( "No name set for Pixels space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainerPixelKey.initialize(m_selectPixels) );

  if (m_spacepointoverlapCollectionKey.key().empty()){
    ATH_MSG_FATAL( "No name set for overlap space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_spacepointoverlapCollectionKey.initialize() );

  // create containers (requires the Identifier Helpers)
  if (m_selectPixels){
    ATH_CHECK(detStore()->retrieve(m_idHelperPixel,"PixelID"));
  }

  if (m_selectSCTs) {
    ATH_CHECK(detStore()->retrieve(m_idHelper,"SCT_ID"));

    // Initialize the key of input SiElementPropertiesTable and SiDetectorElementCollection for SCT
    ATH_CHECK(m_SCTPropertiesKey.initialize());
    ATH_CHECK(m_SCTDetEleCollKey.initialize());
  }

  ATH_CHECK(m_SiSpacePointMakerTool.retrieve());
  ATH_CHECK(m_beamSpotKey.initialize(!m_overrideBS));

  ATH_CHECK(m_SpacePointCache_SCTKey.initialize(!m_SpacePointCache_SCTKey.key().empty()));
  ATH_CHECK(m_SpacePointCache_PixKey.initialize(!m_SpacePointCache_PixKey.key().empty()));
  m_cachemode = !m_SpacePointCache_SCTKey.key().empty() || !m_SpacePointCache_PixKey.key().empty();

  if (!m_monTool.empty()) CHECK(m_monTool.retrieve());

  ATH_MSG_INFO( "SiTrackerSpacePointFinder::initialized for package version " << PACKAGE_VERSION );
  
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode SiTrackerSpacePointFinder::execute (const EventContext& ctx) const
{
  ++m_numberOfEvents;
  const InDetDD::SiDetectorElementCollection* elements = nullptr;
  const SiElementPropertiesTable* properties = nullptr;
  
  auto nReceivedClustersSCT = Monitored::Scalar<int>( "numSctClusters" , 0 );
  auto nReceivedClustersPIX = Monitored::Scalar<int>( "numPixClusters" , 0 );
  auto nSCTspacePoints = Monitored::Scalar<int>( "numSctSpacePoints"   , 0 );
  auto nPIXspacePoints = Monitored::Scalar<int>( "numPixSpacePoints"   , 0 );

  auto mon = Monitored::Group( m_monTool, nReceivedClustersPIX,nReceivedClustersSCT, nPIXspacePoints, nSCTspacePoints );

  if (m_selectSCTs) {
    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
    elements = sctDetEle.retrieve();
    if (elements==nullptr) {
      ATH_MSG_FATAL("Pointer of SiDetectorElementCollection (" << m_SCTDetEleCollKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }
    SG::ReadCondHandle<SiElementPropertiesTable> sctProperties(m_SCTPropertiesKey, ctx);
    properties = sctProperties.retrieve();
    if (properties==nullptr) {
      ATH_MSG_FATAL("Pointer of SiElementPropertiesTable (" << m_SCTPropertiesKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }
  }
  SPFCache r_cache(ctx);
  if (! m_overrideBS){
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
    const InDet::BeamSpotData* beamSpot = *beamSpotHandle;
    r_cache.vertex = beamSpot->beamVtx().position();
  } else {
    r_cache.vertex = Amg::Vector3D(m_xVertex,m_yVertex,m_zVertex);
  }

  // register the IdentifiableContainer into StoreGate
  SG::WriteHandle<SpacePointContainer> spacePointContainerPixel;
  if (m_selectPixels){
    spacePointContainerPixel = SG::WriteHandle<SpacePointContainer>( m_SpacePointContainerPixelKey, ctx );
    if(m_SpacePointCache_PixKey.key().empty()){
       ATH_CHECK( spacePointContainerPixel.record( std::make_unique<SpacePointContainer>(m_idHelperPixel->wafer_hash_max()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainerPixelKey.key() << " N= " << m_idHelperPixel->wafer_hash_max());
    }else{
       SG::UpdateHandle<SpacePointCache> updateH( m_SpacePointCache_PixKey, ctx );
       ATH_CHECK(updateH.isValid());
       ATH_CHECK( spacePointContainerPixel.record( std::make_unique<SpacePointContainer>(updateH.ptr()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainerPixelKey.key() << " from cache " << m_SpacePointCache_PixKey.key());
    }
  }

  SG::WriteHandle<SpacePointContainer> spacePointContainer_SCT;
  if (m_selectSCTs){
    spacePointContainer_SCT = SG::WriteHandle<SpacePointContainer>( m_SpacePointContainer_SCTKey, ctx );
    if(m_SpacePointCache_SCTKey.key().empty()){
       ATH_CHECK( spacePointContainer_SCT.record( std::make_unique<SpacePointContainer>(m_idHelper->wafer_hash_max()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainer_SCTKey.key() << " N= " << m_idHelper->wafer_hash_max());
    }else{
       SG::UpdateHandle<SpacePointCache> updateH( m_SpacePointCache_SCTKey, ctx );
       ATH_CHECK(updateH.isValid());
       ATH_CHECK( spacePointContainer_SCT.record( std::make_unique<SpacePointContainer>(updateH.ptr()) ) );
       ATH_MSG_DEBUG("Created SpacePointContainer " << m_SpacePointContainer_SCTKey.key() << " from cache " << m_SpacePointCache_SCTKey.key());
    }
  }

  SG::WriteHandle<SpacePointOverlapCollection> spacepointoverlapCollection( m_spacepointoverlapCollectionKey, ctx );
  ATH_CHECK( spacepointoverlapCollection.record( std::make_unique<SpacePointOverlapCollection>() ) );

  ATH_MSG_DEBUG( "Container '" << spacepointoverlapCollection.name() << "' initialised" );

  int sctCacheCount = 0;
  int pixCacheCount = 0;

  if (m_selectSCTs){

    // retrieve SCT cluster container
    SG::ReadHandle<SCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
    if (!sct_clcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    r_cache.SCTCContainer = sct_clcontainer.cptr();

    ATH_MSG_DEBUG( "SCT Cluster container found: " << sct_clcontainer->size() << " collections" );
    // Get hold of all clusters and iterate through them.
    // Pixel clusters will be converted immediately to pixel space points.
    // For SCT clusters, posssible pairs will be found and space points computed.
    // It is possible, though unlikely, that there are no clusters for the
    // event. Therefore we do not terminate the job if we don't find any.
    SCT_ClusterContainer::const_iterator it = sct_clcontainer->begin();
    SCT_ClusterContainer::const_iterator itend = sct_clcontainer->end();

    for (; it != itend; ++it){
      const SCT_ClusterCollection *colNext=&(**it);
      nReceivedClustersSCT = colNext->size();

      // Create SpacePointCollection
      IdentifierHash idHash = colNext->identifyHash();
      SpacePointContainer::IDC_WriteHandle lock = spacePointContainer_SCT->getWriteHandle(idHash);
      if(lock.OnlineAndPresentInAnotherView()){
          ATH_MSG_DEBUG("SCT Hash " << idHash << " is already in cache");
          ++sctCacheCount;
          continue; //Skip if already present in cache
      }

      Identifier elementID = colNext->identify();
      auto spacepointCollection = std::make_unique<SpacePointCollection>(idHash);
      spacepointCollection->setIdentifier(elementID);

      if(!colNext->empty() && !elements->getDetectorElement(colNext->identifyHash())->isStereo()) {
        addSCT_SpacePoints(colNext, properties, elements,
                           spacepointCollection.get(), spacepointoverlapCollection.ptr(), r_cache);
      } else {
        ATH_MSG_DEBUG( "Empty SCT cluster collection" );
      }

      size_t size = spacepointCollection->size();
      nSCTspacePoints = size;
      if (size == 0){
        ATH_MSG_VERBOSE( "SiTrackerSpacePointFinder algorithm found no space points" );
      } else {
        //In a MT environment the cache maybe filled by another thread in which case this will delete the duplicate
        StatusCode sc= lock.addOrDelete( std::move(spacepointCollection) );
        if (sc.isFailure()){
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( size << " SpacePoints successfully added to Container !" );
      }
    }
  }

  if (m_selectPixels)
  {

    SG::ReadHandle<PixelClusterContainer> pixel_clcontainer( m_Pixel_clcontainerKey, ctx );
    if (!pixel_clcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< pixel_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    ATH_MSG_DEBUG( "Data object " << pixel_clcontainer.name() << " found" );

    // loop over Pixel clusterCollections
    PixelClusterContainer::const_iterator colNext = pixel_clcontainer->begin();
    PixelClusterContainer::const_iterator lastCol = pixel_clcontainer->end();


    int numColl=0;
    for (; colNext != lastCol; ++colNext)
    {
      ATH_MSG_VERBOSE( "Collection num " << numColl++ );
      nReceivedClustersPIX = (*colNext)->size();
      IdentifierHash idHash = (*colNext)->identifyHash();
      SpacePointContainer::IDC_WriteHandle lock = spacePointContainerPixel->getWriteHandle(idHash);
      if(lock.OnlineAndPresentInAnotherView()){
          ATH_MSG_DEBUG("pixel Hash " << idHash << " is already in cache");
          ++pixCacheCount;
          continue;
      }
      // Create SpacePointCollection
      Identifier elementID = (*colNext)->identify();
      auto spacepointCollection = std::make_unique< SpacePointCollection >(idHash);
      spacepointCollection->setIdentifier(elementID);

      if ((*colNext)->size() != 0)
      {
        m_SiSpacePointMakerTool->fillPixelSpacePointCollection(*colNext,spacepointCollection.get());
      }
      else
      {
        ATH_MSG_DEBUG( "Empty pixel cluster collection" );
      }
      size_t size = spacepointCollection->size();
      nPIXspacePoints = spacepointCollection->size();
      if (size == 0)
      {
        ATH_MSG_DEBUG( "SiTrackerSpacePointFinder algorithm found no space points" );
      }
      else
      {
        StatusCode sc = lock.addOrDelete( std::move(spacepointCollection) );
        if (sc.isFailure())
        {
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( size
            << " SpacePoints successfully added to Container !" );
      }
    }
  }

  // store the overlap space points.
  // check that the set isn't empty.
  if (spacepointoverlapCollection->size()==0)
  {
    ATH_MSG_DEBUG( "No overlap space points found" );
  }
  else
  {
    ATH_MSG_DEBUG( spacepointoverlapCollection->size() <<" overlap space points registered." );
  }
  if (m_selectPixels) {
    auto c = spacePointContainerPixel->numberOfCollections();
    m_numberOfPixel += c;
  }
  if (m_selectSCTs) {
    auto c = spacePointContainer_SCT->numberOfCollections();
    m_numberOfSCT   += c;
  }
  if(m_cachemode)//Prevent unnecessary atomic counting
  {
     m_sctCacheHits  += sctCacheCount;
     m_pixCacheHits  += pixCacheCount;
  }
  
  return StatusCode::SUCCESS;
  
}

//---------------------------------------------------------------------------
StatusCode SiTrackerSpacePointFinder::finalize()
{
  ATH_MSG_INFO( "SiTrackerSpacePointFinder::finalize()" );
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  ATH_MSG_INFO( m_numberOfPixel << " pixel collections processed" );
  ATH_MSG_INFO( m_numberOfSCT << " sct collections processed" );
  if(m_cachemode){
    //These are debug messages because they can be indeterminate in an MT environment and it could
    //lead to confusing log comparisons.
    ATH_MSG_DEBUG( m_sctCacheHits << " sct cache hits" );
    ATH_MSG_DEBUG( m_pixCacheHits << " pix cache hits" );
  }
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::addSCT_SpacePoints(const SCT_ClusterCollection* sctClusters,
                                                   const SiElementPropertiesTable* properties,
                                                   const InDetDD::SiDetectorElementCollection* elements,
                                                   SpacePointCollection* spacepointCollection, 
                                                   SpacePointOverlapCollection* spacepointOverlapCollection, 
                                                   SPFCache &r_cache) const
{
  // For each trigger element, first evaluate and collect the quantities you need to build the space points.
  // The detector element array has capacity 6:
  // [0] is the trigger element
  // [1] is the opposite element
  // [2]-[3] are the elements tested for eta overlaps
  // [4]-[5] are the elements tested for phi overlaps
  // For each element you save the corresponding cluster collections and the 
  // space point compatibility range as described below.
  // 
  // For the opposite element and the ones tested for eta overlaps, you have to check 
  // if clusters are compatible with the local position of the trigger cluster
  // requiring that the distance between the two clusters in phi is withing a specified range.
  // - For the clusters on the opposite element: [-m_overlapLimitOpposite, m_overlapLimitOpposite]
  //
  // - For the eta overlapping clusters : you use m_overlapLimitEtaMin and m_overlapLimitEtaMax
  //   in different combination depending if you are checking a stereo module or not
  //
  // For each element, the extremes of these ranges are saved in the overlapExtents array 
  // which is later on used in SiSpacePointMakerTool::fillSCT_SpacePointCollection
  //   - overlapExtents[0], overlapExtents[1] are filled for the opposite element
  //   - overlapExtents[2], overlapExtents[3], overlapExtents[4], overlapExtents[5] are filled for the eta overlapping elements
  //
  // For the elements tested for phi overlaps, you have to check 
  // if clusters are compatible with the local position of the trigger cluster. 
  // This needs that the trigger cluster is at the edge of the trigger module:
  // e.g. -hwidth < locX_trigger < -hwidth+m_overlapLimitPhi (or hwidth-m_overlapLimitPhi < locX_trigger < hwidth)
  // and that the other cluster is on the compatible edge of its module:
  // e.g. hwidth-m_overlapLimitPhi < locX_other < hwidth (or -hwidth < locX_other < -hwidth+m_overlapLimitPhi)
  //
  // For each element, the extremes of these ranges are saved in the overlapExtents array 
  // which is later on used in SiSpacePointMakerTool::fillSCT_SpacePointCollection
  //   - overlapExtents[6], overlapExtents[7], overlapExtents[10], overlapExtents[11]
  //     overlapExtents[8], overlapExtents[9], overlapExtents[12], overlapExtents[13] are filled for the phi overlapping elements
  
  enum NeighbourIndices{ThisOne, Opposite, PhiMinus, PhiPlus, EtaMinus, EtaPlus, nNeighbours};
  
  std::array<const SCT_ClusterCollection*, nNeighbours> neighbourClusters{};
  std::array<const InDetDD::SiDetectorElement*, nNeighbours> neighbourElements{};
  std::array<double, 14> overlapExtents{};
  
  // Get the detector element (and its Identifier) correspoding to the cluster collection
  IdentifierHash triggerIdHash(sctClusters->identifyHash());
  const InDetDD::SiDetectorElement *triggerElement = elements->getDetectorElement(triggerIdHash);
  Identifier thisID = triggerElement->identify();

  // Retrieve the neighbours of the detector element
  const std::vector<IdentifierHash>* others(properties->neighbours(triggerIdHash));
  if (others==0 || others->empty() ) return;

  // Save the current detector element and clusters
  neighbourElements[0]  = triggerElement;
  neighbourClusters[0]  = sctClusters;
  int n    = 0      ;
  
  // Default case: you test the opposite element and the overlapping in phi (total 3 elements)
  int Nmax = 4      ; 
  
  // In the barrel, test the eta overlaps as well (total 5 elements)
  if (m_idHelper->is_barrel(thisID)) Nmax = 6;
  
  // You can remove all the overlaps if requrested. Here you test only the opposite element
  if(!m_overlap) Nmax = 2;

  float hwidth(properties->halfWidth(triggerIdHash));
  
  // This flag is use to trigger if the search should be performed.
  // In case there are no other detector elements and/or cluster collection
  // this flag stays false.
  bool  search = false;
  
  // The order of the elements in others is such that you first get the opposite element, 
  // the overlapping in phi and then the overlapping in eta
  // For this reason you need to re-order the indices, since the SiSpacePointMakerTool will process 
  // first the eta overlaps and then the phi ones
  const std::array<size_t, nNeighbours> neigbourIndices{ThisOne, Opposite, EtaMinus, EtaPlus, PhiMinus, PhiPlus};
  
  for (const auto& otherHash : *others) {

    if(++n==Nmax) break;
    const SCT_ClusterCollection*      otherClusters = r_cache.SCTCContainer->indexFindPtr      (otherHash);
    const InDetDD::SiDetectorElement*  otherElement = elements             ->getDetectorElement(otherHash);

    if(!otherElement || !otherClusters) continue;

    neighbourElements[neigbourIndices[n]] = otherElement; 
    neighbourClusters[neigbourIndices[n]] = otherClusters;
    search      = true ;
    
    switch (n) {
      case Opposite: {
        overlapExtents[ 0] = -m_overlapLimitOpposite;
        overlapExtents[ 1] =  m_overlapLimitOpposite;
        break;
      }
      case PhiMinus: {
        overlapExtents[ 6] =-hwidth;
        overlapExtents[ 7] =-hwidth+m_overlapLimitPhi;
        overlapExtents[ 8] = hwidth-m_overlapLimitPhi;
        overlapExtents[ 9] = hwidth;
        break;
      }
      case PhiPlus: {
        overlapExtents[10] = hwidth-m_overlapLimitPhi;
        overlapExtents[11] = hwidth;
        overlapExtents[12] =-hwidth;
        overlapExtents[13] =-hwidth+m_overlapLimitPhi;
        break;
      } 
      case EtaMinus: {
        if ((m_idHelper->layer_disk(thisID) & 1) == 0) {
          overlapExtents[ 2] = m_overlapLimitEtaMin;
          overlapExtents[ 3] = m_overlapLimitEtaMax;
        } else {
          overlapExtents[ 2] =-m_overlapLimitEtaMax;
          overlapExtents[ 3] =-m_overlapLimitEtaMin;
        }
        break;
      }
      default: {
        if ((m_idHelper->layer_disk(thisID) & 1) == 0) {
          overlapExtents[ 4] = -m_overlapLimitEtaMax;
          overlapExtents[ 5] = -m_overlapLimitEtaMin;
        } else {
          overlapExtents[ 4] = m_overlapLimitEtaMin;
          overlapExtents[ 5] = m_overlapLimitEtaMax;
        }
        break;
      }
    }
  }
  
  if(search) {
    m_SiSpacePointMakerTool->fillSCT_SpacePointCollection(neighbourElements,neighbourClusters,overlapExtents,m_allClusters,r_cache.vertex,spacepointCollection,spacepointOverlapCollection);
  }
}

} //namespace
