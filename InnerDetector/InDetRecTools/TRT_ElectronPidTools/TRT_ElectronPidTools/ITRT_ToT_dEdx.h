/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITRT_TOT_DEDX_H
#define ITRT_TOT_DEDX_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "TrkParameters/TrackParameters.h"  // typedef

namespace Trk { class Track ; } 
namespace Trk { class TrackStateOnSurface ; } 
namespace InDet { class TRT_DriftCircleOnTrack ; }

static const InterfaceID IID_ITRT_ToT_dEdx("ITRT_ToT_dEdx", 1, 0);
  
class ITRT_ToT_dEdx : virtual public IAlgTool {

  
public:
  
  /** Virtual destructor */
  virtual ~ITRT_ToT_dEdx(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITRT_ToT_dEdx; };

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param Event context ctx
   * @param track pointer to track
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @return dEdx value
   */
  virtual double dEdx(const EventContext& ctx,
                      const Trk::Track* track,
                      bool useHitsHT = true) const = 0;
  
  virtual double dEdx(const Trk::Track* track, bool useHitsHT = true) const ;
  
  /**
   * @brief function to calculate number of used hits
   * @param Event context ctx
   * @param track pointer
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @return nHits
   */
  virtual double usedHits(const EventContext& ctx,
                          const Trk::Track* track,
                          bool useHitsHT = true) const = 0;

  virtual double usedHits(const Trk::Track* track,
                          bool useHitsHT = true) const;


  /**
   * @brief function to calculate likelihood ratio test
   * @param Event context ctx
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param antihypothesis
   * @param number of used hits
   * @return test value between 0 and 1
   */
  virtual double getTest(const EventContext& ctx,
                         const double dEdx_obs,
                         const double pTrk,
                         Trk::ParticleHypothesis hypothesis,
                         Trk::ParticleHypothesis antihypothesis,
                         int nUsedHits) const = 0;

  virtual double getTest(const double dEdx_obs,
                         const double pTrk,
                         Trk::ParticleHypothesis hypothesis,
                         Trk::ParticleHypothesis antihypothesis,
                         int nUsedHits) const;
};

// implement the non ctx aware methods in terms of the ctx
// aware ones
inline double
ITRT_ToT_dEdx::dEdx(const Trk::Track* track, bool useHitsHT) const
{
  return dEdx(Gaudi::Hive::currentContext(), track, useHitsHT);
}

inline double
ITRT_ToT_dEdx::usedHits(const Trk::Track* track, bool useHitsHT) const
{
  return usedHits(Gaudi::Hive::currentContext(), track, useHitsHT);
}

inline double
ITRT_ToT_dEdx::getTest(const double dEdx_obs,
                       const double pTrk,
                       Trk::ParticleHypothesis hypothesis,
                       Trk::ParticleHypothesis antihypothesis,
                       int nUsedHits) const
{
  return getTest(Gaudi::Hive::currentContext(),
                 dEdx_obs,
                 pTrk,
                 hypothesis,
                 antihypothesis,
                 nUsedHits);
}

#endif // ITRT_TOT_DEDX_H
