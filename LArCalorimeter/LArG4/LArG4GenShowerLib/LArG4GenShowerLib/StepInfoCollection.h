/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4GENSHOWERLIB_STEPINFOCOLLECTION_H
#define LARG4GENSHOWERLIB_STEPINFOCOLLECTION_H

// athena includes
#include "AthContainers/DataVector.h"

// local include
#include "LArG4GenShowerLib/StepInfo.h"
//#include <string>

/// Namespace for the ShowerLib related classes
namespace ShowerLib
{

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

  class StepInfoCollection : public DataVector<StepInfo>
  {

  public:

    StepInfoCollection() : detector(), invalid_energy(0) {}

    char  detector[10];            //10 should be enough
    float invalid_energy;          //the amount of energy deposited outside the region

  };

} // namespace ShowerLib

#include "AthenaKernel/CLASS_DEF.h"

CLASS_DEF( ShowerLib::StepInfoCollection , 1151967000 , 1 )

#endif // LARG4GENSHOWERLIB_STEPINFOCOLLECTION_H
