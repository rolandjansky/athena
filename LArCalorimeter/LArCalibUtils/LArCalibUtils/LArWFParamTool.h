//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARWFPARAMTOOL_H
#define LARWFPARAMTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h" 

#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArWaveHelper.h" 

static const InterfaceID IID_LArWFParamTool("LArWFParamTool", 1 , 0); 

class LArCablingSvc;
class LArOnlineID;
class LArEM_ID;

class LArWFParamTool : public AthAlgTool
{

public: 

  enum {
    FailExtract = -999 ,
    DoExtract ,
    DefaultNotSet = 0
  } ;
  
  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArWFParamTool; }

  // constructor
  LArWFParamTool(const std::string& type, const std::string& name, const IInterface* parent ) ;
  
  // destructor 
  virtual ~LArWFParamTool(); 
  
  virtual StatusCode initialize();
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}
  
  // this is supposed to be a quick test on cali waves, to flags those 
  // clearly problematic. but cuts must be tuned, so for the time being 
  // don't call this method!
  //unsigned checkStatus(const LArCaliWave &larCaliWave) const ;
  
			      
  StatusCode getLArWaveParams(const LArCaliWave& larCaliWave, 
			      const HWIdentifier chid, 
			      const CaloGain::CaloGain gain,
			      LArWFParams& wfParams,
			      LArCaliWave* omegaScan=NULL,
			      LArCaliWave* resOscill0=NULL,
			      LArCaliWave* resOscill1=NULL
			      ) const;
 
  
  enum { OK           = 0 ,
	 TooShort     = 1 ,
	 NotFilled    = 2 ,
	 LowStat      = 3 , 
	 Noisy        = 4 ,
	 Oscillating  = 5 
  } ;
  
private:

  static const double m_DEFAULT;
  static const double m_TINY;
  static const double m_SQRT_EPSILON;
  static const double m_EPSILON;


  struct WaveTiming_t {
    unsigned Tstart;
    unsigned Tpeak;
    unsigned Tcross;
    unsigned Tmin;
    //unsigned Ttail;
    double Amplitude;
  };


  ToolHandle<LArCablingService> m_larCablingSvc;
  const LArEM_ID* m_emId;
  const LArOnlineID* m_onlineHelper;

  LArWaveHelper m_wHelper;


  double m_Tshaper;  
 
  std::vector<bool> m_cosRespScan ;
  std::vector<double> m_omegamin;
  std::vector<double> m_omegamax;
  std::vector<unsigned> m_npoints ;
  
  //AlgTool properties:
  std::vector<bool> m_storeResOscill;
  unsigned int m_NBaseline;
  bool m_ShiftToStart;
  bool m_SubtractBaseline;
  bool m_UseOmegaScanHelper;
  std::vector<int> m_TtailMin;
  std::vector<int> m_TtailMax;
  std::vector<int> m_DeltaTtail; 


  //Internal data structures
  struct waveRange_t {
    unsigned min;
    unsigned max;
  };

  struct omegaScanParams_t {
    bool cosRespScan;
    double omegamin;
    double omegamax;
    int Npoint; 
    double DeltaOmega;
    waveRange_t range;
  };


  WaveTiming_t timeExtr(LArWave& gCali) const;
  double expTail(const LArWave& gCali, const WaveTiming_t& wt) const;
  StatusCode GetFstep(const LArWave& gCali, LArWFParams& wfParams, const WaveTiming_t& wt) const;
  double fstepMin (const LArWave& gCali, double fstep, const double Tc, const unsigned Ttail) const;
  double dFstep (const LArWave& gCali, const double fstep, const double Tc, const unsigned Ttail) const ;
  LArWave stepResp (const LArWave& gCali, const double fstep, const double Tc) const ;
  LArWave stepCorr(const LArWave& gCali, const double& fstep, const double& Tc) const ;
  LArWave dstepRespDfstep (const LArWave& gCali, const double& fstep, const double& Tc) const ;
  double dstepCorrDfstep (const double t, const double& fstep, const double& Tc ) const ;
  LArWave dstepCorrDfstep(const LArWave& gCali, const double& fstep, const double& Tcal ) const ;
  StatusCode RTM_Omega0(const LArWave& gCali, const HWIdentifier chid, LArWFParams& wfParams, const WaveTiming_t& wt, LArCaliWave* omegaScan=NULL) const;
  StatusCode RTM_Taur(const LArWave& gCali, LArWFParams& wfParams, const WaveTiming_t& wt) const;
  double logChi2CosRespShaper (const double omega, const LArWave& gCali, const LArWFParams& wf, const waveRange_t& range) const;
  double logChi2InjRespRes (const double taur, const LArWave& gCali, const LArWFParams& wf, const waveRange_t& range ) const ;
  LArWave cosRespShaper (const LArWave& gCali, const double& fstep, const double& tcal, const double& w, const double& tsh ) const ;
  LArWave cosResp (const LArWave& gCali, const double& fstep, const double& tcal, const double& omega) const ;
  LArWave injRespRes (const LArWave& w, const double omega0, const double taur) const ;
  LArWave injCorr(const unsigned N, const double dt,const double omega0, const double taur) const ;
  double omega0MinNum (double omegaMin, double omegaMax, const double tol,const LArWave& gCali, const LArWFParams& wf, const waveRange_t&) const;
  LArWave cosCorr(const unsigned N, const double dt, const double fstep, const double tcal, const double omega) const ;
  double taurMinNum (const double taurmin, const double taurmax, const double tol, const LArWave& gCali, const LArWFParams& wf,const waveRange_t&) const;

  double fminbr(double a, double b, const double tol, double (LArWFParamTool::*f)(double, const LArWave&, const LArWFParams&, const waveRange_t&) const, 
		const LArWave& gCali, const LArWFParams& wf, const waveRange_t& range) const; 

  unsigned checkStatus(const LArCaliWave &larCaliWave) const  ;

  omegaScanParams_t OmegaScanHelper(const Identifier id,  const  WaveTiming_t& wt) const;
  
 
};
#endif
