/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
 
#include <algorithm>
#include "FastSiDigitization/SCT_FastDigitizationTool.h"

// Pile-up
#include "PileUpTools/PileUpMergeSvc.h"
#include "PileUpTools/PileUpTypeHelper.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Hit class includes
#include "InDetSimData/InDetSimDataCollection.h"
#include "Identifier/Identifier.h"
#include "CxxUtils/make_unique.h"

// Det Descr includes
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

// CLHEP
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandLandau.h"
#include "CLHEP/Random/RandFlat.h"


#include "InDetReadoutGeometry/SiDetectorDesign.h"
#include "SCT_ReadoutGeometry/SCT_ModuleSideDesign.h"
#include "SCT_ReadoutGeometry/SCT_BarrelModuleSideDesign.h"
#include "SCT_ReadoutGeometry/SCT_ForwardModuleSideDesign.h"

// FATRAS
#include "ReadoutGeometryBase/SiCellId.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkExUtils/LineIntersection2D.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetPrepRawData/SiCluster.h"
#include "SiClusterizationTool/ClusterMakerTool.h"
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetPrepRawData/SiClusterContainer.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeneratorObjects/McEventCollectionHelper.h"
#include "HepMC/GenParticle.h"

#include <sstream>
#include <string>

// Path resolver
#include "PathResolver/PathResolver.h"

#include <TFile.h>
#include <TH1.h>

static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

