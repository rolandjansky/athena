/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////
// PixelFastDigitizationTool.cxx
//   Implementation file for class PixelFastDigitizationTool
////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software

// Pixel digitization includes
#include "FastSiDigitization/PixelFastDigitizationTool.h"

// Det Descr
#include "Identifier/Identifier.h"
//#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"

#include "InDetReadoutGeometry/SiReadoutCellId.h"
#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetSimData/InDetSimDataCollection.h"

// Random numbers
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandLandau.h"

// DataHandle
#include "StoreGate/DataHandle.h"

// Pile-up
#include "PileUpTools/PileUpMergeSvc.h"

#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

// Fatras
#include "InDetPrepRawData/PixelCluster.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "TrkExUtils/LineIntersection2D.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "SiClusterizationTool/IPixelClusteringTool.h"
#include "SiClusterizationTool/PixelGangedAmbiguitiesFinder.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"
#include "InDetPrepRawData/SiClusterContainer.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "HepMC/GenParticle.h"

using namespace InDetDD;
using namespace InDet;

static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

// Constructor with parameters:
PixelFastDigitizationTool::PixelFastDigitizationTool(const std::string &type, const std::string &name,
                                                     const IInterface* parent):

  PileUpToolBase(type, name, parent),
  m_thpcsi(NULL),
  m_rndmSvc("AtRndmGenSvc",name),
  m_randomEngine(0),
  m_randomEngineName("FastPixelDigitization"),
  m_manager(NULL),
  m_pixel_ID(NULL),
  m_clusterMaker("InDet::ClusterMakerTool/FatrasClusterMaker"),
  m_pixUseClusterMaker(true),
  m_pixelClusterContainer(0),
  m_pixel_SiClustersName("PixelClusters"),
  m_mergeSvc("PileUpMergeSvc",name),
  m_HardScatterSplittingMode(0),
  m_HardScatterSplittingSkipper(false),
  m_vetoThisBarcode(crazyParticleBarcode),
  m_pixelClusterMap(0),
  m_prdTruthNamePixel("PRD_MultiTruthPixel"),
  m_pixPrdTruth(0),
  //  m_pixelCondSummarySvc("PixelConditionsSummarySvc", name),
  m_gangedAmbiguitiesFinder("InDet::PixelGangedAmbiguitiesFinder"),
  m_inputObjectName("PixelHits"),
  m_pixTanLorentzAngleScalor(1.),
  m_pixEmulateSurfaceCharge(true),
  m_pixSmearPathLength(0.01),
  m_pixSmearLandau(true),
  m_siDeltaPhiCut(0),
  m_siDeltaEtaCut(0),
  m_pixMinimalPathCut(0.0 * Gaudi::Units::micrometer),
  m_pixPathLengthTotConv(125.),
  m_pixModuleDistortion(true), // default: false
  m_pixDistortionTool("PixelDistortionsTool/PixelDistortionsTool"),
  m_pixErrorStrategy(2),
  m_pixelClusterAmbiguitiesMapName("PixelClusterAmbiguitiesMap"),
  m_ambiguitiesMap(0)
{
  declareInterface<IPixelFastDigitizationTool>(this);
  declareProperty("RndmSvc"                        , m_rndmSvc,                  "Random Number Service used in Pixel digitization" );
  declareProperty("RndmEngine"                     , m_randomEngineName,         "Random engine name");
  declareProperty("ClusterMaker"                   , m_clusterMaker);
  declareProperty("PixelUseClusterMaker"           , m_pixUseClusterMaker);
  declareProperty("PixelClusterContainerName"      , m_pixel_SiClustersName);
  declareProperty("MergeSvc"                       , m_mergeSvc,                 "Merge service" );
  declareProperty("TruthNamePixel"                 , m_prdTruthNamePixel);
  //declareProperty("PixelConditionsSummarySvc"      , m_pixelCondSummarySvc,      "Name of the pixel conditions service");
  declareProperty("gangedAmbiguitiesFinder"        , m_gangedAmbiguitiesFinder);
  declareProperty("InputObjectName"                , m_inputObjectName,          "Input Object name" );
  declareProperty("PixelEmulateSurfaceCharge"      , m_pixEmulateSurfaceCharge);
  declareProperty("PixelSmearPathSigma"            , m_pixSmearPathLength);
  declareProperty("PixelSmearLandau"               , m_pixSmearLandau);
  declareProperty("PixelMinimalPathLength"         , m_pixMinimalPathCut);
  declareProperty("PixelPathLengthTotConversion"   , m_pixPathLengthTotConv);
  declareProperty("PixelEmulateModuleDistortion"   , m_pixModuleDistortion);
  declareProperty("PixelDistortionTool"            , m_pixDistortionTool);
  declareProperty("PixelErrorPhi"                  , m_pixPhiError);
  declareProperty("PixelErrorEta"                  , m_pixEtaError);
  declareProperty("PixelErrorStrategy"             , m_pixErrorStrategy);
  declareProperty("PixelClusterAmbiguitiesMapName" , m_pixelClusterAmbiguitiesMapName);
  declareProperty("HardScatterSplittingMode"       , m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty( "ParticleBarcodeVeto"           , m_vetoThisBarcode, "Barcode of particle to ignore");
}

PixelFastDigitizationTool::~PixelFastDigitizationTool() {
  if(m_pixelClusterMap) {
    delete m_pixelClusterMap;
  }
}

// Initialize method:
StatusCode PixelFastDigitizationTool::initialize()
{

  ATH_MSG_DEBUG ( "PixelDigitizationTool::initialize()" );

  //locate the AtRndmGenSvc and initialize our local ptr
  if (!m_rndmSvc.retrieve().isSuccess())
    {
      ATH_MSG_ERROR ( "Could not find given RndmSvc" );
      return StatusCode::FAILURE;
    }

  // Get the Pixel Detector Manager
  if (StatusCode::SUCCESS != detStore()->retrieve(m_manager,"Pixel") ) {
    ATH_MSG_ERROR ( "Can't get Pixel_DetectorManager " );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( "Retrieved Pixel_DetectorManager with version "  << m_manager->getVersion().majorNum() );
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
  else
    {
      ATH_MSG_DEBUG ( "Input objects: '" << m_inputObjectName << "'" );
    }

  // retrieve the offline cluster maker : for pixel and/or sct
  if ( (m_pixUseClusterMaker) &&  m_clusterMaker.retrieve().isFailure()){
    ATH_MSG_WARNING( "Could not retrieve " << m_clusterMaker );
    ATH_MSG_WARNING( "-> Switching to simplified cluster creation!" );
    m_pixUseClusterMaker = false;
  }

  if (m_pixModuleDistortion)
    if (m_pixDistortionTool.retrieve().isFailure()){
      ATH_MSG_WARNING( "Could not retrieve " << m_pixDistortionTool );
      ATH_MSG_WARNING( "-> Switching stave distortion off!" );
      m_pixModuleDistortion = false;
    }

  //locate the PileUpMergeSvc and initialize our local ptr
  if (!m_mergeSvc.retrieve().isSuccess()) {
    ATH_MSG_ERROR ( "Could not find PileUpMergeSvc" );
    return StatusCode::FAILURE;
  }


  // get the InDet::PixelGangedAmbiguitiesFinder
  if ( m_gangedAmbiguitiesFinder.retrieve().isFailure() ) {
    ATH_MSG_FATAL( m_gangedAmbiguitiesFinder.propertyName() << ": Failed to retrieve tool " << m_gangedAmbiguitiesFinder.type() );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG ( m_gangedAmbiguitiesFinder.propertyName() << ": Retrieved tool " << m_gangedAmbiguitiesFinder.type() );
  }


  return StatusCode::SUCCESS;
}



StatusCode PixelFastDigitizationTool::prepareEvent(unsigned int)
{

  m_siHitCollList.clear();
  m_thpcsi = new TimedHitCollection<SiHit>();
  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;
}


StatusCode PixelFastDigitizationTool::processBunchXing(int bunchXing,
                                                       SubEventIterator bSubEvents,
                                                       SubEventIterator eSubEvents)
{

  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
    PileUpTimeEventIndex thisEventIndex(PileUpTimeEventIndex(static_cast<int>(iEvt->time()),iEvt->index()));
    const SiHitCollection* seHitColl(NULL);
    if (!seStore.retrieve(seHitColl,m_inputObjectName).isSuccess()) {
      ATH_MSG_ERROR ( "SubEvent SiHitCollection not found in StoreGate " << seStore.name() );
      return StatusCode::FAILURE;
    }


    ATH_MSG_DEBUG ( "SiHitCollection found with " << seHitColl->size() << " hits" );
    //Copy Hit Collection
    SiHitCollection* siHitColl(new SiHitCollection("PixelHits"));
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


StatusCode PixelFastDigitizationTool::processAllSubEvents() {

  m_pixelClusterContainer = new InDet::PixelClusterContainer(m_pixel_ID->wafer_hash_max());

  if(!m_pixelClusterContainer) {
    ATH_MSG_FATAL( "[ --- ] Could not create PixelClusterContainer");
    return StatusCode::FAILURE;
  }

  InDet::SiClusterContainer* symSiContainer=0;

  // --------------------------------------
  // Pixel Cluster container registration
  m_pixelClusterContainer->cleanup();
  if ((evtStore()->record(m_pixelClusterContainer, m_pixel_SiClustersName)).isFailure())   {
    ATH_MSG_FATAL("[ hitproc ] Error while registering PixelCluster container");
    return StatusCode::FAILURE;
  }

  // symlink the Pixel Cluster Container
  if ((evtStore()->symLink(m_pixelClusterContainer,symSiContainer)).isFailure()) {
    ATH_MSG_FATAL( "[ --- ] PixelClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "[ hitproc ] PixelClusterContainer symlinked to SiClusterContainer in StoreGate" );
  }

  // truth info

  m_pixPrdTruth = new PRD_MultiTruthCollection;

  if ((evtStore()->contains<PRD_MultiTruthCollection>(m_prdTruthNamePixel))){
    if((evtStore()->retrieve(m_pixPrdTruth, m_prdTruthNamePixel)).isFailure()){
      ATH_MSG_FATAL("Could not retrieve collection " << m_prdTruthNamePixel);
      return StatusCode::FAILURE;
    }
  }else{
    if((evtStore()->record(m_pixPrdTruth, m_prdTruthNamePixel)).isFailure()){
      ATH_MSG_FATAL("Could not record collection " << m_prdTruthNamePixel);
      return StatusCode::FAILURE;
    }
  }


  m_ambiguitiesMap =new PixelGangedClusterAmbiguities();


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

  m_HardScatterSplittingSkipper = false;
  // loop on the hit collections
  while ( iColl != endColl ) {
    //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
    if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; ++iColl; continue; }
    if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { ++iColl; continue; }
    if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
    const SiHitCollection* p_collection(iColl->second);
    thpcsi.insert(iColl->first, p_collection);
    ATH_MSG_DEBUG ( "SiHitCollection found with " << p_collection->size() << " hits" );
    ++iColl;
  }
  m_thpcsi = &thpcsi;

  // Process the Hits straw by straw: get the iterator pairs for given straw
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


  if ((evtStore()->setConst(m_pixelClusterContainer)).isFailure()) {
    ATH_MSG_ERROR("[ ---- ] Could not set Pixel ROT container ");
  }
  ATH_MSG_DEBUG ("Ambiguities map has " << m_ambiguitiesMap->size() << " elements" );
  StatusCode sc = evtStore()->record(m_ambiguitiesMap,m_pixelClusterAmbiguitiesMapName,false);
  if (sc.isFailure()){
    ATH_MSG_FATAL ( "PixelClusterAmbiguitiesMap could not be recorded in StoreGate !" );
    return StatusCode::FAILURE;
  }else{
    ATH_MSG_DEBUG ( "PixelClusterAmbiguitiesMap recorded in StoreGate" );
  }




  return StatusCode::SUCCESS;

}



StatusCode PixelFastDigitizationTool::mergeEvent()
{

  m_pixelClusterContainer = new InDet::PixelClusterContainer(m_pixel_ID->wafer_hash_max());

  if(!m_pixelClusterContainer) {
    ATH_MSG_FATAL( "[ --- ] Could not create PixelClusterContainer");
    return StatusCode::FAILURE;
  }

  InDet::SiClusterContainer* symSiContainer=0;

  // --------------------------------------
  // Pixel_Cluster container registration
  m_pixelClusterContainer->cleanup();
  if ((evtStore()->record(m_pixelClusterContainer, "PixelClusters")).isFailure())   {
    ATH_MSG_FATAL("[ hitproc ] Error while registering PixelCluster container");
    return StatusCode::FAILURE;
  }

  // symlink the SCT Container
  if ((evtStore()->symLink(m_pixelClusterContainer,symSiContainer)).isFailure()) {
    ATH_MSG_FATAL( "[ --- ] PixelClusterContainer could not be symlinked to SiClusterContainter in StoreGate !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "[ hitproc ] PixelClusterContainer symlinked to SiClusterContainer in StoreGate" );
  }



  if (m_thpcsi != 0) {
    if(digitize().isFailure()) {
      ATH_MSG_FATAL ( "Pixel digitize method failed!" );
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


StatusCode PixelFastDigitizationTool::digitize()
{


  TimedHitCollection<SiHit>::const_iterator i, e;

  //bool Ganged = false;

  if(!m_pixelClusterMap) { m_pixelClusterMap = new Pixel_detElement_RIO_map; }
  else { m_pixelClusterMap->clear(); }

  while (m_thpcsi->nextDetectorElement(i, e)) {

    Pixel_detElement_RIO_map PixelDetElClusterMap;

    int nnn = 0;

    std::vector<int> trkNo;
    std::vector<Identifier> detEl;

    while (i != e) {

      nnn+=1;

      TimedHitPtr<SiHit> hit(*i++);


      const int barrelEC = hit->getBarrelEndcap();
      const int layerDisk = hit->getLayerDisk();
      const int phiModule = hit->getPhiModule();
      const int etaModule = hit->getEtaModule();

      const InDetDD::SiDetectorElement* hitSiDetElement = m_manager->getDetectorElement(barrelEC,layerDisk,phiModule,etaModule);
      if (!hitSiDetElement) {ATH_MSG_ERROR( " could not get detector element "); continue;}

      if (!(hitSiDetElement->isPixel())) {continue;}

      const IdentifierHash waferID = m_pixel_ID->wafer_hash(hitSiDetElement->identify());


      const int trkn = hit->trackNumber();

      const Identifier hitId = hitSiDetElement->identify();
      //const IdentifierHash hitIdHash = hitSiDetElement->identifyHash();




      bool isRep = false;

      for (unsigned int j=0; j<trkNo.size();j++) {
        for (unsigned int k=0; k<detEl.size();k++) {
          if ((trkn > 0) && (trkn == trkNo[j]) && (hitId == detEl[k])) {isRep = true; break;}
        }
        if (isRep) break; 
      }

      if (isRep) continue;

      trkNo.push_back(trkn);
      detEl.push_back(hitId);

      const double hitDepth  = hitSiDetElement->hitDepthDirection();
      HepGeom::Point3D<double> localStartPosition = hit->localStartPosition();
      HepGeom::Point3D<double> localEndPosition = hit->localEndPosition();

      localStartPosition = hitSiDetElement->hitLocalToLocal3D(localStartPosition);
      localEndPosition = hitSiDetElement->hitLocalToLocal3D(localEndPosition);

      const double localEntryX = localStartPosition.x();
      const double localEntryY = localStartPosition.y();
      const double localEntryZ = localStartPosition.z();
      const double localExitX = localEndPosition.x();
      const double localExitY = localEndPosition.y();
      const double localExitZ = localEndPosition.z();

      const double thickness = hitSiDetElement->thickness();

      const Amg::Vector2D localEntry(localEntryX,localEntryY);
      const Amg::Vector2D localExit(localExitX,localExitY);
      const Amg::Vector3D localExit3D(localExitX,localExitY,localExitZ);

      // the pixel positions and other needed stuff for the geometrical clustering
      std::vector<Amg::Vector2D>        pixelPositions;
      std::vector<Identifier>           rdoList;
      std::vector<int>                  totList;
      std::map<Identifier, double>      surfaceChargeWeights;

      // Pixel Design needed -------------------------------------------------------------
      const InDetDD::PixelModuleDesign* design
        (dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design()));

      if (!design) {
        ATH_MSG_DEBUG ( "Could not get design"<< design) ;
        continue;
      }


      const Amg::Vector3D localDirection(localExitX-localEntryX, localExitY-localEntryY, localExitZ-localEntryZ);
      // path length statistics
      double pathGeom     = localDirection.mag();    // geometrical path length
      double pathStep     = 0.;                         // path calculated through stepping
      double pathDrift    = 0.;                         // path calculated through drift charge
      double pathUsed     = 0.;                         // path used (contains smearing & cut if applied)

      const double distX = localExitX-localEntryX;
      const double distY = localExitY-localEntryY;
      const double distZ = localExitZ-localEntryZ;
      // relational slope
      const double slopeYX = distY/distX;
      const double slopeXZ = distX/distZ;
      //double slopeZX = distZ/distX;
      // signs of stepping
      const int signX = distX > 0. ? 1 : -1 ;
      const int signY = distY > 0. ? 1 : -1 ;
      const int signZ = distZ > 0. ? 1 : -1 ;

      // get the identifier of the entry and the exit
      Identifier entryId = hitSiDetElement->identifierOfPosition(localEntry);
      Identifier exitId  = hitSiDetElement->identifierOfPosition(localExit);
      // now get the cellIds and check whether they're valid
      InDetDD::SiCellId entryCellId = hitSiDetElement->cellIdFromIdentifier(entryId);
      InDetDD::SiCellId exitCellId = hitSiDetElement->cellIdFromIdentifier(exitId);
      // entry / exit validity
      const bool entryValid = entryCellId.isValid();
      const bool exitValid  = exitCellId.isValid();

      // the intersecetion id and cellId of it
      const double par = -localEntryZ/(localExitZ-localEntryZ);
      const double interX = localEntryX + par*(localExitX-localEntryX);
      const double interY = localEntryY + par*(localExitY-localEntryY);
      // double interX = 0.5*(localEntryX+localExitX);
      // double interY = 0.5*(localEntryY+localExitY);
      const Amg::Vector2D intersection(interX,interY);
      const Identifier intersectionId            =  hitSiDetElement->identifierOfPosition(intersection);
      const InDetDD::SiCellId intersectionCellId = hitSiDetElement->cellIdFromIdentifier(intersectionId);

      // the delta ranges
      int deltaRawPhi = 0;
      // apply the lorentz correction
      const double tanLorAng = m_pixTanLorentzAngleScalor*hitSiDetElement->getTanLorentzAnglePhi();
      const int lorentzDirection = tanLorAng > 0. ? 1 : -1;
      // shift parameters
      const double shift = hitSiDetElement->getLorentzCorrection();
      // validation filling

      // lorenz angle effects : offset goes against the lorentzAngle
      const double xLoffset  = -lorentzDirection*thickness*tanLorAng;

      // --------------------------------------------------------------------------------------
      // fast exit: skip non-valid entry && exit
      if ((!entryValid && !exitValid)){
        continue;
        // fast exit 1 - sub % lorentz effect, don't care about that
      } else if ( ( ( entryCellId == exitCellId && entryCellId.isValid())  || (distX*distX < 10e-5) ) && (fabs(tanLorAng) < 0.01) ) {
        // get the current position
        const Amg::Vector2D currentCenterPosition = hitSiDetElement->rawLocalPositionOfCell(intersectionCellId);
        // fast exit 2 - single hit / or diffused, drifted second hit in next phi pixel
        pixelPositions.push_back(currentCenterPosition);
        // the weight is the full step
        pathGeom  = localDirection.mag();
        surfaceChargeWeights[intersectionId] = pathGeom;
        // the current diode parameters
        const InDetDD::SiDiodesParameters currentDiodeParameters = design->parameters(intersectionCellId);
        // the currentPitchY
        //double currentPitchY = currentDiodeParameters.width().xEta();
      } else {
        // the entry and exit index in phi
        const int entryPhiIndex = entryCellId.phiIndex();
        const int exitPhiIndex  = exitCellId.phiIndex();
        // the exit index for stopping the loop
        const int exitEtaIndex = exitCellId.etaIndex();
        // get the maximum number of rows
        const int numberOfDiodesPhi = design->rows();
        const int numberOfDiodesEta = design->columns();
        // needed for a veto later on
        deltaRawPhi = exitPhiIndex-entryPhiIndex;
        // relational slopes
        const double slopeXY = distX/distY;
        const double slopeZX = distZ/distX;
        // raw (i.e. before cut) pixel sizes
        int phiIndexMaxRaw = -10000;
        int phiIndexMinRaw =  10000;
        int etaIndexMaxRaw = -10000;
        int etaIndexMinRaw =  10000;
        // ============================== the clusteristiaon core ========================================================
        // there are two independent loops
        // (a) the geometrical steps through the pixels : labelled current
        Identifier          currentId       = entryId;
        InDetDD::SiCellId   currentCellId   = entryCellId;
        Amg::Vector2D currentCenterPosition = hitSiDetElement->rawLocalPositionOfCell(currentCellId);
        Amg::Vector3D currentPosition3D(interX-0.5*distX,interY-0.5*distY,-signZ*0.5*thickness);
        double              currentStepX    = 0.;
        double              currentStepY    = 0.;
        double              currentPitchX   = 0.;
        double              currentPitchY   = 0.;
        Amg::Vector3D       currentStep3D(0.,0.,0.);
        Amg::Vector2D       currentCenterStep(0.,0.);
        // check for non-valid entry diode ... this needs to be reset then
        if (!entryValid){
          // find out where to go - we know that at least the exit is valid
          const bool outsidePhi = exitPhiIndex <= 2 || exitPhiIndex >= numberOfDiodesPhi-2;
          // now make the step to the border
          double stepExitX = 0.;
          double stepExitY = 0.;
          double stepExitZ = 0.;
          // calculate the sensitive border parameters
          if (outsidePhi){
            // get the last valid phi border
            const int phiExitIndex   = exitPhiIndex <= 2 ? 0 : numberOfDiodesPhi-1;
            const InDetDD::SiCellId  phiExitId(phiExitIndex,0);
            const Amg::Vector2D phiExitCenter = hitSiDetElement->rawLocalPositionOfCell(phiExitId);
            const double xExitPosition = phiExitCenter.x()-signX*0.5*hitSiDetElement->phiPitch();
            // fill the step parameters
            stepExitX = xExitPosition-localEntryX;
            stepExitY = stepExitX*slopeYX;
            stepExitZ = stepExitX*slopeZX;
          } else {
            // get the last valid eta border
            const int etaExitIndex = exitEtaIndex <= 2 ? 0 : numberOfDiodesEta-1;
            const InDetDD::SiCellId  etaExitId(0,etaExitIndex);
            const Amg::Vector2D etaExitCenter = hitSiDetElement->rawLocalPositionOfCell(etaExitId);
            const InDetDD::SiDiodesParameters eixtDiodeParameters = design->parameters(etaExitId);
            const double exitPitchY    = eixtDiodeParameters.width().xEta();
            const double yExitPosition = etaExitCenter.y()-signY*0.5*exitPitchY;
            // fill the step parameters
            stepExitY = yExitPosition-localEntryY;
            stepExitX = stepExitY*slopeXY;
            stepExitZ = stepExitX*slopeZX;
          }
          // get to the first valid pixel
          const Amg::Vector3D  stepExitToFirstValid(stepExitX,stepExitY,stepExitZ);
          currentPosition3D += stepExitToFirstValid;
          pathStep += stepExitToFirstValid.mag();
          // the necessary step into the pixel for the resetting of start parameters
          // for simplicity step twice the phiPitch it's safe !
          const Amg::Vector2D positionInFirstValid(currentPosition3D.x()+signX*0.5*hitSiDetElement->phiPitch(),
                                             currentPosition3D.y()+signY*0.5*hitSiDetElement->phiPitch());
          // reset the entry parameters to the first valid pixel
          currentId       = hitSiDetElement->identifierOfPosition(positionInFirstValid);
          currentCellId   = hitSiDetElement->cellIdFromIdentifier(currentId);
          currentCenterPosition = hitSiDetElement->rawLocalPositionOfCell(currentCellId);

        }

        // (b) the collection of pixels due to surface charge
        //     there is a critical break in (b) when the track is above the lorentz angle plane
        //      however, there are tracks that will never go above that plane (i.e. tan(track) > tan(lorentz))
        // the lorentz plane setp
        double         lplaneStepX = 0.;
        double         lplaneStepY = 0.;
        Amg::Vector3D  lplaneIntersection(0.,0.,0.);
        Amg::Vector3D  driftPrePosition3D(currentPosition3D);
        Amg::Vector3D  driftPostPosition3D(currentPosition3D);
        // (c) simple cache for pure geometrical clustering
        Identifier     lastId  = currentId;

        int n = 0;
        for ( ; ; ){
          int estimatedDirection = 0;
          // break the loop
          if (!exitValid && !currentCellId.isValid()) break;
          // these are numerical instabilities
          if (pathStep > pathGeom) break;
          // current phi/eta indices
          const int currentPhiIndex = currentCellId.phiIndex();
          const int currentEtaIndex = currentCellId.etaIndex();
          // cache it
          phiIndexMinRaw = currentPhiIndex < phiIndexMinRaw ?  currentPhiIndex : phiIndexMinRaw;
          phiIndexMaxRaw = currentPhiIndex > phiIndexMaxRaw ?  currentPhiIndex : phiIndexMaxRaw;
          etaIndexMinRaw = currentEtaIndex < etaIndexMinRaw ?  currentEtaIndex : etaIndexMinRaw;
          etaIndexMaxRaw = currentEtaIndex > etaIndexMaxRaw ?  currentEtaIndex : etaIndexMaxRaw;
          // find out if this is the last step
          const bool lastInPhi = (currentPhiIndex == exitPhiIndex);
          const bool lastInEta = (currentEtaIndex == exitEtaIndex);
          const bool lastPixel = lastInPhi && lastInEta;
          // diode parameters for the phi/eta pitch
          const InDetDD::SiDiodesParameters currentDiodeParameters = design->parameters(currentCellId);
          currentPitchX = currentDiodeParameters.width().xPhi();
          currentPitchY = currentDiodeParameters.width().xEta();
          // calculate the potential step in X and Y
          const double stepInX  = lastInPhi ? localExitX-currentPosition3D.x() :
            0.5*currentPitchX+signX*(currentCenterPosition.x()-currentPosition3D.x());
          const double stepInY         = lastInEta ? localExitY - currentPosition3D.y() :
            0.5*currentPitchY+signY*(currentCenterPosition.y()-currentPosition3D.y());
          // check whether the direction needs to be estimated first
          if (!estimatedDirection){
            // some magic here to find out whether an xstep of y step needs to be applied
            const double endY    =  currentPosition3D.y() + signX*stepInX*slopeYX;
            const double cornerYpos = currentCenterPosition.y() + (0.5*currentPitchY);
            const double cornerYneg = currentCenterPosition.y() - (0.5*currentPitchY);
            // stay within the y boundaries while performing the x step
            if (endY > cornerYneg && endY < cornerYpos && deltaRawPhi) estimatedDirection = 1;
            else estimatedDirection = 2;
          }
          // record for the full validation
          // (a) steps through the pixels
          // the next & previous pixel borders are needed (next is w.r.t to the track direction)
          std::vector<double> lorentzLinePositions;
          const int trackDir = slopeXZ > 0 ? 1 : -1;
          lorentzLinePositions.reserve(3);
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
            const Trk::LineIntersection2D intersectLorentzPlane(localEntryX,-0.5*signZ*thickness,localExitX,0.5*signZ*thickness,
                                                          (*lorentzLinePosIter)+xLoffset,-0.5*hitDepth*thickness,
                                                          (*lorentzLinePosIter),0.5*hitDepth*thickness);
            // avoid repeating intersections
            const double formerPlaneStepZ = intersectLorentzPlane.interY-lplaneIntersection.z();
            if (formerPlaneStepZ*formerPlaneStepZ < 10e-5){
              lplaneInterInCurrent = false;
              continue; // jumps to the new lorentz intersection candidate
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
              const double distToNextLineY = currentCenterPosition.y() + 0.5*currentPitchY - lplaneCandidate.y();
              const double distToPrevLineY = currentCenterPosition.y() - 0.5*currentPitchY - lplaneCandidate.y();
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
            const double trackZatlplaneHalfX = (lorentzPlaneHalfX-localEntryX)*slopeXZ - 0.5*thickness;
            lplaneDirection = trackZatlplaneHalfX < 0. ? -1 : 1;
          }
          // record the lorentz plane intersections
          // perform the x step -------------------------------------------------------------------
          if (estimatedDirection==1) {
            ATH_MSG_VERBOSE( "[ cluster - pix ] Perform a step in x, direction: " << signX );
            // calculate the 2D weight
            currentStepX = signX*stepInX;
            currentStepY = signX*stepInX*slopeYX;
            // recalculate if necessary
            currentCenterStep = Amg::Vector2D(signX*currentPitchX,0.);
            // perform the y step -------------------------------------------------------------------
          } else if (estimatedDirection==2){
            ATH_MSG_VERBOSE( "[ cluster - pix ] Perform a step in y, direction: " << signY );
            // the magic numbers are for the long strip positions
            double nextPitchY = currentPitchY;
            // new eta index
            const int nextEtaIndex = currentEtaIndex+signY;
            // check on range
            if (nextEtaIndex >= 0 && nextEtaIndex < numberOfDiodesEta){
              const InDetDD::SiCellId nextCellId(currentPhiIndex,nextEtaIndex);
              const InDetDD::SiDiodesParameters nextDiodeParameters = design->parameters(nextCellId);
              nextPitchY = nextDiodeParameters.width().xEta();
            }
            // calculate the 2D weight
            currentStepX = signY*stepInY*slopeXY;
            currentStepY = signY*stepInY;
            // recalculate if necessary
            currentCenterStep = Amg::Vector2D(0.,signY*(0.5*currentPitchY+0.5*nextPitchY));
          }
          // the step to the pixel border
          currentStep3D = lastPixel ?
            Amg::Vector3D(localExitX-currentPosition3D.x(),
                          localExitY-currentPosition3D.y(),
                          0.5*signZ*thickness-currentPosition3D.z()) :
            Amg::Vector3D(currentStepX,currentStepY,currentStepX*slopeZX);
          // update the current values for the next step
          currentCenterPosition += currentCenterStep;
          currentId              = hitSiDetElement->identifierOfPosition(currentCenterPosition);
          currentCellId          = hitSiDetElement->cellIdFromIdentifier(currentId);
          // The new current Position && the path length for monitoring
          pathStep          += currentStep3D.mag();
          currentPosition3D += currentStep3D;
          // set the drift positions
          driftPostPosition3D = lplaneInterInCurrent ? lplaneIntersection : currentPosition3D;
          // ---------- the surface charge is emulated -------------------------------------------------------
          if (m_pixEmulateSurfaceCharge){
            // loop to catch lpintersection in last step
            const int currentDrifts = (lastPixel && lplaneInterInCurrent) ? 2 : 1;
            for (int idrift = 0; idrift < currentDrifts; ++idrift){
              // const assignment for fast access, take intersection solution first, then cell exit
              const Amg::Vector3D& currentDriftPrePosition  = idrift ? driftPostPosition3D : driftPrePosition3D;
              const Amg::Vector3D& currentDriftPostPosition = idrift ? localExit3D : driftPostPosition3D;
              // get the center of the step and drift it to the surface
              const Amg::Vector3D driftStepCenter = 0.5*(currentDriftPrePosition+currentDriftPostPosition);
              // respect the reaout side through the hit dept direction
              const double driftZtoSurface     = 0.5*hitDepth*thickness-driftStepCenter.z();
              const Amg::Vector2D driftAtSurface(driftStepCenter.x() + tanLorAng*driftZtoSurface,
                                           driftStepCenter.y());
              const Identifier surfaceChargeId = hitSiDetElement->identifierOfPosition(driftAtSurface);
              if (surfaceChargeId.is_valid()){
                // check if the pixel has already got some charge
                if (surfaceChargeWeights.find(surfaceChargeId) == surfaceChargeWeights.end())
                  surfaceChargeWeights[surfaceChargeId] = (currentDriftPostPosition-currentDriftPrePosition).mag();
                else
                  surfaceChargeWeights[surfaceChargeId] += (currentDriftPostPosition-currentDriftPrePosition).mag();
              }
            } // end of last step intersection check
          } else // ---------- purely geometrical clustering w/o surface charge ---------------------------
            surfaceChargeWeights[lastId] = currentStep3D.mag();
          // next pre is current post && lastId is currentId
          driftPrePosition3D = driftPostPosition3D;
          lastId = currentId;
          // -------------------------------------------------------------------------------------------------
          if (lastPixel )
            break;
          if (currentPosition3D.z() > 0.5*thickness)
            break;

          n += 1;
          if (n > 100) {
            ATH_MSG_WARNING ( "PixelFastDigitizationTool::digitize() n > 100 - bailing out!!" );
            break;
          }

        } // end of clusterisation core  =========================================================================

      } // end of n-dimensional cluster


      // create the cluster parameters
      Amg::Vector2D       clusterPosition(0.,0.);
      bool                isGanged = false;
      bool                hasBorderPixel = false; // check if pixel at border has been hit
      Identifier gangedChargeId;
      double chargeWeightG = 0;
      InDetDD::SiCellId chargeCellIdG;

      int gangedTot;
      std::vector<Identifier> rdoListGanged;
      std::vector<int> totListGanged;
      // for the validation
      int phiIndexMax = -10000;
      int phiIndexMin =  10000;
      int etaIndexMax = -10000;
      int etaIndexMin =  10000;

      std::map<Identifier,double>::iterator weightIter =  surfaceChargeWeights.begin();
      for ( ; weightIter != surfaceChargeWeights.end(); ++weightIter){
        // map content
        Identifier chargeId = weightIter->first;
        InDetDD::SiCellId chargeCellId =  hitSiDetElement->cellIdFromIdentifier(chargeId);
        double chargeWeight = weightIter->second;
        pathDrift += chargeWeight;
        // treshold & pixel position
        Amg::Vector2D chargeCenterPosition = hitSiDetElement->rawLocalPositionOfCell(chargeId);
        // verbose output
        ATH_MSG_VERBOSE("[ cluster - pix] Adding " << chargeCenterPosition.x() << ", " << chargeCenterPosition.y()
                        << " with weight :" << chargeWeight);

        // two options fro charge smearing: landau / gauss
        if ( m_pixSmearPathLength > 0. ) {
          // create the smdar parameter
          double sPar = m_pixSmearLandau ?
            m_pixSmearPathLength*CLHEP::RandLandau::shoot(m_randomEngine) :
            m_pixSmearPathLength*CLHEP::RandGauss::shoot(m_randomEngine);
          chargeWeight *=  (1.+sPar);
        }

        // check the treshold for the minimal path cut
        bool aboveThreshold = chargeWeight > m_pixMinimalPathCut;
        // record for validation
        if (!aboveThreshold) continue;
        // phi/ eta index
        int chargePhiIndex = chargeCellId.phiIndex();
        int chargeEtaIndex = chargeCellId.etaIndex();
        // set max min
        phiIndexMin = chargePhiIndex < phiIndexMin ?  chargePhiIndex : phiIndexMin;
        phiIndexMax = chargePhiIndex > phiIndexMax ?  chargePhiIndex : phiIndexMax;
        etaIndexMin = chargeEtaIndex < etaIndexMin ?  chargeEtaIndex : etaIndexMin;
        etaIndexMax = chargeEtaIndex > etaIndexMax ?  chargeEtaIndex : etaIndexMax;
        // get number of columns per circuit
        int numberOfCircuitsEta = design->columnsPerCircuit();
        // check if the cluster has a border pixel
        hasBorderPixel = hasBorderPixel ? hasBorderPixel : (!(chargeEtaIndex % numberOfCircuitsEta) || !((chargeEtaIndex+1) % numberOfCircuitsEta));
        // update the cluster Position
        clusterPosition += chargeWeight * chargeCenterPosition;
        pathUsed += chargeWeight;
        // record - positions, rdoList and totList
        pixelPositions.push_back(chargeCenterPosition);
        rdoList.push_back(chargeId);
        totList.push_back(int(chargeWeight*m_pixPathLengthTotConv));

        isGanged = isGanged ? isGanged : (hitSiDetElement->numberOfConnectedCells(chargeCellId) > 1);
        bool pixGanged = (hitSiDetElement->numberOfConnectedCells(chargeCellId) > 1);
        if (pixGanged) {gangedChargeId = chargeId; gangedTot = int(chargeWeight*m_pixPathLengthTotConv);           rdoListGanged.push_back(gangedChargeId);
            totListGanged.push_back(gangedTot); chargeWeightG = chargeWeight; chargeCellIdG = hitSiDetElement->gangedCell(chargeCellId);}


      }

      // the col/row
      m_siDeltaPhiCut = phiIndexMax-phiIndexMin+1;
      m_siDeltaEtaCut = etaIndexMax-etaIndexMin+1;

      // bail out if 0 pixel or path length problem
      if (!rdoList.size() || pathUsed < m_pixMinimalPathCut) {
        continue;
      }

      // weight the cluster position
      clusterPosition *= 1./pathUsed;
      Identifier clusterId = hitSiDetElement->identifierOfPosition(clusterPosition);
      int lvl1a = 0; // needs to be checked with Pixel experts


      // merging clusters

      bool merged = false;

      for(Pixel_detElement_RIO_map::iterator currentClusIter = PixelDetElClusterMap.begin(); currentClusIter != PixelDetElClusterMap.end();) {
        //make a temporary to use within the loop and possibly erase - increment the main interator at the same time.
        Pixel_detElement_RIO_map::iterator clusIter = currentClusIter++;
        InDet::PixelCluster* currentCluster = clusIter->second;
        std::vector<Identifier> currentRdoList = currentCluster->rdoList();
        bool areNb = false;
        for (std::vector<Identifier>::const_iterator rdoIter = rdoList.begin(); rdoIter != rdoList.end(); ++rdoIter) {
          areNb = this->areNeighbours(currentRdoList, *rdoIter, const_cast<InDetDD::SiDetectorElement*>(hitSiDetElement),*m_pixel_ID);
          if (areNb) { break; }
        }
        if (areNb) {
            std::vector<int> currentTotList = currentCluster->totList();
            rdoList.insert(rdoList.end(), currentRdoList.begin(), currentRdoList.end() );
            totList.insert(totList.end(), currentTotList.begin(), currentTotList.end() );
            Amg::Vector2D       currentClusterPosition(currentCluster->localPosition());
            float c1 = (float)currentRdoList.size();
            float c2 = (float)rdoList.size();
            clusterPosition = (clusterPosition*c2 + currentClusterPosition*c1)/((c1 + c2));
            clusterId = hitSiDetElement->identifierOfPosition(clusterPosition);
            merged = true;
            PixelDetElClusterMap.erase(clusIter);
            delete currentCluster;
           //break; //commenting out this break statement allows for multiple existing clusters to be merged.
        }
      }

      bool not_valid = false;
      for (unsigned int entry=0; entry < rdoList.size(); entry++) {
        if (!(rdoList[entry].is_valid())) { not_valid = true; break;}
      }
      if (not_valid) continue;

      if(merged) {
        //Hacks for merged clusters
        for (std::vector<Identifier>::const_iterator rdoIter = rdoList.begin(); rdoIter != rdoList.end(); ++rdoIter) {
          const InDetDD::SiCellId& chargeCellId =  hitSiDetElement->cellIdFromIdentifier(*rdoIter);
          // phi/ eta index
          int chargePhiIndex = chargeCellId.phiIndex();
          int chargeEtaIndex = chargeCellId.etaIndex();
          // set max min
          phiIndexMin = chargePhiIndex < phiIndexMin ?  chargePhiIndex : phiIndexMin;
          phiIndexMax = chargePhiIndex > phiIndexMax ?  chargePhiIndex : phiIndexMax;
          etaIndexMin = chargeEtaIndex < etaIndexMin ?  chargeEtaIndex : etaIndexMin;
          etaIndexMax = chargeEtaIndex > etaIndexMax ?  chargeEtaIndex : etaIndexMax;
        }
        m_siDeltaPhiCut = (phiIndexMax-phiIndexMin)+1;
        m_siDeltaEtaCut = (etaIndexMax-etaIndexMin)+1;
      }

      Amg::Vector2D gangedChargeCenterPosition(0.,0.);
      Amg::Vector2D gangedClusterPosition(0.,0.);

      bool prodGanged = true;
      if (isGanged && prodGanged && !merged) {

        gangedChargeCenterPosition = hitSiDetElement->rawLocalPositionOfCell(chargeCellIdG);
        gangedClusterPosition += chargeWeightG * gangedChargeCenterPosition;

        gangedClusterPosition *= 1./pathUsed;
        Identifier gangedClusterId = hitSiDetElement->identifierOfPosition(gangedClusterPosition);
        double etaWidth = design->widthFromColumnRange(etaIndexMin, etaIndexMax);
        double phiWidth = design->widthFromRowRange(phiIndexMin, phiIndexMax);

        InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(m_siDeltaPhiCut,m_siDeltaEtaCut),
                                                     Amg::Vector2D(phiWidth,etaWidth) );
        int lvl1a = 0; // needs to be checked with Pixel experts

        InDet::PixelCluster* pixelClusterGanged = 0;

        pixelClusterGanged = m_clusterMaker->pixelCluster(gangedClusterId,
                                                          gangedClusterPosition,
                                                          rdoListGanged,
                                                          lvl1a,
                                                          totListGanged,
                                                          *siWidth,
                                                          hitSiDetElement,
                                                          true,
                                                          m_pixErrorStrategy,
                                                          *m_pixel_ID);


        prodGanged = false;

        pixelClusterGanged->setGangedPixel(isGanged);
        if(!(pixelClusterGanged->identify().is_valid())) {
          delete pixelClusterGanged;
          continue;
        }

        if (! (m_pixel_ID->is_pixel(pixelClusterGanged->identify()))) {delete pixelClusterGanged; continue;}

//        if ((pixelClusterGanged->identify()) == 0xfffff4402fffffffULL) continue;

        (void) PixelDetElClusterMap.insert(Pixel_detElement_RIO_map::value_type(waferID, pixelClusterGanged));
        //Ganged = true;


      }


      // ---------------------------------------------------------------------------------------------
      //  PART 2: Cluster && ROT creation
      //
      InDet::PixelCluster* pixelCluster = 0;
      // from InDetReadoutGeometry: width from eta
      double etaWidth = design->widthFromColumnRange(etaIndexMin, etaIndexMax);
      // from InDetReadoutGeometry : width from phi
      double phiWidth = design->widthFromRowRange(phiIndexMin, phiIndexMax);

      InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(m_siDeltaPhiCut,m_siDeltaEtaCut),
                                                   Amg::Vector2D(phiWidth,etaWidth) );
      // correct shift implied by the scaling of the Lorentz angle
      double newshift = 0.5*thickness*tanLorAng;
      double corr = ( shift - newshift );
      // 2a) Cluster creation ------------------------------------
      if (m_pixUseClusterMaker){
        // correct for the shift that will be applied in the cluster maker
        // (only if surface charge emulation was switched off )
        if (!m_pixEmulateSurfaceCharge && shift*shift > 0.)
          clusterPosition -= Amg::Vector2D(shift,0.);
        // safe to compare m_pixTanLorentzAngleScalor with 1. since it is set not calculated
        else if (m_pixTanLorentzAngleScalor != 1. && shift*shift > 0.)
          clusterPosition += Amg::Vector2D(corr,0.);
        // use the distortion tool
        if (m_pixModuleDistortion &&  hitSiDetElement->isBarrel() )
          clusterPosition = m_pixDistortionTool->correctSimulation(hitSiDetElement->identify(), clusterPosition, localDirection);

        // use the cluster maker from the offline software
        pixelCluster = m_clusterMaker->pixelCluster(clusterId,
                                                    clusterPosition,
                                                    rdoList,
                                                    lvl1a,
                                                    totList,
                                                    *siWidth,
                                                    hitSiDetElement,
                                                    isGanged,
                                                    m_pixErrorStrategy,
                                                    *m_pixel_ID);
        if (isGanged)  pixelCluster->setGangedPixel(isGanged);
      } else if (m_pixPhiError.size() && m_pixEtaError.size()) {
        // create your own cluster
        // you need to correct for the lorentz drift -- only if surface charge emulation was switched on
        double appliedShift = m_pixEmulateSurfaceCharge ? m_pixTanLorentzAngleScalor*shift : (1.-m_pixTanLorentzAngleScalor)*shift;
        Amg::Vector2D*  lcorrectedPosition = new Amg::Vector2D(clusterPosition.x()+appliedShift,clusterPosition.y());
        // create the parameterised error
        double sigmaPhi = m_siDeltaPhiCut-1 < int(m_pixPhiError.size()) ?
          m_pixPhiError[m_siDeltaPhiCut-1] : m_pixPhiError[m_pixPhiError.size()-1];
        double sigmaEta = m_siDeltaEtaCut-1 < int(m_pixEtaError.size()) ?
          m_pixEtaError[m_siDeltaEtaCut-1] : m_pixEtaError[m_pixEtaError.size()-1];
        // covariance matrix && error description
        AmgSymMatrix(2) covariance;
        covariance.setIdentity();
        covariance(Trk::locX,Trk::locX) = sigmaPhi*sigmaPhi;
        covariance(Trk::locY,Trk::locY) = sigmaEta*sigmaEta;
        Amg::MatrixX* clusterErr = new Amg::MatrixX(covariance);
        // create the cluster
        pixelCluster = new InDet::PixelCluster(clusterId,
                                               *lcorrectedPosition,
                                               rdoList,
                                               lvl1a,
                                               totList,
                                               *siWidth,
                                               hitSiDetElement,
                                               clusterErr);
        //if (isGanged)  pixelCluster->setGangedPixel(isGanged); //FIXME is this needed here?
      } else {
        ATH_MSG_WARNING("[ cluster - pix ] No pixels errors provided, but configured to use them.");
        ATH_MSG_WARNING("                  -> No pixels cluster will be created.");
        continue;
      }

      if(!(pixelCluster->identify().is_valid()))
        {
          delete pixelCluster;
          continue;
        }

      if (! (m_pixel_ID->is_pixel(pixelCluster->identify()))) {delete pixelCluster; continue;}

      (void) PixelDetElClusterMap.insert(Pixel_detElement_RIO_map::value_type(waferID, pixelCluster));

      if (hit->particleLink().isValid()){
        const int barcode( hit->particleLink().barcode());
        if ( barcode !=0 && barcode != m_vetoThisBarcode ) {
          m_pixPrdTruth->insert(std::make_pair(pixelCluster->identify(), hit->particleLink()));
          ATH_MSG_DEBUG("Truth map filled with cluster" << pixelCluster << " and link = " << hit->particleLink());
        }
      }else{
        ATH_MSG_DEBUG("Particle link NOT valid!! Truth map NOT filled with cluster" << pixelCluster << " and link = " << hit->particleLink());
      }

    } // end hit while

    (void) m_pixelClusterMap->insert(PixelDetElClusterMap.begin(), PixelDetElClusterMap.end());


  } // end nextDetectorElement while


  return StatusCode::SUCCESS;
}

StatusCode PixelFastDigitizationTool::createAndStoreRIOs()
{

  Pixel_detElement_RIO_map::iterator i = m_pixelClusterMap->begin();
  Pixel_detElement_RIO_map::iterator e = m_pixelClusterMap->end();

  InDet::PixelClusterCollection* clusterCollection = 0;
  IdentifierHash waferID;

  for (; i != e; i = m_pixelClusterMap->upper_bound(i->first)){

    std::pair <Pixel_detElement_RIO_map::iterator, Pixel_detElement_RIO_map::iterator> range;
    range = m_pixelClusterMap->equal_range(i->first);

    Pixel_detElement_RIO_map::iterator firstDetElem;
    firstDetElem = range.first;

    IdentifierHash waferID;
    waferID = firstDetElem->first;

    const InDetDD::SiDetectorElement* detElement = m_manager->getDetectorElement(waferID);

    clusterCollection = new InDet::PixelClusterCollection(waferID);
    clusterCollection->setIdentifier(detElement->identify());


    for ( Pixel_detElement_RIO_map::iterator iter = range.first; iter != range.second; ++iter){

      InDet::PixelCluster* pixelCluster = const_cast<InDet::PixelCluster*>((*iter).second);
      pixelCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
      clusterCollection->push_back(pixelCluster);

    }


    if (clusterCollection) {
      if (clusterCollection->size() != 0) {
        ATH_MSG_DEBUG ( "Filling ambiguities map" );
        m_gangedAmbiguitiesFinder->execute(clusterCollection,*m_manager,*m_ambiguitiesMap);
        ATH_MSG_DEBUG ( "Ambiguities map: " << m_ambiguitiesMap->size() << " elements" );
        if ((m_pixelClusterContainer->addCollection(clusterCollection, waferID)).isFailure()){
          ATH_MSG_WARNING( "Could not add collection to Identifyable container !" );
        }
      }
      else {delete clusterCollection;}
    }


  } // end for

  m_pixelClusterMap->clear();

  return StatusCode::SUCCESS;
}


/*// copied from FATRAS
bool PixelFastDigitizationTool::isActiveAndGood(const ServiceHandle<IInDetConditionsSvc> &svc, const IdentifierHash &idHash, const Identifier &id, bool querySingleChannel, const char * elementName, const char * failureMessage) const
{
  //     if (!m_useConditions) return true; // shortcut: assume all elements to be active and good when not using conditions services

  bool isActive, isGood;
  if (!querySingleChannel) {
    isActive = svc->isActive(idHash); // active = "element returns data"
    isGood   = svc->isGood(idHash);   // good   = "data are reliable"
  } else {
    // Christoph Ruwiedel says: When querying a single channel, use both the IdentifierHash (of the element) and the Identifier (of the channel).
    isActive = svc->isActive(idHash, id); // active = "element returns data"
    isGood   = svc->isGood(idHash, id);   // good   = "data are reliable"
  }

  return isActive && isGood;
} */

// copied from PixelClusteringToolBase
bool PixelFastDigitizationTool::areNeighbours
(const std::vector<Identifier>& group,
 const Identifier& rdoID,
 InDetDD::SiDetectorElement* /*element*/,
 const PixelID& pixelID) const
{

  int m_splitClusters = 0;
  int m_acceptDiagonalClusters = 1;

  std::vector<Identifier>::const_iterator groupBegin = group.begin();
  std::vector<Identifier>::const_iterator groupEnd = group.end();

  //Identifier elementID = element->identify();

  int row2 = pixelID.phi_index(rdoID);
  int col2 = pixelID.eta_index(rdoID);
  int maxZsize=999;
  if(m_splitClusters == 1){
    int etamodule = pixelID.eta_module(rdoID);
    int isBarrel = pixelID.barrel_ec(rdoID);
    if(isBarrel == 0){
      // this goes from 0 (central stave) to 6
      int dz = abs(etamodule-6);
      if(dz<4){ maxZsize=2; }
      else{ maxZsize=3; }
    }
    else{
      maxZsize = 2;
    }
    if(maxZsize > 3){
      ATH_MSG_WARNING (" areNeighbours - problems ");
    }
  }
  if(m_splitClusters == 2) maxZsize = 2;

  int rowmin = row2;
  int rowmax = row2;
  int colmin = col2;
  int colmax = col2;
  bool match=false;
  while (groupBegin!=groupEnd)
    {
      Identifier id = *groupBegin;
      int row1 = pixelID.phi_index(id);
      int col1 = pixelID.eta_index(id);
      if(row1 > rowmax) rowmax = row1;
      int deltarow = abs(row2-row1);
      int deltacol = abs(col2-col1);

      // a side in common
      if(deltacol+deltarow < 2) match = true;
      if(m_acceptDiagonalClusters != 0 && deltacol == 1
         && deltarow == 1) match = true;

      // check cluster size
      if(m_splitClusters != 0){
        if(row1 > rowmax) rowmax = row1;
        if(row1 < rowmin) rowmin = row1;
        if(col1 > colmax) colmax = row1;
        if(col1 < colmin) colmin = row1;
      }

      ++groupBegin;
    }
  if(match && m_splitClusters != 0){
    if(colmax-colmin > maxZsize-1) match = false;
    if(rowmax-rowmin > 1) match = false;
  }

  return match;
}
