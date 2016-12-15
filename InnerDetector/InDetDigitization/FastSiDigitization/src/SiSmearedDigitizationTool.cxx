/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// SiSmearedDigitizationTool.cxx
//   Implementation file for class SiSmearedDigitizationTool
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software

// Pixel digitization includes
#include "FastSiDigitization/SiSmearedDigitizationTool.h"

// Det Descr
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimData/InDetSimDataCollection.h"

// Random numbers
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandLandau.h"

// Gaudi includes
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"

// DataHandle
#include "StoreGate/DataHandle.h"

// Pile-up
#include "PileUpTools/PileUpMergeSvc.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "InDetPrepRawData/SiCluster.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <cmath>

using namespace InDetDD;

// Constructor with parameters:
SiSmearedDigitizationTool::SiSmearedDigitizationTool(const std::string &type, const std::string &name, 
const IInterface* parent):

PileUpToolBase(type, name, parent),
m_thpcsi(NULL),
m_rndmSvc("AtRndmGenSvc",name),
m_manager_pix(NULL),
m_pixel_ID(0),
m_randomEngine(0),
m_randomEngineName("SiSmearedDigitization"),
m_pixelClusterContainer(0),
m_mergeSvc("PileUpMergeSvc",name),
m_prdTruthName("PRD_MultiTruthPixel"),
m_sigmaX(0.005),
m_sigmaY(0.005)
{
  declareInterface<ISiSmearedDigitizationTool>(this);
  declareProperty("RndmSvc",                   m_rndmSvc, "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("RndmEngine",                m_randomEngineName, "Random engine name");
  declareProperty("InputObjectName",           m_inputObjectName="PixelHits", "Input Object name" );
  declareProperty("ClusterContainerName",      m_siClustersName="PixelClusters");
  declareProperty("PRD_MultiTruthName",        m_prdTruthName="PRD_MultiTruthPixel");
  declareProperty("SigmaX",                    m_sigmaX);
  declareProperty("SigmaY",                    m_sigmaY);
}


// Initialize method:
StatusCode SiSmearedDigitizationTool::initialize()
{

  ATH_MSG_DEBUG ( "---  SiSmearedDigitizationTool : initialize()" );

  //locate the AtRndmGenSvc and initialize our local ptr
  if (!m_rndmSvc.retrieve().isSuccess()) 
  {
    ATH_MSG_ERROR ( "Could not find given RndmSvc" );
    return StatusCode::FAILURE;
  }

  // Get the Pixel Detector Manager
  if (StatusCode::SUCCESS != detStore()->retrieve(m_manager_pix,"Pixel") ) {
    ATH_MSG_ERROR ( "Can't get Pixel_DetectorManager " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved Pixel_DetectorManager with version "  << m_manager_pix->getVersion().majorNum() );
  }
  
  if (detStore()->retrieve(m_pixel_ID, "PixelID").isFailure()) {
    ATH_MSG_ERROR ( "Could not get Pixel ID helper" );
    return StatusCode::FAILURE;
  }  
  
  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine) {
    ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
    return StatusCode::FAILURE; 
  }
  
  if (m_inputObjectName=="") 
  {
    ATH_MSG_FATAL ( "Property InputObjectName not set !" );
    return StatusCode::FAILURE;
  } 
  else ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );
  
  //locate the PileUpMergeSvc and initialize our local ptr
  if (!m_mergeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode SiSmearedDigitizationTool::finalize()
{
  ATH_MSG_DEBUG ( "--- SiSmearedDigitizationTool : finalize()" );
  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::prepareEvent(unsigned int) 
{

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: prepareEvent() ---" );
  
  m_siHitCollList.clear();
  m_thpcsi = new TimedHitCollection<SiHit>();
  
  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::processBunchXing(int bunchXing,
PileUpEventInfo::SubEvent::const_iterator bSubEvents,
PileUpEventInfo::SubEvent::const_iterator eSubEvents) 
{

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: processBunchXing() ---" );

  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->pSubEvtSG);
    PileUpTimeEventIndex thisEventIndex(PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()));
    const SiHitCollection* seHitColl(NULL);
    if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent SiHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }

    //Copy Hit Collection
    SiHitCollection* siHitColl(new SiHitCollection(m_inputObjectName));
    SiHitCollection::const_iterator i(seHitColl->begin());
    SiHitCollection::const_iterator e(seHitColl->end());
    // Read hits from this collection
    for (; i!=e; ++i) {
      const SiHit sihit(*i);
      siHitColl->Insert(sihit);
    }
    m_thpcsi->insert(thisEventIndex, siHitColl);
    //store these for deletion at the end of mergeEvent
    m_siHitCollList.push_back(siHitColl);
    ++iEvt;


  }
  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::processAllSubEvents() {

  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: processAllSubEvents() ---" );

  InDet::SiClusterContainer* symSiContainer=0;
  
  m_pixelClusterContainer = new InDet::PixelClusterContainer(m_pixel_ID->wafer_hash_max());
    
  if(!m_pixelClusterContainer) {
    ATH_MSG_FATAL( "[ --- ] Could not create PixelClusterContainer");
    return StatusCode::FAILURE;
  }
  
  // --------------------------------------
  // PixelCluster container registration 
  m_pixelClusterContainer->cleanup();
  if ((evtStore()->record(m_pixelClusterContainer, m_siClustersName)).isFailure())   {
    if ((evtStore()->retrieve(m_pixelClusterContainer, m_siClustersName)).isFailure())   {
      ATH_MSG_FATAL("[ hitproc ] Error while registering PixelCluster container");
      return StatusCode::FAILURE;
    }
  }
  
  // symlink the Pixel Container 
  // Pixel
  if ((evtStore()->symLink(m_pixelClusterContainer,symSiContainer)).isFailure()) {
    ATH_MSG_FATAL( "[ --- ] PixelClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "[ hitproc ] PixelClusterContainer symlinked to SiClusterContainer in StoreGate" );
  }
    
    
  if (retrieveTruth().isFailure()) {
    ATH_MSG_FATAL ( "retrieveTruth() failed!" );
    return StatusCode::FAILURE;
  }

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;
  m_simHitColl = new SiHitCollection;

  TimedHitCollList hitCollList;
  unsigned int numberOfSimHits(0);
  if ( !(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList, numberOfSimHits).isSuccess()) && hitCollList.size()==0 ) {
    ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( hitCollList.size() << " SiHitCollections with key " << m_inputObjectName << " found" );
  }
  
  // Define Hit Collection
  TimedHitCollection<SiHit> thpcsi(numberOfSimHits);
  
  //now merge all collections into one
  TimedHitCollList::iterator   iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end()  );
  
  // loop on the hit collections
  while ( iColl != endColl ) {
    const SiHitCollection* p_collection(iColl->second);
    thpcsi.insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "SiHitCollection found with " << p_collection->size() << " hits" );
    ++iColl;
  }
  m_thpcsi = &thpcsi;
  
  m_nTruthClusters=0;

  // Process the Hits straw by straw: get the iterator pairs for given straw
  if(this->digitize().isFailure()) {
    ATH_MSG_FATAL ( "digitize method failed!" );
    return StatusCode::FAILURE;
  }
  
  if(this->mergeEvent().isFailure()) {
    ATH_MSG_FATAL ( "merge method failed!" );
    return StatusCode::FAILURE;
  }
  
  if (createAndStoreRIOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRIOs() failed!" );
    return StatusCode::FAILURE;
  }
  else ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );
  
  return StatusCode::SUCCESS;
  
}

