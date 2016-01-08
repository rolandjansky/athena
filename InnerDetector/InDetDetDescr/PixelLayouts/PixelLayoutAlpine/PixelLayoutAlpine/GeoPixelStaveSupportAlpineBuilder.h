/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELALPINESTAVESUPPORTTOOL_H
#define GEOPIXELALPINESTAVESUPPORTTOOL_H

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTransform.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 
#include "PixelLayoutAlpine/PixelStaveSupportXMLHelper.h"

namespace InDet{
  class GeoComponent;
}
class GeoDetModulePixel;

class GeoPixelStaveSupportAlpineBuilder : public PixelGeoBuilder {

public:  
  GeoPixelStaveSupportAlpineBuilder(const PixelGeoBuilderBasics* basics,
				    int iLayer, int iSector,  
				    const GeoDetModulePixel* brlModule, 
				    double maxTiltedModuleThick,
				    double zEndOfBrlModulePos,
				    std::vector<HepGeom::Point3D<double> > endcapModPos);
				    
  virtual GeoVPhysVol* Build();
  void preBuild();
  void BuildStaveSupport(int layerIndex, int geoIndex,  bool bBarrelStave, double zLength, HepGeom::Transform3D trf, int side=0) ;
			 
  virtual GeoVPhysVol* getPhysVol() const {return m_physVol;}
  virtual InDet::GeoComponent* getGeoCompVol() const {return m_geoVol;}

  virtual const HepGeom::Transform3D & transform() const {return m_transform;}
  virtual double thicknessP() const {return m_thicknessP;}
  virtual double thicknessN() const {return m_thicknessN;}
  virtual double thickness() const {return m_thicknessN+m_thicknessP; }
  virtual double width() const {return m_width; }
  virtual double length() const {return m_length; }
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape( double safetyMargin);

  virtual void computeStaveEnvelopTransformAndSize(double moduleThickN,double moduleThickP, double moduleWidth,
						   double moduleThickN3D,double moduleThickP3D, double moduleWidth3D);

  virtual double getFacePlateThickness() const { return m_FacePlateTotalThick; }
  virtual double getFacePlateGreaseThickness() const { return m_FacePlateGreaseThick; }

  void DumpVolumeAndMaterial(const GeoLogVol* logVol, double normFactor);

private:

  PixelStaveSupportXMLHelper* m_staveDBHelper;

  GeoPhysVol* m_physVol;
  InDet::GeoComponent* m_geoVol;
  GeoSimplePolygonBrep* m_staveEnvelopShape;
  GeoSimplePolygonBrep* m_basicStaveEnvelopShape;

  int m_layer;
  int m_sector;
  bool m_bGeoComp;
  const GeoDetModulePixel* m_brlModule;
  double m_maxTiltedModuleThick;
  double m_zEndOfBrlModulePos;

  double m_omegaTotalThick;
  double m_flexTotalThick;
  double m_StaveModuleOffset;
  double m_MechanicalStaveOffset;
  double m_FacePlateTotalThick;
  double m_FacePlateGreaseThick;

  double m_staveBarrelWidth;

  HepGeom::Transform3D m_transform;
  double m_thicknessP;
  double m_thicknessN;
  double m_width;
  double m_length;
  double m_SafetyMargin;
  bool m_bVerbose;

  void RemoveCoincidentAndColinearPointsFromShape(std::vector<double> &xPoint, std::vector<double>&yPoint);
  void GetSurroundingConvexShape(std::vector<double> &xPoint, std::vector<double>&yPoint, 
				 std::vector<int> iExcept = std::vector<int>());
  void AddSurroundingXYMargin(double vMarginX, double vMarginY, std::vector<double> &xPoint, std::vector<double>&yPoint);
  const GeoShape* addShape(const GeoShape * lastShape, const GeoShape * nextShape, const HepGeom::Transform3D & trans);

  int m_PlanarModuleNumber, m_3DModuleNumber;

  double m_StaveLength;

  double m_OmegaThick;
  double m_OmegaGlueThick;
  double m_OmegaMiddleThick;
  double m_OmegaTopWidth;

  double m_FoamThick;
  double m_FacePlateThick;
  double m_FacePlateWidth;

  double m_FoamBaseThick;
  double m_FoamBaseThickTrans;
  std::vector<HepGeom::Point3D<double> > m_endcapModPos;

  HepGeom::Point3D<double> omegaTopPoint;
  HepGeom::Point3D<double> omegaEndPoint_bottom;
  HepGeom::Point3D<double> omegaTopPoint_bottom;
  HepGeom::Point3D<double> foamTopPoint;
  HepGeom::Point3D<double> foamEndPoint;
  HepGeom::Point3D<double> glueTopPoint_top;
  HepGeom::Point3D<double> glueEndPoint_top;
  HepGeom::Point3D<double> glueTopPoint_bottom;
  HepGeom::Point3D<double> glueEndPoint_bottom;

  void ComputeStaveExternalShape(double,double, double);
  HepGeom::Point3D<double> IntersectionPoint(double Ax,double Ay,double Bx,double By,double Cx,double Cy,double Dx,double Dy);
  CLHEP::Hep3Vector NormalizeDir(CLHEP::Hep3Vector v);
  HepGeom::Point3D<double> NeighbourPoint_Rad(HepGeom::Point3D<double> p, CLHEP::Hep3Vector v, double delta);
  HepGeom::Point3D<double> NeighbourPoint_Perp(HepGeom::Point3D<double> p, CLHEP::Hep3Vector v, double delta, int iDir);
  double ComputeAngle(double ux, double uy, double vx, double vy);
  double ComputeDistance(HepGeom::Point3D<double> p, HepGeom::Point3D<double> q);

};

#endif

