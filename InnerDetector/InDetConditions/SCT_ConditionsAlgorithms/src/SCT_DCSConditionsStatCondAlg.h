// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 

#ifndef SCT_DCSCONDITIONSSTATCONDALG
#define SCT_DCSCONDITIONSSTATCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "SCT_ConditionsData/SCT_DCSStatCondData.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class SCT_DCSConditionsStatCondAlg : public AthReentrantAlgorithm 
{  
 public:
  SCT_DCSConditionsStatCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SCT_DCSConditionsStatCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; }

 private:
  // Meaning of state word is found at
  // https://twiki.cern.ch/twiki/bin/view/Atlas/SctDCSSoftware#Decoding_Status_words
  enum StateWord {OFF=0x0,
                  ON=0x1,
                  STANDBY=0x2,
                  MANUAL=0x3,
                  MASK_OFF=0x4,
                  MASK_ON=0x5,
                  HARD_RESET=0x6,
                  DISABLED=0x7,
                  RAMPING=0x8,
                  INTERLOCKED=0x9,
                  TRIP_HW=0xA,
                  TRIP_SW=0xB,
                  LVCARD_LATCH=0xC,
                  NO_MATCH=0xD,
                  UNKNOWN=0xE,
                  ANY=0xF};

  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyHV{this, "ReadKeyHV", "/SCT/DCS/HV", "Key of input (raw) HV conditions folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState{this, "ReadKeyState", "/SCT/DCS/CHANSTAT", "Key of input (raw) State conditions folder"};
  SG::WriteCondHandleKey<SCT_DCSStatCondData> m_writeKeyState{this, "WriteKeyState", "SCT_DCSStatCondData", "Key of output (derived) State conditions data"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  bool m_doState{true};
  BooleanProperty m_readAllDBFolders{this, "ReadAllDBFolders", true};
  BooleanProperty m_returnHVTemp{this, "ReturnHVTemp", true};
  StringProperty m_chanstatCut{this, "StateCut", "NORM"};
  FloatProperty m_hvLowLimit{this, "HVCutLow", 0.0};
  FloatProperty m_hvUpLimit{this, "HVCutUp", 1000000.0};
  BooleanProperty m_useDefaultHV{this, "UseDefaultHV", false};
  FloatProperty m_useHVLowLimit{this, "useHVLow", 19.};
  FloatProperty m_useHVUpLimit{this, "useHVUp", 1000000.0};
  StringProperty m_useHVChanCut{this, "useHVChan", "LOOSE"};
};

#endif // SCT_DCSCONDITIONSSTATCONDALG
