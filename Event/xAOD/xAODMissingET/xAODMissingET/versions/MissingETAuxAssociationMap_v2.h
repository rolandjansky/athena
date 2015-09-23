// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETAUXASSOCIATIONMAP_v2_H
#define XAODMISSINGET_VERSIONS_MISSINGETAUXASSOCIATIONMAP_v2_H

#include "xAODCore/AuxContainerBase.h"

#include "xAODMissingET/versions/MissingETCompositionBase.h"

#include <vector>

namespace xAOD
{
  /*! @brief Auxiliary data store for xAOD::MissingETAssociationMap_v2
   *
   *  These are the underlying stores for all MissingETAssociation_v2 entries in the composition map. 
   *
   *   @note The variable names used in this store are the keys in the auxiliary store. To access the data, these keys have to be used with
   *         exactly the same spelling as the variable names.
   */
  class MissingETAuxAssociationMap_v2 : public AuxContainerBase
  {
  public:

    /*! @brief Default constructor */
    MissingETAuxAssociationMap_v2();
    /*! @brief Base class destructor */
    virtual ~MissingETAuxAssociationMap_v2();

  private:
    
    /*! @name Data content */
    /*!@{*/
    std::vector<MissingETBase::Types::jetlink_t> jetLink;            ///< @brief Link to MissingET_v2 object
    std::vector<MissingETBase::Types::objlink_vector_t> objectLinks; ///< @brief Link to object contributing to the MissingET_v2 object
    std::vector<std::vector<double> > calpx;                         ///< @brief Object constituent calo 4-vector @f$ P_{x} @f$ for overlap removal
    std::vector<std::vector<double> > calpy;                         ///< @brief Object constituent calo 4-vector @f$ P_{y} @f$ for overlap removal
    std::vector<std::vector<double> > calpz;                         ///< @brief Object constituent calo 4-vector @f$ P_{z} @f$ for overlap removal
    std::vector<std::vector<double> > cale;                          ///< @brief Object constituent calo 4-vector @f$ E @f$ for overlap removal
    std::vector<std::vector<double> > calsumpt;                      ///< @brief Object constituent calo 4-vector sumpt for overlap removal
    std::vector<std::vector<MissingETBase::Types::bitmask_t> > calkey;                        ///< @brief Object constituent calo region key for overlap removal
    //
    std::vector<std::vector<double> > trkpx;                         ///< @brief Object constituent track 4-vector @f$ P_{x} @f$ for overlap removal
    std::vector<std::vector<double> > trkpy;                         ///< @brief Object constituent track 4-vector @f$ P_{y} @f$ for overlap removal
    std::vector<std::vector<double> > trkpz;                         ///< @brief Object constituent track 4-vector @f$ P_{z} @f$ for overlap removal
    std::vector<std::vector<double> > trke;                          ///< @brief Object constituent track 4-vector @f$ E @f$ for overlap removal
    std::vector<std::vector<double> > trksumpt;                      ///< @brief Object constituent track 4-vector sumpt for overlap removal
    std::vector<std::vector<MissingETBase::Types::bitmask_t> > trkkey;                        ///< @brief Object constituent track region key for overlap removal
    //
    std::vector<double> jettrkpx;                                    ///< @brief Jet track 4-vector @f$ P_{x} @f$ for overlap removal
    std::vector<double> jettrkpy;                                    ///< @brief Jet track 4-vector @f$ P_{y} @f$ for overlap removal
    std::vector<double> jettrkpz;                                    ///< @brief Jet track 4-vector @f$ P_{z} @f$ for overlap removal
    std::vector<double> jettrke;                                     ///< @brief Jet track 4-vector @f$ E @f$ for overlap removal
    std::vector<double> jettrksumpt;                                 ///< @brief Jet track 4-vector sumpt for overlap removal
    //
    std::vector<std::vector<std::vector<size_t> > > overlapIndices;  ///< @brief Indices of objects that overlap one another in an association
    std::vector<std::vector<std::vector<unsigned char> > > overlapTypes;  ///< @brief Types of overlap between matched objects
    std::vector<char> isMisc;                                        ///< @brief Flags an association as the miscellaneous association
    /*!@}*/
  };
}

#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::MissingETAuxAssociationMap_v2, xAOD::AuxContainerBase );
#endif
