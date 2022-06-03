/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AFP_DBTOOLS_AFPDBTESTER_H
#define AFP_DBTOOLS_AFPDBTESTER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadCondHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/CondCont.h"
#include <string>

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CaloConditions/CaloLocalHadCoeff.h"
#include "GaudiKernel/EventContext.h"

#include "nlohmann/json.hpp"

/**
 * @class AFPDBTester
 * @brief 
 **/
class AFPDBTester : public AthAlgorithm {
public:
  AFPDBTester(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~AFPDBTester() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_rch_locshiftX {this, "locshiftXkey", "/FWD/Onl/AFP/Align/Local", "read cond handle local shiftXkey"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_rch_globshiftX {this, "globshiftXkey", "/FWD/Onl/AFP/Align/Global", "read cond handle global shiftXkey"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_rch_loctof {this, "locToFkey", "/FWD/Onl/AFP/ToFParameters/Local", "read cond handle local ToF"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_rch_vtxtof {this, "vtxToFkey", "/FWD/Onl/AFP/ToFParameters/Vertex", "read cond handle vertex ToF"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey { this, "BeamSpotFolder", "/Indet/Beampos", "DB folder from which to read raw beam spot data" };
  SG::ReadCondHandleKey<CaloLocalHadCoeff> m_key;

};

#ifndef __CINT__
  CLASS_DEF( AFPDBTester , 205749847 , 1 )
#endif

#endif // AFP_DBTOOLS_AFPDBTESTER_H
