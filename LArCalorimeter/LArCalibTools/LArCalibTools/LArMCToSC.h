//-*- C++ -*-
/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   
 */

#ifndef LARCALIBTOOLS_LARMCTOSC_H
#define LARCALIBTOOLS_LARMCTOSC_H 1

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "LArRawConditions/LArConditionsContainer.h"
#include "LArRawConditions/LArSingleFloatP.h"
#include "LArCabling/LArCablingService.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/DataHandle.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "LArCabling/LArSuperCellCablingTool.h"

//class LArOnlineID;
//class CaloSuperCellDetDescrManager;
class CondAttrListCollection;
class ICaloSuperCellIDTool;
class ILArADC2MeVTool;
// class ILArOFC;
class ILArRamp;
// class LArShapeMC;
class ILArDAC2uA;
class ILAruA2MeV;
class ILArfSampl;
class ILArAutoCorr;
class ILArNoise;
class ILArPedestal;
class ILArMinBias;
class ILArMinBiasAverage;

class LArMCToSC: public AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
public: 
  /// Constructor with parameters: 
  LArMCToSC( const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  virtual ~LArMCToSC(); 

  // Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute() {return StatusCode::SUCCESS;}
  StatusCode  finalize() {return StatusCode::SUCCESS;}
  StatusCode  stop();

private: 
  /// Default constructor: 
  LArMCToSC();

  CondAttrListCollection* uA2MeVSC(const ILAruA2MeV* input, const std::string& outputName);
  CondAttrListCollection* rampSC(const ILArRamp* input, const std::string& outputName);
  CondAttrListCollection* DAC2uASC(const ILArDAC2uA* input, const std::string& outputName);
  CondAttrListCollection* fSamplSC(const ILArfSampl* input, const std::string& outputName);
  CondAttrListCollection* larMinBiasAverageSC(const ILArMinBiasAverage* input, const std::string& outputName);
  CondAttrListCollection* autoCorrSC(const ILArAutoCorr* input, const std::string& outputName);
  CondAttrListCollection* noiseSC(const ILArNoise* input, const std::string& outputName);
  CondAttrListCollection* pedestalSC(const std::string& outputName);
  CondAttrListCollection* minBiasSC(const std::string& outputName);
  void minBiasCheck(const ILArMinBias* input);
  void writeCellInfo();
  void writeSCellInfo();

  void FillNoise();
  void FillMinBias();
  void FillLArMinBias();

  unsigned m_hashMax;
  
  const LArOnline_SuperCellID* m_onlSCID;
  const CaloCell_SuperCell_ID* m_scid;
  const CaloCell_ID* m_cid;

  const DataHandle<CaloSuperCellDetDescrManager> m_sem_mgr;
  const DataHandle<CaloDetDescrManager> m_em_mgr;

  /// Property: Offline / supercell mapping tool.
  ToolHandle<ICaloSuperCellIDTool>     m_scidtool;
  ToolHandle<LArSuperCellCablingTool> m_scCablingTool;
  ToolHandle<ILArADC2MeVTool>     m_adc2mevtool;

  //ToolHandle<LArCablingService> m_cablingSvc;

  /// Geometry manager.


  bool m_fixedLSB;
  bool m_quantEt;
  double m_mQuant;
  double m_emQuant;
  double m_forwardQuant;

  ///InputSGKeys
  std::string m_uA2MeVInput;
  std::string m_DAC2uAInput;
  std::string m_fSamplInput;
  std::string m_RampInput;
  std::string m_autoCorrInput;
  std::string m_noiseInput;
  std::string m_pedestalInput;
  std::string m_MinBiasInput;
  std::string m_MinBiasAverageInput;

  // Input from noise xls table
  std::vector <int> m_xlsIsFCAL;  
  std::vector <int> m_xlsIsHEC; 
  std::vector <int> m_xlsIsEM; 
  std::vector <int> m_xlsSampl;
  std::vector <float> m_xlsEta;
  std::vector <float> m_xlsLSB; 
  std::vector <float> m_xlsNoise;

  std::vector <int> m_fcalSampl;
  std::vector <int> m_fcalEtaI; 
  std::vector < std::vector <float> > m_fcalNoise;
  std::vector < std::vector <float> > m_fcalLSB;

  std::vector <float> m_scADC2MeV;

  std::vector <float> m_minBiasInput;
  std::vector <uint> m_offlIDMinBias;

  std::vector <float> m_minBiasLArInput;
  std::vector <uint> m_offlIDLArMinBias;

  
}; 

#endif //> !LARCALIBTOOLS_LARMCTOSC_H
