/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCONDUTILS_LARFEBTEMPALG_H
#define LARCONDUTILS_LARFEBTEMPALG_H

#include "LArRecConditions/LArFEBTempData.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

class LArFEBTempAlg:public AthReentrantAlgorithm {
 
 public: 
   LArFEBTempAlg(const std::string& name, ISvcLocator* pSvcLocator);
   
   virtual ~LArFEBTempAlg()=default;
   
   virtual StatusCode initialize() override final;
   virtual StatusCode execute(const EventContext& ctx) const override final;
   
 private:
   
   SG::ReadCondHandleKey<CondAttrListCollection> m_foldernameKey{this, "folderName", "/LAR/DCS/FEBTEMP", "DCS folder with temperatures"};
   SG::WriteCondHandleKey<LArFEBTempData> m_tempKey{this, "keyOutput", "LArFEBTemp", "Output key for LAr Feb temperature data"}; 
};

#endif
