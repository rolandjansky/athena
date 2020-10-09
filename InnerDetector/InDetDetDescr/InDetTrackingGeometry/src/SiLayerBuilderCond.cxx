/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiLayerBuilderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometry/SiLayerBuilderCond.h"
#include "InDetTrackingGeometry/PixelOverlapDescriptor.h"
#include "InDetTrackingGeometry/SCT_OverlapDescriptor.h"
#include "InDetTrackingGeometry/DiscOverlapDescriptor.h"
//InDet include
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/SCT_ID.h" 
#include "InDetIdentifier/PixelID.h" 
// Trk inlcude
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
// GeoModel
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
// STL
#include <map>

std::vector<const Trk::CylinderLayer*> InDet::SiLayerBuilderCond::s_splitCylinderLayers;
std::vector<const Trk::DiscLayer*>     InDet::SiLayerBuilderCond::s_splitDiscLayers;
EventIDRange                           InDet::SiLayerBuilderCond::s_splitIOVRange;
double InDet::SiLayerBuilderCond::s_splitRadius = 0.;
    
// constructor
InDet::SiLayerBuilderCond::SiLayerBuilderCond(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixelCase(true),
  m_siMgr(0),
  m_siMgrLocation("Pixel"),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_setLayerAssociation(true),
  m_barrelLayerBinsZ(100),
  m_barrelLayerBinsPhi(1),
  m_barrelEnvelope(0.1),
  m_barrelEdbTolerance(0.05),
  m_endcapRingLayout(false),
  m_endcapLayerBinsR(100),
  m_endcapLayerBinsPhi(1),
  m_endcapEnvelope(0.1),
  m_endcapComplexRingBinning(true),
  m_identification("Pixel"),
  m_splitMode(0),
  m_splitTolerance(10.),   
  m_runGeometryValidation(true)
{
  declareInterface<Trk::ILayerBuilderCond>(this);
  // general steering
  declareProperty("PixelCase"                        , m_pixelCase);
  declareProperty("SiDetManagerLocation"             , m_siMgrLocation);
  declareProperty("SetLayerAssociation"              , m_setLayerAssociation);
  // For the Active Barrel Material
  declareProperty("BarrelAdditionalLayerRadii"       , m_barrelAdditionalLayerR);
  declareProperty("BarrelAdditionalLayerType"        , m_barrelAdditionalLayerType);
  declareProperty("BarrelLayerBinsZ"                 , m_barrelLayerBinsZ);
  declareProperty("BarrelLayerBinsPhi"               , m_barrelLayerBinsPhi);
  declareProperty("BarrelEnvelope"                   , m_barrelEnvelope);
  declareProperty("BarrelEdbTolerance"               , m_barrelEdbTolerance);
  // For the Active Endcap Material
  declareProperty("EndcapRingLayout"                 , m_endcapRingLayout);
  declareProperty("EndcapAdditionalLayerPositionsZ"  , m_endcapAdditionalLayerPosZ);
  declareProperty("EndcapAdditionalLayerType"        , m_endcapAdditionalLayerType);
  declareProperty("EndcapLayerBinsR"                 , m_endcapLayerBinsR);
  declareProperty("EndcapLayerBinsPhi"               , m_endcapLayerBinsPhi);
  declareProperty("EndcapEnvelope"                   , m_endcapEnvelope);
  declareProperty("EndcapComplexRingBinning"         , m_endcapComplexRingBinning);
  // identification
  declareProperty("Identification"                   , m_identification);
  // split mode for multiple pixel systems (upgrade)
  declareProperty("SplitMode"                        , m_splitMode);
  declareProperty("SplitTolerance"                   , m_splitTolerance);      
  // Validation
  declareProperty("GeometryValidation"               , m_runGeometryValidation);
}

// destructor
InDet::SiLayerBuilderCond::~SiLayerBuilderCond()
{}

// Athena standard methods
// initialize
StatusCode InDet::SiLayerBuilderCond::initialize()
{

    ATH_MSG_DEBUG( "initialize()" );
    // get Pixel Detector Description Manager
    if (m_pixelCase){
        const InDetDD::PixelDetectorManager* pixMgr = 0;
        if ((detStore()->retrieve(pixMgr, m_siMgrLocation)).isFailure()) {
            ATH_MSG_ERROR( "Could not get PixelDetectorManager '" << m_siMgrLocation << "', no layers for Pixel Detector will be built. " );
        } else {
            ATH_MSG_VERBOSE( "PixelDetectorManager retrieved with key '" << m_siMgrLocation <<"'." );
            // assign the detector manager to the silicon manager
            m_siMgr = pixMgr;
            if (detStore()->retrieve(m_pixIdHelper, "PixelID").isFailure())
                ATH_MSG_ERROR("Could not get Pixel ID helper");
        }
        ATH_CHECK(m_PixelReadKey.initialize());
    } else {
        const InDetDD::SCT_DetectorManager* sctMgr = 0;
        if ((detStore()->retrieve(sctMgr, m_siMgrLocation)).isFailure()) {
            ATH_MSG_ERROR( "Could not get SCT_DetectorManager '" << m_siMgrLocation << "', no layers for SCT Detector will be built. " );
        } else {
            ATH_MSG_VERBOSE( "SCT_DetectorManager retrieved with key '" << m_siMgrLocation <<"'." );
            // assign the detector manager to the silicon manager
            m_siMgr = sctMgr;
            if (detStore()->retrieve(m_sctIdHelper, "SCT_ID").isFailure())
                ATH_MSG_ERROR("Could not get SCT ID helper");
        }
        
        ATH_CHECK(m_SCT_ReadKey.initialize());
        ATH_CHECK(m_PixelReadKey.initialize());

    }
    return StatusCode::SUCCESS;
}

// finalize
StatusCode InDet::SiLayerBuilderCond::finalize()
{
    ATH_MSG_DEBUG( "finalize() successful" );
    return StatusCode::SUCCESS;
}


/** LayerBuilder interface method - returning Barrel-like layers */
std::pair<EventIDRange, const std::vector<const Trk::CylinderLayer*>*> InDet::SiLayerBuilderCond::cylindricalLayers(const EventContext& ctx) const
{

  // split mode 2nd part return the already built layers 
  if (m_splitMode && s_splitCylinderLayers.size() ){
      ATH_MSG_DEBUG( "[ Split mode/ Part 2 ] Returning " << s_splitCylinderLayers.size() << " cylinder layers." );
      ATH_MSG_VERBOSE( "                       Split radius was set to " << s_splitRadius );
      std::vector<const Trk::CylinderLayer*>* splitCylinderLayers = dressCylinderLayers(s_splitCylinderLayers);
      s_splitCylinderLayers.clear();
      return std::make_pair(s_splitIOVRange, splitCylinderLayers); 
  } else if (m_splitMode){
      ATH_MSG_DEBUG( "[ Split mode/ Part 1 ] Initializing." );
      s_splitRadius = m_splitMode < 0 ? 10e10 : 0.;
  }
  

  // sanity check for ID Helper
  if (!m_pixIdHelper && !m_sctIdHelper){
    ATH_MSG_ERROR("Neither Pixel nor SCT Detector Manager or ID Helper could be retrieved - giving up.");
    //create dummy infinite range
    EventIDRange range;
    return std::pair<EventIDRange, const std::vector<const Trk::CylinderLayer*>*>(range,nullptr);
  }

  // take the numerlogoy
  const InDetDD::SiNumerology& siNumerology = m_siMgr->numerology();     
    
  // pre-set parameters for the run
  size_t barrelLayers = 0;
  // save way to estimate the number of barrel layers : they can be deactivated hence the useLayer(i) check
  for (int i = 0; i < siNumerology.numLayers(); i++)
       if (siNumerology.useLayer(i)) barrelLayers++;
  

  // screen output
  ATH_MSG_DEBUG( "Configured to build " << barrelLayers << " (active) barrel layers (out of " << siNumerology.numLayers() << " )" );
  if (m_barrelAdditionalLayerR.size())
      ATH_MSG_DEBUG( "Additionally " <<  m_barrelAdditionalLayerR.size() << " material layers will be built.");  
      
  // split mode for SLHC setup
  if (m_splitMode)
      ATH_MSG_DEBUG( "[ Split mode ] Some layers may be cached." );
        
  // for barrels (the statistics for ordering the modules)
  std::vector<double>                                               layerRadius(barrelLayers,0.);
  std::vector<double>                                               layerRmin(barrelLayers,10e10);
  std::vector<double>                                               layerRmax(barrelLayers,0.);
  std::vector<double>                                               layerThickness(barrelLayers,0.);
  std::vector<double>                                               layerMinZ(barrelLayers,0.);
  std::vector<double>                                               layerMaxZ(barrelLayers,0.);
  std::vector<double>                                               layerHalfLength(barrelLayers,0.);
  std::vector<double>                                               layerMinPhi(barrelLayers,0.);
  std::vector<double>                                               layerMaxPhi(barrelLayers,0.);
  std::vector<size_t>                                               layerPhiSectors(barrelLayers,0);
  std::vector<size_t>                                               layerZsectors(barrelLayers,0);
  std::vector< std::vector<float> >                                 layerZboundaries(barrelLayers, std::vector<float>());
  std::vector< std::vector< Trk::SurfaceOrderPosition > >           layerSurfaces(barrelLayers, std::vector< Trk::SurfaceOrderPosition >());
 
  // cache needed
  double minHalflengthZ         = 10e10;
  double maxHalflengthZ         = 0;
  size_t sumCheckBarrelModules  = 0;
  size_t barrelModules          = 0;
 
  // [-A-] ------------------------ LOOP over Detector Elements of sensitive layers -----------------------------------
  // iterate over the detector elements for layer dimension, etc.   
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection>* readHandle;
  if(m_pixelCase){
    readHandle = new SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> (m_PixelReadKey, ctx);
  }else{
    readHandle = new SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> (m_SCT_ReadKey, ctx);
  }
  const InDetDD::SiDetectorElementCollection* readCdo{**readHandle};
  InDetDD::SiDetectorElementCollection::const_iterator sidetIter = readCdo->begin();    
  for (; sidetIter != readCdo->end(); ++sidetIter){
     // Barrel check
     if ((*sidetIter) && (*sidetIter)->isBarrel()){
       // unit test
       ++barrelModules;
       // get the identifier
       Identifier    currentId((*sidetIter)->identify());
       int currentlayer = m_pixIdHelper ? m_pixIdHelper->layer_disk(currentId) : m_sctIdHelper->layer_disk(currentId);
       //skip the layer if it is chosen to be switched off
       if ( !m_siMgr->numerology().useLayer(currentlayer) ) continue;
       // (A) Determination of phi / eta sectors  ---------------------------------------------------
       // only do the exercise if it hasn't been done already 
       if (layerPhiSectors[currentlayer] == 0){                        
          ATH_MSG_VERBOSE("Pre-processing Elements from Layer (id from idHelper): " << currentlayer );      
          // set number of phiSectors
          layerPhiSectors[currentlayer] = m_siMgr->numerology().numPhiModulesForLayer(currentlayer);
          // set number of etaSectors
          layerZsectors[currentlayer] = m_siMgr->numerology().numEtaModulesForLayer(currentlayer);
          // get the HalfLength of the Layer
          const InDetDD::SiDetectorElement* countPtr = (*sidetIter);
          // needed for the complex z binning if activated
          double lastModuleZ = 0.; 
          std::vector<float> zboundaries; 
          zboundaries.reserve(layerZsectors[currentlayer]+1);               
          // walk all along to negative eta
          while ((countPtr->prevInEta()))
                 countPtr = countPtr->prevInEta();
           layerMinZ[currentlayer] = countPtr->center().z() - 0.5*fabs(countPtr->length());
           zboundaries.push_back(layerMinZ[currentlayer]);
           lastModuleZ   = countPtr->center().z();
           // register the layer minZ into the zboundaries         
           // now walk all along to positive eta
           while ((countPtr->nextInEta())) {
                  countPtr = countPtr->nextInEta();
                  // for complex binning
                  double currentModuleZ   = countPtr->center().z();
                  double currentZboundary = 0.5*(lastModuleZ+currentModuleZ);
                  zboundaries.push_back(currentZboundary);
                  lastModuleZ   = currentModuleZ;
           }
           layerMaxZ[currentlayer] = fabs(countPtr->center().z()) + 0.5*fabs(countPtr->length());
           zboundaries.push_back(layerMaxZ[currentlayer]);
           
           // complex z binning mode
           layerZboundaries[currentlayer] = zboundaries;
           // chose which one to register for the split mode (SLHC)
           layerHalfLength[currentlayer] =  layerMinZ[currentlayer]*layerMinZ[currentlayer] > layerMaxZ[currentlayer]*layerMaxZ[currentlayer] ?
               fabs(layerMinZ[currentlayer]) : layerMaxZ[currentlayer];
           // get the haflength of the layer
           takeSmaller( minHalflengthZ, layerHalfLength[currentlayer]);
           takeBigger( maxHalflengthZ, layerHalfLength[currentlayer]);
           ATH_MSG_VERBOSE("      -> Determined Layer z range with  : " << layerMinZ[currentlayer] << " / " << layerMaxZ[currentlayer] );
           ATH_MSG_VERBOSE("      -> Symmetric half length taken    : " << layerHalfLength[currentlayer]);
       }
                    
       // (B) Determination of the radius ------------------------------------------------
       // getting inner module
       const InDetDD::SiDetectorElement* otherSide = (*sidetIter)->otherSide();
       // take the maximum layer radius 
       double currentR    = (*sidetIter)->center().perp(); 
       double currentRmax = (*sidetIter)->rMax();
       double currentRmin = (*sidetIter)->rMin();
       layerRadius[currentlayer]  = (currentR > layerRadius[currentlayer]) ? currentR : layerRadius[currentlayer];
       if (otherSide){
           takeBigger( currentRmax, (otherSide)->rMax() );
           takeSmaller( currentRmin, (otherSide)->rMin() );
       }
       takeSmaller( layerRmin[currentlayer], currentRmin );
       takeBigger(  layerRmax[currentlayer], currentRmax );

       // handle the split mode
       if (m_splitMode > 0) takeBigger(s_splitRadius, layerRadius[currentlayer]);
       else if (m_splitMode < 0) takeSmaller (s_splitRadius, layerRadius[currentlayer]);
           
       // fill the Surface vector
       Amg::Vector3D orderPosition((*sidetIter)->center());
       double currentPhi = orderPosition.phi();
	   takeSmaller( layerMinPhi[currentlayer], currentPhi);
       takeBigger( layerMaxPhi[currentlayer], currentPhi);
       
       // decide which one to register on the Radius: always the one with smaller radius
       bool takeIt =  (!otherSide || (*sidetIter)->center().perp() < otherSide->center().perp() );
       const Trk::Surface* moduleSurface = takeIt ? (&((*sidetIter)->surface())) : (&(otherSide->surface()));
  
       // register the module surface
       Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface, Trk::do_not_delete<const Trk::Surface>);
  
       Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
       if (takeIt) (layerSurfaces[currentlayer]).push_back(surfaceOrder);
     
     } else if (!(*sidetIter)) // barrel chek and screen output
        ATH_MSG_WARNING("NULL pointer to Barrel module given by SiDetectorManager! Please check db & dict.xml");
  
  } // SiDet Loop

  // --------------------------- enf of LOOP over Detector Elements of sensitive layers ----------------------------------
   
  // [-B-] ------------------------ Construction of the layers -----------------------------------          
  // [-B-1] construct the detection layers
  std::vector< const Trk::CylinderLayer* > cylinderDetectionLayers;
  int layerCounter          = 0;
  double currentLayerExtend = 0.;
  
  // construct detection layers
  for (auto& layerRadiusIter : layerRadius) {
                  
      Trk::CylinderLayer* activeLayer   = 0;
      double currentLayerRadius         = 10e10;
      bool splitDone                    = false;
      // non-equidistant binning used ? auto-detection 
      bool nonEquidistantBinning = false;
      {
          // calculate the average bin size
          const double averageBinSize = (layerMaxZ[layerCounter]-layerMinZ[layerCounter])/(layerZsectors[layerCounter]);
          const double inv_averageBinSize2 = 1. / (averageBinSize*averageBinSize);
          // loop over the boundaries and check if theyare outside the tolerance
          auto bIter  = layerZboundaries[layerCounter].begin();
          auto bIterE = layerZboundaries[layerCounter].end();
          for ( ++bIter; bIter != bIterE; ++bIter ){
              float cZ = (*bIter);
              float pZ = (*(bIter-1));
              nonEquidistantBinning =  (cZ-pZ)*(cZ-pZ)*inv_averageBinSize2 < (1.-m_barrelEdbTolerance) *(1.-m_barrelEdbTolerance);
              if (nonEquidistantBinning){
                  ATH_MSG_VERBOSE("Non-equidistant binning for (Silicon) Surfaces on this layer with radius " << layerRadiusIter << " detected. ");
                  ATH_MSG_VERBOSE("Difference " << (cZ-pZ)/averageBinSize << " at a allowed tolerance of : " << m_barrelEdbTolerance );
                  break;
              }
          }
      }
      
      //(1) the detecting layer : sensitive modules --------------------------------------------------------------------
      // create the BinKeyUtility - the phi binning is identical
      double halfPhiStep = M_PI/layerPhiSectors[layerCounter];
      // protection in case phi value was fluctuating around 0 or M_PI in parsing
      if (fabs(layerMinPhi[layerCounter]+layerMaxPhi[layerCounter])< halfPhiStep && fabs(M_PI+layerMinPhi[layerCounter]) < 0.5*halfPhiStep ){
          ATH_MSG_VERBOSE("Detected module fluctuation around +/- M_PI, correcting for it.");
          ATH_MSG_VERBOSE("    min phi / max phi detected  : "  << layerMinPhi[layerCounter] << " / " << layerMaxPhi[layerCounter] );
          layerMinPhi[layerCounter] += 2*halfPhiStep;
      } 
      // now prepare the phi values
      ATH_MSG_VERBOSE("Preparing the Phi-binning for   : " << layerPhiSectors[layerCounter] << " sectors.");
      ATH_MSG_VERBOSE("    min phi / max phi detected  : " << layerMinPhi[layerCounter] << " / " << layerMaxPhi[layerCounter] );
      double minPhiCorrected = layerMinPhi[layerCounter]-halfPhiStep;
      double maxPhiCorrected = layerMaxPhi[layerCounter]+halfPhiStep;
      // catch if the minPhi falls below M_PI
      if (minPhiCorrected < -M_PI){
          minPhiCorrected += 2*halfPhiStep;
          maxPhiCorrected += 2*halfPhiStep;
      }
      ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );
       
      
      Trk::BinUtility* currentBinUtility   =  new Trk::BinUtility(layerPhiSectors[layerCounter],
	  						                                      minPhiCorrected,
	  						                                      maxPhiCorrected,
	  						                                      Trk::closed, Trk::binPhi);
      if (nonEquidistantBinning)
          (*currentBinUtility) += Trk::BinUtility(layerZboundaries[layerCounter],
                                                  Trk::open,
                                                  Trk::binZ);
      else 
          (*currentBinUtility) += Trk::BinUtility(layerZsectors[layerCounter],
                                                  layerMinZ[layerCounter],
                                                  layerMaxZ[layerCounter],
                                                  Trk::open,
                                                  Trk::binZ);
      // creating the binned array output
      ATH_MSG_VERBOSE("Creating the binned array for the sensitive detector elements with BinUtility :");
      ATH_MSG_VERBOSE( *currentBinUtility );
      // the binned array for the senstive surfaces to be built            
      Trk::BinnedArray<Trk::Surface>* currentBinnedArray = 
            new Trk::BinnedArray2D<Trk::Surface>(layerSurfaces[layerCounter],currentBinUtility);       
      // unit test for sub surface ordering
	  const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects();          

      if (m_runGeometryValidation){
         // checking for :
         //   - empty surface bins
         //   - doubly filled bins
         std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
         auto usmIter = uniqueSurfaceMap.end();
         // ------- iterate  
         auto asurfIter = arraySurfaces.begin();
         auto asurfIterEnd = arraySurfaces.end();
         for ( ; asurfIter != asurfIterEnd; ++asurfIter){
             if ( (*asurfIter) ) {
                ++sumCheckBarrelModules;
                usmIter = uniqueSurfaceMap.find(*asurfIter);
                if ( usmIter != uniqueSurfaceMap.end() )
                    ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
                else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
	       }
	       else 
               ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector");
         }
      }

      // dynamic layer extend and splitDone check (split mode need the dynamic extend !)
      currentLayerExtend = layerHalfLength[layerCounter];
      
      // check if split has been done
      // split mode < 0 : compare to maxHalfLength
      // split mode > 0 : compare to minHalflength
      double compareHalfLengthZ = m_splitMode < 0 ? maxHalflengthZ : minHalflengthZ;
      
      splitDone = m_splitMode && fabs(layerHalfLength[layerCounter]-compareHalfLengthZ) > m_splitTolerance;
      if (m_splitMode){
           ATH_MSG_DEBUG( "[ Split mode / part 1 ] Layer Halflength determined as: " << layerHalfLength[layerCounter]);
           ATH_MSG_DEBUG( "                               while minHalflengthZ is: " << minHalflengthZ );
           ATH_MSG_DEBUG( "                                     -> consequence is: " << (splitDone ? "store for iteration 2" : "build") );
       }
      
      // dynamic layer extend determined by the sensitive layer dimensions
      layerRadius[layerCounter] = 0.5*(layerRmax[layerCounter] + layerRmin[layerCounter]);
      layerThickness[layerCounter] = layerRmax[layerCounter] - layerRmin[layerCounter];

      // create the material
      const Trk::LayerMaterialProperties* layerMaterial = barrelLayerMaterial(layerRadius[layerCounter], currentLayerExtend);
      double currentLayerThickness = layerThickness[layerCounter]+m_barrelEnvelope;
      
      // screen output
      ATH_MSG_DEBUG( "Construct BinnedArray for CylinderLayer with "<< (layerSurfaces[layerCounter]).size() << " SubSurfaces." );
      ATH_MSG_DEBUG( "Building a CylinderLayer with " << layerPhiSectors[layerCounter]
          << " / " <<   ( nonEquidistantBinning ? layerZboundaries[layerCounter].size() :  layerZsectors[layerCounter] ) << " phi/z bins. "   );
      ATH_MSG_DEBUG( "  -> With Radius     :  " << layerRadius[layerCounter]    );       
      ATH_MSG_DEBUG( "  -> With Thickness  :  " << currentLayerThickness << " - includes envelope tolerance : " << m_barrelEnvelope );
      ATH_MSG_DEBUG( "  -> With Zmin/Zmax  :  " << -currentLayerExtend << " / " << currentLayerExtend );
      
      if ( nonEquidistantBinning && layerZboundaries[layerCounter].size() ){
          // overrule the min bin - with the min radius 
          // do the output to screen
          double currentZ = -currentLayerExtend;
          msg(MSG::DEBUG) <<  "  -> Z binning at      :  ";
          for (size_t zbin = 0; zbin < layerZboundaries[layerCounter].size(); ++zbin) {
              currentZ += layerZboundaries[layerCounter][zbin];
              msg(MSG::DEBUG) << currentZ;
              if (zbin <  layerZboundaries[layerCounter].size()-1) 
                 msg(MSG::DEBUG) << ", ";
          }
          msg(MSG::DEBUG) << endmsg;           
      }
      // prepare the right overlap descriptor       
      Trk::OverlapDescriptor* olDescriptor = 0;
      if (m_pixelCase)
          olDescriptor = new InDet::PixelOverlapDescriptor;
      else olDescriptor = new  InDet::SCT_OverlapDescriptor;
      // for eventual use with the passive layer
      currentLayerRadius = layerRadius[layerCounter];

      // construct the layer (finally)
      activeLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(layerRadius[layerCounter],currentLayerExtend),
                                           currentBinnedArray,
                                           *layerMaterial,
                                           layerThickness[layerCounter],
                                           olDescriptor);
      // cleanup of the layer material --------------------------------------------------------------
      delete layerMaterial; 
      // register the layer to the surfaces
      const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
      registerSurfacesToLayer(layerSurfaces,*activeLayer);

      // (3) register the layers --- either in the split vector or in the return vector 
      if (splitDone) {
          ATH_MSG_DEBUG( "[ Split mode / Part 1 ] Layer cached for Part 2" );
          readHandle->range(s_splitIOVRange);
          s_splitCylinderLayers.push_back(activeLayer);   
          // get the split radius to the smallest one possible
          if (m_splitMode > 0) takeSmaller( s_splitRadius, currentLayerRadius);
          ATH_MSG_DEBUG("[ Split mode / part 1 ] Split radius (temproarily) set to : " << s_splitRadius );
       } else {
          if (m_splitMode < 0) takeBigger ( s_splitRadius, currentLayerRadius );
          cylinderDetectionLayers.push_back(activeLayer);
       } 
       // increase the layer counter --- it is built
       ++layerCounter;            

  } // layer construction
  // --------------------------- enf of detection layer construction loop ----------------------------------

  ATH_MSG_DEBUG("Creating the final CylinderLayer collection with (potentially) additional layers.");
  std::vector< const Trk::CylinderLayer* >* cylinderLayers = dressCylinderLayers(cylinderDetectionLayers);
  
  // multiply the check number in case of SCT 
  sumCheckBarrelModules *= (m_pixelCase) ? 1 : 2; 
  ATH_MSG_DEBUG( barrelModules << " Barrel Modules parsed for Cylinder Layer dimenstions." );
  ATH_MSG_DEBUG( sumCheckBarrelModules << " Barrel Modules filled in Cylinder Layer Arrays." );
  if ( barrelModules-sumCheckBarrelModules )
     ATH_MSG_WARNING( barrelModules-sumCheckBarrelModules << " Modules not registered properly in binned array." );       

  if (m_splitMode)
     ATH_MSG_DEBUG("[ Split mode / part 1 ] Split radius determined as : " << s_splitRadius );

  ATH_MSG_DEBUG("Returning " << cylinderLayers->size() << " cylinder layers.");
  EventIDRange range;
  readHandle->range(range);
  delete readHandle;
  std::pair<EventIDRange, const std::vector<const Trk::CylinderLayer*>*> cylinderLayersPair = std::make_pair(range,cylinderLayers);
  return cylinderLayersPair;
} 
      
