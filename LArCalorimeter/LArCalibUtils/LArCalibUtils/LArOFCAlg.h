//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArOFCAlgorithm_H
#define LArOFCAlgorithm_H
 
#include <vector>
#include <string>
 
#include "LArRawConditions/LArWaveCumul.h"

#include "GaudiKernel/ToolHandle.h"
#include "LArElecCalib/ILArAutoCorrDecoderTool.h"

#include "LArElecCalib/ILArPedestal.h"
#include "CaloIdentifier/CaloGain.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "LArRawConditions/LArPhysWaveContainer.h"

#include "StoreGate/DataHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"

#include <Eigen/Dense>


class LArOnlineID; 
class CaloDetDescrManager; 

class LArCablingService  ; 
#include "GaudiKernel/ToolHandle.h"

class LArOFCAlg:public AthAlgorithm {
 
public:
 
  LArOFCAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute() {return StatusCode::SUCCESS;}
  virtual StatusCode stop();
  StatusCode finalize(){return StatusCode::SUCCESS;}
  Eigen::VectorXd  getAmpCoef();
  Eigen::VectorXd  getATauCoef();

private:

  void           perCalc();
  void           Optfilt(std::vector<float> &, std::vector<float> &);

  // LArPhysWaveContainer
  typedef LArPhysWaveContainer::ConstConditionsMapIterator PhysWaveIt;
  StatusCode     initPhysWaveContainer(const std::string& keyPhys,
 				       unsigned&   gain_it, 
				       PhysWaveIt& wave_it) ; 
  const LArWaveCumul* getWave         (unsigned&   gain_it,
                                       PhysWaveIt& wave_it) ;  

  // LArCaliWaveContainer
  typedef LArCaliWaveContainer::ConstConditionsMapIterator    CaliCellIt;
  typedef LArCaliWaveContainer::LArCaliWaves::const_iterator  CaliWaveIt;
  StatusCode     initCaliWaveContainer(const std::string& keyCali, 
				       unsigned&   gain_it,
				       CaliCellIt& cell_it, 
				       CaliWaveIt& wave_it) ;
  const LArWaveCumul* getWave         (unsigned&   gain_it,
                                       CaliCellIt& cell_it,
				       CaliWaveIt& wave_it) ;  
    
  std::string              m_dumpOFCfile ;
  std::vector<std::string> m_keylist;
  bool                     m_verify; 
  bool                     m_normalize;
  bool                     m_timeShift;
  int                      m_timeShiftByIndex ;
 //  CLHEP::HepVector         m_gResp;
//   CLHEP::HepVector         m_gDerivResp;
//  CLHEP::HepMatrix         m_AutoCorrMatrix;
//   CLHEP::HepVector         m_a; // the a coefficients  for A
//   CLHEP::HepVector         m_b; // the b coefficients for A*Tau

  Eigen::VectorXd          m_gResp;
  Eigen::VectorXd          m_gDerivResp;
  Eigen::MatrixXd          m_AutoCorrMatrix;
  Eigen::VectorXd          m_a; // the a coefficients  for A
  Eigen::VectorXd          m_b; // the b coefficients for A*Tau


  int                      m_optNpt;
  unsigned int             m_nSamples;
  unsigned int             m_nPhases;
  unsigned int             m_dPhases; // number of samples between two neighboring phases (OFC sets)
  unsigned int             m_nDelays ;

  float                    m_addOffset;

  ToolHandle<ILArAutoCorrDecoderTool> m_AutoCorrDecoder;

  double m_errAmpl;
  double m_errTime;

  bool                     m_readCaliWave ;
  bool                     m_fillShape ;
  std::string              m_ofcKey; 
  std::string              m_shapeKey; 
  bool                     m_storeMaxPhase;
  std::string              m_ofcBinKey;

  const LArCaliWaveContainer* m_larCaliWaveContainer;
  const LArPhysWaveContainer* m_larPhysWaveContainer;
  
  // Groupinf type
  std::string              m_groupingType;
  std::string              m_larPhysWaveBinKey;

  int                      m_useDelta;
  bool                     m_thisChan_useDelta;
  const LArOnlineID*       m_onlineID; 

  //CLHEP::HepVector         m_delta;
  Eigen::VectorXd          m_delta;
  HWIdentifier             m_id; 
  void getDelta(std::vector<float>& v);
  ToolHandle<LArCablingService> m_cablingService;
  const CaloDetDescrManager* m_calo_dd_man;

};

#endif

