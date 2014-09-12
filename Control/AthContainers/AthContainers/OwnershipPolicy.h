/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHCONTAINERS_OWNERSHIPPOLICY_H
# define ATHCONTAINERS_OWNERSHIPPOLICY_H

/** @enum OwnershipPolicy
 * @brief  describes the possible element ownership policies
 *         (see e.g. DataVector)
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 *$Id: OwnershipPolicy.h 581165 2014-02-03 10:42:54Z krasznaa $	
 */

namespace SG {
  enum OwnershipPolicy {
    OWN_ELEMENTS,  ///< this data object owns its elements 
    VIEW_ELEMENTS  ///< this data object is a view, it does not own its elmts
  };
}

#endif // ATHCONTAINERS_OWNERSHIPPOLICY_H
