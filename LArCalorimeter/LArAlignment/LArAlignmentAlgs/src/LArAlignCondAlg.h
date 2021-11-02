/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARALIGNMENTALGS_LARALIGNCONDALG_H
#define LARALIGNMENTALGS_LARALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "DetDescrConditions/DetCondKeyTrans.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "GeoModelUtilities/StoredAlignX.h"

#include "LArGeoCode/LArAlignHelper.h"

/**
 * @class LArAlignCondAlg
 *
 * @brief Condition Algorithm for applying LAr Alignments
 *
 **/

class LArAlignCondAlg final : public AthAlgorithm
{
 public:
  using AthAlgorithm::AthAlgorithm;
  virtual ~LArAlignCondAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override {return StatusCode::SUCCESS;};

 private:
  SG::ReadCondHandleKey<DetCondKeyTrans> m_readLArAlignKey {this
      , "LArAlignFolder"
      , "/LAR/Align"
      , "SG key of DetCondKeyTrans object with LAr alignments" };

  SG::WriteCondHandleKey<GeoAlignmentStore>  m_writeGeoAlignKey {this
      , "LArAlignmentStore"
      , "LArAlignmentStore"
      , "SG key of the resulting GeoAlignmentStore for LAr" };

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  LArAlignHelper m_alignHelper;
};

#endif
