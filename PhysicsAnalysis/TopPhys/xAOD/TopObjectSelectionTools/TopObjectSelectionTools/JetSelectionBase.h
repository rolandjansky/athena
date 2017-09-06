/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETSELECTIONBASE_H_
#define JETSELECTIONBASE_H_

#include "xAODJet/Jet.h"

namespace top {

class JetSelectionBase {
public:
    JetSelectionBase();

    virtual ~JetSelectionBase();

    virtual bool passSelection(const xAOD::Jet&) = 0;

    virtual void print(std::ostream&) const = 0;
};

}

std::ostream& operator<<(std::ostream& os, const top::JetSelectionBase& selection);

#endif
