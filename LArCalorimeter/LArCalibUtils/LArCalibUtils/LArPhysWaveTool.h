//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARPHYSWAVETOOL_H
#define LARPHYSWAVETOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArWaveHelper.h"

static const InterfaceID IID_LArPhysWaveTool("LArPhysWaveTool", 1 , 0); 

class LArPhysWaveTool : public AthAlgTool
{
 public: 

  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArPhysWaveTool; }

  LArPhysWaveTool(const std::string& type, const std::string& name,
	 	  const IInterface* parent ) ;

  virtual ~LArPhysWaveTool(); 

  virtual StatusCode initialize(){return StatusCode::SUCCESS;}
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}

  //LArPhysWave  makeLArPhysWave(const LArWFParams &, const LArCaliWave &,
  //			       int region=-1, int layer=-1);

  StatusCode makeLArPhysWave(const LArWFParams &, const LArCaliWave &,
  			     int region, int layer,
			     LArPhysWave & predLArPhysWave,
			     float & MphysMcali);

  //double MphysMcali() const { return m_MphysMcali ; }

 private:
  static const int DEFAULT ;
  LArCaliWave m_gCali;
  LArPhysWave m_gPhys;

  //  bool m_verb;
  int  m_method = 0;
  int m_region = 0, m_layer = 0 ;
  bool m_injPointCorr , m_normalizeCali ,
       m_timeOriginShift , m_subtractBaseline;
  std::vector<bool> m_injPointCorrLayer, m_injPointUseTauR ;
  
  int    m_N_FFT = 0 ;
  double m_Tdrift = 0.0, m_Fstep = 0.0, m_Tcal = 0.0, m_Omega0 = 0.0, m_Taur = 0.0, m_MphysMcali = 0.0 ;
  unsigned m_Tstart = 0U, m_Tpeak = 0U, m_Tcross = 0U, m_Tmin = 0U, m_Ttail = 0U ;
  void predict_phys();
  LArWave exp2Tri(const LArWave &) const ;
  LArWave physPred(LArCaliWave &);
  double caliPhysCorr ( double t ) const;
  LArWave caliPhysCorr() const;
  //  LArPhysWave physPred ( int mode , LArPhysWave W ) const;
  //  LArPhysWave physPred ( LArPhysWave W ) const;
  LArWave injResp (const LArWave& w) const;
  LArWave stepResp () const;
  LArWave step2Tri (const LArWave& w) const ;
  double stepPhysCorr ( double t ) const;
  LArWave stepPhysCorr() const;
  LArWave stepCorr() const;
  double stepCorr ( double t ) const;
  LArWave injCorr() const;
  double injCorr ( double t ) const;
  

};

#endif
