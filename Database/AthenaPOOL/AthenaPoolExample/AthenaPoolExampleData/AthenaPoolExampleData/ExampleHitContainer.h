/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLEXAMPLEDATA_EXAMPLEHITCONTAINER_H
#define ATHENAPOOLEXAMPLEDATA_EXAMPLEHITCONTAINER_H

/** @file ExampleHitContainer.h
 *  @brief This file contains the class definition for the ExampleHitContainer class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHitContainer.h,v 1.1 2005-12-08 20:02:03 gemmeren Exp $
 **/

#include "AthContainers/DataVector.h"
#include "AthenaPoolExampleData/ExampleHit.h"

/** @class ExampleHitContainer
 *  @brief This class provides a data vector for ExampleHit objects in AthenaPool
 **/
class ExampleHitContainer : public DataVector<ExampleHit> {

public: // Constructor and Destructor
   /// Default Constructor
   ExampleHitContainer(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS) : DataVector<ExampleHit>(ownPolicy) {};
   /// Destructor
   virtual ~ExampleHitContainer() {};
};
#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(ExampleHitContainer, 9102, 1)
#endif
