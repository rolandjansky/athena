///////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
//
// ITkEndcapBuilder.cxx
///////////////////////////////////////////////////////////////////

#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include <InDetTrackingGeometryXML/XMLGeoTemplates.h>
#include <InDetTrackingGeometryXML/XMLReaderSvc.h>
#include "InDetTrackingGeometryXML/IModuleProvider.h"
#include "InDetTrackingGeometryXML/EndcapBuilderXML.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TMath.h"
#include <string>
#include <iostream>

namespace Trk {
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

InDet::EndcapBuilderXML::EndcapBuilderXML(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_pixelCase(true),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","InDetXMLReaderSvc"),
  m_moduleProvider("InDet::SiModuleProvider/SiModuleProvider"),
  m_endcapLayerBinsR(50),
  m_endcapLayerBinsPhi(1),
  m_customMaterial(false),
  m_discEnvelope(0.1)
  //m_moduleProvider("iFatras::PlanarModuleProvider/PlanarModuleProvider"),
{
  declareInterface<EndcapBuilderXML>(this);

  declareProperty("PixelCase",          m_pixelCase);
  declareProperty("InDetXMLReader",     m_xmlReader);
  declareProperty("ModuleProvider",     m_moduleProvider);
  declareProperty("EndcapLayerBinR",    m_endcapLayerBinsR);
  declareProperty("EndcapLayerBinPhi",  m_endcapLayerBinsPhi);
  declareProperty("CustomMaterial",     m_customMaterial);
  declareProperty("DiscEnvelope",       m_discEnvelope);

}

InDet::EndcapBuilderXML::~EndcapBuilderXML()
{
}

StatusCode InDet::EndcapBuilderXML::initialize()
{
  ATH_MSG_INFO("InDet::EndcapBuilderXML::initialize()");
  
  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_INFO("InDet::XMLReaderSvc successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("InDet::XMLReaderSvc: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode InDet::EndcapBuilderXML::finalize()
{
  ATH_MSG_INFO("InDet::EndcapBuilderXML::finalize()");
  return StatusCode::SUCCESS;
}

unsigned int InDet::EndcapBuilderXML::nbOfLayers()
{ 
  if(m_pixelCase) return m_xmlReader->nbOfPixelEndcapLayers();
  return m_xmlReader->nbOfSCTEndcapLayers();
}

InDet::EndcapLayerTmp *InDet::EndcapBuilderXML::getLayerTmp(unsigned int ilayer) const
{
  if(m_pixelCase) return m_xmlReader->getPixelEndcapLayerTemplate(ilayer); 
  return m_xmlReader->getSCTEndcapLayerTemplate(ilayer); 
}

void InDet::EndcapBuilderXML::createActiveLayers(unsigned int itmpl, int side, int startLayer, int endLayer, 
						   std::vector< Trk::Layer* >& v_layers) const
{  
  // Get layer template for this index 
  InDet::EndcapLayerTmp *layerTmp = getLayerTmp(itmpl);
  if(!layerTmp) return;

  // Check Layer type - if it's disc, build everything
  if(layerTmp->isDisc) {
    int layer_pos = layerTmp->layer_pos;
    if(layer_pos<startLayer) return; 
    if(endLayer>=0 && layer_pos>endLayer) return;
    createActiveDiscLayers(itmpl, side, v_layers);
  }  else { 
    // if it's rings, check if ring layer has to be built: 
    int layer_pos = layerTmp->layer_pos;
    if(layer_pos<startLayer) return;
    if(endLayer>=0 && layer_pos>endLayer) return;
    createActiveRingLayers(itmpl, side, v_layers);
  } 

  return;  
}

 void InDet::EndcapBuilderXML::createActiveDiscLayers(unsigned int itmpl, int side, 
							std::vector< Trk::Layer* >& v_layers) const
{  

  // Get layer template
  InDet::EndcapLayerTmp *layerTmp = getLayerTmp(itmpl);
  if(!layerTmp) return;

   // prepare vector to store disc elements
  std::vector<Trk::TrkDetElementBase*> cElements;
  
  //store the centerOnModule
  std::vector<Amg::Vector3D> centersOnModule;

  // prepare the variables needed to compute disc Bounds
  double rMin =  999999;
  double rMax = -999999;
  double zMin =  999999;
  double zMax = -999999;

  unsigned int nRings = layerTmp->ringpos.size();

  ATH_MSG_DEBUG("Building Disc Layer " << itmpl << " '" << layerTmp->name << "' side " << side << " with " <<  nRings << " rings");

  // prepare vectors to store BinUtilitiesPhi vs R
  std::vector<float> rCenters;
  std::vector< Trk::BinUtility*>* subBinUtilities = new std::vector<Trk::BinUtility*>;

  // Create elements of all rings
  for(unsigned int iring = 0; iring < nRings; iring++){

    int nsectors          = layerTmp->nsectors.at(iring);
    double zpos           = side*layerTmp->ringpos.at(iring);
    double modzoff        = layerTmp->zoffset.at(iring);
    double mod0phioffset  = layerTmp->phioffset.at(iring);
    double thck           = layerTmp->thickness.at(iring);
    double innerR         = layerTmp->innerRadius.at(iring);
    double outerR         = layerTmp->outerRadius.at(iring);
    double centerR        = 0.5*(innerR+outerR);

    ATH_MSG_DEBUG("  DISC: building ring " << iring << " z=" << zpos <<  " zoffset="  << modzoff 
		 << " phioffset=" << mod0phioffset << " thck=" << thck << " " << nsectors 
		 << " innerR=" << innerR << " outerR=" << outerR << " centerR = <" << centerR << "> side=" << side);

    // create elements for current ring
    createDiscModules(itmpl,iring,side,cElements,centersOnModule);
    
    // check zMin, zMax, rMin and rMax to compute disc bounds
    rMin = std::min(innerR, rMin); 
    rMax = std::max(outerR, rMax); 

    zMin = std::min(zpos-thck*0.5, zMin); 
    zMax = std::max(zpos+thck*0.5, zMax); 

    // compute bin utility in Phi 
    // find lowest and highest bounds in phi (not trivial when there is a phi offset of module 0 !)
    float phiLowBound = 99999;
    float phiHighBound = -99999;
    for (unsigned int cEl = 0 ; cEl<centersOnModule.size(); cEl++) {
      float radius = sqrt(pow(centersOnModule.at(cEl).x(),2)+pow(centersOnModule.at(cEl).y(),2));
      if (radius > outerR or radius < innerR)
	continue;
      
      float centerphi =  centersOnModule.at(cEl).phi();
      if( centerphi < phiLowBound ) phiLowBound = centerphi;
      if( centerphi > phiHighBound ) phiHighBound = centerphi;

    }
    double halfPhiStep = TMath::Pi()/nsectors;
    phiLowBound  -= halfPhiStep;
    phiHighBound += halfPhiStep;

    ATH_MSG_DEBUG("Creating phi BinUtility with these parameters:");
    ATH_MSG_DEBUG("halfPhiStep = " << halfPhiStep << "    phiHighBound = " << phiHighBound << "   phiLowBound = " << phiLowBound);
  
    Trk::BinUtility* tmpBinUtilityPhi = new Trk::BinUtility(nsectors,
							    phiLowBound,
							    phiHighBound,
							    Trk::closed,
							    Trk::binPhi);
  
    // find insertion point (we want the phi binUtilities ordered by increasing r)
    int ordindex = 0;
    for(unsigned int ip=0;ip<rCenters.size();ip++){
      if(centerR > rCenters.at(ip)) ordindex++;
      else break;
    }

    // insertion of R and binUtility at the right place
    rCenters.insert(rCenters.begin()+ordindex,innerR);
    subBinUtilities->insert(subBinUtilities->begin()+ordindex,tmpBinUtilityPhi);

  }// end rings loop

  // last bin
  rCenters.push_back(rMax);

  // compute steering BinUtility in R   
  Trk::BinUtility* steerBinUtility = new Trk::BinUtility(rCenters,
  							 Trk::open,
  							 Trk::binR);

  // generic solution: each ring can have a different number of sectors: need 1D1D 
  Trk::BinnedArray<Trk::Surface>* binnedArray = getBinnedArray1D1D(*steerBinUtility,*subBinUtilities,cElements,centersOnModule);
 
  // prepare the overlap descriptor       
  std::vector<Trk::BinUtility*>* BinUtils = new std::vector<Trk::BinUtility*>;
  if (subBinUtilities) {
    std::vector<Trk::BinUtility*>::iterator binIter = subBinUtilities->begin();
    for ( ; binIter != subBinUtilities->end(); ++binIter)
      BinUtils->push_back((*binIter)->clone());
  }
  Trk::OverlapDescriptor* olDescriptor = m_moduleProvider->getDiscOverlapDescriptor(m_pixelCase,binnedArray,BinUtils);
    
  // position & bounds of the disc layer
  double disc_thickness = std::fabs(zMax-zMin);
  double disc_pos = (zMax+zMin)*0.5;

  Amg::Transform3D*  transf = new Amg::Transform3D;
  (*transf) = Amg::Translation3D(0.,0.,disc_pos);
  
  // get the layer material from the helper method
  const Trk::LayerMaterialProperties* disc_material = endcapLayerMaterial(rMin,rMax,layerTmp);

  // create disc layer
  Trk::DiscLayer* layer = new Trk::DiscLayer(transf,
					     new Trk::DiscBounds(rMin,rMax),
					     binnedArray,
					     *disc_material,
					     disc_thickness,
					     olDescriptor); 

  // register the layer to the surfaces --- if necessary to the other side as well
  const std::vector<const Trk::Surface*>& layerSurfaces     = binnedArray->arrayObjects();
  registerSurfacesToLayer(layerSurfaces,*layer);
  
  ATH_MSG_DEBUG("DiscPos = " << disc_pos << " rMin/rMax = "<< rMin << "/" << rMax 
	       << " zMin/zMax = " << zMin << "/" << zMax << " thickness = " << disc_thickness);

  // store disc layer
  v_layers.push_back(layer);

  // cleanup
  delete disc_material;

  return;
  
} 
   
void InDet::EndcapBuilderXML::createActiveRingLayers(unsigned int itmpl, int side, 
						       std::vector< Trk::Layer* >& v_layers) const
{  

  // Get layer template for this layer index 
  InDet::EndcapLayerTmp *layerTmp = getLayerTmp(itmpl);
  if(!layerTmp) return;

  ATH_MSG_DEBUG("Building Ring Layer " << itmpl << " '" << layerTmp->name << "' side " << side);

  // Loop over rings - create one discLayer per ring
  unsigned int nRings = layerTmp->ringpos.size();
  for(unsigned int iring = 0; iring < nRings; iring++){
    
    double zpos = side*layerTmp->ringpos.at(iring);
    double innerR = layerTmp->innerRadius.at(iring);
    double outerR = layerTmp->outerRadius.at(iring);
    
    // you need to correct the outer radius to fit the corners of the modules
    double phiOffset = layerTmp->phioffset.at(iring);
    int nsectors = layerTmp->nsectors.at(iring);
    double disc_thickness = layerTmp->thickness.at(iring); 

     // position & bounds of the disc layer
    Amg::Transform3D*  transf = new Amg::Transform3D;
    (*transf) = Amg::Translation3D(0.,0.,zpos);

    ATH_MSG_DEBUG("  RING: building ring " << iring << " zpos=" << zpos <<  " thck="  << disc_thickness << " nsect=" << nsectors 
		 << " innerR=" << innerR << " outerR=" << outerR << " side=" << side );

    // create elements
    std::vector<Trk::TrkDetElementBase*> cElements ;
    
    //store the centerOnModule
    std::vector<Amg::Vector3D> centersOnModule;
  
    createDiscModules(itmpl,iring,side,cElements,centersOnModule);

    // compute bin utility in Phi 
    // find lowest and highest bounds in phi (not trivial when there is a phi offset of module 0 !)
    float phiLowBound = 99999;
    float phiHighBound = -99999;
    for (unsigned int cEl = 0 ; cEl<centersOnModule.size(); cEl++) {
      float centerphi =  centersOnModule.at(cEl).phi();
      if( centerphi < phiLowBound ) phiLowBound = centerphi;
      if( centerphi > phiHighBound ) phiHighBound = centerphi;
//       std::cout << "z " << cEl << " --> " << centersOnModule.at(cEl).z() << std::endl;
    }
    
    InDet::ModuleTmp* moduleTmp = m_xmlReader->getModuleTemplate(layerTmp->modtype.at(iring));
    double hwidth = moduleTmp->widthmax*0.5;
    
    double corr = cos(atan(hwidth/outerR));
    outerR *= 1./corr;
    
    double zmin =  99999;
    double zmax = -99999;
    for (auto& el : cElements) {
      zmin = std::min(zmin,el->center().z());
      zmax = std::max(zmax,el->center().z());
    }
    
    disc_thickness+=fabs(zmax-zmin);
    
    double halfPhiStep = TMath::Pi()/nsectors;
    phiLowBound  -= halfPhiStep;
    phiHighBound += halfPhiStep;
    
    ATH_MSG_DEBUG("Creating phi BinUtility with these parameters:");
    ATH_MSG_DEBUG("halfPhiStep = " << halfPhiStep << "    phiHighBound = " << phiHighBound << "   phiLowBound = " << phiLowBound);
        
    Trk::BinUtility* BinUtilityPhi = new Trk::BinUtility(nsectors,
							 phiLowBound,
							 phiHighBound,
							 Trk::closed,
							 Trk::binPhi);

    ATH_MSG_DEBUG("  PHI bin utility - lowhigh -2  : RING "<<iring<<" "<<phiLowBound<<" "<<phiHighBound<<"  //  "<<phiLowBound+2.*TMath::Pi()<<" "<<phiHighBound<<"  Offset/step "<<phiOffset<<" "<<2.*halfPhiStep<<"     - "<<nsectors );

    // create binned array
    Trk::BinnedArray<Trk::Surface>* binnedArray = getBinnedArray1D(*BinUtilityPhi, cElements, centersOnModule);

    // prepare the overlap descriptor       
    Trk::OverlapDescriptor* olDescriptor = m_moduleProvider->getDiscOverlapDescriptor(m_pixelCase);
    
    // get the layer material from the helper method
    const Trk::LayerMaterialProperties* disc_material = endcapLayerMaterial(innerR-m_discEnvelope,
									    outerR+m_discEnvelope,
									    layerTmp);

    // create disc layer
    Trk::DiscLayer* layer = new Trk::DiscLayer(transf,
					       new Trk::DiscBounds(innerR-m_discEnvelope,outerR+m_discEnvelope),
					       binnedArray,
					       *disc_material,
					       disc_thickness,
					       olDescriptor);

    // register the layer to the surfaces --- if necessary to the other sie as well
    const std::vector<const Trk::Surface*>& layerSurfaces     = binnedArray->arrayObjects();
    registerSurfacesToLayer(layerSurfaces,*layer);

    // store disc layer
    v_layers.push_back(layer);
    
    // cleanup
    delete disc_material;

  } // end rings loop
   
  
  return;
}


// LayerBuilder interface method - returning Disc modules for one ring
void InDet::EndcapBuilderXML::createDiscModules(int itmpl, int iring, int side, 
						  std::vector<Trk::TrkDetElementBase*>& cElements, std::vector<Amg::Vector3D>& centersOnModule) const 
{
  InDet::EndcapLayerTmp *layerTmp = getLayerTmp(itmpl);
  if(!layerTmp) return;

  Trk::TrkDetElementBase* elem         = 0;
  Trk::TrkDetElementBase* previousElem = 0;
  ATH_MSG_DEBUG("Building sector 0 for layer index " << itmpl << " iring " << iring);
  Trk::TrkDetElementBase* firstElem    = (Trk::TrkDetElementBase *) createDiscDetElement(itmpl,iring,0,side,centersOnModule);
  cElements.push_back(firstElem);
  previousElem = firstElem;
  int nsectors = layerTmp->nsectors.at(iring);

  // loop over phi sectors
  for(int isector=1;isector<nsectors;isector++) {

    ATH_MSG_DEBUG("Building sector " << isector << " for layer " << itmpl << " iring " << iring);
    elem = (Trk::TrkDetElementBase *) createDiscDetElement(itmpl,iring,isector,side,centersOnModule);
    if(elem==0) continue;
    cElements.push_back(elem);

    // set neighbors in phi
    m_moduleProvider->setPhiNeighbours(elem,previousElem);
    previousElem = elem;    
  }

  // last element: close the loop in phi  
  m_moduleProvider->setPhiNeighbours(firstElem,elem);
  
  ATH_MSG_DEBUG("Number of sectors built=" << nsectors);
  
  return;
}

// create the disc planar element
Trk::TrkDetElementBase* InDet::EndcapBuilderXML::createDiscDetElement(int itmpl, int iring, int isector, int side, std::vector<Amg::Vector3D>& centersOnModule) const
{

  InDet::EndcapLayerTmp *layerTmp = getLayerTmp(itmpl);
  if(!layerTmp) return 0;

  InDet::ModuleTmp* moduleTmp = m_xmlReader->getModuleTemplate(layerTmp->modtype.at(iring));
  
  // rings parameters
  int nsectors             = layerTmp->nsectors.at(iring);
  double zpos              = side*layerTmp->ringpos.at(iring); 
  double zoffset           = layerTmp->zoffset.at(iring); 
  double mod0phioffset     = layerTmp->phioffset.at(iring);
  double ring_rmin         = layerTmp->outerRadius.at(iring);
  double ring_rmax         = layerTmp->innerRadius.at(iring);
  bool useDisc             = layerTmp->useDiscSurface;

  // set identifiers variables
  int brl_ec = 2*side; // For identifier : endcap element brl_ec = +/-2 (neg ec/pos ec)

  int iphi   = side>0 ? isector : (2*nsectors-isector-1)%(nsectors);  
 
  int ieta   = iring; 
  int ilayer = layerTmp->ilayer;
  bool isOuterMost = false; // JL -- needs to be fixed 
  bool isBarrel = false;
  bool debug = false; 

  Identifier idwafer(0);
  IdentifierHash idhash(0);

  ATH_MSG_DEBUG("brl_ec= " << brl_ec << " disc = " << ilayer << " iphi= " << iphi << " ieta = " << ieta );

  m_moduleProvider->setIdentifier(m_pixelCase,idwafer,idhash,brl_ec,ilayer,iphi,ieta,0);

  // create the transform parameters  
  double phistep = 2*TMath::Pi()/nsectors;
  double phi = phistep*(isector)+ mod0phioffset;
  if(phi > TMath::Pi()) phi -= 2*TMath::Pi();
  if(phi < -TMath::Pi()) phi += 2*TMath::Pi();
  
  // First the translation
  double  r = ring_rmin + (ring_rmax-ring_rmin)/2.0; 
  double dr = 0.; 
  // default: create outer stereo module
  double  z = zpos + 0.5*zoffset*pow(-1,isector) - zpos/fabs(zpos)*0.5*layerTmp->stereoSep;
  double stereoO = layerTmp->stereoO;
  double xshift = 0.;
  double tilt = 0.;
  double rot = 0.5*TMath::Pi();
  
  Amg::Vector3D centerOnModule(r*cos(phi), r*sin(phi), z);
      
  Amg::Transform3D transform = m_moduleProvider->getTransform(r,dr,xshift,z,stereoO,tilt,rot,phi,useDisc);    
  Trk::TrkDetElementBase* planElement = (Trk::TrkDetElementBase *) m_moduleProvider->getDetElement(idwafer,idhash, moduleTmp, centerOnModule, 
												   transform, m_pixelCase, isBarrel, isOuterMost, debug,
												   useDisc, ring_rmin, ring_rmax, stereoO);
 
  if (!planElement) ATH_MSG_WARNING("Inside createDiscDetElement() --> Null pointer for the Planar Detector Element.");


  centerOnModule = Amg::Vector3D(planElement->surface().center().perp()*cos(planElement->surface().center().phi()),
				 planElement->surface().center().perp()*sin(planElement->surface().center().phi()),
				 planElement->surface().center().z());
  
  
  // Add outer stereo layer  // each element points to the other face
  if(layerTmp->double_sided) {
 
    m_moduleProvider->setIdentifier(m_pixelCase,idwafer,idhash,brl_ec,ilayer,iphi,ieta,1);

    // warning: add stereo module inside -- add stereo steparation
    z += zpos/fabs(zpos)*layerTmp->stereoSep;
    double stereoI = layerTmp->stereoI;
    Amg::Vector3D centerOnModule_os(r*cos(phi), r*sin(phi), z);
    
    Amg::Transform3D transform_os = m_moduleProvider->getTransform(r,dr,xshift,z,stereoI,tilt,rot,phi,useDisc);    
    Trk::TrkDetElementBase* planElement_os = (Trk::TrkDetElementBase *) m_moduleProvider->getDetElement(idwafer,idhash, moduleTmp, centerOnModule_os, 
													transform_os, m_pixelCase, isBarrel, isOuterMost,debug,
													useDisc, ring_rmin, ring_rmax, stereoI);
    
    
    m_moduleProvider->setFrontAndBackSides(planElement,planElement_os);

    if (!planElement_os) ATH_MSG_WARNING("Inside createDiscDetElement() --> Null pointer for the other side of Planar Detector Element.");
    
  }
  
   centersOnModule.push_back(centerOnModule);

  return planElement;  
}


///////////////////////////////////////////////////
            //  BIN UTILITIES  // 
///////////////////////////////////////////////////

Trk::BinnedArray<Trk::Surface>* InDet::EndcapBuilderXML::getBinnedArray1D(Trk::BinUtility& BinUtility, 
									    std::vector<Trk::TrkDetElementBase*>& cElements,
									    std::vector<Amg::Vector3D>& centersOnModule ) const
{
  // put surfaces in a vector
  std::vector<Trk::SurfaceOrderPosition> surfaces;
  std::vector<Trk::TrkDetElementBase*>::const_iterator Elem_Iter = cElements.begin();
  for (unsigned int cEl = 0 ; Elem_Iter != cElements.end(); Elem_Iter++, cEl++) {
    const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
    Amg::Vector3D orderPosition(centersOnModule.at(cEl));
    
    // register the module surface
    Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface,true);
    Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
    surfaces.push_back(surfaceOrder);
    
    ATH_MSG_DEBUG("Surface " << (*moduleSurface));
    //ATH_MSG_DEBUG("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
    ATH_MSG_DEBUG("Surface Phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta()  
		  << "  Z = " << moduleSurface->center().z());
    ATH_MSG_DEBUG("Surface Phi = " << centersOnModule.at(cEl).phi() << "  Eta = " << centersOnModule.at(cEl).eta()  
		  << "  Z = " << centersOnModule.at(cEl).z());
  }

  // create BinnedArray
  Trk::BinnedArray<Trk::Surface>* currentBinnedArray = new Trk::BinnedArray1D<Trk::Surface>(surfaces,&BinUtility);       
  ATH_MSG_DEBUG("Creating the binned array for the sensitive detector elements with BinUtility :");
  ATH_MSG_DEBUG( BinUtility );

  // check the registered surfaces in the binned array
  const std::vector<const Trk::Surface*>& arraySurfaces = currentBinnedArray->arrayObjects();
  
  // checking for :
  //   - empty surface bins
  //   - doubly filled bins
  std::map< const Trk::Surface*,Amg::Vector3D > uniqueSurfaceMap;
  std::map< const Trk::Surface*,Amg::Vector3D >::iterator usmIter = uniqueSurfaceMap.end();
    
  std::vector<const Trk::Surface*>::const_iterator asurfIter = arraySurfaces.begin();
  std::vector<const Trk::Surface*>::const_iterator asurfIterEnd = arraySurfaces.end();
  unsigned int counter = 0;
  for ( ; asurfIter != asurfIterEnd; ++asurfIter ){
    if ( (*asurfIter) ) {
      usmIter = uniqueSurfaceMap.find(*asurfIter);
      if ( usmIter != uniqueSurfaceMap.end() )
	ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() 
			<< " / " << (*asurfIter)->center().phi());
      else {
	uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
	ATH_MSG_DEBUG("Surface found with z/eta/phi = " << (*asurfIter)->center().z() 
		      << " / " << (*asurfIter)->center().eta() << " / " << (*asurfIter)->center().phi());
      }
    } 
    else {
      ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector " << counter);
    }
  }

  return currentBinnedArray;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Trk::BinnedArray<Trk::Surface>* InDet::EndcapBuilderXML::getBinnedArray1D1D(Trk::BinUtility& steerBinUtility,
									      std::vector<Trk::BinUtility*>& subBinUtility,  
									      std::vector<Trk::TrkDetElementBase*>& cElements,
									      std::vector<Amg::Vector3D>& centersOnModule) const
{
  // put surfaces in a vector
  std::vector<Trk::SurfaceOrderPosition> surfaces;

  ATH_MSG_DEBUG("Found " << cElements.size() << " elements to store in BinutilityRPhi");

  std::vector<Trk::TrkDetElementBase*>::const_iterator Elem_Iter = cElements.begin();
  for (unsigned int cEl = 0 ; Elem_Iter != cElements.end(); Elem_Iter++, cEl++) {
    const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
    Amg::Vector3D orderPosition(centersOnModule.at(cEl));
    // register the module surface
    Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface,true);
    Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
    surfaces.push_back(surfaceOrder);
      
    ATH_MSG_DEBUG("Surface " << (*moduleSurface));
    ATH_MSG_DEBUG("phi = " << moduleSurface->center().phi() << "  Eta = " << moduleSurface->center().eta() 
		 << " z = " << moduleSurface->center().z() 
		 << " r = " << sqrt(pow(moduleSurface->center().x(),2)+pow(moduleSurface->center().y(),2)));
    
    //ATH_MSG_DEBUG("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
    ATH_MSG_DEBUG("phi = " << centersOnModule.at(cEl).phi() << "  Eta = " << centersOnModule.at(cEl).eta() 
		 << " z = " << centersOnModule.at(cEl).z() 
		 << " r = " << sqrt(pow(centersOnModule.at(cEl).x(),2)+pow(centersOnModule.at(cEl).y(),2)));
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Trk::BinnedArray<Trk::Surface>* InDet::EndcapBuilderXML::getBinnedArray2D(Trk::BinUtility& BinUtility,
									      std::vector<Trk::TrkDetElementBase*>& cElements,
									      std::vector<Amg::Vector3D>& centersOnModule) const
{
  // put surfaces in a vector
  std::vector<Trk::SurfaceOrderPosition> surfaces;

  ATH_MSG_DEBUG("Found " << cElements.size() << " elements to store in BinutilityRPhi");

  std::vector<Trk::TrkDetElementBase*>::const_iterator Elem_Iter = cElements.begin();
  for (unsigned int cEl = 0 ; Elem_Iter != cElements.end(); Elem_Iter++, cEl++) {
    const Trk::Surface* moduleSurface = &((*Elem_Iter)->surface());
    Amg::Vector3D orderPosition(centersOnModule.at(cEl));
    // register the module surface
    Trk::SharedObject<const Trk::Surface> sharedSurface(moduleSurface,true);
    Trk::SurfaceOrderPosition surfaceOrder(sharedSurface, orderPosition);
    surfaces.push_back(surfaceOrder);
      
    ATH_MSG_DEBUG("Surface " << (*moduleSurface));
    //ATH_MSG_DEBUG("TransformHit = " << Amg::toString(Amg::CLHEPTransformToEigen((*Elem_Iter)->transformHit())));
    ATH_MSG_DEBUG("phi = " << centersOnModule.at(cEl).phi() << "  Eta = " << centersOnModule.at(cEl).eta() 
		 << " z = " << centersOnModule.at(cEl).z() 
		 << " r = " << sqrt(pow(centersOnModule.at(cEl).x(),2)+pow(centersOnModule.at(cEl).y(),2)));
  }

  // create 2D-dimensional BinnedArray
  Trk::BinnedArray<Trk::Surface>* currentBinnedArray = new Trk::BinnedArray2D<Trk::Surface>(surfaces,&BinUtility);
  ATH_MSG_DEBUG("Creating the binned array for the sensitive detector elements with SteerBinUtility :");
  ATH_MSG_DEBUG( BinUtility );

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
	ATH_MSG_WARNING("Non-unique surface found with eta/phi = " << (*asurfIter)->center().eta() 
			<< " / " << (*asurfIter)->center().phi());
      else uniqueSurfaceMap[(*asurfIter)] = (*asurfIter)->center();
    } else{
      ATH_MSG_WARNING("Null surface defined in BinUtility ArraySurfaces vector");
    }
  }

  return currentBinnedArray;
}


void InDet::EndcapBuilderXML::registerSurfacesToLayer(const std::vector<const Trk::Surface*>& layerSurfaces, const Trk::Layer& lay) const
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

const Trk::LayerMaterialProperties* InDet::EndcapBuilderXML::endcapLayerMaterial(double rMin, double rMax, InDet::EndcapLayerTmp* layerTmp, bool isActive) const
{
  if (m_customMaterial && layerTmp) return m_xmlReader->getHomogeneousMaterial(layerTmp->support_material);
  
  Trk::LayerMaterialProperties* layerMaterial = 0;
  // --------------- material estimation ----------------------------------------------------------------

  Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,rMin,rMax,Trk::open, Trk::binR);
  // -- material with 1D binning
  if (m_endcapLayerBinsPhi==1 || isActive){
    layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } else { // -- material with 2D binning
    Trk::BinUtility layerBinUtilityPhi(m_endcapLayerBinsPhi,-TMath::Pi(),TMath::Pi(),Trk::closed,Trk::binPhi);
    layerBinUtilityR += layerBinUtilityPhi;
    layerMaterial     = new Trk::BinnedLayerMaterial(layerBinUtilityR);
  } 
  // --------------- material estimation ----------------------------------------------------------------
  return layerMaterial;   
}     
