/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXONTRACK_VERTEXONTRACK_H
#define TRKVERTEXONTRACK_VERTEXONTRACK_H

// Trk
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/SurfaceHolders.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/GaudiException.h"
#include <iosfwd>
#include <optional>

class MsgStream;
class TrackCollectionCnv;

namespace Trk {

class RecVertex;

/** @class VertexOnTrack

  Class to handle Vertex On Tracks,
  it inherits from the common MeasurementBase.

  @author haertel@mppmu.mpg.de
  @author christos anastopoulos (Athena MT modification)

 */

class VertexOnTrack final
  : public MeasurementBase
  , public PerigeeSurfacePtrHolder
{

  friend class ::TrackCollectionCnv;

public:
  /** Default Constructor for POOL */
  VertexOnTrack() = default;

  // default copy move dtor
  VertexOnTrack(const VertexOnTrack&) = default;
  VertexOnTrack(VertexOnTrack&&) noexcept = default;
  VertexOnTrack& operator=(const VertexOnTrack&) = default;
  VertexOnTrack& operator=(VertexOnTrack&&) noexcept = default;
  virtual ~VertexOnTrack() = default;

  /** Constructor with (LocalParameters&, LocalErrorMatrix&, PerigeeSurface&)
  The associated PerigeeSurface is cloned*/
  VertexOnTrack(const LocalParameters& locpars,
                const Amg::MatrixX& locerr,
                const PerigeeSurface& assocSurf);

  VertexOnTrack(const LocalParameters& locpars,
                const Amg::MatrixX& locerr,
                SurfaceUniquePtrT<const PerigeeSurface> assocSurf);

  /** Constructor from: RedVertex, Perigee - the perigee is needed for the
   * measurement frame */
  VertexOnTrack(const Trk::RecVertex& rvertex, const Trk::Perigee& mperigee);


  /** Pseudo-constructor, needed to avoid excessive RTTI*/
  VertexOnTrack* clone() const override final;

  /** NVI clone returning unique_ptr*/
  std::unique_ptr<VertexOnTrack> uniqueClone() const
  {
    return std::unique_ptr<VertexOnTrack>(clone());
  }

  /** returns the surface for the local to global transformation
  - interface from MeasurementBase */
  virtual const PerigeeSurface& associatedSurface() const override final;

  /**Interface method to get the global Position
  - interface from MeasurementBase */
  virtual const Amg::Vector3D& globalPosition() const override final;

  /** Extended method checking the type*/
  virtual bool type(MeasurementBaseType::Type type) const override final
  {
    return (type == MeasurementBaseType::VertexOnTrack);
  }

  /**returns the some information about this VertexOnTrack. */
  virtual MsgStream& dump(MsgStream& out) const override final;
  /**returns the some information about this VertexOnTrack. */
  virtual std::ostream& dump(std::ostream& out) const override final;

  // perhaps return Vertex Object

protected:
  /** Global position of the VoT*/
  std::optional<Amg::Vector3D> m_globalPosition = std::nullopt;
};

inline VertexOnTrack*
VertexOnTrack::clone() const
{
  return new VertexOnTrack(*this);
}

inline const PerigeeSurface&
VertexOnTrack::associatedSurface() const
{
  return *m_associatedSurface;
}

}

#endif // TRKVERTEXONTRACK_VERTEXONTRACK_H

