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
//#include "AthenaKernel/IIOVDbSvc.h"
//#include "AthenaKernel/IIOVSvc.h"

#include "TRT_ToT_Tools/ITRT_ToT_dEdx.h"

#include "TrkTrack/Track.h"

//gas type selection
#include "TRT_ConditionsServices/ITRT_StrawStatusSummarySvc.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODEventInfo/EventInfo.h"
#include "TRT_ConditionsData/TRTDedxcorrection.h"


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

namespace InDetDD {
  class TRT_DetectorManager;
}
//namespace InDet {
// class TRT_DriftCircleOnTrack ;
// } 
//namespace Trk { class TrackParameters ; } 
class TRT_ToT_dEdx : virtual public ITRT_ToT_dEdx, public AthAlgTool 
{
public:
  IChronoStatSvc  *m_timingProfile;                                     // Timing measurements
  ServiceHandle<ITRT_StrawStatusSummarySvc> m_TRTStrawSummarySvc; 

public:
  //////////////////////////////////////////////////////////////////////////
  // enums
  //////////////////////////////////////////////////////////////////////////
  enum EDataBaseType {kOldDB,kNewDB};
  enum EstCalc  {kAlgStandard,kAlgScalingToXe,kAlgReweight,kAlgReweightTrunkOne};
  enum EToTEstimatorType {kToTLargerIsland,kToTHighOccupancy,kToTHighOccupancySmart};

private:
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this,"EventInfoKey","EventInfo","RHK to retrieve xAOD::EventInfo"};
  const TRT_ID* m_trtId;                                                // ID TRT helper 
  Trk::ParticleMasses        m_particlemasses;  
  const InDetDD::TRT_DetectorManager* m_trtman;                         // ID TRT detector manager 

  // Algorithm switchers
  bool m_corrected;                 // If true - make correction using rs-distributions
  bool m_divideByL;                 // If true - divide ToT to the L of track in straw.
  bool m_useHThits;                 // If true - use HT hit for dEdX estimator calculation

  int  m_whichToTEstimatorAlgo;     // If true - use getToTNewApproach(), else - use getToTlargerIsland()
  mutable int  m_useTrackPartWithGasType;   // If kUnset - use any gas for dEdX calculation;
  int  m_toolScenario;              // Algorithm type for dEdX estimator calculation:
  // kAlgStandard               - old dEdX estimator calculation algorithm;
  // kAlgScalingToXe            - mimicToXe other gastype hits and apply Xenon calibrations;
  // kAlgReweight               - calculate dEdX estimator using reweighting of separate gas types estimator using numbers of hits; truncate 1 max hit for each gas;
  // kAlgReweightTrunkOne       - same as previous, but truncate only one max hit on track;
  bool m_applyMimicToXeCorrection;  // Possibility to apply mimicToXenon function for any algorithm. For kAlgScalingToXe that always true.


  // Event info
  mutable bool m_isData;                  // Is Data?

  // Track info
  float m_trackConfig_maxRtrack;  // maximum track radius
  float m_trackConfig_minRtrack;  // maximum track radius

  bool m_useZeroRHitCut;  // skip tracks with where RHit=0

  // Hit info
  //    mutable EGasType m_gasTypeInStraw;                  // Starw gas type. 0:Xenon, 1:Argon, 2:Krypton
  mutable double m_L; // Length in straw

