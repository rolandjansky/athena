/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @class InDet::InDetAdaptivePriVxFinderTool
 *
 * @author Giacinto Piacquadio (Freiburg University)
 *
 *
 * This class provides an implementation for a primary
 * vertex finding tool, which uses the Adaptive Vertex
 * Fitter to reject outliers not belonging to the primary
 * vertex interaction.
 *
 * The steps done are simply;
 * - Tracks are selected according to the specified cuts
 * - The Adaptive Vertex Finder is used to fit them
 *
 * Contrary to the InDetPriVxFinderTool, the outlier
 * rejection is done by the fitter and not by the finder.
 *
 * One only vertex can be fit, so it is not suited (as a
 * finder) when many minimum bias vertices can be expected.
 * In this case please use the <i>InDetPriVxFinderTool</i>.
 *
 *
 * (this is a modified verson of InDetPriVxFinderTool.h of A. Wildauer & F.
 *Akesson) changes : 06/12/2006   Kirill.Prokofiev@cern.ch EDM cleanup and
 *switching to the FitQuality use
 *
 *      2016-04-26   David Shope <david.richard.shope@cern.ch>
 *      EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex
 *
 *        findVertex will now always return an xAOD::VertexContainer,
 *        even when using a TrackCollection or a TrackParticleBaseCollection
 *        as input.
 *
 ***************************************************************************/

// implemented using as template the InDetPriVxFinderTool class of A. Wildauer
// and F. Akesson

#ifndef INDETPRIVXFINDERTOOL_INDETADAPTIVEPRIVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETADAPTIVEPRIVXFINDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h" // type def ...
#include "TrkTrack/TrackCollection.h"                    // type def ...
/**
 * Forward declarations
 */

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexFwd.h"

namespace Trk {
class IVertexFitter;
class Track;
class TrackParticleBase;
class IVxCandidateXAODVertex;
}

namespace InDet {
class IInDetTrackSelectionTool;

class InDetAdaptivePriVxFinderTool
  : public AthAlgTool
  , virtual public IVertexFinder
{

public:
  /**
   * Constructor
   */

  InDetAdaptivePriVxFinderTool(const std::string& t,
                               const std::string& n,
                               const IInterface* p);

  /**
   * Destructor
   */

  virtual ~InDetAdaptivePriVxFinderTool();

  virtual StatusCode initialize() override;

  /**
   * Finding method.
   * Has as input a track collection and as output
   * a VxContainer.
   */

  using IVertexFinder::findVertex;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(
    const TrackCollection* trackTES) const override;
 
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(
    const xAOD::TrackParticleContainer* trackParticles) const override;

  virtual StatusCode finalize() override;

private:
  /** the common finding code (regardless of Track or TrackParticle(Base) is
   * here */
  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(
    std::vector<const Trk::TrackParameters*>& origParameters) const;

  ToolHandle<Trk::IVertexFitter> m_iVertexFitter;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkFilter;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{
    this,
    "BeamSpotKey",
    "BeamSpotData",
    "SG key for beam spot"
  };

  void SGError(const std::string& errService);

  /**
   * Internal method to print the parameters setting
   */

  virtual void printParameterSettings();

}; // end of class definitions
} // end of namespace definitions
#endif

