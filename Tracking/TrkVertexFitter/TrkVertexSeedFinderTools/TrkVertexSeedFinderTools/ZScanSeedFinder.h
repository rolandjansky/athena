/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVERTEXSEEDFINDERTOOLS_ZScanSeedFinder_H
#define TRKVERTEXSEEDFINDERTOOLS_ZScanSeedFinder_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkVertexFitterInterfaces/IVertexSeedFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/SlotSpecificObj.h"

#include "TrkVertexSeedFinderUtils/IMode1dFinder.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"


#include "CxxUtils/checker_macros.h"

#include <unordered_map>

namespace Trk
{

  class Track;

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


  class ZScanSeedFinder final: public extends<AthAlgTool, IVertexSeedFinder>
  {
  public:
    // Standard Gaudi constructor.
    ZScanSeedFinder (const std::string& t,
                     const std::string& n,
                     const IInterface*  p);


    virtual ~ZScanSeedFinder();


    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    
    using IVertexSeedFinder::findSeed;

    /**
     *  Finds a linearization point out of a vector of tracks
     *  and returns it as an Amg::Vector3D object. If you want an 
     *  additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::Track*> & vectorTrk,
              const xAOD::Vertex * constraint=0) const override final;
    

    /** 
     * Finds a linearization point out of a vector of TrackParameters
     *  and returns it as an Amg::Vector3D object. If you want an 
     * additional constraint can be taken into account.
     */
    virtual Amg::Vector3D
    findSeed (const std::vector<const Trk::TrackParameters*> & perigeeList,
              const xAOD::Vertex * constraint=0) const override final;


    /**
     * Finds full vector of linearization points from a vector of tracks
     *  and returns it as an Amg::Vector3D object.  Intended for seed finders that produce all at once.
     *  If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds (const std::vector<const Trk::Track*>& vectorTrk,
                    const xAOD::Vertex * constraint=0) const override final;


    /**
     * Finds full vector of linearization points from a vector
     * of TrackParameters and returns it as an Amg::Vector3D object.
     * Intended for seed finders that produce all at once.
     * If you want an additional constraint can be taken into account.
     */
    virtual std::vector<Amg::Vector3D>
    findMultiSeeds(const std::vector<const Trk::TrackParameters*>& perigeeList,
                   const xAOD::Vertex * constraint=0) const override final; 


  private:
    /// Estimate z-position and weight for one track.
    std::pair<double, double>
    estimateWeight (const Trk::Perigee& iTrk,
                    const xAOD::Vertex* constraint) const;

    std::vector<Trk::DoubleAndWeight>
    getPositionsUncached (const std::vector<const Trk::TrackParameters*> & perigeeList,
                          const xAOD::Vertex * constraint) const;


    std::vector<Trk::DoubleAndWeight>
    getPositionsCached (const std::vector<const Trk::TrackParameters*> & perigeeList,
                        const xAOD::Vertex * constraint) const;

    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{
      this,
      "EventInfo",
      "EventInfo",
      "key for EventInfo retrieval"
    };

    ToolHandle<IMode1dFinder> m_mode1dfinder{ this,
                                              "Mode1dFinder",
                                              "Trk::FsmwMode1dFinder" };

    ToolHandle<ITrackToVertexIPEstimator> m_IPEstimator{
      this,
      "IPEstimator",
      "Trk::TrackToVertexIPEstimator"
    };

    bool m_disableAllWeights;
    float m_constraintcutoff;
    float m_constrainttemp;
    bool m_useLogPt;
    double m_minPt;
    bool m_usePt;
    double m_expPt;
    bool m_cacheWeights;


    struct Cache {
      typedef std::pair<Trk::Perigee, Amg::Vector2D> key_t;
      typedef std::pair<double, double> value_t;

      // functor to hash key for unordered_map
      struct hash_perigee {
        size_t operator()(const key_t& p) const
        {
          return 
            std::hash<double>()(p.first.parameters()[Trk::d0]) ^
            std::hash<double>()(p.first.parameters()[Trk::z0]) ^
            std::hash<double>()(p.first.parameters()[Trk::phi]) ^
            std::hash<double>()(p.first.parameters()[Trk::theta]) ^
            std::hash<double>()(p.first.parameters()[Trk::qOverP]) ^
            std::hash<double>()(p.second.x()) ^
            std::hash<double>()(p.second.y());
        }
      };

      // functor to compare two unordered_map Key values for equality
      struct pred_perigee {
        bool operator()(const key_t& left, const key_t& right) const
        {
          const AmgVector(5)& lparams = left.first.parameters();
          const AmgVector(5)& rparams = right.first.parameters();
          return 
            lparams[Trk::d0]     == rparams[Trk::d0] &&
            lparams[Trk::z0]     == rparams[Trk::z0] &&
            lparams[Trk::phi]    == rparams[Trk::phi] &&
            lparams[Trk::theta]  == rparams[Trk::theta] &&
            lparams[Trk::qOverP] == rparams[Trk::qOverP] &&
            left.second.x() == right.second.x() &&
            left.second.y() == right.second.y();
        }
      };


      // hashtable to avoid computing perigee more than once per track
      typedef std::unordered_map< key_t, value_t, hash_perigee, pred_perigee>
      WeightMap_t;
      WeightMap_t m_weightMap;

      EventContext::ContextEvt_t m_evt = -1;
      std::mutex m_mutex;
    };

    mutable SG::SlotSpecificObj<Cache> m_cache ATLAS_THREAD_SAFE;
  };
}
#endif
