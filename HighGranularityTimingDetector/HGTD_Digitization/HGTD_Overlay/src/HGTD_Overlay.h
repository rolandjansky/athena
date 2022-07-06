/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HGTDOVERLAY_HGTDOVERLAY_H
#define HGTDOVERLAY_HGTDOVERLAY_H

#include <IDC_OverlayBase/IDC_OverlayBase.h>
#include <HGTD_RawData/HGTD_RDO_Container.h>


class HGTD_Overlay : public IDC_OverlayBase
{
public:

  HGTD_Overlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  SG::ReadHandleKey<HGTD_RDO_Container> m_bkgInputKey{ this, "BkgInputKey", "Bkg_HGTD_RDOs", "ReadHandleKey for Background Input HGTD_RDO_Container" };
  SG::ReadHandleKey<HGTD_RDO_Container> m_signalInputKey{ this, "SignalInputKey", "Sig_HGTD_RDOs", "ReadHandleKey for Signal Input HGTD_RDO_Container" };
  SG::WriteHandleKey<HGTD_RDO_Container> m_outputKey{ this, "OutputKey", "HGTD_RDOs", "WriteHandleKey for Output HGTD_RDO_Container" };
};

#endif // HGTDOVERLAY_HGTDOVERLAY_H
