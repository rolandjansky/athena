/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "EndcapRingRef/GeoPixelRingECRingRef.h"
#include "EndcapRingRef/PixelRingSupportXMLHelper.h"

#include "PixelGeoModelModule/GeoDetModulePixel.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "PathResolver/PathResolver.h"

#include <sstream>


GeoPixelRingECRingRef::GeoPixelRingECRingRef(int iLayer, int iRing, double ringRadius, double ringOuterRadius,
					     double zOffset, double phiOffset,
					     int ringSide, int numModules, std::string moduleType, 
					     int diskId, int back_front)
  : m_layer(iLayer),
    m_ring(iRing),
    m_ringId(-1),
    m_radius(ringRadius),
    m_outerRadius(ringOuterRadius),
    m_zOffset(zOffset),
    m_phiOffset(phiOffset),
    m_ringSide(ringSide),
    m_numModules(numModules),
    m_moduleType(moduleType),
    m_diskId(diskId),
    m_front_back(back_front),
    m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
    m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc")
{
  m_bPrebuild = false;
}

GeoPixelRingECRingRef::~GeoPixelRingECRingRef()
{
}

void GeoPixelRingECRingRef::preBuild(const PixelGeoBuilderBasics* basics ) 
{

  // Retrieve PixelModuleSvc (svc that builds and store the modules)
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
    std::cout << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << std::endl;

  // Open link to the xml file that describes the rings
  PixelRingSupportXMLHelper ringHelper;

  // Get ringmodule from PixelModuelSvc
  GeoDetModulePixel* ringModule = m_pixelModuleSvc->getModule(basics,2,m_layer,m_moduleType);
  //  InDetDD::PixelModuleDesign* ringModuleDesign = m_pixelDesignSvc->getDesign(basics,m_moduleType);

  // Compute module radial min max size
  double rmin = m_radius;
  double rmax = ComputeRMax(rmin, 0.01, ringModule->Length(), ringModule->Width());
  // ... module thicknesses
  double thickHyb = ringModule->ThicknessN();
  double thickChip = ringModule->ThicknessP();

  m_halfLength = (thickHyb+thickChip)*.5;
//   double zModuleShift = thickChip-halflength;
//   if(m_front_back==1) zModuleShift*=-1;

//   // Correct rmin rmax values depending on support services
//   int nbSvcSupport = ringHelper.getNbSupport(m_layer);
//   for(int iSvc=0; iSvc<nbSvcSupport; iSvc++){
//     double rminSvc = ringHelper.getRingSupportRMin(iSvc);
//     double rmaxSvc = ringHelper.getRingSupportRMax(iSvc);
//     rmin = std::min(rmin,rminSvc);
//     rmax = std::max(rmax,rmaxSvc);
//     }

  m_ringRMin = rmin-0.001;
  m_ringRMax = rmax+0.001;
  m_ringZMin = -thickHyb;
  m_ringZMax = thickChip;
  if(m_front_back==-1){
    m_ringZMin = -thickChip;
    m_ringZMax = thickHyb;
  }
  
  m_ringZMin-=0.001;
  m_ringZMax+=0.001;

  // In case the sensor length is smaller than the module length...
  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = m_radius + ringModule->getModuleSensorLength()*.5;
  double moduleHalfLength = ringModule->Length()*.5;
  rmin = moduleRadius-moduleHalfLength;
  
  if(rmin<m_ringRMin) m_ringRMin = rmin-0.001;
    
//   m_ringZmin = -m_halfLength; 
//   m_ringZMax = m_halfLength; 
  m_ringZShift = (thickChip-m_halfLength);

  m_ringId = 2*m_ring;
  if(m_front_back==-1) m_ringId++;

  m_bPrebuild = true;  
}

