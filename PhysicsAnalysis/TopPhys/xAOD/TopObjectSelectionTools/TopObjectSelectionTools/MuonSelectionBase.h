/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSELECTIONBASE_H_
#define MUONSELECTIONBASE_H_

#include "xAODMuon/Muon.h"

namespace top {

class MuonSelectionBase {
public:
    MuonSelectionBase();
    virtual ~MuonSelectionBase();
    
    ///No copy construction
    MuonSelectionBase(const MuonSelectionBase& rhs) = delete;

    ///No moving - really? Seems a bit strict.  We'll see
    MuonSelectionBase(MuonSelectionBase&& rhs) = delete;

    ///No assignment
    MuonSelectionBase& operator=(const MuonSelectionBase& rhs) = delete;

    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::Muon&) const = 0;

    ///For the loose (e.g. fakes) object selection
    virtual bool passSelectionLoose(const xAOD::Muon&) const = 0;

    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;    

};

}

std::ostream& operator<<(std::ostream& os, const top::MuonSelectionBase& selection);

#endif
