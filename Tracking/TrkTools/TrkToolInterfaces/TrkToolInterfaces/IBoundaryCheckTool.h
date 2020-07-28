/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef ATHENA_TRK_TOOLS_INTERFACES_BOUNDARYCHECKTOOL
#define ATHENA_TRK_TOOLS_INTERFACES_BOUNDARYCHECKTOOL

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

static const InterfaceID IID_IBoundaryCheckTool("Trk::IBoundaryCheckTool", 1, 0);

namespace Trk {
    enum class BoundaryCheckResult {
        Candidate,
        Insensitive,
        DeadElement,
        Error
    };
    
    class IBoundaryCheckTool : virtual public IAlgTool {
    public:
        virtual BoundaryCheckResult boundaryCheck(const Trk::TrackParameters &) const = 0;

        static const InterfaceID &interfaceID() {
            return IID_IBoundaryCheckTool;
        }
    };
}

#endif
