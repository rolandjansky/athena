/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include "FastSiDigitization/SCT_FastDigitizationTool.h"

// Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

// Gaudi
#include "CLHEP/Random/RandomEngine.h"

// Hit class includes
#include "InDetSimData/InDetSimDataCollection.h"
#include "Identifier/Identifier.h"

// Det Descr includes 
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// CLHEP
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandLandau.h"


#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SCT_ModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "InDetReadoutGeometry/SCT_ForwardModuleSideDesign.h"

// FATRAS
#include "InDetReadoutGeometry/SiCellId.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkExUtils/LineIntersection2D.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/SiCluster.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"

#include <sstream>
#include <string>



SCT_FastDigitizationTool::SCT_FastDigitizationTool(const std::string& type, 
					   const std::string& name,
					   const IInterface* parent) : 

  PileUpToolBase(type, name, parent),
  m_sct_ID(0),
  m_manager(NULL),
  m_mergeSvc("PileUpMergeSvc",name),
  m_rndmSvc("AtRndmGenSvc",name),
  m_randomEngine(0),
  m_randomEngineName("SCT_Digitization"),
  m_thpcsi(NULL),
  m_clusterMaker("InDet::ClusterMakerTool"),
  m_sctUseClusterMaker(true),
  m_sctClusterContainer(0),
  m_prdTruthNameSCT("PRD_MultiTruthSCT"),
  m_sctEmulateSurfaceCharge(true),
  m_sctTanLorentzAngleScalor(1.),
  m_sctAnalogStripClustering(false),
  m_sctErrorStrategy(2),
  m_sctRotateEC(true),
  m_sctMinimalPathCut(0 * CLHEP::micrometer)

