/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEFMISSINGET_APPROXIMATETRACKTOLAYERTOOL_H
#define TRIGEFMISSINGET_APPROXIMATETRACKTOLAYERTOOL_H

#include "TrigEFMissingET/IExtendTrackToLayerTool.h"
#include "AsgTools/AsgTool.h"

class ApproximateTrackToLayerTool : public virtual IExtendTrackToLayerTool, public asg::AsgTool
{
public:
  ASG_TOOL_CLASS(ApproximateTrackToLayerTool, IExtendTrackToLayerTool)

  ApproximateTrackToLayerTool(const std::string &name);

  virtual TrackExtension extendTrack(
      const EventContext &,
      const xAOD::TrackParticle &track) const override;

private:
  // Extrapolation approximation taken from ATL-COM-PHYS-2016-430
  Gaudi::Property<double> m_trackExtrapolationQuartic{
      this, "TrackExtrapolationQuarticTerm", 14.6027571,
      "The quartic term in the track extrapolation"};
  Gaudi::Property<double> m_trackExtrapolationQuadratic{
      this, "TrackExtrapolationQuadraticTerm", -44.7818374,
      "The quadratic term in the track extrapolation"};
  Gaudi::Property<double> m_trackExtrapolationLinear{
      this, "TrackExtrapolationQuarticTerm", 540.656643,
      "The linear term in the track extrapolation"};
}; //> end class ApproximateTrackToLayerTool

#endif //> !TRIGEFMISSINGET_APPROXIMATETRACKTOLAYERTOOL_H