/** LayerBuilder interface method - returning Endcap-like layers */
std::pair<EventIDRange, const std::vector<const Trk::DiscLayer*>*> InDet::SiLayerBuilderCond::discLayers(const EventContext& ctx) const
{
 
  // TODO: remove s_splitDiscLayers cache to make threadsafe. ignored for now as is only for ITk
  // split mode 2nd 
  if (m_splitMode && s_splitDiscLayers.size() ){
    ATH_MSG_DEBUG( "[ Split mode/ Part 2 ] Returning " << s_splitDiscLayers.size() << " disc layers." );
    std::vector<const Trk::DiscLayer*>* splitDiscs = new std::vector<const Trk::DiscLayer*>(s_splitDiscLayers);
    s_splitDiscLayers.clear();
    return std::make_pair(s_splitIOVRange, splitDiscs); 
  }
  // sanity check for ID Helper
  if (!m_pixIdHelper && !m_sctIdHelper){
       ATH_MSG_ERROR("Neither Pixel nor SCT Detector Manager or ID Helper could be retrieved - giving up.");
    //create dummy infinite range
    EventIDRange range;
    return std::pair<EventIDRange, const std::vector<const Trk::DiscLayer*>*>(range,nullptr);
  } 

  // check for DBMS
  int nDBMLayers = m_siMgr->numerology().numEndcapsDBM();
  if (!nDBMLayers) return createDiscLayers(ctx);
  
  ATH_MSG_DEBUG( "Found " << m_siMgr->numerology().numEndcapsDBM() << " DBM layers active, building first ECs, then DBMS");
  std::pair<EventIDRange, std::vector<const Trk::DiscLayer*>*>  ecLayers = createDiscLayers(ctx);
  if (ecLayers.second) {
      ATH_MSG_VERBOSE( "Created " << ecLayers.second->size() << " endcap layers w/o  DBM.");
      ecLayers = createDiscLayers(ctx, ecLayers.second);
      ATH_MSG_VERBOSE( "Created " << ecLayers.second->size() << " endcap layers with DBM.");
  }
  return ecLayers;

}

