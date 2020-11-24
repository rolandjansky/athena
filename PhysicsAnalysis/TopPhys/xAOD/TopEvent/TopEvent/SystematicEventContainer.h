// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ANALYSISTOP_TOPEVENT_SYSTEMATICEVENTCONTAINER_H
#define ANALYSISTOP_TOPEVENT_SYSTEMATICEVENTCONTAINER_H

// EDM include(s).
#include "AthContainers/DataVector.h"
#include "xAODCore/CLASS_DEF.h"

// Local include(s).
#include "TopEvent/SystematicEvent.h"

namespace xAOD {

   /// Definition of the @c xAOD::SystematicEventContainer type
   typedef DataVector< SystematicEvent > SystematicEventContainer;

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF(xAOD::SystematicEventContainer, 1133646307, 1)

#endif // not ANALYSISTOP_TOPEVENT_SYSTEMATICEVENTCONTAINER_H
