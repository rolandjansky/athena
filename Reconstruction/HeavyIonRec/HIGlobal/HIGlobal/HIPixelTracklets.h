/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETS_H
#define HIPIXELTRACKLETS_H
#include<vector>
#include<iostream>
#include "InDetPrepRawData/PixelCluster.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLIDSvc/CLASS_DEF.h"

class HIPixelTracklets
{
  typedef InDet::PixelCluster PixelCluster;
 public:
  HIPixelTracklets();
  HIPixelTracklets(const HepGeom::Point3D<double>& vrt, const std::vector<PixelCluster*>& clusters);  
  virtual ~HIPixelTracklets(){}
  double GetEta() const {return m_eta;}
  double GetPhi() const {return m_phi;}
  double GetPt() const {return m_pt;}
  double GetEtaS() const {return m_etaS; }
  double GetPhiS() const {return m_phiS; }
  const HepGeom::Point3D<double> & GetVertex() const {return m_vrt;}
  const std::vector<PixelCluster*>* GetClusters() const { return &m_clusters;}
  void Merge(HIPixelTracklets* trkl);
  
 private:
  double m_eta;
  double m_phi;
  double m_pt;
  double m_etaS; //second cluster
  double m_phiS; //second cluster
  HepGeom::Point3D<double>  m_vrt;
  std::vector<PixelCluster*>  m_clusters;
};

CLASS_DEF( HIPixelTracklets , 179679982 , 1 )
#endif
