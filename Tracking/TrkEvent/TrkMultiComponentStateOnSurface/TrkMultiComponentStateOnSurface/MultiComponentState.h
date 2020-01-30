/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        MultiComponentState.h  -  description
                        -------------------------------------
begin                : Friday 31st December 2004
author               : atkinson, amorley,anastopoulos
email                : Anthony.Morley@cern.ch
decription           : Basic definitions for a track state described by more
                       than one set of Track Parameters. The resulting state is
                       a mixture of components.
                       Each component is described by a ComponentParameters object
                       which is of the type std::pair< std::unique_ptr<TrackParameters>, double>
                       The double describes the weighting of the component -
                       or its relative importance in the mixture.
*********************************************************************************/

#ifndef TrkMultiComponentState
#define TrkMultiComponentState

#include "TrkMultiComponentStateOnSurface/ComponentParameters.h"
#include <vector>

class MsgStream;
namespace Trk {
/**
 * MultiComponentState is just a typedef
 */
typedef std::vector<ComponentParameters> MultiComponentState;

namespace MultiComponentStateHelpers {
/** Clone method */
std::unique_ptr<MultiComponentState>
clone(const MultiComponentState& in);

/** Clone with covariance matricies scaled by a factor */
std::unique_ptr<MultiComponentState>
cloneWithScaledError(const MultiComponentState& in, double);

/** Clone with covariance matrix componants scaled by individual factors
    This will only work if there are 5 track parameters in each componant
*/
std::unique_ptr<MultiComponentState>
cloneWithScaledError(const MultiComponentState&, double, double, double, double, double);

/** Check to see if all components in the state have measured track parameters */
bool
isMeasured(const MultiComponentState& in);

/** Performing renormalisation of total state weighting to one */
void
renormaliseState(MultiComponentState&, double norm = 1);

/** Dump methods */
MsgStream&
dump(MsgStream&, const MultiComponentState&);
std::ostream&
dump(std::ostream&, const MultiComponentState&);

/** Overload of << operator for MsgStream and std::ostream */
MsgStream&
operator<<(MsgStream&, const MultiComponentState&);
std::ostream&
operator<<(std::ostream&, const MultiComponentState&);
} // end of MultiComponentStateHelpers
} // end Trk namespace

#endif
