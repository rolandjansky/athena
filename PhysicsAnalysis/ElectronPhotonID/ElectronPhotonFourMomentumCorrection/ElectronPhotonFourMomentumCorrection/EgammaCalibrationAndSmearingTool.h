// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMA_CALIB_TOOL_H_
#define EGAMMA_CALIB_TOOL_H_

#include "AsgTools/AsgTool.h"
#include "ElectronPhotonFourMomentumCorrection/IEgammaCalibrationAndSmearingTool.h"
#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/SystematicSet.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEventInfo/EventInfo.h"

#include "ElectronPhotonFourMomentumCorrection/egammaEnergyCorrectionTool.h"

namespace CP {

class EgammaCalibrationAndSmearingTool : virtual public IEgammaCalibrationAndSmearingTool, 
			virtual public CP::ISystematicsTool, 
			public asg::AsgTool {
  // Create a proper constructor for Athena
  ASG_TOOL_CLASS2( EgammaCalibrationAndSmearingTool, IEgammaCalibrationAndSmearingTool, CP::ISystematicsTool)

public:
  
  EgammaCalibrationAndSmearingTool( const std::string& name );
  
  ~EgammaCalibrationAndSmearingTool() {};
  
  StatusCode initialize();
  
  StatusCode finalize();

  //Apply the correction on a modifyable egamma object 
  virtual CP::CorrectionCode applyCorrection(xAOD::Egamma&, const xAOD::EventInfo*);
  
  //Create a corrected copy from a constant egamma object
  //WARNING: this method compiles but crashes when running in ROOT => under investigation, do not use for now
  virtual CP::CorrectionCode correctedCopy( const xAOD::Egamma&, xAOD::Egamma*&, const xAOD::EventInfo*);
  
  //systematics 
  //Which systematics have an effect on the tool's behaviour?
  virtual CP::SystematicSet affectingSystematics() const;
  //Is the tool affected by a specific systematic? 
  virtual bool isAffectedBySystematic( const CP::SystematicVariation& systematic ) const; 
  //Systematics to be used for physics analysis
  virtual CP::SystematicSet recommendedSystematics() const;
  //Use specific systematic
  virtual CP::SystematicCode applySystematicVariation ( const CP::SystematicSet& systConfig );
  //set default configuration 
  virtual void setDefaultConfiguration(const xAOD::EventInfo*);
  virtual void forceSmearing( bool force); 
  virtual void forceScaleCorrection( bool force);
  virtual void setRandomSeed( unsigned seed=0 );
  
private:
  std::string m_ESModel;             
  egEnergyCorr::ESModel m_TESModel;
  unsigned m_seed;
  bool m_debug;
  bool m_doScaleCorrection;
  bool m_doSmearing;
  double m_varSF;
  std::string m_ResolutionType;
  egEnergyCorr::Resolution::resolutionType m_TResolutionType;
  //flags duplicated from the underlying ROOT tool
  bool m_useLayerCorrection; 
  bool m_usePSCorrection; 
  bool m_useS12Correction; 
  bool m_useLayer2Recalibration; 
  bool m_useIntermoduleCorrection; 
  bool m_usePhiUniformCorrection; 
  bool m_useGainCorrection;
  //corresponding flags to force these corrections
  bool m_forceLayerCorrection;
  bool m_forcePSCorrection;
  bool m_forceS12Correction;
  bool m_forceLayer2Recalibration;
  bool m_forceIntermoduleCorrection;
  bool m_forcePhiUniformCorrection;
  bool m_forceGainCorrection;
  
public:
  virtual double getEnergy(const xAOD::Egamma*, const xAOD::EventInfo*); 
  virtual double getElectronEnergy(const xAOD::Electron*, const xAOD::EventInfo*); 
  virtual double getPhotonEnergy(const xAOD::Photon*, const xAOD::EventInfo*);  
  virtual double getElectronMomentum(const xAOD::Electron*, const xAOD::EventInfo*);
  
private:
  double m_CalibratedEnergy;
  double m_CorrectedMomentum;
  
  // A pointer to the underlying ROOT tool 
  AtlasRoot::egammaEnergyCorrectionTool* m_rootTool;

  //Systematics maps
  mutable std::map<egEnergyCorr::Resolution::Variation , CP::SystematicVariation> m_resSystMap; 
  mutable std::map<egEnergyCorr::Scale::Variation , CP::SystematicVariation> m_scaleSystMap; 
  
  //These are modified by the ISystematicsTool methods 
  egEnergyCorr::Scale::Variation m_currentScaleVariation; 
  egEnergyCorr::Resolution::Variation m_currentResolutionVariation;
  
};

}
#endif
