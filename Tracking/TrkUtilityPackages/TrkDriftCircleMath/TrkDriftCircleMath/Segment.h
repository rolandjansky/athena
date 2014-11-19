/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_SEGMENT_H
#define DCMATH_SEGMENT_H

#include <ostream>
#include <vector>

#include "GaudiKernel/MsgStream.h"

#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/Cluster.h"

namespace TrkDriftCircleMath
{

  class Segment {
  public:
    Segment (const Line& l, const DCOnTrackVec& dcs, double chi2, unsigned int ndof, double dtheta, double dy0)
      : m_line (l), m_dcs (dcs), m_chi2 (chi2), m_ndof (ndof), m_dtheta (dtheta), m_dy0 (dy0), m_t0Shift(-99999.), m_t0Error(-99999.),
      m_deltaAlpha(-99999.),m_deltab(-99999.),m_deltas (0), m_hitsOutOfTime (0), m_hitsOnTrack (0), m_closeHits (0), m_showerHits (0),
      m_crossedTubesMl1 (0), m_crossedTubesMl2 (0), m_hitsMl1 (0), m_hitsMl2 (0), m_ambigue (0), m_clusterLayers (0)  {}
            
    Segment (const Line& l, const DCOnTrackVec& dcs)
      : m_line (l), m_dcs (dcs), m_chi2 (1e9), m_ndof (0), m_dtheta (1e9), m_dy0 (1e9), m_t0Shift(-99999.), m_t0Error(-99999.),
      m_deltaAlpha(-99999.),m_deltab(-99999.),m_deltas (0), m_hitsOutOfTime (0), m_hitsOnTrack (0), m_closeHits (0), m_showerHits (0),
      m_crossedTubesMl1 (0), m_crossedTubesMl2 (0), m_hitsMl1 (0), m_hitsMl2 (0), m_ambigue (0), m_clusterLayers (0) {}
            
    ~Segment() {}
    
    const Line&           line() const { return m_line; }
    const DCOnTrackVec&   dcs() const { return m_dcs; }
    const CLVec&          clusters() const { return m_clusters; }
    const DCVec&          emptyTubes() const { return m_emptyTubes; }

    /** non-const access to reduce memory usage when re-using segments */
    Line&           line() { return m_line; }
    DCOnTrackVec&   dcs() { return m_dcs; }
    CLVec&          clusters() { return m_clusters; }
    DCVec&          emptyTubes() { return m_emptyTubes; }

    /** set methods to replace calling constructors */
    void set(double chi2, unsigned int ndof, double dtheta, double dy0);
    void setAndReset(double chi2, unsigned int ndof, double dtheta, double dy0);
 
    double                chi2() const { return m_chi2; }
    unsigned int          ndof() const { return m_ndof; }
    double                dtheta() const { return m_dtheta; }
    double                dy0() const { return m_dy0; }
    
    void dcs (const DCOnTrackVec& dcs) { m_dcs = dcs; }
    void clusters (const CLVec& cls) { m_clusters = cls; }
    void emptyTubes (const DCVec& emptyTubes) { m_emptyTubes = emptyTubes; }
    void deltas (unsigned int dts) { m_deltas = dts; }
    void hitsOutOfTime (unsigned int hitsOutOfTime) { m_hitsOutOfTime = hitsOutOfTime; }
    void hitsOnTrack (unsigned int hitsOnTrack) { m_hitsOnTrack = hitsOnTrack; }
    void closeHits (unsigned int closeHits) { m_closeHits = closeHits; }
    void showerHits (unsigned int showerHits) { m_showerHits = showerHits; }
    void crossedTubes (unsigned int crossedTubesMl1, unsigned int crossedTubesMl2) {
        m_crossedTubesMl1 = crossedTubesMl1;
        m_crossedTubesMl2 = crossedTubesMl2;
    }
    void hitsPerMl (unsigned int hml1, unsigned int hml2) { m_hitsMl1 = hml1; m_hitsMl2 = hml2; }
    void ambigue (unsigned int amb) { m_ambigue = amb; }
    void clusterLayers (unsigned int ncl) { m_clusterLayers = ncl; }

    void setT0Shift( double t0Shift, double error ) { 
      m_t0Shift = t0Shift; 
      m_t0Error = error; 
    }
    bool hasT0Shift() const { return m_t0Shift != -99999.; }    

