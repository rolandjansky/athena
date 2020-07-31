/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETCONVERSIONFINDERTOOLS_SINGLETRACKCONVERSION_H
#define INDETCONVERSIONFINDERTOOLS_SINGLETRACKCONVERSION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"

namespace Trk {
class Track;
class TrackParticleBase;
class ITrackSummaryTool;
class IExtrapolator;
}

namespace InDet {
class ConversionFinderUtils;

/**
   @class SingleTrackConversionTool
   Helper tools to reconstruct single track conversions
   @author Tatjana Lenz , Thomas Koffas
*/

class SingleTrackConversionTool : public AthAlgTool
{

public:
  SingleTrackConversionTool(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);
  virtual ~SingleTrackConversionTool();

  static const InterfaceID& interfaceID();
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  /** Build single track conversion candidate. Trk::Track interface.  */
  xAOD::Vertex* buildSingleTrackConversion(const Trk::Track* track) const;
  /** Select single track conversion candidates.  Trk::Track interface. */
  bool selectSingleTrackConversion(const Trk::Track* track) const;
  /** Build single track conversion candidate. xAOD::TrackParticle interface. */
  xAOD::Vertex* buildSingleTrackParticleConversion(
    const xAOD::TrackParticle*,
    xAOD::VertexContainer* container) const;
  /** Select single track conversion candidates.  xAOD::TrackParticle. */
  bool selectSingleTrackParticleConversion(const xAOD::TrackParticle*) const;

protected:
  /** Conversion helper tool. */
  ToolHandle<InDet::ConversionFinderUtils> m_helpertool{
    this,
    "ConversionFinderHelperTool",
    "InDet::ConversionFinderUtils",
    "Helper for conversion finding"
  };
  //!< Track extrapolator tool.
  ToolHandle<Trk::IExtrapolator> m_extrapolator{ this,
                                                 "Extrapolator",
                                                 "",
                                                 "Track Extrapolator Tool" };
  double m_minInitR; /** Minimum initial hit radius in order to consider track
                        as coming from photon conversion */
  double m_minInitR_noBLay; /** Minimum initial hit radius in order to consider
                               track as coming from photon conversion */
  double m_singleThreshold; /** Minimum ratio of HL hits for a track to be
                               labeled as electron */
  int m_maxBLhits;          /** Maximum number of blayer hits */
  // double m_maxPhiVtxTrk;    /** maximum difference in pointing */
  bool m_PIDonlyForXe; /** Only check TRT PID if all hits are Xe hits */
};

}
#endif // INDETCONVERSIONFINDERTOOLS_SINGLETRACKCONVERSION_H

