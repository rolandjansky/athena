/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelLayoutAlpine/GeoPixelLayerAlpineBuilder.h"
#include "PixelLayoutAlpine/GeoPixelLadderAlpineBuilder.h"

#include "InDetGeoModelUtils/ExtraMaterial.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "InDetTrackingGeometryXML/XMLGeoTemplates.h"

#include "PixelGeoComponent/GeoStaveAlpine.h"

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

GeoPixelLayerAlpineBuilder::GeoPixelLayerAlpineBuilder(const PixelGeoBuilderBasics* basics, int iLayer, double rmin, double rmax):
  PixelGeoBuilder(basics),
  m_layer(iLayer),
  m_radiusMin(rmin),
  m_radiusMax(rmax),
  m_xmlReader("InDet::XMLReaderSvc/InDetXMLReaderSvc","XMLReaderSvc"),
  m_layerGeoComp(0)
{

  if (m_xmlReader.retrieve().isSuccess()){
    ATH_MSG_DEBUG("ITkXMLReader successfully retrieved " << m_xmlReader );
  } else {
    ATH_MSG_WARNING("ITkXMLReader: Couldn't retrieve " << m_xmlReader );
  }

  m_bGeoComp = basics->isGeoComponent();
  m_physVol = Build();
}

GeoVPhysVol* GeoPixelLayerAlpineBuilder::Build() {

  //
  // create a barrel layer
  //

  InDet::BarrelLayerTmp *layerTmp = m_xmlReader->getPixelBarrelLayerTemplate(m_layer);

  printf("************** BUILD LAYER  %d\n", m_layer);

  // retrieve stave template for this layer
  //  int layerIndex           = layerTmp->index;
  std::string layerName    = layerTmp->name;
  double layerRadius       = layerTmp->radius;
  int    nSectors          = layerTmp->stave_n; 
  double ladderTilt        = layerTmp->stave_tilt;
  double staveOffset       = layerTmp->stave_zoffset;   
  std::string staveType    = layerTmp->stave_type;

  double phiOfStaveZero = 0.;

  std::cout<<layerName<<" "<<staveType<<" "<<nSectors<<"   ladder offset : "<<staveOffset<<std::endl;
  std::cout<<"*****************************************************************************"<<std::endl;

  double deltaPhi = 360.*CLHEP::deg/(double)nSectors;

  HepGeom::Transform3D transRadiusAndTilt = HepGeom::TranslateX3D(layerRadius)*HepGeom::RotateZ3D(ladderTilt);
  std::cout<<"---> build ladder"<<std::endl;

  //  std::vector<InDet::StaveTmp *>staveTmp = m_xmlReader->getPixelStaveTemplate(m_layer);
  std::vector<InDet::StaveTmp *> staveTmp = m_xmlReader->getPixelStaveTemplate(m_layer);
  GeoPixelLadderAlpineBuilder pixelLadder(getBasics(), staveTmp[0], m_layer, transRadiusAndTilt); 
  GeoFullPhysVol* layerPhys = 0;

  // Register the number of stave defined for the layer
  getBasics()->getDetectorManager()->numerology().setNumPhiModulesForLayer(m_layer,nSectors);

  // Loop over the sectors and place everything
  //
  for(int ii = 0; ii < nSectors; ii++) {
    //    gmt_mgr->SetPhi(ii);

    double ladderOffset = (ii%2==0)? staveOffset: -staveOffset;
    double phiOfSector = phiOfStaveZero + ii*deltaPhi;
    HepGeom::Transform3D ladderTransform = HepGeom::TranslateZ3D(ladderOffset) * HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTilt;

    // Build ladder
    pixelLadder.setSector(ii);
    pixelLadder.setLadderTransform(ladderTransform);
    GeoVPhysVol *ladderPhys = 0;
    InDet::GeoStaveAlpine* sector = 0;
    ladderPhys=pixelLadder.Build();
    if(m_bGeoComp)
      sector=dynamic_cast<InDet::GeoStaveAlpine*>(pixelLadder.getGeoCompVol());

    if(ii==0){

      double ladderLength = pixelLadder.length(); //+staveOffset;
      //      double rmin = layerRadius - pixelLadder.rmin();
      //      double rmax = layerRadius + pixelLadder.rmax();
      double rmin = pixelLadder.rmin()-1.;
      double rmax = pixelLadder.rmax()+1.;
      //      if(m_layer==0&&rmin<34) rmin=34.1;
      //      if(m_layer==1&&rmax>105) rmax=104.;
      //      if(m_layer==1&&rmin<59.7) rmin=59.71;
      //      if(m_layer==1&&rmin<59.615) rmin=59.616;

      if(rmin<m_radiusMin) rmin=m_radiusMin+0.01;
      if(rmax>m_radiusMax) rmax=m_radiusMax-0.01;

      m_length = ladderLength+2*staveOffset+.01;

      // Now make the layer envelope
      // 
      msg(MSG::INFO)<<"Layer "<<m_layer<<" in/out radius "<<rmin<<"  "<<layerRadius<<"  "<<rmax<<"   length : "<<ladderLength<<endreq;
      const GeoMaterial* air = matMgr()->getMaterial("std::Air");
      std::ostringstream lname;
      lname << "Layer" << m_layer;
      const GeoTube* layerTube = new GeoTube(rmin,rmax,0.5*m_length+0.01); //solid
      const GeoLogVol* layerLog = new GeoLogVol(lname.str(),layerTube,air); //log volume
      layerPhys = new GeoFullPhysVol(layerLog); // phys vol
      if(m_bGeoComp)
	m_layerGeoComp = new InDet::GeoLayerAlpine( layerPhys);
    }


    // transform equiv  RotZ(phiOfSector)*TransZ(layerRadius)*RotZ(tilt)
    //    HepGeom::Transform3D ladderTransform = HepGeom::TranslateZ3D(ladderOffset) * HepGeom::RotateZ3D(phiOfSector) * transRadiusAndTilt;

    //
    // Place the active ladders
    //
    if(!m_bGeoComp){
      std::ostringstream nameTag; 
      nameTag << "Ladder" << ii;
      GeoNameTag * tag = new GeoNameTag(nameTag.str());
      GeoTransform* xform = new GeoTransform(ladderTransform);
      layerPhys->add(tag);
      layerPhys->add(new GeoIdentifierTag(ii) );
      layerPhys->add(xform);
      layerPhys->add(ladderPhys);
    }
    else {
      // Place stave
      m_layerGeoComp->placeStave( *sector, ladderTransform, ii);
    }

  }

  return layerPhys;

}

