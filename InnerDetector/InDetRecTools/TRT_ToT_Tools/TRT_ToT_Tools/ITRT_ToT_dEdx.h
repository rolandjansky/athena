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
//namespace Trk { class ParticleHypothesis ; } 

//namespace Trk { class TrackParameters ; } 

namespace InDet { class TRT_DriftCircleOnTrack ; } 
//#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"

static const InterfaceID IID_ITRT_ToT_dEdx("ITRT_ToT_dEdx", 1, 0);
  
class ITRT_ToT_dEdx : virtual public IAlgTool {
  
 public:

  /** Virtual destructor */
  virtual ~ITRT_ToT_dEdx(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITRT_ToT_dEdx; };
  


  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer
   * @param bool variable to specify whether data or MC correction
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used
   * @return ToT
   */
  virtual double dEdx(const Trk::Track*, bool data, bool DivideByL, bool useHThits= true, bool corrected = true, int nVtx=-1) const = 0;

  /** 
   * @brief function to define what is a good hit to be used for dEdx calculation
   * cuts on track level can be made later by the user
   * @param driftcircle object
   * @param track parameter object
   * @return decision
   */
  virtual bool isGood_Hit(const Trk::TrackStateOnSurface *itr, bool useHThits) const = 0;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used
   * @return nHits
   */
  virtual double usedHits(const Trk::Track* track, bool DivideByL, bool useHThits) const = 0;

  /**
   * @brief correct overall dEdx normalization on track level
   * @param dEdx definition (ToT or ToT/L)
   * @param scale value (needed for data)
   * @param number of primary vertices per event
   * @return scaling variable
   */
  virtual double correctNormalization(bool divideLength,bool scaledata, int nVtx=-1) const = 0;


  /**
   * @brief function to calculate likelihood from prediction and resolution
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param number of used hits
   * @return brobability  value between 0 and 1
   */
  virtual double getProb(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool dividebyL=true) const = 0;

  /**
   * @brief function to calculate likelihood ratio test
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param antihypothesis
   * @param number of used hits
   * @return test value between 0 and 1
   */
  virtual double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits, bool dividebyL=true) const = 0;


  /**
   * @brief function to calculate expectation value for dEdx using BB fit
   * @param track momentum
   * @param hypothesis
   * @return dEdx_pred
   */
  virtual double predictdEdx(const double pTrk, Trk::ParticleHypothesis hypothesis, bool dividebyL=true)const = 0;

  /**
   * @brief function to extract most likely mass in bg [0,3]
   * @param track momentum
   * @param measured dEdx
   * @return mass
   */
  virtual double mass(const double pTrk, double dEdx )const = 0;



  /**
   * @brief function to correct ToT/L used by the PIDTool parameters obtimized to be consistend with existing functions
   * @param track parameter object
   * @param trift circle object
   * @param number to decide whether it is barrel or endcap
   * @param number to identify layer ID
   * @param number to identify strawlayer id
   * @param bool to set data or MC
   * @return corrected ToT/L (returns 0 if hit criteria are not fulfilled)
   */
  virtual double correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool data) const = 0;
 
  virtual double correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool data) const = 0;
 
  /**
   * @brief main function to correct ToT values on hit level as a function of track radius and z-position
   * @param track on surface object
   * @param bool variable to specify whether ToT or ToT/L correction
   * @param bool variable to specify whether data or MC correction
   * @param bool variable whether correction should actually be applied
   * @return corrected value for ToT
   */
  virtual double correctToT_corrRZ(const Trk::TrackStateOnSurface *itr, bool DividebyL, bool data, bool corrected) const = 0;


  /**
   * @brief compute ToT time for largest island
   * @param bitpattern
   * @return ToT
   */
  virtual double getToTlargerIsland(unsigned int BitPattern) const = 0;
  virtual double getToTonly1bits(unsigned int BitPattern) const = 0;

};

#endif // ITRT_TOT_DEDX_H
