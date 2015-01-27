// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETAUXCOMPONENTMAP_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETAUXCOMPONENTMAP_V1_H

#include "xAODCore/AuxContainerBase.h"

#include "xAODMissingET/versions/MissingETCompositionBase.h"

#include <vector>

namespace xAOD
{
  /*! @brief Auxiliary data store for xAOD::MissingETComponentMap_v1
   *
   *  These are the underlying stores for all MissingETComponent_v1 entries in the composition map. 
   *
   *   @note The variable names used in this store are the keys in the auxiliary store. To access the data, these keys have to be used with
   *         exactly the same spelling as the variable names.
   */
  class MissingETAuxComponentMap_v1 : public AuxContainerBase
  {
  public:

    /*! @brief Default constructor */
    MissingETAuxComponentMap_v1();
    /*! @brief Base class destructor */
    virtual ~MissingETAuxComponentMap_v1();

  private:
    
    /*! @name Data content */
    /*!@{*/
    std::vector<MissingETBase::Types::metlink_t> metLink;            ///< @brief Link to MissingET_v1 object
    std::vector<MissingETBase::Types::objlink_vector_t> objectLinks; ///< @brief Link to object contributing to the MissingET_v1 object
    std::vector<std::vector<double> > wpx;                           ///< @brief Kinematic weight of object @f$ p_{x} @f$ ( @f$ w_{x} @f$ ) in MET
    std::vector<std::vector<double> > wpy;                           ///< @brief Kinematic weight of object @f$ p_{y} @f$ ( @f$ w_{y} @f$ ) in MET
    std::vector<std::vector<double> > wet;                           ///< @brief Kinematic weight of object @f$ p_{\rm T} @f$ ( @f$ w_{\rm T} @f$ ) in MET
    std::vector<MissingETBase::Types::bitmask_t> statusWord;         ///< @brief Status of MET term
    /*!@}*/
  };
}

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MissingETAuxComponentMap_v1, xAOD::AuxContainerBase );

#endif
