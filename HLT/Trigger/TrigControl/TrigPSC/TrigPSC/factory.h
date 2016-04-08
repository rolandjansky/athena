/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * factory.h
 *
 *  Created on: Apr 29, 2013
 *      Author: ricab
 */

#ifndef FACTORY_H_
#define FACTORY_H_

#include "hltinterface/HLTInterface.h"

extern "C" {
  hltinterface::HLTInterface* hlt_factory();
}


#endif /* FACTORY_H_ */
