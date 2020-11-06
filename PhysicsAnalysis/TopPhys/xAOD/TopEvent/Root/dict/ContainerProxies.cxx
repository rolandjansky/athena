//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "TopEvent/KLFitterResultContainer.h"
#include "TopEvent/PseudoTopResultContainer.h"
#include "TopEvent/SystematicEventContainer.h"

// EDM include(s).
#include "xAODCore/AddDVProxy.h"

// Set up the collection proxies.
ADD_NS_DV_PROXY( xAOD, KLFitterResultContainer );
ADD_NS_DV_PROXY( xAOD, PseudoTopResultContainer );
ADD_NS_DV_PROXY( xAOD, SystematicEventContainer );
