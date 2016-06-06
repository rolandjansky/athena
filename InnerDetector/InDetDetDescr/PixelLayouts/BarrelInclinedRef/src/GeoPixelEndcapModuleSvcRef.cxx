/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelInclinedRef/GeoPixelEndcapModuleSvcRef.h"
#include "PixelGeoModelModule/GeoDetModulePixel.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "BarrelInclinedRef/PixelInclRefStaveXMLHelper.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"

							
GeoPixelEndcapModuleSvcRef::GeoPixelEndcapModuleSvcRef(const PixelGeoBuilderBasics* basics,
						   int iLayer, 
						   const GeoDetModulePixel* endcapModule, const GeoDetModulePixel* transModule, 
						   double endcapTilt, double transTilt)
						   
  : PixelGeoBuilder(basics),
    m_layer(iLayer), m_endcapModule(endcapModule), m_transModule(transModule), 
    m_endcapTiltAngle(endcapTilt), m_transTiltAngle(transTilt),
    m_endcapFoam(0),m_transFoam(0)
{
  preBuild();
}


void GeoPixelEndcapModuleSvcRef::preBuild()
{

  msg(MSG::INFO) <<"Foam description "<<m_layer<<endreq;

  PixelInclRefStaveXMLHelper staveDBHelper(m_layer, getBasics());
  //  int geoIndex = staveDBHelper.getStaveGeoIndexEC();
  //  msg(MSG::INFO) <<"Foam description "<<m_layer<<" "<<geoIndex<<endreq;
  
  buildFoamModules();
}

GeoVPhysVol* GeoPixelEndcapModuleSvcRef::Build()
{
  return 0;
}


void GeoPixelEndcapModuleSvcRef::buildFoamModules()
{
}