public:
  /** AlgTool like constructor */
  TRT_ToT_dEdx(const std::string&,const std::string&,const IInterface*);
  TRT_ToT_dEdx(const std::string& t, const std::string& n, const IInterface* p, 
               bool DivideByL, bool useHThits, bool corrected, bool useHighOccToTAlgo, 
               float minRtrack, float maxRtrack, bool useZeroRHitCut);
  
  /** Virtual destructor*/
  virtual ~TRT_ToT_dEdx();
  /** AlgTool initailize method.*/
  StatusCode initialize();
  /** AlgTool finalize method */
  StatusCode finalize();

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used 
   * @return ToT
   */
  double dEdx(const Trk::Track*, bool DivideByL, bool useHThits, bool corrected) const;

  /**
   * @brief function to calculate sum ToT normalised to number of used hits
   * @param track pointer
   * @return ToT
   */
  double dEdx(const Trk::Track*) const;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @param bool variable to decide wheter ToT or ToT/L should be used
   * @param bool variable whether HT hits shoule be used
   * @return nHits
   */
  double usedHits(const Trk::Track* track, bool DivideByL, bool useHThits) const;

  /**
   * @brief function to calculate number of used hits
   * @param track pointer
   * @return nHits
   */
  double usedHits(const Trk::Track* track) const;

  /** 
   * @brief function to define what is a good hit to be used for dEdx calculation
   * cuts on track level can be made later by the user
   * @param driftcircle object
   * @param track parameter object
   * @return decision
   */
  bool isGood_Hit(const Trk::TrackStateOnSurface *itr) const;
  bool isGood_Hit(const Trk::TrackStateOnSurface *itr, bool divideByL, bool useHThits) const;

  /**
   * @brief correct overall dEdx normalization on track level
   * @param dEdx definition (ToT or ToT/L)
   * @param scaling correction (needed for data)
   * @param number of primary vertices per event
   * @return scaling variable
   */
  double correctNormalization(bool divideLength, bool scaledata, double nVtx=-1) const;

  /**
   * @brief function to calculate likelihood from prediction and resolution
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param number of used hits
   * @return brobability  value between 0 and 1
   */
  double getProb(const Trk::TrackStateOnSurface *itr, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits) const;
  double getProb(const Trk::TrackStateOnSurface *itr, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool dividebyL) const;
  double getProb(EGasType gasType, const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, int nUsedHits, bool dividebyL) const;

  /**
   * @brief function to calculate likelihood ratio test
   * @param observed dEdx
   * @param track parameter
   * @param particle hypothesis
   * @param antihypothesis
   * @param number of used hits
   * @return test value between 0 and 1
   */
  double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits) const;
  double getTest(const double dEdx_obs, const double pTrk, Trk::ParticleHypothesis hypothesis, Trk::ParticleHypothesis antihypothesis, int nUsedHits, bool dividebyL) const;

  /**
   * @brief function to calculate expectation value for dEdx using BB fit
   * @param track momentum
   * @param hypothesis
   * @return dEdx_pred
   */
  double predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis, bool dividebyL) const;
  double predictdEdx(const Trk::TrackStateOnSurface *itr, const double pTrk, Trk::ParticleHypothesis hypothesis) const;

  double predictdEdx(EGasType gasType, const double pTrk, Trk::ParticleHypothesis hypothesis, bool dividebyL) const;



  /**
   * @brief function to extract most likely mass in bg [0,3]
   * @param track momentum
   * @param measured dEdx
   * @return mass
   */
  double mass(const Trk::TrackStateOnSurface *itr, const double pTrk, double dEdx ) const;

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
  double correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer,bool isData) const;

  double correctToT_corrRZL(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer,bool isData, bool useHThits) const;


  double correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer,bool isData) const;
  double correctToT_corrRZ(const Trk::TrackParameters* trkP,const InDet::TRT_DriftCircleOnTrack *driftcircle, int HitPart,int Layer,int StrawLayer,bool isData, bool useHThits) const;

  /**
   * @brief main function to correct ToT values on hit level as a function of track radius and z-position
   * @param track on surface object
   * @param bool variable to specify whether ToT or ToT/L correction
   * @param bool variable to specify whether data or MC correction
   * @param bool variable whether correction should actually be applied
   * @param bool variable whether mimic ToT to other gas hits shoule be used 
   * @return corrected value for ToT
   */
  double correctToT_corrRZ(const Trk::TrackStateOnSurface *itr) const;
  double correctToT_corrRZ(const Trk::TrackStateOnSurface *itr, bool divideByL, bool corrected) const;

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

  /**
   * @brief compute ToT time using special validity gate. Also take in account new LE and TE selection 
   * @param bitpattern
   * @return ToT
   */
  double getToTHighOccupancy(unsigned int BitPattern) const;

  /**
   * @brief LE for getToTHighOccupancy()
   * @param bitpattern
   * @return ToT
   */
  int DriftTimeBin_v2(unsigned int BitPattern) const;

  /**
   * @brief TE for getToTHighOccupancy()
   * @param bitpattern
   * @return ToT
   */
  int TrailingEdge_v2(unsigned int BitPattern) const;

  /**
   * @brief same as getToTHighOccupancy(), but use TrailingEdge_v3() with search of second TE
   * @param bitpattern
   * @return ToT
   */
  double getToTHighOccupancySmart(unsigned int BitPattern) const;

  /**
   * @brief TE for getToTHighOccupancy() with search second TE
   * @param bitpattern
   * @return ToT
   */
  int TrailingEdge_v3(unsigned int BitPattern) const;

  /**
   * @brief function to compute correction factor to mimic ToT Ar to the Xe
   * @param driftradius
   * @param straw layer index
   * @param sign for positive or negative side
   * @param gas type variable (0-Xe,1-Ar,2-Kr)
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double mimicToXeHit_Endcap(EGasType gasType, double driftRadius, int Layer, int sign) const;

  /**
   * @brief function to compute correction factor to mimic ToT Ar to the Xe
   * @param driftradius
   * @param layer index
   * @param straw layer index
   * @param gas type variable (0-Xe,1-Ar,2-Kr)
   * @param bool variable to specify whether data or MC correction
   * @return correction
   */
  double mimicToXeHit_Barrel(EGasType gasType, double driftRadius, int Layer, int Strawlayer) const;
    
  /**
   * @brief return gas type for that hit
   * @param track on surface object
   * @return gasType
   */
  EGasType gasTypeInStraw(const Trk::TrackStateOnSurface *itr) const; 
  EGasType gasTypeInStraw(const InDet::TRT_DriftCircleOnTrack *driftcircle) const; 
      
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
  double fitFuncEndcap_corrRZL(EGasType gasType, double driftRadius,double radialPosition, int Layer, int sign) const;
   
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
  double fitFuncBarrel_corrRZL(EGasType gasType, double driftRadius,double zPosition, int Layer, int StrawLayer) const;

  /**
   * @brief choose estimator algo using m_whichToTEstimatorAlgo and return ToT from bitpattern.
   * @param bitpattern
   * @return ToT
   */
  double getToT(unsigned int BitPattern) const;


