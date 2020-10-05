/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSCATTERINGANGLESIGNIFICANCETOOL_H
#define MUONSCATTERINGANGLESIGNIFICANCETOOL_H

#include "MuonCombinedToolInterfaces/IMuonScatteringAngleSignificance.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"

namespace Trk {
  class Volume;
}

namespace Rec {

/** @class MuonScatteringAngleSignificanceTool
    @brief Tool to calculate the scattering angle significance from the detailed
           information (parameters, scattering angles) of a track fit.
  */

class MuonScatteringAngleSignificanceTool : public AthAlgTool, virtual public IMuonScatteringAngleSignificance {
  public:
    MuonScatteringAngleSignificanceTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonScatteringAngleSignificanceTool()=default;

    StatusCode initialize();

    /** Calculate ScatteringAngleSignificance of a muon, stepping down to the relevant track */
    ScatteringAngleSignificance scatteringAngleSignificance(const xAOD::Muon& muon) const;

    /** Calculate ScatteringAngleSignificance of a track */
    ScatteringAngleSignificance scatteringAngleSignificance(const Trk::Track& track) const;

  private:
    /** does track have TrackParameters at every TSOS ? Method for
        compatibility with release < 17, where SlimmedTrack property
        isn't filled.  */
    bool isSlimmed(const Trk::Track& track) const;

    // tools and services
    ToolHandle<Trk::ITrackFitter> m_fitter{this,"TrackFitter","","tool for unslimming via track fit"};                                                             //!< tool for unslimming via track fit
    ServiceHandle<Trk::ITrackingVolumesSvc> m_trackingVolumesSvc{this,"TrackingVolumesSvc","TrackingVolumesSvc","geometry for analysing track lengths"};

    // constants
    const Trk::Volume* m_calorimeterVolume;  //!< cache the calo volume pointer
    const Trk::Volume* m_indetVolume;        //!< cache the ID volume pointer

    // steering flags
    bool m_inDetOnly;       //!< scatterers from ID only (or ID + calo)
    bool m_refitInDetOnly;  //!< steer if to unslim only ID
};

}  // namespace Rec

#endif