StatusCode SiSmearedDigitizationTool::retrieveTruth(){
  m_prdTruthCollection = new PRD_MultiTruthCollection;    
  if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prdTruthName))){
    if((evtStore()->retrieve(m_prdTruthCollection, m_prdTruthName)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_prdTruthName);
      return StatusCode::FAILURE;
    }
  } else {
    if((evtStore()->record(m_prdTruthCollection, m_prdTruthName)).isFailure()){
      ATH_MSG_FATAL("Could not record collection " << m_prdTruthName);
      return StatusCode::FAILURE;
    }
  }
 
  return StatusCode::SUCCESS;
  
}

template<typename CLUSTER> 
StatusCode SiSmearedDigitizationTool::FillTruthMap(PRD_MultiTruthCollection * map, CLUSTER * cluster, TimedHitPtr<SiHit> hit) {

  ATH_MSG_DEBUG("Truth map filling with cluster " << *cluster << " and link = " << hit->particleLink());
  if (hit->particleLink().isValid()){
    map->insert(std::make_pair(cluster->identify(), hit->particleLink()));
    m_nTruthClusters++;
    ATH_MSG_DEBUG("Truth map filled with cluster " << *cluster << " and link = " << hit->particleLink());
  } else ATH_MSG_DEBUG("Particle link NOT valid!! Truth map NOT filled with cluster" << cluster << " and link = " << hit->particleLink());
  
  return StatusCode::SUCCESS;
}

