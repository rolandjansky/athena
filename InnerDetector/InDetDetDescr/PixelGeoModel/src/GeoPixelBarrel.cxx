/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelBarrel.h"
#include "PixelGeoModel/GeoPixelLayer.h"
#include "PixelGeoModel/GeoPixelServices.h"
#include "PixelGeoModel/GeoPixelOldFrame.h"
#include "PixelGeoModel/GeoPixelStaveRing.h"
#include "PixelGeoModel/GeoPixelIFlexServices.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "InDetIdentifier/PixelID.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"


#include <sstream>

using namespace std;
GeoPixelBarrel::GeoPixelBarrel(const GeoPixelServices * pixServices)
  : m_pixServices(pixServices)
{}

GeoVPhysVol* GeoPixelBarrel::Build( ) {

  //
  // create the Barrel Mother volume
  // 
  double rmin = gmt_mgr->PixelBarrelRMin();
  double rmax = gmt_mgr->PixelBarrelRMax();
  double halflength = gmt_mgr->PixelBarrelHalfLength();
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* barrelTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* barrelLog = new GeoLogVol("Barrel",barrelTube,air);
  GeoFullPhysVol* barrelPhys = new GeoFullPhysVol(barrelLog);

  // Set numerology
  DDmgr->numerology().setNumLayers(gmt_mgr->PixelBarrelNLayer());

  // In case of IBL stave detailed description -> add stave ring support and emdblocks
  bool bAddIBLStaveRings=false;
  gmt_mgr->SetCurrentLD(0);
  if(gmt_mgr->ibl()&&gmt_mgr->PixelStaveLayout()>3&&gmt_mgr->PixelStaveLayout()<7)
    {
      bAddIBLStaveRings=true;
    }

  //
  // Build the layers inside
  //
  GeoPixelLayer layer;
  for(int ii = 0; ii < gmt_mgr->PixelBarrelNLayer(); ii++){
    //cout << "Layer" << ii << endl;
    gmt_mgr->SetCurrentLD(ii);
    if(gmt_mgr->isLDPresent()){
      std::ostringstream lname;
      lname << "Layer" << ii;
      //      GeoAlignableTransform * xform = new GeoAlignableTransform(HepGeom::Transform3D()); 

      // IBL layer shift ( 2mm shift issue )
      double layerZshift = gmt_mgr->PixelLayerGlobalShift();
      GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::Transform3D(CLHEP::HepRotation(),CLHEP::Hep3Vector(0.,0.,layerZshift)));

      GeoVPhysVol* layerphys = layer.Build();
      GeoNameTag *tag = new GeoNameTag(lname.str());         
      barrelPhys->add(tag);
      barrelPhys->add(new GeoIdentifierTag(ii));
      barrelPhys->add(xform);
      barrelPhys->add(layerphys);
	  
      // Store the transform (at level 1)
      Identifier id = gmt_mgr->getIdHelper()->wafer_id(0,ii,0,0);
      DDmgr->addAlignableTransform(1, id, xform, layerphys);

      // IBL stave ring service area  ( ring + endblocks + flexes + pipe + ...)
      if(m_pixServices&&bAddIBLStaveRings&&ii==0)
	{
	  // ----------- end of stave services (side A)
	  GeoNameTag * tagSupportA = new GeoNameTag("StaveRingAndEndblocks_A");
	  GeoTransform *xformSupportA = layer.getSupportTrfA();
	  GeoPhysVol *supportPhys_A = layer.getSupportA();
	  barrelPhys->add(tagSupportA);
	  barrelPhys->add(xformSupportA);
	  barrelPhys->add(supportPhys_A);
	  
	  // ----------- end of stave services (side C)
	  GeoNameTag * tagSupportC = new GeoNameTag("StaveRingAndEndblocks_C");
	  GeoTransform *xformSupportC = layer.getSupportTrfC();
	  GeoPhysVol *supportPhys_C =  layer.getSupportC();
	  barrelPhys->add(tagSupportC);
	  barrelPhys->add(xformSupportC);
	  barrelPhys->add(supportPhys_C);

	  // ----------- middle of stave services 
	  if(gmt_mgr->PixelStaveAxe()==0) {
	    GeoNameTag *tagM = new GeoNameTag("Brl0M_StaveRing");         
	    GeoTransform *xformSupportMidRing = layer.getSupportTrfMidRing();
	    GeoVPhysVol *supportPhysMidRing = layer.getSupportMidRing();
	    barrelPhys->add(tagM);
	    barrelPhys->add(xformSupportMidRing);
	    barrelPhys->add(supportPhysMidRing);
	  }
	  
	  // ----------- end of stave PP0 services (insde barrel)
	  
	  if(gmt_mgr->IBLFlexAndWingDefined()){
	    GeoPixelIFlexServices iFlexSrv(0);
	    iFlexSrv.Build();
	    
	    GeoNameTag * tagFlexA = new GeoNameTag("PP0Flex_A");
	    GeoTransform *xformFlexA = iFlexSrv.getSupportTrfA();
	    GeoPhysVol *flexPhys_A = iFlexSrv.getSupportA();
	    barrelPhys->add(tagFlexA);
	    barrelPhys->add(xformFlexA);
	    barrelPhys->add(flexPhys_A);
	    
	    GeoNameTag * tagFlexC = new GeoNameTag("PP0Flex_C");
	    GeoTransform *xformFlexC = iFlexSrv.getSupportTrfC();
	    GeoPhysVol *flexPhys_C = iFlexSrv.getSupportC();
	    barrelPhys->add(tagFlexC);
	    barrelPhys->add(xformFlexC);
	    barrelPhys->add(flexPhys_C);
	  }
	}
    }
    else 
      {
	if(gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) << "Layer " << ii << " not built" << endmsg;
      }
  }
  
  if(m_pixServices) {
    //
    // Add the services inside the barrel volume
    //
    m_pixServices->getBuilder()->buildAndPlace("B", barrelPhys);
    if (m_pixServices->getServMatBuilder()) {
      m_pixServices->getServMatBuilder()->buildAndPlace("B", barrelPhys);
    }
    
    //
    // Add the pixel frame inside the barrel volume
    // In recent versions this taken care of in the general services.
    if (gmt_mgr->oldFrame()) {
      GeoPixelOldFrame frame;
      frame.BuildInBarrel(barrelPhys);      
    }
  }
  
  // Extra Material
  InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
  xMat.add(barrelPhys,"PixelBarrel");
  
  return barrelPhys;
}

