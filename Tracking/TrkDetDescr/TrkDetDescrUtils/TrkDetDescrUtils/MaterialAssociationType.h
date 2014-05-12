/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKDETDESCRUTILS_MATERIALASSOCIATIONTYPE_H
#define TRKDETDESCRUTILS_MATERIALASSOCIATIONTYPE_H

namespace Trk {

  /** @enum MaterialAssociationType
    */
  enum MaterialAssociationType {
                          EffectiveNumSteps = 0,      // cout the number of steps per layer
                          EffectiveNumAtoms = 1,      // compute the effective number of atoms
                          RadiationLength   = 2,       // weight by radiation length
                          StepLength        = 4
                       };

}


#endif
