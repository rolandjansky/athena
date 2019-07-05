/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EndcapRingRef/GeoPixelRingECRingRef.h"
#include "EndcapRingRef/PixelRingSupportXMLHelper.h"

#include "PixelGeoModelModule/GeoDetModulePixel.h"
#include "PixelGeoModel/PixelGeoBuilder.h"

#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h"

#include "PathResolver/PathResolver.h"

#include <sstream>

#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"


GeoPixelRingECRingRef::GeoPixelRingECRingRef(int iLayer, int iRing, double ringRadius, double ringOuterRadius,
					     double zOffset, double phiOffset,
					     int ringSide, int numModules, std::string moduleType, 
					     int diskId, int back_front, SplitMode mode)
  : m_layer(iLayer),
    m_ring(iRing),
    m_ringId(-1),
    m_mode(mode),
    m_radius(ringRadius),
    m_outerRadius(ringOuterRadius),
    m_zOffset(zOffset),
    m_phiOffset(phiOffset),
    m_ringSide(ringSide),
    m_numModules(numModules),
    m_moduleType(moduleType),
    m_diskId(diskId),
    m_front_back(back_front),
    m_readoutRegion("EC"),
    m_readoutLayer(iLayer),
    m_readoutEta(iRing),
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
    basics->msgStream()<<MSG::WARNING<< "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << endreq;

  m_pixelModuleSvc->initModuleMap(basics);
  m_pixelDesignSvc->initModuleMap(basics);

  // Open link to the xml file that describes the rings
  PixelRingSupportXMLHelper ringHelper(basics);

  // Get ringmodule from PixelModuelSvc
  GeoDetModulePixel* ringModule = m_pixelModuleSvc->getModule(basics,2,m_layer,m_moduleType);

  // Compute module radial min max size
  double rmin = m_radius;
  double rmax = ComputeRMax(rmin, 0.01, ringModule->Length(), ringModule->Width());
  // ... module thicknesses
  double thickHyb = ringModule->ThicknessN();
  double thickChip = ringModule->ThicknessP();

  m_halfLength = ringModule->Thickness()/2.;

  m_ringRMin = rmin-0.001;
  m_ringRMax = rmax+0.001;
  
  m_ringZMin = -std::max(thickHyb,thickChip) - 0.001;
  m_ringZMax = std::max(thickHyb,thickChip) + 0.001;

  // In case the sensor length is smaller than the module length...
  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = m_radius + ringModule->getModuleSensorLength()*.5;
  double moduleHalfLength = ringModule->Length()*.5;
  rmin = moduleRadius-moduleHalfLength;
  
  if(rmin<m_ringRMin) m_ringRMin = rmin-0.001;
  
  m_ringZShift = (thickChip-m_halfLength);

  // coding scheme 300
  //m_ringId = 2*m_ring;
  //if(m_front_back==-1) m_ringId++;
  // coding scheme 600
  m_ringId = m_ring;

  m_bPrebuild = true;  
}

void GeoPixelRingECRingRef::readoutId(std::string region, int lay, int etaIndex )
{
  m_readoutRegion = region;
  m_readoutLayer = lay;
  m_readoutEta = etaIndex;
}

GeoVPhysVol* GeoPixelRingECRingRef::Build(const PixelGeoBuilderBasics* basics, int endcapSide )
{
 // Check that the prebuild phase is done
  if(!m_bPrebuild) preBuild(basics);

  // Open link to the xml file that describes the rings
  PixelRingSupportXMLHelper ringHelper(basics);

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
  int endcapId = (endcapSide>0)? 2 : -2;
 
  // loop over modules
  for (int imod=0; imod<nmodules; imod++) {
    // Fi stagger is non zero, alternating modules are stagger low/high.
    double local_front_back = (imod%2 ? -1 : 1);
    if(m_front_back==local_front_back) {
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
      
      double phi = angle;
      if (phi> CLHEP::pi*CLHEP::rad) phi -= 2*CLHEP::pi*CLHEP::rad;
      if (phi<-CLHEP::pi*CLHEP::rad) phi += 2*CLHEP::pi*CLHEP::rad;
     
   
      // Rotate the module to switch to a radial plane
      HepGeom::Transform3D initModule = HepGeom::RotateY3D(90.*CLHEP::deg);
      if(m_front_back==1 || m_ringSide<0 ) initModule = HepGeom::RotateX3D(180.*CLHEP::deg) * initModule;

      // synchronize the sim/reco identification
      if (m_readoutRegion=="B" ) endcapId= endcapSide>0 ? 1 : -1;   
      // coding scheme 300 
      //m_ringId = 2*m_readoutEta;
      //if(m_front_back==-1) m_ringId++;
      // coding scheme 600
      m_ringId = m_readoutEta;      

      HepGeom::Point3D<double> modulePos(moduleRadius*cos(phi),moduleRadius*sin(phi),zModuleShift);
      HepGeom::Transform3D moduleTrf = HepGeom::Translate3D(modulePos)*HepGeom::RotateZ3D(phi)*initModule;
     
      // Build and place the module - 600 is the new geoTagId for ITk endcap ring sensors
      std::ostringstream modName;
      modName<<"_"<<2*endcapSide<<"_"<<m_readoutLayer<<"_"<<phiId<<"_"<<m_ringId;
      GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrf);
      GeoFullPhysVol * modulePhys = ringModule->Build(endcapId, m_readoutLayer, phiId, m_ringId, 600, modName.str());

      std::ostringstream ostr;
      ostr << "Layer" << m_readoutLayer << "_Sector" << m_ringId;
      ringPhys->add(new GeoNameTag(ostr.str()));
      ringPhys->add(new GeoIdentifierTag(phiId));
      ringPhys->add(xform);
      ringPhys->add(modulePhys);

      // Build and store DetectorElement
      Identifier idwafer = basics->getIdHelper()->wafer_id(endcapId, m_readoutLayer, phiId, m_readoutEta);
      InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, ringModuleDesign,
                                                                            modulePhys, basics->getCommonItems());
      basics->getDetectorManager()->addDetectorElement(element);
    
      // Now store the xform by identifier: alignement studies
      basics->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);
    }
  }

  return ringPhys;
}

