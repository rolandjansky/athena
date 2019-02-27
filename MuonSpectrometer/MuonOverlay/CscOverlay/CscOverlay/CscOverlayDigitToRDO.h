/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCOVERLAY_CSCOVERLAYDIGITTORDO_H
#define CSCOVERLAY_CSCOVERLAYDIGITTORDO_H

#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "MuonRDO/CscRawDataContainer.h"

class CscOverlayDigitToRDO : public AthAlgorithm  {
public:
  CscOverlayDigitToRDO(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~CscOverlayDigitToRDO() = default;
  /** Framework implemenrtation for the event loop */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:
  SG::ReadHandleKey<CscRawDataContainer> m_inputBkgRDOKey{this,"InputBkgRDOKey","OriginalEvent_SG+CSCRDO",""};
  ToolHandle<IMuonDigitizationTool> m_rdoTool2{this, "MakeRDOTool2", "CscDigitToCscRDOTool2", ""};
  ToolHandle<IMuonDigitizationTool> m_rdoTool4{this, "MakeRDOTool4", "CscDigitToCscRDOTool4", ""};
};

#endif/*CSCOVERLAY_CSCOVERLAYDIGITTORDO_H*/
