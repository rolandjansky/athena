/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITRT_TOT_DEDX_H
#define ITRT_TOT_DEDX_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
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
   * @param track pointer to track
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @return dEdx value
   */
  virtual double dEdx(const Trk::Track* track, bool useHitsHT=true) const = 0;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @return nHits
   */
  virtual double usedHits(const Trk::Track* track, bool useHitsHT=true) const = 0;

  /**
   * @brief function to calculate likelihood ratio test
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param antihypothesis
   * @param number of used hits
   * @return test value between 0 and 1
   */
  virtual double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits) const = 0;
};

#endif // ITRT_TOT_DEDX_H
