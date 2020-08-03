/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MSSurfaces.h"

#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/CylinderSurface.h"

MSSurfaces::MSSurfaces(){ 
  Trk::Surface* empty =0;
  SurfaceVec tmp(12,empty);
  m_vec = tmp;
  SurfaceStations sta(12);
  m_station = sta;

  double rMinEI = 1700., rMinEM = 1500., rMinEO = 2200., rMinEE = 5000.;
  double rMaxEI = 6200., rMaxEM = 12000., rMaxEO = 12000., rMaxEE = 9500.;
  double rBI = 5000., rBM = 7000., rBO = 9500., rBE = 5000. ;
  double lenBI = 12000., lenBM = 10000., lenBO = 13000., lenBE = 12000.;
  const Amg::Vector3D posEI(0.,0.,7500.), posEM(0.,0.,13500.), posEO(0.,0.,21000.), posEE(0.,0.,10000.), zero(0.,0.,0.);
  Amg::Transform3D* transEIA = new Amg::Transform3D(Amg::Translation3D(posEI));
  Amg::Transform3D* transEMA = new Amg::Transform3D(Amg::Translation3D(posEM));
  Amg::Transform3D* transEOA = new Amg::Transform3D(Amg::Translation3D(posEO));
  Amg::Transform3D* transEEA = new Amg::Transform3D(Amg::Translation3D(posEE));
  Amg::Transform3D* transEIC = new Amg::Transform3D(Amg::Translation3D(-posEI));
  Amg::Transform3D* transEMC = new Amg::Transform3D(Amg::Translation3D(-posEM));
  Amg::Transform3D* transEOC = new Amg::Transform3D(Amg::Translation3D(-posEO));
  Amg::Transform3D* transEEC = new Amg::Transform3D(Amg::Translation3D(-posEE)); 

  m_vec[BI] = new Trk::CylinderSurface(rBI, lenBI );
  m_station[BI] = "BI" ;

  m_vec[BM] = new Trk::CylinderSurface(rBM, lenBM );
  m_station[BM] = "BM" ;

  m_vec[BO] = new Trk::CylinderSurface(rBO, lenBO );
  m_station[BO] = "BO" ;

  m_vec[BE] = new Trk::CylinderSurface(rBE, lenBE );
  m_station[BE] = "BE" ;

  m_vec[EIA] = new Trk::DiscSurface(transEIA, rMinEI, rMaxEI );
  m_station[EIA] = "EIA" ;

  m_vec[EMA] = new Trk::DiscSurface(transEMA, rMinEM, rMaxEM );
  m_station[EMA] = "EMA" ;

  m_vec[EOA] = new Trk::DiscSurface(transEOA, rMinEO, rMaxEO );
  m_station[EOA] = "EOA" ;

  m_vec[EEA] = new Trk::DiscSurface(transEEA, rMinEE, rMaxEE );
  m_station[EEA] = "EEA" ;

  m_vec[EIC] = new Trk::DiscSurface(transEIC, rMinEI, rMaxEI );
  m_station[EIC] = "EIC" ;

  m_vec[EMC] = new Trk::DiscSurface(transEMC, rMinEM, rMaxEM );
  m_station[EMC] = "EMC" ;

  m_vec[EOC] = new Trk::DiscSurface(transEOC, rMinEO, rMaxEO );
  m_station[EOC] = "EOC" ;

  m_vec[EEC] = new Trk::DiscSurface(transEEC, rMinEE, rMaxEE );
  m_station[EEC] = "EEC" ;

}

MSSurfaces::~MSSurfaces(){
  for( unsigned int i=0; i< m_vec.size(); ++i ){
    if( m_vec[i] )    delete m_vec[i];
  }

  m_vec.clear();
  m_station.clear();

}

