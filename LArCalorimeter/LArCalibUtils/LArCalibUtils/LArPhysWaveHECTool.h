//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef LARPHYSWAVEHECTOOL_H
#define LARPHYSWAVEHECTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "LArRawConditions/LArWFParams.h"
#include "LArRawConditions/LArCaliWave.h"
#include "LArRawConditions/LArPhysWave.h"
#include "LArRawConditions/LArWaveHelper.h"

// includes from calib2physHEC.h:
#include <stdio.h>
#include <math.h>
#include <Rtypes.h>
#include <TSpline.h>
#include <TF1.h>
#include <TFile.h>
#include <TMinuit.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TProfile.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <iostream>
#include <vector>

static const InterfaceID IID_LArPhysWaveHECTool("LArPhysWaveHECTool", 1 , 0); 

class LArPhysWaveHECTool : public AthAlgTool
{
 public: 

  // Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_LArPhysWaveHECTool; }

  LArPhysWaveHECTool(const std::string& type, const std::string& name,
		 const IInterface* parent ) ;

  virtual ~LArPhysWaveHECTool(); 

  virtual StatusCode initialize();
  virtual StatusCode finalize(){return StatusCode::SUCCESS;}

 
  StatusCode makeLArPhysWaveHEC(LArWFParams& wfParams, LArCaliWave &caliWave,
			       LArPhysWave & predLArPhysWave, const LArPhysWave & LArIdealPhysWave,
			       float & MphysMcali, const HWIdentifier& chid, const int gain, 
			       int & LArPhysWaveFlag);

//   StatusCode makeLArPhysWaveHEC(const LArWFParams &, const LArCaliWave &,
//   			     int region, int layer,
// 			     LArPhysWave & predLArPhysWave,
// 			     float & MphysMcali, const HWIdentifier& chid, 
// 				unsigned gain, int & LArPhysWaveFlag);

  //double MphysMcali() const { return m_MphysMcali ; }
  
 private:
  static const int DEFAULT ;
  //LArCaliWave m_gCali;
  //LArPhysWave m_gPhys;
  const LArPhysWave* m_gIdealPhys = nullptr;	
  
  const LArOnlineID* m_onlineHelper = nullptr;
  //  bool m_verb;
  //int m_region, m_layer ;
  bool m_normalizeCali , m_timeOriginShift , m_subtractBaseline;

  double   m_TcalMin, m_TcalMax, m_TcalAverage, 
           m_FstepMin, m_FstepMax, m_FstepAverage;

  //  std::vector<bool> m_injPointCorrLayer, m_injPointUseTauR ;
  
  double m_Omega0 = 0.0, m_Taur = 0.0;
  unsigned m_Tstart = 0U;
  double m_MinAmp;
  //void predict_phys_HEC(const HWIdentifier& chid, unsigned gain);
  void predict_phys_HEC(LArWFParams& wfParams, LArCaliWave &caliWave,
			LArPhysWave & predLArPhysWave,
			float & MphysMcali, const HWIdentifier& chid, const int gain);

  TF1* CaliWave2PhysWaveHEC(TProfile *pcal, Double_t *par, double *parCL, TF1 *& deriv,Bool_t uset0, Bool_t norm, int adc,  Double_t *xmax ,bool gsl_flag);
  
  //Double_t DTp(Double_t *tt, Double_t *par);
  //Double_t Tp(Double_t *tt, Double_t *par);
  //Double_t Tp_gsl(Double_t *tt, Double_t *par);
  //Double_t Tp4_gsl(Double_t t);
  //Double_t Tp5_gsl(Double_t t);
};

#endif