public:
  // Setters and getters

  void SetDefaultConfiguration();

  // void  SwitchOnRSCorrection()                { m_corrected=true;           }
  // void  SwitchOffRSCorrection()               { m_corrected=false;          }
  bool  GetStatusRSCorrection() const         { return m_corrected;         }

  // void  SwitchOnDivideByL()                   { m_divideByL=true;           }
  // void  SwitchOffDivideByL()                  { m_divideByL=false;          }
  bool  GetStatusDivideByL() const            { return m_divideByL;         }

  // void  SwitchOnUseHThits()                   { m_useHThits=true;           }
  // void  SwitchOffUseHThits()                  { m_useHThits=false;          }
  bool  GetStatusUseHThits() const            { return m_useHThits;         }

  void  SetLargerIslandToTEstimatorAlgo()           { m_whichToTEstimatorAlgo=kToTLargerIsland;        }
  void  SetHighOccupancyToTEstimatorAlgo()          { m_whichToTEstimatorAlgo=kToTHighOccupancy;       }
  void  SetHighOccupancySmartToTEstimatorAlgo()     { m_whichToTEstimatorAlgo=kToTHighOccupancySmart;  }
  bool  GetStatusToTEstimatorAlgo() const           { return m_whichToTEstimatorAlgo;                  }


  void  SetMinRtrack(float minRtrack)         { m_trackConfig_minRtrack=minRtrack;}
  float GetMinRtrack() const                  { return m_trackConfig_minRtrack;   }

  void  SetMaxRtrack(float maxRtrack)         { m_trackConfig_maxRtrack=maxRtrack;}
  float GetMaxRtrack() const                  { return m_trackConfig_maxRtrack;   }

  void  SwitchOnUseZeroRHitCut()              { m_useZeroRHitCut=true;   }
  void  SwitchOffUseZeroRHitCut()             { m_useZeroRHitCut=false;  }
  bool  GetStatusUseZeroRHitCut() const       { return m_useZeroRHitCut; }

  void  SetXenonFordEdXCalculation()          { m_useTrackPartWithGasType=kXenon;   }
  void  SetArgonFordEdXCalculation()          { m_useTrackPartWithGasType=kArgon;   }
  void  SetKryptonFordEdXCalculation()        { m_useTrackPartWithGasType=kKrypton; }
  void  UnsetGasTypeFordEdXCalculation()      { m_useTrackPartWithGasType=kUnset;   }
  int   GetGasTypeFordEdXCalculation() const  { return m_useTrackPartWithGasType;   }

  // void  SetXenonGasTypeInStraw()              { m_gasTypeInStraw=kXenon;   }
  // void  SetArgonGasTypeInStraw()              { m_gasTypeInStraw=kArgon;   }
  // void  SetKryptonGasTypeInStraw()            { m_gasTypeInStraw=kKrypton; }
  // void  UnsetGasTypeInStraw()                 { m_gasTypeInStraw=kUnset;   }
  // int   GetStatusGasTypeInStraw() const       { return m_gasTypeInStraw;   }

  void  UseStandardAlgorithm()                { m_toolScenario=kAlgStandard;         }
  void  UseScalingAlgorithm()                 { m_toolScenario=kAlgScalingToXe; SwitchOnMimicToXeCorrection(); }
  void  UseReweightingAlgorithm()             { m_toolScenario=kAlgReweight;         }
  void  UseReweightingAlgorithmTrunkOne()     { m_toolScenario=kAlgReweightTrunkOne; }
  int   GetStatusAlgorithm() const            { return m_toolScenario;               }

  void  SwitchOnMimicToXeCorrection()         { m_applyMimicToXeCorrection=true;   }
  void  SwitchOffMimicToXeCorrection()        { m_applyMimicToXeCorrection=false;  }
  bool  GetStatusMimicToXeCorrection() const  { return m_applyMimicToXeCorrection; }


  void  ShowDEDXSetup() const;

private:
  bool isData() const;
  mutable bool m_isDataSet;

  
};

#endif // TRT_TOT_DEDX_H
