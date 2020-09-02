/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 *
 * @class InDet::InDetIterativePriVxFinderTool
 *
 * @author Giacinto Piacquadio (Freiburg University)
 *
 *
 * This class provides an implementation for a primary
 * vertex finding tool, which uses the Adaptive Vertex
 * Fitter to reject outliers not belonging to the primary
 * vertex interaction.
 *
 * ------------------------------------------------------
 * Changes:
 *
 * David Shope <david.richard.shope@cern.ch> (2016-04-26)
 *
 * EDM Migration to xAOD - from Trk::VxCandidate to xAOD::Vertex
 *
 *   findVertex will now always return an xAOD::VertexContainer,
 *   even when using a TrackCollection or a TrackParticleBaseCollection
 *   as input.
 *
 * Dave Casper <dcasper@uci.edu> (2018-03-28)
 *   Migrate beam spot service to conditions handle
 *
 ***************************************************************************/

// implemented using as template the InDetPriVxFinderTool class of A. Wildauer
// and F. Akesson

#ifndef INDETPRIVXFINDERTOOL_INDETITERATIVEPRIVXFINDERTOOL_H
#define INDETPRIVXFINDERTOOL_INDETITERATIVEPRIVXFINDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "GaudiKernel/ToolHandle.h"
#include "InDetRecToolInterfaces/IVertexFinder.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/TrackCollection.h"                    // type def ...
/**
 * Forward declarations
 */

#include "xAODTracking/TrackParticleContainerFwd.h"
#include "xAODTracking/TrackParticleFwd.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTracking/VertexFwd.h"

namespace Trk {
class IVertexFitter;
class Track;
class ITrackLink;
class IVertexSeedFinder;
class IImpactPoint3dEstimator;
// class IVertexTrackCompatibilityEstimator;
// class ImpactPoint3dAtaPlaneFactory;
class IVertexLinearizedTrackFactory;
//  class ITrkDistanceFinder;

class IVxCandidateXAODVertex;
}

namespace InDet {
class IInDetTrackSelectionTool;

class InDetIterativePriVxFinderTool
  : public AthAlgTool
  , virtual public IVertexFinder
{

public:
  /**
   * Constructor
   */

  InDetIterativePriVxFinderTool(const std::string& t,
                                const std::string& n,
                                const IInterface* p);

  /**
   * Destructor
   */

  virtual ~InDetIterativePriVxFinderTool();

  virtual StatusCode initialize() override;

  /**
   * Finding method.
   * Has as input a track collection and as output
   * a VxContainer.
   */
  using IVertexFinder::findVertex;
  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx,
             const TrackCollection* trackTES) const override;

  virtual std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*>
  findVertex(const EventContext& ctx,
             const xAOD::TrackParticleContainer* trackParticles) const override;

  virtual StatusCode finalize() override;

private:
  std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> findVertex(
    const EventContext& ctx,
    const std::vector<Trk::ITrackLink*>& trackVector) const;

  void removeCompatibleTracks(
    xAOD::Vertex* myxAODVertex,
    std::vector<const Trk::TrackParameters*>& perigeesToFit,
    std::vector<Trk::ITrackLink*>& seedTracks) const;

  void removeAllFrom(std::vector<const Trk::TrackParameters*>& perigeesToFit,
                     std::vector<Trk::ITrackLink*>& seedTracks) const;

  double compatibility(const Trk::TrackParameters& measPerigee,
                       const xAOD::Vertex& vertex) const;

  void countTracksAndNdf(xAOD::Vertex* myxAODVertex,
                         double& ndf,
                         int& ntracks) const;

  ToolHandle<Trk::IVertexFitter> m_iVertexFitter;
  ToolHandle<InDet::IInDetTrackSelectionTool> m_trkFilter;
  ToolHandle<Trk::IVertexSeedFinder> m_SeedFinder;
  ToolHandle<Trk::IImpactPoint3dEstimator> m_ImpactPoint3dEstimator;
  ToolHandle<Trk::IVertexLinearizedTrackFactory> m_LinearizedTrackFactory;

  SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{
    this,
    "BeamSpotKey",
    "BeamSpotData",
    "SG key for beam spot"
  };

  bool m_useBeamConstraint;
  double m_significanceCutSeeding;
  double m_maximumChi2cutForSeeding;
  double m_maxVertices;

  bool m_createSplitVertices;
  int m_splitVerticesTrkInvFraction; ///< Integer: 1./fraction of tracks to be
                                     ///< assigned to the tag split vertex

  bool m_reassignTracksAfterFirstFit;

  bool m_doMaxTracksCut;
  unsigned int m_maxTracks;

  void SGError(const std::string& errService);

  /**
   * Internal method to print the parameters setting
   */

  virtual void printParameterSettings();

}; // end of class definitions
} // end of namespace definitions
#endif