/** LayerBuilder interface method - returning Endcap-like layers */
std::pair<EventIDRange, std::vector< const Trk::DiscLayer* >* > InDet::SiLayerBuilderCond::createDiscLayers(const EventContext& ctx, std::vector<const Trk::DiscLayer*>* dLayers) const {
 
  // this is the DBM flag
  bool isDBM = (dLayers!=NULL);
  
  // get general layout
  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection>* readHandle;
  if(m_pixelCase){
    readHandle = new SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> (m_PixelReadKey, ctx);
  }else{
    readHandle = new SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> (m_SCT_ReadKey, ctx);
  }
  const InDetDD::SiDetectorElementCollection* readCdo{**readHandle};
  InDetDD::SiDetectorElementCollection::const_iterator sidetIter = readCdo->begin();    
    
  // save way to estimate the number of barrels
  unsigned int endcapLayers = 0;
  if (isDBM){
     endcapLayers = m_siMgr->numerology().numDisksDBM();
     ATH_MSG_DEBUG( "Configured to build " << endcapLayers << "*2 disc-like DBM layers" );
  } else {           
      for (int i = 0; i < m_siMgr->numerology().numDisks(); i++)
          if (m_siMgr->numerology().useDisk(i)) endcapLayers++;
     ATH_MSG_DEBUG( "Configured to build " << endcapLayers << " *2 disc-like layers  (+ additional support layers)." );
  }
 
  if (m_splitMode) 
      ATH_MSG_DEBUG( "[ Split mode ] Some layers may bee cached." );

  // prepare the vectors
  std::vector<double>                                     discZmin(2*endcapLayers,10e10);
  std::vector<double>                                     discZmax(2*endcapLayers,-10e10);
  std::vector<double>                                     discZpos(2*endcapLayers,0.);
  std::vector<double>                                     discRmin(2*endcapLayers,10e10);
  std::vector<double>                                     discRmax(2*endcapLayers,0);
  std::vector<double>                                     discThickness(2*endcapLayers,0.);
  
  std::vector< std::vector<Trk::SurfaceOrderPosition> >   discSurfaces;  
  std::vector< std::vector<int> >                         discPhiSectors;
  std::vector< std::vector<double> >                      discPhiMin;
  std::vector< std::vector<double> >                      discPhiMax;
  std::vector< std::vector<double> >                      discRingMinR;
  std::vector< std::vector<double> >                      discRingMaxR;  
    
  // let's make sure the discs are ordered in z: that's the z / map index
  std::map< double, int>                                  discZposLayerIndex;  
    
  // reserve -- better memory management    
  discPhiMin.reserve(2*endcapLayers);
  discPhiMax.reserve(2*endcapLayers);
  discPhiSectors.reserve(2*endcapLayers);
  discSurfaces.reserve(2*endcapLayers);
  discRingMinR.reserve(2*endcapLayers);
  discRingMaxR.reserve(2*endcapLayers);
   
  // initialize for checks (pos/neg discs -> 2 times discs)
  for (unsigned int endcap=0; endcap<2*endcapLayers; endcap++){
        discPhiMin.push_back(std::vector<double>());
        discPhiMax.push_back(std::vector<double>());
        discPhiSectors.push_back(std::vector<int>());
        discSurfaces.push_back( std::vector<Trk::SurfaceOrderPosition>() );
        // auto-detection
        discRingMinR.push_back(std::vector<double>());
        discRingMaxR.push_back(std::vector<double>());      
   } // end of for loop
   
  int endcapModules = 0;
  int sumCheckEndcapModules = 0;
  unsigned int currentlayer = 0;
  unsigned int currentdisk  = 0;
  unsigned int currentring  = 0;
  
  // [-A1-] ------------------------ first LOOP over Detector Elements of sensitive layers -------------------------------                 
  // -- get the missing dimensions by loop over DetElements
  sidetIter = readCdo->begin();
  for (; sidetIter != readCdo->end(); ++sidetIter){
     // take it - if 
     // a) you have a detector element ... protection
     // b) the detector element is EC (in the non-DBM case)
     // c) the detector elemet is DBM in the DBM case
     if ( (*sidetIter) && ( (!isDBM && (*sidetIter)->isEndcap()) || (isDBM && (*sidetIter)->isDBM())) ){
     
       // increase the counter of endcap modules
        endcapModules++;
        // parse all z positions for the mean value of the discs
        double currentZ = (*sidetIter)->center().z();
        // get the identifier & calculate current layer and current disk from it     
        Identifier    currentId((*sidetIter)->identify());
        currentdisk  = m_pixIdHelper ? m_pixIdHelper->layer_disk(currentId) : m_sctIdHelper->layer_disk(currentId);
        currentlayer = currentdisk;
        currentlayer += currentZ > 0. ? endcapLayers : 0;

        // check the current ring
        currentring = m_pixIdHelper ? m_pixIdHelper->eta_module(currentId) : m_sctIdHelper->eta_module(currentId);

        takeSmallerBigger(discZmin[currentlayer],discZmax[currentlayer],currentZ);

        // set the disc Rmin / Rmax  
        double currentRmin = (*sidetIter)->rMin();
        double currentRmax = (*sidetIter)->rMax();

        // how many rings do we have ?
        unsigned int diskRings =  isDBM ? 
            m_siMgr->numerology().numRingsForDiskDBM(currentdisk) : 
            m_siMgr->numerology().numRingsForDisk(currentdisk);
               
        // the current phi 
        double currentPhi = (*sidetIter)->center().phi();
        takeSmaller(discRmin[currentlayer],currentRmin);
        takeBigger( discRmax[currentlayer],currentRmax);

        //fill the number of phi sectors for the different rings
        if (!discPhiSectors[currentlayer].size()){
           ATH_MSG_VERBOSE("Pre-processing Elements from Disk/Layer (id from idHelper): " << currentdisk << "/" << currentlayer );
           // prepare the ring bins, initialise the first one to be something big
           discPhiMin[currentlayer]      = std::vector<double>(diskRings,100.); 
           discPhiMax[currentlayer]      = std::vector<double>(diskRings,-100.); 
           discRingMinR[currentlayer]    = std::vector<double>(diskRings,1e10);
           discRingMaxR[currentlayer]    = std::vector<double>(diskRings,0);
           // fill the phi sectors 
           ATH_MSG_VERBOSE("-> The current disk has " << diskRings << " ring(s)");
           for (unsigned int iring=0; iring < diskRings; ++iring){
               unsigned int phiSectorsRing = isDBM ? 
                   m_siMgr->numerology().numPhiModulesForDiskRingDBM(currentdisk, iring) :
                   m_siMgr->numerology().numPhiModulesForDiskRing(currentdisk, iring);
              // get the number of phi sectors
              ATH_MSG_VERBOSE("--> Ring " << iring << " has " << phiSectorsRing << " phi sectors");
              discPhiSectors[currentlayer].push_back(phiSectorsRing);
           }
        }
        // we take the phi / r binning only from the closer to IP module 
        if ( !(*sidetIter)->otherSide() || fabs(currentZ) < fabs((*sidetIter)->otherSide()->center().z())){
            // take phi-min and phimax
            takeSmaller(discPhiMin[currentlayer][currentring],currentPhi);
            takeBigger(discPhiMax[currentlayer][currentring],currentPhi);
            // record the smalles ring & biggest ring radius
            takeSmaller(discRingMinR[currentlayer][currentring],currentRmin);
            takeBigger(discRingMaxR[currentlayer][currentring],currentRmax);
        }
     } else if (!(*sidetIter))
        ATH_MSG_WARNING("NULL pointer to Endcap module given by SCT_DetectorManager! Please check db & dict.xml");
  } // DetElement loop 

  double minRmin = 10e10;
  double maxRmax = -10e10;
  
  ATH_MSG_VERBOSE("Estimating the average z position and the radius for each disk.");
  // get the average z-position per layer & estimate thes thickness
  for (unsigned int iec=0; iec<2*endcapLayers; ++iec){
       takeSmaller(minRmin,discRmin[iec]); takeBigger(maxRmax,discRmax[iec]);
       // average it out
       discZpos[iec]        = 0.5 * (discZmin[iec] + discZmax[iec]);
       discThickness[iec]   = isDBM ? 1. : fabs(discZmax[iec]-discZmin[iec]);
       // make the map z / index
       discZposLayerIndex.insert(std::make_pair(discZpos[iec],iec));
  }

  // [-A2-] ------------------------ second LOOP over Detector Elements of sensitive layers -------------------------------                 
  // fill the elements for the layers into the surface arrays
  sidetIter = readCdo->begin();
  for (; sidetIter != readCdo->end(); ++sidetIter){
    // Endcap
    if ( ((*sidetIter) && ((!isDBM && (*sidetIter)->isEndcap()) || (isDBM && (*sidetIter)->isDBM()))) ){     
        // get the identifier & calculate current layer and current disk from it     
        Identifier    currentId((*sidetIter)->identify());
        currentdisk  = m_pixIdHelper ? m_pixIdHelper->layer_disk(currentId) : m_sctIdHelper->layer_disk(currentId);
        currentlayer = currentdisk;
        currentlayer += (*sidetIter)->center().z() > 0. ? endcapLayers : 0;
        // decision which module to take
        const InDetDD::SiDetectorElement* otherSide = (*sidetIter)->otherSide();    
        bool takeIt =  (!otherSide || fabs((*sidetIter)->center().z()) < fabs(otherSide->center().z()) );
        const InDetDD::SiDetectorElement* chosenSide = takeIt ?  (*sidetIter) : otherSide;
        // get the center position
        const Amg::Vector3D& orderPosition = chosenSide->center();
        // register the chosen side in the object array
        Trk::SharedObject<const Trk::Surface> sharedSurface(&(chosenSide->surface()), [](const Trk::Surface*){});
        Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
        if (takeIt) (discSurfaces[currentlayer]).push_back(surfaceOrder);      
    }      
  } //end of filling
    
  // [-B-] ------------------------ Construction of the layers -----------------------------------
  // construct the layers
  std::vector< const Trk::DiscLayer* >* discLayers = dLayers ? dLayers : new std::vector< const Trk::DiscLayer* >;
  std::vector<double>::iterator discZposIter = discZpos.begin();
  int discCounter = 0;
                                              
  for ( ; discZposIter != discZpos.end(); ++discZposIter){
       // the gathered R sectors  
       size_t discRsectors = (discPhiSectors[discCounter]).size();
       // dynamic estimation 1: estimate the layer thickness dynamically
       double thickness = discThickness[discCounter]+m_endcapEnvelope;       
       // put rMin in and sort the bins
       std::vector<double> discRingMinRcopy = discRingMinR[discCounter];
       std::sort(discRingMinRcopy.begin(), discRingMinRcopy.end());
       bool reverseRsectors = !(discRingMinRcopy == discRingMinR[discCounter]); 
              
       // this causes a reverse order for the phi sectors
       if (reverseRsectors){
           ATH_MSG_VERBOSE("Auto-detect: rings in R are in " << ( reverseRsectors ? "reversed" : "standard") << " order.");
           std::reverse(discPhiSectors[discCounter].begin(),discPhiSectors[discCounter].end());
           std::reverse(discPhiMin[discCounter].begin(),discPhiMin[discCounter].end()); 
           std::reverse(discPhiMax[discCounter].begin(),discPhiMax[discCounter].end()); 
           std::reverse(discRingMaxR[discCounter].begin(),discRingMaxR[discCounter].end());
       }       
       // preperare the rboundaries
       std::vector<float> discRboundaries;
       discRboundaries.push_back(float(*discRingMinRcopy.begin()));
       for ( std::vector<double>::iterator ringMaxRIter = discRingMaxR[discCounter].begin(); 
            ringMaxRIter != discRingMaxR[discCounter].end(); ++ringMaxRIter)   
           discRboundaries.push_back(float(*ringMaxRIter));

       // screen output           
       ATH_MSG_DEBUG( "Building a DiscLayer with " << discRsectors << " R sectors. " );
       ATH_MSG_DEBUG( "  -> At Z - Position      :  " << discZpos[discCounter] );
       ATH_MSG_DEBUG( "  -> With Thickness       :  " << thickness   << " i- ncludes envelope tolerance : " << m_endcapEnvelope );
       ATH_MSG_DEBUG( "  -> With Rmin/Rmax (est) :  " << discRmin[discCounter] << " / " << discRmax[discCounter] );
       ATH_MSG_DEBUG( "  -> With Rings           :  " << discRsectors );
          
       for (size_t irings=0; irings<discRsectors; ++irings)
         ATH_MSG_DEBUG( " --> " << irings <<  " R sector has " << discPhiSectors[discCounter][irings] << " phi sectors. " );
            
       // prepare the binned array, it can be with one to several rings            
       Trk::BinnedArray<Trk::Surface>* currentBinnedArray = 0;
       std::vector<Trk::BinUtility*>* singleBinUtils = new std::vector<Trk::BinUtility*>;
       bool weOwnSingleBinUtils{true};
       if (discRsectors==1){
            double halfPhiStep = M_PI/discPhiSectors[discCounter][0];
            // protection in case phi value was fluctuating around 0 or M_PI in parsing
            if (fabs(discPhiMin[discCounter][0]+discPhiMax[discCounter][0])< halfPhiStep && fabs(discPhiMin[discCounter][0]) < 0.5*halfPhiStep ){
                ATH_MSG_VERBOSE("Detected module fluctuation around +/- M_PI, correcting for it.");
                ATH_MSG_VERBOSE("    min phi / max phi detected  : "  << discPhiMin[discCounter][0] << " / " <<discPhiMax[discCounter][0] );
                discPhiMin[discCounter][0] += 2*halfPhiStep;
            } 
            // prepare min phi and max phi & eventually a sub stepvalue
            ATH_MSG_VERBOSE("    min phi / max phi detected  : " << discPhiMin[discCounter][0] << " / " << discPhiMax[discCounter][0] );
            double minPhiCorrected = discPhiMin[discCounter][0]-halfPhiStep;
            double maxPhiCorrected = discPhiMax[discCounter][0]+halfPhiStep;
            // catch if the minPhi falls below M_PI
            if (minPhiCorrected < -M_PI){
                minPhiCorrected += 2*halfPhiStep;
                maxPhiCorrected += 2*halfPhiStep;
            }
            ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );
            
            ATH_MSG_VERBOSE("Constructing a one-dimensional BinnedArray with phiMin / phiMax (bins) = " 
                << minPhiCorrected << " / " << maxPhiCorrected
                << " (" << discPhiSectors[discCounter][0] << ")");
            // an easier bin utility can be used                 
            Trk::BinUtility* currentBinUtility = new Trk::BinUtility(discPhiSectors[discCounter][0] ,
                                                                     minPhiCorrected,
                                                                     maxPhiCorrected,
                                                                     Trk::closed,
                                                                     Trk::binPhi);
            // a one-dimensional BinnedArray is sufficient
            currentBinnedArray = new Trk::BinnedArray1D<Trk::Surface>(discSurfaces[discCounter],currentBinUtility);
        } else {
            ATH_MSG_VERBOSE("Constructing a two-dimensional BinnedArray.");
            // get the binning in R first (can still be improved with non-aequidistant binning) 
            Trk::BinUtility* currentSteerBinUtility = 0;
            if (m_endcapComplexRingBinning && discRsectors > 1 ){
                // respecting the actual element boundaires
                ATH_MSG_VERBOSE("Non-equidistant binning detected.");
                // now create the bin utility                                                          
                currentSteerBinUtility = new Trk::BinUtility(discRboundaries,
                                                                    Trk::open,
                                                                    Trk::binR);
             } else 
                currentSteerBinUtility =  new Trk::BinUtility(discRsectors,
                                                              discRmin[discCounter],
                                                              discRmax[discCounter],
                                                              Trk::open,
                                                              Trk::binR);
            ATH_MSG_VERBOSE("Steering bin utility constructed as : " << *currentSteerBinUtility);
            // the single phi bin utilities
            //std::vector<Trk::BinUtility*>* singleBinUtils = new std::vector<Trk::BinUtility*>;
            singleBinUtils->reserve(discRsectors);
            for (size_t irings=0; irings < discRsectors; ++irings){
                    double halfPhiStep = M_PI/discPhiSectors[discCounter][irings];
                    ATH_MSG_VERBOSE("    min phi / max phi detected  : " << discPhiMin[discCounter][irings] << " / " << discPhiMax[discCounter][irings] );
                    double minPhiCorrected = discPhiMin[discCounter][irings]-halfPhiStep;
                    double maxPhiCorrected = discPhiMax[discCounter][irings]+halfPhiStep;
                    // catch if the minPhi falls below M_PI
                    if (minPhiCorrected < -M_PI){
                        minPhiCorrected += 2*halfPhiStep;
                        maxPhiCorrected += 2*halfPhiStep;
                    }
                    ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );
                    ATH_MSG_VERBOSE("Constructing for ring " << irings << " phi utility with phiMin / phiMax (bins) = " 
                        <<  minPhiCorrected << " / " << maxPhiCorrected << " (" << discPhiSectors[discCounter][irings] << ")") ;
                    singleBinUtils->push_back(new Trk::BinUtility(discPhiSectors[discCounter][irings],
                                                                  minPhiCorrected,
                                                                  maxPhiCorrected,
                                                                  Trk::closed,
                                                                  Trk::binPhi));
            }
            // a two-dimensional BinnedArray is needed ; takes possession of singleBinUtils and
            // will delete it on destruction.
            weOwnSingleBinUtils=false;                                                       
            currentBinnedArray = new Trk::BinnedArray1D1D<Trk::Surface>(discSurfaces[discCounter],
                                                                        currentSteerBinUtility,
                                                                        singleBinUtils);
        }


        int discSurfacesNum = (discSurfaces[discCounter]).size();
        ATH_MSG_DEBUG( "Constructed BinnedArray for DiscLayer with "<< discSurfacesNum << " SubSurfaces." );

        // always run the geometry validation to catch flaws
        
        // checking for :
        //   - empty surface bins
        //   - doubly filled bins
        std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
        std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
        // check the registered surfaces in the binned array
        const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects();
        size_t dsumCheckSurfaces = 0;
        double lastPhi = 0.;
        for (auto& asurfIter : arraySurfaces){
            if ( asurfIter ) {
                ++dsumCheckSurfaces;
                usmIter = uniqueSurfaceMap.find(asurfIter);
                lastPhi = asurfIter->center().phi();
                if ( usmIter != uniqueSurfaceMap.end() )
                    ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << asurfIter->center().eta() << " / " << asurfIter->center().phi());
                else uniqueSurfaceMap[asurfIter] = asurfIter->center();
            } else 
                ATH_MSG_WARNING("Zero-pointer in array detected in this ring, last valid phi value was = " << lastPhi);            
        }
        sumCheckEndcapModules +=  dsumCheckSurfaces;   
           
        // force same size for material collection - may be refined later
        double rMin = (m_splitMode || m_endcapRingLayout) ? discRmin[discCounter] : minRmin;
        double rMax = (m_splitMode || m_endcapRingLayout) ? discRmax[discCounter] : maxRmax;
       
        ATH_MSG_DEBUG( "  -> With Rmin/Rmax (corr) :  " << minRmin << " / " << maxRmax );
       
       // get the layer material from the helper method
        const Trk::LayerMaterialProperties* layerMaterial = endcapLayerMaterial(rMin,rMax);

        // position & bounds of the active Layer
        Amg::Transform3D*  activeLayerTransform = new Amg::Transform3D;
        bool weOwnActiveLayerTransform=true;
        (*activeLayerTransform) = Amg::Translation3D(0.,0.,discZpos[discCounter]);
       
        Trk::DiscBounds* activeLayerBounds    = new Trk::DiscBounds(rMin,rMax);
        // prepare the right overlap descriptor       
        Trk::OverlapDescriptor* olDescriptor = 0;
        if (m_pixelCase)
            olDescriptor = new InDet::PixelOverlapDescriptor;
        //else olDescriptor = new  InDet::SCT_OverlapDescriptor;
	else {
	  std::vector<Trk::BinUtility*>* binUtils = new std::vector<Trk::BinUtility*>;
	  if (singleBinUtils) {
	    std::vector<Trk::BinUtility*>::iterator binIter = singleBinUtils->begin();
	    for ( ; binIter != singleBinUtils->end(); ++binIter){
	      binUtils->push_back((*binIter)->clone());
	    }
	  }
	  //DiscOverlapDescriptor takes possession of binUtils, will delete it on destruction.
	  // but *does not* manage currentBinnedArray.
	  olDescriptor = new  InDet::DiscOverlapDescriptor(currentBinnedArray, binUtils);
	}
        
        // layer creation; deletes currentBinnedArray in baseclass 'Layer' upon destruction
        // activeLayerTransform deleted in 'Surface' baseclass
        Trk::DiscLayer* activeLayer = new Trk::DiscLayer(activeLayerTransform,
                                                         activeLayerBounds,
                                                         currentBinnedArray,
                                                         *layerMaterial,
                                                         thickness,
                                                         olDescriptor);
        weOwnActiveLayerTransform=false;
        // cleanup
        delete layerMaterial;
        // register the layer to the surfaces --- if necessary to the other sie as well
        const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
        registerSurfacesToLayer(layerSurfaces,*activeLayer);
        if (m_splitMode){
            ATH_MSG_DEBUG( "[ Split mode ] Checking if this layer needs to be cached." );
            if (m_splitMode < 0 && rMin > s_splitRadius){
                ATH_MSG_VERBOSE( "            Split mode is negative and rMin > splitRadius (" << rMin  << " > " << s_splitRadius << ").");
                ATH_MSG_VERBOSE( "            -> Caching this disk.");
                readHandle->range(s_splitIOVRange);
                s_splitDiscLayers.push_back(activeLayer);
            }
            else if (m_splitMode > 0 && rMax < s_splitRadius){
                ATH_MSG_VERBOSE( "            Split mode is positive and rMax < splitRadius (" << rMax  << " < " << s_splitRadius << ").");
                ATH_MSG_VERBOSE( "            -> Caching this disk.");
                readHandle->range(s_splitIOVRange);
                s_splitDiscLayers.push_back(activeLayer);
            }
        } else 
            discLayers->push_back(activeLayer);
       // increase the disc counter by one
       ++discCounter;
       if (weOwnSingleBinUtils){
         delete singleBinUtils;
         singleBinUtils=nullptr;
       }
       if (weOwnActiveLayerTransform){
        delete activeLayerTransform;
        activeLayerTransform=nullptr;
       }
  }  

  // multiply the check modules for SCT case
  sumCheckEndcapModules *= (m_pixelCase) ? 1 : 2;        
  ATH_MSG_DEBUG( endcapModules << " Endcap Modules parsed for Disc Layer dimensions." );
  ATH_MSG_DEBUG( sumCheckEndcapModules << " Endcap Modules filled in Disc Layer Arrays." );
  if ( endcapModules-sumCheckEndcapModules )
     ATH_MSG_WARNING( endcapModules-sumCheckEndcapModules << " Modules not registered properly in binned array." );       

 
  // sort the vector
  Trk::DiscLayerSorterZ zSorter;
  std::vector<const Trk::DiscLayer*>::iterator sortIter = discLayers->begin();
  std::vector<const Trk::DiscLayer*>::iterator sortEnd   = discLayers->end(); 
  std::sort(sortIter, sortEnd, zSorter);
 
  // if there are additional layers to be built - never build for the DBM loop
  if (m_endcapAdditionalLayerPosZ.size() && !isDBM){
      // sort also the additional layer z positions
      auto addLayerIter     = m_endcapAdditionalLayerPosZ.begin();
      auto addLayerIterEnd  = m_endcapAdditionalLayerPosZ.end();
      auto addLayerTypeIter = m_endcapAdditionalLayerType.begin();
      // reassign the iterators
      sortIter = discLayers->begin();
      sortEnd   = discLayers->end();
      // get the last rmin / rmax
      double lastRmin = 0.;
      double lastRmax = 0.;
      // build the additional layers -------------------------------------------
      for ( ; sortIter != sortEnd || addLayerIter != addLayerIterEnd; ){
          // cache befor last parameters are overwritten 
          double layerRmin       = lastRmin;
          double layerRmax       = lastRmax;
          double layerZposition   = 0.; 
          // check if the z-position is smaller than the 
          if ( sortIter != sortEnd){
              // get the current z position to guarantee a symmetrical setup
              layerZposition = (*sortIter)->surfaceRepresentation().center().z();
              // get the bounds for the rMin / rMax setting
              const Trk::DiscBounds* currentBounds = dynamic_cast<const Trk::DiscBounds*>(&((*sortIter)->surfaceRepresentation().bounds()));
              lastRmin = currentBounds ? currentBounds->rMin() : 0.;
              lastRmax = currentBounds ? currentBounds->rMax() : 0.;
              ++sortIter;           
          }
          if ( addLayerIter != addLayerIterEnd){
              // symmetric setup around 0.
              double rMin = layerZposition > 0. ? layerRmin : lastRmin;
              double rMax = layerZposition > 0. ? layerRmax : lastRmax;
              // the passive layer
              Trk::DiscLayer* passiveLayer = 0;
              // passive layer creation
              Amg::Transform3D* passiveDiscTransf = new Amg::Transform3D;
              (*passiveDiscTransf) = Amg::Translation3D(0.,0.,*addLayerIter);
              if (*addLayerTypeIter){
                  ATH_MSG_DEBUG("Building an additional DiscLayer w/o sensitive modules at");
                  // create the material and the passive layer
                  const Trk::LayerMaterialProperties* passiveLayerMaterial = endcapLayerMaterial(rMin,rMax);
                  passiveLayer = new Trk::DiscLayer(passiveDiscTransf,
                                                    new Trk::DiscBounds(rMin,rMax),
                                                    *passiveLayerMaterial,
                                                    1.*Gaudi::Units::mm);
              } else
                  passiveLayer = new Trk::DiscLayer(passiveDiscTransf, new Trk::DiscBounds(rMin,rMax), 0);
              ATH_MSG_DEBUG( "  -> At Z - Position       :  " << *addLayerIter );
              ATH_MSG_DEBUG( "  -> With Rmin/Rmax (corr) :  " << rMin << " / " << rMax );
              
              // increase the iterator and push back the new layer
             ++addLayerIter; 
             discLayers->push_back(passiveLayer);
          }
      } // the additional layers are build ------------------------------------

    // another round of sorting needed after adding the passive layers
    sortIter = discLayers->begin();
    sortEnd   = discLayers->end(); 
    std::sort(sortIter, sortEnd, zSorter);
  }
 
  EventIDRange range;
  readHandle->range(range);
  delete readHandle;
  return std::make_pair(range, discLayers);
}



