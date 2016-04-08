/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************************
            MultiComponentStateOnSurface.h  -  description
            -----------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson, amorley
email                : Anthony.Morley@cern.ch (adapted from Edward Moyse)
description          : This class is a multi component adaption of the class
               TrackStateOnSurface. In that class the track state was
               represented by a single 5 component track paramter
               vector (a0, z0, phi0, theta0, q/p) and the associated
               covariance matrix. In its multi-component form the track
               state on surface is represented by many track parameters
               each with a covariance matrix and additionally a
               weighting is attached to each component which reflects
               the importance of that particular component in the
               overall mixture of components which is used to describe
               the track state at that surface.
*******************************************************************************/

#ifndef TrkMultiComponentStateOnSurface_H
#define TrkMultiComponentStateOnSurface_H

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h" //typedef
#include "TrkParameters/TrackParameters.h"
#include <iostream>

class MsgStream;

namespace Trk {

class MultiComponentState;
class MaterialEffectsBase;
class MeasurementBase;

class MultiComponentStateOnSurface : public TrackStateOnSurface
{
 
 public:
    
  /*-----------------------------------------------------------------
    BE CAREFUL: The objects passed in belong to the this object. Never
    delete these objects yourself
    -----------------------------------------------------------------*/
  
  /** Default constructor for POOL. This should not be used! */
  MultiComponentStateOnSurface();

  /** Create a MultiComponentStateOnSurface Object. This has the same form as the singular version (Trk::TrackStateOnSurface) with the exception that the pointer to
      a single track paramters vector is now repleaced with a pointer to a multi-component state */
  MultiComponentStateOnSurface( const MeasurementBase*,
				const MultiComponentState*,
				const FitQualityOnSurface*,
				const MaterialEffectsBase* materialEffectsOnTrack = 0,
				double modeQoverP = 0.
 				);
	
  /** Create a MultiComponentStateOnSurface Object with an explicit declaration of the track parameters to be passed to the Trk::TrackStateOnSurface base class */
  MultiComponentStateOnSurface( const MeasurementBase*,
				const TrackParameters*,
				const MultiComponentState*,
				const FitQualityOnSurface*,
				const MaterialEffectsBase* materialEffectsOnTrack = 0,
				double modeQoverP = 0.
				);
	
  /** Create TrackStateOnSurface with TrackStateOnSurfaceType. */
  MultiComponentStateOnSurface( const MeasurementBase*,
				const MultiComponentState*,
				const FitQualityOnSurface*,
				const MaterialEffectsBase*,
				const std :: bitset<NumberOfTrackStateOnSurfaceTypes>&,
				double modeQoverP = 0.
        );
	
  /** Create a MultiComponentStateOnSurface Object with an explicit declaration o the track parameters to be passed to the base and also a TrackStateOnSurfaceType */
  MultiComponentStateOnSurface( const MeasurementBase*,
				const TrackParameters*,
				const MultiComponentState*,
				const FitQualityOnSurface*,
				const MaterialEffectsBase*,
				const std :: bitset<NumberOfTrackStateOnSurfaceTypes>& types,
				double modeQoverP = 0.
        );
	
  /** Constructor without a FitQualityOnSurface. */
  MultiComponentStateOnSurface( const MeasurementBase*,
                const MultiComponentState* );
    
  /** Copy constructor */
  MultiComponentStateOnSurface( const MultiComponentStateOnSurface& );

  /** Deleted assignment */
  MultiComponentStateOnSurface & operator=( const MultiComponentStateOnSurface& ) = delete;
  
  /** Virtual destructor */
  virtual ~MultiComponentStateOnSurface();

  /** Clone method for deep copy of MultiComponentStateOnSurface - overidden from base class */
  virtual TrackStateOnSurface* clone() const;

  /** Method to return a pointer to the multi-component state */
  const MultiComponentState* components() const;

  /** Method to return the mode of the multi-component state */
  double mixtureModeQoverP() const;
    
private:
  const MultiComponentState*         m_multiComponentState;
  double                             m_mixtureModeQoverP;
};

/** Overload of << operator for MsgStream for debug output */ 
MsgStream& operator << ( MsgStream&, const MultiComponentStateOnSurface& );

/** Overload of << operator for std::ostream for debug output */ 
std::ostream& operator << ( std::ostream&, const MultiComponentStateOnSurface& );


} //end of Trk namespace

inline const Trk::MultiComponentState* Trk::MultiComponentStateOnSurface::components() const
{ return m_multiComponentState; }

inline double Trk::MultiComponentStateOnSurface::mixtureModeQoverP() const
{ return m_mixtureModeQoverP; }

#endif
