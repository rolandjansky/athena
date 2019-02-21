///////////////////////////////////////////////////////////////////
// BarrelBuilderXML.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include <InDetTrackingGeometryXML/XMLGeoTemplates.h>
#include <InDetTrackingGeometryXML/XMLReaderSvc.h>
#include "InDetTrackingGeometryXML/BarrelBuilderXML.h"
#include "InDetTrackingGeometryXML/StaveBuilderXML.h"
#include "InDetTrackingGeometryXML/IModuleProvider.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometryAlpine/AlpineLayer.h"
#include "TrkGeometryAlpine/AlpineStave.h"
#include "TrkGeometryAlpine/AlpineApproachDescriptor.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "GaudiKernel/IToolSvc.h"
#include "TMath.h"
#include <string>
#include <iostream>
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
//
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/MaterialLayer.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

namespace Trk {
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

InDet::BarrelBuilderXML::BarrelBuilderXML(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixelCase(true),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","InDetXMLReaderSvc"),
  m_staveBuilder("InDet::StaveBuilderXML/StaveBuilderXML"),
  m_moduleProvider("InDet::SiModuleProvider/SiModuleProvider"),
  m_barrelLayerBinsZ(100),
  m_barrelLayerBinsPhi(1),
  m_customMaterial(false),
  m_impMatDescription(false)
{
  declareInterface<BarrelBuilderXML>(this);

  declareProperty("PixelCase",          m_pixelCase);
  declareProperty("InDetXMLReader",     m_xmlReader);
  declareProperty("StaveBuilder",       m_staveBuilder);
  declareProperty("ModuleProvider",     m_moduleProvider);
  declareProperty("BarrelLayerBinsZ",   m_barrelLayerBinsZ);
  declareProperty("BarrelLayerBinsPhi", m_barrelLayerBinsPhi);
  declareProperty("CustomMaterial",     m_customMaterial);
  declareProperty("ImpMatDescription",  m_impMatDescription);
}

InDet::BarrelBuilderXML::~BarrelBuilderXML()
{
}

StatusCode InDet::BarrelBuilderXML::initialize()
{
  ATH_MSG_INFO("InDet::BarrelBuilderXML::initialize()");

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_INFO("InDet::XMLReaderSvc successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("InDet::XMLReaderSvc: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  const IGeoModelSvc* geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure()) return sc;
  //Adding check to tell whether to run with the improved material description
  //Only works on specific geoTags 19-00-03 (not yet added) and 20-00-03
  if( geoModel->atlasVersion() == "ATLAS-P2-ITK-20-00-03" ) {
    ATH_MSG_INFO(geoModel->atlasVersion() << ": running with improved material description for the inclined region");
    m_impMatDescription = true;
  }
  return StatusCode::SUCCESS;
}

StatusCode InDet::BarrelBuilderXML::finalize()
{
  ATH_MSG_INFO("InDet::BarrelBuilderXML::finalize()");
  return StatusCode::SUCCESS;
}

bool InDet::BarrelBuilderXML::isAlpineLayer(unsigned int ilayer) const 
{
  InDet::BarrelLayerTmp *layerTmp = getLayerTmp(ilayer);
  if(layerTmp->stave_type.compare("Alpine")==0) return true;
  
  return false;
}

unsigned int InDet::BarrelBuilderXML::nbOfLayers() const
{ 
  if(m_pixelCase) return m_xmlReader->nbOfPixelBarrelLayers();
  return m_xmlReader->nbOfSCTBarrelLayers();
}

void InDet::BarrelBuilderXML::setPixelCase(bool isPixel)
{
  m_pixelCase = isPixel;
  // Set pixel or SCT case for stave builder
  m_staveBuilder->setPixelCase(m_pixelCase);
}

InDet::BarrelLayerTmp *InDet::BarrelBuilderXML::getLayerTmp(unsigned int ilayer) const
{
  if(m_pixelCase) return m_xmlReader->getPixelBarrelLayerTemplate(ilayer); 
  return m_xmlReader->getSCTBarrelLayerTemplate(ilayer); 
}

std::vector<InDet::StaveTmp*> InDet::BarrelBuilderXML::getStaveTmp(unsigned int ilayer) const
{
  if(m_pixelCase) return m_xmlReader->getPixelStaveTemplate(ilayer); 
  return m_xmlReader->getSCTStaveTemplate(ilayer); 
}

Trk::CylinderLayer *InDet::BarrelBuilderXML::createActiveLayer(unsigned int ilayer, int startLayer, int endLayer) const
{  
  m_staveBuilder->setPixelCase(m_pixelCase);
  if(isAlpineLayer(ilayer)) {
    return createActiveAlpineLayer(ilayer, startLayer, endLayer);
  }else{
    return createActiveCylinderLayer(ilayer, startLayer, endLayer);
  }
}

//////////////////////////////////
// Create special AlpineLayers  //
//////////////////////////////////

Trk::AlpineLayer *InDet::BarrelBuilderXML::createActiveAlpineLayer(unsigned int ilayer, int startLayer, int endLayer) const
{  

//   std::cout<<"ALPINE : Trk::AlpineLayer *InDet::BarrelBuilderXML::createActiveAlpineLayer(unsigned int ilayer, int startLayer, int endLayer) const"<<std::endl;


  // Get layer for this layer index 
  InDet::BarrelLayerTmp *layerTmp = getLayerTmp(ilayer);
  if(!layerTmp) return 0;

  // Check if layer is to be built
  int layer_index = layerTmp->index;
  if(layer_index<startLayer) return 0;
  if(endLayer>=0 && layer_index>endLayer) return 0;

  // retrieve stave infos for this layer
  double R                = layerTmp->radius;
  int    nstaves          = layerTmp->stave_n;  
  double zTilt            = layerTmp->stave_tilt;
  double zOffset          = layerTmp->stave_zoffset;  
  double phiOffset        = layerTmp->phioffset;
  std::string stave_type  = layerTmp->stave_type;
  bool   isOuterMost      = false;

  // Get stave templates list for this layer
  std::vector<InDet::StaveTmp*> staveTmpList = getStaveTmp(ilayer);
  int nStaveTmp = staveTmpList.size();
  if(nStaveTmp==0) {
    ATH_MSG_WARNING("No stave template found for layer " << ilayer);
    return 0;
  }

  // to compute stave support length and active layer bounds
  double support_halfLength  = 0.;
  double bound_Rmax  = 0.;
  double bound_Rmin  = 1000.;

  // Create the staves for the active layer - to be done first, needed to compute layer bounds
  std::vector<Trk::AlpineStave *> staves;
  if(nstaves%nStaveTmp != 0) {
    ATH_MSG_ERROR("Can not build " << nstaves << " staves from " <<  nStaveTmp << " stave templates - not a multiple");
  }

  std::vector<const Trk::TrkDetElementBase*> allElements;  
  // prepare bin utility in Phi - steering BinUtility vs Phi
  double halfPhiStep =  TMath::Pi()/nstaves;
  double minphi =  99999.;
  double maxphi = -99999.;
  
  int stave_idx = 0;
  std::vector< Trk::BinUtility*>* subBinUtilitiesZ = new std::vector<Trk::BinUtility*>;
  for(int istave=0;istave<int(nstaves/nStaveTmp);istave++) {
    zOffset *= -1;
    // Loop over stave templates
    for(int itmp=0;itmp<nStaveTmp;itmp++){
      ATH_MSG_DEBUG("Building stave " << istave << " with staveTemplate " << itmp );
      InDet::StaveTmp* staveTmp = staveTmpList.at(itmp);
      staves.push_back(m_staveBuilder->createStave(staveTmp,stave_idx,nstaves,R,zTilt,zOffset,phiOffset,isOuterMost));
      //staveTmp->Print();
      support_halfLength  = std::max(support_halfLength,staveTmp->support_halflength);
      bound_Rmax  = std::max(bound_Rmax,staveTmp->rMax + 0.1);
      bound_Rmin  = std::min(bound_Rmin,staveTmp->rMin - 0.1);
      stave_idx++;
      
      std::vector<const Trk::TrkDetElementBase*>  currentElements;
      for (auto& surf: (staves.back())->getSurfaces()){
	currentElements.push_back(surf->associatedDetectorElement());
      }

      std::sort(currentElements.begin(),currentElements.end(),sortElements);
      
      //for (unsigned int el = 0; el<currentElements.size(); el++) 
      //ATH_MSG_INFO("Element :"<< el << "  " << currentElements.at(el)->center().z() <<  "   " << currentElements.at(el)->center().phi());
      
      allElements.insert(allElements.end(), currentElements.begin(), currentElements.end());

      // filling z boundaries
      std::vector<float> z_boundaries;
      z_boundaries.push_back(-support_halfLength);
      for (unsigned int el = 0; el<currentElements.size(); el++) {
	minphi = std::min(allElements.at(el)->center().phi(), minphi);
	maxphi = std::max(allElements.at(el)->center().phi(), maxphi);
	if (el<(currentElements.size()-1))
	  z_boundaries.push_back(0.5*(currentElements.at(el)->center().z() + currentElements.at(el+1)->center().z()));
      }
      z_boundaries.push_back(support_halfLength);
      
      // create and save BinUtility vs Z for each stave
      Trk::BinUtility* tmpBinUtilityZ = new Trk::BinUtility(z_boundaries,
							    Trk::open,
							    Trk::binZ);
      
      subBinUtilitiesZ->push_back(tmpBinUtilityZ);
      //ATH_MSG_INFO("Creating z BinUtility :" << *tmpBinUtilityZ);
    }
  }  
  
  if (fabs(minphi+maxphi)< halfPhiStep && fabs(TMath::Pi()+minphi) < 0.5*halfPhiStep ){
    ATH_MSG_DEBUG("Detected module fluctuation around +/- M_PI, correcting for it.");
    ATH_MSG_DEBUG("    min phi / max phi detected  : "  << minphi << " / " << maxphi );
    minphi += 2*halfPhiStep;
  }
  
  // now prepare the phi values
  ATH_MSG_DEBUG("Preparing the Phi-binning for   : " << nstaves << " sectors.");
  ATH_MSG_DEBUG("    min phi / max phi detected  : " << minphi << " / " << maxphi );
  double minPhiCorrected = minphi-halfPhiStep;
  //double maxPhiCorrected = maxphi+halfPhiStep;   // ST limit correction
  double maxPhiCorrected = minPhiCorrected + 2*TMath::Pi();
  
  // catch if the minPhi falls below M_PI
  if (minPhiCorrected < -TMath::Pi()){
    minPhiCorrected += 2*halfPhiStep;
    maxPhiCorrected += 2*halfPhiStep;
  }
  
  ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );
  
  Trk::BinUtility* BinUtilityPhi = new Trk::BinUtility(nstaves,
						       minPhiCorrected,
						       maxPhiCorrected,
						       Trk::closed,
						       Trk::binPhi);

  //ATH_MSG_INFO("Creating phi BinUtility :" << *BinUtilityPhi);
 
  // compute layer thickness and active layer bounds
  double bound_R     = (bound_Rmax+bound_Rmin)/2.0;
  double layer_thickness = bound_Rmax-bound_Rmin; 

  // compute and store approach surfaces for plain
  Trk::ApproachSurfaces* aSurfaces = new Trk::ApproachSurfaces();
  aSurfaces->push_back(new Trk::CylinderSurface(bound_Rmax,support_halfLength+zOffset)); // outer = 0
  aSurfaces->push_back(new Trk::CylinderSurface(bound_Rmin,support_halfLength+zOffset)); // inner = 1

  // create the approach descriptor
  Trk::IApproachDescriptor *aDescriptor = new Trk::AlpineApproachDescriptor(aSurfaces,false); // false = no rebuild 

  // create the material - suppose all staves have same support for now 
  const Trk::LayerMaterialProperties* material = barrelLayerMaterial(bound_R, support_halfLength+zOffset,staveTmpList.at(0));
  
  // prepare the active overlap descriptor       
  Trk::OverlapDescriptor* olDescriptor = m_moduleProvider->getPlanarOverlapDescriptor(m_pixelCase, true);

  // create the active layer
  ATH_MSG_DEBUG("Create Trk::AlpineLayer ");
  Trk::AlpineLayer *activeLayer = new Trk::AlpineLayer(new Trk::CylinderBounds(bound_R,support_halfLength+zOffset),
						       *material,
						       layer_thickness,
						       olDescriptor,
						       aDescriptor,
						       int(Trk::active));
  
  // register the staves to the active layer
  for(int istave=0;istave<nstaves;istave++) activeLayer->addStave(staves.at(istave));

  // register the layer to the surfaces
  const std::vector<const Trk::Surface*>& layerSurfaces     = activeLayer->getSurfaces();
  registerSurfacesToLayer(layerSurfaces,*activeLayer);
  
  // generic solution: each stave can have a different number of modules: we need 1D1D BinnedArray
  Trk::BinnedArray<Trk::Surface>* binnedArray = getBinnedArray1D1D(*BinUtilityPhi,*subBinUtilitiesZ,allElements);
  activeLayer->fillSurfaceArray(binnedArray);
  
  if (material)
    ATH_MSG_DEBUG("ALPINE Material Properties Input:" <<  *(material) << "  Output: " << *(activeLayer->layerMaterialProperties()) 
		  << " needs material properties:" << activeLayer->needsMaterialProperties());
  // Cleanup
  delete material;

  // Return created layer
  return activeLayer;

  ATH_MSG_DEBUG( "Stave Successfully returned");

}

//////////////////////////////////////
// Create  standard CylinderLayers  //
/////////////////////////////////////


Trk::CylinderLayer *InDet::BarrelBuilderXML::createActiveCylinderLayer(unsigned int ilayer, int startLayer, int endLayer) const
{

  // Get layer & stave templates for this layer index 
  InDet::BarrelLayerTmp *layerTmp = getLayerTmp(ilayer); 
  if(!layerTmp) return 0;
  
  // Check if layer is to be built (in case of multiple providers)
  int layer_index = layerTmp->index;
  if(layer_index<startLayer) return 0;
  if(endLayer>=0 && layer_index>endLayer) return 0;
  
  // retrieve stave infos for this layer
  double R                = layerTmp->radius;
  int    nstaves          = layerTmp->stave_n;  
  std::string stave_type  = layerTmp->stave_type;

  // Retrieve stave templates for this layer
  std::vector<InDet::StaveTmp*> staveTmpList = getStaveTmp(ilayer); 
  int nStaveTmp = staveTmpList.size();
  if(nStaveTmp==0){
    ATH_MSG_WARNING("No stave template found for layer " << ilayer);
    return 0;
  }    
  // create detector elements - layers bounds computed inside
  // prepare vector to store barrel elements
  std::vector<const Trk::TrkDetElementBase*> allElements;
  std::vector<Trk::TrkDetElementBase*> currentElements;
  std::vector<Trk::TrkDetElementBase*> prevElementsInPhi;
  std::vector<Trk::TrkDetElementBase*> firstElementsInPhi;
  
  // to compute stave support length and active layer bounds
  double active_halfLength  = 0.;
  double bound_Rmax  = 0.;
  double bound_Rmin  = 1000.;

  // prepare vectors to store BinUtilities vs Z
  std::vector< Trk::BinUtility*>* subBinUtilitiesZ = new std::vector<Trk::BinUtility*>;

  // Loop over staves
  if(nstaves%nStaveTmp != 0) {
    ATH_MSG_ERROR("Can not build " << nstaves << " staves from " <<  nStaveTmp << " stave templates - not a multiple");
  }

  int stave_idx = 0;
  for(int istave=0;istave<int(nstaves/nStaveTmp);istave++) {
    // Loop over stave templates
    for(int itmp=0;itmp<nStaveTmp;itmp++){

      currentElements.clear();

      InDet::StaveTmp* staveTmp = staveTmpList.at(itmp);
      int nmodules = staveTmp->b_modn;
      if(staveTmp == 0 || nmodules == 0 ){
	ATH_MSG_WARNING("Cylinder layer " << ilayer << " stave template " << itmp << " not found !");
	continue;
      }
      createCylinderBarrelModules(ilayer,stave_idx,staveTmp,currentElements);
      ATH_MSG_DEBUG("Cylinder stave " << stave_idx << " built in layer " << ilayer << " with " << nmodules << " modules using staveTemplate " << staveTmp->name);
      // compute new layer bounds
      active_halfLength  = std::max(active_halfLength,staveTmp->active_halflength);
      bound_Rmax  = std::max(bound_Rmax,staveTmp->rMax + 0.1);
      bound_Rmin  = std::min(bound_Rmin,staveTmp->rMin - 0.1);
      // create and save BinUtility vs Z for each stave
      // WARNING: if STUB, it's a bit more tricky
      if(layerTmp->stave_type.compare("STUB") == 0){
	std::vector<float> zbounds;
	InDet::ModuleTmp* moduleTmp = m_xmlReader->getModuleTemplate(staveTmp->b_type); 
	float zstep = moduleTmp->length+staveTmp->b_gap;
	for(int im=0;im<nmodules+1;im++){
	  if(im <= nmodules/2.) zbounds.push_back(-staveTmp->support_halflength + im*zstep);
	  else zbounds.push_back(staveTmp->support_halflength - (nmodules-im)*zstep);
	} 
	Trk::BinUtility* tmpBinUtilityZ = new Trk::BinUtility(zbounds,
							      Trk::open,
							      Trk::binZ);
	subBinUtilitiesZ->push_back(tmpBinUtilityZ);
      }else{
	Trk::BinUtility* tmpBinUtilityZ = new Trk::BinUtility(nmodules, 
							      -staveTmp->active_halflength,
							      staveTmp->active_halflength,
							      Trk::open,
							      Trk::binZ);
	subBinUtilitiesZ->push_back(tmpBinUtilityZ);
      }
      stave_idx++;

      allElements.insert(allElements.end(), currentElements.begin(), currentElements.end());
      if(firstElementsInPhi.size()==0) {
	firstElementsInPhi = currentElements;
	prevElementsInPhi = currentElements;
      } else {
//       std::cout << prevElementsInPhi
	setPhiNeighbors(prevElementsInPhi,currentElements);
	prevElementsInPhi = currentElements;
      }
    } // end stave templates loop
  } // end staves loop

  // Close the neighbors loop in phi
  setPhiNeighbors(currentElements,firstElementsInPhi);
  
  // compute layer thickness and active layer bounds
  double bound_R     = (bound_Rmax+bound_Rmin)/2.0;
  double layer_thickness = bound_Rmax-bound_Rmin; 
  
  ATH_MSG_DEBUG("Building CylinderLayer " << layerTmp->name << " R=" << R << " thickness=" << layer_thickness 
	       << " active halflength=" << active_halfLength);
  
  // prepare bin utility in Phi - steering BinUtility vs Phi
  double halfPhiStep =  TMath::Pi()/nstaves;
    
  double minphi =  99999.;
  double maxphi = -99999.;
  
  for (unsigned int allEl = 0 ; allEl<allElements.size(); allEl++) {
     minphi = std::min(allElements.at(allEl)->center().phi(), minphi);
     maxphi = std::max(allElements.at(allEl)->center().phi(), maxphi);
  }
  
  if (fabs(minphi+maxphi)< halfPhiStep && fabs(TMath::Pi()+minphi) < 0.5*halfPhiStep ){
    ATH_MSG_DEBUG("Detected module fluctuation around +/- M_PI, correcting for it.");
    ATH_MSG_DEBUG("    min phi / max phi detected  : "  << minphi << " / " << maxphi );
    minphi += 2*halfPhiStep;
  }
  
  // now prepare the phi values
  ATH_MSG_DEBUG("Preparing the Phi-binning for   : " << nstaves << " sectors.");
  ATH_MSG_DEBUG("    min phi / max phi detected  : " << minphi << " / " << maxphi );
  double minPhiCorrected = minphi-halfPhiStep;
  //double maxPhiCorrected = maxphi+halfPhiStep;   // ST limit correction
  double maxPhiCorrected = minPhiCorrected + 2*TMath::Pi();
  
  // catch if the minPhi falls below M_PI
  if (minPhiCorrected < -TMath::Pi()){
    minPhiCorrected += 2*halfPhiStep;
    maxPhiCorrected += 2*halfPhiStep;
  }
  
  ATH_MSG_VERBOSE("    min phi / max phi corrected : " << minPhiCorrected << " / " << maxPhiCorrected );

  Trk::BinUtility* BinUtilityPhi       =  new Trk::BinUtility(nstaves,
							      minPhiCorrected,
							      maxPhiCorrected,
							      Trk::closed, 
							      Trk::binPhi);
  
  
  // generic solution: each stave can have a different number of modules: we need 1D1D BinnedArray
  Trk::BinnedArray<Trk::Surface>* binnedArray = getBinnedArray1D1D(*BinUtilityPhi,*subBinUtilitiesZ,allElements);
 

  // prepare the overlap descriptor       
  Trk::OverlapDescriptor* olDescriptor = m_moduleProvider->getPlanarOverlapDescriptor(m_pixelCase);

  // prepare the material - suppose all staves have same support for now 
  const Trk::LayerMaterialProperties* material = barrelLayerMaterial(bound_R, active_halfLength,staveTmpList.at(0));
      
  // create layer
  Trk::CylinderLayer* activeLayer = new Trk::CylinderLayer(new Trk::CylinderBounds(bound_R,active_halfLength),
							   binnedArray,
							   *material,
							   layer_thickness,
							   olDescriptor);
  
//   // get the subsurface array
//   const Trk::SurfaceArray* surfArray = activeLayer->surfaceArray();
//   if (surfArray) {
//       const std::vector<const Trk::Surface*>& layerSurfaces = surfArray->arrayObjects();
//       ATH_MSG_DEBUG(__PRETTY_FUNCTION__ << "   ---> has " << layerSurfaces.size() << " surfaces on the layer.");
//   }
 
  // register the layer to the surfaces
  const std::vector<const Trk::Surface*>& layerSurfaces     = binnedArray->arrayObjects();
  registerSurfacesToLayer(layerSurfaces,*activeLayer);

  if (material)
    ATH_MSG_DEBUG("CYLINDER Material Properties Input:" <<  *(material) << "  Output: " << *(activeLayer->layerMaterialProperties()) 
		  << " needs material properties:" << activeLayer->needsMaterialProperties());
    
  // cleanup
  delete material;
  
  return activeLayer;
}

////////////////////////////////////
// Create Cylinder Barrel Modules  //
////////////////////////////////////

void InDet::BarrelBuilderXML::createCylinderBarrelModules(unsigned int ilayer, unsigned int istave, InDet::StaveTmp* staveTmp, std::vector<Trk::TrkDetElementBase*>& cElements) const 
{
  if(!staveTmp) return ;
  int nmodules = staveTmp->b_modn;

  Trk::TrkDetElementBase* elem           = 0;
  Trk::TrkDetElementBase* prevElemInEta  = 0;
  // start loop over eta modules
  for (int iz = 0; iz < nmodules; iz++) {      
    ATH_MSG_DEBUG("Building module " <<  iz  << " for stave " << istave);
    elem = (Trk::TrkDetElementBase*) CylinderDetElement(ilayer, staveTmp, istave+1, iz+1);
    if(elem==0) {
      ATH_MSG_WARNING("Could not create module for stave " << istave << " iz=" << iz+1);
      continue;
    }

    // Set neighbors in eta
    if(prevElemInEta) {
      m_moduleProvider->setEtaNeighbours(elem,prevElemInEta);
    }

    cElements.push_back(elem);    
    prevElemInEta = elem;

  } // end loop over eta modules

  return;
}

////////////////////////////////
// Create planarDetElements //
////////////////////////////////

Trk::TrkDetElementBase* InDet::BarrelBuilderXML::CylinderDetElement(unsigned int ilayer,  InDet::StaveTmp *staveTmp, int isector, int iz) const
{
  InDet::BarrelLayerTmp *layerTmp = getLayerTmp(ilayer);
  if(!layerTmp) return 0;
  InDet::ModuleTmp* moduleTmp = m_xmlReader->getModuleTemplate(staveTmp->b_type);
  if(!moduleTmp) return 0;

  bool isOuterMost  = (ilayer == nbOfLayers()); 
  bool isBarrel = true;
  bool debug = false;

  int nmodules      = staveTmp->b_modn;
  int nsectors      = layerTmp->stave_n;  
  double radius     = layerTmp->radius;
  double zTilt      = layerTmp->stave_tilt;
  double phiOffset  = layerTmp->phioffset;

  // compute identifier inputs
  int brl_ec       = 0;                       // barrel elements have brl_ec = 0 
  int iphi         = isector + 1 + round(nsectors/2.);
  if(isector*2+4>nsectors) iphi = isector + 1 - nsectors/2;
  double phimin    = -TMath::Pi();
  double phimax    =  TMath::Pi();
  double phistep   = (phimax-phimin)/double(nsectors);
  //double phi       = phimin+isector*phistep-phistep/2.0 + phiOffset;
  double phi       = phimin+(isector+1)*phistep + phiOffset;
  int ieta;
  if((nmodules%2)==0) { // even number of modules, no eta identifier 0
    ieta = iz - nmodules/2;     
    if(iz<=nmodules/2) ieta--;
  } else {
    ieta = iz - (nmodules+1)/2; // eta identifier 0 is the central element
  }

  Identifier id(0);
  IdentifierHash idhash(0);

  m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,0);

