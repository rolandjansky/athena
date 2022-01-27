 
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_LayerBuilderCond.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HGTD_TrackingGeometry/HGTD_LayerBuilderCond.h"
#include "HGTD_TrackingGeometry/HGTD_OverlapDescriptor.h"
//HGTD include
#include "HGTD_ReadoutGeometry/HGTD_DetectorManager.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElement.h"
#include "HGTD_ReadoutGeometry/HGTD_DetectorElementCollection.h"
#include "HGTD_Identifier/HGTD_ID.h"

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
#include "TrkGeometry/DiscLayer.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/DiscBounds.h"
// GeoModel
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
// Athena
#include "AthenaKernel/IOVInfiniteRange.h"
// STL
#include <map>

// constructor
HGTD_LayerBuilderCond::HGTD_LayerBuilderCond(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_hgtdMgr(nullptr),
  m_hgtdHelper(nullptr),
  m_setLayerAssociation(true),
  m_identification("HGTD"),
  m_rBins(50),
  m_phiBins(100),
  m_discEnvelopeR(50.),
  m_discThickness(0.2),
  m_runGeometryValidation(true)
{
  declareInterface<Trk::ILayerBuilderCond>(this);
  // general steering
  declareProperty("SetLayerAssociation"           , m_setLayerAssociation);
  // identification
  declareProperty("Identification"                , m_identification);
  // set some parameters
  declareProperty("DiscMaterialBinsR"             , m_rBins);
  declareProperty("DiscMaterialBinsPhi"           , m_phiBins);
  declareProperty("DiscEnvelope"                  , m_discEnvelopeR);
  declareProperty("DiscThickness"                 , m_discThickness);
  // validation
  declareProperty("RunValidation"                 , m_runGeometryValidation);
}

// destructor
HGTD_LayerBuilderCond::~HGTD_LayerBuilderCond()
{}

// Athena standard methods
// initialize
StatusCode HGTD_LayerBuilderCond::initialize()
{

    ATH_MSG_DEBUG( "initialize()" );
    // get HGTD Detector Description Manager and HGTD Helper
    ATH_CHECK(detStore()->retrieve(m_hgtdMgr, "HGTD"));
    ATH_CHECK(detStore()->retrieve(m_hgtdHelper, "HGTD_ID"));
    
    // get HGTD detector element collection
    ATH_CHECK(m_HGTD_ReadKey.initialize());
    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode HGTD_LayerBuilderCond::finalize()
{
    ATH_MSG_DEBUG( "finalize() successful" );
    return StatusCode::SUCCESS;
}

SG::ReadCondHandle<InDetDD::HGTD_DetectorElementCollection> HGTD_LayerBuilderCond::retrieveHGTDdetElements(const EventContext& ctx) const
{
  auto readHandle = SG::ReadCondHandle<InDetDD::HGTD_DetectorElementCollection> (m_HGTD_ReadKey, ctx);
  if (*readHandle==nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object of " << m_HGTD_ReadKey.key());
  }
  return readHandle;
}


/** LayerBuilder interface method - returning Endcap-like layers */
std::pair<EventIDRange, const std::vector<Trk::DiscLayer*>*>
HGTD_LayerBuilderCond::discLayers(const EventContext& ctx) const
{
  ATH_MSG_DEBUG( "calling HGTD_LayerBuilderCond::discLayers()" );
  
  // sanity check for HGTD Helper
  if (!m_hgtdHelper){
    ATH_MSG_ERROR("HGTD Detector Manager or ID Helper could not be retrieved - giving up.");
    //create dummy infinite range
    EventIDRange range = IOVInfiniteRange::infiniteMixed();
    return std::pair<EventIDRange, const std::vector<Trk::DiscLayer*>*>(range,nullptr);
  } 
  
  // get general layout
  SG::ReadCondHandle<InDetDD::HGTD_DetectorElementCollection> readHandle = retrieveHGTDdetElements(ctx);
  if(*readHandle == nullptr){
    EventIDRange range = IOVInfiniteRange::infiniteMixed();
    return std::pair<EventIDRange, std::vector<Trk::DiscLayer*>*>(range,nullptr);
  }
  const InDetDD::HGTD_DetectorElementCollection* readCdo{*readHandle};
  InDetDD::HGTD_DetectorElementCollection::const_iterator hgtdDetIter = readCdo->begin();
    
  // loop on all modules (selecting only one endcap side)
  // and evaluates the number of discs
  // assuming you have the same number of modules on both sides
  int nlayers = 0;
  for (; hgtdDetIter != readCdo->end(); hgtdDetIter++){
    Identifier    currentId((*hgtdDetIter)->identify());
    // skipping negative side
    if (m_hgtdHelper->endcap(currentId)<0) continue;
    if (m_hgtdHelper->layer(currentId)>nlayers)
      nlayers++;    
  }
  // adding one layer offset
  nlayers+=1;
  
  ATH_MSG_DEBUG( "Configured to build " << nlayers << " *2 disc-like layers  (+ additional support layers)." );
  
  // prepare the vectors
  std::vector<float>                                     discZpos(2*nlayers,0.);
  std::vector< std::vector<Trk::SurfaceOrderPosition> >  discSurfaces(2*nlayers, std::vector<Trk::SurfaceOrderPosition>()); 
  
  int hgtdModules = 0;
  int sumCheckhgtdModules = 0;
  unsigned int currentlayer = 0;
  float maxRmax = -std::numeric_limits<float>::max();
  float minRmin =  std::numeric_limits<float>::max();
  
  // get the missing dimensions by loop over DetElements
  hgtdDetIter = readCdo->begin();
  for (; hgtdDetIter != readCdo->end(); hgtdDetIter++){
  // take it - if 
  // a) you have a detector element ... protection
  if ( (*hgtdDetIter) ) {
     // get the identifier
     Identifier    currentId((*hgtdDetIter)->identify());

     ATH_MSG_DEBUG("Element : " << m_hgtdHelper->endcap(currentId) << "/"
                                << m_hgtdHelper->layer(currentId) << "/"
                                << m_hgtdHelper->eta_module(currentId) << "/"
                                << m_hgtdHelper->phi_module(currentId));

     // increase the counter of HGTD modules
     hgtdModules++;
     
     // parse all z positions for the mean value of the discs
     float currentZ = (*hgtdDetIter)->center().z();
     //calculate current layer and current disk from it     
     currentlayer  = m_hgtdHelper->layer(currentId);
     // adding the numbe of layers per side as offset
     currentlayer += currentZ > 0. ? nlayers : 0;
     ATH_MSG_DEBUG( " ----  layer: " << currentlayer );
     
     // evaluate the z-position per layer
     // all modules on the same HGTD layer have the same z
     discZpos[currentlayer] = currentZ;     

     // evaluate the r-extension per layer
     float currentRmin = (*hgtdDetIter)->rMin();
     float currentRmax = (*hgtdDetIter)->rMax();
     ATH_MSG_DEBUG( " ----  rmin/rmax: " << currentRmin << "/" << currentRmax );
     if (maxRmax<currentRmax)  
       maxRmax = currentRmax;
     if (minRmin>currentRmin)  
       minRmin = currentRmin;
     
     // fill the elements for the layers into the surface arrays
     // get the center position
     const Amg::Vector3D& orderPosition = (*hgtdDetIter)->center();
     
     // register the chosen side in the object array
     Trk::SharedObject<const Trk::Surface> sharedSurface(&((*hgtdDetIter)->surface()), [](const Trk::Surface*){});
     Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
     
     discSurfaces[currentlayer].push_back(surfaceOrder);    
     
   } else if (!(*hgtdDetIter))
       ATH_MSG_WARNING("Not valid pointer to HGTD Detector element... something wrong with the Id dictionary?");
  }

  // adding some envelope
  maxRmax += m_discEnvelopeR;
  minRmin -= m_discEnvelopeR;  
  
  // construct the layers
  std::vector<Trk::DiscLayer*>* discLayers = new std::vector<Trk::DiscLayer* >;
  
  double thickness = m_discThickness;
  
  int discCounter = 0;
  for (auto& thisDiscZpos : discZpos) {    
    // screen output           
    ATH_MSG_DEBUG( "Building a DiscLayer: " );
    ATH_MSG_DEBUG( "  -> At Z - Position      :  " << thisDiscZpos );
    ATH_MSG_DEBUG( "  -> With Thickness       :  " << thickness   << " i- ncludes envelope tolerance : " << m_discEnvelopeR );
    ATH_MSG_DEBUG( "  -> With Rmin/Rmax (est) :  " << minRmin << " / " << maxRmax );
    
    ATH_MSG_DEBUG( "... creating binned array ... ");
    
    std::vector<float> rBins = {minRmin};
    std::vector<std::vector<float>> phiBins = {{}};
    
    evaluateBestBinning(discSurfaces[discCounter], rBins, maxRmax, phiBins);
    
    // Build the BinUtilities using the bins defined at construction
    // the extension is provided in the previous loop
    Trk::BinUtility* BinUtilityR = new Trk::BinUtility(rBins, Trk::open, Trk::binR);
    std::vector<Trk::BinUtility*>* subBinUtilitiesPhi = new std::vector<Trk::BinUtility*>;
    ATH_MSG_DEBUG("BinUtilityR --> " << *BinUtilityR );
    
    for (unsigned int bin = 0; bin < rBins.size()-1; bin++) {
      Trk::BinUtility* BinUtilityY = new Trk::BinUtility(phiBins.at(bin), Trk::closed, Trk::binPhi);
      subBinUtilitiesPhi->push_back(BinUtilityY);
      ATH_MSG_DEBUG(bin << ") BinUtilityPhi --> " << *BinUtilityY );
    }
    
    // prepare the binned array, it can be with one to several rings            
    Trk::BinnedArray<Trk::Surface>* currentBinnedArray = new Trk::BinnedArray1D1D<Trk::Surface>(discSurfaces[discCounter], BinUtilityR, subBinUtilitiesPhi);
    
    ATH_MSG_DEBUG( "... done!" ); 
    
    int discSurfacesNum = (discSurfaces[discCounter]).size();
    
    ATH_MSG_DEBUG( "Constructed BinnedArray for DiscLayer with "<< discSurfacesNum << " SubSurfaces." );

    // always run the geometry validation to catch flaws
    if (m_runGeometryValidation) {
      // checking for :
      //   - empty surface bins
      //   - doubly filled bins
      std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
      std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
      // check the registered surfaces in the binned array
      const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects();
      size_t dsumCheckSurfaces = 0;
      double lastPhi = 0.;
      for (const auto & asurfIter : arraySurfaces){
        if ( asurfIter ) {
          ++dsumCheckSurfaces;
          usmIter = uniqueSurfaceMap.find(asurfIter);
          lastPhi = asurfIter->center().phi();
          if (usmIter != uniqueSurfaceMap.end()) {
            ATH_MSG_WARNING("Non-unique surface found with eta/phi = "
                            << asurfIter->center().eta() << " / "
                            << asurfIter->center().phi());
          } else {
            uniqueSurfaceMap[asurfIter] = asurfIter->center();
          }
        } else {
          ATH_MSG_WARNING("Zero-pointer in array detected in this ring, last "
                          "valid phi value was = "
                          << lastPhi << " --> discCounter: " << discCounter);
        }
      }
      sumCheckhgtdModules +=  dsumCheckSurfaces;   
    }

    // get the layer material from the helper method
    const Trk::LayerMaterialProperties& layerMaterial = discLayerMaterial(minRmin,maxRmax);
    
    /// position & bounds of the active Layer
    Amg::Transform3D activeLayerTransform ;
    activeLayerTransform = Amg::Translation3D(0.,0.,thisDiscZpos);
      
    Trk::DiscBounds* activeLayerBounds = new Trk::DiscBounds(minRmin, maxRmax);
     
    Trk::OverlapDescriptor* olDescriptor = new HGTD_OverlapDescriptor(currentBinnedArray, 
                                                                       rBins, phiBins);
     
    // layer creation; deletes currentBinnedArray in baseclass 'Layer' upon destruction
    // activeLayerTransform deleted in 'Surface' baseclass
    Trk::DiscLayer* activeLayer = new Trk::DiscLayer(activeLayerTransform,
                                                     activeLayerBounds,
                                                     currentBinnedArray,
                                                     layerMaterial,
                                                     thickness,
                                                     olDescriptor);
    
    // register the layer to the surfaces
    const std::vector<const Trk::Surface*>& layerSurfaces = currentBinnedArray->arrayObjects();
    registerSurfacesToLayer(layerSurfaces,*activeLayer);
    discLayers->push_back(activeLayer);
    // increase the disc counter by one
    ++discCounter;     
  }  

  // 
  ATH_MSG_DEBUG( hgtdModules << " HGTD Modules parsed for Disc Layer dimensions." );
  if (m_runGeometryValidation) {
    ATH_MSG_DEBUG( sumCheckhgtdModules << " HGTD Modules filled in Disc Layer Arrays." );
    if ( hgtdModules-sumCheckhgtdModules )
      ATH_MSG_WARNING( hgtdModules-sumCheckhgtdModules << " Modules not registered properly in binned array." );       
  }

  // sort the vector
  Trk::DiscLayerSorterZ zSorter;
  std::sort(discLayers->begin(), discLayers->end(), zSorter);
 
  EventIDRange range = readHandle.getRange();
  return std::make_pair(range, discLayers);
}

const Trk::BinnedLayerMaterial HGTD_LayerBuilderCond::discLayerMaterial(double rMin, double rMax) const
{
  Trk::BinUtility layerBinUtilityR(m_rBins, rMin, rMax, Trk::open, Trk::binR);
  Trk::BinUtility layerBinUtilityPhi(m_phiBins, -M_PI, M_PI, Trk::closed, Trk::binPhi);
  layerBinUtilityR += layerBinUtilityPhi;
  return Trk::BinnedLayerMaterial(layerBinUtilityR);  
}     

void HGTD_LayerBuilderCond::registerSurfacesToLayer(const std::vector<const Trk::Surface*>& layerSurfaces, const Trk::Layer& lay) const
{
   if (!m_setLayerAssociation) return;    
   // register the surfaces to the layer
   for (const auto & surfaces : layerSurfaces) {
     if (surfaces) { 
       // register the current surfaces --------------------------------------------------------
       // Needs care for Athena MT 
       Trk::ILayerBuilderCond::associateLayer(lay, const_cast<Trk::Surface&>(*surfaces));
     }
   }
   return;
}

void HGTD_LayerBuilderCond::evaluateBestBinning(std::vector<Trk::SurfaceOrderPosition>& surfaces,
                                                std::vector<float>& rBins, float& maxRadius,
                                                std::vector<std::vector<float>>& phiBins) 
{
  // get all the centers (r,phi), as you want to play with them
  std::vector < std::pair< float, float> > centers = {};
  centers.reserve(surfaces.size());
  for ( auto& orderedSurface : surfaces) {
    centers.emplace_back(orderedSurface.second.perp(), orderedSurface.second.phi());
  }
  
  // sorting the centers accordingly to r
  std::sort(centers.begin(), centers.end(), 
            [](const std::pair< float, float>& a, const std::pair< float, float>& b) -> bool {
              return a.first < b.first; 
            });
  
  // at the beginning use a fine binning in phi
  // it is updated later to fit the amount of surfaces
  // once you have defined a bin in radius
  int bins = 100;
  float step = 2*M_PI/float(bins);
  std::vector<float> finerBinning = {};
  finerBinning.reserve(bins);
  
  for (int bin = 0; bin<=bins; bin++) {
    finerBinning.push_back(-M_PI+step*bin);
  }
  
  // use this vector to save the indices and 
  // guess when you have to add 
  // an additional bin in r
  std::vector<int> phiIndices = {};
  std::vector<float> tmpRadii = {};
  
  for (auto& center : centers) {
    float phi = center.second;
    const auto boundVal =  std::lower_bound(finerBinning.begin(), finerBinning.end(), phi);
    int phiIndex = std::distance(finerBinning.begin(), boundVal);
    // if the element fits in the given r bin, add it, 
    // otherwise reset the indices and start a new r bin
    if (std::find(phiIndices.begin(), phiIndices.end(), phiIndex)==phiIndices.end()) {
      phiIndices.push_back(phiIndex);
      tmpRadii.push_back(center.first);
    } else {
      phiIndices.clear();
      for (unsigned int index = (tmpRadii.size()-1); index>0; index--) {
        auto& prevRadius = tmpRadii.at(index);
        if ( std::abs(prevRadius - center.first)<1e-5 ) {
          const auto boundVal =  std::lower_bound(finerBinning.begin(), finerBinning.end(), phi);
          int phiIndex = std::distance(finerBinning.begin(), boundVal);
          phiIndices.push_back(phiIndex);
          continue;
        } else {
          float r = 0.5*(prevRadius+center.first);
          rBins.push_back(r);
          tmpRadii = {prevRadius};
          break;
        }
      }
    }
  }
  
  rBins.push_back(maxRadius);
  
  // now we have the best binning in r and want to
  // map the centers accordingly to this  
  std::vector< std::vector < float > > binnedCenters = {{}};
  
  for (auto& center : centers) {
    float r = center.first;
    float phi = center.second;
    const auto boundVal =  std::lower_bound(rBins.begin(), rBins.end(), r);
    int rIndex = std::distance(rBins.begin(), boundVal);
    if (int(binnedCenters.size())<rIndex)
      binnedCenters.push_back({phi});
    else 
      binnedCenters.back().push_back(phi);
  }
  
  // now that we have the centers binned in r, we evaluate the best 
  // bin in phi for each of those bins
  bool isFirst = true;
  for (auto& centersInBin : binnedCenters) {
    // sorting the centers accordingly to phi_bins
    std::sort(centersInBin.begin(), centersInBin.end());
    if (isFirst) {
      phiBins.back().push_back(-M_PI);
      isFirst=false;
    } else phiBins.push_back({-M_PI});
    for (unsigned int index = 0; index<(centersInBin.size()-1); index++) {
      float phi = 0.5*(centersInBin.at(index)+centersInBin.at(index+1));
      phiBins.back().push_back(phi);
    }  
  }
    
  return;
}
