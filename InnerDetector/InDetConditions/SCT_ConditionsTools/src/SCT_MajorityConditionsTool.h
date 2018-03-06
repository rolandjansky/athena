/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_MajorityConditionsTool.h
 * header file for tool
 * @author gwilliam@mail.cern.ch
 **/

#ifndef SCT_MajorityConditionsTool_h
#define SCT_MajorityConditionsTool_h

// STL includes
#include <string>
#include <list>
#include <mutex>

// Gaudi includes
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_ConditionsTools/ISCT_DetectorLevelConditionsTool.h"
#include "SCT_ConditionsData/SCT_MajorityCondData.h"

// Read Handle Key
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"

/**
 * @class SCT_MajorityConditionsTool
 * Tool which reports on whether the majority of the SCT (or component) is in LV/HV on state with 
 * at least a fraction (default 0.9) of the HV in that state
 * 
 **/

class SCT_MajorityConditionsTool: public extends<AthAlgTool, ISCT_DetectorLevelConditionsTool> {

 public:
  //@name Tool methods
  //@{
  SCT_MajorityConditionsTool(const std::string &type, const std::string &name, const IInterface *parent);
  virtual ~SCT_MajorityConditionsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  //@}
  
  /**Is the detector good?*/
  virtual bool                             isGood();

  /**Is a barrel/endcap good?*/
  virtual bool                             isGood(int bec);

 private:
  bool                                     m_overall;                       //!< Use overall vvalue or ECA/B/ECC
  float                                    m_majorityFraction;              //!< Required fraction in majority state

  // Mutex to protect the contents.
  mutable std::mutex m_mutex;
  // Cache to store events for slots
  mutable std::vector<EventContext::ContextEvt_t> m_cache;
  // Pointer of SCT_MajorityCondData
  mutable Gaudi::Hive::ContextSpecificPtr<const SCT_MajorityCondData> m_condData;

  SG::ReadCondHandleKey<SCT_MajorityCondData> m_condKey;
  const SCT_MajorityCondData* getCondData(const EventContext& ctx) const;
};

#endif // SCT_MajorityConditionsTool_h