  // Compute transform inputs  
  double gap     = staveTmp->b_gap;
  double lengthY = moduleTmp->length; 
  double zstart  = -0.5*(nmodules*lengthY + (nmodules-1)*gap) + lengthY/2.;
  double zstep   = lengthY + gap; 
  double zpos    = zstart + (iz-1)*zstep;
  double tilt    = zTilt + staveTmp->b_tilt; // stave tilt along z
  double xshift  = 0.;                       // shift along x-axis
  double dR      = staveTmp->b_stereoSep/2.; // module center translation w.r.t layer radius     
  double stereoO = staveTmp->b_stereoO;      // stereo angle outer // the one built by default
  double rot     = 0;                        // plain module tilt w.r.t z-axis - none on the central module z=0
  if (!staveTmp->b_sameAngle) {
    if(iz-1<nmodules/2) rot = staveTmp->b_angle;  // then +/- for positive/negative part
    if(iz-1>nmodules/2) rot = -staveTmp->b_angle;
  } else
    rot = staveTmp->b_angle;
    
  // WARNING: if STUB, it's a bit more tricky
  if(layerTmp->stave_type.compare("STUB") == 0){
    if(iz <= nmodules/2.) zpos = -staveTmp->support_halflength + 0.5*lengthY + (iz-1)*zstep;
    else zpos = staveTmp->support_halflength - 0.5*lengthY - (nmodules-iz)*zstep;   
  }

