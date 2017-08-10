/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef G4USERACTIONS_STEPINFOCOLLECTION_H
#define G4USERACTIONS_STEPINFOCOLLECTION_H

// athena includes
#include "DataModel/DataVector.h"

// local include
#include "LArG4GenShowerLib/StepInfo.h"
//#include <string>
// Namespace for the ShowerLib related classes
namespace ShowerLib {

  /**
   *
   *   @short Class for collection of StepInfo class (G4 hits)
   *
   *
   *  @author Wolfgang Ehrenfeld, University of Hamburg, Germany
   *  @author Sasha Glazov, DESY Hamburg, Germany
   *
   * @version $Id: StepInfoCollection.h 498105 2012-04-27 11:38:03Z gsedov $
   *
   */

  class StepInfoCollection : public DataVector<StepInfo> {

  public:

	  StepInfoCollection() : detector(), invalid_energy(0) {}

	  char  detector[10];            //10 should be enough
	  float invalid_energy;          //the amount of energy deposited outside the region

  private:

  };

} // namespace ShowerLib

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif

CLASS_DEF( ShowerLib::StepInfoCollection , 1151967000 , 1 )

#endif

