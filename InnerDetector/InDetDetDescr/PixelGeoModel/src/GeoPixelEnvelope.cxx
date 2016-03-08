/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelEnvelope.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "PixelGeoModel/GeoPixelBarrel.h"
#include "PixelGeoModel/GeoPixelEndCap.h"
#include "PixelGeoModel/GeoPixelServices.h"
#include "PixelGeoModel/GeoPixelFrame.h"
#include "PixelGeoModel/GeoPixelOldFrame.h"
#include "PixelGeoModel/GeoPixelIFlexServices.h"
#include "PixelGeoModel/GeoPixelIBLFwdServices.h"
#include "PixelGeoModel/DBM_Det.h"

#include "InDetGeoModelUtils/VolumeBuilder.h"
#include "InDetGeoModelUtils/VolumeSplitterUtils.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"

#include "GeoModelInterfaces/IGeoSubDetTool.h"
#include "GaudiKernel/StatusCode.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"

GeoVPhysVol* GeoPixelEnvelope::Build( ) {
  //
  // Control which parts get built. This is mainly of use for
  // Cosmic runs where the whole detector is not present
  //
  

  bool barrelPresent   = gmt_mgr->partPresent("Barrel");
  bool endcapAPresent  = gmt_mgr->partPresent("EndcapA");
  bool endcapCPresent  = gmt_mgr->partPresent("EndcapC");

   // NB. We apply the part transforms only to the active parts.
  // In general though they should always be zero. It would make more sense to apply
  // a transform to the overl pixel if one wants to move the detector.

  //
  // create the External volume where I'll put all the pixel detector
  // This can either be a simple tube or a more general PCON.
  // 
  const GeoShape * envelopeShape;
  InDetDD::Zone * pixZone = 0; 
  if (gmt_mgr->PixelSimpleEnvelope()) {
    double rmin = gmt_mgr->PixelRMin();
    double rmax = gmt_mgr->PixelRMax();
    double halflength = gmt_mgr->PixelHalfLength();
    envelopeShape = new GeoTube(rmin,rmax,halflength);
    pixZone = new InDetDD::TubeZone("Pixel",-halflength,halflength,rmin,rmax);
  } else {
    GeoPcon* envelopeShapeTmp  = new GeoPcon(0.,2*CLHEP::pi);
    // table contains +ve z values only and envelope is assumed to be symmetric around z.
    int numPlanes = gmt_mgr->PixelEnvelopeNumPlanes();
    for (int i = 0; i < numPlanes * 2; i++) {
      int tableIndex = (i < numPlanes) ?  numPlanes - 1 - i :  i - numPlanes;
      int sign       = (i < numPlanes) ? -1 : 1;
      double z = gmt_mgr->PixelEnvelopeZ(tableIndex) * sign;
      double rmin = gmt_mgr->PixelEnvelopeRMin(tableIndex);
      double rmax = gmt_mgr->PixelEnvelopeRMax(tableIndex);
      envelopeShapeTmp->addPlane(z, rmin, rmax);
    }
    envelopeShape = envelopeShapeTmp;
    pixZone = new InDetDD::PconZone("Pixel",envelopeShapeTmp);
  }
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeShape,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);;

  GeoPixelServices * pixServices = 0;
  if(gmt_mgr->DoServices() ) {
    // Takes ownership of pixZone
    pixServices = new GeoPixelServices(pixZone);
  } else {
    delete pixZone;
  }

  if (barrelPresent) {
    
    DDmgr->numerology().addBarrel(0);
    //
    // Add the Barrel:
    //
    HepGeom::Transform3D barrelTransform = gmt_mgr->partTransform("Barrel");
    
    gmt_mgr->SetBarrel();
    GeoPixelBarrel brl(pixServices);
    GeoNameTag* tag = new GeoNameTag("Barrel");
    GeoVPhysVol* barrelPhys =  brl.Build() ;
    envelopePhys->add(tag);
    envelopePhys->add(new GeoIdentifierTag(0));
    envelopePhys->add(new GeoTransform(barrelTransform));
    envelopePhys->add(barrelPhys );
  }
  //
  // Add the EndCaps
  if (endcapAPresent || endcapCPresent) {
    gmt_mgr->SetEndcap();

    GeoPixelEndCap pec(pixServices);
    double zpos = (gmt_mgr->PixelEndcapZMax()+gmt_mgr->PixelEndcapZMin())/2.;

    // EndCap A
    if (endcapAPresent) {

      DDmgr->numerology().addEndcap(2);
    
      HepGeom::Transform3D endcapATransform = gmt_mgr->partTransform("EndcapA");
      
      gmt_mgr->SetPos();
      GeoTransform* xform = new GeoTransform(endcapATransform * HepGeom::TranslateZ3D(zpos));
      GeoNameTag* tag  = new GeoNameTag("EndCapA");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(2));
      envelopePhys->add(xform);
      envelopePhys->add(pec.Build() );
    }
    
    // EndCap C
    if (endcapCPresent) {

      DDmgr->numerology().addEndcap(-2);
      
      HepGeom::Transform3D endcapCTransform = gmt_mgr->partTransform("EndcapC");
      
      gmt_mgr->SetEndcap();
      gmt_mgr->SetNeg();
      GeoTransform* xform = new GeoTransform(endcapCTransform * HepGeom::TranslateZ3D(-zpos) *  HepGeom::RotateY3D(180*CLHEP::deg));
      GeoNameTag* tag  = new GeoNameTag("EndCapC");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(-2));
      envelopePhys->add(xform);
      envelopePhys->add(pec.Build() );
      
    }
 
    if(gmt_mgr->DoServices() ) {
      // Pixel Frame. In recent versions this taken care of in the general services.
      if (gmt_mgr->oldFrame()) {
	GeoPixelOldFrame frame;
	frame.BuildOutBarrel(envelopePhys);
      }
    }
  }

  
  if(pixServices) {
    //
    // Add the Barrel and endcap services outside the barrel and endcap volume
    //
    pixServices->getBuilder()->buildAndPlace("Pixel", envelopePhys);
    if (pixServices->getServMatBuilder()) {
      pixServices->getServMatBuilder()->buildAndPlace("Pixel", envelopePhys);
    }

    //
    // Add IBL services
    //
    gmt_mgr->SetCurrentLD(0);
    if(gmt_mgr->ibl()&&gmt_mgr->IBLFlexAndWingDefined()&&gmt_mgr->PixelStaveLayout()>3&&gmt_mgr->PixelStaveLayout()<7)
      {
	// Build IBL services from endblock to PP0
	for(int iSection=1; iSection<4; iSection++)
	  {

	    // ----------- end of stave PP0 services (insde barrel)
	    
	    GeoPixelIFlexServices iFlexSrv(iSection);
	    iFlexSrv.Build();

	    GeoNameTag * tagFlexA = new GeoNameTag("PP0Flex_A");
	    GeoTransform *xformFlexA = iFlexSrv.getSupportTrfA();
	    GeoPhysVol *flexPhys_A = iFlexSrv.getSupportA();
	    envelopePhys->add(tagFlexA);
	    envelopePhys->add(xformFlexA);
	    envelopePhys->add(flexPhys_A);
	    
	    GeoNameTag * tagFlexC = new GeoNameTag("PP0Flex_C");
	    GeoTransform *xformFlexC = iFlexSrv.getSupportTrfC();
	    GeoPhysVol *flexPhys_C = iFlexSrv.getSupportC();
	    envelopePhys->add(tagFlexC);
	    envelopePhys->add(xformFlexC);
	    envelopePhys->add(flexPhys_C);
	  }

	// Build IBL fwd services (wavy shapes)
	int iSection=2;
	GeoPixelIBLFwdServices fwdSrv(iSection);
	if(fwdSrv.isComplexShapeDefined()){
	  
	  fwdSrv.Build();
	  
	  GeoNameTag * tagFwdSvcA = new GeoNameTag("FwdSvc_A");
	  GeoTransform *xformFwdSvcA = fwdSrv.getSupportTrfA();
	  GeoPhysVol *svcPhys_A = fwdSrv.getSupportA();
	  envelopePhys->add(tagFwdSvcA);
	  envelopePhys->add(xformFwdSvcA);
	  envelopePhys->add(svcPhys_A);
	  
	  GeoNameTag * tagFwdSvcC = new GeoNameTag("FwdSvc_C");
	  GeoTransform *xformFwdSvcC = fwdSrv.getSupportTrfC();
	  GeoPhysVol *svcPhys_C = fwdSrv.getSupportC();
	  envelopePhys->add(tagFwdSvcC);
	  envelopePhys->add(xformFwdSvcC);
	  envelopePhys->add(svcPhys_C);
	}
      }
  }
  
  // Build detailed frame
  if(gmt_mgr->detailedFrame() && gmt_mgr->DoServices()) {
    int numSections = gmt_mgr->PixelFrameSections();
    GeoPixelFrame frame;
    for (int iSection = 0; iSection <  numSections; iSection++) {
      //GeoVPhysVol * framePhys =  frame.Build(iSection);
      //envelopePhys->add(framePhys);
      frame.BuildAndPlace(envelopePhys,iSection);
    }      
  }

  // Extra Material 
  InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
  xMat.add(envelopePhys,"Pixel");

  // DBM
  // Z-positon is Hardcoded, 
  // so if change then change in DBM_module too

  if (gmt_mgr->dbm()) {
    HepGeom::Translate3D dbmTransform1( 0, 0, 887.002*CLHEP::mm + ( gmt_mgr->DBMTelescopeZ() )/2.); //Add 0.002mm to 887mm for safety

    //DDmgr->numerology().addEndcap(4);
    gmt_mgr->SetPartsDBM();
    gmt_mgr->SetPos();
    DBM_Det theDBM;
    GeoNameTag* tag1 = new GeoNameTag("DBMA");
    GeoVPhysVol* dbmPhys1 =  theDBM.Build() ;
    envelopePhys->add(tag1);
    envelopePhys->add(new GeoIdentifierTag(4));
    envelopePhys->add(new GeoTransform(dbmTransform1));
    envelopePhys->add(dbmPhys1 );

    //DDmgr->numerology().addEndcap(-4);
    gmt_mgr->SetNeg();
    GeoNameTag* tag2 = new GeoNameTag("DBMC");
    GeoTransform* dbmTransform2 = new GeoTransform(HepGeom::TranslateZ3D(-887.002*CLHEP::mm - ( gmt_mgr->DBMTelescopeZ() )/2.) *  HepGeom::RotateY3D(180*CLHEP::deg));
    envelopePhys->add(tag2);
    envelopePhys->add(new GeoIdentifierTag(-4));
    envelopePhys->add(dbmTransform2);
    envelopePhys->add(theDBM.Build());
  } else {
    DDmgr->numerology().setNumDisksDBM(0);
    DDmgr->numerology().setNumBarrelDBM(0);
  }

  // reset BarrelEndcap value to zero
  gmt_mgr->SetBarrel();


  // BCM
  if (gmt_mgr->athenaComps()->bcm()) {
    // Already retrieved in PixelDetectorTool
    StatusCode sc = gmt_mgr->athenaComps()->bcm()->build(envelopePhys);
    if (sc.isFailure()) {
      std::cout << "PixelGeoModel failed to build BCM." << std::endl;
    }
  }
	  
  // BLM
  if (gmt_mgr->athenaComps()->blm()) {
    // Already retrieved in PixelDetectorTool
    StatusCode sc = gmt_mgr->athenaComps()->blm()->build(envelopePhys);
    if (sc.isFailure()) {
      std::cout << "PixelGeoModel failed to build BLM." << std::endl;
    }
  }

  if(pixServices) delete pixServices;

  return envelopePhys;
  

}


