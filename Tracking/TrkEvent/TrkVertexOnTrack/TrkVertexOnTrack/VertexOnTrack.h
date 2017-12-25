/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VertexOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVERTEXONTRACK_VERTEXONTRACK_H
#define TRKVERTEXONTRACK_VERTEXONTRACK_H

// Trk
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include <ostream>

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"
#include "TrkParameters/TrackParameters.h"

#include "GaudiKernel/GaudiException.h"

class MsgStream;
class TrackCollectionCnv;

namespace Trk{

  class RecVertex;

/** @class VertexOnTrack

  Class to handle Vertex On Tracks,
  it inherits from the common MeasurementBase. 

  @author haertel@mppmu.mpg.de

 */

  class VertexOnTrack : public MeasurementBase {

    friend class ::TrackCollectionCnv;

    public:
      /** Default Constructor for POOL */
      VertexOnTrack();
      /** Copy Constructor */
      VertexOnTrack(const VertexOnTrack& vot);
      /** Assignment operator */
      VertexOnTrack& operator=(const VertexOnTrack& vot);

      /** Constructor with (LocalParameters&, LocalErrorMatrix&, PerigeeSurface&)
      The associated PerigeeSurface is cloned*/
      VertexOnTrack( const LocalParameters& locpars,
                     const Amg::MatrixX& locerr,
                     const PerigeeSurface& assocSurf);

      VertexOnTrack( const LocalParameters& locpars,
                     const Amg::MatrixX& locerr,
                     SurfaceUniquePtrT<const PerigeeSurface> assocSurf);

      /** Constructor from: RedVertex, Perigee - the perigee is needed for the measurement frame */
      VertexOnTrack( const Trk::RecVertex& rvertex, const Trk::Perigee& mperigee);

      /** Destructor */
      virtual ~VertexOnTrack();

      /** Pseudo-constructor, needed to avoid excessive RTTI*/
      VertexOnTrack* clone() const;

      /** returns the surface for the local to global transformation 
      - interface from MeasurementBase */
      const PerigeeSurface& associatedSurface() const;

      /**Interface method to get the global Position
      - interface from MeasurementBase */
      const Amg::Vector3D& globalPosition() const;

      /**returns the some information about this VertexOnTrack. */
      virtual MsgStream&    dump( MsgStream& out ) const;
      /**returns the some information about this VertexOnTrack. */
      virtual std::ostream& dump( std::ostream& out ) const;

     // perhaps return Vertex Object

    protected:
      /** Perigee surface of the VoT*/
      mutable const PerigeeSurface* m_associatedSurface;

      /** Global position of the VoT*/
      mutable const Amg::Vector3D*  m_globalPosition;
  };

  inline VertexOnTrack* VertexOnTrack::clone() const 
  { return new VertexOnTrack(*this); }

  inline const PerigeeSurface& VertexOnTrack::associatedSurface() const
  { return *m_associatedSurface; }

  inline const Amg::Vector3D& VertexOnTrack::globalPosition() const
  {
    if (m_globalPosition == 0) {m_globalPosition = new Amg::Vector3D();}
    return *m_globalPosition;
  }

}

#endif // TRKVERTEXONTRACK_VERTEXONTRACK_H

