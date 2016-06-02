/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutLoI/GeoPixelEndcapLoITool.h"
#include "PixelLayoutLoI/GeoPixelDiskLoI.h"
#include "PixelLayoutUtils/PixelGeneralXMLHelper.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelInterfaces/IPixelServicesTool.h"

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


GeoPixelEndcapLoITool::GeoPixelEndcapLoITool( const std::string& type, const std::string& name, const IInterface*  parent )
  : AthAlgTool(type, name, parent),
    m_IDserviceTool(),
    m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc")
{
  declareInterface<IGeoPixelEndcapTool>(this);
  declareProperty("PixelServicesTool", m_IDserviceTool);

}


//================ Initialisation =================================================

StatusCode GeoPixelEndcapLoITool::initialize()
{
  
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  ATH_MSG_INFO("GeoPixelEndcapLoITool initialize() successful in " << name());

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
    return StatusCode::FAILURE;
  }

  m_bPrebuild = false;

  return StatusCode::SUCCESS;
}


//================ Create =================================================

StatusCode GeoPixelEndcapLoITool::create()
{
  
  StatusCode result = StatusCode::SUCCESS;

  return result;

}

//================ Finalisation =================================================

StatusCode GeoPixelEndcapLoITool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


// Register callback function on ConDB object
// Empty for now
StatusCode GeoPixelEndcapLoITool::registerCallback( StoreGateSvc*)
{
  return StatusCode::SUCCESS;
}


void GeoPixelEndcapLoITool::preBuild(const PixelGeoBuilderBasics* basics)
{
  PixelGeneralXMLHelper genDBHelper("PIXEL_PIXELGENERAL_GEO_XML", basics);

  std::cout<<"LoI - GeoPixelEndcapLoITool"<<std::endl;

  //
  // create the Endcap Mother volume
  m_endcapRMin = genDBHelper.getEndcapRMin();
  m_endcapRMax = genDBHelper.getEndcapRMax();
  m_endcapZMin = genDBHelper.getEndcapZMin();
  m_endcapZMax = genDBHelper.getEndcapZMax();

  // Number of disk read from xmlReader
  m_ndisks = m_xmlReader->nbOfPixelEndcapLayers();

  std::cout<<"ENDCAP : "<<m_endcapRMin<<" "<<m_endcapRMax<<" "<<m_endcapZMin<<" "<<m_endcapZMax<<"   # disc "<<m_ndisks<<std::endl;

  // Build and collect the service regions
  // double barrelHalfLen = genDBHelper.getBarrelHalfLength();

  // double halflength = (m_endcapZMax-m_endcapZMin)*.5;
  // double zMiddle =  (m_endcapZMax+m_endcapZMin)*.5;
  m_endcapSvcRegions.clear();

  m_ecPosSvcRegion.push_back("A");
  m_ecNegSvcRegion.push_back("C");
  m_endcapSvcRegions.push_back(new InDetDD::TubeZone("A",m_endcapZMin,m_endcapZMax,m_endcapRMin,m_endcapRMax));
  m_endcapSvcRegions.push_back(new InDetDD::TubeZone("C",-m_endcapZMax,-m_endcapZMin,m_endcapRMin,m_endcapRMax,true));

  m_bPrebuild=true;
}


GeoVPhysVol* GeoPixelEndcapLoITool::buildEndcap(const PixelGeoBuilderBasics* basics, int side)
{
  // Check if the endcap is already prebuild
  if(!m_bPrebuild)preBuild(basics);

  // links to material manager and msgstream
  m_matMgr = basics->matMgr();
  m_msg = basics->msgStream();

  double halflength = (m_endcapZMax-m_endcapZMin)*.5;
  double zMiddle =  (m_endcapZMax+m_endcapZMin)*.5;
  double endcapZOffset = zMiddle;

  // Build the endcap envelope
  const GeoMaterial* air = m_matMgr->getMaterial("std::Air");

  const GeoTube* ecTube = new GeoTube(m_endcapRMin,m_endcapRMax,halflength);
  const GeoLogVol* ecLog = new GeoLogVol("EndCap",ecTube,air);
  GeoFullPhysVol* ecPhys = new GeoFullPhysVol(ecLog);

  m_msg << "Number of disks is " << m_ndisks << endreq;

  // Set numerology
  basics->getDetectorManager()->numerology().setNumDisks(m_ndisks);

  //
  // Place the disks and cables on both sides
  //
  
  GeoPixelDiskLoI* disk = new GeoPixelDiskLoI(basics,side);

  for(int idisk = 0; idisk < m_ndisks; idisk++) {

    m_msg << "##################################################################" << endreq;
    m_msg << "Build disc " << idisk << endreq;
    m_msg << "##################################################################" << endreq;
    
    disk->setDiskNumber(idisk);
    GeoVPhysVol* diskPhys = disk->Build();

    //position of the disk
    double zdisk = disk->getZPosition();
    std::cout<<"DISK position : "<<idisk<<" "<<zdisk<<std::endl;

    // place the disk
    std::ostringstream nameTag; 
    nameTag << "Disk" << idisk;
    GeoNameTag * tag = new GeoNameTag(nameTag.str());
    GeoAlignableTransform* xform = new GeoAlignableTransform(HepGeom::TranslateZ3D(zdisk-zMiddle));
    
    ecPhys->add(tag);
    ecPhys->add(new GeoIdentifierTag(idisk));
    ecPhys->add(xform);
    ecPhys->add(diskPhys);

    // Store the alignable transform
    int brl_ec = 2*side;
    Identifier id = basics->getIdHelper()->wafer_id(brl_ec,idisk,0,0);
    basics->getDetectorManager()->addAlignableTransform(1, id, xform, diskPhys);
  }

  // Retrieve the service tool
  StatusCode sc = m_IDserviceTool.retrieve(); 
  if (sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve " <<  m_IDserviceTool << ",  some services will not be built." << endreq;
  }
  else{
    msg(MSG::INFO) << "Service builder tool retrieved: " << m_IDserviceTool << endreq;
  }

  // Place the services in the endcap envelope
  if(m_IDserviceTool){
    std::cout<<"ENDCAP : build services "<<std::endl;
    std::string region =(side>0)?"A":"C";
    m_IDserviceTool->buildAndPlace(region, ecPhys, endcapZOffset);
  }
  else
    std::cout<<"ENDCAP : build services - no services defined "<<std::endl;

//   {
//     // Extra Material
//     InDetDD::ExtraMaterial xMat(gmt_mgr->distortedMatManager());
//     xMat.add(ecPhys,"PixelEndcap",endcapZOffset);
//     if (gmt_mgr->GetSide()>0) { // EndcapA
//       xMat.add(ecPhys,"PixelEndcapA",endcapZOffset);
//     } else {            // EndcapC
//       xMat.add(ecPhys,"PixelEndcapC",endcapZOffset);
//     }
//   }

  return ecPhys;
}
