/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  CosmicMuon.cxx
*/
#include <iostream>
#include <cmath>
#include "TrigCosmicEvent/CosmicMuon.h"


using namespace std;

double CosmicMuon::residual(double z, double r, 
			    const double* p, const double* v) {
  double num = -v[1]*(z-p[0]) + v[0]*(r-p[1]);
  double den = sqrt(v[0]*v[0] + v[1]*v[1]);
  if (den != 0.0) {
    return num/den;
  } else {
    return 0.0;
  }
}

CosmicMuon::CosmicMuon() 
  : mP(0.0), mRadius(0.0), mTheta(0.0), mPhi(0.0), mT(0.0), 
    mIsIngoing(0), mNRpcPairs(0), mNTgcPairs(0), mNMdtHits(0), mNMdtSegs(0) {
  for (int i=0; i<3; ++i) {
    mPoint[i] = 0.0;
  }
}

CosmicMuon::CosmicMuon(const double p[3], double theta, double phi)
  : mP(0.0), mRadius(0.0), mTheta(theta), mPhi(phi), mT(0.0), 
    mIsIngoing(0), mNRpcPairs(0), mNTgcPairs(0), mNMdtHits(0), mNMdtSegs(0) {
  for (int i=0; i<3; ++i) {
    mPoint[i] = p[i];
  }
}

CosmicMuon::~CosmicMuon() {
}

double CosmicMuon::residual(double z, double r) const {
  double ZRvec[2], ZRPoint[2];
  ZRvec[0] = cos(mTheta);
  ZRvec[1] = sin(mTheta);
  ZRPoint[0] = mPoint[2];
  ZRPoint[1] = sqrt(mPoint[0]*mPoint[0] + mPoint[1]*mPoint[1]);

  double num = -ZRvec[1]*(z-ZRPoint[0]) + ZRvec[0]*(r-ZRPoint[1]);
  double den = sqrt(ZRvec[0]*ZRvec[0] + ZRvec[1]*ZRvec[1]);
  if (den != 0.0) {
    return num/den;
  } else {
    return 0.0;
  }
}

void CosmicMuon::getRefPoint(double vec[3]) const {
  for (int i=0; i<3; ++i) {
    vec[i] = mPoint[i];
  }
}

void CosmicMuon::setRefPoint(const double p[3]) {
  for (int i=0; i<3; ++i) {
    mPoint[i] = p[i];
  }
}

double CosmicMuon::longPos(double z, double r) const {
  double ZRvec[2], ZRPoint[2];
  ZRvec[0] = cos(mTheta);
  ZRvec[1] = sin(mTheta);
  ZRPoint[0] = mPoint[2];
  ZRPoint[1] = sqrt(mPoint[0]*mPoint[0] + mPoint[1]*mPoint[1]);
  double t = ZRvec[0]*(z-ZRPoint[0]) + ZRvec[1]*(r-ZRPoint[1]);
  return t;
}

void CosmicMuon::perigee(double point[3], 
			 double x0, double y0, double z0) const {
  double vec[3];
  double p[3] = { mPoint[0]-x0, mPoint[1]-y0, mPoint[2]-z0 };
  vec[0] = sin(mTheta)*cos(mPhi);
  vec[1] = sin(mTheta)*sin(mPhi);
  vec[2] = cos(mTheta);
  double t = -(vec[0]*p[0]+vec[1]*p[1]+vec[2]*p[2]);
  for (int i=0; i<3; ++i) {
    point[i] = mPoint[i] + t*vec[i];
  }
}

void CosmicMuon::perigee2D(double point[3], 
			   double x0, double y0) const {
  double vec[3];
  double vec2[2];
  double p[2] = { mPoint[0]-x0, mPoint[1]-y0 };
  vec[0] = sin(mTheta)*cos(mPhi);
  vec[1] = sin(mTheta)*sin(mPhi);
  vec[2] = cos(mTheta);
  vec2[0] = cos(mPhi);
  vec2[1] = sin(mPhi);
  double t = -(vec2[0]*p[0]+vec2[1]*p[1]);
  double a = 1.0;
  if (sin(mTheta) != 0.0) a = 1/sin(mTheta);
  for (int i=0; i<3; ++i) {
    point[i] = mPoint[i] + (t/a)*vec[i];
  }
}

double CosmicMuon::dca3D(double x0, double y0, double z0) const {
  double point[3];
  perigee(point, x0, y0, z0);
  return sqrt(point[0]*point[0]+point[1]*point[1]+point[2]*point[2]);
}

double CosmicMuon::dcaXY(double x0, double y0) const {
  double point[3];
  perigee2D(point, x0, y0);
  return sqrt(point[0]*point[0]+point[1]*point[1]);
}

double CosmicMuon::dca2D_Z(double x0, double y0) const {
  double point[3];
  perigee2D(point, x0, y0);
  return point[2];
}

void CosmicMuon::clear() {
  mP = 0.0;
  mRadius = 0.0;
  mTheta = 0.0;
  mPhi = 0.0;
  mNRpcPairs = 0;
  mNMdtHits = 0;
  mNMdtSegs = 0;
  mT = 0.0;
  mIsIngoing = 0;
  for (int i=0; i<3; ++i) {
    mPoint[i] = 0.0;
  }
}

