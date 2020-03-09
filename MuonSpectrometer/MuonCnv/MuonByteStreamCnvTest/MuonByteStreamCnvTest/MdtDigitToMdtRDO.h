/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTDIGITTOMDTRDO_H
#define MDTDIGITTOMDTRDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonRDO/MdtCsmContainer.h"

#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"


/////////////////////////////////////////////////////////////////////////////

class MdtDigitToMdtRDO : public AthReentrantAlgorithm {

 public:

  MdtDigitToMdtRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MdtDigitToMdtRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode fill_MDTdata(const EventContext& ctx) const;
  StatusCode fillTagInfo() const;

 protected:

  bool m_BMEpresent{false};
  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  SG::WriteHandleKey<MdtCsmContainer> m_csmContainerKey{this,"OutputObjectName","MDTCSM","WriteHandleKey for Output MdtCsmContainer"};
  SG::ReadHandleKey<MdtDigitContainer> m_digitContainerKey{this,"InputObjectName","MDT_DIGITS","ReadHandleKey for Input MdtDigitContainer"};
  SG::ReadCondHandleKey<MuonMDT_CablingMap> m_readKey{this, "ReadKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};

};

#endif
