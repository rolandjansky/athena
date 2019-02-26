/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_ZScanSeedFinder_H
#define TRKVERTEXSEEDFINDERTOOLS_ZScanSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "xAODEventInfo/EventInfo.h"

#include <unordered_map>

namespace Trk
{

  class Track;
  class IMode1dFinder;
  class ITrackToVertexIPEstimator;

  // @author N. Giacinto Piacquadio (Albert-Ludwigs-Universitaet Freiburg - Germany)
  //
  // @ATLAS software
  //
  // This class implements a seed finder for the primary vertex finding 
  // which is based on the use of a clustering algorithm over the z position at 
  // point of closest approach to the beam (2d).
  // 
  // -------------------------------------------
  // Changes:
  //
  // David Shope <david.richard.shope@cern.ch> (2016-04-19)
  //
  // EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex, 
  //                         from Trk::RecVertex   to xAOD::Vertex,
  //                         from Trk::Vertex      to Amg::Vector3D


  class ZScanSeedFinder : public AthAlgTool, virtual public IVertexSeedFinder
  {
  public:
    StatusCode initialize();
    StatusCode finalize();

    //default constructor due to Athena interface
    ZScanSeedFinder(const std::string& t, const std::string& n, const IInterface*  p);
    
    //destructor
    virtual ~ZScanSeedFinder();

    // Interface for Tracks with starting seed/linearization point
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::Track*> & vectorTrk,const xAOD::Vertex * constraint=0);
    
    /** Interface for MeasuredPerigee with starting point */
    virtual Amg::Vector3D findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint=0);

    // Interface for finding vector of seeds from tracks
    virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::Track*>& vectorTrk,const xAOD::Vertex * constraint=0);

    // Interface for finding vector of seeds from track parameters
    virtual std::vector<Amg::Vector3D> findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,const xAOD::Vertex * constraint=0);

    //The below four functions are dummy functions so that this compiles. The functions are needed in the interface IMode3dFinder.h for Mode3dFromFsmw1dFinder (the seed finder for the Inclusive Secondary Vertex Finder)

    virtual void setPriVtxPosition( double vx, double vy );

    virtual int perigeesAtSeed( std::vector<const Trk::TrackParameters*> * a,
                              const std::vector<const Trk::TrackParameters*> & b) const;

    virtual int getModes1d(std::vector<float>& a, std::vector<float>& b, 
			   std::vector<float>& c, std::vector<float>& d) const;
    virtual void getCorrelationDistance( double &cXY, double &cZ );


  private:
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey { this, "EventInfo", "EventInfo", "key for EventInfo retrieval" };

    ToolHandle< IMode1dFinder > m_mode1dfinder;

    ToolHandle< ITrackToVertexIPEstimator > m_IPEstimator;

    bool m_disableAllWeights;
    float m_constraintcutoff;
    float m_constrainttemp;
    bool m_useLogPt;
    double m_minPt;
    bool m_usePt;
    double m_expPt;
    bool m_cacheWeights;

    // functor to hash key for unordered_map
    struct hash_perigee {
      size_t operator()(const Trk::Perigee& perigee) const
      {
	return 
	  std::hash<double>()(perigee.parameters()[Trk::d0]) ^
	  std::hash<double>()(perigee.parameters()[Trk::z0]) ^
	  std::hash<double>()(perigee.parameters()[Trk::phi]) ^
	  std::hash<double>()(perigee.parameters()[Trk::theta]) ^
	  std::hash<double>()(perigee.parameters()[Trk::qOverP]); 
      }
    };

    // functor to compare two unordered_map Key values for equality
    struct pred_perigee {
      bool operator()(const Trk::Perigee& left, const Trk::Perigee& right) const
      {
	return 
	  (left.parameters()[Trk::d0] == right.parameters()[Trk::d0]) &&
	  (left.parameters()[Trk::z0] == right.parameters()[Trk::z0]) &&
	  (left.parameters()[Trk::phi] == right.parameters()[Trk::phi]) &&
	  (left.parameters()[Trk::theta] == right.parameters()[Trk::theta]) &&
	  (left.parameters()[Trk::qOverP] == right.parameters()[Trk::qOverP]);
      }
    };

    // hashtable to avoid computing perigee more than once per track
    std::unordered_map< Trk::Perigee , std::pair<double, double>, hash_perigee, pred_perigee> m_weightMap;

    // record of last seen values; if any of these change, the cached perigees are invalidated
    uint32_t m_cachedRunNumber;
    unsigned long long m_cachedEventNumber;
    double m_cachedConstraintX;
    double m_cachedConstraintY;

  };
}
#endif
