/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// Gaudi/Athena include(s):
#include "SGTools/BaseInfo.h"

// EDM include(s):
#include "EventKernel/INavigable4Momentum.h"
#include "TrigCaloEvent/TrigT2Jet.h"

SG_ADD_BASE( TrigT2Jet, INavigable4Momentum );
