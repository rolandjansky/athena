// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETAUXCONTAINER_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETAUXCONTAINER_V1_H

#include "xAODCore/AuxContainerBase.h"

#include "xAODMissingET/versions/MissingET_v1.h"

#include <vector> 

namespace xAOD
{
  /*! @brief Auxiliary data store for xAOD::MissingETContainer */
  class MissingETAuxContainer_v1 : public AuxContainerBase
  {
  public:
    /*! @brief Default constructor */
    MissingETAuxContainer_v1();
    /*! @brief Base class destructor */
    virtual ~MissingETAuxContainer_v1();

  private:

    /*! @name Data content
     * 
     *   The storage provided here is the data content of the xAOD::MissingET
     *   objects collected in xAOD::MissingETContainer. 
     *
     *   @note The variable names used here are the keys in the auxiliary 
     *         store. To access the data, these keys have to be used with
     *         exactly the same spelling as the variable names.
     */
    /*!@{*/
    std::vector<float>                           mpx;    ///< @brief @f$ p_{x,{\rm miss}} = -\sum p_{x} @f$
    std::vector<float>                           mpy;    ///< @brief @f$ p_{y,{\rm miss}} = -\sum p_{x} @f$
    std::vector<float>                           sumet;  ///< @brief @f$ \sigma E_{\rm T} = \sum p_{\rm T} @f$
    std::vector<std::string>                     name;   ///< @brief Name of MET contribution
    std::vector<MissingETBase::Types::bitmask_t> source; ///< @brief Source of MET contribution
    /*!@}*/
  };
}

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MissingETAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODMISSINGET_VERSIONS_MISSINGETAUXCONTAINER_V1_H
