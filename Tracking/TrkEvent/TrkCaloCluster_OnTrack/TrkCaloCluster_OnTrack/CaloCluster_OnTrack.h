/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKCaloCluster_OnTrack_H
#define TRKCaloCluster_OnTrack_H
//
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/SurfaceHolders.h"
//
#include <iosfwd>
#include <memory>
class MsgStream;

namespace Trk {
class EnergyLoss;

/** @class CaloCluster_OnTrack
  Class to handle Cluster On Tracks (ROT) for CaloClusters,
  it inherits from the common MeasurementBase.

  The Track holds a vector of TrackStateOnSurface
  that carry the MeasurmentBase class pointers.
*/

class CaloCluster_OnTrack final
  : public MeasurementBase
  , public SurfacePtrHolder
{
public:
  /** Default Constructor for POOL */
  CaloCluster_OnTrack();
  /** Copy Constructor */
  CaloCluster_OnTrack(const CaloCluster_OnTrack& cot);
  /** Assignment operator */
  CaloCluster_OnTrack& operator=(const CaloCluster_OnTrack& cot);
  /** Move Constructor */
  CaloCluster_OnTrack(CaloCluster_OnTrack&& cot) noexcept = default;
  /** Move Assignment  operator */
  CaloCluster_OnTrack& operator=(CaloCluster_OnTrack&& cot) noexcept = default;

  /** Constructor with parameters  LocalParameters*,  LocalErrorMatrix*,
  Surface& This class owns the LocalParameters, ErrorMatrix & EnergyLoss.  A
  copy of the surface will be made if it is not owned by a detector element. */
  CaloCluster_OnTrack(const LocalParameters& locpars,
                      const Amg::MatrixX& locerr,
                      const Surface& surf,
                      const EnergyLoss* eloss = nullptr);

  /** Destructor */
  virtual ~CaloCluster_OnTrack() = default;

  /** Pseudo-constructor, needed to avoid excessive RTTI*/
  virtual CaloCluster_OnTrack* clone() const override final;

  /**NVI Clone giving up unique pointer */
  std::unique_ptr<CaloCluster_OnTrack> uniqueClone() const
  {
    return std::unique_ptr<CaloCluster_OnTrack>(clone());
  }

  /** returns the surface for the local to global transformation
  - interface from MeasurementBase */
  virtual const Surface& associatedSurface() const override final;

  /** Interface method to get the global Position
  - interface from MeasurementBase */
  virtual const Amg::Vector3D& globalPosition() const override final;

  /** Extended method to get the EnergyLoss */
  const Trk::EnergyLoss* energyLoss() const;

  /** Extended method checking the type*/
  virtual bool type(MeasurementBaseType::Type type) const override final
  {
    return (type == MeasurementBaseType::CaloCluster_OnTrack);
  }

  /**returns the some information about this CaloCluster_OnTrack. */
  virtual MsgStream& dump(MsgStream& out) const override final;

  /**returns the some information about this CaloCluster_OnTrack. */
  virtual std::ostream& dump(std::ostream& out) const override final;

protected:
  /** global position of the cluster hit*/
  Amg::Vector3D m_globalpos;

  /** Energy Loss */
  std::unique_ptr<const Trk::EnergyLoss> m_eloss;

}; // End of Class

} // Namespace

inline const Trk::Surface&
Trk::CaloCluster_OnTrack::associatedSurface() const
{
  return *m_associatedSurface;
}

inline const Trk::EnergyLoss*
Trk::CaloCluster_OnTrack::energyLoss() const
{
  return m_eloss.get();
}

#endif // TRKCaloCluster_OnTrack_H
