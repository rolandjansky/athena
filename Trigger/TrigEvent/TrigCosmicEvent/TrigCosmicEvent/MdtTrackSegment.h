/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __MdtTrackSegment_h__
#define __MdtTrackSegment_h__
/*
  MdtTrackSegment.h
*/
#include <string>
#include <vector>
#include <list>
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "CLHEP/Vector/TwoVector.h"

class MdtTrackSegment {
public:
  /** 
      Constructor
  */
  MdtTrackSegment();
  MdtTrackSegment(double alpha, double b, int s,double t0,double ch,int nh);
  ~MdtTrackSegment();

  /**
     Set the MDT station ID
  */
  void setStationId(int id) { mStationId = id; }
  /**
     Set the index of the track the segment is associated with in the 
     in the container of CosmicMuon
  */
  void setTrackId(int id) { mTrackId = id; }
  /**
     Set the number of MDT hits in this track segment
  */
  void setNHits(int n) { mNHits = n; }
  /**
     Set the track parameter, alpha, of this track
  */
  void setAlpha(double a) { mAlpha = a; }
  /**
     Set the track parameter, beta, of this track
  */
  void setB(double b) { mB = b; }
  /**
     Set the swap parameter (cm)
  */
  void setSwap(int n) { mSwap = n; }
  /**
     Set three track parameters at once
  */
  void setParameters(double a, double b, int swap);
  /**
     Set the arrival timing of the track (ns)
  */
  void setT0(double t0) { mT0 = t0; }
  /**
     Set the Chi2 of the track fit (drift circle fit)
  */
  void setChi2(double x) { mChi2 = x; }
  //  function that stores the average value of R by using hits belong to the segment
  void setR(double r) { mR = r; }
  //  function that stores the average value of Z by using hits belong to the segment
  void setZ(double z) { mZ = z; }

  /** function that returns MDT chamber's Id number*/
  //  void addMdtHit(const MdtHit& hit);

  /**
     @return MDT station ID
  */
  unsigned int stationId() const { return mStationId; }
  /**
     @return track ID which is the index of CosmicMuon in its container
  */
  int trackId() const { return mTrackId; }
  /**
     @return number of MDT hits in this track segment
  */
  int NHits() const { return mNHits; }
  /**
     @return theta of the track
  */
  double theta() const { return alpha(); }
  /**
     @return alpha parameter of the track
  */
  double alpha() const { return mAlpha; }
  /**
     @return b parameter of the track
  */
  double b() const { return mB; }
  /**
     @return swap parameter of the track (segment's orientation wrt anode wire)
  */
  int swap() const { return mSwap; }
  /**
     @return arrival timing of the track (ns) at the MDT chamber
  */
  double t0() const { return mT0; }
  /**
     @ return chi2 of the track fit
  */
  double chi2() const { return mChi2; }
  //  function that returns the average value of R by using hits belong to the segment
  double r() const { return mR; }
  //  function that returns the average value of Z by using hits belong to the segment
  double z() const { return mZ; }

  //  std::list<MdtHit> hits() { return mHits; }
  //  const std::list<MdtHit> hits() const { return mHits; }
  /**
     @ return the normalized chi2 = (chi2/NHits)
  */
  double normalizedChi2() const;

  /**
     @retrun direction of the track in (Z,R) plane
  */
  CLHEP::Hep2Vector direction() const;
  /**
     @return the reference point in (Z,R) plane (cm)
  */
  CLHEP::Hep2Vector refPoint() const;

  void clear();



private:
  /** MDT station ID, the compact form used in the offline identifier */
  int mStationId;
  /** CosmicMuon ID which this segment belongs to */
  int mTrackId;

  /** track parameter in (Z, R) plane. R=tan(alpha)*Z+b */
  double mAlpha; // Need double precision
  /** track parameter in (Z, R) plane. R=tan(alpha)*Z+b */
  double mB; // Need double precision
  /** segment's orientation wrt anode wire */
  int mSwap;

  /** arrival timing of the track at this chamber (ns) to the MDT chamber*/
  float mT0; // Fitted t0 (ns)
  
  /** chi2 of the track fit */
  float mChi2;
  /** number of MDT hits in this track */
  int mNHits;
  float mR;
  float mZ;

};

bool operator<(const MdtTrackSegment& lhs, const MdtTrackSegment& rhs);

/* bool operator<(const MdtTrackSegment* lhs, const MdtTrackSegment* rhs) */
/* { */
/*   if (!lhs or !rhs) return false; */
/*     if( lhs->r() < rhs->r() ) return true; */
/*     else if( lhs->r() == rhs->r() && lhs->r() < rhs->r() ) return true; */
/*     else return false; */
/* } */

class order_by_radius
{
public:
    bool operator()(const MdtTrackSegment& lhs,const MdtTrackSegment& rhs)
    {
        return lhs.r() < rhs.r();
    }
};

CLASS_DEF( MdtTrackSegment , 210121850 , 2 )

#endif // __MdtTrackSegment_h__
