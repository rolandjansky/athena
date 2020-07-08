/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  enum EGasType {kXenon,kArgon,kKrypton,kUnset};
  enum EOccupancyCorrection{kRSOnly, kHitBased, kTrackBased, kGlobal};
  
  /** Virtual destructor */
  virtual ~ITRT_ToT_dEdx(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITRT_ToT_dEdx; };

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer to track
   * @param correctionType choice of occupancy correction
   * @return dEdx value
   */
  virtual double dEdx(const Trk::Track* track, EOccupancyCorrection correctionType=EOccupancyCorrection::kTrackBased) const = 0;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @return nHits
   */
  virtual double usedHits(const Trk::Track* track) const = 0;

  /**
   * @brief function to calculate likelihood from prediction and resolution
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param number of used hits
   * @return brobability  value between 0 and 1
   */
  virtual double getProb(const Trk::TrackStateOnSurface *itr, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits) const = 0;

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


  /**
   * @brief function to calculate expectation value for dEdx using BB fit
   * @param track momentum
   * @param hypothesis
   * @return dEdx_pred
   */
  virtual double predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis) const = 0;

  /**
   * @brief function to extract most likely mass in bg [0,3]
   * @param track momentum
   * @param measured dEdx
   * @return mass
   */
  virtual double mass(const Trk::TrackStateOnSurface *itr, const double pTrk, double dEdx ) const = 0;
};

#endif // ITRT_TOT_DEDX_H
