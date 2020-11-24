/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#ifndef FWDELECTRONSELECTIONBASE_H_
#define FWDELECTRONSELECTIONBASE_H_

#include "xAODEgamma/Electron.h"

#include <memory>

namespace top {
/**
 * @brief Base class for implementing an electron selection.
 *
 * If you want to build a new electron selection, you should inherit from this.
 * Why didn't I use ASG tools as a base?  I suspect it's a bit over-complicated
 * for this simple task, and this way we can force people to implement
 * passSelection for an Electron.  I think it would be a less-clean solution if
 * we were to use the 'proper' CP tool style.
 */
  class FwdElectronSelectionBase {
  public:
    FwdElectronSelectionBase();
    virtual ~FwdElectronSelectionBase();

    ///No copy construction
    FwdElectronSelectionBase(const FwdElectronSelectionBase& rhs) = delete;

    ///No moving - really? Seems a bit strict.  We'll see
    FwdElectronSelectionBase(FwdElectronSelectionBase&& rhs) = delete;

    ///No assignment
    FwdElectronSelectionBase& operator = (const FwdElectronSelectionBase& rhs) = delete;

    ///For the main analysis object selection
    virtual bool passSelection(const xAOD::Electron&, int bcid, int runNumber) const = 0;

    ///For the loose (e.g. fakes) object selection
    virtual bool passSelectionLoose(const xAOD::Electron&, int bcid, int runNumber) const = 0;

    ///Because everybody likes to know what object definitions they ran with
    virtual void print(std::ostream&) const = 0;
  };

  std::ostream& operator << (std::ostream& os, const top::FwdElectronSelectionBase& selection);
}

#endif
