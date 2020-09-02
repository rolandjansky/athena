/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SiHitFillerTool.h"

#include "AthenaKernel/errorcheck.h"
//#include "InDetReadoutGeometry/SiDetectorElement.h"
//#include "InDetIdentifier/PixelID.h"
//#include "InDetPrepRawData/PixelCluster.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "AtlasHepMC/GenParticle.h"
//#include "GeoAdaptors/GeoSiHit.h"

/*
using HepGeom::Point3D;


namespace D3PD {


SiHitFillerTool::SiHitFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<SiHit> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode SiHitFillerTool::initialize(){

  CHECK( Base::initialize() );

  // if (detStore()->retrieve(m_pixId,"PixelID").isFailure() ){
  //  REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve pixel ID helper";
  //  return StatusCode::FAILURE;
  //}

  return StatusCode::SUCCESS;
}

StatusCode SiHitFillerTool::book(){

  CHECK( addVariable ("identifier", m_identifier) );
  CHECK( addVariable ("localStartPos_x",m_localStartPos_x) );
  CHECK( addVariable ("localStartPos_y",m_localStartPos_y) );
  CHECK( addVariable ("localStartPos_z",m_localStartPos_z) );

  CHECK( addVariable ("localEndPos_x",m_localEndPos_x) );
  CHECK( addVariable ("localEndPos_y",m_localEndPos_y) );
  CHECK( addVariable ("localEndPos_z",m_localEndPos_z) );

  CHECK( addVariable ("globalPos_x",m_globalPos_x) );
  CHECK( addVariable ("globalPos_y",m_globalPos_y) );
  CHECK( addVariable ("globalPos_z",m_globalPos_z) );
  CHECK( addVariable ("globalPos_r",m_globalPos_r) );

  CHECK( addVariable ("energyLoss",m_energyLoss) );
  CHECK( addVariable ("meanTime",m_meanTime) );
  CHECK( addVariable ("trackNumber",m_trackNumber) );
  CHECK( addVariable ("isPixel",m_isPixel) );
  CHECK( addVariable ("isSCT",m_isSCT) );
  CHECK( addVariable ("BarrelEndcap",m_BarrelEndcap) );

  CHECK( addVariable ("m_LayerDisk",m_LayerDisk) );
  CHECK( addVariable ("EtaModule",m_EtaModule) );
  CHECK( addVariable ("PhiModule",m_PhiModule) );
  CHECK( addVariable ("Side",m_Side) );

  CHECK( addVariable ("TruthBarcode",m_truthbarcode) );
 
  CHECK( addVariable ("TruthBarcode2",m_truthbarcode2) );



  return StatusCode::SUCCESS;
}

StatusCode SiHitFillerTool::fill (const SiHit& hit)
{

  //REPORT_MESSAGE (MSG::DEBUG) << "got hit";


  *m_identifier=hit.identify();

  const Point3D<double>& startPos=hit.localStartPosition();
  *m_localStartPos_x=startPos.x();
  *m_localStartPos_y=startPos.y();
  *m_localStartPos_z=startPos.z();

 const Point3D<double>& endPos=hit.localEndPosition();
  *m_localEndPos_x=endPos.x();
  *m_localEndPos_y=endPos.y();
  *m_localEndPos_z=endPos.z();

  GeoSiHit ghit(hit);
  const Point3D<double>& globalPos=ghit.getGlobalPosition();
  *m_globalPos_x = globalPos.x();
  *m_globalPos_y = globalPos.y();
  *m_globalPos_z = globalPos.z();
  *m_globalPos_r = globalPos.perp();

  *m_energyLoss=hit.energyLoss();
  *m_meanTime=hit.meanTime();
  *m_trackNumber=hit.trackNumber();
  //const HepMcParticleLink & 	particleLink () const
  *m_isPixel=hit.isPixel();
  *m_isSCT=hit.isSCT();
  *m_BarrelEndcap=hit.getBarrelEndcap();
  *m_LayerDisk=hit.getLayerDisk();
  *m_EtaModule=hit.getEtaModule();
  *m_PhiModule=hit.getPhiModule();
  *m_Side=hit.getSide(); 

  const HepMcParticleLink& mcLink = hit.particleLink();
  *m_truthbarcode2 = mcLink.barcode();

  const HepMC::GenParticle* genPart= mcLink.cptr();
  if(genPart){
   *m_truthbarcode = genPart->barcode();
  }else{
   *m_truthbarcode=-999;
  }

  return StatusCode::SUCCESS;
}

} // namespace D3PD
*/