  Amg::Transform3D transform = m_moduleProvider->getTransform(radius,-dR,xshift,zpos,tilt,stereoO,rot,phi);    

  ATH_MSG_DEBUG("Inside CylinderDetElement() --> Creating DetectorElement with " << Amg::toString(transform)
		<< " ilayer" << ilayer << " iz=" << iz-1 << " iphi=" << iphi-1 << " z=" << zpos << " phi=" << phi);

  Trk::TrkDetElementBase* planElement = (Trk::TrkDetElementBase *) m_moduleProvider->getDetElement(id,idhash, moduleTmp, transform.translation(), transform, 
												   m_pixelCase, isBarrel, isOuterMost,debug);
  
//   std::cout << "        -->  Barrel Cylinder Element: " << std::endl;
//   std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 0" << std::endl;
//   std::cout << "        -->  Surface Center = " << planElement->surface().center() << std::endl;
//   std::cout << "        -->  Surface Phi = " << planElement->surface().center().phi() << "     Eta = "<< planElement->surface().center().eta() << std::endl;
    

  // use existing tools to compute layer bounds
  m_xmlReader->computeRbounds(transform, moduleTmp, staveTmp->rMin, staveTmp->rMax);
  m_xmlReader->computeZbounds(transform, moduleTmp, staveTmp->active_halflength);

