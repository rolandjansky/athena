//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARDELTARESPTOOL_H
#define LARDELTARESPTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArWaveHelper.h"

static const InterfaceID IID_LArDeltaRespTool("LArDeltaRespTool", 1 , 0); 

class LArDeltaRespTool : public AthAlgTool
{
 public: 

  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArDeltaRespTool; }

  // constructor
   LArDeltaRespTool(const std::string& type, const std::string& name, const IInterface* parent ) ;

  // destructor 
  virtual ~LArDeltaRespTool(); 

  virtual StatusCode initialize(){return StatusCode::SUCCESS;}
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}

  LArCaliWave makeLArDeltaResp(const LArWFParams &, const LArCaliWave &);

 private:
  
  static const int DEFAULT;
  
  LArCaliWave m_gCali, m_gDelta;
  
  bool m_injPointCorr, m_normalizeCali, m_timeOriginShift, m_subtractBaseline;
  
  double   m_Tdrift, m_Fstep, m_Tcal, m_Omega0, m_Taur;
  unsigned m_Tstart;

  void compute_deltaresp();

  LArWave deltaResp( const LArWave & ) const ;  
  LArWave deltaCorr() const;
  double  deltaCorr( double t ) const;

  LArWave injResp (const LArWave& w) const;
  LArWave injCorr() const;
  double injCorr ( double t ) const;

};

#endif
