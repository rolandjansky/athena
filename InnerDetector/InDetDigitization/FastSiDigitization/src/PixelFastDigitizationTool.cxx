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

// Gaudi includes
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SystemOfUnits.h"

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

//Package For New Tracking:
// Amg includes
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
// Trk includes

#include "TrkDigEvent/RectangularSegmentation.h"
#include "TrkDigEvent/TrapezoidSegmentation.h"

#include "TrkDigEvent/DigitizationCell.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/SurfaceCollection.h"


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
  //m_pixMinimalPathCut(0.0 * Gaudi::Units::micrometer),
  m_pixMinimalPathCut(0.02),// Optimized choice of threshold
  m_pixPathLengthTotConv(125.),
  m_pixModuleDistortion(true), // default: false
  m_pixDistortionTool("PixelDistortionsTool/PixelDistortionsTool"),
  m_pixErrorStrategy(2),
  m_mergeCluster(true),
  m_splitClusters(0),
  m_acceptDiagonalClusters(1),
  m_pixelClusterAmbiguitiesMapName("PixelClusterAmbiguitiesMap"),
  m_ambiguitiesMap(0),
  m_digitizationStepper("Trk::PlanarModuleStepper"),
  m_DiffusionShift(0.004)
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
  declareProperty("DigitizationStepper",     m_digitizationStepper);
  declareProperty("DiffusionShif", m_DiffusionShift);
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
                                                       PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                                                       PileUpEventInfo::SubEvent::const_iterator eSubEvents)
{

  m_seen.push_back(std::make_pair(std::distance(bSubEvents,eSubEvents), bunchXing));
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
  PileUpEventInfo::SubEvent::const_iterator iEvt(bSubEvents);
  while (iEvt != eSubEvents) {
    StoreGateSvc& seStore(*iEvt->pSubEvtSG);
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


StatusCode PixelFastDigitizationTool::digitize()
{


  TimedHitCollection<SiHit>::const_iterator i, e;

  //bool Ganged = false;

  if(!m_pixelClusterMap) { m_pixelClusterMap = new Pixel_detElement_RIO_map; }
  else { m_pixelClusterMap->clear(); }

  while (m_thpcsi->nextDetectorElement(i, e)) {

    Pixel_detElement_RIO_map PixelDetElClusterMap;

    //std::cout<<"Next Detector element  "<<std::endl;
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
      
      double pixMinimalPathCut= 1. / m_pixPathLengthTotConv;
      
      //Avoid to store pixels with 0 ToT
      if (m_pixMinimalPathCut > pixMinimalPathCut) {
	pixMinimalPathCut=m_pixMinimalPathCut; }
      
      
     // std::cout<<"Next Hit  "<<std::endl;
      std::vector<HepMcParticleLink> hit_vector; //Store the hits in merged cluster

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
      
      
      //New function to tune the cluster size
      bool diffusion = Diffuse(localStartPosition, localEndPosition, m_DiffusionShift);

      const Amg::Vector3D localDirection(localEndPosition.x()-localStartPosition.x(), localEndPosition.y()-localStartPosition.y(), localEndPosition.z()-localStartPosition.z());
      
      Amg::Vector3D entryPoint(localStartPosition.x(),localStartPosition.y(),localStartPosition.z());
      Amg::Vector3D exitPoint(localEndPosition.x(),localEndPosition.y(),localEndPosition.z());

      const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design()));
      
      const Amg::Vector2D localEntry(localStartPosition.x(),localStartPosition.y());
      const Amg::Vector2D localExit(localEndPosition.x(),localEndPosition.y());
      
      Identifier entryId = hitSiDetElement->identifierOfPosition(localEntry);
      Identifier exitId  = hitSiDetElement->identifierOfPosition(localExit);
      
      InDetDD::SiCellId entryCellId = hitSiDetElement->cellIdFromIdentifier(entryId);
      InDetDD::SiCellId exitCellId = hitSiDetElement->cellIdFromIdentifier(exitId);
      
//       std::cout<<"Entry index eta e phi "<<entryCellId.etaIndex()<<" "<<entryCellId.phiIndex()<<std::endl;
//       std::cout<<"Exit index eta e phi "<<exitCellId.etaIndex()<<" "<<exitCellId.phiIndex()<<std::endl;
      
      double halfthickness = hitSiDetElement->thickness()/2.;
      
      bool EntryValid(entryCellId.isValid());
      bool ExitValid(exitCellId.isValid());
      
      if (!EntryValid || !ExitValid) 
      { 
	//If entry or exit aren't valid search for the valid entry/exit of the hit as Intersection with the module
	
	//std::cout<<"Valid ? "<<halfthickness<<" "<<localStartPosition.z()<<" "<<localEndPosition.z()<<std::endl;
	if ( !EntryValid && !ExitValid) continue;
	//std::cout<<"Yes "<<std::endl;
	
	Amg::Vector3D Point = EntryValid ? entryPoint : exitPoint ;
	
	Amg::Vector3D Intersection =  CalculateIntersection(Point, localDirection, Amg::Vector2D(design->width() * 0.5,design->length() * 0.5),halfthickness);
	
	if( Intersection == Amg::Vector3D(0.,0.,0.))
	{
	  ATH_MSG_WARNING("ATTENTION THE INTERSECTION COORDINATE IS OUT OF THE MODULE");
	  continue;
	}
	
	//std::cout<<"Intersection "<<Intersection<<std::endl;
	const Amg::Vector2D Intersection_2d(Intersection.x(),Intersection.y());
	Identifier Intersection_2dId = hitSiDetElement->identifierOfPosition(Intersection_2d);
	
	InDetDD::SiCellId Intersection_2dCellId = hitSiDetElement->cellIdFromIdentifier(Intersection_2dId);
	
	//std::cout<<"Intersection "<<Intersection_2dCellId.etaIndex()<<" "<<Intersection_2dCellId.phiIndex()<<std::endl;
	
	if(!Intersection_2dCellId.isValid()) continue;
	
	if(EntryValid) 
	  exitPoint = Intersection;
	else
	  entryPoint = Intersection;
	
	
      }
       
      

      Trk::DigitizationModule * m_digitizationModule = buildDetectorModule(hitSiDetElement);
      if(!m_digitizationModule){
	ATH_MSG_FATAL( " could not get build detector module "); 
	return StatusCode::FAILURE;
      }
      
      // Getting the steps in the sensor
      std::vector<Trk::DigitizationStep> digitizationSteps = m_digitizationStepper->cellSteps(*m_digitizationModule,entryPoint,exitPoint);

      
      
      // the pixel positions and other needed stuff for the geometrical clustering
      InDet::PixelCluster* pixelCluster = 0;
      Amg::Vector2D       clusterPosition(0.,0.);
      
      std::vector<Identifier>           rdoList;
      std::vector<int>                  totList;
      std::vector<int>                  PixelIndicesX;
      std::vector<int>                  PixelIndicesY;
      
      bool   isGanged = false;
      int lvl1a = 0;
      
      double m_accumulatedPathLength=0.;
      double m_clusterRecoX = 0.;
      double m_clusterRecoY = 0.;
      std::vector < double > paths;
     
       //ATTENTION index max e min da rdo + manager
      int phiIndexMax = -999999;
      int phiIndexMin = 1000000;
      int etaIndexMax = -999999;
      int etaIndexMin = 1000000;
      
      
      for (auto& dStep : digitizationSteps){
	
	double pathlenght = dStep.stepLength;
	 // two options fro charge smearing: landau / gauss
        if ( m_pixSmearPathLength > 0. ) {
          // create the smdar parameter
          double sPar = m_pixSmearLandau ?
            m_pixSmearPathLength*CLHEP::RandLandau::shoot(m_randomEngine) :
            m_pixSmearPathLength*CLHEP::RandGauss::shoot(m_randomEngine);
          pathlenght *=  (1.+sPar);
        }
	
	
	if (pathlenght < pixMinimalPathCut) continue;
	
	// position on the diode map
	Trk::DigitizationCell cell(dStep.stepCell.first,dStep.stepCell.second);
	Amg::Vector2D PositionOnModule = m_digitizationModule->segmentation().cellPosition(cell);
	InDetDD::SiCellId diode = hitSiDetElement->cellIdOfPosition(PositionOnModule);
	
	
	if (!diode.isValid()) 
	  continue;
	
	Amg::Vector2D chargeCenterPosition = hitSiDetElement->rawLocalPositionOfCell(diode);
	
	//ATTENTION passaggio inutile? SiCellId -> Identifier?
	const Identifier rdoId            =  hitSiDetElement->identifierOfPosition(chargeCenterPosition);
	clusterPosition += pathlenght * chargeCenterPosition;

	int currentEtaIndex = diode.etaIndex();
	int currentPhiIndex = diode.phiIndex();
	if(currentEtaIndex > etaIndexMax) etaIndexMax = currentEtaIndex;
	if(currentEtaIndex < etaIndexMin) etaIndexMin = currentEtaIndex;
	if(currentPhiIndex > phiIndexMax) phiIndexMax = currentPhiIndex;
	if(currentPhiIndex < phiIndexMin) phiIndexMin = currentPhiIndex;
	
	//std::cout<<"Index Step "<<currentEtaIndex<<" "<<currentPhiIndex<<std::endl;
	
        // record - positions, rdoList and totList
	m_accumulatedPathLength += pathlenght;
        //Fail
	rdoList.push_back(rdoId);
        totList.push_back(int(pathlenght*m_pixPathLengthTotConv));
	//std::cout<<"path lenght "<<pathlenght<<std::endl;
	
      }

      delete m_digitizationModule;
      
      // the col/row
      int m_siDeltaPhiCut = phiIndexMax-phiIndexMin+1;
      int m_siDeltaEtaCut = etaIndexMax-etaIndexMin+1;

      int totalToT=0;
	for (unsigned int i=0; i<totList.size() ; i++){
	  totalToT+=totList[i];
	 
       }
      // bail out if 0 pixel or path length problem
      if (!rdoList.size() || m_accumulatedPathLength < pixMinimalPathCut || totalToT == 0) {
        if (totalToT == 0 && rdoList.size() > 0 ) ATH_MSG_WARNING("The total ToT of the cluster is 0, this should never happen");
	continue;
      }
     
      // weight the cluster position
      clusterPosition *= 1./m_accumulatedPathLength;
      Identifier clusterId = hitSiDetElement->identifierOfPosition(clusterPosition);


      // merging clusters

      bool merged = false;
      if(m_mergeCluster){ // merge to the current cluster "near" cluster in the cluster map, in the current detector element
	
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
	    
	    //Store HepMcParticleLink connected to the cluster removed from the collection
	    std::pair<PRD_MultiTruthCollection::iterator,PRD_MultiTruthCollection::iterator> saved_hit = m_pixPrdTruth->equal_range(currentCluster->identify());	    
	    for (PRD_MultiTruthCollection::iterator this_hit = saved_hit.first; this_hit != saved_hit.second; this_hit++)
	    {
	      hit_vector.push_back(this_hit->second);
	    }
	    //Delete all the occurency of the currentCluster from the multi map
	    if (saved_hit.first != saved_hit.second) m_pixPrdTruth->erase(currentCluster->identify());
	    delete currentCluster;
	    //break; //commenting out this break statement allows for multiple existing clusters to be merged.
	  }
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

      // ---------------------------------------------------------------------------------------------
      //  PART 2: Cluster && ROT creation
      
   
      //ATTENTION      
//       // correct shift implied by the scaling of the Lorentz angle
//       double newshift = 0.5*thickness*tanLorAng;
//       double corr = ( shift - newshift );
      // 2a) Cluster creation ------------------------------------
      if (m_pixUseClusterMaker){
	
	
	//ATTENTION this can be enabled, take a look to localDirection
//         if (m_pixModuleDistortion &&  hitSiDetElement->isBarrel() )
//           clusterPosition = m_pixDistortionTool->correctSimulation(hitSiDetElement->identify(), clusterPosition, localDirection);

   
	// from InDetReadoutGeometry: width from eta
	double etaWidth = dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design())->widthFromColumnRange(etaIndexMin, etaIndexMax);
	// from InDetReadoutGeometry : width from phi
	double phiWidth = dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design())->widthFromRowRange(phiIndexMin, phiIndexMax);
	
	InDet::SiWidth* siWidth = new InDet::SiWidth(Amg::Vector2D(m_siDeltaPhiCut,m_siDeltaEtaCut),
                                                   Amg::Vector2D(phiWidth,etaWidth));

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

        delete siWidth; 	
	
      }  else {
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
      
      //Add all hit that was connected to the cluster
      for(HepMcParticleLink p: hit_vector){
	
	 m_pixPrdTruth->insert(std::make_pair(pixelCluster->identify(), p ));
      }


      hit_vector.clear();
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

Trk::DigitizationModule* PixelFastDigitizationTool::buildDetectorModule(const InDetDD::SiDetectorElement* hitSiDetElement ) const {
  
  const InDetDD::PixelModuleDesign* design(dynamic_cast<const InDetDD::PixelModuleDesign*>(&hitSiDetElement->design()));
  
  if (!design) {
    ATH_MSG_DEBUG ( "Could not get design"<< design) ;
    return nullptr;
  }
  
    //Read from the SiDetectorElement information to build the digitization module 
    const double halfThickness = hitSiDetElement->thickness() * 0.5;
    const double halfWidth     = design->width() * 0.5;
    const double halfLenght    = design->length() * 0.5;
    
    int binsX = design->rows();
    int binsY = design->columns();
    double numberOfChip = design->numberOfCircuits();
    
    InDetDD::SiCellId cell(0,binsY/2);
    float LongPitch  =design->parameters(cell).width().xEta();
    //std::cout<<"numberOfChip "<<numberOfChip<<" LongPitch "<<LongPitch<<std::endl;

    ATH_MSG_VERBOSE("Retrieving infos: halfThickness = " << halfThickness << " --- halfWidth = " << halfWidth << " --- halfLenght = " << halfLenght );
    ATH_MSG_VERBOSE("Retrieving infos: binsX = " << binsX << " --- binsY = " << binsY << " --- numberOfChip = " << numberOfChip);
    
    int readoutDirection = design->readoutSide();
    
    //bool m_useLorentzAngle = (hitSiDetElement->getLorentzCorrection() == 0.);
    bool m_useLorentzAngle = true;
    float lorentzAngle   = m_useLorentzAngle ? hitSiDetElement->hitDepthDirection()*hitSiDetElement->hitPhiDirection()*std::atan(hitSiDetElement->getTanLorentzAngle()) : 0.; 

    // added for degugging
    
    // std::cout << "barrel_ec  = " << m_detID->barrel_ec(hitSiDetElement->identify())
    //           << " --  layer_disk = " << m_detID->layer_disk(hitSiDetElement->identify())
    //           << " --  eta_module = " << m_detID->eta_module(hitSiDetElement->identify()) 
    //           << " --  lorentzAngle = " << lorentzAngle
    // 	      << " --  lorentzCorrection = " << shift
    // 	      << " --  readoutDirection = " << readoutDirection << std::endl;
    // std::cout << "element->hitDepthDirection() = " << hitSiDetElement->hitDepthDirection()
    // 	      << " --  element->hitPhiDirection() = " << hitSiDetElement->hitPhiDirection() << std::endl;
    
    // rectangle bounds 
    std::shared_ptr<const Trk::RectangleBounds> rectangleBounds(new Trk::RectangleBounds(halfWidth,halfLenght));    
    ATH_MSG_VERBOSE("Initialized rectangle Bounds");
    // create the segmentation
    std::shared_ptr<const Trk::Segmentation> rectangleSegmentation(new Trk::RectangularSegmentation(rectangleBounds,(size_t)binsX,LongPitch,(size_t)binsY, numberOfChip));
    // build the module
    ATH_MSG_VERBOSE("Initialized rectangleSegmentation");
    Trk::DigitizationModule * m_digitizationModule = new Trk::DigitizationModule(rectangleSegmentation, 
										 halfThickness,
										 readoutDirection,
										 lorentzAngle);
    ATH_MSG_VERBOSE("Building Rectangle Segmentation with dimensions (halfX, halfY) = (" << halfWidth << ", " << halfLenght << ")");
    
    // success return
    return m_digitizationModule;   
}


Amg::Vector3D PixelFastDigitizationTool::CalculateIntersection(Amg::Vector3D Point, Amg::Vector3D Direction, Amg::Vector2D PlaneBorder, double halfthickness) const
{  
  Amg::Vector3D Intersection(0.,0.,0.);
  
  //Using parameter definition of a line in 3d z=z_point + direction_z t
  std::vector<double> parameters;
   parameters.push_back((PlaneBorder.x() - Point.x())/Direction.x());
   parameters.push_back((-PlaneBorder.x() - Point.x())/Direction.x());
   parameters.push_back((PlaneBorder.y() - Point.y())/Direction.y());
   parameters.push_back((-PlaneBorder.y() - Point.y())/Direction.y());
  
  for(double parameter: parameters)
  {
    double z =  Point.z() + Direction.z() * parameter;
//     std::cout<<"z "<<z<<std::endl;
    if( fabs(z) > halfthickness )
      continue;
    
    
    double x = Point.x() + Direction.x() * parameter;
    double y = Point.y() + Direction.y() * parameter;
//     std::cout<<"x "<<x<<std::endl;
//     std::cout<<"y "<<y<<std::endl;
    
    if(fabs(x) > PlaneBorder.x() || fabs(y) > PlaneBorder.y())
      continue;
    
    
    Intersection = Amg::Vector3D(x,y,z);
    break;
    
  }
  
  return Intersection;
}

bool PixelFastDigitizationTool::Diffuse(HepGeom::Point3D<double>& localEntry, HepGeom::Point3D<double>& localExit, double shift){

  double localEntryX = localEntry.x();
  double localEntryY = localEntry.y();
  double localExitX = localExit.x();
  double localExitY = localExit.y();  

  double signX = localExitX>localEntryX ? 1:-1 ;
  double signY = localExitY>localEntryY ? 1:-1 ;
  
  localEntryX += shift*signX*(-1);
  localExitX += shift*signX;
  localEntryY += shift*signY*(-1);
  localExitY += shift*signY; 
  
  localEntry.setX(localEntryX);
  localEntry.setY(localEntryY);
  localExit.setX(localExitX);
  localExit.setY(localExitY);

  return true;
}
