/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHITCONTAINER_P1_H
#define ATHENAPOOLEXAMPLECONVERTER_EXAMPLEHITCONTAINER_P1_H

/** @file ExampleHitContainer_p1.h
 *  @brief This file contains the class definition for the ExampleHitContainer_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHitContainer_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include "AthenaPoolExampleConverter/ExampleHit_p1.h"
#include <vector>

/** @class ExampleHitContainer_p1
 *  @brief This class provides a persistent state for the ExampleHitContainer class
 **/
class ExampleHitContainer_p1 : public std::vector<ExampleHit_p1> {
};
#endif
