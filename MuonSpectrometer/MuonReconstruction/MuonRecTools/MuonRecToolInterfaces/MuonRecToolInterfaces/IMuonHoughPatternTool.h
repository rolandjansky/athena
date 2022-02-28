/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNTOOLS_IMUONHOUGHPATTERNTOOL_H
#define MUONHOUGHPATTERNTOOLS_IMUONHOUGHPATTERNTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonHoughPatternEvent/MuonHoughPatternCollection.h"
#include "MuonPattern/MuonPatternCollection.h"

class MuonHoughHitContainer;

class IMuonHoughPatternTool : virtual public IAlgTool {
public:
    /** @todo should be rethought and possibly using the Moore Interface */

    DeclareInterfaceID(IMuonHoughPatternTool, 1, 0);

    /** Builds Patterns */
    virtual void makePatterns(const MuonHoughHitContainer& hitcontainer, MuonHoughPatternContainerShip& houghpatterns) const = 0;

    /** returns phi patterns */
    virtual std::unique_ptr<MuonPrdPatternCollection> getPhiMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const = 0;
    /** returns eta patterns */
    virtual std::unique_ptr<MuonPrdPatternCollection> getEtaMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const = 0;
    /** returns houghpatterns arrays*/
    virtual MuonHoughPatternContainerShip emptyHoughPattern() const = 0;
};

#endif  // MUONHOUGHPATTERNTOOLS_IMUONHOUGHPATTERNTOOL_H
