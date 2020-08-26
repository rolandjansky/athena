/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CUTBOOKKEEPERUTILS_H
#define CUTBOOKKEEPERUTILS_H

#include <xAODCutFlow/CutBookkeeperContainer.h>

namespace xAOD::CutBookkeeperUtils
{

  /// @brief Helper function to update a container with information from another one
  StatusCode updateContainer(xAOD::CutBookkeeperContainer *contToUpdate,
                             const xAOD::CutBookkeeperContainer *otherCont);

} // xAOD::CutBookkeepersUtils

#endif // CUTBOOKKEEPERUTILS_H
