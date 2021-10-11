/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef AFP_DBTOOLS_SIALIGNDBTESTER_H
#define AFP_DBTOOLS_SIALIGNDBTESTER_H

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
 * @class SiAlignDBTester
 * @brief 
 **/
class SiAlignDBTester : public AthAlgorithm {
public:
  SiAlignDBTester(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SiAlignDBTester() override;

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_rch_locshiftX {this, "locshiftXkey", "/FWD/Onl/AFP/Align/Local", "read cond handle local shiftXkey"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_rch_globshiftX {this, "globshiftXkey", "/FWD/Onl/AFP/Align/Global", "read cond handle global shiftXkey"};
  SG::ReadCondHandleKey<AthenaAttributeList> m_readKey { this, "BeamSpotFolder", "/Indet/Beampos", "DB folder from which to read raw beam spot data" };
  SG::ReadCondHandleKey<CaloLocalHadCoeff> m_key;

};

#ifndef __CINT__
  CLASS_DEF( SiAlignDBTester , 8327923 , 1 )
#endif

#endif // AFP_DBTOOLS_SIALIGNDBTESTER_H
