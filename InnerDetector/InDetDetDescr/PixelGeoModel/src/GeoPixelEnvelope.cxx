/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoPixelEnvelope.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "GeoPixelBarrel.h"
#include "GeoPixelEndCap.h"
#include "GeoPixelServices.h"
#include "GeoPixelFrame.h"
#include "GeoPixelOldFrame.h"
#include "GeoPixelIFlexServices.h"
#include "GeoPixelIBLFwdServices.h"
#include "DBM_Det.h"

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
  

  bool barrelPresent   = m_gmt_mgr->partPresent("Barrel");
  bool endcapAPresent  = m_gmt_mgr->partPresent("EndcapA");
  bool endcapCPresent  = m_gmt_mgr->partPresent("EndcapC");

   // NB. We apply the part transforms only to the active parts.
  // In general though they should always be zero. It would make more sense to apply
  // a transform to the overl pixel if one wants to move the detector.

  //
  // create the External volume where I'll put all the pixel detector
  // This can either be a simple tube or a more general PCON.
  // 
  const GeoShape * envelopeShape;
  InDetDD::Zone * pixZone = 0; 
  if (m_gmt_mgr->PixelSimpleEnvelope()) {
    double rmin = m_gmt_mgr->PixelRMin();
    double rmax = m_gmt_mgr->PixelRMax();
    double halflength = m_gmt_mgr->PixelHalfLength();
    envelopeShape = new GeoTube(rmin,rmax,halflength);
    pixZone = new InDetDD::TubeZone("Pixel",-halflength,halflength,rmin,rmax);
  } else {
    GeoPcon* envelopeShapeTmp  = new GeoPcon(0.,2*Gaudi::Units::pi);
    // table contains +ve z values only and envelope is assumed to be symmetric around z.
    int numPlanes = m_gmt_mgr->PixelEnvelopeNumPlanes();
    for (int i = 0; i < numPlanes * 2; i++) {
      int tableIndex = (i < numPlanes) ?  numPlanes - 1 - i :  i - numPlanes;
      int sign       = (i < numPlanes) ? -1 : 1;
      double z = m_gmt_mgr->PixelEnvelopeZ(tableIndex) * sign;
      double rmin = m_gmt_mgr->PixelEnvelopeRMin(tableIndex);
      double rmax = m_gmt_mgr->PixelEnvelopeRMax(tableIndex);
      envelopeShapeTmp->addPlane(z, rmin, rmax);
    }
    envelopeShape = envelopeShapeTmp;
    pixZone = new InDetDD::PconZone("Pixel",envelopeShapeTmp);
  }
  const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
  const GeoLogVol* envelopeLog = new GeoLogVol("Pixel",envelopeShape,air);
  GeoFullPhysVol* envelopePhys = new GeoFullPhysVol(envelopeLog);

  GeoPixelServices * pixServices = 0;
  if(m_gmt_mgr->DoServices() ) {
    // Takes ownership of pixZone
    pixServices = new GeoPixelServices(pixZone);
  } else {
    delete pixZone;
  }

  if (barrelPresent) {
    
    m_DDmgr->numerology().addBarrel(0);
    //
    // Add the Barrel:
    //
    GeoTrf::Transform3D barrelTransform = m_gmt_mgr->partTransform("Barrel");
    
    m_gmt_mgr->SetBarrel();
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
    m_gmt_mgr->SetEndcap();

    GeoPixelEndCap pec(pixServices);
    double zpos = (m_gmt_mgr->PixelEndcapZMax()+m_gmt_mgr->PixelEndcapZMin())/2.;

    // EndCap A
    if (endcapAPresent) {

      m_DDmgr->numerology().addEndcap(2);
    
      GeoTrf::Transform3D endcapATransform = m_gmt_mgr->partTransform("EndcapA");
      
      m_gmt_mgr->SetPos();
      GeoTransform* xform = new GeoTransform(endcapATransform * GeoTrf::TranslateZ3D(zpos));
      GeoNameTag* tag  = new GeoNameTag("EndCapA");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(2));
      envelopePhys->add(xform);
      envelopePhys->add(pec.Build() );
    }
    
    // EndCap C
    if (endcapCPresent) {

      m_DDmgr->numerology().addEndcap(-2);
      
      GeoTrf::Transform3D endcapCTransform = m_gmt_mgr->partTransform("EndcapC");
      
      m_gmt_mgr->SetEndcap();
      m_gmt_mgr->SetNeg();
      GeoTransform* xform = new GeoTransform(endcapCTransform * GeoTrf::TranslateZ3D(-zpos) *  GeoTrf::RotateY3D(180*Gaudi::Units::deg));
      GeoNameTag* tag  = new GeoNameTag("EndCapC");
      envelopePhys->add(tag);
      envelopePhys->add(new GeoIdentifierTag(-2));
      envelopePhys->add(xform);
      envelopePhys->add(pec.Build() );
      
    }
 
    if(m_gmt_mgr->DoServices() ) {
      // Pixel Frame. In recent versions this taken care of in the general services.
      if (m_gmt_mgr->oldFrame()) {
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
    m_gmt_mgr->SetCurrentLD(0);
    if(m_gmt_mgr->ibl()&&m_gmt_mgr->IBLFlexAndWingDefined()&&m_gmt_mgr->PixelStaveLayout()>3&&m_gmt_mgr->PixelStaveLayout()<7)
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
  if(m_gmt_mgr->detailedFrame() && m_gmt_mgr->DoServices()) {
    int numSections = m_gmt_mgr->PixelFrameSections();
    GeoPixelFrame frame;
    for (int iSection = 0; iSection <  numSections; iSection++) {
      //GeoVPhysVol * framePhys =  frame.Build(iSection);
      //envelopePhys->add(framePhys);
      frame.BuildAndPlace(envelopePhys,iSection);
    }      
  }

  // Extra Material 
  InDetDD::ExtraMaterial xMat(m_gmt_mgr->distortedMatManager());
  xMat.add(envelopePhys,"Pixel");

  // DBM
  // Z-positon is Hardcoded, 
  // so if change then change in DBM_module too

  if (m_gmt_mgr->dbm()) {
    GeoTrf::Translate3D dbmTransform1( 0, 0, 887.002*Gaudi::Units::mm + ( m_gmt_mgr->DBMTelescopeZ() )/2.); //Add 0.002mm to 887mm for safety

    //m_DDmgr->numerology().addEndcap(4);
    m_gmt_mgr->SetPartsDBM();
    m_gmt_mgr->SetPos();
    DBM_Det theDBM;
    GeoNameTag* tag1 = new GeoNameTag("DBMA");
    GeoVPhysVol* dbmPhys1 =  theDBM.Build() ;
    envelopePhys->add(tag1);
    envelopePhys->add(new GeoIdentifierTag(4));
    envelopePhys->add(new GeoTransform(dbmTransform1));
    envelopePhys->add(dbmPhys1 );

    //m_DDmgr->numerology().addEndcap(-4);
    m_gmt_mgr->SetNeg();
    GeoNameTag* tag2 = new GeoNameTag("DBMC");
    GeoTransform* dbmTransform2 = new GeoTransform(GeoTrf::TranslateZ3D(-887.002*Gaudi::Units::mm - ( m_gmt_mgr->DBMTelescopeZ() )/2.) *  GeoTrf::RotateY3D(180*Gaudi::Units::deg));
    envelopePhys->add(tag2);
    envelopePhys->add(new GeoIdentifierTag(-4));
    envelopePhys->add(dbmTransform2);
    envelopePhys->add(theDBM.Build());
  } else {
    m_DDmgr->numerology().setNumDisksDBM(0);
    m_DDmgr->numerology().setNumBarrelDBM(0);
  }

  // reset BarrelEndcap value to zero
  m_gmt_mgr->SetBarrel();


  // BCM
  if (m_gmt_mgr->athenaComps()->bcm()) {
    // Already retrieved in PixelDetectorTool
    StatusCode sc = m_gmt_mgr->athenaComps()->bcm()->build(envelopePhys);
    if (sc.isFailure()) {
      std::cout << "PixelGeoModel failed to build BCM." << std::endl;
    }
  }
	  
  // BLM
  if (m_gmt_mgr->athenaComps()->blm()) {
    // Already retrieved in PixelDetectorTool
    StatusCode sc = m_gmt_mgr->athenaComps()->blm()->build(envelopePhys);
    if (sc.isFailure()) {
      std::cout << "PixelGeoModel failed to build BLM." << std::endl;
    }
  }

  if(pixServices) delete pixServices;

  return envelopePhys;
  

}