std::pair<GeoVPhysVol*,GeoVPhysVol*> GeoPixelRingECRingRef::BuildSplit(const PixelGeoBuilderBasics* basics, int endcapSide ) 
{
  GeoVPhysVol* _np = NULL;
  std::pair<GeoVPhysVol*,GeoVPhysVol*> halfRingsAwayTwdBS = std::make_pair(_np,_np);
    
 // Check that the prebuild phase is done
  if(!m_bPrebuild)preBuild(basics); 

  // Open link to the xml file that describes the rings
  PixelRingSupportXMLHelper ringHelper(basics);

  // Get ringmodule from PixelModuelSvc
  GeoDetModulePixel* ringModule = m_pixelModuleSvc->getModule(basics,2,m_layer,m_moduleType);
  InDetDD::PixelModuleDesign* ringModuleDesign = m_pixelDesignSvc->getDesign(basics,m_moduleType);

  double halflength = m_halfLength+.001;

  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = m_radius + ringModule->getModuleSensorLength()*.5;

  int nmodules = m_numModules;

  // in case no modules are defined for the ring
  if(nmodules==0) return halfRingsAwayTwdBS;

  // deltaPhi is angle between two adjacent modules regardless of side of the Layer
  double deltaPhi = 360.*CLHEP::deg / (double)nmodules;

  // This is the start angle of the even modules
  // Start angle could eventually come from the database...
  // double startAngle = gmt_mgr->PixelECStartPhi();
  double startAngle = m_phiOffset;
  
  // build ring envelope
  const GeoMaterial* air = basics->matMgr()->getMaterial("std::Air");
  std::ostringstream logStr; 
  logStr << "ringLog" << ((m_front_back==1)?"F":"B");
  //
  // half-rings with z-gap for services
  // constructor params: GeoTubs* tubs = new GeoTubs(rmin,rmax,halfthick, startangle,delta_angle);
  // to accomodate for services, this half-ring will get a z-shift toward beam-spot
  // the geometry coordinates have pi/2 = 1/4 ring angle offset wrt ATLAS ref. frame
  // to make sure corners of off-set modules are within the envelope, we add the extension in phi
  // from the center of the module to the bottom corner.
  double moduleHwidth = 0.5*ringModule->Width();
  double phiAtBottomCorner = atan(moduleHwidth/m_radius);
  // this is the extension in phi of each half tube
  double ExtensionPhi= deltaPhi*(nmodules*0.5 - 1.) + 2.*phiAtBottomCorner;
  
  // here we define the starting points (in phi) of the two half tubes
  // the startAngle is the phi of the first element in the ring (defined as offset in the XML)
  double phiHalfRingTwdsStart =  (m_mode==NONE) ? startAngle : startAngle - phiAtBottomCorner;
  double phiHalfRingAwayStart =  phiHalfRingTwdsStart + CLHEP::pi*CLHEP::rad;  
        
  // to accomodate for services, this half-ring will get a z-shift toward beam-spot  
  const GeoTubs* ringTubsTwdBS = new GeoTubs(m_ringRMin,m_ringRMax,halflength, phiHalfRingTwdsStart,ExtensionPhi);
  // to accomodate for services, this half-ring will get a z-shift away from beam-spot
  const GeoTubs* ringTubsAwayBS = new GeoTubs(m_ringRMin,m_ringRMax,halflength, phiHalfRingAwayStart,ExtensionPhi);

  std::ostringstream logStrTwdBS;
  logStrTwdBS << logStr.str() << "TwdBS";
  GeoLogVol* _ringLogTwdBS = new GeoLogVol(logStrTwdBS.str(),ringTubsTwdBS,air);

  std::ostringstream logStrAwayBS;
  logStrAwayBS << logStr.str() << "AwayBS";
  GeoLogVol* _ringLogAwayBS = new GeoLogVol(logStrAwayBS.str(),ringTubsAwayBS,air);
    
  GeoFullPhysVol* ringPhysTwdBS = new GeoFullPhysVol(_ringLogTwdBS);
  GeoFullPhysVol* ringPhysAwayBS = new GeoFullPhysVol(_ringLogAwayBS);
  halfRingsAwayTwdBS.first = ringPhysTwdBS;
  halfRingsAwayTwdBS.second = ringPhysAwayBS;
  
  // -1 means near IP (back), +1 means away from IP (front), 0 means alternate
  int endcapId = (endcapSide>0)? 2 : -2;
  
  // loop over modules
  for (int imod=0; imod<nmodules; imod++) {
    // Fi stagger is non zero, alternating modules are stagger low/high.
    double local_front_back = (imod%2 ? -1 : 1);
    if(m_front_back==local_front_back) {
      //
      // Build both endcaps the same but re-number phiId in endcap C to
      // get correct offline numbering.  Endcap C is obtained by
      // rotating endcap A by 180 around y axis.
      //
      int phiId = endcapSide>0 ? imod : (2*nmodules-imod-1)%(nmodules);
      double angle = double(imod)*deltaPhi+startAngle;
    
      // check if the module belongs to the TwdBS side or AwayBS side
      bool isTwd = ( angle < (startAngle+CLHEP::pi*CLHEP::rad)  && angle>=(startAngle)) ? true : false;
      
      // Rotate the module to switch to a radial plane
      HepGeom::Transform3D initModule = HepGeom::RotateY3D(90.*CLHEP::deg);

      double zModuleShift = m_ringZShift;      
      if(m_front_back==1 || m_ringSide<0 ) {
	initModule = HepGeom::RotateX3D(180.*CLHEP::deg) * initModule;
	zModuleShift *= -1;
      }

      bool rotate = false;
      if ((nmodules/2)%2==0 and (m_mode==MIDDLE or m_mode==GOOD) and not isTwd)
	rotate = true;
      else if ((nmodules/2)%2==1 and ((m_mode==MIDDLE and isTwd) or m_mode==GOOD))
	rotate = true;
      
      if (rotate) {
	initModule = HepGeom::RotateX3D(180.*CLHEP::deg) * initModule;
	zModuleShift *= -1;
      }
      
      double phi = angle;
      if (phi> CLHEP::pi*CLHEP::rad) phi -= 2*CLHEP::pi*CLHEP::rad;
      if (phi<-CLHEP::pi*CLHEP::rad) phi += 2*CLHEP::pi*CLHEP::rad;
      
      HepGeom::Point3D<double> modulePos(moduleRadius*cos(phi),moduleRadius*sin(phi),zModuleShift);
      HepGeom::Transform3D moduleTrf = HepGeom::Translate3D(modulePos)*HepGeom::RotateZ3D(phi)*initModule;  
    
      // synchronize the sim/reco identification
      if (m_readoutRegion=="B" ) endcapId= endcapSide>0 ? 1 : -1;   
      // coding scheme 300
      //m_ringId = 2*m_readoutEta;
      //if(m_front_back==-1) m_ringId++;

      // coding scheme 600
      m_ringId = m_readoutEta;  

      // Build and place the module - 600 is the new geoTagId for ITk endcap ring sensors
      std::ostringstream modName; 
      modName<<"_"<<2*endcapSide<<"_"<<m_readoutLayer<<"_"<<phiId<<"_"<<m_readoutEta;
      GeoAlignableTransform* xform = new GeoAlignableTransform(moduleTrf);
      GeoFullPhysVol * modulePhys = ringModule->Build(endcapId, m_readoutLayer, phiId, m_ringId, 600, modName.str());
      std::ostringstream ostr; 
      ostr << "Layer" << m_readoutLayer << "_Sector" << m_ringId;
      
      if (isTwd) {
	ringPhysTwdBS->add(new GeoNameTag(ostr.str()));
	ringPhysTwdBS->add(new GeoIdentifierTag(phiId));
	ringPhysTwdBS->add(xform);
	ringPhysTwdBS->add(modulePhys);
      }
      else {
	ringPhysAwayBS->add(new GeoNameTag(ostr.str()));
	ringPhysAwayBS->add(new GeoIdentifierTag(phiId));
	ringPhysAwayBS->add(xform);
	ringPhysAwayBS->add(modulePhys);
      }
     
      // Build and store DetectorElement
      Identifier idwafer = basics->getIdHelper()->wafer_id(endcapId, m_readoutLayer, phiId, m_readoutEta);

      InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, ringModuleDesign, 
                    modulePhys, basics->getCommonItems());
      basics->getDetectorManager()->addDetectorElement(element);
    
    // Now store the xform by identifier: alignement studies
      basics->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);
    }
  }

  return halfRingsAwayTwdBS;
}


double GeoPixelRingECRingRef::ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth) 
{
  double xCorner = moduleWidth*.5;
  double yCorner = rMin + moduleLength;
  
  double ringRmax = sqrt(xCorner*xCorner+yCorner*yCorner);
  return ringRmax + std::abs(safety);

}
