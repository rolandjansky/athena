/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  MdtTrackSegment.cxx
*/
#include "TrigCosmicEvent/MdtTrackSegment.h"
#include <cmath>

using namespace std;

MdtTrackSegment::MdtTrackSegment() 
  : mStationId(0), mTrackId(-1), mAlpha(0.0), mB(0.0), mSwap(0), 
    mT0(0.0), mChi2(0.0), mNHits(0) {
}
 
MdtTrackSegment::MdtTrackSegment(double alpha, double b, int s, double t0, 
				 double ch, int nh)
  : mStationId(0), mTrackId(-1), mAlpha(alpha), mB(b), mSwap(s), 
    mT0(t0), mChi2(ch), mNHits(nh) {
}

MdtTrackSegment::~MdtTrackSegment() {
}

void MdtTrackSegment::setParameters(double a, double b, int swap) {
  mAlpha = a;
  mB = b;
  mSwap = swap;
}

double MdtTrackSegment::normalizedChi2() const {
  const int npar = 3;
  int ndof = mNHits - npar;
  if (ndof > 0) return mChi2/ndof;
  else  return -1.0;
}

CLHEP::Hep2Vector MdtTrackSegment::direction() const {
  return CLHEP::Hep2Vector(mZ, mR);
}

CLHEP::Hep2Vector MdtTrackSegment::refPoint() const {
  return CLHEP::Hep2Vector(cos(mAlpha), sin(mAlpha));
}

void MdtTrackSegment::clear() {
  mNHits = 0;
  mAlpha = 0.0;
  mB = 0.0;
  mSwap = 0;
  mT0 = 0.0;
  mChi2 = 0.0;
  mR = 0.0;
  mZ = 0.0;
}

bool operator<(const MdtTrackSegment& lhs, const MdtTrackSegment& rhs)
{

    if( lhs.r() < rhs.r() ) return true;
    else if( lhs.r() == rhs.r() && lhs.r() < rhs.r() ) return true;
    else return false;
}
