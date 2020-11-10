// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULTCONTAINER_H
#define ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULTCONTAINER_H

// EDM include(s).
#include "AthContainers/DataVector.h"
#include "xAODCore/CLASS_DEF.h"

// Local include(s).
#include "TopEvent/PseudoTopResult.h"

namespace xAOD {

   /// Definition of the @c xAOD::PseudoTopResultContainer type
   typedef DataVector< PseudoTopResult > PseudoTopResultContainer;

} // namespace xAOD

// Define a ClassID for the type.
CLASS_DEF( xAOD::PseudoTopResultContainer, 1219079565, 1 )

#endif // not ANALYSISTOP_TOPEVENT_PSEUDOTOPRESULTCONTAINER_H
