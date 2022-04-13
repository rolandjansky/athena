
#ifndef TRKEXTOOLS_ParametersNextVolume_H
#define TRKEXTOOLS_ParametersNextVolume_H

#include "TrkGeometry/TrackingVolume.h" //for BoundarySurfaceFace
#include "ObjContainer.h"
#include "TrkParameters/TrackParameters.h"


namespace Trk{
  class TrackingGeometry;
  
  struct ParametersNextVolume{
    using ManagedTrackParmPtr = ObjPtr<Trk::TrackParameters>;
    using TrackParmContainer = ObjContainer<Trk::TrackParameters>;

    //!< the members
    const TrackingVolume* nextVolume;
    ManagedTrackParmPtr nextParameters;
    ManagedTrackParmPtr navParameters;
    BoundarySurfaceFace exitFace;

    ParametersNextVolume(TrackParmContainer& track_parm_container)
      : nextVolume(nullptr)
      , nextParameters(track_parm_container)
      , navParameters(track_parm_container)
      , exitFace(undefinedFace){
    }

    //!< update the boundaryInformation
    void boundaryInformation(const TrackingVolume* tvol,
                             ManagedTrackParmPtr nextPars,
                             ManagedTrackParmPtr navPars,
                             BoundarySurfaceFace face = undefinedFace){
      nextVolume = tvol;
      nextParameters = std::move(nextPars);
      navParameters = std::move(navPars);
      exitFace = face;
    }
    //!< reset the boundary information by invalidating it
    void resetBoundaryInformation(){
      nextVolume = nullptr;
      exitFace = undefinedFace;
      nextParameters = ManagedTrackParmPtr();
      navParameters = ManagedTrackParmPtr();
    }
  };
}
#endif