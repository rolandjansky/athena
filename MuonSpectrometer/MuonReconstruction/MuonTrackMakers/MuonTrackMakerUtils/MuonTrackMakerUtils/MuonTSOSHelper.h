/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"


namespace Muon {

  class MuonTSOSHelper {
  public:
    
    /** clone input, update the type */
    static Trk::TrackStateOnSurface* cloneTSOS( const Trk::TrackStateOnSurface& tsos, Trk::TrackStateOnSurface::TrackStateOnSurfaceType type ){

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = tsos.types();  
      if( type == Trk::TrackStateOnSurface::Outlier ) typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
      else if( type == Trk::TrackStateOnSurface::Measurement ) typePattern.set(Trk::TrackStateOnSurface::Outlier,false);       		
      typePattern.set(type);       		
	  
      return new Trk::TrackStateOnSurface( tsos.measurementOnTrack() ? tsos.measurementOnTrack()->clone() : 0,
					   tsos.trackParameters() ? tsos.trackParameters()->clone() : 0,
					   tsos.fitQualityOnSurface() ? tsos.fitQualityOnSurface()->clone() : 0,
					   tsos.materialEffectsOnTrack() ? tsos.materialEffectsOnTrack()->clone() : 0,
					   typePattern );
    }

    /** clone input, replacing the track parameteres and the measurement base and updating the type */
    static Trk::TrackStateOnSurface* cloneTSOSWithUpdate( const Trk::TrackStateOnSurface& tsos, 
							  const Trk::MeasurementBase& meas,
							  const Trk::TrackParameters& pars,
							  Trk::TrackStateOnSurface::TrackStateOnSurfaceType type ){

      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = tsos.types();  
      if( type == Trk::TrackStateOnSurface::Outlier ) typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
      else if( type == Trk::TrackStateOnSurface::Measurement ) typePattern.set(Trk::TrackStateOnSurface::Outlier,false);       		
      typePattern.set(type);       		
	  
      return new Trk::TrackStateOnSurface( meas.clone(),
					   pars.clone(),
					   tsos.fitQualityOnSurface() ? tsos.fitQualityOnSurface()->clone() : 0,
					   tsos.materialEffectsOnTrack() ? tsos.materialEffectsOnTrack()->clone() : 0,
					   typePattern );
    }

    /** create a perigee TSOS, takes ownership of the Perigee */
    static Trk::TrackStateOnSurface* createPerigeeTSOS( const Trk::TrackParameters* perigee ){
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Perigee);       		
      return new Trk::TrackStateOnSurface(0,perigee,0,0,typePattern);
    }

    /** create a TSOS with a measurement, takes ownership of the pointers */
    static Trk::TrackStateOnSurface* createMeasTSOS( const Trk::MeasurementBase* meas, 
						     const Trk::TrackParameters* pars,
						     Trk::TrackStateOnSurface::TrackStateOnSurfaceType type ){
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      if( type == Trk::TrackStateOnSurface::Outlier ) typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
      typePattern.set(type);       		
      return new Trk::TrackStateOnSurface(meas,pars,0,0,typePattern);
    }

   /** create a TSOS with a measurement, takes ownership of the pointers */
    static Trk::TrackStateOnSurface* createMeasTSOSWithUpdate( const Trk::TrackStateOnSurface& tsos, 
							       const Trk::MeasurementBase* meas, 
							       const Trk::TrackParameters* pars,
							       Trk::TrackStateOnSurface::TrackStateOnSurfaceType type ){
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern = tsos.types();  
      if( type == Trk::TrackStateOnSurface::Outlier ) typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
      else if( type == Trk::TrackStateOnSurface::Measurement ) typePattern.set(Trk::TrackStateOnSurface::Outlier,false);       		
      typePattern.set(type);       		
      return new Trk::TrackStateOnSurface( meas,
					   pars,
					   tsos.fitQualityOnSurface() ? tsos.fitQualityOnSurface()->clone() : 0,
					   tsos.materialEffectsOnTrack() ? tsos.materialEffectsOnTrack()->clone() : 0,
					   typePattern );
    }

    /** create a hole TSOS, takes ownership of the pointers */
    static Trk::TrackStateOnSurface* createHoleTSOS( const Trk::TrackParameters* pars ){
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      typePattern.set(Trk::TrackStateOnSurface::Hole);       		
      return new Trk::TrackStateOnSurface(0,pars,0,0,typePattern);
    }

  };
}
