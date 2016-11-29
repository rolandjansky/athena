/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelGeoModel/GeoPixelEndCap.h"
#include "PixelGeoModel/GeoPixelDisk.h"
#include "PixelGeoModel/GeoPixelDiskSLHC.h"
#include "PixelGeoModel/GeoPixelECCable.h"
#include "PixelGeoModel/GeoPixelServices.h"
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
#include <sstream>

#include "InDetGeoModelUtils/VolumeBuilder.h"

GeoPixelEndCap::GeoPixelEndCap(const GeoPixelServices * pixServices)
  : m_pixServices(pixServices)
{}

GeoVPhysVol* GeoPixelEndCap::Build( ) {
  

  //
  // create the Barrel Mother volume
  // 
  double rmin = gmt_mgr->PixelEndcapRMin();
  double rmax = gmt_mgr->PixelEndcapRMax();
  double halflength = (gmt_mgr->PixelEndcapZMax()-gmt_mgr->PixelEndcapZMin())/2.;
  const GeoMaterial* air = mat_mgr->getMaterial("std::Air");
  const GeoTube* ecTube = new GeoTube(rmin,rmax,halflength);
  const GeoLogVol* ecLog = new GeoLogVol("EndCap",ecTube,air);
  GeoFullPhysVol* ecPhys = new GeoFullPhysVol(ecLog);

  int ndisks = gmt_mgr->PixelEndcapNDisk();
  gmt_mgr->msg(MSG::DEBUG) << "Number of disks is " << ndisks << endmsg;
  
  // Set numerology
  DDmgr->numerology().setNumDisks(ndisks);

  // position of the endcap
  double endcapZOffset = 0.5* (gmt_mgr->PixelEndcapZMax()+gmt_mgr->PixelEndcapZMin());
  
  //
  // Place the disks and cables on both sides
  //
  GeoPixelDiskSLHC * pdslhc = 0;
  GeoPixelDisk * pd = 0;
  GeoPixelECCable * pecc = 0;
  if (gmt_mgr->slhc()) {
    pdslhc = new GeoPixelDiskSLHC;
  } else {
    pd = new GeoPixelDisk;
    pecc = new GeoPixelECCable;      
  }  
  for(int idisk = 0; idisk < ndisks; idisk++) {
    gmt_mgr->SetCurrentLD(idisk);
    // Some method is accessing the eta before the disk is built so we set it 
    // to a valid value.
    gmt_mgr->SetEta(0);
    if(gmt_mgr->isLDPresent()) {
      //
      // The position is given w.r.t to the center of the detector!
      //
      //position of the disk
      double zdisk = gmt_mgr->PixelDiskZPosition()-endcapZOffset;
      // place the disk
      std::ostringstream nameTag; 
      nameTag << "Disk" << idisk;
      GeoNameTag * tag = new GeoNameTag(nameTag.str());
      GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zdisk));

      GeoVPhysVol * diskPhys = 0;
      if (gmt_mgr->slhc()) {
	diskPhys = pdslhc->Build();
      } else {
	diskPhys = pd->Build();
      }	

      ecPhys->add(tag);
      ecPhys->add(new GeoIdentifierTag(idisk));
      ecPhys->add(xform);
      ecPhys->add(diskPhys);

      // Store the alignable transform
      int brl_ec = 2*gmt_mgr->GetSide();
      Identifier id = gmt_mgr->getIdHelper()->wafer_id(brl_ec,idisk,0,0);
      DDmgr->addAlignableTransform(1, id, xform, diskPhys);

      //
      // place the cables twice for the two active parts
      //
      if (pecc && pd) { // Not in SLHC
	double dz = pd->Thickness()/2. + gmt_mgr->PixelECCablesDistance() ;
	GeoTransform * xformCablesPlus = new GeoTransform(HepGeom::TranslateZ3D(zdisk+dz));
	ecPhys->add(xformCablesPlus);
	ecPhys->add(pecc->Build() );
	GeoTransform * xformCablesMinus = new GeoTransform(HepGeom::TranslateZ3D(zdisk-dz));
	ecPhys->add(xformCablesMinus);
	ecPhys->add(pecc->Build() );
      }
    } else {
      if(gmt_mgr->msgLvl(MSG::DEBUG))	gmt_mgr->msg(MSG::DEBUG) << "Disk " << idisk << " not built" << endmsg;
    }
  }
  delete pdslhc;
  delete pd;
  delete pecc;

  if(m_pixServices) {
    //
    // Add the Endcap services inide the EC volume
    //
    std::string region;
    if (gmt_mgr->GetSide()>0) { 
      // EndcapA
      region = "A";
    } else {
      // EndcapC
      region = "C";
    }     
  
    m_pixServices->getBuilder()->buildAndPlace(region, ecPhys, endcapZOffset);
    if (m_pixServices->getServMatBuilder()) {
      m_pixServices->getServMatBuilder()->buildAndPlace(region, ecPhys, endcapZOffset);
    }
  }

  {
    // Extra Material
    InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
    xMat.add(ecPhys,"PixelEndcap",endcapZOffset);
    if (gmt_mgr->GetSide()>0) { // EndcapA
      xMat.add(ecPhys,"PixelEndcapA",endcapZOffset);
    } else {            // EndcapC
      xMat.add(ecPhys,"PixelEndcapC",endcapZOffset);
    }
  }

  return ecPhys;
}
