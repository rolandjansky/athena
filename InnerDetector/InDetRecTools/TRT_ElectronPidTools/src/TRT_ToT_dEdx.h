/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_TOT_DEDX_H
#define TRT_TOT_DEDX_H

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h" //MJ
#include "GaudiKernel/IToolSvc.h"  //MJ
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "StoreGate/ReadDecorHandleKey.h"

#include "TRT_ElectronPidTools/ITRT_ToT_dEdx.h"

#include "TrkTrack/Track.h"

//gas type selection
#include "TRT_ConditionsServices/ITRT_StrawStatusSummaryTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "TRT_ConditionsData/TRTDedxcorrection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"

#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TRT_ElectronPidTools/ITRT_LocalOccupancy.h"

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
class ITRT_StrawSummaryTool;

namespace InDet {
   class ITRT_LocalOccupancy;
}

//namespace InDet {
// class TRT_DriftCircleOnTrack ;
// } 
//namespace Trk { class TrackParameters ; } 
class TRT_ToT_dEdx : virtual public ITRT_ToT_dEdx, public AthAlgTool 
{
public:
  IChronoStatSvc  *m_timingProfile;                                     // Timing measurements
  ToolHandle<ITRT_StrawStatusSummaryTool> m_TRTStrawSummaryTool; 

public:
  //////////////////////////////////////////////////////////////////////////
  // enums
  //////////////////////////////////////////////////////////////////////////
  enum EDataBaseType {kOldDB,kNewDB};
  enum EstCalc  {kAlgStandard,kAlgReweight,kAlgReweightTrunkOne};
  enum EOccupancyCorrection{kRSOnly, kHitBased, kTrackBased, kGlobal};
  enum EGasType {kXenon,kArgon,kKrypton,kUnset};

private:
  SG::ReadDecorHandleKey<xAOD::EventInfo> m_rdhkEvtInfo {this
      , "AveIntPerXKey"
      , "EventInfo.AveIntPerXDecor"
      , "Decoration for Average Interaction Per Crossing"};
  const TRT_ID* m_trtId;                                                // ID TRT helper 
  Trk::ParticleMasses        m_particlemasses;

  ToolHandle< Trk::IPRD_AssociationTool >  m_assoTool;
  ToolHandle< InDet::ITRT_LocalOccupancy > m_localOccTool;     //!< the track selector tool

  // Algorithm switchers

  bool m_corrected;                 // If true - make correction using rs-distributions
  bool m_divideByL;                 // If true - divide ToT to the L of track in straw.

  int  m_useTrackPartWithGasType;   // If kUnset - use any gas for dEdX calculation;
  int  m_toolScenario;              // Algorithm type for dEdX estimator calculation;
  int  m_correctionType;            // Type of dEdx correction
  ////////////////////////////////////////////////////
  // Different cases for correctionType            //
  // kRSOnly: only r-S calibration                  //
  // kHitBased: Hit-based occupancy calibration     //
  // kTrackBased: Track-based occupancy calibration //
  // kGlobal: Global occupancy calibration          //
  ////////////////////////////////////////////////////


  // Event info
  bool m_isData;                  // Is Data?

  // Track info
  float m_trackConfig_maxRtrack;  // maximum track radius
  float m_trackConfig_minRtrack;  // maximum track radius

  bool m_useZeroRHitCut;  // skip tracks with where RHit=0

  unsigned int m_nTrunkateHits = 1;


public:
  /** AlgTool like constructor */
  TRT_ToT_dEdx(const std::string&,const std::string&,const IInterface*);
  
  /** Virtual destructor*/
  virtual ~TRT_ToT_dEdx();
  /** AlgTool initailize method.*/
  virtual StatusCode initialize() override;
  /** AlgTool finalize method */
  virtual StatusCode finalize() override;

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer to track
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @return dEdx value
   */
  using ITRT_ToT_dEdx::dEdx;
  using ITRT_ToT_dEdx::usedHits;
  using ITRT_ToT_dEdx::getTest;
  virtual double dEdx(const EventContext& ctx,
                      const Trk::Track* track,
                      bool useHThits = true) const override final;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @return nHits
   */
  virtual double usedHits(const EventContext& ctx,
                          const Trk::Track* track,
                          bool useHThits = true) const override final;
  /**
   * @brief function to calculate likelihood ratio test
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
                         int nUsedHits) const override final;

protected:
  /** 
   * @brief function to define what is a good hit to be used for dEdx calculation
   * cuts on track level can be made latekAlgStandardr by the user. Also returns the length in the straw.
   * @param trackState measurement on track
   * @param useHitsHT decide if HT hits should be used in the estimate
   * @param length length in straw
   * @return decision
   */
  bool isGoodHit(const EventContext& ctx,
                 const Trk::TrackStateOnSurface* trackState,
                 bool useHitsHT,
                 double& length) const;

  /**
   * @brief correct overall dEdx normalization on track level
   * @param number of primary vertices per event
   * @return scaling variable
   */
  double correctNormalization(const EventContext& ctx, double nVtx = -1) const;

  /**
   * @brief function to calculate likelihood from prediction and resolution
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param number of used hits
   * @return brobability  value between 0 and 1
   */
  double getProb(const EventContext& ctx,
                 const Trk::TrackStateOnSurface* itr,
                 const double dEdx_obs,
                 const double pTrk,
                 Trk::ParticleHypothesis hypothesis,
                 int nUsedHits) const;
  double getProb(const EventContext& ctx,
                 EGasType gasType,
                 const double dEdx_obs,
                 const double pTrk,
                 Trk::ParticleHypothesis hypothesis,
                 int nUsedHits) const;

