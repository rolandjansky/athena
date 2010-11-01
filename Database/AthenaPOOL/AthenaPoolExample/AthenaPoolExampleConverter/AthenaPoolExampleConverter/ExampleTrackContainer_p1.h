/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLECONVERTER_EXAMPLETRACKCONTAINER_P1_H
#define ATHENAPOOLEXAMPLECONVERTER_EXAMPLETRACKCONTAINER_P1_H

/** @file ExampleTrackContainer_p1.h
 *  @brief This file contains the class definition for the ExampleTrackContainer_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleTrackContainer_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include "AthenaPoolExampleConverter/ExampleTrack_p1.h"

/** @class ExampleTrackContainer_p1
 *  @brief This class provides a persistent state for the ExampleTrackContainer class
 **/
class ExampleTrackContainer_p1 : public std::vector<ExampleTrack_p1> {
};
#endif
