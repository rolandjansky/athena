/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
-------------------
ATLAS Collaboration
***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "SiSpacePointFormation/SiTrackerSpacePointFinder.h"
#include "SiSpacePointFormation/SiElementPropertiesTable.h"

// For processing clusters
#include "InDetReadoutGeometry/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

// Space point Classes,
#include "TrkSpacePoint/SpacePoint.h"
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointContainer.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkSpacePoint/SpacePointCLASS_DEF.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"


// general Atlas classes
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"


#include "CxxUtils/make_unique.h"

namespace InDet {

//------------------------------------------------------------------------
  SiTrackerSpacePointFinder::SiTrackerSpacePointFinder(const std::string& name,
      ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
    //m_SCT_ClustersName("SCT_Clusters"),
    //m_PixelsClustersName("PixelClusters"),
    //m_spacePointsSCTName("SCT_SpacePoints"),
    //m_spacePointsPixelName("PixelSpacePoints"),
    //m_spacePointsOverlapName("OverlapSpacePoints"),
    m_Sct_clcontainerKey("SCT_Clusters"),
    m_Pixel_clcontainerKey("PixelClusters"),
    m_selectPixels(true),
    m_selectSCTs(true),
    m_overlap(true),  // process overlaps of SCT wafers.
    m_allClusters(false),  // process all clusters without limits.
    m_overlapLimitOpposite(2.8),  // overlap limit for opposite-neighbour.
    m_overlapLimitPhi(5.64),  // overlap limit for phi-neighbours.
    m_overlapLimitEtaMin(1.68),  // low overlap limit for eta-neighbours.
    m_overlapLimitEtaMax(3.0),  // high overlap limit for eta-neighbours.
    m_epsWidth(0.02),  // safety margin for half-widths, in cm.
    m_overrideBS(false),
    m_xVertex(0.),
    m_yVertex(0.),
    m_zVertex(0.),
    m_iBeamCondSvc("BeamCondSvc",name),
    m_numberOfEvents(0),
    m_manager(0),
    m_idHelper(nullptr),
    m_idHelperPixel(nullptr),
    m_properties(nullptr),
    m_SpacePointContainer_SCTKey("SCT_SpacePoints"),
    m_SpacePointContainerPixelKey("PixelSpacePoints"),
    m_spacepointoverlapCollectionKey("OverlapSpacePoints"),
    m_SiSpacePointMakerTool("InDet::SiSpacePointMakerTool", this)
{
  //Use the same space point name both for internal use (for graphics) end
  // for storing in TDS. If a name is the empty string, do not retrieve
  // those clusters.
  declareProperty("SCT_ClustersName", m_Sct_clcontainerKey, "SCT clContainer" );
  declareProperty("PixelsClustersName", m_Pixel_clcontainerKey, "Pixel clContainer");

  declareProperty("SpacePointsSCTName", m_SpacePointContainer_SCTKey, "SpacePoint SCT container");
  declareProperty("SpacePointsPixelName", m_SpacePointContainerPixelKey, "SpacePoint Pixel container");
  declareProperty("SpacePointsOverlapName", m_spacepointoverlapCollectionKey, "Space Point Overlap collection" );
  
  
  declareProperty("SiSpacePointMakerTool", m_SiSpacePointMakerTool);
  declareProperty("BeamPositionSvc", m_iBeamCondSvc);
  declareProperty("ProcessPixels", m_selectPixels);
  declareProperty("ProcessSCTs", m_selectSCTs);
  declareProperty("ProcessOverlaps", m_overlap);
  declareProperty("AllClusters", m_allClusters);
  declareProperty("OverlapLimitOpposite", m_overlapLimitOpposite);
  declareProperty("OverlapLimitPhi", m_overlapLimitPhi);
  declareProperty("OverlapLimitEtaMin", m_overlapLimitEtaMin);
  declareProperty("OverlapLimitEtaMax", m_overlapLimitEtaMax);
  declareProperty("OverrideBeamSpot", m_overrideBS);
  declareProperty("VertexX", m_xVertex);
  declareProperty("VertexY", m_yVertex);
  declareProperty("VertexZ", m_zVertex);
}
//--------------------------------------------------------------------------
SiTrackerSpacePointFinder::~SiTrackerSpacePointFinder()
{
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
  ATH_CHECK( m_Sct_clcontainerKey.initialize() );

  if (m_selectPixels && m_Pixel_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "Pixels selected and no name set for Pixel clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Pixel_clcontainerKey.initialize() );

  if (m_SpacePointContainer_SCTKey.key().empty()){
    ATH_MSG_FATAL( "No name set for SCT space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainer_SCTKey.initialize() );

  if (m_SpacePointContainerPixelKey.key().empty()){
    ATH_MSG_FATAL( "No name set for Pixels space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_SpacePointContainerPixelKey.initialize() );

  if (m_spacepointoverlapCollectionKey.key().empty()){
    ATH_MSG_FATAL( "No name set for overlap space points");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_spacepointoverlapCollectionKey.initialize() );

  // create containers (requires the Identifier Helpers)
  if (m_selectPixels){
    ATH_CHECK(detStore()->retrieve(m_idHelperPixel,"PixelID"));
    //m_SpacePointContainerPixel = new SpacePointContainer(m_idHelperPixel->wafer_hash_max());

  }

  if (m_selectSCTs) {
    ATH_CHECK(detStore()->retrieve(m_idHelper,"SCT_ID"));

    // also need the SCT Manager to get the detectorElementCollection
    ATH_CHECK(detStore()->retrieve(m_manager,"SCT"));

    // Make a table of neighbours and widths of side 1 SCT wafers
    //ADAM - removed const cast to test without
    const InDetDD::SiDetectorElementCollection* elements =(m_manager->getDetectorElementCollection());
    m_properties = new SiElementPropertiesTable(*m_idHelper, *elements, m_epsWidth);
  }

  ATH_CHECK(m_SiSpacePointMakerTool.retrieve());
  if (!m_overrideBS){
    if (m_iBeamCondSvc.retrieve().isFailure()) {
      m_overrideBS = true;
      ATH_MSG_WARNING( "Could not retrieve Beam Conditions Service. " );
      ATH_MSG_WARNING( "Using instead pre-set beam spot at ( " << m_xVertex
          <<" , "<< m_yVertex << " , " << m_yVertex<< " ) " );
    }
  }
  ATH_MSG_INFO( "SiTrackerSpacePointFinder::initialized for package version " << PACKAGE_VERSION );
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode SiTrackerSpacePointFinder::execute()
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  ++m_numberOfEvents;

  if (! m_overrideBS){
    m_vertex = m_iBeamCondSvc->beamVtx().position();
  } else {
    m_vertex = Amg::Vector3D(m_xVertex,m_yVertex,m_zVertex);
  }

  // register the IdentifiableContainer into StoreGate
  SG::WriteHandle<SpacePointContainer> spacePointContainerPixel;
  if (m_selectPixels){

    spacePointContainerPixel = SG::WriteHandle<SpacePointContainer>( m_SpacePointContainerPixelKey, ctx );
    ATH_CHECK( spacePointContainerPixel.record( CxxUtils::make_unique<SpacePointContainer>(m_idHelperPixel->wafer_hash_max()) ) );

    ATH_MSG_DEBUG( "Container '" << spacePointContainerPixel.name() << "' initialised and recorded in StoreGate" );
  }

  SG::WriteHandle<SpacePointContainer> spacePointContainer_SCT;
  if (m_selectSCTs){
    spacePointContainer_SCT = SG::WriteHandle<SpacePointContainer>( m_SpacePointContainer_SCTKey, ctx );
    ATH_CHECK( spacePointContainer_SCT.record( CxxUtils::make_unique<SpacePointContainer>(m_idHelper->wafer_hash_max()) ) );

    //m_SpacePointContainer_SCT->addRef();
    ATH_MSG_DEBUG( "Container '" << spacePointContainer_SCT.name() << "' initialised and recorded in StoreGate" );
  }

  SG::WriteHandle<SpacePointOverlapCollection> spacepointoverlapCollection( m_spacepointoverlapCollectionKey, ctx );
  ATH_CHECK( spacepointoverlapCollection.record( CxxUtils::make_unique<SpacePointOverlapCollection>() ) );

  //m_spacepointoverlapCollection->addRef();
  ATH_MSG_DEBUG( "Container '" << spacepointoverlapCollection.name() << "' initialised" );

  if (m_selectSCTs){

    // retrieve SCT cluster container
    SG::ReadHandle<SCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
    if (!sct_clcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }
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
      // Create SpacePointCollection
      IdentifierHash idHash = colNext->identifyHash();
      Identifier elementID = colNext->identify();
      SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash);
      spacepointCollection->setIdentifier(elementID);

      if ( colNext->size() != 0){
        addSCT_SpacePoints(colNext,spacepointCollection,spacepointoverlapCollection.ptr());
      } else {
        ATH_MSG_DEBUG( "Empty SCT cluster collection" );
      }

      if (spacepointCollection->size() == 0){
        ATH_MSG_VERBOSE( "SiTrackerSpacePointFinder algorithm found no space points" );
        // -me- clean up memory
        delete (spacepointCollection);
      } else {
        StatusCode sc= spacePointContainer_SCT->addCollection( spacepointCollection,
            spacepointCollection->identifyHash() );
        if (sc.isFailure()){
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( spacepointCollection->size()<< " SpacePoints successfully added to Container !" );
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

      // Create SpacePointCollection
      Identifier elementID = (*colNext)->identify();
      IdentifierHash idHash = (*colNext)->identifyHash();
      SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash);
      spacepointCollection->setIdentifier(elementID);

      if ((*colNext)->size() != 0)
      {
        m_SiSpacePointMakerTool->fillPixelSpacePointCollection(*colNext,spacepointCollection);
      }
      else
      {
        ATH_MSG_DEBUG( "Empty pixel cluster collection" );
      }

      if (spacepointCollection->size() == 0)
      {
        ATH_MSG_DEBUG( "SiTrackerSpacePointFinder algorithm found no space points" );
        // -me- clean up memory
        delete (spacepointCollection);
      }
      else
      {
        StatusCode sc= spacePointContainerPixel->addCollection( spacepointCollection,
            spacepointCollection->identifyHash() );
        if (sc.isFailure())
        {
          ATH_MSG_ERROR( "Failed to add SpacePoints to container" );
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_VERBOSE( spacepointCollection->size()
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

  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode SiTrackerSpacePointFinder::finalize()
{
  ATH_MSG_INFO( "SiTrackerSpacePointFinder::finalize()" );
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );

  delete m_properties;

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::
addSCT_SpacePoints(const SCT_ClusterCollection* next,
    SpacePointCollection* spacepointCollection, SpacePointOverlapCollection* spacepointOverlapCollection)
{

  // Do nothing unless this is a side 1 detector (strips of const phi).
  IdentifierHash thisHash(next->identifyHash());

  // if it is not the stereo side
  const InDetDD::SiDetectorElement *element = m_manager->getDetectorElement(thisHash);
  if (element && !(element->isStereo())){
    //if (m_idHelper->side(thisID)==1){
    // Space points are created from clusters for all possibly
    // overlapping pairs in the given range if m_overlap=true.
    // Otherwise just the opposite pair are processed.
    // Pick up the five neighbours of detector, and check ranges
    // for which overlap is possible. 
    // "check1" is used for opposite and eta overlaps.
    // check2 for phi overlaps

    std::vector<IdentifierHash>*
      others(m_properties->neighbours(thisHash));
    if (others==0 || others->empty() ) return;
    std::vector<IdentifierHash>::const_iterator otherHash = others->begin();

    bool overlapColl = false;
    // check opposite wafer
    checkForSCT_Points(next, *otherHash,
        -m_overlapLimitOpposite, +m_overlapLimitOpposite,
        spacepointCollection,overlapColl,spacepointOverlapCollection);

    if (! m_overlap) return;

    // check phi overlaps
    overlapColl = true;
    ++otherHash;
    if (otherHash == others->end() ) return;
    float hwidth(m_properties->halfWidth(thisHash));
    // half-width of wafer

    checkForSCT_Points(next, *otherHash,
        -hwidth, -hwidth+m_overlapLimitPhi,
        +hwidth-m_overlapLimitPhi, +hwidth,spacepointOverlapCollection);
    ++otherHash;
    if (otherHash == others->end() ) return;
    checkForSCT_Points(next, *otherHash,
        +hwidth-m_overlapLimitPhi, +hwidth,
        -hwidth, -hwidth+m_overlapLimitPhi,spacepointOverlapCollection);

    // if barrel, check the eta overlaps too.
    // In this case, action depends on whether layer is even or odd
    // Also, whether we look at "lower in eta", which comes first,
    // or "higher".
    ++otherHash;
    if (otherHash == others->end() )  return;
    // get Identifier without hashing or so
    Identifier thisID = element->identify();

    //if (m_idHelper->barrel_ec(thisID)==0)
    if (m_idHelper->is_barrel(thisID))
    {
      //if (m_idHelper->layer_disk(thisID)==0 || m_idHelper->layer_disk(thisID)==2)
      if (m_idHelper->layer_disk(thisID)%2 == 0)
      {
        checkForSCT_Points(next, *otherHash, +m_overlapLimitEtaMin, 
            +m_overlapLimitEtaMax,
            spacepointCollection,overlapColl,spacepointOverlapCollection);
        ++otherHash;
        if (otherHash == others->end() )  return;

        checkForSCT_Points(next, *otherHash, -m_overlapLimitEtaMax, 
            -m_overlapLimitEtaMin,
            spacepointCollection,overlapColl,spacepointOverlapCollection);
      }else{
        checkForSCT_Points(next, *otherHash, -m_overlapLimitEtaMax, 
            -m_overlapLimitEtaMin,
            spacepointCollection,overlapColl,spacepointOverlapCollection);
        ++otherHash;
        if (otherHash == others->end() )  return;


        checkForSCT_Points(next, *otherHash, +m_overlapLimitEtaMin,
            +m_overlapLimitEtaMax,
            spacepointCollection,overlapColl,spacepointOverlapCollection);
      }
    }
  }
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
    const IdentifierHash& id2, double min, double max,
    SpacePointCollection* spacepointCollection, bool overlapColl, SpacePointOverlapCollection* spacepointOverlapCollection)
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  // Get the cluster collections for these two detectors.
  // Require that (xPhi2 - xPhi1) lie in the range specified.
  // Used for opposite and eta modules

  SG::ReadHandle<SCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
  SCT_ClusterContainer::const_iterator
    it(sct_clcontainer->indexFind(id2));
  if (it==sct_clcontainer->end()) return;

  const SCT_ClusterCollection * clusters2 = *it;

  if (!overlapColl) {
    m_SiSpacePointMakerTool->fillSCT_SpacePointCollection(clusters1, clusters2, min, max, m_allClusters, m_vertex, m_manager, spacepointCollection);
  }
  else {
    m_SiSpacePointMakerTool->fillSCT_SpacePointEtaOverlapCollection(clusters1, clusters2, min, max, m_allClusters, m_vertex, m_manager, spacepointOverlapCollection);
  }
}
//--------------------------------------------------------------------------
void SiTrackerSpacePointFinder::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
    const IdentifierHash& id2,
    double min1, double max1, double min2, double max2, SpacePointOverlapCollection* spacepointOverlapCollection)
{
#ifdef GAUDI_SYSEXECUTE_WITHCONTEXT 
  const EventContext& ctx = getContext();
#else
  const EventContext& ctx = *getContext();
#endif

  // get the cluster collections for these two detectors. Clus1 must lie
  // within min1 and max1 and clus between min2 and max2. Used for phi
  // overlaps.

  SG::ReadHandle<SCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
  SCT_ClusterContainer::const_iterator
    it(sct_clcontainer->indexFind(id2));
  if (it==sct_clcontainer->end()) return;
  const SCT_ClusterCollection * clusters2 (&(**it));

  m_SiSpacePointMakerTool->fillSCT_SpacePointPhiOverlapCollection(clusters1, clusters2, min1, max1, min2, max2, m_allClusters, m_vertex, m_manager, spacepointOverlapCollection);
}

} //namespace