  /**
   * @brief function to calculate expectation value for dEdx using BB fit
   * @param track momentum
   * @param hypothesis
   * @return dEdx_pred
   */
  double predictdEdx(const EventContext& ctx,
                     const Trk::TrackStateOnSurface* itr,
                     const double pTrk,
                     Trk::ParticleHypothesis hypothesis) const;

  double predictdEdx(const EventContext& ctx,
                     EGasType gasType,
                     const double pTrk,
                     Trk::ParticleHypothesis hypothesis) const;

  /**
   * @brief function to extract most likely mass in bg [0,3]
   * @param track momentum
   * @param measured dEdx
   * @return mass
   */
  double mass(const EventContext& ctx,
              const Trk::TrackStateOnSurface* itr,
              const double pTrk,
              double dEdx) const;

  /**
   * @brief main function to correct ToT values on hit level as a function of track radius and z-position
   * @param track on surface object
   * @param bool variable to specify whether ToT or ToT/L correction
   * @param bool variable to specify whether data or MC correction
   * @param bool variable whether correction should actually be applied
   * @param bool variable whether mimic ToT to other gas hits shoule be used 
   * @return corrected value for ToT
   */
  double correctToT_corrRZ(const EventContext& ctx,
                           const Trk::TrackStateOnSurface* itr,
                           double length) const;

  /**
   * @brief return gas type for that hit
   * @param track on surface object
   * @return gasType
   */
  EGasType gasTypeInStraw(const EventContext& ctx,
                          const Trk::TrackStateOnSurface* itr) const;
  EGasType gasTypeInStraw(
    const EventContext& ctx,
    const InDet::TRT_DriftCircleOnTrack* driftcircle) const;

private:

  SG::ReadCondHandleKey<TRTDedxcorrection> m_ReadKey{this,"Dedxcorrection","Dedxcorrection","Dedx constants in-key"};   

  /**
   * @brief function to compute correction factor in endcap region
   * @param driftradius
   * @param radial position
   * @param straw layer index
   * @param sign for positive or negative side
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double fitFuncEndcap_corrRZ(EGasType gas, double driftRadius, double rPosition, int Layer, int sign) const;
  /**
   * @brief function to compute correction factor in barrel region
   * @param driftradius
   * @param z position
   * @param layer index
   * @param straw layer index
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double fitFuncBarrel_corrRZ(EGasType gas, double driftRadius,double zPosition,int Layer, int StrawLayer) const;

  /**
   * @brief function called by fitFuncBarrel_corrRZ for long straws
   */
  double fitFuncBarrelLong_corrRZ(EGasType gasType, double driftRadius,double zPosition,int Layer, int StrawLayer) const;

  /**
   * @brief function called by fitFuncBarrel_corrRZ for short straws
   */
  double fitFuncBarrelShort_corrRZ(EGasType gasType, double driftRadius,double zPosition, int StrawLayer) const;

  /**
   * @brief function called by fitFuncBarrel_corrRZ and fitFuncEndcap_corrRZ
   */
  double fitFuncPol_corrRZ(EGasType gasType, int parameter, double driftRadius, int Layer, int Strawlayer, int sign, int set) const;

  /**
   * @brief function to compute correction factor in endcap region
   * @param driftradius
   * @param radial position
   * @param straw layer index
   * @param sign for positive or negative side
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double fitFuncEndcap_corrRZL(const EventContext& ctx,
                               EGasType gasType,
                               double driftRadius,
                               double radialPosition,
                               int Layer,
                               int sign) const;

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
  double fitFuncBarrel_corrRZL(const EventContext& ctx,
                               EGasType gasType,
                               double driftRadius,
                               double zPosition,
                               int Layer,
                               int StrawLayer) const;

  /* Calibration functions for occupancy corrections */
  double hitOccupancyCorrection(const EventContext& ctx,
                                const Trk::TrackStateOnSurface* itr) const;
  double trackOccupancyCorrection(const Trk::Track* track,
                                  bool useHThits) const;

public:
  // Setters and getters

  void  setDefaultConfiguration();

  void  setStatusCorrection(bool value)       { m_corrected = value;        }
  bool  getStatusCorrection() const           { return m_corrected;         }

  void  setCorrectionType(EOccupancyCorrection value)  { m_correctionType = value;        }
  bool  getCorrectionType() const                      { return m_correctionType;         }

  void  setMinRtrack(float minRtrack)         { m_trackConfig_minRtrack=minRtrack;}
  float getMinRtrack() const                  { return m_trackConfig_minRtrack;   }

  void  setMaxRtrack(float maxRtrack)         { m_trackConfig_maxRtrack=maxRtrack;}
  float getMaxRtrack() const                  { return m_trackConfig_maxRtrack;   }

  void  setStatusUseZeroRHitCut(bool value)   { m_useZeroRHitCut = value; }
  bool  getStatusUseZeroRHitCut() const       { return m_useZeroRHitCut; }

  void  setGasTypeFordEdXCalculation(EGasType gasType) { m_useTrackPartWithGasType = gasType; }
  int   getGasTypeFordEdXCalculation() const           { return m_useTrackPartWithGasType;    }

  void  setAlgorithm(EstCalc alg)             { m_toolScenario=alg;    }
  int   getAlgorithm() const                  { return m_toolScenario; }

  void  showDEDXSetup() const;

// static methods
static double calculateTrackLengthInStraw(const Trk::TrackStateOnSurface* trackState, const TRT_ID* identifier);
  
};

#endif // TRT_TOT_DEDX_H
