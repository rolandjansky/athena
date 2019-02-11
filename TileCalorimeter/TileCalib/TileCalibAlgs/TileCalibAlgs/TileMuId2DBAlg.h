/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "CaloInterface/ICaloNoiseTool.h"
#include "TileConditions/TileCellNoiseTool.h"

class TileMuId2DBAlg: public AthAlgorithm {

 public:

  TileMuId2DBAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~TileMuId2DBAlg();

  StatusCode initialize();  
  StatusCode execute();
  StatusCode finalize();

 private:

  const CaloCell_ID* m_calo_id;

  const DataHandle<CaloIdManager> m_caloIdMgr;
  const DataHandle<CaloDetDescrManager> m_calodetdescrmgr;

  ToolHandle<ICaloNoiseTool> m_noiseTool;

  //float m_eta;
  //float m_phi;
  float m_noise;
  int m_module;
};

#endif // TileCalibAlgs_TileMuId2DBAlg_h
