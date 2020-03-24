/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef SOFTMUONSELECTIONBASE_H_
#define SOFTMUONSELECTIONBASE_H_

#include "xAODMuon/Muon.h"

namespace top {
  class SoftMuonSelectionBase {
  public:
    SoftMuonSelectionBase();
    virtual ~SoftMuonSelectionBase();

    ///No copy construction
    SoftMuonSelectionBase(const SoftMuonSelectionBase& rhs) = delete;

    ///No moving - really? Seems a bit strict.  We'll see
    SoftMuonSelectionBase(SoftMuonSelectionBase&& rhs) = delete;

    ///No assignment
    SoftMuonSelectionBase& operator = (const SoftMuonSelectionBase& rhs) = delete;

    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::Muon&) const = 0;

    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;
  };

  std::ostream& operator << (std::ostream& os, const top::SoftMuonSelectionBase& selection);
}

#endif