{
  declareInterface<ISCT_FastDigitizationTool>(this);
  declareProperty("RndmSvc",         m_rndmSvc,                                  "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("InputObjectName", m_inputObjectName="SCT_Hits",                       "Input Object name" );
  declareProperty("SCT_ScaleTanLorentzAngle"      , m_sctTanLorentzAngleScalor);  
  declareProperty("SCT_SmearPathSigma"            , m_sctSmearPathLength);  
  declareProperty("SCT_SmearLandau"               , m_sctSmearLandau); 
  declareProperty("SCT_MinimalPathLength"         , m_sctMinimalPathCut);
  declareProperty("SCT_AnalogClustering"          , m_sctAnalogStripClustering); 
  declareProperty("SCT_UseClusterMaker"           , m_sctUseClusterMaker);
  declareProperty("ClusterMaker"                  , m_clusterMaker);
  declareProperty("SCT_ErrorStrategy"             , m_sctErrorStrategy);
  declareProperty("SCT_RotateEndcapClusters"      , m_sctRotateEC);
  declareProperty("MergeSvc"                      , m_mergeSvc,	"Merge service" );
  declareProperty("SCT_ClusterContainerName"      , m_Sct_SiClustersName="SCT_Clusters");
  declareProperty("TruthNameSCT"                  , m_prdTruthNameSCT);
  
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_FastDigitizationTool::initialize() 
{

  //locate the AtRndmGenSvc and initialize our local ptr
  if (!m_rndmSvc.retrieve().isSuccess()) 
    {
      ATH_MSG_ERROR ( "Could not find given RndmSvc" );
      return StatusCode::FAILURE;
    }

  // Get the SCT Detector Manager
  if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,"SCT") ) {
    ATH_MSG_ERROR ( "Can't get SCT_DetectorManager " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved SCT_DetectorManager with version "  << m_manager->getVersion().majorNum() );
  }

  if (detStore()->retrieve(m_sct_ID, "SCT_ID").isFailure()) {
    ATH_MSG_ERROR ( "Could not get SCT ID helper" );
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
  else 
    {
      ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );
    }

  // retrieve the offline cluster maker : for pixel and/or sct
  if ( (m_sctUseClusterMaker) &&  m_clusterMaker.retrieve().isFailure()){
       ATH_MSG_WARNING( "Could not retrieve " << m_clusterMaker );
       ATH_MSG_WARNING( "-> Switching to simplified cluster creation!" );
       m_sctUseClusterMaker = false;
  } 

  //locate the PileUpMergeSvc and initialize our local ptr
  if (!m_mergeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS ;
}

StatusCode SCT_FastDigitizationTool::prepareEvent(unsigned int)
{

  m_siHitCollList.clear();
  m_thpcsi = new TimedHitCollection<SiHit>();

  return StatusCode::SUCCESS;

}

StatusCode SCT_FastDigitizationTool::processBunchXing(int bunchXing,
						 PileUpEventInfo::SubEvent::const_iterator bSubEvents,
						 PileUpEventInfo::SubEvent::const_iterator eSubEvents) {

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
    SiHitCollection* siHitColl(new SiHitCollection("SCT_Hits"));
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



StatusCode SCT_FastDigitizationTool::processAllSubEvents() {

     m_sctClusterContainer = new InDet::SCT_ClusterContainer(m_sct_ID->wafer_hash_max());

     if(!m_sctClusterContainer) {
       ATH_MSG_FATAL( "[ --- ] Could not create SCT_ClusterContainer");
       return StatusCode::FAILURE;
     }

     InDet::SiClusterContainer* symSiContainer=0;
     
     // --------------------------------------
     // SCT_Cluster container registration
       m_sctClusterContainer->cleanup();
        if ((evtStore()->record(m_sctClusterContainer, m_Sct_SiClustersName)).isFailure())   {
         ATH_MSG_FATAL("[ hitproc ] Error while registering SCT_Cluster container");
         return StatusCode::FAILURE;
       }
       // symlink the SCT Container 
       // SCT
       
       if ((evtStore()->symLink(m_sctClusterContainer,symSiContainer)).isFailure()) {
         ATH_MSG_FATAL( "[ --- ] SCT_ClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
         return StatusCode::FAILURE;
       } else {
         ATH_MSG_DEBUG( "[ hitproc ] SCT_ClusterContainer symlinked to SiClusterContainer in StoreGate" );
       }


  // truth info

    m_sctPrdTruth = new PRD_MultiTruthCollection;
   
      if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prdTruthNameSCT))){
        if((evtStore()->retrieve(m_sctPrdTruth, m_prdTruthNameSCT)).isFailure()){
          ATH_MSG_FATAL("Could not retrieve collection " << m_prdTruthNameSCT);
          return StatusCode::FAILURE;
        }
      }else{
        if((evtStore()->record(m_sctPrdTruth, m_prdTruthNameSCT)).isFailure()){
          ATH_MSG_FATAL("Could not record collection " << m_prdTruthNameSCT);
          return StatusCode::FAILURE;
        }
      }  


  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;

  //this is a list<pair<time_t, DataLink<SCTUncompressedHitCollection> > >
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

  // Process the Hits
  if(this->digitize().isFailure()) {
    ATH_MSG_FATAL ( "digitize method failed!" );
    return StatusCode::FAILURE;
  }

  if (createAndStoreRIOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRIOs() failed!" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );
  }



  return StatusCode::SUCCESS;

}



StatusCode SCT_FastDigitizationTool::mergeEvent()
{

     m_sctClusterContainer = new InDet::SCT_ClusterContainer(m_sct_ID->wafer_hash_max());

     if(!m_sctClusterContainer) {
       ATH_MSG_FATAL( "[ --- ] Could not create SCT_ClusterContainer");
       return StatusCode::FAILURE;
     }

     InDet::SiClusterContainer* symSiContainer=0;
     
     // --------------------------------------
     // SCT_Cluster container registration
       m_sctClusterContainer->cleanup();
        if ((evtStore()->record(m_sctClusterContainer, m_Sct_SiClustersName)).isFailure())   {
         ATH_MSG_FATAL("[ hitproc ] Error while registering SCT_Cluster container");
         return StatusCode::FAILURE;
       }
       // symlink the SCT Container 
       // SCT
       
       if ((evtStore()->symLink(m_sctClusterContainer,symSiContainer)).isFailure()) {
         ATH_MSG_FATAL( "[ --- ] SCT_ClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
         return StatusCode::FAILURE;
       } else {
//         ATH_MSG_INFO( "[ hitproc ] SCT_ClusterContainer symlinked to SiClusterContainer in StoreGate" );
       }



  if (m_thpcsi != 0) {
    if(this->digitize().isFailure()) {
      ATH_MSG_FATAL ( "SCT digitize method failed!" );
      return StatusCode::FAILURE;
    }
  }

  delete m_thpcsi;
  std::list<SiHitCollection*>::iterator siHitColl(m_siHitCollList.begin());
  std::list<SiHitCollection*>::iterator siHitCollEnd(m_siHitCollList.end());
  while(siHitColl!=siHitCollEnd)
    {
      delete (*siHitColl);
      ++siHitColl;
    }
  m_siHitCollList.clear();


  if (createAndStoreRIOs().isFailure()) {
    ATH_MSG_FATAL ( "createAndStoreRIOs() failed!" );
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );
  }


  return StatusCode::SUCCESS;
}


StatusCode SCT_FastDigitizationTool::digitize()
{

  TimedHitCollection<SiHit>::const_iterator i, e;

  m_sctClusterMap = new SCT_detElement_RIO_map;

  int n = 0;
  std::vector<int> trkNo;
  std::vector<Identifier> detEl;

  while (m_thpcsi->nextDetectorElement(i, e)) {

    while (i != e) {


  TimedHitPtr<SiHit> hit(*i++);

  int barrelEC = hit->getBarrelEndcap();
  int layerDisk = hit->getLayerDisk();
  int phiModule = hit->getPhiModule();
  int etaModule = hit->getEtaModule();
  int side = hit->getSide();

  int trkn = hit->trackNumber();

  //const HepMcParticleLink McLink = HepMcParticleLink(trkn,hit.eventId());

  const InDetDD::SiDetectorElement* hitSiDetElement = m_manager->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule,side);
  if (!hitSiDetElement) {ATH_MSG_ERROR( " could not get detector element "); continue;}
  // the module design
  const InDetDD::SCT_ModuleSideDesign* design = dynamic_cast<const InDetDD::SCT_ModuleSideDesign*>(&hitSiDetElement->design());
   if (!design) {
     ATH_MSG_INFO ( "SCT_DetailedSurfaceChargesGenerator::process can not get "<< design) ;
     continue;
   }

  IdentifierHash waferID;
  waferID = m_sct_ID->wafer_hash(hitSiDetElement->identify());
  Identifier detElId = hitSiDetElement->identify();

  // Process only one hit by the same particle in the same detector element
  bool isRep = false;
  for (unsigned int j=0; j<trkNo.size();j++) {
    for (unsigned int k=0; k<detEl.size();k++) {
      if ((trkn > 0) && (trkn == trkNo[j]) && (detElId == detEl[k])) {isRep = true; break;}
    }
  }
  if (isRep) continue;
  trkNo.push_back(trkn);
  detEl.push_back(detElId);

  double hitDepth  = hitSiDetElement->hitDepthDirection();

  HepGeom::Point3D<double> localStartPosition = hit->localStartPosition();
  HepGeom::Point3D<double> localEndPosition = hit->localEndPosition();

  localStartPosition = hitSiDetElement->hitLocalToLocal3D(localStartPosition);
  localEndPosition = hitSiDetElement->hitLocalToLocal3D(localEndPosition);

  HepGeom::Point3D<double> tempLocalStartPosition = localStartPosition;
  HepGeom::Point3D<double> tempLocalEndPosition = localEndPosition;

  double localEntryX = localStartPosition.x();
  double localEntryY = localStartPosition.y();
  double localEntryZ = localStartPosition.z();
  double localExitX = localEndPosition.x();
  double localExitY = localEndPosition.y();
  double localExitZ = localEndPosition.z();

  Amg::Vector2D localEntry(localEntryX,localEntryY);
  Amg::Vector2D localExit(localExitX,localExitY);
  Amg::Vector3D localExit3D(localExitX,localExitY,localExitZ);

  std::vector<Identifier>          rdoList;
  std::map<Identifier, double>     surfaceChargeWeights;  
  double thickness = hitSiDetElement->thickness();

  const Trk::Surface* hitSurface = &hitSiDetElement->surface();

  double distX = localExitX-localEntryX;
  double distY = localExitY-localEntryY;
  double distZ = localExitZ-localEntryZ;

  Amg::Vector3D localDirection(distX, distY, distZ);
  // path length statistics
  double pathGeom     = localDirection.mag();    // geometrical path length
  double pathStep     = 0.;                         // path calculated through stepping
  double pathDrift    = 0.;                         // path calculated through drift charge
  double pathUsed     = 0.;                         // path used (contains smearing & cut if applied)    

  // relational slope
  double slopeYX = distY/distX;
  double slopeXZ = distX/distZ;
  double slopeZX = distZ/distX;
  // signs of stepping
  int signX = distX > 0. ? 1 : -1 ;
  int signY = distY > 0. ? 1 : -1 ;
  int signZ = distZ > 0. ? 1 : -1 ;

  // get the identifier of the entry and the exit
  Identifier entryId = hitSiDetElement->identifierOfPosition(localEntry);
  Identifier exitId  = hitSiDetElement->identifierOfPosition(localExit);
  // now get the cellIds and check whether they're valid
  InDetDD::SiCellId entryCellId = hitSiDetElement->cellIdFromIdentifier(entryId);
  InDetDD::SiCellId exitCellId = hitSiDetElement->cellIdFromIdentifier(exitId);
  // entry / exit validity    
  bool entryValid = entryCellId.isValid();
  bool exitValid  = exitCellId.isValid();
  // min/max indices
  int phiIndexMinRaw = 1000;
  int phiIndexMaxRaw = -1000;

  // the intersecetion id and cellId of it
  double par = -localEntryZ/(localExitZ-localEntryZ);
  double interX = localEntryX + par*(localExitX-localEntryX);
  double interY = localEntryY + par*(localExitY-localEntryY);
//  double interX = 0.5*(localEntryX+localExitX);
//  double interY = 0.5*(localEntryY+localExitY);
  Amg::Vector2D intersection(interX,interY);
  Identifier intersectionId            =  hitSiDetElement->identifierOfPosition(intersection);
  //InDetDD::SiCellId intersectionCellId = hitSiDetElement->cellIdFromIdentifier(intersectionId);    

  // apply the lorentz correction 
  double tanLorAng     = m_sctTanLorentzAngleScalor*hitSiDetElement->getTanLorentzAnglePhi();
  int lorentzDirection = tanLorAng > 0. ? 1 : -1;
  bool useLorentzDrift = fabs(tanLorAng) > 0.01;
  // shift parameters
  double shift = hitSiDetElement->getLorentzCorrection();
  // lorenz angle effects : offset goes against the lorentzAngle
  double xLoffset  = -lorentzDirection*thickness*tanLorAng;

  // --------------------------------------------------------------------------------------
  // fast exit: skip non-valid entry && exit
  if ((!entryValid && !exitValid)){
       continue;
  } 

  // is it a single strip w/o drift effect ? - also check the numerical stability
  bool singleStrip = ( (entryCellId == exitCellId && entryCellId.isValid()) || (distX*distX < 10e-5) );
    if (!singleStrip || useLorentzDrift) {
      // the start parameters
      int strips = 0;    
      // needed for both approaches with and w/o drift
      Identifier          currentId         	= entryId;
      InDetDD::SiCellId   currentCellId     	= entryCellId;      
      Amg::Vector2D  currentCenterPosition = hitSiDetElement->rawLocalPositionOfCell(currentCellId);
      Amg::Vector3D  currentPosition3D(localEntryX,localEntryY,localEntryZ);              
      Amg::Vector3D  currentStep3D(0.,0.,0.);

      // ============================== the clusteristiaon core =====================================================
      // ----------------------- barrel geometrical clustering with drift ------------------------------------------- 
      // there are two independent loops
      // (a) the geometrical steps through the strips : labelled current      
      Amg::Vector2D  currentCenterStep(0.,0.);      
      // check for non-valid entry diode ... this needs to be reset then
      // ---------------------------------------------------
      // start position needs to be reset : --------
      if (!entryValid){    
         // the number of strips in Phi
         int numberOfDiodesPhi = design->cells();
		 // the simple case is if the exit is outside locY
		 if (!hitSurface->bounds().insideLoc2(localEntry)){
		    // step towards the border	
			double stepInY = signY*(fabs(localEntryY)-0.5*hitSiDetElement->length());
			double stepInX = stepInY*distX/distY;
            double stepInZ = stepInY*distZ/distY;
	    currentStep3D = Amg::Vector3D(stepInX,stepInY,stepInZ);
		} else {
           //get the last valid phi border
           int phiExitIndex   = exitCellId.phiIndex() <= 2 ? 0 : numberOfDiodesPhi-1;

           InDetDD::SiCellId  phiExitId(phiExitIndex);
           Amg::Vector2D phiExitCenter = hitSiDetElement->rawLocalPositionOfCell(phiExitId);
           // fill the step parameters
           // this may need to be changed to Rectangular/Trapezoid check
           currentStep3D = stepToStripBorder(*hitSiDetElement,
                                             //*hitSurface,
                                             localEntryX, localEntryY,
                                             localExitX, localExitY,
                                             slopeYX, 
                                             slopeZX,
                                             phiExitCenter,
                                             signX);
         } // ENDIF !hitSurface->bounds().insideLoc2(localEntry)
   
         // get to the first valid strip ---------------------------------------------------
         currentPosition3D += currentStep3D;
		 // for the record
		 pathStep += currentStep3D.mag();		 
		 ATH_MSG_VERBOSE("[ cluster - sct ] Strip entry shifted by " << currentStep3D << " ( " <<  pathStep << " )");		 
         // for step epsilon into the first valid
         Amg::Vector2D positionInFirstValid(currentPosition3D.x()+0.01*signX,currentPosition3D.y()+0.01*signY);
         // reset the entry parameters to the first valid pixel
         currentId             = hitSiDetElement->identifierOfPosition(positionInFirstValid);
         currentCellId         = hitSiDetElement->cellIdFromIdentifier(currentId);
         currentCenterPosition = hitSiDetElement->rawLocalPositionOfCell(currentId);  

        } // ----- start position has been reset -------- ( // endif (!entryValid) )
        
        // (b) the collection of strips due to surface charge
        // the lorentz plane setp 
        double              lplaneStepX = 0.;
        double              lplaneStepY = 0.;
        Amg::Vector3D          lplaneIntersection(0.,0.,0.);        
        Amg::Vector3D          driftPrePosition3D(currentPosition3D);
        Amg::Vector3D          driftPostPosition3D(currentPosition3D);
        // (c) simple cache for the purely geometrical clustering
        Identifier          lastId = currentId;
        bool                lastStrip = false;
        ATH_MSG_VERBOSE("[ cluster - sct ] CurrentPosition " << currentPosition3D ); 

        // the steps between the strips -------------------------------------------
        for ( ; ; ++strips){
           // -------------------------------------------------------------------------------------------------
           // current phi/eta indices 
           int currentPhiIndex = currentCellId.phiIndex();
           // record for the full validation
           // (a) steps through the strips 
           // sct break for last strip or if you step out of the game                                      
           if (lastStrip || currentPosition3D.z() > 0.5*thickness || strips > 4)
              break;
           // no single valid strip 
           if (!exitValid && !currentCellId.isValid()) 
              break;
           // cache it
           phiIndexMinRaw = currentPhiIndex < phiIndexMinRaw ?  currentPhiIndex : phiIndexMinRaw;
           phiIndexMaxRaw = currentPhiIndex > phiIndexMaxRaw ?  currentPhiIndex : phiIndexMaxRaw;
           // find out if this is the last step
           lastStrip = (currentPhiIndex == exitCellId.phiIndex()); 
           // get the current Pitch
           double currentPitchX = hitSiDetElement->phiPitch(currentCenterPosition);
           // the next & previous sct borders are needed (next is w.r.t to the track direction)
           std::vector<double> lorentzLinePositions;
           int trackDir = slopeXZ > 0 ? 1 : -1;
           lorentzLinePositions.reserve(2);           
           // the both pixel borders left/right
           lorentzLinePositions.push_back(currentCenterPosition.x() + trackDir*0.5*currentPitchX);
           lorentzLinePositions.push_back(currentCenterPosition.x() - trackDir*0.5*currentPitchX);
           // the third line is possible -> it is due to xOffset > pitchX
           if (xLoffset*xLoffset > currentPitchX*currentPitchX)
               lorentzLinePositions.push_back(currentCenterPosition.x() + lorentzDirection*1.5*currentPitchX);
                     
           // intersect the effective lorentz plane if the intersection is not valid anymore
           bool        lplaneInterInCurrent = false;
           double      lorentzPlaneHalfX  = 0.;
           Amg::Vector3D  lplaneCandidate(0.,0.,0.);           
           std::vector<double>::iterator lorentzLinePosIter = lorentzLinePositions.begin();
           // find the intersection solutions for the three cases
           int lplaneDirection = 100; // solve the intersection case first
           // test left and right lorentz planes of this pixel            
           for ( ; lorentzLinePosIter != lorentzLinePositions.end(); ++lorentzLinePosIter){
             // first - do the intersection : the readout side is respected by the hit depth direction
             Trk::LineIntersection2D intersectLorentzPlane(localEntryX,-0.5*signZ*thickness,localExitX,0.5*signZ*thickness,
                                                          (*lorentzLinePosIter)+xLoffset,-0.5*hitDepth*thickness,
                                                          (*lorentzLinePosIter),0.5*hitDepth*thickness);
             // avoid repeating intersections
             double formerPlaneStepZ = intersectLorentzPlane.interY-lplaneIntersection.z();
             if (formerPlaneStepZ*formerPlaneStepZ < 10e-5){
                lplaneInterInCurrent = false; 
                continue;
             }
             // is the intersection within the z thickness ?
             lplaneInterInCurrent = intersectLorentzPlane.interY > -0.5*thickness 
                                 && intersectLorentzPlane.interY < 0.5*thickness;
             // the step in z from the former plane intersection
             // only go on if it is worth it 
             // (a) it has to be within z boundaries
             if (lplaneInterInCurrent){
               // record: the half position of the loretnz plane - for estimation to be under/over
               lorentzPlaneHalfX = (*lorentzLinePosIter)+0.5*xLoffset;
               // plane step parameters
               lplaneStepX = intersectLorentzPlane.interX-localEntryX;
               lplaneStepY = lplaneStepX*slopeYX;
               // todo -> break condition if you are hitting the same intersection
               lplaneCandidate = Amg::Vector3D(intersectLorentzPlane.interX,
					       localEntryY+lplaneStepY,
					       intersectLorentzPlane.interY);
               // check in y, the x direction is only needed to find out the drift direction
               double distToNextLineY = 0.5*hitSiDetElement->length()-lplaneCandidate.y();
               double distToPrevLineY = -0.5*hitSiDetElement->length()-lplaneCandidate.y();
               lplaneInterInCurrent = (distToNextLineY*distToPrevLineY) < 0.;
               // we have an intersection candidate - needs to be resolved for +1/-1
               lplaneDirection = lplaneInterInCurrent ? 0 : lplaneDirection;            
               if (lplaneInterInCurrent) break;                                 
             }   
           }
           // now assign it (if valid)
           if (lplaneInterInCurrent) lplaneIntersection = lplaneCandidate;
           ATH_MSG_VERBOSE( "[ cluster - pix ] Lorentz plane intersection x/y/z = " 
                            << lplaneCandidate.x() << ", "
                            << lplaneCandidate.y() << ", "
                            << lplaneCandidate.z() );   
            
           // now solve for 1 / -1 if needed
           if (lplaneDirection) {
              // check the z position of the track at this stage
              double trackZatlplaneHalfX = (lorentzPlaneHalfX-localEntryX)*slopeXZ - 0.5*thickness;
              lplaneDirection = trackZatlplaneHalfX < 0. ? -1 : 1;            
           }           

           // record the lorentz plane intersections 
           // calculate the potential step in X and Y
           currentStep3D = lastStrip ? localExit3D-currentPosition3D :  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                           stepToStripBorder(*hitSiDetElement,
                                             //*hitSurface,
                                             currentPosition3D.x(), currentPosition3D.y(),
                                             localExitX, localExitY,
                                             slopeYX, 
                                             slopeZX,
                                             currentCenterPosition,
                                             signX);	

           // add the current Step to the current position
           currentPosition3D     += currentStep3D;     
           // check whether the step has led outside
           if (!hitSurface->bounds().insideLoc2(Amg::Vector2D(currentPosition3D.x(),
							      currentPosition3D.y())))
           { // stepping outside in y calls for last step
             lastStrip = true;
             // correct to the new position
             currentPosition3D -= currentStep3D;
             double stepInY = signY*0.5*hitSiDetElement->length()-currentPosition3D.y();
             double stepInX = distX/distY*stepInY;
             double stepInZ = slopeZX*stepInX;
             // update to the new currentStep
             currentStep3D = Amg::Vector3D(stepInX,stepInY,stepInZ);
             currentPosition3D += currentStep3D;
           } 
//           if (currentPosition3D.z() > 0.501*signZ*thickness){
           if (fabs(currentPosition3D.z()) > 0.501*thickness){
             // step has led out of the silicon, correct for it               
             lastStrip = true;
             // correct to the new position (has been seen only three times ... probably numerical problem)
             currentPosition3D -= currentStep3D;
             currentStep3D      = localExit3D-currentPosition3D;
             currentPosition3D  = localExit3D;
             // 
             ATH_MSG_VERBOSE("[ cluster - sct ] - current position set to local Exit position !");
           }

           // update the current values for the next step            
           Amg::Vector2D currentInsidePosition(currentPosition3D.x()+0.01*signX,currentPosition3D.y()+0.01*signY);                
           currentId              = hitSiDetElement->identifierOfPosition(currentInsidePosition);
           currentCellId          = hitSiDetElement->cellIdFromIdentifier(currentId);
           // just to be sure also for fan structure cases
           currentCenterPosition = hitSiDetElement->rawLocalPositionOfCell(currentCellId);           
           // The new current Position && the path length for monitoring 
           pathStep              += currentStep3D.mag(); 
		   ATH_MSG_VERBOSE("[ cluster - sct ] CurrentPosition " << currentPosition3D 
						<< " ( yielded by :" << currentStep3D  << ")");                      
           // setting the drift Post position 
           driftPostPosition3D = lplaneInterInCurrent ? lplaneIntersection : currentPosition3D;
          // ---------- the surface charge is emulated -------------------------------------------------------
          if (m_sctEmulateSurfaceCharge && useLorentzDrift){  
            // loop to catch lpintersection in last step            
            int currentDrifts = (lastStrip && lplaneInterInCurrent) ? 2 : 1;
            for (int idrift = 0; idrift < currentDrifts; ++idrift){
               // const assignment for fast access, take intersection solution first, then cell exit
               const Amg::Vector3D& currentDriftPrePosition  = idrift ? driftPostPosition3D : driftPrePosition3D;
               const Amg::Vector3D& currentDriftPostPosition = idrift ? localExit3D : driftPostPosition3D;
               // get the center of the step and drift it to the surface 
               Amg::Vector3D driftStepCenter = 0.5*(currentDriftPrePosition+currentDriftPostPosition);
               // respect the reaout side through the hit dept direction
               double driftZtoSurface    = 0.5*hitDepth*thickness-driftStepCenter.z();
               // record the drift position on the surface                                   
               double driftPositionAtSurfaceX = driftStepCenter.x() + tanLorAng*driftZtoSurface;
               Amg::Vector2D driftAtSurface(driftPositionAtSurfaceX,
                                                 driftStepCenter.y());
               Identifier surfaceChargeId = hitSiDetElement->identifierOfPosition(driftAtSurface);
               if (surfaceChargeId.is_valid()){
                 // check if the pixel has already got some charge
                 if (surfaceChargeWeights.find(surfaceChargeId) == surfaceChargeWeights.end())
                     surfaceChargeWeights[surfaceChargeId] = (currentDriftPostPosition-currentDriftPrePosition).mag();
                 else      
                     surfaceChargeWeights[surfaceChargeId] += (currentDriftPostPosition-currentDriftPrePosition).mag();
               }
               // record the drift step for validation
           } // end of last step intersection check
          } else // ---------- purely geometrical clustering w/o surface charge ---------------------------           
              surfaceChargeWeights[lastId] = currentStep3D.mag();          
          // next pre is current post && lastId is currentId
          driftPrePosition3D = driftPostPosition3D;
          lastId = currentId;

        } // end of steps between strips ----------------------------------------------------------------------        
        m_siDeltaPhiRaw = strips+1;    

    } else {
         // ----------------------- single strip lucky case  ----------------------------------------    
         // 1 strip cluster without drift effect
         surfaceChargeWeights[intersectionId] = pathGeom;
      }

    // the calculated local position
    double totalWeight = 0.;
    Amg::Vector2D clusterPosition(0.,0.);
    std::map<Identifier,double>::iterator weightIter =  surfaceChargeWeights.begin();
    for ( ; weightIter != surfaceChargeWeights.end(); ++weightIter){
        // get the (effective) path length in the strip
        double chargeWeight = (weightIter)->second; 
        // path statistics
        pathDrift += chargeWeight;
        Identifier chargeId = (weightIter)->first;
        // charge smearing if set : 2 possibilities landau/gauss
        // two options fro charge smearing: landau / gauss
        if ( m_sctSmearPathLength > 0. ) {
           // create the smdar parameter
           double sPar = m_sctSmearLandau ?
                  m_sctSmearPathLength*CLHEP::RandLandau::shoot(m_randomEngine) :
                  m_sctSmearPathLength*CLHEP::RandGauss::shoot(m_randomEngine);
           chargeWeight *=  (1.+sPar);  
           // for validation purpose
           m_siSmearRn = sPar;
        }           
	    // the threshold cut
  	    bool aboveThreshold = chargeWeight > m_sctMinimalPathCut;
            if (!aboveThreshold) continue;

        // get the position according to the identifier
        Amg::Vector2D chargeCenterPosition = hitSiDetElement->rawLocalPositionOfCell(chargeId);
        // the cut off
//        if ( !aboveThreshold ) abTh += 1; continue;
        // path statistics
        pathUsed += chargeWeight;
        // taken Weight (Fatras can do analog SCT clustering)
        double takenWeight =  m_sctAnalogStripClustering ? chargeWeight : 1.;
        totalWeight += takenWeight;
        clusterPosition += takenWeight * chargeCenterPosition;
        // and record the rdo
        rdoList.push_back(chargeId);
    }
    // bail out - no left overs after cut
 
    if (!rdoList.size() || pathUsed < m_sctMinimalPathCut) {
//        return 0;
        continue;
    }



    // ---------------------------------------------------------------------------------------------
    //  PART 2: Cluster && ROT creation
    //
    // the Cluster Parameters -----------------------------
    // normalize cluster position && get identifier
    clusterPosition *= 1./totalWeight;
    /*const */Identifier clusterId = hitSiDetElement->identifierOfPosition(clusterPosition);
    if (!clusterId.is_valid()) continue;

    // merging clusters

    SCT_detElement_RIO_map::iterator clusIter;
    int c = 0;
    std::vector<Identifier>::const_iterator rdoIter = rdoList.begin();
    for ( ; rdoIter != rdoList.end(); ++rdoIter) {
      c += 1;
      if (c>100) break;
      for(clusIter = m_sctClusterMap->begin(); clusIter != m_sctClusterMap->end(); ++clusIter) {
        const InDet::SCT_Cluster* currentCluster = (clusIter->second);
        std::vector<Identifier> currentRdoList = currentCluster->rdoList();
        std::vector<Identifier>::const_iterator crdoIter = currentRdoList.begin();
        bool isNeighbour = false;
        for( ; crdoIter != currentRdoList.end(); ++crdoIter) {
          if(abs(crdoIter - rdoIter) < 2) {isNeighbour = true; break;}
        }
      if (!isNeighbour) break;
           rdoList.insert(rdoList.end(), currentRdoList.begin(), currentRdoList.end() );
	   Amg::Vector2D     currentClusterPosition(currentCluster->localPosition());
           clusterPosition = (clusterPosition + currentClusterPosition)/2;
           clusterId = hitSiDetElement->identifierOfPosition(clusterPosition);
           m_sctClusterMap->erase(clusIter); break;
        }
      }

    // check whether this is a trapezoid
    bool isTrapezoid = (design->shape()==InDetDD::Trapezoid);    
    // prepare & create the siWidth
    InDet::SCT_Cluster* sctCluster = 0;
    // Find length of strip at centre
    double clusterWidth = rdoList.size()*hitSiDetElement->phiPitch(clusterPosition); //!< @TODO CHECK 
    const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(clusterPosition));
    double stripLength = fabs(ends.first.xEta()-ends.second.xEta());
    InDet::SiWidth* siWidth = new InDet::SiWidth( Amg::Vector2D(int(rdoList.size()),1),
                                                  Amg::Vector2D(clusterWidth,stripLength) );    
    // correct shift implied by the scaling of the Lorentz angle 
    double newshift = 0.5*thickness*tanLorAng;
    double corr = ( shift - newshift );                                                
    // 2a) Cluster creation ------------------------------------
    if (!m_sctUseClusterMaker){
        // correct for the shift that will be applied in the lcuster maker 
        // (only if surface charge emulation was switched off )
        if (!m_sctEmulateSurfaceCharge && shift*shift > 0.)
            clusterPosition -=  Amg::Vector2D(shift,0.);
        // safe to compare m_sctTanLorentzAngleScalor with 1. since it is set not calculated
        else if (m_sctTanLorentzAngleScalor != 1. && shift*shift > 0.)
            clusterPosition +=  Amg::Vector2D(corr,0.);

        sctCluster = m_clusterMaker->sctCluster(clusterId,
                                                clusterPosition,
                                                rdoList,
                                                *siWidth,
                                                hitSiDetElement,
                                                m_sctErrorStrategy);
    } else {
        // you need to correct for the lorentz drift -- only if surface charge emulation was switched on 
        double appliedShift = m_sctEmulateSurfaceCharge ? m_sctTanLorentzAngleScalor*shift : (1.-m_sctTanLorentzAngleScalor)*shift;
        Amg::Vector2D*  lcorrectedPosition = new Amg::Vector2D(clusterPosition.x()+appliedShift,clusterPosition.y());
	AmgSymMatrix(2) mat; 
	mat.setIdentity(); 
        mat(1,1) = (clusterWidth*clusterWidth)/12.;
        mat(2,2) = (stripLength*stripLength)/12.;
        // rotation for endcap SCT
        if(isTrapezoid  && m_sctRotateEC) {
           double Sn      = hitSiDetElement->sinStereoLocal(intersection);
           double Sn2     = Sn*Sn;
           double Cs2     = 1.-Sn2;
           //double W       = detElement->phiPitch(*clusterLocalPosition)/detElement->phiPitch();
           //double V0      = mat(1,1)*W*W;
           double V0      = mat(1,1);
           double V1      = mat(2,2);
           mat(1,1) = (Cs2*V0+Sn2*V1);
           mat(2,1) = (Sn*sqrt(Cs2)*(V0-V1));
           mat(2,2) = (Sn2*V0+Cs2*V1);
         }            
        // covariance matrix && error description
	Amg::MatrixX* clusterErr = new Amg::MatrixX(mat); 
        // create a custom cluster 

        sctCluster = new InDet::SCT_Cluster(clusterId,
                                            *lcorrectedPosition,
                                            rdoList,                                                                 
                                            *siWidth,
                                            hitSiDetElement,
                                            clusterErr);
      } 

    m_sctClusterMap->insert(std::pair<IdentifierHash, const InDet::SCT_Cluster* >(waferID, sctCluster));

    if (hit->particleLink().isValid()){
      m_sctPrdTruth->insert(std::make_pair(sctCluster->identify(), hit->particleLink()));
      ATH_MSG_DEBUG("Truth map filled with cluster" << sctCluster << " and link = " << hit->particleLink());
    }else{
      ATH_MSG_DEBUG("Particle link NOT valid!! Truth map NOT filled with cluster" << sctCluster << " and link = " << hit->particleLink());
    }

    n += 1;

    }
   }

  return StatusCode::SUCCESS;
}


