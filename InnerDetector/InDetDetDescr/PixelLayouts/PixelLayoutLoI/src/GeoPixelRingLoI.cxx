/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutLoI/GeoPixelRingLoI.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 

/*#include "TrkDetDescrTools/XMLReaderSvc.h"
  #include "TrkDetDescrTools/TrkGeoTemplates.h"*/
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

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

#include <sstream>


GeoPixelRingLoI::GeoPixelRingLoI(const PixelGeoBuilderBasics* basics,
				 int endcapSide, int iDisk, int iRing, double ringRadius, double zOffset, int ringSide, 
				 int numModules, std::string moduleType,
				 int front_back)
  : PixelGeoBuilder(basics),
    m_endcapSide(endcapSide),
    m_disk(iDisk),
    m_ring(iRing),
    m_radius(ringRadius),
    m_zOffset(zOffset),
    m_ringSide(ringSide),
    m_numModules(numModules),
    m_moduleType(moduleType),
    m_front_back(front_back),
    m_pixelModuleSvc("PixelModuleSvc","PixelModuleSvc"),
    m_pixelDesignSvc("PixelDesignSvc","PixelDesignSvc"),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")

{
  preBuild();

}

GeoPixelRingLoI::~GeoPixelRingLoI()
{
}

void GeoPixelRingLoI::preBuild()
{

  // Check if the envelope is defined as a tool and available in ToolSvc
  StatusCode sc = m_pixelModuleSvc.retrieve();
  if(sc.isFailure())
    {
      std::cout << "Could not retrieve pixel module builder tool " <<  m_pixelModuleSvc << ",  some services will not be built." << std::endl;
    } 
  else 
    {
      std::cout << "Pixel module builder tool retrieved: " << m_pixelModuleSvc << std::endl;
    }

}

GeoVPhysVol* GeoPixelRingLoI::Build( ) 
{

  // Retrieve ring module from PixelModuleService
  GeoDetModulePixel* ringModule = m_pixelModuleSvc->getModule(getBasics(),2,m_disk,m_moduleType);
  InDetDD::PixelModuleDesign* ringModuleDesign = m_pixelDesignSvc->getDesign(getBasics(),m_moduleType);

  double rmin = m_radius;
  double rmax = ComputeRMax(rmin, 0.01, ringModule->Length(), ringModule->Width());

  double halflength = ringModule->Thickness() +0.01;

  // Build ring envelope
  const GeoMaterial* air = matMgr()->getMaterial("std::Air");

  const GeoTube* ringTube = new GeoTube(rmin,rmax,halflength);
  GeoLogVol* _ringLog = new GeoLogVol("ringLog",ringTube,air);
  GeoFullPhysVol* ringPhys = new GeoFullPhysVol(_ringLog);

  // Module number
  int nmodules = m_numModules;

  // in case no modules are defined for the ring
  if(nmodules==0) return ringPhys;

  // deltaPhi is angle between two adjacent modules regardless of side of the disk
  double deltaPhi = 360.*CLHEP::deg / (double)nmodules;

  // This is the start angle of the even modules
  // Start angle could eventually come from the database...
  // double startAngle = gmt_mgr->PixelECStartPhi();
  double startAngle = -deltaPhi/2.;

  // This is the radius of the center of the active sensor (also center of the module)
  double moduleRadius = rmin + ringModule->Length()*.5;

  // -1 means near IP (back), +1 means away from IP (front), 0 means alternate
  //  int ringSide = m_ringSide;
  double stagger = m_zOffset;
  
  int endcapId = (m_endcapSide>0)? 2 : -2;

   // loop over modules
  for (int imod=0; imod<nmodules; imod++) {

//     // If ringside == 0 then we only build even modules if front side and odd modules if back side
//     if (!ringSide && gmt_mgr->isDiskFront() &&   imod%2 )    continue; // skip odd module  
//     if (!ringSide && gmt_mgr->isDiskBack()  && !(imod%2)) continue ;  // skip even module
    
    // Fi stagger is non zero, alternating modules are stagger low/high.
    double local_front_back = (imod%2 ? 1 : -1);
    double zpos = 0.5*stagger * (imod%2 ? 1 : -1);
    
    zpos=0.;
    
    if(m_front_back==local_front_back) {

      int phiId = m_endcapSide<0 ? imod : (2*nmodules-imod-1)%(nmodules);         
      double angle = startAngle-imod*deltaPhi;
      
      //     cout<<"GPRing: disk="<< idisk<<" ring="<< iring <<" nmods="<< nmodules
      // 	<<", imod="<< imod <<", phiId="<< phiId
      // 	<<", angle="<< angle
      // 	<<" diskSide="<< (gmt_mgr->isAside() ? 'A' : 'C')
      // 	<<" diskFace="<< (gmt_mgr->isDiskFront()?"front":"back")
      // 	<< endl;
      
      CLHEP::HepRotation rm;
      rm.rotateY(90*CLHEP::deg);
      if( m_ringSide>0 ) rm.rotateX(180.*CLHEP::deg); // depth axis points towards disk.
      
      rm.rotateZ(angle);
      CLHEP::Hep3Vector pos(moduleRadius,0,zpos);
      pos.rotateZ(angle);
      GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::Transform3D(rm,pos));
      
      // Build the module
      GeoFullPhysVol * modulePhys = ringModule->Build(endcapId, m_disk, phiId, m_ring);
      std::ostringstream ostr; 
      ostr << "Disk" << m_disk << "_Sector" << m_ring;
      ringPhys->add(new GeoNameTag(ostr.str()));
      ringPhys->add(new GeoIdentifierTag(phiId));
      ringPhys->add(xform);
      ringPhys->add(modulePhys);
      
      // Register the module as SiDetectorElement
      Identifier idwafer = getBasics()->getIdHelper()->wafer_id(endcapId, m_disk, phiId, m_ring);
      InDetDD::SiDetectorElement* element = new InDetDD::SiDetectorElement( idwafer, ringModuleDesign, 
									    modulePhys, getBasics()->getCommonItems());
      getBasics()->getDetectorManager()->addDetectorElement(element);
      
      // Now store the xform by identifier:
      getBasics()->getDetectorManager()->addAlignableTransform(0,idwafer,xform,modulePhys);
    }
  }

  // Usual minmax stuff...
  m_ringRMin = rmin-0.01;
  m_ringRMax = rmax+0.01;
  m_ringZMin = -halflength-0.01;
  m_ringZMax = halflength+0.01;

  std::cout<<"Ring minmax : "<<m_ringRMin<<" "<<m_ringRMax<<" / "<<m_ringZMin<<" "<<m_ringZMax<<std::endl;

  return ringPhys;
}


double GeoPixelRingLoI::ComputeRMax(double rMin, double safety, double moduleLength, double moduleWidth) 
{
  //
  // ringRmax (OB) is the longest side of the triangle OAB, formed by
  // rinRmin (OA) and the line AB joining the center of the inner edge
  // of the module (A) to any of the two outer vertices of the module
  // (B).  Point O is the center of the ring.
  //
  // Default safety value is 0.01mm, but for exact calculations, safety=0 must be forced
  // modL,modW,modD are module length,width

  //  I can't see what's wrong here...
  double modL = moduleLength;
  double modW = moduleWidth;
  double distAB_sqr = modL*modL + modW*modW/4.;
  double distAB = sqrt(distAB_sqr);
  double distOA = rMin;
  double costheta = modL/distAB;
  double distOB_sqr = distOA*distOA + distAB_sqr + 2*distOA*distAB*costheta;
  double ringRmax = sqrt(distOB_sqr);
  return ringRmax + std::abs(safety);
}
