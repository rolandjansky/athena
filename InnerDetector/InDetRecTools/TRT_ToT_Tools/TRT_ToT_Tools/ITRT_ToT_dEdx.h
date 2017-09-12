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

  enum EGasType {kXenon,kArgon,kKrypton,kUnset};

  /** Virtual destructor */
  virtual ~ITRT_ToT_dEdx(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_ITRT_ToT_dEdx; };

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used 
   * @return ToT
   */
  virtual double dEdx(const Trk::Track*, bool DivideByL, bool useHThits, bool corrected ) const = 0;
  virtual double dEdx(const Trk::Track*) const = 0;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used
   * @return nHits
   */
  virtual double usedHits(const Trk::Track* track, bool DivideByL, bool useHThits) const = 0;
  virtual double usedHits(const Trk::Track* track) const = 0;

  /** 
   * @brief function to define what is a good hit to be used for dEdx calculation
   * cuts on track level can be made later by the user
   * @param driftcircle object
   * @param track parameter object
   * @return decision
   */
  virtual bool isGood_Hit(const Trk::TrackStateOnSurface *itr) const = 0;

  /**
   * @brief correct overall dEdx normalization on track level
   * @param dEdx definition (ToT or ToT/L)
   * @param scale value (needed for data)
   * @param number of primary vertices per event
   * @return scaling variable
   */
  virtual double correctNormalization(bool divideLength, bool scaledata, double nVtx=-1) const = 0;

  /**
   * @brief function to calculate likelihood from prediction and resolution
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param number of used hits
   * @return brobability  value between 0 and 1
   */
  virtual double getProb(const Trk::TrackStateOnSurface *itr, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool dividebyL) const = 0;
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
  virtual double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits, bool dividebyL) const = 0;
  virtual double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits) const = 0;


  /**
   * @brief function to calculate expectation value for dEdx using BB fit
   * @param track momentum
   * @param hypothesis
   * @return dEdx_pred
   */
  virtual double predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis, bool dividebyL) const = 0;
  virtual double predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis) const = 0;

  /**
   * @brief function to extract most likely mass in bg [0,3]
   * @param track momentum
   * @param measured dEdx
   * @return mass
   */
  virtual double mass(const Trk::TrackStateOnSurface *itr, const double pTrk, double dEdx ) const = 0;



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
  virtual double correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer,bool isData) const = 0;
 
  virtual double correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer,bool isData) const = 0;
 
  /**
   * @brief main function to correct ToT values on hit level as a function of track radius and z-position
   * @param track on surface object
   * @param bool variable to specify whether ToT or ToT/L correction
   * @param bool variable to specify whether data or MC correction
   * @param bool variable whether correction should actually be applied
   * @param bool variable whether mimic ToT to other gas hits shoule be used 
   * @return corrected value for ToT
   */
  virtual double correctToT_corrRZ(const Trk::TrackStateOnSurface *itr) const = 0;


  /**
   * @brief compute ToT time for largest island
   * @param bitpattern
   * @return ToT
   */
  virtual double getToTlargerIsland(unsigned int BitPattern) const = 0;
  virtual double getToTonly1bits(unsigned int BitPattern) const = 0;
  virtual double getToTHighOccupancy(unsigned int BitPattern) const = 0;
  virtual double getToTHighOccupancySmart(unsigned int BitPattern) const = 0;

  /**
   * @brief return gas type for that hit
   * @param track on surface object
   * @return gasType
   */
  virtual EGasType gasTypeInStraw(const Trk::TrackStateOnSurface *itr) const = 0; 
  virtual EGasType gasTypeInStraw(const InDet::TRT_DriftCircleOnTrack *driftcircle) const = 0; 

  /**
   * @brief setters and getters
   */
  virtual void  SetDefaultConfiguration() = 0;

  // virtual void  SwitchOnRSCorrection() = 0;
  // virtual void  SwitchOffRSCorrection() = 0;
  virtual bool  GetStatusRSCorrection() const = 0;

  // virtual void  SwitchOnDivideByL() = 0;
  // virtual void  SwitchOffDivideByL() = 0;
  virtual bool  GetStatusDivideByL() const = 0;

  // virtual void  SwitchOnUseHThits() = 0;
  // virtual void  SwitchOffUseHThits() = 0;
  virtual bool  GetStatusUseHThits() const = 0;

  virtual void  SetLargerIslandToTEstimatorAlgo() = 0;
  virtual void  SetHighOccupancyToTEstimatorAlgo() = 0;
  virtual void  SetHighOccupancySmartToTEstimatorAlgo() = 0;
  virtual bool  GetStatusToTEstimatorAlgo() const = 0;


  virtual void  SetMinRtrack(float minRtrack) = 0;
  virtual float GetMinRtrack() const = 0;

  virtual void  SetMaxRtrack(float maxRtrack) = 0;
  virtual float GetMaxRtrack() const = 0;

  virtual void  SwitchOnUseZeroRHitCut() = 0;
  virtual void  SwitchOffUseZeroRHitCut() = 0;
  virtual bool  GetStatusUseZeroRHitCut() const = 0;

  virtual void  SetXenonFordEdXCalculation() = 0;
  virtual void  SetArgonFordEdXCalculation() = 0;
  virtual void  SetKryptonFordEdXCalculation() = 0;
  virtual void  UnsetGasTypeFordEdXCalculation() = 0;
  virtual int   GetGasTypeFordEdXCalculation() const = 0;

  // virtual void  SetXenonGasTypeInStraw() = 0;
  // virtual void  SetArgonGasTypeInStraw() = 0;
  // virtual void  SetKryptonGasTypeInStraw() = 0;
  // virtual void  UnsetGasTypeInStraw() = 0;
  // virtual int   GetStatusGasTypeInStraw() const = 0;

  virtual void  UseStandardAlgorithm() = 0;
  virtual void  UseScalingAlgorithm() = 0;
  virtual void  UseReweightingAlgorithm() = 0;
  virtual void  UseReweightingAlgorithmTrunkOne() = 0;
  virtual int   GetStatusAlgorithm() const = 0;

  virtual void  SwitchOnMimicToXeCorrection() = 0;
  virtual void  SwitchOffMimicToXeCorrection() = 0;
  virtual bool  GetStatusMimicToXeCorrection() const = 0;

};

#endif // ITRT_TOT_DEDX_H
