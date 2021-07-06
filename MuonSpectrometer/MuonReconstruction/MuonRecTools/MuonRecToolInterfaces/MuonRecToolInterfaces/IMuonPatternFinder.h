/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONPATTERNFINDER_H
#define IMUONPATTERNFINDER_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonPatternFinder("Muon::IMuonPatternFinder", 1, 0);

namespace Trk {
    class PrepRawData;
}

namespace Muon {

    class MuonPattern;

    /** interface class for tools producing Muon::MuonPattern objects from Trk::PrepRawData objects */
    class IMuonPatternFinder : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID();

        /** @brief find patterns using passed Trk::PrepRawData
            @param prds a vector of Trk::PrepRawData
            @return a pointer to a vector of Muon::MuonPattern objects, zero if no patterns found.
                    The ownership of the patterns is passed to the client calling the tool.
        */
        virtual std::vector<const MuonPattern*>* find(std::vector<const Trk::PrepRawData*>& prds) const = 0;
    };

    inline const InterfaceID& IMuonPatternFinder::interfaceID() { return IID_IMuonPatternFinder; }
}  // namespace Muon

#endif  // IMuonPatternFinder_H
