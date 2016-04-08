/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FieldIntegralByTrackQueryTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKFIELDINTEGRALBYTRACKQUERYTOOL_H
#define TRKFIELDINTEGRALBYTRACKQUERYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkToolInterfaces/ITrackMagneticFieldIntegrator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"

//class ITrackingVolumesSvc;

namespace Trk 
{
  class Volume;
  class TrackStateOnSurface;

  /** @class FieldIntegralByTrackQueryTool

    @brief tool implementing ITrackMagneticFieldIntegrator using a technique that sums
           up the observed bending along the trajectory.

           For that it needs the unslimmed track and parameters at measurement TSoS.
           Separate interfaces return the ID solenoid or MS toroid integrated field.
           For the MS toroid field integral the start or end points with measurement
           constraints in the non-bending plane are excluded, leading to small field
           integral if a track has no Inner Station precision hits.

           Further work could foresee a refit in case slimmed tracks are met or a
           (then separate implementation of this tool) version that works with the
           propagators and polls the magnetic field service to build the field integral.

    @author Alan Poppleton, Wolfgang Liebig
  */
  class FieldIntegralByTrackQueryTool : virtual public ITrackMagneticFieldIntegrator, public AthAlgTool
    {
    public:
      FieldIntegralByTrackQueryTool(const std::string&,const std::string&,const IInterface*);
      
      /** default destructor */
      virtual ~FieldIntegralByTrackQueryTool ();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode initialize();
      
      /** standard Athena-Algorithm method */
      virtual StatusCode finalize  ();
      
  /** this method skims through the unslimmed indet track for deflections to the track
      parameter direction, subtracking scattering angles */
      virtual double indetFieldIntegral( const Track& track ) const;

  /** this method skims through the unslimmed muon spectro track for deflections to the track
      parameter direction, subtracking scattering angles */
      virtual double spectrometerFieldIntegral( const Track& track) const;
      
    private:
      
      double fieldIntegral(const Track& track, bool takeMSNotIndet) const;

      ServiceHandle<ITrackingVolumesSvc>              m_trackingVolumesSvc;

      const Trk::Volume*                              m_indetVolume;
      const Trk::Volume*                              m_caloVolume;

    }; 
} // end of namespace

#endif 
