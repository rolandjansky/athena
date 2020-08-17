/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonCluster.h
//   Header file for class CscPrepData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Muon
///////////////////////////////////////////////////////////////////
// Version 1.0 03/7/2007 Niels van Eldik
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_MUONCLUSTER_H
#define MUONPREPRAWDATA_MUONCLUSTER_H

// Base classes
#include "TrkPrepRawData/PrepRawData.h"
#include "Identifier/IdentifierHash.h"
#include "MuonReadoutGeometry/MuonClusterReadoutElement.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "CxxUtils/CachedUniquePtr.h"

#include<vector>

namespace Muon
{

  /**@brief Class representing clusters in the muon system.
     @author Niels.van.Eldik@cern.ch
     @author Edward.Moyse@cern.ch
     @author Ketevi A. Assamagan*/
  class MuonCluster :   public Trk::PrepRawData
  {

  public:

    /**@brief Default constructor.

    For use by POOL only - do not use! */
    MuonCluster();
    MuonCluster(const MuonCluster &);
    MuonCluster(MuonCluster &&) noexcept = default;
    MuonCluster &operator=(const MuonCluster &);
    MuonCluster &operator=(MuonCluster &&) noexcept = default;

    /** @brief Full constructor.
       @param RDOId The channel identifier of the central measurement of the cluster.
       @param collectionHash the hash associated with the PRD collection in storegate
       @param locpos The position of the cluster in the surface reference frame.
                the first coordinate is the precision coordinate, the second is set to
                the centre of the strip. Ownership passes to this object!
       @param rdoList List of channels associated with cluster
       @param locErrMat The measurement errors. Ownership passes to this object!
    */
    MuonCluster( const Identifier& RDOId,
                 const IdentifierHash& collectionHash,
                 const Amg::Vector2D& locpos,
                 const std::vector<Identifier>& rdoList,
                 const Amg::MatrixX* locErrMat
         );

    /** @brief Destructor*/
    virtual ~MuonCluster();

    /** @brief Returns the global position of the measurement (calculated on the fly) */
    virtual const Amg::Vector3D& globalPosition() const = 0;

    /** @brief Returns the IdentifierHash corresponding to the PRD collection in the PRD container. */
    virtual IdentifierHash collectionHash() const;

    /** @brief Returns the detector element corresponding to this PRD.
    The pointer will be zero if the det el is not defined (i.e. it was not passed in by the ctor)*/
    virtual const MuonGM::MuonClusterReadoutElement* detectorElement() const override = 0;

    /** Interface method checking the type*/
    virtual bool type(Trk::PrepRawDataType::Type type) const override = 0;

    /** @brief Dumps information about the PRD*/
    virtual MsgStream&    dump( MsgStream&    stream) const override;

    /** @brief Dumps information about the PRD*/
    virtual std::ostream& dump( std::ostream& stream) const override;

  protected:
    /**@brief Global position of measurement.
    Calculated on demand and cached */
    CxxUtils::CachedUniquePtr<const Amg::Vector3D> m_globalPosition;

  };

  // /////////////////////////////////////////////////////////////////
  // Inline methods:
  // /////////////////////////////////////////////////////////////////



  inline IdentifierHash MuonCluster::collectionHash() const
  {
      return getHashAndIndex().collHash();
  }


}

#endif // MUONPREPRAWDATA_MUONCLUSTER_H

