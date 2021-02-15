// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETAUXASSOCIATIONMAP_V1_H
#define XAODMISSINGET_VERSIONS_MISSINGETAUXASSOCIATIONMAP_V1_H

#include "xAODCore/AuxContainerBase.h"

#include "xAODMissingET/versions/MissingETCompositionBase.h"

#include <vector>

namespace xAOD
{
  /*! @brief Auxiliary data store for xAOD::MissingETAssociationMap_v1
   *
   *  These are the underlying stores for all MissingETAssociation_v1 entries in the composition map. 
   *
   *   @note The variable names used in this store are the keys in the auxiliary store. To access the data, these keys have to be used with
   *         exactly the same spelling as the variable names.
   */
  class MissingETAuxAssociationMap_v1 : public AuxContainerBase
  {
  public:

    /*! @brief Default constructor */
    MissingETAuxAssociationMap_v1();
    /*! @brief Base class destructor */
    virtual ~MissingETAuxAssociationMap_v1();

  private:
    
    /*! @name Data content */
    /*!@{*/
    std::vector<MissingETBase::Types::jetlink_t> jetLink;            ///< @brief Link to MissingET_v1 object
    std::vector<MissingETBase::Types::objlink_vector_t> objectLinks; ///< @brief Link to object contributing to the MissingET_v1 object
    std::vector<std::vector<float> > calpx;                          ///< @brief Object constituent calo 4-vector @f$ P_{x} @f$ for overlap removal
    std::vector<std::vector<float> > calpy;                          ///< @brief Object constituent calo 4-vector @f$ P_{y} @f$ for overlap removal
    std::vector<std::vector<float> > calpz;                          ///< @brief Object constituent calo 4-vector @f$ P_{z} @f$ for overlap removal
    std::vector<std::vector<float> > cale;                           ///< @brief Object constituent calo 4-vector @f$ E @f$ for overlap removal
    std::vector<std::vector<float> > calsumpt;                       ///< @brief Object constituent calo 4-vector sumpt for overlap removal
    //
    std::vector<std::vector<float> > trkpx;                          ///< @brief Object constituent track 4-vector @f$ P_{x} @f$ for overlap removal
    std::vector<std::vector<float> > trkpy;                          ///< @brief Object constituent track 4-vector @f$ P_{y} @f$ for overlap removal
    std::vector<std::vector<float> > trkpz;                          ///< @brief Object constituent track 4-vector @f$ P_{z} @f$ for overlap removal
    std::vector<std::vector<float> > trke;                           ///< @brief Object constituent track 4-vector @f$ E @f$ for overlap removal
    std::vector<std::vector<float> > trksumpt;                       ///< @brief Object constituent track 4-vector sumpt for overlap removal
    //
    std::vector<float> jettrkpx;                                     ///< @brief Jet track 4-vector @f$ P_{x} @f$ for overlap removal
    std::vector<float> jettrkpy;                                     ///< @brief Jet track 4-vector @f$ P_{y} @f$ for overlap removal
    std::vector<float> jettrkpz;                                     ///< @brief Jet track 4-vector @f$ P_{z} @f$ for overlap removal
    std::vector<float> jettrke;                                      ///< @brief Jet track 4-vector @f$ E @f$ for overlap removal
    std::vector<float> jettrksumpt;                                  ///< @brief Jet track 4-vector sumpt for overlap removal
    //
    std::vector<std::vector<std::vector<size_t> > > overlapIndices;  ///< @brief Indices of objects that overlap one another in an association
    std::vector<std::vector<std::vector<unsigned char> > > overlapTypes;  ///< @brief Types of overlap between matched objects
    std::vector<char> isMisc;                                        ///< @brief Flags an association as the miscellaneous association
    /*!@}*/
  };
}

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MissingETAuxAssociationMap_v1, xAOD::AuxContainerBase );
#endif
