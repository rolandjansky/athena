/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuidMaterialEffectsOnTrackProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MuidMaterialEffectsOnTrackProvider_H
#define MuidMaterialEffectsOnTrackProvider_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkDetDescrInterfaces/IMaterialEffectsOnTrackProvider.h"
#include "TrkSurfaces/PlaneSurface.h"

#include "TrkParameters/TrackParameters.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"

#include "GaudiKernel/ToolHandle.h"

namespace Trk {
    class Surface;
}

  /** @class MuidMaterialEffectsOnTrackProvider
      
  @author thijs.cornelissen@cern.ch
  */
namespace Rec{  
  class MuidMaterialEffectsOnTrackProvider : public AthAlgTool,
    virtual public Trk::IMaterialEffectsOnTrackProvider {
      
  public:
      
      /** AlgTool like constructor */
      MuidMaterialEffectsOnTrackProvider(const std::string&,const std::string&,const IInterface*);
      
      /**Virtual destructor*/
      virtual ~MuidMaterialEffectsOnTrackProvider();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** return all MaterialLayers associated to this track **/
      std::vector< Trk::MaterialEffectsOnTrack > extrapolationSurfacesAndEffects(const Trk::TrackingVolume& ,
														  const Trk::IPropagator& ,
														  const Trk::TrackParameters& ,
														  const Trk::Surface& ,
														  Trk::PropDirection ,
									
				  Trk::ParticleHypothesis ) const;
  private:      
      
     ToolHandle< Rec::IMuidCaloTrackStateOnSurface > m_calotsos;
     ToolHandle< Rec::IMuidCaloTrackStateOnSurface > m_calotsosparam;
     ToolHandle< Trk::IMultipleScatteringUpdator > m_scattool;
     
     
     bool m_cosmics;
    };
  } // end namespace

#endif // MuidMaterialEffectsOnTrackProvider_H
