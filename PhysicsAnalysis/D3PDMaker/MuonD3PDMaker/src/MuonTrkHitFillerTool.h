/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef D3PDMAKER_MUONTRKHITFILLERTOOL_H
#define D3PDMAKER_MUONTRKHITFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODMuon/Muon.h"
#include <vector>
#include <stdint.h>


namespace D3PD {


class MuonTrkHitFillerTool
  : public BlockFillerTool<xAOD::Muon>
{
public:
  MuonTrkHitFillerTool (const std::string& type,
                        const std::string& name,
                        const IInterface* parent);

  virtual StatusCode book();

  virtual StatusCode fill (const xAOD::Muon& p);


private:
  std::vector<std::pair<xAOD::SummaryType, uint8_t*> > m_vars;

  // toggle saving of track summary for ID and MS separately
  bool m_saveIDMuonHitSummary;
  bool m_saveMSMuonHitSummary;
};


} // namespace D3PD

#endif // not D3PDMAKER_MUONTRKHITFILLERTOOL_H
