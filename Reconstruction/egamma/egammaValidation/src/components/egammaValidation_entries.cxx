/*
 * Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
 */

#include <GaudiKernel/DeclareFactoryEntries.h>

#include "../../egammaValidation/EgammaMonitoring.h"

DECLARE_COMPONENT( EgammaMonitoring )

DECLARE_ALGORITHM_FACTORY (EgammaMonitoring)

DECLARE_FACTORY_ENTRIES  ( egammaValidation )
{

  DECLARE_ALGORITHM (EgammaMonitoring);

}
