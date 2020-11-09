// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ANALYSISTOP_TOPEVENT_KLFITTERRESULTCONTAINER_H
#define ANALYSISTOP_TOPEVENT_KLFITTERRESULTCONTAINER_H

// EDM include(s).
#include "AthContainers/DataVector.h"
#include "xAODCore/CLASS_DEF.h"

// Local include(s).
#include "TopEvent/KLFitterResult.h"

namespace xAOD {

   /// Definition of the @c KLFitterResultContainer type
   typedef DataVector< KLFitterResult > KLFitterResultContainer;

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF( xAOD::KLFitterResultContainer, 1116647492, 1 )

#endif // not ANALYSISTOP_TOPEVENT_KLFITTERRESULTCONTAINER_H
