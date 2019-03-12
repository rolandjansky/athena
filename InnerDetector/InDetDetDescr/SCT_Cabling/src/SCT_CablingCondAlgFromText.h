/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_CablingCondAlgFromText_H
#define SCT_CablingCondAlgFromText_H
/**   
 *   @file SCT_CablingCondAlgFromText.h
 *
 *   @brief Fills an SCT cabling object from a plain text source
 *
 *   @author Susumu Oda
 *   @date 20/08/2018
 */

//Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "SCT_Cabling/SCT_CablingData.h"
#include "StoreGate/WriteCondHandleKey.h"

//Gaudi includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ICondSvc.h"

//STL includes
#include <string>

//Forward declarations
class SCT_ID;

/**
 *    @class SCT_CablingCondAlgFromText
 *    @brief Service which fill the SCT Cabling from plain text (a file).
 *
 */

class SCT_CablingCondAlgFromText: public AthReentrantAlgorithm {
 public:

  SCT_CablingCondAlgFromText(const std::string& name, ISvcLocator* svc);
  virtual ~SCT_CablingCondAlgFromText() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  
private:

  bool insert(const IdentifierHash& hash, const SCT_OnlineId& onlineId, const SCT_SerialNumber& sn, SCT_CablingData* data) const;
  StringProperty m_source{this, "DataSource", "SCT_MC_FullCabling_svc.dat", "a plain text file for the SCT Cabing"};
  SG::WriteCondHandleKey<SCT_CablingData> m_writeKey{this, "WriteKey", "SCT_CablingData", "Key of output (derived) conditions folder"};
  ServiceHandle<ICondSvc> m_condSvc;

  const SCT_ID* m_idHelper;
};//end of class

#endif // SCT_CablingCondAlgFromText_H
