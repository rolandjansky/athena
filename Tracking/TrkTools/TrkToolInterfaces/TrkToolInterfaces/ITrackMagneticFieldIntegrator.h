/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRACKMAGNETICFIELDINTEGRATOR_H
#define ITRACKMAGNETICFIELDINTEGRATOR_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITrackMagneticFieldIntegrator("Trk::ITrackMagneticFieldIntegrator", 1, 0);

namespace Trk {

  class Track;

/** @brief Interface for tool to return a field integral from a given track.

    @author Wolfgang Liebig (at) cern.ch */
  class ITrackMagneticFieldIntegrator : virtual public IAlgTool
  {
    public:

      static const InterfaceID& interfaceID( ) ;
  /** calculate a field integral along the inner detector part of a track,
      in case of ID tracks or full atlas muon tracks. Returns 0.0 if no ID hits.
      @param track the input track for trajectory
      @return the returned field integral */
      virtual double indetFieldIntegral( const Track& track ) const = 0;

  /** calculate a field integral along the muon spectrometer part of a track,
      in case of MS tracks or full atlas muon tracks. Returns 0.0 if no MS hits.
      @param track the input track for trajectory
      @return the returned field integral */
      virtual double spectrometerFieldIntegral( const Track& track) const = 0;
  };

}//end of namespace

inline const InterfaceID& Trk::ITrackMagneticFieldIntegrator::interfaceID()
{
  return IID_ITrackMagneticFieldIntegrator;
}

#endif 