  if (!planElement) ATH_MSG_WARNING("Inside CylinderDetElement() --> Null pointer for the Planar Detector Element.");

  // Add inner stereo layer  // each element points to the other face
  if(staveTmp->double_sided) {

    isOuterMost  = false;
    double stereoI = staveTmp->b_stereoI;      // stereo angle inner 
    Amg::Transform3D transform_os = m_moduleProvider->getTransform(radius,dR,xshift,zpos,tilt,stereoI,rot,phi);  

    m_moduleProvider->setIdentifier(m_pixelCase,id,idhash,brl_ec,ilayer,iphi,ieta,1);

    m_xmlReader->computeRbounds(transform_os, moduleTmp, staveTmp->rMin, staveTmp->rMax);
    m_xmlReader->computeZbounds(transform_os, moduleTmp, staveTmp->active_halflength);
    Trk::TrkDetElementBase* planElement_os = (Trk::TrkDetElementBase *) m_moduleProvider->getDetElement(id,idhash, moduleTmp, transform_os.translation(), transform_os, 
													m_pixelCase, isBarrel, isOuterMost,debug);
    
//     std::cout << "        -->  Barrel Cylinder Element: " << std::endl;
//     std::cout << "        -->  brl_ec = " << brl_ec << "     layer_disc = " << ilayer << "     iphi = " << iphi << "     ieta = " << ieta << "     side = 1" << std::endl;
//     std::cout << "        -->  Surface Center = " << planElement_os->surface().center() << std::endl;
//     std::cout << "        -->  Surface Phi = " << planElement_os->surface().center().phi() << "     Eta = "<< planElement_os->surface().center().eta() << std::endl;
    
  
    m_moduleProvider->setFrontAndBackSides(planElement,planElement_os);
    
    if (!planElement_os) ATH_MSG_WARNING("Inside CylinderDetElement() --> Null pointer for the other side Planar Detector Element.");

  }
   