StatusCode SCT_FastDigitizationTool::createAndStoreRIOs()
{

     SCT_detElement_RIO_map::iterator i = m_sctClusterMap->begin();
     SCT_detElement_RIO_map::iterator e = m_sctClusterMap->end();

     InDet::SCT_ClusterCollection* clusterCollection = 0;

     for (; i != e; i = m_sctClusterMap->upper_bound(i->first)){
        std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range;
        range = m_sctClusterMap->equal_range(i->first);
        SCT_detElement_RIO_map::iterator firstDetElem;
        firstDetElem = range.first;
        IdentifierHash waferID;
        waferID = firstDetElem->first;
        const InDetDD::SiDetectorElement* detElement = m_manager->getDetectorElement(waferID);

        clusterCollection = new InDet::SCT_ClusterCollection(waferID);
        clusterCollection->setIdentifier(detElement->identify());
        for ( SCT_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){
            InDet::SCT_Cluster* sctCluster = const_cast<InDet::SCT_Cluster*>((*iter).second);
            sctCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
            clusterCollection->push_back(sctCluster);
        }
        if (clusterCollection) {
           if ((m_sctClusterContainer->addCollection(clusterCollection, clusterCollection->identifyHash())).isFailure()){
              ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
           }
        }
     } // end for

     m_sctClusterMap->clear();


  return StatusCode::SUCCESS;
}

