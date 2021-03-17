/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PseudoMeasurementOnTrack.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKPMONTRACK_PSEUDOMEASUREMENTONTRACK_H
#define TRKPMONTRACK_PSEUDOMEASUREMENTONTRACK_H

#include "TrkMeasurementBase/MeasurementBase.h"
//#include "TrkEventPrimitives/GlobalPosition.h"
#include "EventPrimitives/EventPrimitives.h"  
#include "GeoPrimitives/GeoPrimitives.h" 
#include "TrkSurfaces/Surface.h"
#include <ostream>

class MsgStream;
class TrackCollectionCnv;

namespace Trk{

  class Surface;

/**
 @brief Class to handle pseudo-measurements in fitters and on track objects.

 Such virtual measurements are typically used to feed crude values into
 track fitters for otherwise unconstraint fit parameters.
 Inherits from the common Trk::MeasurementBase but NOT from Trk::RIO_OnTrack
 since it is not necessarily connected to a "real" identifier, PRD
 or detectorElement. Through the Trk::MeasurementBase inheritance this
 class is stored and persistified on ESD.

 @author Common Tracking SW Group

 */

  class PseudoMeasurementOnTrack final: public MeasurementBase {

    friend class ::TrackCollectionCnv;

    public:
      //! Default Constructor for POOL 
      PseudoMeasurementOnTrack();
      //! Copy Constructor 
      PseudoMeasurementOnTrack(const PseudoMeasurementOnTrack& pmot);
      //! Assignment operator 
      PseudoMeasurementOnTrack& operator=(const PseudoMeasurementOnTrack& pmot);

      //! Constructor with (LocalParameters*, LocalErrorMatrix*, Surface&) Note: the associated surface is cloned 
      PseudoMeasurementOnTrack( const LocalParameters& locpars,
                                const Amg::MatrixX&     locerr,
                                const Surface&         assocSurf);

      PseudoMeasurementOnTrack( const LocalParameters& locpars,
                                const Amg::MatrixX&     locerr,
                                ConstSurfaceUniquePtr   assocSurf);

      //! Destructor 
      virtual ~PseudoMeasurementOnTrack() override final;

      //! virtual constructor, not absolutely needed but given for EDM symmetry 
      virtual PseudoMeasurementOnTrack* clone() const override final;

      //! move constructor
      PseudoMeasurementOnTrack(PseudoMeasurementOnTrack&& pmot) noexcept;

      //! move assignment operator
      PseudoMeasurementOnTrack& operator=(PseudoMeasurementOnTrack&& pmot) noexcept;   

      //! returns the surface for the local to global transformation (interface from MeasurementBase)
      virtual const Surface& associatedSurface() const override final;

      //! Test to see if an associated surface exists.
      bool hasSurface() const;

      //! returns the global Position (interface from MeasurementBase)
      virtual const Amg::Vector3D& globalPosition() const override final;

      /** Extended method checking the type*/
       virtual bool type(MeasurementBaseType::Type type) const override final{
         return (type==MeasurementBaseType::PseudoMeasurementOnTrack);
       }

      //! produces logfile output about its content in MsgStream form. 
      virtual MsgStream&    dump( MsgStream& out ) const override final;
      //! produces logfile output about its content in stdout form. 
      virtual std::ostream& dump( std::ostream& out ) const override final;

    protected:
      //! holds the surface to which the PMoT is associated. The surface is
      //! responsible for the correct local-to-global transformation.
      const Surface* m_associatedSurface;

      //! Global position of the PMoT
      const Amg::Vector3D* m_globalPosition;
  };

  inline PseudoMeasurementOnTrack* PseudoMeasurementOnTrack::clone() const 
  { return new PseudoMeasurementOnTrack(*this); }

  inline const Surface& PseudoMeasurementOnTrack::associatedSurface() const
  { return *m_associatedSurface; }

  inline
  bool PseudoMeasurementOnTrack::hasSurface() const
  { return m_associatedSurface != nullptr; }
}

#endif // TRKPMONTRACK_PSEUDOMEASUREMENTONTRACK_H

