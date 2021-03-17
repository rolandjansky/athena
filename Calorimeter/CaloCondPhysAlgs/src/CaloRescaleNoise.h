/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// CaloRescaleNoise.h
//

#ifndef CALOCONDPHYSALGS_CALORESCALENOISE_H
#define CALOCONDPHYSALGS_CALORESCALENOISE_H

#include <string>

// Gaudi includes

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloInterface/ICaloMBAverageTool.h"
#include "LArElecCalib/ILArHVScaleCorr.h"
#include "StoreGate/ReadCondHandleKey.h"  

#include "LArCabling/LArOnOffIdMapping.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

class CaloNoise;

class CaloRescaleNoise : public AthAlgorithm {

  public:
    //Gaudi style constructor and execution methods
    /** Standard Athena-Algorithm Constructor */
    CaloRescaleNoise(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    virtual ~CaloRescaleNoise();
    
    /** standard Athena-Algorithm method */
    virtual StatusCode          initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          execute() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          finalize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode          stop() override;
    
  private:

  //---------------------------------------------------
  // Member variables
  //---------------------------------------------------
  ITHistSvc* m_thistSvc;

  const CaloCell_ID*       m_calo_id;

  SG::ReadCondHandleKey<CaloNoise> m_totalNoiseKey
    { this, "TotalNoiseKey", "totalNoise", "SG key for total noise" };
  SG::ReadCondHandleKey<CaloNoise> m_elecNoiseKey
    { this, "ElecNoiseKey", "electronicNoise", "SG key for electronic noise" };
  SG::ReadCondHandleKey<CaloNoise> m_pileupNoiseKey
    { this, "PileupNoiseKey", "pileupNoise", "SG key for pileup noise" };

  SG::ReadCondHandleKey<ILArHVScaleCorr> m_scaleCorrKey
  { this, "LArHVScaleCorr", "LArHVScaleCorrRecomputed", "" };
  SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
  SG::ReadCondHandleKey<ILArHVScaleCorr> m_onlineScaleCorrKey
  { this, "OnlineLArHVScaleCorr", "LArHVScaleCorr", "" };

  int m_iCool;
  int m_SubHash;
  int m_Hash;
  int m_OffId;
  float m_eta;
  float m_phi;
  int m_layer;
  int m_Gain;
  float m_noise;
  float m_elecNoise;
  float m_pileupNoise; 
  float m_elecNoiseRescaled;
  TTree* m_tree;
  bool m_absScaling;
};
#endif
