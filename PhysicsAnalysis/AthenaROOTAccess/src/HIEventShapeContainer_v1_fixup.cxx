/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAccess/src/HIEventShapeContainer_v1_fixup.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2015
 * @brief Run fixup for HIEventShapeContainer_v1.
 *
 * Persistency for HIEventShapeContainer_v1 is totally FUBAR.
 */


#include "AthenaROOTAccess/HIEventShapeContainer_v1_fixup.h"
#include "xAODHIEvent/versions/HIEventShapeContainer_v1.h"


namespace AthenaROOTAccess {


void HIEventShapeContainer_v1_fixup::fixupBeforeAux (void* data,
                                                     long long /*entry*/)
{
  xAOD::HIEventShapeContainer_v1* c =
    reinterpret_cast<xAOD::HIEventShapeContainer_v1*> (data);
  if (!c->trackIndices()) {
    // Was saved as a (*&(@Q*#$ view container.
    xAOD::HIEventShapeContainer_v1 tmp;
    tmp.swap (*c);
    for (xAOD::HIEventShape_v1* p : tmp)
      c->push_back (p);
  }
}


} // namespace AthenaROOTAccess

