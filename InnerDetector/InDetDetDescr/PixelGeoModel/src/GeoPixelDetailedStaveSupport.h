/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOPIXELDETAILEDSTAVESUPPORT_H
#define GEOPIXELDETAILEDSTAVESUPPORT_H

#include "GeoPixelStaveSupport.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GeoModelKernel/GeoPhysVol.h"


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
  virtual GeoPhysVol* getEndblockEnvelopShape( int ) const override;
  virtual GeoTransform* getEndblockEnvelopShapeTrf( int ) const override;
  virtual double getEndblockZpos() const override { return m_endblockZpos; };
  virtual double getServiceZpos() const override { return m_serviceZpos; };
  virtual double getEndblockLength() const override { return m_endblockLength+m_endblockSrvLength; };
  virtual void computeStaveEnvelopTransformAndSize(double moduleThickN,double moduleThickP, double moduleWidth,
						   double moduleThickN3D,double moduleThickP3D, double moduleWidth3D) override;
  virtual int PixelNModule() const override { return m_PlanarModuleNumber+m_3DModuleNumber; }
  virtual int PixelNPlanarModule() const  override{ return m_PlanarModuleNumber; }
  virtual int PixelN3DModule() const override { return m_3DModuleNumber; }

private:
  GeoVPhysVol* m_physVol;
  GeoSimplePolygonBrep* m_staveEnvelopShape;
  GeoSimplePolygonBrep* m_basicStaveEnvelopShape;

  GeoPhysVol* m_endblockAPhysVol;
  GeoPhysVol* m_endblockCPhysVol;
  double m_endblockZpos, m_endblockLength, m_serviceZpos;
  double m_endblockSrvLength;
  GeoPhysVol* m_endblockFlexPhysVol;
  GeoTransform* m_endblockFlexTrf;
  GeoPhysVol* m_serviceCoolPipePhysVol;
  GeoTransform* m_serviceCoolPipeTrfA;
  GeoTransform* m_serviceCoolPipeTrfC;

  GeoTrf::Transform3D m_transform;
  double m_thicknessP;
  double m_thicknessN;
  double m_thicknessN_svc;
  double m_width_svc;
  double m_SafetyMargin;
  bool m_bVerbose;

  void RemoveCoincidentAndColinearPointsFromShape(std::vector<double> &xPoint, std::vector<double>&yPoint);
  void GetSurroundingConvexShape(std::vector<double> &xPoint, std::vector<double>&yPoint, 
				 std::vector<int> iExcept = std::vector<int>());
  void AddSurroundingXYMargin(double vMarginX, double vMarginY, std::vector<double> &xPoint, std::vector<double>&yPoint);

  int m_PlanarModuleNumber, m_3DModuleNumber;

  double m_StaveLength;
  double m_FoamEdgePointX;
  double m_FoamEdgePointY;
  double m_FoamMiddleThick;
  double m_OmegaEndStaveThick;
  double m_OmegaEndStavePointX;
  double m_OmegaEndStavePointY;
  double m_OmegaMidStaveThick;
  double m_MidStaveSidePointX;
  double m_MidStaveSidePointY;

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

