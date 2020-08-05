/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCluster.h
//   Header file for class SiCluster
///////////////////////////////////////////////////////////////////
// Class to implement Cluster for Si
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_SICLUSTER_H
#define TRKPREPRAWDATA_SICLUSTER_H

// Base class
#include "Identifier/Identifier.h"
#include "InDetPrepRawData/SiWidth.h"
#include "InDetReadoutGeometry/SiDetectorElement.h" // cant be forward declared
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkSurfaces/Surface.h"
#include <memory>

class PixelClusterContainerCnv;
class SCT_ClusterContainerCnv;

class MsgStream;

class PixelClusterContainerCnv_p2;
class PixelClusterContainerCnv_p0;
class SCT_ClusterContainerCnv_p0;
class SCT_ClusterContainerCnv_p1;
class SiClusterCnv_p1;

namespace InDet {

class SiCluster : public Trk::PrepRawData
{
  /**
   * @name Friend class declarations
   */
  //@{
  friend class ::PixelClusterContainerCnv;
  friend class ::SCT_ClusterContainerCnv;
  friend class ::PixelClusterContainerCnv_p2;
  friend class PixelClusterContainerCnv_p1;
  friend class ::PixelClusterContainerCnv_p0;
  friend class SCT_ClusterContainerCnv_p1;
  friend class ::SCT_ClusterContainerCnv_p0;
  friend class ::SiClusterCnv_p1;
  //@}

public:
  // Used by TPCnv converters
  SiCluster() = default;
  /// Copy constructor
  SiCluster(const SiCluster&) = default;
  /// Move constructor
  SiCluster(SiCluster&&) = default;
  /// Assignment operator
  SiCluster& operator=(const SiCluster&) = default;
  /// Move assignment operator
  SiCluster& operator=(SiCluster&&) = default;

  /**
   * Constructor with parameters using pointer of Amg::MatrixX.
   * Last parameter might not be always filled and will be nullptr by
   * default. The others including SiDetectorElement have to be given!
   */
  SiCluster(const Identifier& RDOId,
            const Amg::Vector2D& locpos,
            const std::vector<Identifier>& rdoList,
            const InDet::SiWidth& width,
            const InDetDD::SiDetectorElement* detEl,
            const Amg::MatrixX* locErrMat = nullptr);

  /**
   * Constructor with parameters using unique_ptr of Amg::MatrixX.
   * All parameters have to be given!
   */
  SiCluster(const Identifier& RDOId,
            const Amg::Vector2D& locpos,
            std::vector<Identifier>&& rdoList,
            const InDet::SiWidth& width,
            const InDetDD::SiDetectorElement* detEl,
            std::unique_ptr<const Amg::MatrixX> locErrMat);

  /// Destructor:
  virtual ~SiCluster();

  /**
   * @name Virtual methods
   */
  //@{
  /// return width class reference
  virtual const InDet::SiWidth& width() const;

  /// return global position reference
  virtual const Amg::Vector3D& globalPosition() const;

  /// set the flag of this cluster containing a gangedPixel
  virtual void setGangedPixel(bool ganged);

  /// return the flag of this cluster containing a gangedPixel
  virtual bool gangedPixel() const;
  /// return the detector element corresponding to this PRD
  /// The pointer will be zero if the det el is not defined (i.e. it was not
  /// passed in by the ctor)
  virtual const InDetDD::SiDetectorElement* detectorElement() const override final;

  /** Interface method checking the type*/
  virtual bool type(Trk::PrepRawDataType::Type type) const override final;

  /// dump information about the SiCluster
  virtual MsgStream& dump(MsgStream& stream) const override;
  /// dump information about the SiCluster
  virtual std::ostream& dump(std::ostream& stream) const override;
 //@}

  /// Set the m_detEl and calculate globalPostion
  //used by TPCnv converters
  void setDetectorElement(const InDetDD::SiDetectorElement* detEl);

private:
  Amg::Vector3D m_globalPosition = Amg::Vector3D::Zero();
  /// col, row, and width in mm
  InDet::SiWidth m_width{};
  ///Not owning points (ownned by the store
  const InDetDD::SiDetectorElement* m_detEl = nullptr;
  bool m_gangedPixel = false;
};

MsgStream&
operator<<(MsgStream& stream, const SiCluster& prd);
std::ostream&
operator<<(std::ostream& stream, const SiCluster& prd);

}

#include "InDetPrepRawData/SiCluster.icc"
#endif // TRKPREPRAWDATA_SICLUSTER_H
