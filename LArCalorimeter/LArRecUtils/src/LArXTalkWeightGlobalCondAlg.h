/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECUTILS_LARXTALKWEIGHTGLOBALCONDALG_H
#define LARRECUTILS_LARXTALKWEIGHTGLOBALCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "GaudiKernel/ICondSvc.h"
#include "LArRecConditions/LArXTalkWeightGlobal.h"
#include "LArRecConditions/LArXTalkWeight.h"

class LArXTalkWeightGlobalCondAlg: public AthReentrantAlgorithm {
  public:
    LArXTalkWeightGlobalCondAlg(const std::string& name, ISvcLocator* pSvcLocator); 
    virtual ~LArXTalkWeightGlobalCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
  protected:
    // Conditions keys write:
    SG::WriteCondHandleKey<LArXTalkWeightGlobal> m_xtalkKey{this, "OutputKey", "LArXTalkWeightGlobal", "SG key for output"};
    // Conditions key read in :
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtstripKey{this,"StripKey","LArXTalkWeight_strip","SG Key of XTalk strip object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtstrip_ecKey{this,"Strip_ecKey","LArXTalkWeight_strip_ec","SG Key of XTalk strip_ec object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddlebackKey{this,"MiddleBackKey","LArXTalkWeight_middleback","SG Key of XTalk MiddleBack object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddleback_ecowKey{this,"MiddleBack_ecowKey","LArXTalkWeight_middleback_ecow","SG Key of XTalk MiddleBack ecow object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddleback_eciwKey{this,"MiddleBack_eciwKey","LArXTalkWeight_middleback_eciw","SG Key of XTalk MiddleBack eciw object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtstripmiddleKey{this,"StripMiddleKey","LArXTalkWeight_stripmiddle","SG Key of XTalk StripMiddle object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtstripmiddle_ecKey{this,"StripMiddle_ecKey","LArXTalkWeight_stripmiddle_ec","SG Key of XTalk StripMiddle ec object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xt2stripKey{this,"TwoStripKey","LArXTalkWeight_2strip","SG Key of XTalk 2strip object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xt2strip_ecKey{this,"Two2Strip_ecKey","LArXTalkWeight_2strip_ec","SG Key of XTalk 2strip_ec object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddle1Key{this,"Middle1Key","LArXTalkWeight_middle1","SG Key of XTalk Middle1 object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddle2Key{this,"Middle2Key","LArXTalkWeight_middle2","SG Key of XTalk Middle2 object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddle1_ecKey{this,"Middle1_ecKey","LArXTalkWeight_middle1_ec","SG Key of XTalk Middle1 ec object"};
    SG::ReadCondHandleKey<LArXTalkWeight>    m_xtmiddle2_ecKey{this,"Middle2_ecKey","LArXTalkWeight_middle2_ec","SG Key of XTalk Middle2 ec object"};

    const LArXTalkWeight* pointerFromKey(const EventContext& context, const SG::ReadCondHandleKey<LArXTalkWeight>& key) const;
};

const LArXTalkWeight* LArXTalkWeightGlobalCondAlg::pointerFromKey(const EventContext& context, const SG::ReadCondHandleKey<LArXTalkWeight>& key) const {
  SG::ReadCondHandle<LArXTalkWeight> aHandle(key, context);
  return *aHandle;
};

#endif

