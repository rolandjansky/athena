// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETCONTAINER_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETCONTAINER_V1_H

#include "AthContainers/DataVector.h"

#include "xAODMissingET/versions/MissingETBase.h"
#include "xAODMissingET/versions/MissingET_v1.h"

#include <string>

namespace xAOD
{
  /*! @brief Container for xAOD::MissingET_v1 objects */
  class MissingETContainer_v1 : public DataVector<MissingET_v1>
  {
  public:
    /*! @brief Default constructor */
    MissingETContainer_v1(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
    MissingETContainer_v1(MissingETContainer_v1::iterator first, MissingETContainer_v1::iterator last,
			  SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);
    virtual ~MissingETContainer_v1(); /*!< @brief Base class destructor */


    /*! @name Special object access */
    /*!@{*/
    using DataVector<MissingET_v1>::operator[];                        //  Pull in usage of the base class operator[]
    const MissingET_v1* operator[](const std::string& name) const;             /*!< @brief Allocate a non-modifiable xAOD::MissingET_v1 object by name */
    MissingET_v1*       operator[](const std::string& name);                   /*!< @brief Allocate a modifiable xAOD::MissingET_v1 object by name */
    /*!@}*/
    /*! @name Object search operations */
    /*!@{*/
    const_iterator find(const std::string& name) const;            /*!< @brief Find non-modifiable MET object by name */
    iterator       find(const std::string& name);                   /*!< @brief Find modifiable MET object by name */

    const_iterator find(MissingETBase::Types::bitmask_t src) const; /*!< @brief Find non-modifiable MET object by source indicator */
    iterator       find(MissingETBase::Types::bitmask_t src);       /*!< @brief Find modifiable MET object by source indicator */
    /*!@}*/
  };
}

#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::MissingETContainer_v1, DataVector<xAOD::MissingET_v1>);

#include "xAODMissingET/versions/MissingETContainer_v1.icc"

#endif
