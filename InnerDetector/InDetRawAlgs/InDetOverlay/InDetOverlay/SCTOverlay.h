/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETOVERLAY_SCTOVERLAY_H
#define INDETOVERLAY_SCTOVERLAY_H

#include "IDC_OverlayBase/IDC_OverlayBase.h"
#include "InDetRawData/SCT_RDO_Container.h"

class SCT_ID;

class SCTOverlay : public IDC_OverlayBase
{
public:

  SCTOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

  // SCT collection merging needs an ID helper.  Instead of retrieving
  // it thousands of times per event in the collection merging code,
  // do it once in InDetOverlay initialization and make available here
  const SCT_ID *get_sct_id() const { return m_sctId; }

  enum SCT_numbers {NextBC=0, CurrentBC=1, PreviousBC=2, AnyBC=3, NumberOfBCs=3,
                    NumberOfBitSets=4, NumberOfStrips=768,
                    BkgSource=0, SignalSource=1, NumberOfSources=2};

private:
  const SCT_ID *m_sctId{};

  SG::ReadHandleKey<SCT_RDO_Container> m_bkgInputKey{ this, "BkgInputKey", "OriginalEvent_SG+SCT_RDOs", "ReadHandleKey for Background Input SCT_RDO_Container" };
  SG::ReadHandleKey<SCT_RDO_Container> m_signalInputKey{ this, "SignalInputKey", "BkgEvent_0_SG+SCT_RDOs", "ReadHandleKey for Signal Input SCT_RDO_Container" };
  SG::WriteHandleKey<SCT_RDO_Container> m_outputKey{ this, "OutputKey", "StoreGateSvc+SCT_RDOs", "WriteHandleKey for Output SCT_RDO_Container" };

  BooleanProperty m_includeBkg { this, "includeBkg", true, "Include Background RDO Container" };
};

#endif // INDETOVERLAY_SCTOVERLAY_H
