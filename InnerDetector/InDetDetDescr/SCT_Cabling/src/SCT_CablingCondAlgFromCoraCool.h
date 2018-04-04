/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CablingCondAlgFromCoraCool_H
#define SCT_CablingCondAlgFromCoraCool_H
/**   
 *   @file SCT_CablingCondAlgFromCoraCool.h
 *
 *   @brief Fills an SCT cabling object from the database
 *
 *   @author Susumu Oda
 *   @date 08/03/2018
 */

//STL includes
#include <string>

//Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"

//Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "SCT_Cabling/SCT_CablingData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

/**
 *    @class SCT_CablingCondAlgFromCoraCool
 *    @brief Service which fill the SCT Cabling from the database, using CoraCool.
 *
 */

class SCT_CablingCondAlgFromCoraCool: public AthAlgorithm {
 public:

  SCT_CablingCondAlgFromCoraCool(const std::string& name, ISvcLocator* svc);
  virtual ~SCT_CablingCondAlgFromCoraCool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;
  
private:

  bool insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn, SCT_CablingData* data);

  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyRod;
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyRodMur;
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyMur;
  SG::ReadCondHandleKey<CondAttrListVec> m_readKeyGeo;
  SG::WriteCondHandleKey<SCT_CablingData> m_writeKey;
  ServiceHandle<ICondSvc> m_condSvc;

};//end of class

#endif // SCT_CablingCondAlgFromCoraCool_H
