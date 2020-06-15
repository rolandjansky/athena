/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
-------------------
ATLAS Collaboration
***************************************************************************/

//<<<<<< INCLUDES                                                       >>>>>>


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
#include "GaudiKernel/PropertyMgr.h"


#include "CxxUtils/make_unique.h"

namespace InDet{

//------------------------------------------------------------------------
SiTrackerSpacePointFinder::SiTrackerSpacePointFinder(const std::string&	name, 
						     ISvcLocator*	pSvcLocator)
    :	AthAlgorithm		(name, pSvcLocator),
//	m_SCT_ClustersName	("SCT_Clusters"),
//	m_PixelsClustersName	("PixelClusters"),
//	m_spacePointsSCTName	("SCT_SpacePoints"),
//	m_spacePointsPixelName	("PixelSpacePoints"),
//	m_spacePointsOverlapName("OverlapSpacePoints"),
	m_Sct_clcontainer("SCT_Clusters"),
	m_Pixel_clcontainer("PixelClusters"),
	m_selectPixels		(true),
	m_selectSCTs(		true),
	m_overlap		(true),		// process overlaps of SCT wafers.
	m_allClusters		(false),	// process all clusters without limits.
	m_overlapLimitOpposite	(2.8),		// overlap limit for opposite-neighbour.
	m_overlapLimitPhi	(5.64),		// overlap limit for phi-neighbours.
	m_overlapLimitEtaMin	(1.68),		// low overlap limit for eta-neighbours.
	m_overlapLimitEtaMax	(3.0),		// high overlap limit for eta-neighbours.
	m_epsWidth		(0.02),		// safety margin for half-widths, in cm.
	m_overrideBS		(false),
	m_xVertex		(0.),
	m_yVertex		(0.),
	m_zVertex		(0.),
	m_iBeamCondSvc		("BeamCondSvc",name),
	m_event			(nullptr),
	m_numberOfEvents	(0),
	m_manager(0),
	m_idHelper		(nullptr),
	m_idHelperPixel		(nullptr),
	m_properties		(nullptr),
	m_SpacePointContainer_SCT	("SCT_SpacePoints"),
	m_SpacePointContainerPixel	("PixelSpacePoints"),
	m_spacepointoverlapCollection	("OverlapSpacePoints"),
	m_SiSpacePointMakerTool		("InDet::SiSpacePointMakerTool")
    {
  //Use the same space point name both for internal use (for graphics) end
  // for storing in TDS. If a name is the empty string, do not retrieve
  // those clusters.
  declareProperty("SCT_ClustersName",	m_Sct_clcontainer, "SCT clContainer" ) ;
  declareProperty("PixelsClustersName",	m_Pixel_clcontainer, "Pixel clContainer");
            
  declareProperty("SpacePointsSCTName",	m_SpacePointContainer_SCT, "SpacePoint SCT container");
  declareProperty("SpacePointsPixelName", m_SpacePointContainerPixel, "SpacePoint Pixel container");
  declareProperty("SpacePointsOverlapName", m_spacepointoverlapCollection, "Space Point Overlap collection" );
  
  
  declareProperty("SiSpacePointMakerTool",	m_SiSpacePointMakerTool);
  declareProperty("BeamPositionSvc",		m_iBeamCondSvc);
  declareProperty("ProcessPixels",		m_selectPixels);
  declareProperty("ProcessSCTs",		m_selectSCTs);
  declareProperty("ProcessOverlaps",		m_overlap);
  declareProperty("AllClusters",		m_allClusters);
  declareProperty("OverlapLimitOpposite",	m_overlapLimitOpposite);
  declareProperty("OverlapLimitPhi",		m_overlapLimitPhi);
  declareProperty("OverlapLimitEtaMin",		m_overlapLimitEtaMin);
  declareProperty("OverlapLimitEtaMax",		m_overlapLimitEtaMax);
  declareProperty("OverrideBeamSpot",		m_overrideBS);
  declareProperty("VertexX",			m_xVertex);
  declareProperty("VertexY",			m_yVertex);
  declareProperty("VertexZ",			m_zVertex);
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
  
    if  (m_selectSCTs && m_Sct_clcontainer.name().empty()){
	ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
	return StatusCode::FAILURE;
    }
  
    if (m_selectSCTs && m_SpacePointContainer_SCT.name().empty()){
	ATH_MSG_FATAL( "No name set for SCT space points");
	return StatusCode::FAILURE;
    }
    if (m_selectPixels && m_SpacePointContainerPixel.name().empty()){
	ATH_MSG_FATAL( "No name set for Pixels space points");
	return StatusCode::FAILURE;
    }
    if (m_overlap && m_spacepointoverlapCollection.name().empty()){
	ATH_MSG_FATAL( "No name set for overlap space points");
	return StatusCode::FAILURE;
    }
  
    // create containers (requires the Identifier Helpers)
    if (m_selectPixels){
	  ATH_CHECK(detStore()->retrieve(m_idHelperPixel,"PixelID"));
//	  m_SpacePointContainerPixel = new SpacePointContainer(m_idHelperPixel->wafer_hash_max()); 

    }
 
    if (m_selectSCTs){
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
/*
    if (msgLvl(MSG::DEBUG)){
	  ATH_MSG_DEBUG( "SiTrackerSpacePointFinder::execute()" );
	  // Put out info about event.
	  const DataHandle<xAOD::EventInfo> eventInfo;
	  StatusCode sc = evtStore()->retrieve(eventInfo);
	  if (sc.isFailure()){
	    ATH_MSG_ERROR( "Could not get event" );
	    return StatusCode::RECOVERABLE;
	  }
	  ATH_MSG_DEBUG( "Event number event_" + std::to_string(eventInfo->eventNumber()) );
    }
*/
    ++m_numberOfEvents;
    
    if (! m_overrideBS){
        m_vertex = m_iBeamCondSvc->beamVtx().position();
    } else {
        m_vertex = Amg::Vector3D(m_xVertex,m_yVertex,m_zVertex);
    }

    // register the IdentifiableContainer into StoreGate
    if (m_selectPixels){
//	  m_SpacePointContainerPixel->cleanup();
//	  StatusCode sc = evtStore()->record(m_SpacePointContainerPixel,m_spacePointsPixelName,false);
//	  if (sc.isFailure()){
//	    ATH_MSG_ERROR( "Container '" << m_spacePointsPixelName
//			   << "' could not be recorded in StoreGate !" );
//	    return StatusCode::RECOVERABLE;
//        }
          m_SpacePointContainerPixel = CxxUtils::make_unique<SpacePointContainer>(m_idHelperPixel->wafer_hash_max());
          if (! m_SpacePointContainerPixel.isValid() ){
               msg(MSG:: FATAL) << "SpacePointContainer " << m_SpacePointContainerPixel.name() << "could not be initialised !"<< endreq;
               return StatusCode::FAILURE;
          }
	  // prevent SG from deleting object:
//	  m_SpacePointContainerPixel->addRef();
	  
	  ATH_MSG_DEBUG( "Container '" << m_SpacePointContainerPixel.name() << "' initialised and recorded in StoreGate" );
    }
  
    if (m_selectSCTs){
/*    
	  m_SpacePointContainer_SCT->cleanup();
	  StatusCode sc = evtStore()->record(m_SpacePointContainer_SCT,m_spacePointsSCTName,false);
	  if (sc.isFailure()){
	    ATH_MSG_ERROR( "Container '" << m_spacePointsSCTName
			   << "' could not be recorded in StoreGate !" );
	    return StatusCode::RECOVERABLE;
	  }
*/	  
	  m_SpacePointContainer_SCT = CxxUtils::make_unique<SpacePointContainer>(m_idHelper->wafer_hash_max());
          if (! m_SpacePointContainer_SCT.isValid() ){
               msg(MSG:: FATAL) << "SpacePointContainer " << m_SpacePointContainer_SCT.name() << "could not be initialised !"<< endreq;
               return StatusCode::FAILURE;
          }
          	  	  
//n	  m_SpacePointContainer_SCT->addRef();	  	  
	  ATH_MSG_DEBUG( "Container '" << m_SpacePointContainer_SCT.name() << "' initialised and recorded in StoreGate" );
    }

    // Get hold of IdentifiableContainer holding SCT clusters from TDS
    // ( because we need random access to get the second side).
    // Note there may not be any, but there could still be pixel clusters.
//    m_spacepointoverlapCollection = new SpacePointOverlapCollection();

    if (m_overlap){
      m_spacepointoverlapCollection = CxxUtils::make_unique<SpacePointOverlapCollection>();
      if (! m_spacepointoverlapCollection.isValid() ){
        msg(MSG:: FATAL) << "SpacePointOverlapCollection " << m_spacepointoverlapCollection.name() << "could not be initialised !"<< endreq;
        return StatusCode::FAILURE;
      }
          
      //    m_spacepointoverlapCollection->addRef();
      ATH_MSG_DEBUG( "Container '" << m_spacepointoverlapCollection.name() << "' initialised" );
    }
    
    if (m_selectSCTs){
	  // retrieve SCT cluster container
        if (!m_Sct_clcontainer.isValid()){
          msg(MSG:: FATAL) << "Could not find the data object "<< m_Sct_clcontainer.name() << " !" << endreq;
          return StatusCode::RECOVERABLE;
        }
        ATH_MSG_DEBUG( "SCT Cluster container found:  " << m_Sct_clcontainer->size() << " collections" );
	// Get hold of all clusters and iterate through them.
	// Pixel clusters will be converted immediately to pixel space points. 
	// For SCT clusters, posssible pairs will be found and space points computed.
	// It is possible, though unlikely, that there are no clusters for the
	// event. Therefore we do not terminate the job if we don't find any.  
	SCT_ClusterContainer::const_iterator it     = m_Sct_clcontainer->begin();
	SCT_ClusterContainer::const_iterator itend  = m_Sct_clcontainer->end();
    
	for (; it != itend; ++it){
	    const SCT_ClusterCollection *colNext=&(**it);
	    // Create SpacePointCollection
	    IdentifierHash idHash = colNext->identifyHash(); 
	    Identifier elementID = colNext->identify();
	    SpacePointCollection* spacepointCollection = new SpacePointCollection(idHash); 
	    spacepointCollection->setIdentifier(elementID); 

	    if ( colNext->size() != 0){
		 addSCT_SpacePoints(colNext,spacepointCollection);
	    } else {
		  ATH_MSG_DEBUG( "Empty SCT cluster collection" );
	    }
      
	    if (spacepointCollection->size() == 0){
		  ATH_MSG_VERBOSE( "SiTrackerSpacePointFinder algorithm found no space points" );
		  // -me- clean up memory
		  delete (spacepointCollection);
	    } else {
		  StatusCode sc= m_SpacePointContainer_SCT->addCollection( spacepointCollection,
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
	// retrieve Pixel cluster container
//	m_Pixel_clcontainer=0;
	//StatusCode sc = evtStore()->retrieve(m_Pixel_clcontainer, m_PixelsClustersName);
//	if (sc.isFailure()  || ! m_Pixel_clcontainer)
//	{
//	    ATH_MSG_ERROR( "Pixel Cluster container not found" );
//	    return StatusCode::RECOVERABLE;
//	}
//	ATH_MSG_DEBUG( "Pixel Cluster container found:  " << m_Pixel_clcontainer->size() << " collections" );

        if (!m_Pixel_clcontainer.isValid()){
          msg(MSG:: FATAL) << "Could not find the data object "<< m_Pixel_clcontainer.name() << " !" << endreq;
          return StatusCode::RECOVERABLE;
        }
        
        ATH_MSG_DEBUG( "Data object " << m_Pixel_clcontainer.name() << " found" );    
    
	// loop over Pixel clusterCollections
	PixelClusterContainer::const_iterator colNext  = m_Pixel_clcontainer->begin();
	PixelClusterContainer::const_iterator lastCol  = m_Pixel_clcontainer->end();

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
		StatusCode sc= m_SpacePointContainerPixel->addCollection( spacepointCollection,
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
    if (m_overlap)
    {
      if (m_spacepointoverlapCollection->size()==0)
      {
	  ATH_MSG_DEBUG( "No overlap space points found" );
      }
      else
      {
  	  ATH_MSG_DEBUG( m_spacepointoverlapCollection->size() <<" overlap space points registered." );
      }
    }
/*
    StatusCode sc = evtStore()->record(m_spacepointoverlapCollection, m_spacePointsOverlapName, false);
    if (sc.isFailure())
    {
	ATH_MSG_ERROR( "Failed to register overlap space points" );
	return StatusCode::RECOVERABLE;
    }
*/
    return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode SiTrackerSpacePointFinder::finalize()
{
    ATH_MSG_INFO( "SiTrackerSpacePointFinder::finalize()" );
    ATH_MSG_INFO( m_numberOfEvents << " events processed" );
/*
    if (m_selectPixels)
    {
	m_SpacePointContainerPixel->cleanup();
	m_SpacePointContainerPixel->release();
    }
  
    if (m_selectSCTs)
    {
	m_SpacePointContainer_SCT->cleanup();
	m_SpacePointContainer_SCT->release();
    }
*/
    delete m_properties;
  
    return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::
addSCT_SpacePoints(const SCT_ClusterCollection* next,
		   SpacePointCollection* spacepointCollection) 
{

  // Do nothing unless this is a side 1 detector (strips of const phi).
  IdentifierHash thisHash(next->identifyHash());
  
  // if it is not the stereo side
  const InDetDD::SiDetectorElement *element = m_manager->getDetectorElement(thisHash);
  if (element && !(element->isStereo())){
    //if (m_idHelper->side(thisID)==1) {
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
		       spacepointCollection,overlapColl);
    
    if (! m_overlap) return; 
    
    // check phi overlaps
    overlapColl = true;
    ++otherHash;
    if (otherHash == others->end() ) return;
    float hwidth(m_properties->halfWidth(thisHash)); 
    // half-width of wafer
    
    checkForSCT_Points(next, *otherHash, 
		       -hwidth, -hwidth+m_overlapLimitPhi, 
		       +hwidth-m_overlapLimitPhi, +hwidth);
    ++otherHash;
    if (otherHash == others->end() )  return;
    checkForSCT_Points(next, *otherHash, 
		       +hwidth-m_overlapLimitPhi, +hwidth,
		       -hwidth, -hwidth+m_overlapLimitPhi);
    
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
			       spacepointCollection,overlapColl);
	    ++otherHash;
	    if (otherHash == others->end() )  return;
	    
	    checkForSCT_Points(next, *otherHash, -m_overlapLimitEtaMax, 
			       -m_overlapLimitEtaMin,
			       spacepointCollection,overlapColl);
	  }else{
	    checkForSCT_Points(next, *otherHash, -m_overlapLimitEtaMax, 
			       -m_overlapLimitEtaMin,
			       spacepointCollection,overlapColl);
	    ++otherHash;
	    if (otherHash == others->end() )  return;
	    
	    
	    checkForSCT_Points(next, *otherHash, +m_overlapLimitEtaMin,
			       +m_overlapLimitEtaMax,
			       spacepointCollection,overlapColl);
	  }
      }
  }
}

//--------------------------------------------------------------------------

void SiTrackerSpacePointFinder::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
		   const IdentifierHash& id2, double min, double max,
		   SpacePointCollection* spacepointCollection, bool overlapColl)
{
  
  // Get the cluster collections for these two detectors.
  // Require that (xPhi2 - xPhi1) lie in the range specified.
  // Used for opposite and eta modules
  
  SCT_ClusterContainer::const_iterator
    it(m_Sct_clcontainer->indexFind(id2));
  if (it==m_Sct_clcontainer->end()) return;
  
  const SCT_ClusterCollection * clusters2 = *it;
  
  if (!overlapColl) {
    m_SiSpacePointMakerTool->fillSCT_SpacePointCollection(clusters1, clusters2, min, max, m_allClusters, m_vertex, m_manager, spacepointCollection);
  }
  else {
    m_SiSpacePointMakerTool->fillSCT_SpacePointEtaOverlapCollection(clusters1, clusters2, min, max, m_allClusters, m_vertex, m_manager, m_spacepointoverlapCollection.ptr());
  }
}
//--------------------------------------------------------------------------
void SiTrackerSpacePointFinder::
checkForSCT_Points(const SCT_ClusterCollection* clusters1,
		   const IdentifierHash& id2,
		   double min1, double max1, double min2, double max2)
{
  // get the cluster collections for these two detectors. Clus1 must lie
  // within min1 and max1 and clus between min2 and max2. Used for phi
  // overlaps.
  
  SCT_ClusterContainer::const_iterator
    it(m_Sct_clcontainer->indexFind(id2));
  if (it==m_Sct_clcontainer->end()) return;
  const SCT_ClusterCollection * clusters2 (&(**it));
  
  m_SiSpacePointMakerTool->fillSCT_SpacePointPhiOverlapCollection(clusters1, clusters2, min1, max1, min2, max2, m_allClusters, m_vertex, m_manager, m_spacepointoverlapCollection.ptr());
  
}


}

