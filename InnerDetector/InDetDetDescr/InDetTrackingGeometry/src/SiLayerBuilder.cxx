/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiLayerBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometry/SiLayerBuilder.h"
#include "InDetTrackingGeometry/PixelOverlapDescriptor.h"
#include "InDetTrackingGeometry/SCT_OverlapDescriptor.h"
//InDet include
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
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
// StoreGate
#include "StoreGate/StoreGateSvc.h"
// STL
#include <map>

std::vector<const Trk::CylinderLayer*> InDet::SiLayerBuilder::s_splitCylinderLayers;
std::vector<const Trk::DiscLayer*>     InDet::SiLayerBuilder::s_splitDiscLayers;

// constructor
InDet::SiLayerBuilder::SiLayerBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixelCase(true),
  m_siMgr(0),
  m_siMgrLocation("Pixel"),
  m_pixIdHelper(0),
  m_sctIdHelper(0),
  m_setLayerAssociation(true),
  m_barrelLayerBinsZ(100),
  m_barrelLayerBinsPhi(1),
  m_barrelOverlapLocCheck(true),
  m_barrelOverlapFractionLoc1(0.1),   
  m_barrelOverlapFractionLoc2(0.1),
  m_barrelEdbTolerance(0.01),
  m_endcapLayerBinsR(100),
  m_endcapLayerBinsPhi(1),
  m_endcapComplexRingBinning(true),
  m_endcapOverlapLocCheck(true),
  m_endcapOverlapFractionLoc1(0.1),   
  m_endcapOverlapFractionLoc2(0.1), 
  m_identification("Pixel"),
  m_splitMode(false),
  m_splitRadius(0.),
  m_splitTolerance(10.),
  m_customMaterial(false),
  m_customMaterialThickness(0.),
  m_customMaterialX0(0.),       
  m_customMaterialL0(0.),       
  m_customMaterialA(0.),      
  m_customMaterialZ(0.),       
  m_customMaterialRho(0.),      
  m_runGeometryValidation(true),
  m_robustOverlaps(true)
{
  declareInterface<Trk::ILayerBuilder>(this);
  // general steering
  declareProperty("PixelCase"                        , m_pixelCase);
  declareProperty("SiDetManagerLocation"             , m_siMgrLocation);
  declareProperty("SetLayerAssociation"              , m_setLayerAssociation);
  // For the Active Barrel Material
  declareProperty("BarrelAdditionalLayerRadii"       , m_barrelAdditionalLayerR);
  declareProperty("BarrelLayerBinsZ"                 , m_barrelLayerBinsZ);
  declareProperty("BarrelLayerBinsPhi"               , m_barrelLayerBinsPhi);
  declareProperty("BarrelEdbTolerance"               , m_barrelEdbTolerance);
  // For the Active Endcap Material
  declareProperty("EndcapAdditionalLayerPositionsZ"  , m_endcapAdditionalLayerPosZ);
  declareProperty("EndcapLayerBinsR"                 , m_endcapLayerBinsR);
  declareProperty("EndcapLayerBinsPhi"               , m_endcapLayerBinsPhi);
  declareProperty("EndcapComplexRingBinning"         , m_endcapComplexRingBinning);
  // identification
  declareProperty("Identification"                   , m_identification);
  // split mode for multiple pixel systems (upgrade)
  declareProperty("SplitMode"                        , m_splitMode);
  declareProperty("SplitRadius"                      , m_splitRadius);
  declareProperty("SplitTolerance"                   , m_splitTolerance);
  // Assign custom material
  declareProperty("CustomMaterialThickness"          , m_customMaterialThickness);
  declareProperty("CustomMaterialX0"                 , m_customMaterialX0);       
  declareProperty("CustomMaterialL0"                 , m_customMaterialL0);       
  declareProperty("CustomMaterialA"                  , m_customMaterialA);      
  declareProperty("CustomMaterialZ"                  , m_customMaterialZ);       
  declareProperty("CustomMaterialRho"                , m_customMaterialRho);        
  // Validation
  declareProperty("GeometryValidation"               , m_runGeometryValidation);
}

// destructor
InDet::SiLayerBuilder::~SiLayerBuilder()
{}

// Athena standard methods
// initialize
StatusCode InDet::SiLayerBuilder::initialize()
{

    ATH_MSG_INFO( "initialize()" );
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
    }
    return StatusCode::SUCCESS;
}

// finalize
StatusCode InDet::SiLayerBuilder::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}


