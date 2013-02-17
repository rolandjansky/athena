/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __CosmicMuon_h__
#define __CosmicMuon_h__
/*
  CosmicMuon.hxx
*/
#include <string>
#include <vector>
#include <cmath>
#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"
#include "TrigCosmicEvent/MdtTrackSegment.h"

class CosmicMuon {
public:
  static const int MAX_MDT_HITS = 50;
  static const int MAX_MDT_TRACKS = 20;
  static double residual(double z, double r, const double* p, const double* v);

public:
  /**
     Constructor
  */
  CosmicMuon();
  /**
     Constructor
  */
  CosmicMuon(const double p[3], double theta, double phi);
  /**
     Destructor
  */
  ~CosmicMuon();

  /**
     An access function that returns the momentum of the track in MeV.
     Currently not implemented, and always returns zero.
  */
  double p() const { return mP; }
  /**
     An access function that returns the radius of the track (cm).
     Currently not implemented, always return zero.
  */
  double radius() const { return mRadius; }
  /**
     Returns the reference point of the track (x,y,z) coordinates in cm.
  */
  void getRefPoint(double vec[3]) const;
  /**
     Returns the reference point of the track (x,y,z) coordinates in cm.
  */
  const float* refPoint() const { return mPoint; }
  /**
     An access function which returns theta of the track
     function that returns track's slope in Z - R plane for each chamber
  */
  double theta() const { return mTheta; }
  /**
     An access function which returns the pseudo-rapidity of the track
  */
  double eta() const { return -std::log(tan(0.5*mTheta)); }
  /**
     An access function which returns phi of the track
  */
  double phi() const { return mPhi; }
  /**
     An access function which returns arrival time of the track at the 
     reference point
  */
  double t() const { return mT; }
  /**
     A function which returns true if the track movement is towards
     inside from the interaction point and false otherwise.
  */
  bool isIngoing() const { return mIsIngoing; }
  /**
     Returns the number RPC hit  pairs (eta,phi) associated to this track
  */
  int NRpcPairs() const { return mNRpcPairs; }
  /**
     Returns the number TGC hit pairs (eta,phi) associated to this track
  */
  int NTgcPairs() const { return mNTgcPairs; }
  /**
     Returns the number MDT hits associated to this track
  */
  int NMdtHits() const { return mNMdtHits; }
  /**
     Returns the number MDT segments associated to this track before fitting the arrival time
  */
  int NMdtSegs() const { return mNMdtSegs; }
  //  DataVector<MdtTrackSegment>& mdtTrackSegments() { return mSegs; }
  //  const DataVector<MdtTrackSegment>& mdtTrackSegments() const { return mSegs; }

  /**
     Set the momentum of the track in MeV
  */
  void setP(double p) { mP = p; }
  /**
     Set the radius of the track (cm)
  */
  void setRadius(double r) { mRadius = r; }
  /*
    Set the (x,y,z) coordinates of the reference point of the track in cm.
  */
  void setRefPoint(const double p[3]);
  /**
     Set theta of the track's slope in Z - R plane for each chamber
  */
  void setTheta(double theta) { mTheta = theta; }
  /**
     Set phi of the track
  */
  void setPhi(double phi) { mPhi = phi; }
  /**
     Set the arrival timing of the track at the reference point (ns).
  */
  void setT(double time) { mT = time; }
  /**
     Set whether the track is inwards going or not
   */
  void setIsIngoing(int n) { mIsIngoing = n; }
  /**
     Set the number of RPC hit pairs associated to the track
  */
  void setNRpcPairs(int n) { mNRpcPairs = n; }
  /**
     Set the number of TGC hit pairs associated to the track
  */
  void setNTgcPairs(int n) { mNTgcPairs = n; }
  /**
     Set the number of MDT hits associated to the track
  */
  void setNMdtHits(int n) { mNMdtHits = n; }
  /**
     Set the number of MDT segments associated to the track before fitting the arrival time
  */
  void setNMdtSegs(int n) { mNMdtSegs = n; }

  /**
     Return the residual of a given (z,r) point and this muon track (cm).
  */
  double residual(double z, double r) const;
  /**
     Return the longitudinal position of a given (z,r) point 
     calculated from the ZRPoint[2] of this muon track. It is positive 
     if it's in the direction of ZRVec[2].
  */
  double longPos(double z, double r) const;

  /**
     Return the perigee point with respect to the (x0, y0, z0) parameters
     given in the argument (cm).
  */
  void perigee(double point[3], double x0=0, double y0=0, double z0=0) const;
  /**
     Return the perigee point in x-y plane with respect to the (x0, y0) 
     parameters given in the argument (cm).
  */
  void perigee2D(double point[2], double x0=0, double y0=0) const;

  /**
     Distance of closest approach to (x0, y0, z0) 
  */
  double dca3D(double x0=0.0, double y0=0.0, double z0=0.0) const;
  /**
     Distance of closest approach in x-y plane to (x0, y0)
  */
  double dcaXY(double x0=0.0, double y0=0.0) const;
  /**
     Distance of closest approach in x-y plane to (x0, y0, z=0) 
  */
  double dca2D_Z(double x0=0.0, double y0=0.0) const;

  void clear();

private:
  /** Momentum of the track (GeV). */
  float mP;
  /** Radius of the track (cm) */
  float mRadius;
  /** theta of the track in Z - R plane*/
  float mTheta;
  /** Phi info taken from RPC */
  float mPhi;
  /** reference point of the track (cm)*/
  float mPoint[3];
  /** Arrival timing of the track (ns) */
  float mT;
  /** Whether the track is going inwards towards the interaction point or not*/
  int mIsIngoing;
  /** Number of RPC hit pairs associated to the track */
  int mNRpcPairs;
  /** Number of TGC hit pairs associated to the track */
  int mNTgcPairs;
  /** Number of MDT hits associated to the track */
  int mNMdtHits;
  /** Number of MDT segments associated to the track before fitting the arrival time */
  int mNMdtSegs;

  // MDT track segments
  //  DataVector<MdtTrackSegment> mSegs;
};

CLASS_DEF( CosmicMuon , 150330949 , 2 )


#endif // __CosmicMuon_h__
