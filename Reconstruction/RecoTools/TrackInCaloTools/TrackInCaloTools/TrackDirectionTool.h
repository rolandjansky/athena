/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKDIRECTIONTOOL_H
#define TRACKDIRECTIONTOOL_H
//
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrkParameters/TrackParameters.h" // cannot forward declare because it is a typedef

#include "ITrackInCaloTools/ITrackDirectionTool.h"
#include<vector>

namespace Trk 
{
  class Volume;
  class ITrackingVolumesSvc;
}

/** @class TrackDirectionTool

    Tool used to find out the direction of a track in a cosmic run. 
    For tracks (Trk::Track or Rec::TrackParticle) with TrackParameters in the Inner Detector and the Muon Spectrometer,
    determine if the track points at or away from the perigee

    @author Bruno.Lenzi@cern.ch
*/
class TrackDirectionTool: public AthAlgTool, virtual public ITrackDirectionTool
{
  public:
    TrackDirectionTool (const std::string& type,const std::string& name,const IInterface* parent);
    ~TrackDirectionTool();

    StatusCode initialize();
    StatusCode finalize();
    
    bool getPropagationDirection(const Trk::Track*, Trk::PropDirection&);
    bool getPropagationDirection(const INavigable4Momentum*, Trk::PropDirection&);
    bool getPropagationDirection(std::vector<const Trk::TrackParameters*> trackParameters, Trk::PropDirection &direction);

  private:
    ServiceHandle<Trk::ITrackingVolumesSvc>  m_trackingVolumesSvc;
    Trk::Volume *m_muonspectrometerVolume;
    Trk::Volume *m_calorimeterVolume;
    Trk::Volume *m_indetVolume;
};
#endif //TRACKDIRECTIONTOOL_H
