/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDet_GeoPixelEndcapFoamModuleTool_H
#define InDet_GeoPixelEndcapFoamModuleTool_H

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

class GeoDetModulePixel;

class GeoPixelEndcapFoamModule : public PixelGeoBuilder {
 public:
  
  GeoPixelEndcapFoamModule(const PixelGeoBuilderBasics* basics,
			   int iLayer, const GeoDetModulePixel* transModule, const GeoDetModulePixel* endcapModule,
			   double endcapTilt, double transTilt);
			   
  virtual GeoVPhysVol* Build();
  void preBuild();

  GeoVPhysVol* getEndcapFoam() const { return m_endcapFoam; }
  double getEndcapZshift() const {return m_zEndcapFoamShift; }
  GeoVPhysVol* getTransFoam() const { return m_transFoam; }
  double getTransZshift() const {return m_zTransFoamShift; }

 private:

  int m_layer;
  const GeoDetModulePixel *m_endcapModule;
  const GeoDetModulePixel *m_transModule;
  double m_endcapTiltAngle;
  double m_transTiltAngle;

  void buildFoamModules();

  double m_zEndcapFoamShift;
  double m_zTransFoamShift;
  double m_FacePlateThick;
  double m_FacePlateGlueThick;
  double m_FacePlateGreaseThick;
  double m_FoamBaseThick;
  double m_FoamEdgeThick;
  double m_FoamBaseThickTrans;
  double m_FoamEdgeThickTrans;
  
  GeoPhysVol *m_endcapFoam;
  GeoPhysVol *m_transFoam;

};


#endif
