/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_TOT_DEDX_H
#define TRT_TOT_DEDX_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h" //MJ
#include "GaudiKernel/IToolSvc.h"  //MJ
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"

#include "TrkTrack/Track.h"

/*
  Tool to calculate dE/dx variable for PID
  o Variable is based on ToT of hits
  o ToT is calculated from bitpattern using the largest island
  o Two version availlable (ToT and ToT/L averaged over used hits)
  o Corrections availlable for Data and MC for both versions defined above
  o Parameters for correction are currently stored in TRT_ToT_Corrections namespace 
    should probably go to a database 
*/

class TRT_ID;
class IChronoStatSvc;

namespace InDetDD {
  class TRT_DetectorManager;
}
//namespace InDet {
// class TRT_DriftCircleOnTrack ;
// } 
//namespace Trk { class TrackParameters ; } 
class TRT_ToT_dEdx : public AlgTool, virtual public ITRT_ToT_dEdx 
{

 public:
  /** AlgTool like constructor */
  TRT_ToT_dEdx(const std::string&,const std::string&,const IInterface*);
  /** Virtual destructor*/
  virtual ~TRT_ToT_dEdx();
  /** AlgTool initailize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  //---------------------
  //  Timing measurements
  //   
  IChronoStatSvc  *m_timingProfile;


  //bool m_DATA;

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer
   * @param bool variable to specify whether data or MC correction
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used
   * @return ToT
   */
  double dEdx(const Trk::Track*, bool data, bool DivideByL, bool useHThits= true, bool corrected = true, int nVtx = -1) const;


  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used
   * @return nHits
   */
  double usedHits(const Trk::Track* track, bool DivideByL, bool useHThits) const;


  /**
   * @brief function to calculate likelihood from prediction and resolution
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param number of used hits
   * @return brobability  value between 0 and 1
   */
  double getProb(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool dividebyL) const;

  /**
   * @brief function to calculate likelihood ratio test
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param antihypothesis
   * @param number of used hits
   * @return test value between 0 and 1
   */
  double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits, bool dividebyL) const;


  /**
   * @brief function to calculate expectation value for dEdx using BB fit
   * @param track momentum
   * @param hypothesis
   * @return dEdx_pred
   */
  double predictdEdx(const double pTrk, Trk::ParticleHypothesis hypothesis, bool dividebyL)const;

  /**
   * @brief function to extract most likely mass in bg [0,3]
   * @param track momentum
   * @param measured dEdx
   * @return mass
   */
  double mass(const double pTrk, double dEdx )const;


  /** 
   * @brief function to define what is a good hit to be used for dEdx calculation
   * cuts on track level can be made later by the user
   * @param driftcircle object
   * @param track parameter object
   * @return decision
   */
  bool isGood_Hit(const Trk::TrackStateOnSurface *itr, bool useHThits) const;

  /**
   * @brief correct overall dEdx normalization on track level
   * @param dEdx definition (ToT or ToT/L)
   * @param scaling correction (needed for data)
   * @param number of primary vertices per event
   * @return scaling variable
   */
  double correctNormalization(bool divideLength,bool scaleData, int nVtx=-1) const;


   /**
   * @brief main function to correct ToT values on hit level as a function of track radius and z-position
   * @param track on surface object
   * @param bool variable to specify whether ToT or ToT/L correction
   * @param bool variable to specify whether data or MC correction
   * @param bool variable whether correction should actually be applied
   * @return corrected value for ToT
   */
  double correctToT_corrRZ(const Trk::TrackStateOnSurface *itr, bool DividebyL, bool data, bool corrected) const;

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
  double correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool data) const;

  double correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer, bool data) const;

  /**
   * @brief compute ToT time for largest island
   * @param bitpattern
   * @return ToT
   */
  double getToTlargerIsland(unsigned int BitPattern) const;

  /**
   * @brief compute ToT time counting only 1 bits
   * @param bitpattern
   * @return ToT
   */
  double getToTonly1bits(unsigned int BitPattern) const;
 private:

  /** ID TRT helper */
  const TRT_ID* m_trtId;
  Trk::ParticleMasses        m_particlemasses;

  /** ID TRT detector manager */
  const InDetDD::TRT_DetectorManager* m_trtman;

  /**
   * @brief function to compute correction factor in endcap region
   * @param driftradius
   * @param radial position
   * @param straw layer index
   * @param sign for positive or negative side
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double fitFuncEndcap_corrRZ(double driftRadius, double rPosition, int Layer, int sign, bool data) const;
  /**
   * @brief function to compute correction factor in barrel region
   * @param driftradius
   * @param z position
   * @param layer index
   * @param straw layer index
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double fitFuncBarrel_corrRZ(double driftRadius,double zPosition,int Layer, int StrawLayer, bool data) const;

  /**
   * @brief function called by fitFuncBarrel_corrRZ for long straws
   */
  double fitFuncBarrelLong_corrRZ(double driftRadius,double zPosition,int Layer, int StrawLayer, bool data) const;

  /**
   * @brief function called by fitFuncBarrel_corrRZ for short straws
   */
  double fitFuncBarrelShort_corrRZ(double driftRadius,double zPosition, int StrawLayer, bool data) const;

  /**
   * @brief function called by fitFuncBarrel_corrRZ and fitFuncEndcap_corrRZ
   */
  double fitFuncPol_corrRZ(int parameter, double driftRadius, int Layer, int Strawlayer, int sign, int set, bool data) const;


  /**
   * @brief function called by fitFuncBarrel_corrRZ and fitFuncEndcap_corrRZ
   */
  //  double fitFuncPol_corrRZL(int parameter, double driftRadius, int Layer, int Strawlayer, int sign, int set, bool data) const;
 
 /**
   * @brief function to compute correction factor in endcap region
   * @param driftradius
   * @param radial position
   * @param straw layer index
   * @param sign for positive or negative side
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double fitFuncEndcap_corrRZL(double driftRadius,double radialPosition, int Layer, int sign, bool data) const;
 
 /**  
   * @brief function to compute correction factor in barrel region  
   * @param driftradius
   * @param z position 
   * @param layer index  
   * @param straw layer index  
   * @param desired version of ToT definition
   * @param bool variable to specify whether data or MC correction  
   * @return correction
   */
  double fitFuncBarrel_corrRZL(double driftRadius,double zPosition, int Layer, int StrawLayer, bool data) const;


};

#endif // TRT_TOT_DEDX_H
