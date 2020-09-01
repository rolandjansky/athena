/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTrackSelectorTool_InDetConversionTrackSelectorTool_H
#define InDetTrackSelectorTool_InDetConversionTrackSelectorTool_H

#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "xAODTracking/TrackParticle.h"
/**
 * A tool to be used for track preselection in conversion
 * vertex finding.
 *
 * Thomas Koffas <Thomas.Koffas@cern.ch>
 * June 2008
 */

namespace Trk {
class Vertex;
class Track;
class TrackParticleBase;
}

namespace InDet {

class InDetConversionTrackSelectorTool
  : virtual public Trk::ITrackSelectorTool
  , public AthAlgTool
{

public:
  virtual StatusCode initialize() override;

  InDetConversionTrackSelectorTool(const std::string& t,
                                   const std::string& n,
                                   const IInterface* p);

  virtual ~InDetConversionTrackSelectorTool() =default;

  /** Select a Trk::Track  */
  virtual bool decision(const Trk::Track& track,
                        const Trk::Vertex* vertex) const override final;

  /** Select a Trk::TrackParticleBase  */
  virtual bool decision(const Trk::TrackParticleBase& track,
                        const Trk::Vertex* vertex) const override final;

  virtual bool decision(const xAOD::TrackParticle&,
                        const xAOD::Vertex*) const override final;

private:
  int getCount(const xAOD::TrackParticle& tp, xAOD::SummaryType type) const
  {
    uint8_t val;
    if (!tp.summaryValue(val, type))
      return 0;
    return val > 0 ? val : 0;
  }

  Amg::Vector3D getPosOrBeamSpot(const xAOD::Vertex*) const;

  ToolHandle<Trk::IExtrapolator> m_extrapolator{ this,
                                                 "Extrapolator",
                                                 "",
                                                 "Extrapolator Tool" };
  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{
    this,
    "BeamSpotKey",
    "BeamSpotData",
    "SG key for beam spot"
  };
  /** Properties for track selection:all cuts are ANDed */
  double m_maxSiD0;    //!< Maximal d0 at (0,0,0) for tracks with Si hits
  double m_maxTrtD0;   //!< Maximal d0 at (0,0,0) for standalone TRT tracks
  double m_maxSiZ0;    //!< Maximal z0 at (0,0,0)
  double m_maxTrtZ0;   //!< Maximal z0 at (0,0,0) for standalone TRT tracks
  double m_minPt;      //!< Minimum Pt of tracks
  double m_trRatio1;   //!< TR ratio for tracks with 15-20 TRT hits
  double m_trRatio2;   //!< TR ratio for tracks with 20-25 TRT hits
  double m_trRatio3;   //!< TR ratio for tracks with >25 TRT hits
  double m_trRatioTRT; //!< TR ratio for TRT only tracks
  double m_trRatioV0;  //!< TR ratio for pion selection during V0 reconstruction
  double m_sD0_Si;     //!< Cut on D0 significance of Si tracks
  double m_sD0_Trt;    //!< Cut on D0 significance of TRT tracks
  double m_sZ0_Trt;    //!< Cut on Z0 significance of TRT tracks
  bool m_isConv;       //!< Conversion flag
  bool m_PIDonlyForXe; //!< Only check TRT PID if all hits are Xe hits

}; // end of class definitions
} // end of namespace definitions

#endif