  return planElement;

}

/////////////////
// BinnedArray //
/////////////////

Trk::BinnedArray<Trk::Surface>* InDet::BarrelBuilderXML::getBinnedArray1D1D(Trk::BinUtility& steerBinUtility,
									      std::vector<Trk::BinUtility*>& subBinUtility,  
									      std::vector<const Trk::TrkDetElementBase*>& cElements) const
{
  // put surfaces in a vector
  std::vector<Trk::SurfaceOrderPosition> surfaces;

  ATH_MSG_DEBUG("Found " << cElements.size() << " elements to store in Binutility1D1D");

  std::vector<const Trk::TrkDetElementBase*>::const_iterator Elem_Iter = cElements.begin();
  for ( ; Elem_Iter != cElements.end(); Elem_Iter++) 
    {
      const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
 			//Check if modules are inclined TODO: ADD BOOL to only do this check
			//incase of hacked version...
      if(m_impMatDescription && moduleSurface->normal().z() != 0.){ 
				const InDetDD::SiDetectorElement *detEle = dynamic_cast<const InDetDD::SiDetectorElement*>( *Elem_Iter ) ;
				if(detEle) {
	  			Amg::Vector3D versor(0.,0.,1.);
	  			double length = detEle->length()*0.5*sin(fabs(acos(moduleSurface->normal().dot(versor))));
	  			double center = detEle->center().z();
	  			ATH_MSG_DEBUG("Binning --> " << center-length << "/" <<center+length);
	  			Trk::BinUtility layerBinUtilityZ(1, center-length, center+length, Trk::open, Trk::binZ);
	  			const Trk::LayerMaterialProperties* layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityZ);
	  			const Trk::MaterialLayer* materialLayer = new Trk::MaterialLayer(*moduleSurface, *layerMaterial);
	  			moduleSurface->setMaterialLayer(*materialLayer);
	  			ATH_MSG_DEBUG("Surface with material layer... " << materialLayer->layerIndex().value());
				}
      }

      Amg::Vector3D orderPosition((*Elem_Iter)->center());
      // register the module surface
      Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface, true);
      Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
      surfaces.push_back(surfaceOrder);
      
      ATH_MSG_DEBUG("Surface " << (*moduleSurface));
      //ATH_MSG_DEBUG("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
      ATH_MSG_DEBUG("phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta() 
		    << " z = " << moduleSurface->center().z() 
		    << " r = " << sqrt(pow(moduleSurface->center().x(),2)+pow(moduleSurface->center().y(),2)));
    }
  // create 2D-dimensional BinnedArray
  Trk::BinnedArray<Trk::Surface>* currentBinnedArray = new Trk::BinnedArray1D1D<Trk::Surface>(surfaces,&steerBinUtility,&subBinUtility);
  ATH_MSG_DEBUG("Creating the binned array for the sensitive detector elements with SteerBinUtility :");
  ATH_MSG_DEBUG( steerBinUtility );
  ATH_MSG_DEBUG("Creating the binned array for the sensitive detector elements with  "
		<< subBinUtility.size()<< " subBinUtilities :");
  for(unsigned int ib = 0; ib < subBinUtility.size(); ib++) ATH_MSG_DEBUG( *(subBinUtility.at(ib)) );

  // check the registered surfaces in the binned array
  const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects();

  // checking for :
  //   - empty surface bins
  //   - doubly filled bins
  std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
  std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
    
  std::vector<const Trk::Surface*>::const_iterator asurfIter = arraySurfaces.begin();
  std::vector<const Trk::Surface*>::const_iterator asurfIterEnd = arraySurfaces.end();
  for ( ; asurfIter != asurfIterEnd; ++asurfIter ){
    if ( (*asurfIter) ) {
      double R   = std::sqrt((*asurfIter)->center().x()*(*asurfIter)->center().x()
			     +(*asurfIter)->center().y()*(*asurfIter)->center().y());
      double phi = (*asurfIter)->center().phi();
      double   z = (*asurfIter)->center().z();
      ATH_MSG_DEBUG("SURFACE CENTER " << R << " " << z << " " << phi);
      usmIter = uniqueSurfaceMap.find(*asurfIter);
      if ( usmIter != uniqueSurfaceMap.end() )
	ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
      else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
    } else{
      ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector");
    }
  }

  return currentBinnedArray;
}