/** LayerBuilder interface method - returning Barrel-like layers */
const std::vector< const Trk::CylinderLayer* >* InDet::SiLayerBuilder::cylindricalLayers() const
{

  // split mode 2nd part return the already built layers 
  if (m_splitMode && s_splitCylinderLayers.size() ){
    ATH_MSG_DEBUG( "[ Split mode/ Part 2 ] Returning " << s_splitCylinderLayers.size() << " cylinder layers." );
    std::vector<const Trk::CylinderLayer*>* splitCylinderLayers = 
      new std::vector<const Trk::CylinderLayer*>(s_splitCylinderLayers);
    s_splitCylinderLayers.clear();
    return splitCylinderLayers; 
  }
  // sanity check for ID Helper
  if (!m_pixIdHelper && !m_sctIdHelper){
       ATH_MSG_ERROR("Neither Pixel nor SCT Detector Manager or ID Helper could be retrieved - giving up.");
       return 0;                  
  }

  // take the numerlogoy
  const InDetDD::SiNumerology& siNumerology = m_siMgr->numerology();     
    
  // pre-set parameters for the run
  size_t m_barrelLayers = 0;
  // save way to estimate the number of barrel layers : they can be deactivated hence the useLayer(i) check
  for (int i = 0; i < siNumerology.numLayers(); i++)
       if (siNumerology.useLayer(i)) m_barrelLayers++;
  

  // screen output
  ATH_MSG_DEBUG( "Configured to build " << m_barrelLayers << " (active) barrel layers (out of " << siNumerology.numLayers() << " )" );
  if (m_barrelAdditionalLayerR.size())
      ATH_MSG_DEBUG( "Additionally " <<  m_barrelAdditionalLayerR.size() << " material layers will be built.");  
      
  // split mode for SLHC setup
  if (m_splitMode)
      ATH_MSG_DEBUG( "[ Split mode ] Some layers may bee cached." );
        
  // for barrels (the statistics for ordering the modules)
  std::vector<double>                                               layerRadius(m_barrelLayers,0.);
  std::vector<double>                                               layerRmin(m_barrelLayers,10e10);
  std::vector<double>                                               layerRmax(m_barrelLayers,0.);
  std::vector<double>                                               layerThickness(m_barrelLayers,0.);
  std::vector<double>                                               layerMinZ(m_barrelLayers,0.);
  std::vector<double>                                               layerMaxZ(m_barrelLayers,0.);
  std::vector<double>                                               layerHalfLength(m_barrelLayers,0.);
  std::vector<double>                                               layerMinPhi(m_barrelLayers,0.);
  std::vector<double>                                               layerMaxPhi(m_barrelLayers,0.);
  std::vector<size_t>                                               layerPhiSectors(m_barrelLayers,0);
  std::vector<size_t>                                               layerZsectors(m_barrelLayers,0);
  std::vector< std::vector<float> >                                 layerZboundaries(m_barrelLayers, std::vector<float>());
  std::vector< std::vector< Trk::SurfaceOrderPosition > >           layerSurfaces(m_barrelLayers, std::vector< Trk::SurfaceOrderPosition >());
 
  // cache needed
  double minHalflengthZ         = 10e10;
  size_t sumCheckBarrelModules  = 0;
  size_t barrelModules          = 0;
 
  // [-A-] ------------------------ LOOP over Detector Elements of sensitive layers -----------------------------------
  // iterate over the detector elements for layer dimension, etc.   

  std::map< std::pair< const unsigned int, const unsigned int >, double > layerMinPhiMap;
  std::map< std::pair< const unsigned int, const unsigned int >, double > layerMaxPhiMap;

  InDetDD::SiDetectorElementCollection::const_iterator sidetIter = m_siMgr->getDetectorElementBegin();    
  for (; sidetIter != m_siMgr->getDetectorElementEnd(); sidetIter++){
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
               minHalflengthZ = layerHalfLength[currentlayer] < minHalflengthZ ? 
                                layerHalfLength[currentlayer] : minHalflengthZ;
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
               currentRmax = currentRmax > (otherSide)->rMax() ? currentRmax : (otherSide)->rMax();
               currentRmin = currentRmin < (otherSide)->rMin() ? currentRmin : (otherSide)->rMin();
           }
           layerRmax[currentlayer] = currentRmax > layerRmax[currentlayer] ? currentRmax : layerRmax[currentlayer];
           layerRmin[currentlayer] = currentRmin < layerRmin[currentlayer] ? currentRmin : layerRmin[currentlayer];

           if (m_splitMode){
              // set the split radius to be at the maximum first
              m_splitRadius = layerRadius[currentlayer] > m_splitRadius ? layerRadius[currentlayer] : m_splitRadius;
              ATH_MSG_DEBUG("[ Split mode / part 1 ] Split radius (temproarily) set to : " << m_splitRadius );
           }
  
           // fill the Surface vector
           Amg::Vector3D orderPosition((*sidetIter)->center());

	   // Collect phi mimax values in case of pixel layer ( bug fix related to IBL layer and Blayer reconstruction)
	   //   layerMinPhi/layerMaxPhi parameters collected do not allow to build the reco geometry properly because
	   //   of phi angle values located next to PI and -PI for a same stave  ( IBL and Blayer are both impacted)
	   if(m_pixelCase){

	     int phiIndex = m_pixIdHelper->phi_module(currentId);
	     const Amg::Transform3D transform = (*sidetIter)->moduleTransform();
	     const Amg::Transform3D defTransform = (*sidetIter)->defModuleTransform();
	     const Amg::Transform3D localDelta  = defTransform*transform.inverse();
	     Amg::Vector3D orderPosition_init = localDelta*orderPosition;

	     std::pair< const unsigned int,const unsigned int > phiMinMaxIdx = std::make_pair(currentlayer, phiIndex);
	     
	     std::map< std::pair< const unsigned int, const unsigned int >, double >::iterator layerMinMaxPhi_it;
	     layerMinMaxPhi_it = layerMinPhiMap.find(phiMinMaxIdx);
	     
	     if(layerMinMaxPhi_it == layerMinPhiMap.end()){
	       layerMinPhiMap[phiMinMaxIdx] = orderPosition_init.phi();
	       layerMaxPhiMap[phiMinMaxIdx] = orderPosition_init.phi();
	     }
	     else{
	       if(orderPosition_init.phi()<layerMinPhiMap[phiMinMaxIdx]) layerMinPhiMap[phiMinMaxIdx] = orderPosition_init.phi();
	       if(orderPosition_init.phi()>layerMaxPhiMap[phiMinMaxIdx]) layerMaxPhiMap[phiMinMaxIdx] = orderPosition_init.phi();
	     }
	   }
	   
           layerMinPhi[currentlayer] = (orderPosition.phi() < layerMinPhi[currentlayer]) ? 
                                       orderPosition.phi() : layerMinPhi[currentlayer];
           layerMaxPhi[currentlayer] = (orderPosition.phi() > layerMaxPhi[currentlayer]) ?
                                        orderPosition.phi() : layerMaxPhi[currentlayer];
           
           // decide which one to register on the Radius: always the one with smaller radius
           const Trk::Surface* moduleSurface = (!otherSide || (*sidetIter)->center().perp() < otherSide->center().perp() )?
                                      (&((*sidetIter)->surface())) : (&(otherSide->surface()));
  
           // register the module surface
           Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface, true);
  
           Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
           (layerSurfaces[currentlayer]).push_back(surfaceOrder);
     
     } else if (!(*sidetIter)) // barrel chek and screen output
        ATH_MSG_WARNING("NULL pointer to Barrel module given by SiDetectorManager! Please check db & dict.xml");
  
  } // SiDet Loop


  // Overwrite layerMinPhi and layerMaxPhi values for pixel barrel layers only 
  //    because of issues due to module phi angles next to PI and -PI for a same stave 
  if(m_pixelCase){
  
    for(unsigned int ilayer=0; ilayer<m_barrelLayers; ilayer++){
      
      double phiMin=0., phiMax=0.;
      for(unsigned int iphi=0; iphi<layerPhiSectors[ilayer];iphi++){
	std::pair< const unsigned int,const unsigned int > phiMinMaxIdx = std::make_pair(ilayer, iphi);
	double v=0;
	if(layerMinPhiMap[phiMinMaxIdx]<-M_PI*0.5&&layerMaxPhiMap[phiMinMaxIdx]>M_PI*0.5){
	  v =  (layerMinPhiMap[phiMinMaxIdx]+2*M_PI+layerMaxPhiMap[phiMinMaxIdx])*0.5;
	  if(v>M_PI)v-=2.*M_PI;
	}
	else
	  v =  (layerMinPhiMap[phiMinMaxIdx]+layerMaxPhiMap[phiMinMaxIdx])*0.5;
	if(v<phiMin) phiMin=v;
	if(v>phiMax) phiMax=v;
      }
      
      layerMinPhi[ilayer] = phiMin;
      layerMaxPhi[ilayer] = phiMax;
    }

  }

  // --------------------------- enf of LOOP over Detector Elements of sensitive layers ----------------------------------
   
  // [-B-] ------------------------ Construction of the layers -----------------------------------          
  // construct the layers
  std::vector< const Trk::CylinderLayer* >* cylinderLayers = new std::vector< const Trk::CylinderLayer* >;
  std::vector<double>::iterator layerRadiusIter = layerRadius.begin();     
  int layerCounter          = 0;
  double currentLayerExtend = 0.;
     
  // for the additional layer
  std::vector<double>::const_iterator addLayerIter    = m_barrelAdditionalLayerR.begin();
  std::vector<double>::const_iterator addLayerIterEnd = m_barrelAdditionalLayerR.end();
  
  // construct support - detecting - support - detecting - support - detecting layers 
  for ( ; layerRadiusIter != layerRadius.end() || addLayerIter != m_barrelAdditionalLayerR.end(); ){
                  
      Trk::CylinderLayer* activeLayer   = 0;
      double currentLayerRadius         = 10e10;
      bool splitDone                    = false;
      // non-equidistant binning used ? auto-detection 
      bool nonEquidistantBinning = false;
      {
          // calculate the average bin size
          double averageBinSize = (layerMaxZ[layerCounter]-layerMinZ[layerCounter])/(layerZsectors[layerCounter]);
          // loop over the boundaries and check if theyare outside the tolerance
          std::vector<float>::iterator bIter  = layerZboundaries[layerCounter].begin();
          std::vector<float>::iterator bIterE = layerZboundaries[layerCounter].end();
          for ( ++bIter; bIter != bIterE; ++bIter ){
              float cZ = (*bIter);
              float pZ = (*(bIter-1));
              nonEquidistantBinning =  (cZ-pZ)*(cZ-pZ)/(averageBinSize*averageBinSize) < (1.-m_barrelEdbTolerance) *(1.-m_barrelEdbTolerance);
              if (nonEquidistantBinning){
                  ATH_MSG_VERBOSE("Non-equidistant binning for (Silicon) Surfaces on this layer detected. ");
                  ATH_MSG_VERBOSE("Difference " << (cZ-pZ)/averageBinSize << " at a allowed tolerance of : " << m_barrelEdbTolerance );
                  break;
              }
          }
      }
      
      //(1) the detecting layer : sensitive modules --------------------------------------------------------------------
      if (layerRadiusIter != layerRadius.end()){
          // create the BinKeyUtility - the phi binning is identical
          double halfPhiStep = (M_PI)/layerPhiSectors[layerCounter];
          Trk::BinUtility* currentBinUtility   =  new Trk::BinUtility(layerPhiSectors[layerCounter],
								      (*layerRadiusIter)*(layerMinPhi[layerCounter]-halfPhiStep),
								      (*layerRadiusIter)*(layerMaxPhi[layerCounter]+halfPhiStep),
								      // (*layerRadiusIter)*(layerMinPhi[layerCounter])-halfPhiStep,
								      // (*layerRadiusIter)*(layerMaxPhi[layerCounter])+halfPhiStep,
								      Trk::closed,
                                                                                    Trk::binRPhi);
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
            std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
            // ------- iterate  
            std::vector<const Trk::Surface*>::const_iterator asurfIter = arraySurfaces.begin();
            std::vector<const Trk::Surface*>::const_iterator asurfIterEnd = arraySurfaces.end();
            for ( ; asurfIter != asurfIterEnd; ++asurfIter){
                if ( (*asurfIter) ) {
                   ++sumCheckBarrelModules;
                   usmIter = uniqueSurfaceMap.find(*asurfIter);
                   if ( usmIter != uniqueSurfaceMap.end() )
                       ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
                   else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
		}
		else{
		  ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector");
		}
            }
         }

         // dynamic layer extend and splitDone check (split mode need the dynamic extend !)
         currentLayerExtend = layerHalfLength[layerCounter];
         
         // check if split has been done
         bool splitDone = m_splitMode && fabs(layerHalfLength[layerCounter]-minHalflengthZ) > m_splitTolerance;
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
         // screen output
         ATH_MSG_DEBUG( "Construct BinnedArray for CylinderLayer with "<< (layerSurfaces[layerCounter]).size() << " SubSurfaces." );
         ATH_MSG_DEBUG( "Building a CylinderLayer with " << layerPhiSectors[layerCounter]
             << " / " <<   ( nonEquidistantBinning ? layerZboundaries[layerCounter].size() :  layerZsectors[layerCounter] ) << " phi/z bins. "   );
         ATH_MSG_DEBUG( "  -> With Radius     :  " << layerRadius[layerCounter]    );       
         ATH_MSG_DEBUG( "  -> With Thickness  :  " << layerThickness[layerCounter] );
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
             msg(MSG::DEBUG) << endreq;           
         }
         // prepare the right overlap descriptor       
         Trk::OverlapDescriptor* olDescriptor = 0;
         if (m_pixIdHelper)
	           olDescriptor = new InDet::PixelOverlapDescriptor(m_endcapOverlapFractionLoc1,
	                                                            m_endcapOverlapFractionLoc2,
	                                                            m_endcapOverlapLocCheck,
	                                                            currentBinnedArray,
	                                                            m_pixIdHelper,
	                                                            m_siMgr,
	                                                            m_robustOverlaps);
	         else
	           olDescriptor = new InDet::SCT_OverlapDescriptor(m_endcapOverlapFractionLoc1,
	                                                         m_endcapOverlapFractionLoc2,
	                                                         m_endcapOverlapLocCheck,
	                                                         currentBinnedArray,
	                                                         m_sctIdHelper,
	                                                         m_siMgr,
	                                                         m_robustOverlaps);
         
         // for eventual use with the passive layer
         currentLayerRadius = layerRadius[layerCounter];

         //@HACK to enable IBL reconstruction with senseful output
         // give explicit material to IBL - only in this branch tag
         if (currentLayerRadius < 40.){
             delete layerMaterial;
             Trk::MaterialProperties mat(1.,100.,33.,12.,6.,0.03);
             layerMaterial = new Trk::HomogeneousLayerMaterial(mat, 1.0);
         }

         // construct the layer (finally)
         activeLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(layerRadius[layerCounter],currentLayerExtend),
                                              currentBinnedArray,
                                              *layerMaterial,
                                              layerThickness[layerCounter],
                                              olDescriptor);
         // increase the iterator
         ++layerRadiusIter;                                                        
         // cleanup of the layer material --------------------------------------------------------------
         delete layerMaterial; 
         // register the layer to the surfaces
         const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
         registerSurfacesToLayer(layerSurfaces,*activeLayer);

      }     
        
      //(2) the passive layers : --- given by jobOptions ------------------------------------------------------------------
      bool buildPassiveLayer = m_barrelAdditionalLayerR.size() && (addLayerIter != addLayerIterEnd);
      Trk::CylinderLayer* passiveLayer = 0;
      // check if an additional layer should be built --------------------------------------------              
      if (buildPassiveLayer && (*addLayerIter) < currentLayerRadius){
        // reset the current layer radius
        currentLayerRadius =  *addLayerIter;                                   
       
        ATH_MSG_DEBUG("Building an additional CylinderLayer w/o sensitive modules at");
        ATH_MSG_DEBUG( "  -> With Radius     :  " << currentLayerRadius   );       
        ATH_MSG_DEBUG( "  -> With Zmin/Zmax  :  " << -currentLayerExtend << " / " << currentLayerExtend );
        // the material for the passive layer
        const Trk::LayerMaterialProperties* passiveLayerMaterial = barrelLayerMaterial(currentLayerRadius,currentLayerExtend);
        // create the passive layer
        passiveLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(*addLayerIter,currentLayerExtend),
                                              *passiveLayerMaterial,
                                              1.*Gaudi::Units::mm,
                                              0,0);
        // increase the iterator
        ++addLayerIter;
        // cleanup of the layer material --------------------------------------------------------------
        delete passiveLayerMaterial;                                                         
      }

      // (3) register the layers --- either in the split vector or in the return vector 
      if (splitDone) {
          ATH_MSG_DEBUG( "[ Split mode / Part 1 ] Layer cached for Part 2" );
          if (passiveLayer) s_splitCylinderLayers.push_back(passiveLayer);   
          if (activeLayer) s_splitCylinderLayers.push_back(activeLayer);   
          // get the split radius to the smallest one possible
          m_splitRadius =  currentLayerRadius < m_splitRadius ? currentLayerRadius : m_splitRadius;
          ATH_MSG_DEBUG("[ Split mode / part 1 ] Split radius (temproarily) set to : " << m_splitRadius );
       } else {
          if (passiveLayer) cylinderLayers->push_back(passiveLayer);
          if (activeLayer) cylinderLayers->push_back(activeLayer);
       } 
       // increase the layer counter --- it is built
       ++layerCounter;            

  } // layer construction
  // --------------------------- enf of layer construction loop ----------------------------------

  // multiply the check number in case of SCT 
  sumCheckBarrelModules *= (m_pixelCase) ? 1 : 2; 
  ATH_MSG_DEBUG( barrelModules << " Barrel Modules parsed for Cylinder Layer dimenstions." );
  ATH_MSG_DEBUG( sumCheckBarrelModules << " Barrel Modules filled in Cylinder Layer Arrays." );
  if ( barrelModules-sumCheckBarrelModules )
     ATH_MSG_WARNING( barrelModules-sumCheckBarrelModules << " Modules not registered properly in binned array." );       

  if (m_splitMode)
     ATH_MSG_DEBUG("[ Split mode / part 1 ] Split radius determined as : " << m_splitRadius );

  return cylinderLayers;   
} 
      
