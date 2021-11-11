/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOALIGNMENTALGS_CALOALIGNCONDALG_H
#define CALOALIGNMENTALGS_CALOALIGNCONDALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ICondSvc.h"

#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "CaloConditions/CaloCellPositionShift.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

/**
 * @class CaloAlignCondAlg
 *
 * @brief Condition Algorithm for making CaloDetDescrManager condition object
 *
 **/

class CaloAlignCondAlg final : public AthAlgorithm
{
 public:
  using AthAlgorithm::AthAlgorithm;
  virtual ~CaloAlignCondAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:
  SG::ReadCondHandleKey<GeoAlignmentStore>  m_readKeyGeoAlign {this
      , "LArAlignmentStore"
      , "LArAlignmentStore"
      , "SG key of the GeoAlignmentStore for LAr" };

  SG::ReadCondHandleKey<CaloRec::CaloCellPositionShift> m_readKeyCellPosShift {this
      , "CaloCellPositionShiftFolder"
      , "LArCellPositionShift"
      , "SG key of the CaloCellPositionShift" };

  SG::WriteCondHandleKey<CaloDetDescrManager>  m_writeCaloMgrKey {this
      , "CaloDetDescrManager"
      , "CaloDetDescrManager"
      , "SG key of the resulting CaloDetDescrManager" };

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

};

#endif
