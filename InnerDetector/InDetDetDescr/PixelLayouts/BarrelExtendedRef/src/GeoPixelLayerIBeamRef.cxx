/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelExtendedRef/GeoPixelLayerIBeamRef.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoTransform.h"

#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "PathResolver/PathResolver.h"

#include <sstream>

GeoPixelLayerIBeamRef::GeoPixelLayerIBeamRef(const PixelGeoBuilderBasics* basics, int iLayer, std::string mode):
  PixelGeoBuilder(basics),								     
  m_layer(iLayer),
  m_mode(mode),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  m_physVol = Build();
}

GeoVPhysVol* GeoPixelLayerIBeamRef::Build() {


  InDet::BarrelLayerTmp *layerTmpIn = m_mode=="inner" ? m_xmlReader->getPixelBarrelLayerTemplate(m_layer):
                                                        m_xmlReader->getPixelBarrelLayerTemplate(m_layer-1)  ;
  InDet::BarrelLayerTmp *layerTmpOut = m_mode=="outer" ? m_xmlReader->getPixelBarrelLayerTemplate(m_layer):
                                                         m_xmlReader->getPixelBarrelLayerTemplate(m_layer+1);

  InDet::BarrelLayerTmp *layerTmp = m_mode=="inner" ? layerTmpIn : layerTmpOut;

  // retrieve stave template for inner layer
  std::string layerName      = layerTmp->name;
  int    nSectors            = layerTmp->stave_n; 
  double ladderTilt          = layerTmp->stave_tilt;
  std::string staveType      = layerTmp->stave_type;
  double layerRadius         = layerTmp->radius;

  double layerRadiusIn       = layerTmpIn->radius;
  double layerRadiusOut      = layerTmpOut->radius;

  double phiOfStaveZero = 0.;

  ATH_MSG_DEBUG("IBeam layer:" <<layerName<<" "<<staveType<<" "<<nSectors);

   
  double deltaPhi = 360.*CLHEP::deg/(double)nSectors;

  HepGeom::Transform3D transRadiusAndTilt = HepGeom::TranslateX3D(layerRadius)*HepGeom::RotateZ3D(ladderTilt);
  HepGeom::Transform3D transRadius = HepGeom::TranslateX3D(layerRadius);

  std::vector<InDet::StaveTmp*> staveTmp = m_xmlReader->getPixelStaveTemplate(m_layer);

  // define ladders for stave and modules
  GeoPixelLadderIBeamRef pixelLadder(getBasics(), staveTmp[0], m_layer, transRadiusAndTilt,m_mode); 
  std::pair<double,double> lim = ComputeLayerThickness(pixelLadder, ladderTilt, layerRadius);

  // Register the number of stave defined for the layer
  getBasics()->getDetectorManager()->numerology().setNumPhiModulesForLayer(m_layer,nSectors);

  GeoFullPhysVol* layerPhys = 0;

  double safety = 0.01 * CLHEP::mm;
  double rmin =  layerRadius - lim.first - safety;
  double rmax =  layerRadius + lim.second  + safety;

  double rmid = 0.5*( layerRadiusIn + layerRadiusOut );

  if (m_mode=="inner") rmax = rmid;
  else                 rmin = rmid;

  double ladderLength = pixelLadder.envLength() + 4*epsilon(); // Ladder has length gmt_mgr->PixelLadderLength() +  2*m_epsilon

  // the layer envelope
  const GeoMaterial* air = matMgr()->getMaterial("std::Air");
  std::ostringstream lname;
  lname << "Layer" << m_layer;
  const GeoTube* layerTube = new GeoTube(rmin,rmax,0.5*ladderLength); //solid
  const GeoLogVol* layerLog = new GeoLogVol(lname.str(),layerTube,air); //log volume
  layerPhys = new GeoFullPhysVol(layerLog); // phys vol

  // Loop over the sectors and place everything
  //
  for(int ii = 0; ii < nSectors; ii++) {
    //    gmt_mgr->SetPhi(ii);

    // Build ladder taking as input maximal/minimal extension of layer services
    double rSize = layerRadiusOut-layerRadiusIn;
    pixelLadder.setSector(ii);
    pixelLadder.setRsize(rSize);
    pixelLadder.setRmid(rmid);
    pixelLadder.setTilt(ladderTilt);
    GeoVPhysVol *ladderPhys=pixelLadder.Build();
    GeoVPhysVol *ladderSvc =pixelLadder.getSvcVol();  

    double phiOfSector = phiOfStaveZero + ii*deltaPhi;

    // transform equiv  RotZ(phiOfSector)*TransZ(layerRadius)*RotZ(tilt)
    HepGeom::Transform3D ladderTransform = HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTilt;

    //
    // Place the active ladders
    //
    std::ostringstream nameTag; 
    nameTag << "Ladder" << ii;
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoTransform* xform = new GeoTransform(ladderTransform);
    layerPhys->add(tag);
    layerPhys->add(new GeoIdentifierTag(ii) );
    layerPhys->add(xform);
    layerPhys->add(ladderPhys);   //pixelLadder->Build());

    // Place services
    HepGeom::Transform3D svcTransform = HepGeom::RotateZ3D(phiOfSector) * transRadius;

    std::ostringstream svcTag; 
    svcTag << "Services" << ii;
    GeoNameTag * nSvc = new GeoNameTag(svcTag.str());
    GeoTransform* xSvc = new GeoTransform(svcTransform);
    layerPhys->add(nSvc);
    layerPhys->add(new GeoIdentifierTag(ii) );
    layerPhys->add(xSvc);
    layerPhys->add(ladderSvc);   //pixelLadder->getSvcVol());
  }

  return layerPhys;

}


