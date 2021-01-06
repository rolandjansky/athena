/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetTrackSelectorTool_InDetTrackSelectorTool_H
#define InDetTrackSelectorTool_InDetTrackSelectorTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"//enum, do not fwd declare
#include "TrkParameters/TrackParameters.h" //typedef

/**
 * @file InDetTrackSelectorTool.h
 * @class InDetTrackSelectorTool
 *
 * @brief A tool to be used for track preselection in primary
 * vertex finding. One possible implementation of the
 * track selector tool. Here all possible cuts used in the
 * primary vertex finding will be implemented. The particular
 * cuts will be turned on and off by user through the python
 * jobOptions.
 *
 * @author Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
 * @date Mai 2007
 *
 * @author Daniel Kollar <daniel.kollar@cern.ch>
 */

namespace Trk
{
  class ITrackSummaryTool;
  class Vertex;
  class IExtrapolator;
  class Track;
  class TrackParticleBase;
}

namespace InDet
{
  class InDetTrackSelectorTool : virtual public Trk::ITrackSelectorTool, public AthAlgTool
  {

    public:

      virtual StatusCode initialize() override;

      InDetTrackSelectorTool(const std::string& t, const std::string& n, const IInterface*  p);

      ~InDetTrackSelectorTool();

      virtual bool decision(const Trk::Track& track,const Trk::Vertex* vertex) const override;

      virtual bool decision(const Trk::TrackParticleBase& track,const Trk::Vertex* vertex) const override;

      virtual bool decision(const xAOD::TrackParticle&,const xAOD::Vertex*) const override {
	ATH_MSG_WARNING("xAOD::TrackParticle selection not implemented yet");
	return false;
      }

    private:

      bool decision(const Trk::TrackParameters* track, const Trk::Vertex* vertex, const Trk::ParticleHypothesis) const;

      double m_minPt; //!< Minimum Pt of tracks
      double m_IPz0Max; //!< max. z0: |z0*sin(theta)| < z0Max
      double m_maxZ0; //!< Maximum z0 of tracks
      double m_maxD0; //!< Maximum d0 of tracks
      double m_maxD0overSigmaD0; //!< Maximum d0/sigmad0 of tracks
      int  m_numberOfPixelHits; //!< Check for silicon hits ?
      int  m_numberOfBLayerHits;
      ToolHandle<Trk::ITrackSummaryTool> m_trackSumTool; //<! Track summary tool
      bool m_trackSumToolAvailable;

      ToolHandle<Trk::IExtrapolator> m_extrapolator; //<! Extrapolator tool

  }; //end of class definitions
} //end of namespace definitions

#endif //TrkMultipleVertexSeedFinders_PVFindingTrackSelectoTool_H