SCT_FastDigitizationTool::SCT_FastDigitizationTool(const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent) :

  PileUpToolBase(type, name, parent),
  m_inputObjectName("SCT_Hits"),
  m_sct_ID(nullptr),
  m_manager(nullptr),
  m_mergeSvc("PileUpMergeSvc",name),
  m_HardScatterSplittingMode(0),
  m_HardScatterSplittingSkipper(false),
  m_rndmSvc("AtRndmGenSvc",name),
  m_randomEngine(nullptr),
  m_randomEngineName("FastSCT_Digitization"),
  m_thpcsi(nullptr),
  m_clusterMaker("InDet::ClusterMakerTool"),
  m_vetoThisBarcode(crazyParticleBarcode),
  m_sctClusterMap(nullptr),
  m_sctClusterContainer("SCT_Clusters"),
  m_sctPrdTruth("PRD_MultiTruthSCT"),
  m_sctSmearPathLength(0.01),
  m_sctSmearLandau(true),
  m_sctEmulateSurfaceCharge(true),
  m_sctTanLorentzAngleScalor(1.),
  m_sctAnalogStripClustering(false),  
  m_sctErrorStrategy(2),
  m_sctRotateEC(true),
  m_mergeCluster(true),
  m_DiffusionShiftX_barrel(4),
  m_DiffusionShiftY_barrel(4),
  m_DiffusionShiftX_endcap(2),
  m_DiffusionShiftY_endcap(2),
  m_sctMinimalPathCut(90.),
  m_inefficiencySF(0.008),
  m_ineffSF_filename("FastSiDigitization/fast_pixel_inefficiency_SF.root")
{
  declareInterface<ISCT_FastDigitizationTool>(this);
  declareProperty("InputObjectName"               , m_inputObjectName,          "Input Object name" );
  declareProperty("MergeSvc"                      , m_mergeSvc,                 "Merge service" );
  declareProperty("RndmSvc"                       , m_rndmSvc,                  "Random Number Service used in SCT & Pixel digitization" );
  declareProperty("RndmEngine"                    , m_randomEngineName,         "Random Number Engine used in SCT digitization" );
  declareProperty("ClusterMaker"                  , m_clusterMaker);
  declareProperty("SCT_ClusterContainerName"      , m_sctClusterContainer);
  declareProperty("TruthNameSCT"                  , m_sctPrdTruth);
  declareProperty("SCT_SmearPathSigma"            , m_sctSmearPathLength);
  declareProperty("SCT_SmearLandau"               , m_sctSmearLandau);
  declareProperty("EmulateSurfaceCharge"          , m_sctEmulateSurfaceCharge);
  declareProperty("SCT_ScaleTanLorentzAngle"      , m_sctTanLorentzAngleScalor);
  declareProperty("SCT_AnalogClustering"          , m_sctAnalogStripClustering);
  declareProperty("SCT_ErrorStrategy"             , m_sctErrorStrategy);
  declareProperty("SCT_RotateEndcapClusters"      , m_sctRotateEC);
  declareProperty("SCT_MinimalPathLength"         , m_sctMinimalPathCut);
  declareProperty("DiffusionShiftX_barrel", m_DiffusionShiftX_barrel);
  declareProperty("DiffusionShiftY_barrel", m_DiffusionShiftY_barrel);
  declareProperty("DiffusionShiftX_endcap", m_DiffusionShiftX_endcap);
  declareProperty("DiffusionShiftY_endcap", m_DiffusionShiftY_endcap);
  declareProperty("HardScatterSplittingMode"      , m_HardScatterSplittingMode, "Control pileup & signal splitting" );
  declareProperty("ParticleBarcodeVeto"           , m_vetoThisBarcode, "Barcode of particle to ignore");
  declareProperty("UseMcEventCollectionHelper",   m_needsMcEventCollHelper = false);
  declareProperty("InefficiencySF", m_inefficiencySF);
  declareProperty("ineffSF_filename", m_ineffSF_filename);
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode SCT_FastDigitizationTool::initialize()
{

  //locate the AtRndmGenSvc and initialize our local ptr
  CHECK(m_rndmSvc.retrieve());
  //Get own engine with own seeds:
  m_randomEngine = m_rndmSvc->GetEngine(m_randomEngineName);
  if (!m_randomEngine)
    {
      ATH_MSG_ERROR( "Could not get random engine '" << m_randomEngineName << "'" );
      return StatusCode::FAILURE;
    }

  // Get the SCT Detector Manager
  CHECK(detStore()->retrieve(m_manager,"SCT"));
  ATH_MSG_DEBUG ( "Retrieved SCT_DetectorManager with version "  << m_manager->getVersion().majorNum() );

  CHECK(detStore()->retrieve(m_sct_ID, "SCT_ID"));

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
  if (!m_clusterMaker.empty())
    {
      CHECK(m_clusterMaker.retrieve());
    }

  //locate the PileUpMergeSvc and initialize our local ptr
  CHECK(m_mergeSvc.retrieve());
  
  //Retrieve the histograms to calculate inefficency SF
  m_ineffSF_histograms = {"low_pileup","med_pileup","high_pileup"};
  if ( ReadInefficiencyScaleFactor(m_ineffSF_filename,m_ineffSF_histograms).isFailure() ){
    ATH_MSG_ERROR ( "Unable to retrieve the inefficiency scale factor ");
    return StatusCode::FAILURE;
  }

  //Initialize threshold
  m_sctMinimalPathCut = m_sctMinimalPathCut * Gaudi::Units::micrometer;

  return StatusCode::SUCCESS ;
}

StatusCode SCT_FastDigitizationTool::prepareEvent(unsigned int)
{

  m_siHitCollList.clear();
  m_thpcsi = new TimedHitCollection<SiHit>();
  m_HardScatterSplittingSkipper = false;
  return StatusCode::SUCCESS;

}

StatusCode SCT_FastDigitizationTool::processBunchXing(int /*bunchXing*/,
                                                      SubEventIterator bSubEvents,
                                                      SubEventIterator eSubEvents)
{
  //decide if this event will be processed depending on HardScatterSplittingMode & bunchXing
  if (m_HardScatterSplittingMode == 2 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && m_HardScatterSplittingSkipper )  { return StatusCode::SUCCESS; }
  if (m_HardScatterSplittingMode == 1 && !m_HardScatterSplittingSkipper ) { m_HardScatterSplittingSkipper = true; }
  SubEventIterator iEvt(bSubEvents);
  while (iEvt != eSubEvents)
    {
      StoreGateSvc& seStore(*iEvt->ptr()->evtStore());
      PileUpTimeEventIndex thisEventIndex(iEvt->time(), iEvt->index(), pileupTypeMapper(iEvt->type()));
      const SiHitCollection* seHitColl(nullptr);
      CHECK(seStore.retrieve(seHitColl,m_inputObjectName).isSuccess());

      //Copy Hit Collection
      SiHitCollection* siHitColl(new SiHitCollection("SCT_Hits"));
      SiHitCollection::const_iterator i(seHitColl->begin());
      SiHitCollection::const_iterator e(seHitColl->end());
      // Read hits from this collection
      for (; i!=e; ++i)
        {
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


StatusCode SCT_FastDigitizationTool::createOutputContainers()
{
  if (!m_sctClusterContainer.isValid())
    {
      m_sctClusterContainer = CxxUtils::make_unique<InDet::SCT_ClusterContainer>(m_sct_ID->wafer_hash_max());
      if(!m_sctClusterContainer.isValid())
        {
          ATH_MSG_FATAL( "[ --- ] Could not create SCT_ClusterContainer");
          return StatusCode::FAILURE;
        }
    }
  m_sctClusterContainer->cleanup();

  // --------------------------------------
  // symlink the SCT Container
  InDet::SiClusterContainer* symSiContainer=0;
  CHECK(evtStore()->symLink(m_sctClusterContainer.ptr(),symSiContainer));
  ATH_MSG_DEBUG( "[ hitproc ] SCT_ClusterContainer symlinked to SiClusterContainer in StoreGate" );
  // --------------------------------------

  // truth info
  if (!m_sctPrdTruth.isValid())
    {
      m_sctPrdTruth = CxxUtils::make_unique<PRD_MultiTruthCollection>();
      if (!m_sctPrdTruth.isValid())
        {
          ATH_MSG_FATAL("Could not record collection " << m_sctPrdTruth.name());
          return StatusCode::FAILURE;
        }
    }

  return StatusCode::SUCCESS;
}

StatusCode SCT_FastDigitizationTool::processAllSubEvents() {

  CHECK(this->createOutputContainers());

  //  get the container(s)
  typedef PileUpMergeSvc::TimedList<SiHitCollection>::type TimedHitCollList;

  //this is a list<pair<time_t, DataLink<SCTUncompressedHitCollection> > >
  TimedHitCollList hitCollList;
  unsigned int numberOfSimHits(0);
  if ( !(m_mergeSvc->retrieveSubEvtsData(m_inputObjectName, hitCollList, numberOfSimHits).isSuccess()) && hitCollList.size()==0 )
    {
      ATH_MSG_ERROR ( "Could not fill TimedHitCollList" );
      return StatusCode::FAILURE;
    }
  else
    {
      ATH_MSG_DEBUG ( hitCollList.size() << " SiHitCollections with key " << m_inputObjectName << " found" );
    }

  // Define Hit Collection
  TimedHitCollection<SiHit> thpcsi(numberOfSimHits);

  //now merge all collections into one
  TimedHitCollList::iterator   iColl(hitCollList.begin());
  TimedHitCollList::iterator endColl(hitCollList.end()  );

  m_HardScatterSplittingSkipper = false;
  // loop on the hit collections
  while ( iColl != endColl )
    {
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

  //Get mu value
  const EventInfo* eventInfo = 0;
  if (evtStore()->retrieve(eventInfo)) {
    m_mu_val = eventInfo->averageInteractionsPerCrossing();
  }
  
  // Process the Hits
  CHECK(this->digitize());

  CHECK(this->createAndStoreRIOs());
  ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );

  return StatusCode::SUCCESS;
}



StatusCode SCT_FastDigitizationTool::mergeEvent()
{
  CHECK(this->createOutputContainers());

  //Get mu value
  const EventInfo* eventInfo = 0;
  if (evtStore()->retrieve(eventInfo)) {
    m_mu_val = eventInfo->averageInteractionsPerCrossing();
  }
  
  if (m_thpcsi != 0)
    {
      CHECK(this->digitize());
    }

  //-----------------------------------------------------------------------
  // Clean up temporary containers
  delete m_thpcsi;
  std::list<SiHitCollection*>::iterator siHitColl(m_siHitCollList.begin());
  std::list<SiHitCollection*>::iterator siHitCollEnd(m_siHitCollList.end());
  while(siHitColl!=siHitCollEnd)
    {
      delete (*siHitColl);
      ++siHitColl;
    }
  m_siHitCollList.clear();
  //-----------------------------------------------------------------------

  CHECK(this->createAndStoreRIOs());
  ATH_MSG_DEBUG ( "createAndStoreRIOs() succeeded" );

  return StatusCode::SUCCESS;
}


StatusCode SCT_FastDigitizationTool::digitize()
{
  TimedHitCollection<SiHit>::const_iterator i, e;
  if(!m_sctClusterMap) { m_sctClusterMap = new SCT_detElement_RIO_map; }
  else { m_sctClusterMap->clear(); }

  EBC_EVCOLL currentMcEventCollection(EBC_MAINEVCOLL); // Base on enum defined in HepMcParticleLink.h
  int lastPileupType(6); // Based on enum defined in PileUpTimeEventIndex.h
  while (m_thpcsi->nextDetectorElement(i, e))
  {
    SCT_detElement_RIO_map SCT_DetElClusterMap;
    std::vector<int> trkNo;
    std::vector<Identifier> detEl;
    while (i != e)
    {
      TimedHitPtr<SiHit> currentSiHit(*i++);
      
      const InDetDD::SiDetectorElement *hitSiDetElement = m_manager->getDetectorElement(currentSiHit->getBarrelEndcap(), currentSiHit->getLayerDisk(), currentSiHit->getPhiModule(), currentSiHit->getEtaModule(), currentSiHit->getSide());
      if (!hitSiDetElement)
      {
	ATH_MSG_ERROR( "Could not get detector element.");
	continue;
      }
      
      std::vector<HepMcParticleLink> hit_vector; //Store the hits in merged cluster  
      
      // the module design
      const InDetDD::SCT_ModuleSideDesign* design = dynamic_cast<const InDetDD::SCT_ModuleSideDesign*>(&hitSiDetElement->design());
      if (!design)
      {
	ATH_MSG_WARNING ( "SCT_DetailedSurfaceChargesGenerator::process can not get "<< design) ;
	continue;
      }
      
      // Process only one hit by the same particle in the same detector element
      bool isRep = false;
      const int trkn = currentSiHit->trackNumber();
      const Identifier detElId = hitSiDetElement->identify();
      for (unsigned int j=0; j<trkNo.size();j++)
      {
	for (unsigned int k=0; k<detEl.size();k++)
	{
	  if ((trkn > 0) && (trkn == trkNo[j]) && (detElId == detEl[k])) {isRep = true; break;}
	}
	if (isRep) { break; }
      }
      if (isRep) { continue; }
      trkNo.push_back(trkn);
      detEl.push_back(detElId);
      
      const double hitDepth  = hitSiDetElement->hitDepthDirection();
      
      double shiftX,shiftY;
      if (hitSiDetElement->isBarrel()){
          shiftX=m_DiffusionShiftX_barrel;
          shiftY=m_DiffusionShiftY_barrel;
      }
      else{
          shiftX=m_DiffusionShiftX_endcap;
          shiftY=m_DiffusionShiftY_endcap;
      }
    
      HepGeom::Point3D<double> localStartPosition = hitSiDetElement->hitLocalToLocal3D(currentSiHit->localStartPosition());
      HepGeom::Point3D<double> localEndPosition = hitSiDetElement->hitLocalToLocal3D(currentSiHit->localEndPosition());
    
    
      //TEST ON CLUSTER SIZE 04/03  
      // get the identifier of the entry and the exit
     /* 
      const double localEntryX_prediffuse = localStartPosition.x();
      const double localEntryY_prediffuse = localStartPosition.y();
      const double localEntryZ_prediffuse = localStartPosition.z();
      const double localExitX_prediffuse = localEndPosition.x();
      const double localExitY_prediffuse = localEndPosition.y();
      const double localExitZ_prediffuse = localEndPosition.z();
      
      const Amg::Vector2D localEntry_prediffuse(localEntryX_prediffuse,localEntryY_prediffuse);
      const Amg::Vector2D localExit_prediffuse(localExitX_prediffuse,localExitY_prediffuse);
      
      const Identifier entryId_prediffuse = hitSiDetElement->identifierOfPosition(localEntry_prediffuse);
      const Identifier exitId_prediffuse  = hitSiDetElement->identifierOfPosition(localExit_prediffuse);
      // now get the cellIds and check whether they're valid
      const InDetDD::SiCellId entryCellId_prediffuse = hitSiDetElement->cellIdFromIdentifier(entryId_prediffuse);
      const InDetDD::SiCellId exitCellId_prediffuse = hitSiDetElement->cellIdFromIdentifier(exitId_prediffuse);
      
      std::cout<<"Entry and exit ID before diffuse "<< entryCellId_prediffuse.phiIndex() << "  " << exitCellId_prediffuse.phiIndex() <<std::endl;
      */
      Diffuse(localStartPosition,localEndPosition, shiftX * Gaudi::Units::micrometer, shiftY * Gaudi::Units::micrometer);
      
      const double localEntryX = localStartPosition.x();
      const double localEntryY = localStartPosition.y();
      const double localEntryZ = localStartPosition.z();
      const double localExitX = localEndPosition.x();
      const double localExitY = localEndPosition.y();
      const double localExitZ = localEndPosition.z();
      
      
      
      const Amg::Vector2D localEntry(localEntryX,localEntryY);
      const Amg::Vector2D localExit(localExitX,localExitY);
      const Amg::Vector3D localExit3D(localExitX,localExitY,localExitZ);
      
      const double thickness = hitSiDetElement->thickness();
      
      const Trk::Surface *hitSurface = &hitSiDetElement->surface();
      
      const double distX = localExitX-localEntryX;
      const double distY = localExitY-localEntryY;
      const double distZ = localExitZ-localEntryZ;
      
      const Amg::Vector3D localDirection(distX, distY, distZ);
      // path length statistics
      double potentialClusterPath_Geom  = localDirection.mag();    // geometrical path length
      double potentialClusterPath_Step  = 0.;                      // path calculated through stepping
      double potentialClusterPath_Drift = 0.;                      // path calculated through drift charge
      double potentialClusterPath_Used  = 0.;                      // path used (contains smearing & cut if applied)
      
      // relational slope
      const double slopeYX = distY/distX;
      const double slopeXZ = distX/distZ;
      const double slopeZX = distZ/distX;
      // signs of stepping
      const int signX = distX > 0. ? 1 : -1 ;
      const int signY = distY > 0. ? 1 : -1 ;
      const int signZ = distZ > 0. ? 1 : -1 ;
      
      // get the identifier of the entry and the exit
      const Identifier entryId = hitSiDetElement->identifierOfPosition(localEntry);
      const Identifier exitId  = hitSiDetElement->identifierOfPosition(localExit);
      // now get the cellIds and check whether they're valid
      const InDetDD::SiCellId entryCellId = hitSiDetElement->cellIdFromIdentifier(entryId);
      const InDetDD::SiCellId exitCellId = hitSiDetElement->cellIdFromIdentifier(exitId);
      // entry / exit validity
      const bool entryValid = entryCellId.isValid();
      const bool exitValid  = exitCellId.isValid();
      
      
      // the intersecetion id and cellId of it
      const double par = -localEntryZ/(localExitZ-localEntryZ);
      const double interX = localEntryX + par*(localExitX-localEntryX);
      const double interY = localEntryY + par*(localExitY-localEntryY);
    
      const Amg::Vector2D intersection(interX,interY);
      const Identifier intersectionId            =  hitSiDetElement->identifierOfPosition(intersection);
      
      // apply the lorentz correction
      const double tanLorAng     = m_sctTanLorentzAngleScalor*hitSiDetElement->getTanLorentzAnglePhi();
      const int lorentzDirection = tanLorAng > 0. ? 1 : -1;
      const bool useLorentzDrift = fabs(tanLorAng) > 0.01;
      // shift parameters
      const double shift = hitSiDetElement->getLorentzCorrection();
      // lorenz angle effects : offset goes against the lorentzAngle
      const double xLoffset  = -lorentzDirection*thickness*tanLorAng;
      
      // --------------------------------------------------------------------------------------
      // fast exit: skip non-valid entry && exit
      if (!entryValid && !exitValid)
      {
	continue;
      }
      
      std::vector<Identifier>          potentialClusterRDOList;
      std::map<Identifier, double>     surfaceChargeWeights;
      // min/max indices
      int phiIndexMinRaw = 1000;
      int phiIndexMaxRaw = -1000;
      
      // is it a single strip w/o drift effect ? - also check the numerical stability
      const bool singleStrip = (entryCellId == exitCellId && entryValid);
      if (singleStrip && !useLorentzDrift)
      {
	// ----------------------- single strip lucky case  ----------------------------------------
	// 1 strip cluster without drift effect
	surfaceChargeWeights[intersectionId] = potentialClusterPath_Geom;
      }
      else
      {
	// the start parameters
	int strips = 0;
	// needed for both approaches with and w/o drift
	Identifier          currentId             = entryId;
	InDetDD::SiCellId   currentCellId         = entryCellId;
	Amg::Vector2D  currentCenterPosition = hitSiDetElement->localPositionOfCell(currentCellId);
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
	if (!entryValid)
	{
	  // the number of strips in Phi
	  const int numberOfDiodesPhi = design->cells();
	  // the simple case is if the exit is outside locY
	  if (!hitSurface->bounds().insideLoc2(localEntry))
	  {
	    // step towards the border
	    const double stepInY = signY*(fabs(localEntryY)-0.5*hitSiDetElement->length());
	    const double stepInX = stepInY*distX/distY;
	    const double stepInZ = stepInY*distZ/distY;
	    currentStep3D = Amg::Vector3D(stepInX,stepInY,stepInZ);
	  }
	  else
	  {
	    //get the last valid phi border
	    const int phiExitIndex   = exitCellId.phiIndex() <= 2 ? 0 : numberOfDiodesPhi-1;
	    
	    const InDetDD::SiCellId  phiExitId(phiExitIndex);
	    const Amg::Vector2D phiExitCenter = hitSiDetElement->localPositionOfCell(phiExitId);
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
	  potentialClusterPath_Step += currentStep3D.mag();
	  ATH_MSG_VERBOSE("[ cluster - sct ] Strip entry shifted by " << currentStep3D << " ( " <<  potentialClusterPath_Step << " )");
	  // for step epsilon into the first valid
	  const Amg::Vector2D positionInFirstValid(currentPosition3D.x()+0.01*signX,currentPosition3D.y()+0.01*signY);
	  // reset the entry parameters to the first valid pixel
	  currentId             = hitSiDetElement->identifierOfPosition(positionInFirstValid);
	  currentCellId         = hitSiDetElement->cellIdFromIdentifier(currentId);
	  currentCenterPosition = hitSiDetElement->localPositionOfCell(currentId);
	  
	} // ----- start position has been reset -------- ( // endif (!entryValid) )
	
	// (b) the collection of strips due to surface charge
	// the lorentz plane setp
	double        lplaneStepX = 0.;
	double        lplaneStepY = 0.;
	Amg::Vector3D lplaneIntersection(0.,0.,0.);
	Amg::Vector3D driftPrePosition3D(currentPosition3D);
	Amg::Vector3D driftPostPosition3D(currentPosition3D);
	// (c) simple cache for the purely geometrical clustering
	Identifier    lastId = currentId;
	bool          lastStrip = false;
	ATH_MSG_VERBOSE("[ cluster - sct ] CurrentPosition " << currentPosition3D );
	
	// the steps between the strips -------------------------------------------
	for ( ; ; ++strips)
	{
	  // -------------------------------------------------------------------------------------------------
	  // current phi/eta indices
	  const int currentPhiIndex = currentCellId.phiIndex();
	  
	  // record for the full validation
	  // (a) steps through the strips
	  // sct break for last strip or if you step out of the game
	  
	  if (lastStrip || currentPosition3D.z() > 0.5*thickness || strips > 8)
	  {
	    break;
	  }
	  // no single valid strip
	  if (!exitValid && !currentCellId.isValid())
	  {
	    break;
	  }
	  // cache it
	  phiIndexMinRaw = currentPhiIndex < phiIndexMinRaw ?  currentPhiIndex : phiIndexMinRaw;
	  phiIndexMaxRaw = currentPhiIndex > phiIndexMaxRaw ?  currentPhiIndex : phiIndexMaxRaw;
	  
	  // find out if this is the last step
	  lastStrip = (currentPhiIndex == exitCellId.phiIndex());
	  // get the current Pitch
	  const double currentPitchX = hitSiDetElement->phiPitch(currentCenterPosition);
	  // the next & previous sct borders are needed (next is w.r.t to the track direction)
	  std::vector<double> lorentzLinePositions;
	  const int trackDir = slopeXZ > 0 ? 1 : -1; //FIXME will be multiplying doubles by this int!
	  lorentzLinePositions.reserve(2);
	  // the both pixel borders left/right
	  lorentzLinePositions.push_back(currentCenterPosition.x() + trackDir*0.5*currentPitchX);
	  lorentzLinePositions.push_back(currentCenterPosition.x() - trackDir*0.5*currentPitchX);
	  // the third line is possible -> it is due to xOffset > pitchX
	  if (xLoffset*xLoffset > currentPitchX*currentPitchX)
	  {
	    lorentzLinePositions.push_back(currentCenterPosition.x() + lorentzDirection*1.5*currentPitchX);
	  }
	  // intersect the effective lorentz plane if the intersection is not valid anymore
	  bool          lplaneInterInCurrent(false);
	  double        lorentzPlaneHalfX(0.);
	  Amg::Vector3D lplaneCandidate(0.,0.,0.);
	  std::vector<double>::iterator lorentzLinePosIter = lorentzLinePositions.begin();
	  // find the intersection solutions for the three cases
	  int lplaneDirection = 100; // solve the intersection case first
	  // test left and right lorentz planes of this pixel
	  for ( ; lorentzLinePosIter != lorentzLinePositions.end(); ++lorentzLinePosIter)
	  {
	    // first - do the intersection : the readout side is respected by the hit depth direction
	    Trk::LineIntersection2D intersectLorentzPlane(localEntryX,-0.5*signZ*thickness,localExitX,0.5*signZ*thickness,
							  (*lorentzLinePosIter)+xLoffset,-0.5*hitDepth*thickness,
							  (*lorentzLinePosIter),0.5*hitDepth*thickness);
	    // avoid repeating intersections
	    const double formerPlaneStepZ = intersectLorentzPlane.interY-lplaneIntersection.z();
	    if (formerPlaneStepZ*formerPlaneStepZ < 10e-5)
	    {
	      lplaneInterInCurrent = false;
	      continue;
	    }
	    // is the intersection within the z thickness ?
	    lplaneInterInCurrent = intersectLorentzPlane.interY > -0.5*thickness
	    && intersectLorentzPlane.interY < 0.5*thickness;
	    // the step in z from the former plane intersection
	    // only go on if it is worth it
	    // (a) it has to be within z boundaries
	    if (lplaneInterInCurrent)
	    {
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
	      const double distToNextLineY = 0.5*hitSiDetElement->length()-lplaneCandidate.y();
	      const double distToPrevLineY = -0.5*hitSiDetElement->length()-lplaneCandidate.y();
	      lplaneInterInCurrent = (distToNextLineY*distToPrevLineY) < 0.;
	      // we have an intersection candidate - needs to be resolved for +1/-1
	      lplaneDirection = lplaneInterInCurrent ? 0 : lplaneDirection;
	      if (lplaneInterInCurrent) {break;}
	    }
	  }
	  // now assign it (if valid)
	  if (lplaneInterInCurrent) {lplaneIntersection = lplaneCandidate;}
	  ATH_MSG_VERBOSE( "[ cluster - pix ] Lorentz plane intersection x/y/z = "
	  << lplaneCandidate.x() << ", "
	  << lplaneCandidate.y() << ", "
	  << lplaneCandidate.z() );
	  
	  // now solve for 1 / -1 if needed
	  if (lplaneDirection)
	  {
	    // check the z position of the track at this stage
	    const double trackZatlplaneHalfX = (lorentzPlaneHalfX-localEntryX)*slopeXZ - 0.5*thickness;
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
	    const double stepInY = signY*0.5*hitSiDetElement->length()-currentPosition3D.y();
	    const double stepInX = distX/distY*stepInY;
	    const double stepInZ = slopeZX*stepInX;
	    // update to the new currentStep
	    currentStep3D = Amg::Vector3D(stepInX,stepInY,stepInZ);
	    currentPosition3D += currentStep3D;
	  }
	  //           if (currentPosition3D.z() > 0.501*signZ*thickness){
	  if (fabs(currentPosition3D.z()) > 0.501*thickness)
	  {
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
	  const Amg::Vector2D currentInsidePosition(currentPosition3D.x()+0.01*signX,currentPosition3D.y()+0.01*signY);
	  currentId              = hitSiDetElement->identifierOfPosition(currentInsidePosition);
	  currentCellId          = hitSiDetElement->cellIdFromIdentifier(currentId);
	  
	  // just to be sure also for fan structure cases
	  currentCenterPosition = hitSiDetElement->localPositionOfCell(currentCellId);
	  // The new current Position && the path length for monitoring
	  potentialClusterPath_Step              += currentStep3D.mag();
	  ATH_MSG_VERBOSE("[ cluster - sct ] CurrentPosition " << currentPosition3D
	  << " ( yielded by :" << currentStep3D  << ")");
	  // setting the drift Post position
	  driftPostPosition3D = lplaneInterInCurrent ? lplaneIntersection : currentPosition3D;
	  // ---------- the surface charge is emulated -------------------------------------------------------
	  if (m_sctEmulateSurfaceCharge && useLorentzDrift)
	  {
	    // loop to catch lpintersection in last step
	    const int currentDrifts = (lastStrip && lplaneInterInCurrent) ? 2 : 1;
	    for (int idrift = 0; idrift < currentDrifts; ++idrift)
	    {
	      // const assignment for fast access, take intersection solution first, then cell exit
	      const Amg::Vector3D& currentDriftPrePosition  = idrift ? driftPostPosition3D : driftPrePosition3D;
	      const Amg::Vector3D& currentDriftPostPosition = idrift ? localExit3D : driftPostPosition3D;
	      // get the center of the step and drift it to the surface
	      const Amg::Vector3D driftStepCenter = 0.5*(currentDriftPrePosition+currentDriftPostPosition);
	      // respect the reaout side through the hit dept direction
	      const double driftZtoSurface    = 0.5*hitDepth*thickness-driftStepCenter.z();
	      // record the drift position on the surface
	      const double driftPositionAtSurfaceX = driftStepCenter.x() + tanLorAng*driftZtoSurface;
	      const Amg::Vector2D driftAtSurface(driftPositionAtSurfaceX,
						 driftStepCenter.y());
	      const Identifier surfaceChargeId = hitSiDetElement->identifierOfPosition(driftAtSurface);
	      if (surfaceChargeId.is_valid())
	      {
		// check if the pixel has already got some charge
		if (surfaceChargeWeights.find(surfaceChargeId) == surfaceChargeWeights.end())
		{
		  surfaceChargeWeights[surfaceChargeId] = (currentDriftPostPosition-currentDriftPrePosition).mag();
		}
		else
		{
		  surfaceChargeWeights[surfaceChargeId] += (currentDriftPostPosition-currentDriftPrePosition).mag();
		}
	      }
	      // record the drift step for validation
	    } // end of last step intersection check
	  }
	  else // ---------- purely geometrical clustering w/o surface charge ---------------------------
	  {
	    surfaceChargeWeights[lastId] = currentStep3D.mag();
	  }
	  // next pre is current post && lastId is currentId
	  driftPrePosition3D = driftPostPosition3D;
	  lastId = currentId;
	  
	} // end of steps between strips ----------------------------------------------------------------------
      }
      
      // the calculated local position
      double totalWeight = 0.;
      Amg::Vector2D potentialClusterPosition(0.,0.);
      std::map<Identifier,double>::iterator weightIter =  surfaceChargeWeights.begin();
      for ( ; weightIter != surfaceChargeWeights.end(); ++weightIter)
      {
	// get the (effective) path length in the strip
	double chargeWeight = (weightIter)->second;
	// path statistics
	potentialClusterPath_Drift += chargeWeight;
	const Identifier chargeId = (weightIter)->first;
	
	// charge smearing if set : 2 possibilities landau/gauss
	// two options fro charge smearing: landau / gauss
	if ( m_sctSmearPathLength > 0. )
	{
	  // create the smdar parameter
	  const double sPar = m_sctSmearLandau ?
	  m_sctSmearPathLength*CLHEP::RandLandau::shoot(m_randomEngine) :
	  m_sctSmearPathLength*CLHEP::RandGauss::shoot(m_randomEngine);
	  chargeWeight *=  (1.+sPar);
	}

	// the threshold cut
	if (!(chargeWeight > m_sctMinimalPathCut)) { continue; }
	
	
	// get the position according to the identifier
	const Amg::Vector2D chargeCenterPosition = hitSiDetElement->localPositionOfCell(chargeId);
	potentialClusterPath_Used += chargeWeight;
	// taken Weight (Fatras can do analog SCT clustering)
	const double takenWeight =  m_sctAnalogStripClustering ? chargeWeight : 1.;
	totalWeight += takenWeight;
	potentialClusterPosition += takenWeight * chargeCenterPosition;
	// and record the rdo
	potentialClusterRDOList.push_back(chargeId);
      }
      // bail out - no left overs after cut
      
      if (!potentialClusterRDOList.size() || potentialClusterPath_Used < m_sctMinimalPathCut)
      {
	continue;
      }
      
      
      
      // ---------------------------------------------------------------------------------------------
      //  PART 2: Cluster && ROT creation
      //
      // the Cluster Parameters -----------------------------
      // normalize cluster position && get identifier
      potentialClusterPosition *= 1./totalWeight;
      /*const */Identifier potentialClusterId = hitSiDetElement->identifierOfPosition(potentialClusterPosition);
      if (!potentialClusterId.is_valid()) {continue;}
      
      const IdentifierHash waferID = m_sct_ID->wafer_hash(hitSiDetElement->identify());
      
      
      	
      // merging clusters
      if(m_mergeCluster){
	unsigned int countC(0);
	ATH_MSG_INFO("Before cluster merging there were " << SCT_DetElClusterMap.size() << " clusters in the SCT_DetElClusterMap.");
	for(SCT_detElement_RIO_map::iterator existingClusterIter = SCT_DetElClusterMap.begin(); existingClusterIter != SCT_DetElClusterMap.end();)
	{
	  ++countC;
	  if (countC>100)
	  {
	    ATH_MSG_WARNING("Over 100 clusters checked for merging - bailing out!!");
	    break;
	  }
	  if (m_sct_ID->wafer_hash(((existingClusterIter->second)->detectorElement())->identify()) != waferID) {continue;}
	  //make a temporary to use within the loop and possibly erase - increment the main interator at the same time.
	  SCT_detElement_RIO_map::iterator currentExistingClusterIter = existingClusterIter++;
	  
	  const InDet::SCT_Cluster *existingCluster = (currentExistingClusterIter->second);
	  bool isNeighbour = this->NeighbouringClusters(potentialClusterRDOList, existingCluster);
	  if(isNeighbour)
	  {
	    //Merge the clusters
	    std::vector<Identifier> existingClusterRDOList = existingCluster->rdoList();
	    potentialClusterRDOList.insert(potentialClusterRDOList.end(), existingClusterRDOList.begin(), existingClusterRDOList.end() );
	    Amg::Vector2D existingClusterPosition(existingCluster->localPosition());
	    potentialClusterPosition = (potentialClusterPosition + existingClusterPosition)/2;
	    potentialClusterId = hitSiDetElement->identifierOfPosition(potentialClusterPosition);
	    //FIXME - also need to tidy up any associations to the deleted cluster in the truth container too.
	    SCT_DetElClusterMap.erase(currentExistingClusterIter);    
	    
	    //Store HepMcParticleLink connected to the cluster removed from the collection
	    std::pair<PRD_MultiTruthCollection::iterator,PRD_MultiTruthCollection::iterator> saved_hit = m_sctPrdTruth->equal_range(existingCluster->identify());	    
	    for (PRD_MultiTruthCollection::iterator this_hit = saved_hit.first; this_hit != saved_hit.second; this_hit++)
	    {
	      hit_vector.push_back(this_hit->second);
	    }
	    //Delete all the occurency of the currentCluster from the multi map
	    if (saved_hit.first != saved_hit.second) m_sctPrdTruth->erase(existingCluster->identify());
	    
	    delete existingCluster;
	    ATH_MSG_VERBOSE("Merged two clusters.");
	    //break; // Should look for more than one possible merge.
	  }
	}
	ATH_MSG_INFO("After cluster merging there were " << SCT_DetElClusterMap.size() << " clusters in the SCT_DetElClusterMap.");
      }
      
      // check whether this is a trapezoid
      const bool isTrapezoid(design->shape()==InDetDD::Trapezoid);
      // prepare & create the siWidth
      InDet::SCT_Cluster *potentialCluster(nullptr);
      // Find length of strip at centre
      const double clusterWidth(potentialClusterRDOList.size()*hitSiDetElement->phiPitch(potentialClusterPosition)); //!< @TODO CHECK
      const std::pair<InDetDD::SiLocalPosition, InDetDD::SiLocalPosition> ends(design->endsOfStrip(potentialClusterPosition));
      const double stripLength = fabs(ends.first.xEta()-ends.second.xEta());
      const InDet::SiWidth siWidth( Amg::Vector2D(int(potentialClusterRDOList.size()),1), Amg::Vector2D(clusterWidth,stripLength) );
      // 2a) Cluster creation ------------------------------------
      if (!m_clusterMaker.empty())
      {
	ATH_MSG_INFO("Using ClusterMakerTool to make cluster.");
	// correct for the shift that will be applied in the cluster maker
	// (only if surface charge emulation was switched off )
	if (!m_sctEmulateSurfaceCharge && shift*shift > 0.)
	{
	  potentialClusterPosition -=  Amg::Vector2D(shift,0.);
	}
	// safe to compare m_sctTanLorentzAngleScalor with 1. since it is set not calculated
	else if (m_sctTanLorentzAngleScalor != 1. && shift*shift > 0.)
	{
	  // correct shift implied by the scaling of the Lorentz angle
	  const double newshift = 0.5*thickness*tanLorAng;
	  const double corr = ( shift - newshift );
	  potentialClusterPosition +=  Amg::Vector2D(corr,0.);
	}
	bool not_valid = false;
	for (unsigned int i=0; i < potentialClusterRDOList.size(); i++)
	{
	  if (!(potentialClusterRDOList[i].is_valid()))
	  {
	    not_valid = true;
	    break;
	  }
	}
	if (not_valid)
	{
	  continue;
	}
	potentialCluster = m_clusterMaker->sctCluster(potentialClusterId,potentialClusterPosition,potentialClusterRDOList,siWidth,hitSiDetElement,m_sctErrorStrategy);
      }
      else
      {
	ATH_MSG_INFO("Making cluster locally.");
	// you need to correct for the lorentz drift -- only if surface charge emulation was switched on
	const double appliedShift = m_sctEmulateSurfaceCharge ? m_sctTanLorentzAngleScalor*shift : (1.-m_sctTanLorentzAngleScalor)*shift;
	const Amg::Vector2D lcorrectedPosition = Amg::Vector2D(potentialClusterPosition.x()+appliedShift,potentialClusterPosition.y());
	AmgSymMatrix(2) mat;
	mat.setIdentity();
	mat(Trk::locX,Trk::locX) = (clusterWidth*clusterWidth)/12.;
	mat(Trk::locY,Trk::locY) = (stripLength*stripLength)/12.;
	// rotation for endcap SCT
	if(isTrapezoid  && m_sctRotateEC)
	{
	  const double Sn      = hitSiDetElement->sinStereoLocal(intersection);
	  const double Sn2     = Sn*Sn;
	  const double Cs2     = 1.-Sn2;
	  //double W       = detElement->phiPitch(*clusterLocalPosition)/detElement->phiPitch();
	  //double V0      = mat(Trk::locX,Trk::locX)*W*W;
	  const double V0      = mat(Trk::locX,Trk::locX);
	  const double V1      = mat(Trk::locY,Trk::locY);
	  mat(Trk::locX,Trk::locX) = (Cs2*V0+Sn2*V1);
	  mat(Trk::locY,Trk::locX) = (Sn*sqrt(Cs2)*(V0-V1));
	  mat(Trk::locY,Trk::locY) = (Sn2*V0+Cs2*V1);
	}
	// covariance matrix && error description
	const Amg::MatrixX *potentialClusterErr = new Amg::MatrixX(mat);
	
	// create a custom cluster
	potentialCluster = new InDet::SCT_Cluster(potentialClusterId,lcorrectedPosition,potentialClusterRDOList,siWidth,hitSiDetElement,potentialClusterErr);
      }
      
      (void) SCT_DetElClusterMap.insert(SCT_detElement_RIO_map::value_type(waferID, potentialCluster));
      
      // Build Truth info for current cluster

      if (m_needsMcEventCollHelper) {
        if(currentSiHit.pileupType()!=lastPileupType) {
          MsgStream* amsg = &(msg());
          currentMcEventCollection = McEventCollectionHelper::getMcEventCollectionHMPLEnumFromPileUpType(currentSiHit.pileupType(), amsg);
          lastPileupType=currentSiHit.pileupType();
        }
      }
      const bool isEventIndexIsPosition = (currentSiHit.eventId()==0);
      HepMcParticleLink trklink(currentSiHit->trackNumber(), currentSiHit.eventId(), currentMcEventCollection, isEventIndexIsPosition);
      if (trklink.isValid())
      {
	const int barcode( trklink.barcode());
	if ( barcode !=0 && barcode != m_vetoThisBarcode )
	{
	  m_sctPrdTruth->insert(std::make_pair(potentialCluster->identify(), trklink));
	  ATH_MSG_DEBUG("Truth map filled with cluster" << potentialCluster << " and link = " << trklink);
	}
      }
      else
      {
	ATH_MSG_DEBUG("Particle link NOT valid!! Truth map NOT filled with cluster" << potentialCluster << " and link = " << trklink);
      }
      
      
      for(HepMcParticleLink p: hit_vector){
	m_sctPrdTruth->insert(std::make_pair(potentialCluster->identify(), p ));
      }
      
      hit_vector.clear();
      
      
    } // end hit while
    
  //check inefficiency SF
  if (m_inefficiencySF > 0.0 ){
       
	  for(SCT_detElement_RIO_map::iterator currentClusIter = SCT_DetElClusterMap.begin(); currentClusIter != SCT_DetElClusterMap.end();)
	  {
	   
	   SCT_detElement_RIO_map::iterator clusIter = currentClusIter++;
           const InDet::SCT_Cluster* currentCluster = clusIter->second;
	   bool isBarrel=currentCluster->detectorElement()->isBarrel();
	   double random= CLHEP::RandFlat::shoot(m_randomEngine,0.0,1.0);
	   
	   //Retrieve an eta and mu dependent inefficiency SF
	   double inefficiencySF = RetrieveInefficiencySF(fabs(currentCluster->globalPosition().eta()),m_mu_val,isBarrel);
	   //Apply an eta and mu dependent inefficiency SF
	   if  ( random < inefficiencySF ){
	      m_sctPrdTruth->erase(currentCluster->identify());
	      delete currentCluster;
	      SCT_DetElClusterMap.erase(clusIter);
     }
	  }
   }
  

    (void) m_sctClusterMap->insert(SCT_DetElClusterMap.begin(), SCT_DetElClusterMap.end());
  }
  return StatusCode::SUCCESS;
}


StatusCode SCT_FastDigitizationTool::createAndStoreRIOs()
{
  SCT_detElement_RIO_map::iterator clusterMapGlobalIter = m_sctClusterMap->begin();
  SCT_detElement_RIO_map::iterator endOfClusterMap = m_sctClusterMap->end();
  for (; clusterMapGlobalIter != endOfClusterMap; clusterMapGlobalIter = m_sctClusterMap->upper_bound(clusterMapGlobalIter->first))
    {
      std::pair <SCT_detElement_RIO_map::iterator, SCT_detElement_RIO_map::iterator> range;
      range = m_sctClusterMap->equal_range(clusterMapGlobalIter->first);
      SCT_detElement_RIO_map::iterator firstDetElem = range.first;
      const IdentifierHash waferID = firstDetElem->first;
      const InDetDD::SiDetectorElement *detElement = m_manager->getDetectorElement(waferID);
      InDet::SCT_ClusterCollection *clusterCollection = new InDet::SCT_ClusterCollection(waferID);
      if (clusterCollection)
        {
          clusterCollection->setIdentifier(detElement->identify());
          for ( SCT_detElement_RIO_map::iterator localClusterIter = range.first; localClusterIter != range.second; ++localClusterIter)
            {
              InDet::SCT_Cluster *sctCluster = const_cast<InDet::SCT_Cluster*>(localClusterIter->second);
              sctCluster->setHashAndIndex(clusterCollection->identifyHash(),clusterCollection->size());
              clusterCollection->push_back(sctCluster);
            }
          if (m_sctClusterContainer->addCollection(clusterCollection, clusterCollection->identifyHash()).isFailure())
            {
              ATH_MSG_WARNING( "Could not add collection to Identifiable container !" );
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

  // probably needs to be changed to rect/trapezoid
  if (sidetel.isBarrel())
    {
      // the exit position of the new strip
      double xExitPosition = stripCenter.x()+direction*0.5*sidetel.phiPitch(stripCenter);
      stepExitX = xExitPosition-localStartX;
      stepExitY = stepExitX*slopeYX;
      stepExitZ = stepExitX*slopeZX;
    }
  else
    {
      //In case of endcap cluster calculate carefully the strip border, taking into account the shape 
      std::pair<Amg::Vector3D,Amg::Vector3D> stripEndGlobal = sidetel.endsOfStrip(stripCenter);
      Amg::Vector2D stripEndLocal_inner = sidetel.localPosition(stripEndGlobal.first);
      Amg::Vector2D stripEndLocal_outer = sidetel.localPosition(stripEndGlobal.second);
      const double minWidth(sidetel.phiPitch(stripEndLocal_inner));
      const double maxWidth(sidetel.phiPitch(stripEndLocal_outer));
   
      const double angle=atan((stripEndLocal_inner.x()-stripCenter.x())/stripEndLocal_inner.y());
      const double angle1=atan((stripEndLocal_outer.x()-stripCenter.x())/stripEndLocal_outer.y());
    
      const double InnerX= stripEndLocal_inner.x()+ cos(angle) * direction * minWidth *0.5;
      const double InnerY= stripEndLocal_inner.y()+ sin(angle) * direction * minWidth *0.5;
      
      const double OuterX= stripEndLocal_outer.x()+ cos(angle1) * direction * maxWidth *0.5;
      const double OuterY= stripEndLocal_outer.y()+ sin(angle1) * direction * maxWidth *0.5;
   
      
      // now intersect track with border
      Trk::LineIntersection2D intersectStripBorder(localStartX,localStartY,localEndX,localEndY,
                                                   InnerX,InnerY,
                                                   OuterX,OuterY);
      
      
      // the step in x
      stepExitX = intersectStripBorder.interX - localStartX;
      stepExitY = slopeYX*stepExitX;
      stepExitZ = slopeZX*stepExitX;
    }
  return Amg::Vector3D(stepExitX,stepExitY,stepExitZ);
}

bool SCT_FastDigitizationTool::NeighbouringClusters(const std::vector<Identifier>& potentialClusterRDOList,  const InDet::SCT_Cluster *existingCluster) const
{
  //---------------------------------------------------------------------------------
  bool isNeighbour = false;
  unsigned int countR(0);
  std::vector<Identifier> existingClusterRDOList = existingCluster->rdoList();
  std::vector<Identifier>::const_iterator potentialClusterRDOIter = potentialClusterRDOList.begin();
  for ( ; potentialClusterRDOIter != potentialClusterRDOList.end(); ++potentialClusterRDOIter)
    {
      ++countR;
      if (countR>100)
        {
          ATH_MSG_WARNING("Over 100 RDOs checked for the given cluster - bailing out!!");
          break;
        }
      std::vector<Identifier>::const_iterator existingClusterRDOIter = existingClusterRDOList.begin();
      for( ; existingClusterRDOIter != existingClusterRDOList.end(); ++existingClusterRDOIter)
        {
         if(abs(m_sct_ID->strip(*existingClusterRDOIter) - m_sct_ID->strip(*potentialClusterRDOIter)) < 2)
	   {
              isNeighbour = true;
              break;
            }
        } // end of loop over RDOs in the current existing Cluster
      if (isNeighbour)
        {
          ATH_MSG_VERBOSE("The clusters are neighbours and can be merged.");
          break;
        }
    } // end of loop over RDOs in the potential cluster
  //---------------------------------------------------------------------------------
  return isNeighbour;
}


void SCT_FastDigitizationTool::Diffuse(HepGeom::Point3D<double>& localEntry, HepGeom::Point3D<double>& localExit, double shiftX, double shiftY ) const{
    
    double localEntryX = localEntry.x();
    double localExitX = localExit.x();
    
    double signX =  (localExitX - localEntryX) > 0 ? 1 : -1;
    localEntryX += shiftX * (-1) * signX;
    localExitX += shiftX * signX;
    localEntry.setX(localEntryX);
    localExit.setX(localExitX);
    
    double localEntryY = localEntry.y();
    double localExitY = localExit.y();
    
    double signY =  (localExitY - localEntryY) > 0 ? 1 : -1;
    localEntryY += shiftY * (-1) * signY;
    localExitY += shiftY * signY;
    
    //Check the effect in the endcap
    localEntry.setY(localEntryY);
    localExit.setY(localExitY);
        
}

StatusCode SCT_FastDigitizationTool::ReadInefficiencyScaleFactor(std::string filename, std::vector<std::string> histonames)
{
    std::string fullpath = PathResolverFindCalibFile(filename);
  
    TFile* file = new TFile();
    file=file->Open(fullpath.c_str(),"READ");
    if (!file->IsOpen())
    {
        std::string error="The file "+fullpath+" wasn't found or is impossible to open";
        ATH_MSG_ERROR(error);
        return StatusCode::FAILURE;
    }
    else {
      file->ls();
      std::string info="The file "+fullpath+ " found and opened";
      ATH_MSG_INFO(info);
    }
    //Define detector sectors
    std::vector<std::string> sectors={"barrel","endcap"};
    // Find the histogram by name
    for (auto histoname: histonames){
      for (auto sector: sectors){
	std::string full_histoname="sct/"+sector+"/"+histoname;
	TH1F* h = (TH1F*) file->Get(full_histoname.c_str());
	if (!h)
	{
	  std::string error="The histogram "+full_histoname+" wasn't found";
	  ATH_MSG_ERROR(error);
	  return StatusCode::FAILURE;
	}
	h->SetDirectory(0);
	Ineff_scale_factors[sector+"_"+histoname]=h;
      }
    } 
    
    
    file->Close(); 
    delete file;
    return StatusCode::SUCCESS;
}

double SCT_FastDigitizationTool::RetrieveInefficiencySF(double eta,double mu, bool isBarrel){
  
 std::string pileup_level;
 if (mu < 20.){
  pileup_level="low_pileup";}
 else if( mu < 35.){
  pileup_level="med_pileup";
 }
 else{
  pileup_level="high_pileup"; 
 }
 
 if( isBarrel){
   pileup_level="barrel_"+pileup_level;
 }
 else{
   pileup_level="endcap_"+pileup_level;
 }
   
  int bin =  Ineff_scale_factors[pileup_level]->FindBin(eta);
  return (Ineff_scale_factors[pileup_level]->GetBinContent(bin) - 1);
}