std::vector< const Trk::CylinderLayer* >* InDet::SiLayerBuilderCond::dressCylinderLayers(const std::vector< const Trk::CylinderLayer* >& detectionLayers ) const {


    std::vector< const Trk::CylinderLayer* >* cylinderLayers = new std::vector< const Trk::CylinderLayer* >;
    // --------------------------- start of additional layer construction loop -------------------------------
    // for the additional layer
    if (m_barrelAdditionalLayerR.size()){
        auto cylLayerIter         = detectionLayers.begin();
        auto cylLayerIterEnd      = detectionLayers.end();
        auto addLayerIter         = m_barrelAdditionalLayerR.begin();
        auto addLayerIterEnd      = m_barrelAdditionalLayerR.end();
        auto addLayerTypeIter     = m_barrelAdditionalLayerType.begin();
        auto addLayerTypeIterEnd  = m_barrelAdditionalLayerType.end();
        double cylLayerExtend     = 0;
        for ( ; addLayerIter != addLayerIterEnd && addLayerTypeIter != addLayerTypeIterEnd; ) {
            // build the passive layer if it is smaller the current cylLayerIter - or if it is the last one 
            if ( m_splitMode && s_splitCylinderLayers.size() ){
                ATH_MSG_DEBUG("Called in split mode with split radius = " << s_splitRadius );
                ATH_MSG_DEBUG("[- X -] Skipping additional layer " );
                ATH_MSG_DEBUG( "  -> With Radius     :  " << *addLayerIter   );       
                // increase the additional layer radii
                ++addLayerIter; ++addLayerTypeIter; 
                continue;
            }
            if  ( cylLayerIter == cylLayerIterEnd || (*addLayerIter) < (*cylLayerIter)->bounds().r() ){
              cylLayerExtend = (cylLayerIter == cylLayerIterEnd) ? cylLayerExtend : (*cylLayerIter)->bounds().halflengthZ() ;  
              if ( (*addLayerTypeIter) ) {
                    ATH_MSG_DEBUG("[- M -] Building an additional CylinderLayer w/o sensitive modules");
                    // the material for the passive layer
                    const Trk::LayerMaterialProperties* passiveLayerMaterial = barrelLayerMaterial(*addLayerIter,cylLayerExtend);
                    // create the passive layer
                    cylinderLayers->push_back(new Trk::CylinderLayer(new Trk::CylinderBounds(*addLayerIter,cylLayerExtend),
                                                                     *passiveLayerMaterial,
                                                                     1.*Gaudi::Units::mm,
                                                                     0,0));
                    // cleanup of the layer material --------------------------------------------------------------
                    delete passiveLayerMaterial;                                                       
              } else {
                  ATH_MSG_DEBUG("[- N -] Building an additional NavigationLayer for volume dimension control");
                  // create the passive layer
                  cylinderLayers->push_back(new Trk::CylinderLayer(new Trk::CylinderBounds(*addLayerIter,cylLayerExtend),0));
              }
              ATH_MSG_DEBUG( "  -> With Radius     :  " << *addLayerIter   );       
              // increase the additional layer radii
              ++addLayerIter; ++addLayerTypeIter; 
              continue;
          } 
          ATH_MSG_DEBUG("[- D -] Registering detection CylinderLayer");
          ATH_MSG_DEBUG( "  -> With Radius     :  " << (*cylLayerIter)->bounds().r()   );
          cylinderLayers->push_back(*cylLayerIter);
          ++cylLayerIter;       
        }
    } else 
        for (auto& cylLayerIter : detectionLayers ) {
            ATH_MSG_DEBUG("[- D -] Registering detection CylinderLayer");
            ATH_MSG_DEBUG( "  -> With Radius     :  " << cylLayerIter->bounds().r()   );
            cylinderLayers->push_back(cylLayerIter);
        }
    return cylinderLayers;
}

