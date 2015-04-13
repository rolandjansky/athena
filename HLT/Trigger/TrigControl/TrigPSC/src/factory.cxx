/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * factory.cxx
 *
 *  Created on: Apr 29, 2013
 *      Author: ricab
 */

#include "TrigPSC/factory.h"
#include "TrigPSC/Psc.h"

hltinterface::HLTInterface* hlt_factory()
{
  return new psc::Psc();
}