std::pair<double,double> GeoPixelLayerIBeamRef::ComputeLayerThickness(const GeoPixelLadderIBeamRef& pixelLadder, double ladderTilt, double layerRadius)
{

  //
  // Calculate layerThicknessN: Thickness from layer radius to min radius of envelope
  // Calculate layerThicknessP: Thickness from layer radius to max radius of envelope
  //      
  double ladderHalfThickN = pixelLadder.thicknessN();
  double ladderHalfThickP = pixelLadder.thicknessP();
  double ladderHalfWidth = pixelLadder.width()/2;
  ATH_MSG_DEBUG("ladderHalfThickN, ladderHalfThickP, ladderHalfWidth, ladderTilt, layerRadius:"
	    << ladderHalfThickN << " "<< ladderHalfThickP << " " << ladderHalfWidth 
		<< " " << ladderTilt << " " << layerRadius);
  
  // Calculate distance of closest approach to a line following the surface of the ladder.
  double grad = -1/tan(std::abs(ladderTilt)); // Gradient of this line.
  // x1, y1 is the point on the center of ladder surface.
  double y1 = -ladderHalfThickN*sin(std::abs(ladderTilt));
  double x1 = -ladderHalfThickN*cos(std::abs(ladderTilt))+layerRadius;
  // x1, y2 is the point of closest approach.
  double y2 = (y1 - grad*x1)/(1+grad*grad);
  double x2 = -grad*y2;
  // dist is the distance between these two points. If this is further than the width of the ladder we 
  // can use the corner of the ladder.
  double distToClosestPoint = sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
  // distance of closest approach.
  double radClosest = (y1 - grad*x1)/(sqrt(1+grad*grad));

  //std::cout << "Distance of closest approach: " << radClosest << std::endl;
  //std::cout << "Distance along ladder surface from center to point of closest approach: " <<  distToClosestPoint << std::endl;

  // Calculate the radius of the corners of the ladder.
  HepGeom::Point3D<double> ladderLowerCorner(-ladderHalfThickN, ladderHalfWidth, 0);
  HepGeom::Point3D<double> ladderUpperCorner(ladderHalfThickP, -ladderHalfWidth, 0);
  ladderLowerCorner = HepGeom::TranslateX3D(layerRadius) * HepGeom::RotateZ3D(std::abs(ladderTilt)) * ladderLowerCorner;
  ladderUpperCorner = HepGeom::TranslateX3D(layerRadius) * HepGeom::RotateZ3D(std::abs(ladderTilt)) * ladderUpperCorner;

  std::pair<double,double> limits(layerRadius - ladderLowerCorner.perp(),
				  ladderUpperCorner.perp() - layerRadius);

  // If distance of closest approach is within the ladder width we use that instead
  if (distToClosestPoint < ladderHalfWidth) {
    //m_layerThicknessN = layerRadius - radClosest;
    limits.first = layerRadius - radClosest;
  }

  return limits;
}