void InDet::BarrelBuilderXML::setPhiNeighbors(std::vector<Trk::TrkDetElementBase*>& pElements, std::vector<Trk::TrkDetElementBase*>& cElements) const
{
  // Set det element neighbors in phi
  unsigned int pSize = pElements.size();
  if(pSize==0) return;
  unsigned int cSize = cElements.size();
  
  if(pSize!=cSize) {
    ATH_MSG_ERROR("Different sizes for det element vectors: pSize = " << pSize << " cSize = " << cSize);
    return;
  }

  for(unsigned int i=0;i<cSize;i++) {
    Trk::TrkDetElementBase* elem = (Trk::TrkDetElementBase*) cElements.at(i);
    if(elem==0) continue;
    Trk::TrkDetElementBase* prev = (Trk::TrkDetElementBase*) pElements.at(i);
    if(prev==0) continue;
    
    m_moduleProvider->setPhiNeighbours(elem,prev);
  }
}

void InDet::BarrelBuilderXML::registerSurfacesToLayer(const std::vector<const Trk::Surface*>& layerSurfaces, const Trk::Layer& lay) const
{
  std::vector<const Trk::Surface*>::const_iterator laySurfIter    = layerSurfaces.begin();
  std::vector<const Trk::Surface*>::const_iterator laySurfIterEnd = layerSurfaces.end();
  // register the surfaces to the layer
  for (; laySurfIter != laySurfIterEnd; ++laySurfIter){
    if (*laySurfIter) { 
      const std::vector<const Trk::Surface*>& allSurfacesVector = ((*laySurfIter)->associatedDetectorElement())->surfaces();
      std::vector<const Trk::Surface*>::const_iterator allSurfacesIter = allSurfacesVector.begin();
      for ( ; allSurfacesIter != allSurfacesVector.end(); ++allSurfacesIter )
	(**allSurfacesIter).associateLayer(lay);
    }
  }
  return;
}

const Trk::LayerMaterialProperties* InDet::BarrelBuilderXML::barrelLayerMaterial(double r, double hz, InDet::StaveTmp* staveTmp, bool isActive) const{

  if (m_customMaterial && staveTmp) return m_xmlReader->getHomogeneousMaterial(staveTmp->support_material);
  
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------
  // -- material with 1D binning
  Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -hz, hz, Trk::open, Trk::binZ);
  if (m_barrelLayerBinsPhi==1 || isActive){
    layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityZ);
  } else { // -- material with 2D binning
    Trk::BinUtility layerBinUtilityRPhiZ(m_barrelLayerBinsPhi,
                                         -r*TMath::Pi(), r*TMath::Pi(),
                                         Trk::closed,
                                         Trk::binRPhi);
    layerBinUtilityRPhiZ += layerBinUtilityZ;                                                       
    layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);
  } 
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;   

}
