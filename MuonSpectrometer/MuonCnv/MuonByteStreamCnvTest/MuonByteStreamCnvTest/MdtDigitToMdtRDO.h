/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTDIGITTOMDTRDO_H
#define MDTDIGITTOMDTRDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonCablingData/MuonMDT_CablingMap.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCondData/MdtCondDbData.h"
/////////////////////////////////////////////////////////////////////////////

class MdtDigitToMdtRDO : public AthReentrantAlgorithm {

 public:

  MdtDigitToMdtRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MdtDigitToMdtRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode fill_MDTdata(const EventContext& ctx) const;

  // NOTE: although this function has no clients in release 22, currently the Run2 trigger simulation is still run in
  //       release 21 on RDOs produced in release 22. Since release 21 accesses the TagInfo, it needs to be written to the
  //       RDOs produced in release 22. The fillTagInfo() function thus needs to stay in release 22 until the workflow changes
  StatusCode fillTagInfo() const;

 protected:
      bool m_BMEpresent{false};
      int  m_BME_station_name{-1};
      ///
      bool m_BMGpresent{false};
      int m_BMG_station_name{-1};
      int m_BIS_station_name{-1};
      
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      SG::WriteHandleKey<MdtCsmContainer> m_csmContainerKey{this,"OutputObjectName","MDTCSM","WriteHandleKey for Output MdtCsmContainer"};
      SG::ReadHandleKey<MdtDigitContainer> m_digitContainerKey{this,"InputObjectName","MDT_DIGITS","ReadHandleKey for Input MdtDigitContainer"};
      SG::ReadCondHandleKey<MuonMDT_CablingMap> m_cablingKey{this, "CablingKey", "MuonMDT_CablingMap", "Key of MuonMDT_CablingMap"};
      SG::ReadCondHandleKey<MdtCondDbData> m_condKey{this, "ConditionsKey",  "MdtCondDbData", "Key of MDT condition data"};

     /// Create from the module_id a MdtCsmContainer
     /// -- cabling_ptr: Pointer to the cached cabling map from the conditions
     /// -- Identifier:  Identifier of the current chamber
     /// -- moduleHash: 
     /// -- need_second: Create the second CSM needed for BME and BIS78 (Run-III)
     std::unique_ptr<MdtCsm> make_csm(const MuonMDT_CablingMap* cabling_ptr, const Identifier module_id, IdentifierHash module_hash, bool need_second) const;
     
};

#endif
