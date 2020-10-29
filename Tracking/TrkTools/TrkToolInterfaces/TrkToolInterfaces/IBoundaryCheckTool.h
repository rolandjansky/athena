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
        Candidate,      /// within the sensitive area of an active element
        OnEdge,         /// close to the edge of an active element    
        Insensitive,    /// with the insensitive area of an active element 
        Outside,        /// outside the element 
        DeadElement,    /// within the nominally active area of a dead element 
        Error           /// error-state 
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