Amg::Vector3D SCT_FastDigitizationTool::stepToStripBorder(
                                                    const InDetDD::SiDetectorElement& sidetel,
                                                    //const Trk::Surface& surface,
                                                    double localStartX, double localStartY,
                                                    double localEndX, double localEndY,
                                                    double slopeYX, 
                                                    double slopeZX,
                                                    const Amg::Vector2D& stripCenter,
                                                    int direction) const
{
  double stepExitX = 0.;
  double stepExitY = 0.;
  double stepExitZ = 0.;

  double coef = 1;

  // probably needs to be changed to rect/trapezoid
  if (sidetel.isBarrel()){     

          
    // the exit position of the new strip
    double xExitPosition = stripCenter.x()+direction*0.5*sidetel.phiPitch(stripCenter);
    stepExitX = xExitPosition-localStartX;
    stepExitY = stepExitX*slopeYX;
    stepExitZ = stepExitX*slopeZX;

  } else {
    // the end position of this particular strip
    std::pair<Amg::Vector3D,Amg::Vector3D> stripEndGlobal = sidetel.endsOfStrip(stripCenter);              
    Amg::Vector3D oneStripEndLocal = coef*stripEndGlobal.first;
    Amg::Vector3D twoStripEndLocal = coef*stripEndGlobal.second;

    double oneStripPitch = sidetel.phiPitch(Amg::Vector2D(oneStripEndLocal.x(), oneStripEndLocal.y()));
    double twoStripPitch = sidetel.phiPitch(Amg::Vector2D(twoStripEndLocal.x(), twoStripEndLocal.y()));
    // now intersect track with border
    Trk::LineIntersection2D intersectStripBorder(localStartX,localStartY,localEndX,localEndY,
                                                 oneStripEndLocal.x()+direction*0.5*oneStripPitch,oneStripEndLocal.y(),
                                                 twoStripEndLocal.x()+direction*0.5*twoStripPitch,twoStripEndLocal.y());
    // the step in x
    stepExitX = intersectStripBorder.interX - localStartX;
    stepExitY = slopeYX*stepExitX;
    stepExitZ = slopeZX*stepExitX;

  }
  return Amg::Vector3D(stepExitX,stepExitY,stepExitZ);
}


