/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDETAILEDSTAVESUPPORT_H
#define GEOPIXELDETAILEDSTAVESUPPORT_H

#include "GeoPixelStaveSupport.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include "InDetGeoModelUtils/GeoNodePtr.h"

class GeoPixelDetailedStaveSupport : public GeoPixelStaveSupport {

public:  
  GeoPixelDetailedStaveSupport(InDetDD::PixelDetectorManager* ddmgr,
                               PixelGeometryManager* mgr);
  virtual GeoVPhysVol* Build() override;
  virtual GeoVPhysVol* getPhysVol() override {return m_physVol;}
  virtual const GeoTrf::Transform3D & transform() const override {return m_transform;}
  virtual double thicknessP() const override {return m_thicknessP;}
  virtual double thicknessN() const override {return m_thicknessN;}
  virtual GeoSimplePolygonBrep* computeStaveEnvelopShape( double safetyMargin) override;
  virtual GeoPhysVol* getEndblockEnvelopShape( int ) override;
  virtual GeoTransform* getEndblockEnvelopShapeTrf( int ) override;
  virtual double getEndblockZpos() const override { return m_endblockZpos; };
  virtual double getServiceZpos() const override { return m_serviceZpos; };
  virtual double getEndblockLength() const override { return m_endblockLength+m_endblockSrvLength; };
  virtual void computeStaveEnvelopTransformAndSize(double moduleThickN,double moduleThickP, double moduleWidth,
						   double moduleThickN3D,double moduleThickP3D, double moduleWidth3D) override;
  virtual int PixelNModule() const override { return m_PlanarModuleNumber+m_3DModuleNumber; }
  virtual int PixelNPlanarModule() const  override{ return m_PlanarModuleNumber; }
  virtual int PixelN3DModule() const override { return m_3DModuleNumber; }

private:
  GeoNodePtr<GeoVPhysVol> m_physVol;
  GeoNodePtr<GeoSimplePolygonBrep> m_staveEnvelopShape;
  GeoNodePtr<GeoSimplePolygonBrep> m_basicStaveEnvelopShape;

  GeoPhysVol* m_endblockAPhysVol = nullptr;
  GeoPhysVol* m_endblockCPhysVol = nullptr;
  double m_endblockZpos = 0.0, m_endblockLength = 0.0, m_serviceZpos = 0.0;
  double m_endblockSrvLength = 0.0;
  GeoPhysVol* m_endblockFlexPhysVol = nullptr;
  GeoNodePtr<GeoTransform> m_endblockFlexTrf;
  GeoPhysVol* m_serviceCoolPipePhysVol = nullptr;
  GeoNodePtr<GeoTransform> m_serviceCoolPipeTrfA;
  GeoNodePtr<GeoTransform> m_serviceCoolPipeTrfC;

  GeoTrf::Transform3D m_transform;
  double m_thicknessP = 0.0;
  double m_thicknessN = 0.0;
  double m_thicknessN_svc = 0.0;
  double m_width_svc = 0.0;
  double m_SafetyMargin = 0.0;
  bool m_bVerbose;

  void RemoveCoincidentAndColinearPointsFromShape(std::vector<double> &xPoint, std::vector<double>&yPoint);
  void GetSurroundingConvexShape(std::vector<double> &xPoint, std::vector<double>&yPoint, 
				 std::vector<int> iExcept = std::vector<int>());
  void AddSurroundingXYMargin(double vMarginX, double vMarginY, std::vector<double> &xPoint, std::vector<double>&yPoint);

  int m_PlanarModuleNumber = 0, m_3DModuleNumber = 0;

  double m_StaveLength = 0.0;
  double m_FoamEdgePointX = 0.0;
  double m_FoamEdgePointY = 0.0;
  double m_FoamMiddleThick = 0.0;
  double m_OmegaEndStaveThick = 0.0;
  double m_OmegaEndStavePointX = 0.0;
  double m_OmegaEndStavePointY = 0.0;
  double m_OmegaMidStaveThick = 0.0;
  double m_MidStaveSidePointX = 0.0;
  double m_MidStaveSidePointY = 0.0;

  void ComputeStaveExternalShape();
  GeoTrf::Vector3D IntersectionPoint(double Ax,double Ay,double Bx,double By,double Cx,double Cy,double Dx,double Dy);
  GeoTrf::Vector3D NormalizeDir(GeoTrf::Vector3D v);
  GeoTrf::Vector3D NeighbourPoint_Rad(GeoTrf::Vector3D p, GeoTrf::Vector3D v, double delta);
  GeoTrf::Vector3D NeighbourPoint_Perp(GeoTrf::Vector3D p, GeoTrf::Vector3D v, double delta, int iDir);
  double ComputeAngle(double ux, double uy, double vx, double vy);
  double ComputeDistance(GeoTrf::Vector3D p, GeoTrf::Vector3D q);

  const double m_oneDegree = 180.0/M_PI;

};

#endif