GeoVPhysVol* GeoPixelRingECRingRef::Build(const PixelGeoBuilderBasics* basics, int endcapSide ) 
{
 // Check that the prebuild phase is done
  if(!m_bPrebuild)preBuild(basics); 

  // Open link to the xml file that describes the rings
  PixelRingSupportXMLHelper ringHelper;

  // Get ringmodule from PixelModuelSvc
  GeoDetModulePixel* ringModule = m_pixelModuleSvc->getModule(basics,2,m_layer,m_moduleType);
  InDetDD::PixelModuleDesign* ringModuleDesign = m_pixelDesignSvc->getDesign(basics,m_moduleType);

  double zModuleShift = m_ringZShift;
  double halflength = m_halfLength+.001;
  if(m_front_back==1) zModuleShift*=-1;

  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = m_radius + ringModule->getModuleSensorLength()*.5;

  // build ring envelope
  const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
  std::ostringstream logStr; 
  logStr << "ringLog" << ((m_front_back==1)?"F":"B");
  const GeoTube* ringTube = new GeoTube(m_ringRMin,m_ringRMax,halflength);
  GeoLogVol* _ringLog = new GeoLogVol(logStr.str(),ringTube,air);

  GeoFullPhysVol* ringPhys = new GeoFullPhysVol(_ringLog);

  int nmodules = m_numModules;

  // in case no modules are defined for the ring
  if(nmodules==0) return ringPhys;

  // deltaPhi is angle between two adjacent modules regardless of side of the Layer
  double deltaPhi = 360.*CLHEP::deg / (double)nmodules;

  // This is the start angle of the even modules
  // Start angle could eventually come from the database...
  // double startAngle = gmt_mgr->PixelECStartPhi();
  double startAngle = m_phiOffset;
  
  // -1 means near IP (back), +1 means away from IP (front), 0 means alternate
  //  int ringSide = m_ringSide;
  // double stagger = m_zOffset;
  
  int endcapId = (endcapSide>0)? 2 : -2;
  
  // loop over modules
  for (int imod=0; imod<nmodules; imod++) {

    //     // If ringside == 0 then we only build even modules if front side and odd modules if back side
    //     if (!ringSide && gmt_mgr->isLayerFront() &&   imod%2 )    continue; // skip odd module  
    //     if (!ringSide && gmt_mgr->isLayerBack()  && !(imod%2)) continue ;  // skip even module
    
    // Fi stagger is non zero, alternating modules are stagger low/high.
    double local_front_back = (imod%2 ? -1 : 1);
    // double zpos = 0.5*stagger * (imod%2 ? -1 : 1);
    
    if(m_front_back==local_front_back) {
      
      // zpos = 0.;
      //
      // Build both endcaps the same but re-number phiId in endcap C to
      // get correct offline numbering.  Endcap C is obtained by
      // rotating endcap A by 180 around y axis.
      //
      // Numbering goes in opposite direction with module 0 becoming 23
      // (current layout).  E.g. mapping is 0<->23, 24<->47.
      //
      //current layout:       int phiId = (gmt_mgr->GetSide()>0) ? imod : (3*nmodules-imod-1)%(2*nmodules);
      //     int phiId = gmt_mgr->isAside() ? imod : (2*nmodules-imod-1)%(nmodules);     SES FIXME
      //     gmt_mgr->SetPhi( phiId );
      
      int phiId = endcapSide>0 ? imod : (2*nmodules-imod-1)%(nmodules);     
      double angle = imod*deltaPhi+startAngle;
      
      //     cout<<"GPRing: Layer="<< iLayer<<" ring="<< iring <<" nmods="<< nmodules
      // 	<<", imod="<< imod <<", phiId="<< phiId
      // 	<<", angle="<< angle
      // 	<<" LayerSide="<< (gmt_mgr->isAside() ? 'A' : 'C')
      // 	<<" LayerFace="<< (gmt_mgr->isLayerFront()?"front":"back")
      // 	<< endl;
      
      // Rotate the module to switch to a radial plane
      HepGeom::Transform3D initModule = HepGeom::RotateY3D(90.*CLHEP::deg);
      
      //      if( m_ringSide<0 ||( m_ringSide==0&&(imod%2>0)) ) rm.rotateX(180.*CLHEP::deg); // depth axis points towards Layer.
      if(m_front_back==1 || m_ringSide<0 ) initModule = HepGeom::RotateX3D(180.*CLHEP::deg) * initModule;
      
      HepGeom::Point3D<double> modulePos(moduleRadius*cos(angle),moduleRadius*sin(angle),zModuleShift);
      HepGeom::Transform3D moduleTrf = HepGeom::Translate3D(modulePos)*HepGeom::RotateZ3D(angle)*initModule; 
      
      // Build and place the module - 600 is the new geoTagId for ITk endcap ring sensors
      std::ostringstream modName; 
      modName<<"_"<<2*endcapSide<<"_"<<m_layer<<"_"<<phiId<<"_"<<m_ring;
      GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrf);
      GeoFullPhysVol * modulePhys = ringModule->Build(endcapId, m_layer, phiId, m_ringId, 600, modName.str());
      std::ostringstream ostr; 
      ostr << "Layer" << m_layer << "_Sector" << m_ringId;
      ringPhys->add(new GeoNameTag(ostr.str()));
      ringPhys->add(new GeoIdentifierTag(phiId));
      ringPhys->add(xform);
      ringPhys->add(modulePhys);
      
      // Build and store DetectorElement
      Identifier idwafer = basics->getIdHelper()->wafer_id(endcapId, m_layer, phiId, m_ring);
      InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, ringModuleDesign, 
									    modulePhys, basics->getCommonItems());
      basics->getDetectorManager()->addDetectorElement(element);
      
      // Now store the xform by identifier: alignement studies
      basics->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);
    }
  }

  return ringPhys;
}


double GeoPixelRingECRingRef::ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth) 
{

  double xCorner = moduleWidth*.5;
  double yCorner = rMin + moduleLength;
  
  double ringRmax = sqrt(xCorner*xCorner+yCorner*yCorner);
  return ringRmax + std::abs(safety);

}
