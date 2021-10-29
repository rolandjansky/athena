/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GenericRPCCache_H
#define GenericRPCCache_H

namespace MuonGM {

    struct GenericRPCCache {
        double stripSeparation{0.};
        double stripPanelThickness{0.};
        double rpcLayerThickness{0.};
        double centralSupPanelThickness{0.};
        double GasGapThickness{0.};
        double frontendBoardWidth{0.};
    };

}  // namespace MuonGM
#endif
