/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileMuId2DBAlg.h
// Author   : Aranzazu Ruiz
// Created  : May 2009
//
// DESCRIPTION
//    Create ASCII file with TileMuId thresholds to be stored in COOL DB.
//
// BUGS:
//  
// History:
//  
//****************************************************************************

#ifndef TileCalibAlgs_TileMuId2DBAlg_h
#define TileCalibAlgs_TileMuId2DBAlg_h

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloConditions/CaloNoise.h"
#include "StoreGate/ReadCondHandleKey.h"

class TileMuId2DBAlg: public AthAlgorithm {

 public:

  TileMuId2DBAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~TileMuId2DBAlg();

  virtual StatusCode initialize() override;  
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:

  const CaloCell_ID* m_calo_id;

  SG::ReadCondHandleKey<CaloNoise> m_totalNoiseKey
    { this, "TotalNoiseKey", "totalNoise", "SG key for total noise" };
};

#endif // TileCalibAlgs_TileMuId2DBAlg_h