/** LayerBuilder interface method - returning Endcap-like layers */
const std::vector< const Trk::DiscLayer* >* InDet::SiLayerBuilder::discLayers() const
{
 
  // split mode 2nd 
  if (m_splitMode && s_splitDiscLayers.size() ){
    ATH_MSG_DEBUG( "[ Split mode/ Part 2 ] Returning " << s_splitDiscLayers.size() << " disc layers." );
    std::vector<const Trk::DiscLayer*>* splitDiscs = 
      new std::vector<const Trk::DiscLayer*>(s_splitDiscLayers);
    s_splitDiscLayers.clear();
    return splitDiscs; 
  }
  // sanity check for ID Helper
  if (!m_pixIdHelper && !m_sctIdHelper){
       ATH_MSG_ERROR("Neither Pixel nor SCT Detector Manager or ID Helper could be retrieved - giving up.");
       return 0;                  
  } 

  // get general layout
  InDetDD::SiDetectorElementCollection::const_iterator sidetIter = m_siMgr->getDetectorElementBegin();
    
  // save way to estimate the number of barrels
  unsigned int endcapLayers = 0;
  for (int i = 0; i < m_siMgr->numerology().numDisks(); i++)
      if (m_siMgr->numerology().useDisk(i)) endcapLayers++;
        
  ATH_MSG_DEBUG( "Configured to build " << endcapLayers << " *2 disc-like layers  (+ additional support layers)." );
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
  sidetIter = m_siMgr->getDetectorElementBegin();  
  for (; sidetIter != m_siMgr->getDetectorElementEnd(); sidetIter++){
     // Endcap
   
     if ((*sidetIter) && (*sidetIter)->isEndcap()){
     
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

        // set the sum of z position
        discZmin[currentlayer] = (currentZ < discZmin[currentlayer]) ? currentZ : discZmin[currentlayer];        
        discZmax[currentlayer] = (currentZ > discZmax[currentlayer]) ? currentZ : discZmax[currentlayer];

        // set the disc Rmin / Rmax  
        double currentRmin = (*sidetIter)->rMin();
        double currentRmax = (*sidetIter)->rMax();

        // how many rings do we have ?
        unsigned int diskRings = m_siMgr->numerology().numRingsForDisk(currentdisk);
               
        // the current phi 
        double currentPhi = (*sidetIter)->center().phi();
        discRmin[currentlayer] = currentRmin < discRmin[currentlayer] ? currentRmin : discRmin[currentlayer];                
        discRmax[currentlayer] = currentRmax > discRmax[currentlayer] ? currentRmax : discRmax[currentlayer];      


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
               unsigned int phiSectorsRing = m_siMgr->numerology().numPhiModulesForDiskRing(currentdisk, iring);
              // get the number of phi sectors
              ATH_MSG_VERBOSE("--> Ring " << iring << " has " << phiSectorsRing << " phi sectors");
              discPhiSectors[currentlayer].push_back(phiSectorsRing);
           }
        }
       // take phi-min and phimax
       discPhiMin[currentlayer][currentring] = currentPhi < discPhiMin[currentlayer][currentring]  ? 
         currentPhi : discPhiMin[currentlayer][currentring];
       discPhiMax[currentlayer][currentring] = currentPhi > discPhiMax[currentlayer][currentring]  ? 
         currentPhi : discPhiMax[currentlayer][currentring];
       // record the smalles ring & biggest ring radius
       discRingMinR[currentlayer][currentring] = currentRmin < discRingMinR[currentlayer][currentring] ? 
          currentRmin : discRingMinR[currentlayer][currentring];
       discRingMaxR[currentlayer][currentring] = currentRmax > discRingMaxR[currentlayer][currentring] ? 
          currentRmax : discRingMaxR[currentlayer][currentring];
     } else if (!(*sidetIter))
        ATH_MSG_WARNING("NULL pointer to Endcap module given by SCT_DetectorManager! Please check db & dict.xml");
  } // DetElement loop 

  double minRmin = 10e10;
  double maxRmax = -10e10;
  
  ATH_MSG_VERBOSE("Estimating the average z position and the radius for each disk.");
  // get the average z-position per layer & estimate teh thickness
  for (unsigned int iec=0; iec<2*endcapLayers; ++iec){
       minRmin = discRmin[iec]  <  minRmin ? discRmin[iec] : minRmin;
       maxRmax = discRmax[iec]  >  maxRmax ? discRmax[iec] : maxRmax;
       // average it out
       discZpos[iec]        = 0.5 * (discZmin[iec] + discZmax[iec]);
       discThickness[iec]   = fabs(discZmax[iec]-discZmin[iec]);
       // make the map z / index
       discZposLayerIndex.insert(std::make_pair(discZpos[iec],iec));
  }

  // [-A2-] ------------------------ second LOOP over Detector Elements of sensitive layers -------------------------------                 
  // fill the elements for the layers into the surface arrays
  sidetIter = m_siMgr->getDetectorElementBegin();
  for ( ; sidetIter != m_siMgr->getDetectorElementEnd(); ++sidetIter){
    // Endcap
    if ( (*sidetIter) && ((*sidetIter)->isEndcap())){     
        // get the identifier & calculate current layer and current disk from it     
        Identifier    currentId((*sidetIter)->identify());
        currentdisk  = m_pixIdHelper ? m_pixIdHelper->layer_disk(currentId) : m_sctIdHelper->layer_disk(currentId);
        currentlayer = currentdisk;
        currentlayer += (*sidetIter)->center().z() > 0. ? endcapLayers : 0;
        // decision which module to take
        const InDetDD::SiDetectorElement* otherSide = (*sidetIter)->otherSide();    
        const InDetDD::SiDetectorElement* chosenSide = (!otherSide || fabs((*sidetIter)->center().z()) < fabs(otherSide->center().z()) ) ?
                (*sidetIter) : otherSide;
        // get the center position
        const Amg::Vector3D& orderPosition = chosenSide->center();
        // register the chosen side in the object array
        Trk::SharedObject<const Trk::Surface> sharedSurface(&(chosenSide->surface()), true);
        Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
        (discSurfaces[currentlayer]).push_back(surfaceOrder);      
    }      
  } //end of filling
    
  // [-B-] ------------------------ Construction of the layers -----------------------------------
  // construct the layers
  std::vector< const Trk::DiscLayer* >* discLayers = new std::vector< const Trk::DiscLayer* >;
  std::vector<double>::iterator discZposIter = discZpos.begin();
  int discCounter = 0;
                                              
  for ( ; discZposIter != discZpos.end(); ++discZposIter){
       // the gathered R sectors  
       size_t discRsectors = (discPhiSectors[discCounter]).size();
       // dynamic estimation 1: estimate the layer thickness dynamically
       double thickness = discThickness[discCounter];       
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
       ATH_MSG_DEBUG( "  -> With Thickness       :  " << thickness             );
       ATH_MSG_DEBUG( "  -> With Rmin/Rmax (est) :  " << discRmin[discCounter] << " / " << discRmax[discCounter] );
       ATH_MSG_DEBUG( "  -> With Rings           :  " << discRsectors );
          
       for (size_t irings=0; irings<discRsectors; ++irings)
         ATH_MSG_DEBUG( " --> " << irings <<  " R sector has " << discPhiSectors[discCounter][irings] << " phi sectors. " );
            
       // prepare the binned array, it can be with one to several rings            
       Trk::BinnedArray<Trk::Surface>* currentBinnedArray = 0;
       if (discRsectors==1){
           double halfPhiStep = M_PI/discPhiSectors[discCounter][0];
            ATH_MSG_VERBOSE("Constructing a one-dimensional BinnedArray with phiMin / phiMax (bins) = " 
                << discPhiMin[discCounter][0]-halfPhiStep << " / " << discPhiMax[discCounter][0]+halfPhiStep 
                << " (" << discPhiSectors[discCounter][0] << ")");
            // an easier bin utility can be used                 
            Trk::BinUtility* currentBinUtility = new Trk::BinUtility(discPhiSectors[discCounter][0] ,
                                                                                   discPhiMin[discCounter][0]-halfPhiStep,
                                                                                   discPhiMax[discCounter][0]+halfPhiStep,
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
            std::vector<Trk::BinUtility*>* singleBinUtils = new std::vector<Trk::BinUtility*>;
            singleBinUtils->reserve(discRsectors);
            for (size_t irings=0; irings < discRsectors; ++irings){
                    double halfPhiStep = M_PI/discPhiSectors[discCounter][irings];
                    ATH_MSG_VERBOSE("Constructing for ring " << irings << " phi utility with phiMin / phiMax (bins) = " 
                        << discPhiMin[discCounter][irings]-halfPhiStep << " / " << discPhiMax[discCounter][irings]+halfPhiStep 
                        << " (" << discPhiSectors[discCounter][irings] << ")") ;
                    singleBinUtils->push_back(new Trk::BinUtility(discPhiSectors[discCounter][irings],
                                                                         discPhiMin[discCounter][irings]-halfPhiStep,
                                                                         discPhiMax[discCounter][irings]+halfPhiStep,
                                                                         Trk::closed,
                                                                         Trk::binPhi));
            }
            // a two-dimensional BinnedArray is needed                                                        
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
        std::vector<const Trk::Surface*>::const_iterator asurfIter = arraySurfaces.begin();
        std::vector<const Trk::Surface*>::const_iterator asurfIterEnd = arraySurfaces.end();
        for ( ; asurfIter != asurfIterEnd; ++asurfIter )
          if ( (*asurfIter) ) {
            ++sumCheckEndcapModules;
            usmIter = uniqueSurfaceMap.find(*asurfIter);
            if ( usmIter != uniqueSurfaceMap.end() )
                ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
            else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
        } 
           
        // force same size for material collection - may be refined later
        double rMin = m_splitMode ? discRmin[discCounter] : minRmin;
        double rMax = m_splitMode ? discRmax[discCounter] : maxRmax;
       
        ATH_MSG_DEBUG( "  -> With Rmin/Rmax (corr) :  " << minRmin << " / " << maxRmax );
       
       // get the layer material from the helper method
        const Trk::LayerMaterialProperties* layerMaterial = endcapLayerMaterial(rMin,rMax);

        // position & bounds of the active Layer
        Amg::Transform3D*  activeLayerTransform = new Amg::Transform3D;
        (*activeLayerTransform) = Amg::Translation3D(0.,0.,discZpos[discCounter]);
       
        Trk::DiscBounds* activeLayerBounds    = new Trk::DiscBounds(rMin,rMax);
        // prepare the right overlap descriptor       
        Trk::OverlapDescriptor* olDescriptor = 0;
        if (m_pixIdHelper)
          olDescriptor = new InDet::PixelOverlapDescriptor(m_endcapOverlapFractionLoc1,
                                                           m_endcapOverlapFractionLoc2,
                                                           m_endcapOverlapLocCheck,
                                                           currentBinnedArray,
                                                           m_pixIdHelper
                                                           ,m_siMgr,
                                                           m_robustOverlaps);
        else
            olDescriptor = new InDet::SCT_OverlapDescriptor(m_endcapOverlapFractionLoc1,
                                                            m_endcapOverlapFractionLoc2,
                                                            m_endcapOverlapLocCheck,
                                                            currentBinnedArray,
                                                            m_sctIdHelper,
                                                            m_siMgr,
                                                            m_robustOverlaps);

        
        // layer creation
        Trk::DiscLayer* activeLayer = new Trk::DiscLayer(activeLayerTransform,
                                                         activeLayerBounds,
                                                         currentBinnedArray,
                                                         *layerMaterial,
                                                         thickness,
                                                         olDescriptor);
        // cleanup
        delete layerMaterial;
        // register the layer to the surfaces --- if necessary to the other sie as well
        const std::vector<const Trk::Surface*>& layerSurfaces     = currentBinnedArray->arrayObjects();
        registerSurfacesToLayer(layerSurfaces,*activeLayer);
        
        discLayers->push_back(activeLayer);
       // increase the disc counter by one
       ++discCounter;     
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
 
  // if there are additional layers to be built
  if (m_endcapAdditionalLayerPosZ.size()){
      // sort also the additional layer z positions
      std::vector<double>::const_iterator addLayerIter    = m_endcapAdditionalLayerPosZ.begin();
      std::vector<double>::const_iterator addLayerIterEnd = m_endcapAdditionalLayerPosZ.end();
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
              // create the material and the passive layer
              const Trk::LayerMaterialProperties* passiveLayerMaterial = endcapLayerMaterial(rMin,rMax);
              // passive layer creation
              Amg::Transform3D* passiveDiscTransf = new Amg::Transform3D;
              (*passiveDiscTransf) = Amg::Translation3D(0.,0.,*addLayerIter);
              Trk::DiscLayer* passiveLayer = new Trk::DiscLayer(passiveDiscTransf,
                                                                new Trk::DiscBounds(rMin,rMax),
                                                                *passiveLayerMaterial,
                                                                1.*Gaudi::Units::mm);
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
 
  return discLayers;
}

const Trk::LayerMaterialProperties* InDet::SiLayerBuilder::barrelLayerMaterial(double r, double hz) const
{
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------
  // -- material with 1D binning
  Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -hz, hz, Trk::open, Trk::binZ);
  if (m_barrelLayerBinsPhi==1){
      layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityZ);
  } else if (!m_customMaterial) { // -- material with 2D binning
      Trk::BinUtility layerBinUtilityRPhiZ(m_barrelLayerBinsPhi,
                                                 -r*M_PI, r*M_PI,
                                                 Trk::closed,
                                                 Trk::binRPhi);
      layerBinUtilityRPhiZ += layerBinUtilityZ;                                                       
      layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);
  } else { // -- homogeneous material 
      Trk::MaterialProperties customMaterial(m_customMaterialThickness,
                                                     m_customMaterialX0,
                                                     m_customMaterialL0,
                                                     m_customMaterialA,
                                                     m_customMaterialZ,
                                                     m_customMaterialRho);
       layerMaterial = new Trk::HomogeneousLayerMaterial(customMaterial, 1.0);         
  }
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;    
}