const Trk::LayerMaterialProperties* InDet::SiLayerBuilderCond::barrelLayerMaterial(double r, double hz) const
{
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------
  // -- material with 1D binning
  Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -hz, hz, Trk::open, Trk::binZ);
  if (m_barrelLayerBinsPhi==1){
      layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityZ);
  } else  { // -- material with 2D binning
      Trk::BinUtility layerBinUtilityRPhiZ(m_barrelLayerBinsPhi,
                                                 -r*M_PI, r*M_PI,
                                                 Trk::closed,
                                                 Trk::binRPhi);
      layerBinUtilityRPhiZ += layerBinUtilityZ;                                                       
      layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);
  }
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;    
}

const Trk::LayerMaterialProperties* InDet::SiLayerBuilderCond::endcapLayerMaterial(double rMin, double rMax) const
{
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------

  Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,rMin,rMax,Trk::open, Trk::binR);
  // -- material with 1D binning
  if (m_endcapLayerBinsPhi==1){
      layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } else { // -- material with 2D binning
      Trk::BinUtility layerBinUtilityPhi(m_endcapLayerBinsPhi,-M_PI,M_PI,Trk::closed,Trk::binPhi);
      layerBinUtilityR += layerBinUtilityPhi;
      layerMaterial     = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  }
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;    
}     

