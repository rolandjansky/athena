//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOTOOLS_CALRNOISECONDALG_H
#define CALOTOOLS_CALRNOISECONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "CaloConditions/CaloNoise.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "CaloIdentifier/CaloNoiseHashRanges.h"

class CaloCell_ID;

class CaloNoiseCondAlg: public AthAlgorithm {
 public:

  CaloNoiseCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CaloNoiseCondAlg() {};

  StatusCode initialize() override final;
  StatusCode execute() override final;
  StatusCode finalize() override final {return StatusCode::SUCCESS;}


 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_larNoiseKey{this, "LArNoiseFolder","/LAR/NoiseOfl/CellNoise",
      "SG key of CondAttrListCollection holding the LAr noise"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_tileNoiseKey{this, "TileNoiseFolder","/TILE/OFL02/NOISE/CELL",
      "SG key of CondAttrListCollection holding the Tile noise"};  
  //Legacy folder, still needed?
  SG::ReadCondHandleKey<CondAttrListCollection> m_caloNoiseKey{this, "CaloNoiseFolder","/CALO/Ofl/Noise/CellNoise",
      "SG key of CondAttrListCollection holding both LAr and Tile noise (legacy)"};  
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_hvCorrKey{this, "LArHVCorrKey","LArHVScaleCorrRecomputed",
      "SG Key of HV Scale correction CDO"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_lumiFolderKey{this, "LumiFolder","/TRIGGER/LUMI/LBLESTONL", 
      "SG Key of  CondAttrListCollection holding Luminosity information" };

  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};

  SG::WriteCondHandleKey<CaloNoise> m_outputKey{this, "OutputKey", "TotalNoise", "SG Key of resulting noise CDO"};

  Gaudi::Property<bool> m_useHVCorr{this,"useHVCorr",false,"Use HV Corr on/off"};
  Gaudi::Property<float> m_lumi0{this,"Luminosity",-1.0,"Fixed Luminosity. -1 means read lumi from DB"};

  ServiceHandle<ICondSvc> m_condSvc;

  const CaloCell_ID* m_caloCellID;

  std::unique_ptr<CaloNoiseHashRanges> m_hashRange;

  CaloNoise::NOISETYPE m_noiseType=CaloNoise::TOTAL;

};

#endif
