/*
 Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECUTILS_LARXTALKWEIGHTCONDALG_H
#define LARRECUTILS_LARXTALKWEIGHTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "StoreGate/CondHandleKeyArray.h"
#include "GaudiKernel/ICondSvc.h"
#include "LArRecConditions/LArXTalkWeight.h"

class LArXTalkWeightCondAlg: public AthReentrantAlgorithm {
  public:
    LArXTalkWeightCondAlg(const std::string& name, ISvcLocator* pSvcLocator); 
    virtual ~LArXTalkWeightCondAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;
  protected:
    // Conditions keys write:
    SG::WriteCondHandleKey<LArXTalkWeight> m_xtalkKey{this, "OutputKey", "XTalk_Value", "SG key for output"};
    std::vector<float> m_xtalk_to_inject;
    std::vector<int> m_endcap_case_table;
    float    m_xtalk_factor;
};

class LArXTalkWeightCondAlg_strip: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_strip(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_strip() = default;
};

class LArXTalkWeightCondAlg_strip_ec: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_strip_ec(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_strip_ec() = default;
};

class LArXTalkWeightCondAlg_middleback: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middleback(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middleback() = default;
};

class LArXTalkWeightCondAlg_middleback_ecow: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middleback_ecow(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middleback_ecow() = default;
};

class LArXTalkWeightCondAlg_middleback_eciw: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middleback_eciw(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middleback_eciw() = default;
};

class LArXTalkWeightCondAlg_stripmiddle: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_stripmiddle(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_stripmiddle() = default;
};

class LArXTalkWeightCondAlg_stripmiddle_ec: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_stripmiddle_ec(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_stripmiddle_ec() = default;
};

class LArXTalkWeightCondAlg_2strip: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_2strip(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_2strip() = default;
};

class LArXTalkWeightCondAlg_2strip_ec: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_2strip_ec(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_2strip_ec() = default;
};

class LArXTalkWeightCondAlg_middle1: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middle1(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middle1() = default;
};

class LArXTalkWeightCondAlg_middle2: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middle2(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middle2() = default;
};

class LArXTalkWeightCondAlg_middle1_ec: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middle1_ec(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middle1_ec() = default;
};

class LArXTalkWeightCondAlg_middle2_ec: public LArXTalkWeightCondAlg{
   public:
     LArXTalkWeightCondAlg_middle2_ec(const std::string& name, ISvcLocator* pSvcLocator);
     virtual ~LArXTalkWeightCondAlg_middle2_ec() = default;
};

#endif