const Trk::LayerMaterialProperties* InDet::SiLayerBuilder::endcapLayerMaterial(double rMin, double rMax) const
{
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------

  Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,rMin,rMax,Trk::open, Trk::binR);
  // -- material with 1D binning
  if (m_barrelLayerBinsPhi==1){
      layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } else if (!m_customMaterial) { // -- material with 2D binning
      Trk::BinUtility layerBinUtilityPhi(m_barrelLayerBinsPhi,-M_PI,M_PI,Trk::closed,Trk::binPhi);
      layerBinUtilityR += layerBinUtilityPhi;
      layerMaterial     = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } else { // -- homogeneous material 
      Trk::MaterialProperties customMaterial(m_customMaterialThickness,
                                                     m_customMaterialX0,
                                                     m_customMaterialL0,
                                                     m_customMaterialA,
                                                     m_customMaterialZ,
                                                     m_customMaterialRho);
       layerMaterial = new Trk::HomogeneousLayerMaterial(customMaterial, 1.0);         
  }    
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;    
}     

void InDet::SiLayerBuilder::registerSurfacesToLayer(const std::vector<const Trk::Surface*>& layerSurfaces, const Trk::Layer& lay) const
{
    if (!m_setLayerAssociation) return;
    
    std::vector<const Trk::Surface*>::const_iterator laySurfIter    = layerSurfaces.begin();
    std::vector<const Trk::Surface*>::const_iterator laySurfIterEnd = layerSurfaces.end();
    // register the surfaces to the layer
    for (; laySurfIter != laySurfIterEnd; ++laySurfIter){
        if (*laySurfIter) { 
            // register the current surface --------------------------------------------------------
            Trk::ILayerBuilder::associateLayer(lay, **laySurfIter);
            const InDetDD::SiDetectorElement* detElement 
                = dynamic_cast<const InDetDD::SiDetectorElement*>((*laySurfIter)->associatedDetectorElement());             
            // register the backise if necessary ---------------------------------------------------
            const InDetDD::SiDetectorElement* otherSideElement = detElement ?  detElement->otherSide() : 0;                 
            const Trk::Surface* otherSideSurface = otherSideElement ? &(otherSideElement->surface()) : 0;
            if (otherSideSurface) Trk::ILayerBuilder::associateLayer(lay, *otherSideSurface);
            }
    }   
    
    return;
}