    void setCurvatureParameters(double deltaAlpha, double deltab) { m_deltaAlpha=deltaAlpha; m_deltab=deltab; }
    bool hasCurvatureParameters() const { return m_deltaAlpha != -99999.; }

    double       t0Shift() const { return m_t0Shift; }
    double       t0Error() const { return m_t0Error; }
    double       deltaAlpha() const { return m_deltaAlpha; }
    double       deltab() const { return m_deltab; }
    unsigned int deltas() const { return m_deltas; }
    unsigned int hitsOutOfTime() const { return m_hitsOutOfTime; }
    unsigned int hitsOnTrack() const { return m_hitsOnTrack; }
    unsigned int closeHits() const { return m_closeHits; }
    unsigned int showerHits() const { return m_showerHits; }
    unsigned int crossedTubes() const { return crossedTubesMl1() + crossedTubesMl2(); }
    unsigned int crossedTubesMl1() const { return m_crossedTubesMl1; }
    unsigned int crossedTubesMl2() const { return m_crossedTubesMl2; }
    unsigned int hitsMl1() const { return m_hitsMl1; }
    unsigned int hitsMl2() const { return m_hitsMl2; }
    unsigned int ambigue() const { return m_ambigue; }
    unsigned int clusterLayers() const { return m_clusterLayers; }
    

  private:
    Line           m_line;
    DCOnTrackVec   m_dcs;
    CLVec          m_clusters;
    DCVec          m_emptyTubes;
    double         m_chi2;
    unsigned int   m_ndof;
    double         m_dtheta;            // needed in find5. Error in tube coordinate (pos? dir?)
    double         m_dy0;               // needed in find5. Error in strip coordinate (pos? dir?)
    double         m_t0Shift;
    double         m_t0Error;
    double         m_deltaAlpha;
    double         m_deltab;
    unsigned int   m_deltas;
    unsigned int   m_hitsOutOfTime;
    unsigned int   m_hitsOnTrack;
    unsigned int   m_closeHits;
    unsigned int   m_showerHits;
    unsigned int   m_crossedTubesMl1;
    unsigned int   m_crossedTubesMl2;
    unsigned int   m_hitsMl1;
    unsigned int   m_hitsMl2;
    unsigned int   m_ambigue;
    unsigned int   m_clusterLayers;
    
  };
  
  typedef std::vector<Segment>   SegVec;
  typedef SegVec::iterator       SegIt;
  typedef SegVec::const_iterator SegCit;

  inline void Segment::set(double chi2, unsigned int ndof, double dtheta, double dy0) {
    m_chi2 = chi2;
    m_ndof = ndof;
    m_dtheta = dtheta;
    m_dy0 = dy0;   
    m_t0Shift = -99999.;
    m_deltaAlpha = -99999.;
    m_deltab = -99999.;
    m_deltas = 0;
    m_hitsOutOfTime = 0;
    m_hitsOnTrack = 0;
    m_closeHits = 0;
    m_showerHits = 0;
    m_crossedTubesMl1 = 0;
    m_crossedTubesMl2 = 0;
    m_hitsMl1 = 0;
    m_hitsMl2 = 0;
    m_ambigue = 0;
    m_clusterLayers = 0;    
  }

  inline void Segment::setAndReset(double chi2, unsigned int ndof, double dtheta, double dy0) {
    m_chi2 = chi2;
    m_ndof = ndof;
    m_dtheta = dtheta;
    m_dy0 = dy0;   
    m_t0Shift = -99999.;
    m_deltaAlpha = -99999.;
    m_deltab = -99999.;
    m_deltas = 0;
    m_hitsOutOfTime = 0;
    m_hitsOnTrack = 0;
    m_closeHits = 0;
    m_showerHits = 0;
    m_crossedTubesMl1 = 0;
    m_crossedTubesMl2 = 0;
    m_hitsMl1 = 0;
    m_hitsMl2 = 0;
    m_ambigue = 0;
    m_clusterLayers = 0;
    m_dcs.clear();
    m_clusters.clear();
    m_emptyTubes.clear(); 
    
  }

  std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::Segment& dc );

  MsgStream   & operator<< (MsgStream   & os, const TrkDriftCircleMath::Segment& dc);


}



#endif // DCMATH_SEGMENT_H