using HepGeom::Point3D;


namespace D3PD {


SiHitFillerTool::SiHitFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<SiHit> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode SiHitFillerTool::initialize(){

  CHECK( Base::initialize() );

  // if (detStore()->retrieve(m_pixId,"PixelID").isFailure() ){
  //  REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve pixel ID helper";
  //  return StatusCode::FAILURE;
  //}

  return StatusCode::SUCCESS;
}

StatusCode SiHitFillerTool::book(){

  CHECK( addVariable ("identifier", m_identifier) );
  CHECK( addVariable ("localStartPos_x",m_localStartPos_x) );
  CHECK( addVariable ("localStartPos_y",m_localStartPos_y) );
  CHECK( addVariable ("localStartPos_z",m_localStartPos_z) );

  CHECK( addVariable ("localEndPos_x",m_localEndPos_x) );
  CHECK( addVariable ("localEndPos_y",m_localEndPos_y) );
  CHECK( addVariable ("localEndPos_z",m_localEndPos_z) );

  CHECK( addVariable ("globalPos_x",m_globalPos_x) );
  CHECK( addVariable ("globalPos_y",m_globalPos_y) );
  CHECK( addVariable ("globalPos_z",m_globalPos_z) );
  CHECK( addVariable ("globalPos_r",m_globalPos_r) );

  CHECK( addVariable ("energyLoss",m_energyLoss) );
  CHECK( addVariable ("meanTime",m_meanTime) );
  CHECK( addVariable ("trackNumber",m_trackNumber) );
  CHECK( addVariable ("isPixel",m_isPixel) );
  CHECK( addVariable ("isSCT",m_isSCT) );
  CHECK( addVariable ("BarrelEndcap",m_BarrelEndcap) );

  CHECK( addVariable ("m_LayerDisk",m_LayerDisk) );
  CHECK( addVariable ("EtaModule",m_EtaModule) );
  CHECK( addVariable ("PhiModule",m_PhiModule) );
  CHECK( addVariable ("Side",m_Side) );

  CHECK( addVariable ("TruthBarcode",m_truthbarcode) );
 
  CHECK( addVariable ("TruthBarcode2",m_truthbarcode2) );



  return StatusCode::SUCCESS;
}

StatusCode SiHitFillerTool::fill (const SiHit& /*hit*/)
{

  //REPORT_MESSAGE (MSG::DEBUG) << "got hit";


  *m_identifier=1;

  //const Point3D<double>& startPos=hit.localStartPosition();
  *m_localStartPos_x=1.0;
  *m_localStartPos_y=1.0;
  *m_localStartPos_z=1.0;

  //const Point3D<double>& endPos=hit.localEndPosition();
  *m_localEndPos_x=1.0;
  *m_localEndPos_y=1.0;
  *m_localEndPos_z=1.0;

  //GeoSiHit ghit(hit);
  //const Point3D<double>& globalPos=ghit.getGlobalPosition();
  *m_globalPos_x = 1.0;
  *m_globalPos_y = 1.0;
  *m_globalPos_z = 1.0;
  *m_globalPos_r = 1.0;

  *m_energyLoss=1.0;
  *m_meanTime=1.0;
  *m_trackNumber=1.0;
  *m_isPixel=1;
  *m_isSCT=1;
  *m_BarrelEndcap=1;
  *m_LayerDisk=1;
  *m_EtaModule=1;
  *m_PhiModule=1;
  *m_Side=1; 

  *m_truthbarcode2 = 1;


   *m_truthbarcode = -999;


  return StatusCode::SUCCESS;
}

} // namespace D3PD