StatusCode SiSmearedDigitizationTool::mergeEvent() {
  return StatusCode::SUCCESS;
}



StatusCode SiSmearedDigitizationTool::digitize() {
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in SiSmearedDigizationTool::digitize() ---" );

  TimedHitCollection<SiHit>::const_iterator i, e;
  
  m_pixelClusterMap = new Pixel_detElement_RIO_map;
    
  while (m_thpcsi->nextDetectorElement(i, e)) {
    
    while (i != e) {
      
      TimedHitPtr<SiHit> hit(*i++);
      if (not hit->particleLink().isValid()) continue;
      
      int barrelEC  = hit->getBarrelEndcap();
      int layerDisk = hit->getLayerDisk();
      int phiModule = hit->getPhiModule();
      int etaModule = hit->getEtaModule();
      
      const InDetDD::SiDetectorElement* hitSiDetElement =  m_manager_pix->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule);
      
      ATH_MSG_DEBUG("Pixel SiDetectorElement --> barrel_ec " << barrelEC << ", layer_disk " << layerDisk << ", phi_module " << phiModule << ", eta_module " << etaModule ); 
            
      if ( !hitSiDetElement) {
        ATH_MSG_ERROR( " could not get detector SiDetElement");
        continue;
      }
      
      if (not hitSiDetElement->isPixel()) continue;
           
      IdentifierHash waferID = m_pixel_ID->wafer_hash(hitSiDetElement->identify());
      
      // Transform to reconstruction local coordinates (different x,y,z ordering and sign conventions compared to simulation coords)
      HepGeom::Point3D<double> localStartPosition = hit->localStartPosition();
      HepGeom::Point3D<double> localEndPosition   = hit->localEndPosition();
      
      localStartPosition = hitSiDetElement->hitLocalToLocal3D(localStartPosition);
      localEndPosition = hitSiDetElement->hitLocalToLocal3D(localEndPosition);
 
 
      double localEntryX = localStartPosition.x();
      double localEntryY = localStartPosition.y();
      double localEntryZ = localStartPosition.z();
      double localExitX = localEndPosition.x();
      double localExitY = localEndPosition.y();
      double localExitZ = localEndPosition.z();
         
      // we only consider -/+ z for the moment
      if (localEntryZ * localExitZ > 0 ){
        continue;
      }    
         
      Amg::Vector2D localEntry(localEntryX,localEntryY);
      Amg::Vector2D localExit(localExitX,localExitY);
    
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: start position --- " << localEntry[Trk::locX] << ",  " << localEntry[Trk::locY] << ",  " << localEntryZ );
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: exit position --- " << localExit[Trk::locX] << ",  " << localExit[Trk::locY] << ",  " << localExitZ );
            
      InDetDD::SiCellId entryCellId = hitSiDetElement->cellIdOfPosition(localEntry);
      InDetDD::SiCellId exitCellId  = hitSiDetElement->cellIdOfPosition(localExit);
 
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryCellId " << entryCellId << " --- exitCellId " << exitCellId );

      // entry / exit validity
      bool entryValid = entryCellId.isValid();
      bool exitValid  = exitCellId.isValid();

      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: entryValid? " << entryValid << " --- exitValid? " << exitValid );

      // Noemi has commented this for the moment
      //if (!entryValid or !exitValid) continue;
      
      // the pixel positions and other needed stuff for the geometrical clustering
      std::vector<Identifier> rdoList;
      
      // the intersection id and cellId of it
      Amg::Vector2D avePos = 0.5*(localEntry+localExit);   

      // correct the average position in case it is needed 
      if ( fabs(localEntryZ+localExitZ) > 1e-6 ){
        // 
        const Trk::Surface& hitSiSurface = hitSiDetElement->surface(hitSiDetElement->identify());
        // intersect the surface if necessary
        Amg::Vector3D amgStartPos(localStartPosition.x(),localStartPosition.y(),localStartPosition.z());
        Amg::Vector3D amgEndPosition(localEndPosition.x(),localEndPosition.y(),localEndPosition.z());

        // get the direction 
        Amg::Vector3D amgDirection = (amgEndPosition-amgStartPos).unit();
                
        // get the stuff from surface frame into global 3D frame
        Amg::Vector3D amgPosition3D  = hitSiSurface.transform()*amgStartPos;
        
        Amg::Vector3D amgDirection3D = hitSiSurface.transform().linear()*amgDirection;
        // intersect
        auto sIntersection = hitSiSurface.straightLineIntersection(amgPosition3D, amgDirection3D, false, true);
        
        if (!sIntersection.valid) {
          ATH_MSG_WARNING("Intersection is not valid!");          
          continue;
        }
        // get back into local
        Amg::Vector3D avePositionLocal = hitSiSurface.transform().inverse() * sIntersection.position;
        
        // update the local position
        avePos = Amg::Vector2D(avePositionLocal.x(),avePositionLocal.y());
      } 
      
      Identifier avePosId = hitSiDetElement->identifierOfPosition(avePos);
    
      InDetDD::SiCellId avePosCellId = hitSiDetElement->cellIdFromIdentifier(avePosId);

      bool avePosValid  = avePosCellId.isValid();
      
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: averagePosition : " << avePos[Trk::locX] << "  " << avePos[Trk::locY]);
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: avePosValid? " << avePosValid);

      if (!avePosValid) continue;     
      
      const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design());
      
      // the entry index in phi/eta
      int entryPhiIndex = localEntryX<0. ? 0 : design->rows()-1;
      int entryEtaIndex = localEntryY<0. ? 0 : design->columns()-1;
      if (entryValid) {
        entryPhiIndex = entryCellId.phiIndex();
        entryEtaIndex = entryCellId.etaIndex();
      }
      
      // the exit index in phi/eta
      int exitEtaIndex  =  localExitY<0. ? 0 : design->columns()-1;
      int exitPhiIndex  =  localExitX<0. ? 0 : design->rows()-1;
      if (exitValid) {
        exitPhiIndex = exitCellId.phiIndex();
        exitEtaIndex = exitCellId.etaIndex();
      }
            
      double pitchX = hitSiDetElement->phiPitch(avePos);
      double pitchY = hitSiDetElement->etaPitch();
      
      
      
      double smearPosX = avePos[Trk::locX];
      double smearPosY = avePos[Trk::locY];
      
      //       double distX = fabs(fabs(localEntry[Trk::locX])-fabs(localExit[Trk::locX]));
      //       double distY = fabs(fabs(localEntry[Trk::locY])-fabs(localExit[Trk::locY]));
      
      //the particle crosses at least n pixels (in x direction you have timesX, in y direction you have timesY)
      //       int elementX = floor(distX/pitchX)+1;
      //       int elementY = floor(distY/pitchY)+1;
      
      int elementX = fabs(entryPhiIndex-exitPhiIndex)+1;
      int elementY = fabs(entryEtaIndex-exitEtaIndex)+1;
      
      double distX = fabs(fabs(localEntry[Trk::locX])-fabs(localExit[Trk::locX]));
      int N = floor(distX/pitchX)+1;
      //apply cutoff
      if (elementX>1 and (float(N)*pitchX-distX)<0.5*pitchX) elementX--;
      
      if (CLHEP::RandFlat::shoot(m_randomEngine, 0., 1.0)>0.85) elementX++;
      
      // in the pixel case you smear x and y accordingly to the sigma
      // Smear intersection
      // create the smear parameter    
      double sParX = 0.;
      double sParY = 0.;        
      do {
        sParX = CLHEP::RandGauss::shoot(m_randomEngine, 0., m_sigmaX);
        ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: extracted gaussian value for X --- " << sParX);
      } while (fabs(smearPosX+sParX)>(hitSiDetElement->width()/2.));
      smearPosX += sParX;
      
      do {
        sParY = CLHEP::RandGauss::shoot(m_randomEngine, 0., m_sigmaY);
        ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: extracted gaussian value for Y --- " << sParY);
      } while (fabs(smearPosY+sParY)>(hitSiDetElement->length()/2.));
      smearPosY += sParY;
      
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: elementX --- " << elementX);
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: elementY --- " << elementY);
                                                    
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma X --- " << m_sigmaX);
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: sigma Y --- " << m_sigmaY);
      
            
      Amg::Vector2D smearedPos(smearPosX,smearPosY);
      
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Intersection after smearing: " << smearedPos[Trk::locX] << "  " << smearedPos[Trk::locY]);
      
      Identifier smearedPosId = hitSiDetElement->identifierOfPosition(smearedPos);
      
      rdoList.push_back(smearedPosId);
      InDetDD::SiCellId currentCellId = hitSiDetElement->cellIdFromIdentifier(smearedPosId);
    
      if (!currentCellId.isValid()) continue;
    
      ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: Intersection Id = " << smearedPosId << " --- currentCellId = " << currentCellId );
                       
      //--> Create Pixel Cluster

      double lengthX = elementX*pitchX;
      double lengthY = elementY*pitchY;
      
      InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(elementX,elementY), Amg::Vector2D(lengthX, lengthY));  
		      
      AmgSymMatrix(2) covariance;
      covariance.setIdentity();
      covariance(Trk::locX,Trk::locX) = m_sigmaX*m_sigmaX;
      covariance(Trk::locY,Trk::locY) = m_sigmaY*m_sigmaY;
      Amg::MatrixX* clusterErr = new Amg::MatrixX(covariance);

      // create the cluster
      InDet::PixelCluster* pixelCluster = new InDet::PixelCluster(smearedPosId,
      smearedPos,
      rdoList,
      *siWidth,
      hitSiDetElement,
      clusterErr);
      m_pixelClusterMap->insert(std::pair<IdentifierHash, const InDet::PixelCluster* >(waferID, pixelCluster));
	
      if (FillTruthMap(m_prdTruthCollection, pixelCluster, hit).isFailure()) {
        ATH_MSG_FATAL ( "FillTruthMap() for pixel failed!" );
        return StatusCode::FAILURE;
      }  
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode SiSmearedDigitizationTool::createAndStoreRIOs() 
{
  m_nClusters = 0;
      
  ATH_MSG_DEBUG( "--- SiSmearedDigitizationTool: in pixel createAndStoreRIOs() ---" );
    
  Pixel_detElement_RIO_map::iterator i = m_pixelClusterMap->begin();
  Pixel_detElement_RIO_map::iterator e = m_pixelClusterMap->end();
    
  for ( ; i != e; i = m_pixelClusterMap->upper_bound(i->first) ) {
      
    std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range;
    range = m_pixelClusterMap->equal_range(i->first);
    
    Pixel_detElement_RIO_map::iterator firstDetElem;
    firstDetElem = range.first;
	  
    IdentifierHash waferID;
    waferID = firstDetElem->first;
    
    const InDetDD::SiDetectorElement* detElement = m_manager_pix->getDetectorElement(waferID);
    
    InDet::PixelClusterCollection *clusterCollection = new InDet::PixelClusterCollection(waferID);
    clusterCollection->setIdentifier(detElement->identify());
      
    for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter ) {
      
      InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*>((*iter).second);
      pixelCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
      clusterCollection->push_back(pixelCluster);
      m_nClusters++;
    }
      
    if ( m_pixelClusterContainer->addCollection( clusterCollection, waferID ).isFailure() ) {
      ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
    }
  } // end for
  ATH_MSG_DEBUG("--- SiSmearedDigitizationTool: PixelClusterCollection with " << m_nClusters << " pixel clusters");
  ATH_MSG_DEBUG("--- SiSmearedDigitizationTool: TruthClusterCollection with " << m_nTruthClusters << " clusters");      
  m_pixelClusterMap->clear();
  
  return StatusCode::SUCCESS;
}
