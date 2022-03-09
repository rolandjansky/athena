//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef CALOCELLCORRECTION_CALOCELLPEDCORRCONDALG_H
#define CALOCELLCORRECTION_CALOCELLPEDCORRCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CaloCondBlobObjs/ICaloCoolIdTool.h"
#include "CaloConditions/CaloCellPedShift.h"

class CaloCell_ID;

class CaloCellPedCorrCondAlg: public AthReentrantAlgorithm {
 public:
  using AthReentrantAlgorithm::AthReentrantAlgorithm;
  virtual ~CaloCellPedCorrCondAlg() = default;

  StatusCode initialize() override final;
  StatusCode execute(const EventContext& ctx) const override final;
  StatusCode finalize() override final {return StatusCode::SUCCESS;}


 private:
  //SG Keys and other properties:
  SG::ReadCondHandleKey<CondAttrListCollection> m_pedShiftFolder{this,"PedestalShiftFolder","/CALO/Pedestal/CellPedestal","SG Key of Attr list containing pedestal shifts"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_lumiFolderName{this,"LumiFolderName","/TRIGGER/LUMI/LBLESTONL","SG Key of Attr list for Luminosity estimate"};
  SG::WriteCondHandleKey<CaloCellPedShift> m_pedShiftKey{this,"CaloPedShiftKey","CaloPedShift"};

  ToolHandle<ICaloCoolIdTool> m_caloCoolIdTool{this,"CaloCoolIdTool","CaloCoolIdTool"};

  Gaudi::Property<float> m_lumi0{this,"Luminosity",0.0,"Luminosity in 10**33 units"};

  const CaloCell_ID* m_cellId=nullptr;

};


#endif