void InDet::SiLayerBuilderCond::registerSurfacesToLayer(const std::vector<const Trk::Surface*>& layerSurfaces, const Trk::Layer& lay) const
{
    if (!m_setLayerAssociation) return;
    
    std::vector<const Trk::Surface*>::const_iterator laySurfIter    = layerSurfaces.begin();
    std::vector<const Trk::Surface*>::const_iterator laySurfIterEnd = layerSurfaces.end();
    // register the surfaces to the layer
    for (; laySurfIter != laySurfIterEnd; ++laySurfIter){
        if (*laySurfIter) { 
            // register the current surface --------------------------------------------------------
            Trk::ILayerBuilderCond::associateLayer(lay, **laySurfIter);
            const InDetDD::SiDetectorElement* detElement 
                = dynamic_cast<const InDetDD::SiDetectorElement*>((*laySurfIter)->associatedDetectorElement());             
            // register the backise if necessary ---------------------------------------------------
            const InDetDD::SiDetectorElement* otherSideElement = detElement ?  detElement->otherSide() : 0;                 
            const Trk::Surface* otherSideSurface = otherSideElement ? &(otherSideElement->surface()) : 0;
            if (otherSideSurface) Trk::ILayerBuilderCond::associateLayer(lay, *otherSideSurface);
            }
    }   
    
    return;
}


