/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUSELECTIONBASE_H_
#define TAUSELECTIONBASE_H_

#include "xAODTau/TauJet.h"

namespace top {

class TauSelectionBase {
public:
    TauSelectionBase();
    virtual ~TauSelectionBase();
    
    ///No copy construction
    TauSelectionBase(const TauSelectionBase& rhs) = delete;

    ///No moving - really? Seems a bit strict.  We'll see
    TauSelectionBase(TauSelectionBase&& rhs) = delete;

    ///No assignment
    TauSelectionBase& operator=(const TauSelectionBase& rhs) = delete;

    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::TauJet&) const = 0;

    ///For the loose (e.g. fakes) object selection
    virtual bool passSelectionLoose(const xAOD::TauJet&) const = 0;

    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;    
};

}

std::ostream& operator<<(std::ostream& os, const top::TauSelectionBase& selection);

#endif
