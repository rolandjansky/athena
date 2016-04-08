/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEDATA_EXAMPLETRACKCONTAINER_H
#define ATHENAPOOLEXAMPLEDATA_EXAMPLETRACKCONTAINER_H

/** @file ExampleTrackContainer.h
 *  @brief This file contains the class definition for the ExampleTrackContainer class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleTrackContainer.h,v 1.1 2005-12-08 20:02:03 gemmeren Exp $
 **/

#include "AthContainers/DataVector.h"
#include "AthenaPoolExampleData/ExampleTrack.h"

/** @class ExampleTrackContainer
 *  @brief This class provides a data vector for ExampleTrack objects in AthenaPool
 **/
class ExampleTrackContainer : public DataVector<ExampleTrack> {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleTrackContainer(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS) : DataVector<ExampleTrack>(ownPolicy) {};
   /// Destructor
   virtual ~ExampleTrackContainer() {};
};
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ExampleTrackContainer, 9103, 1)
#endif
