/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRK_CALOEXTENSIONBUILDERALG_H
#define TRK_CALOEXTENSIONBUILDERALG_H
/**
  @class CaloExtensionBuilderAlg
  Based on EMGSFCaloExtensionBuilder
  Algorithm which creates calo extension for all
  Track Particles
  */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "RecoToolInterfaces/IParticleCaloExtensionTool.h"
#include "TrkCaloExtension/CaloExtensionCollection.h"

#include "ITrackToVertex/ITrackToVertex.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrkToolInterfaces/ITrackSelectorTool.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"

#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "TRT_ReadoutGeometry/TRT_DetElementContainer.h"
namespace Trk {
/**
This algorithm creates a calo extension collection and stores it in a Gaudi Gate
named "ParticleCaloExtension" to be read by other algorithms. Clients who want
to use the stored extensions should know that the
Trk::IParticleCaloExtensionTool's function caloExtension(*track) returns a
unique pointer where as the stored extensions come from a
CaloExtensionCollection which is a vector of Trk::CaloExtension pointers. In
order to have access to these extrapolations and have the ability to calculate
extrapolations in case they are not stored in the gate one needs to do the
following: The original calo extension object should be renamed to denote that
it holds an unique pointer and will act as a helper pointer (this is important)
After creating a read handle, one can access the correct Trk::CaloExtension *
objects from the CaloExtensionCollection via the xAOD::TrackParticle's index()
method. The CaloExtensionCollection is indexed the same way as the
xAOD::TrackParticleContainer so if the client has a derived track class that
changes the index, it is important to get the index from the original
xAOD::TrackParticle rather than the derived class.

For usage and compatibility, one should do the same as is done in
TauElectronVetoVariables.cxx, TauTrackFinder.cxx, or
eflowTrackCaloExtensionTool.cxx. In short, the following will work for tracks
that are extrapolated: `extension = (*particleCache)[index];` In the case that
the desired extension is not extrapolated by this algorithm, the following needs
to be done: `uniqueExtension = theTrackExtrapolatorTool->caloExtension(*track);`
`extension = uniqueExtension.get();` One should avoid the use of `extension =
theTrackExtrapolatorTool->caloExtension(*track).get();` as it has unpredictable
behavior.
*/
class CaloExtensionBuilderAlg : public AthAlgorithm
{
public:
  using AthAlgorithm::AthAlgorithm;

  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  virtual StatusCode execute() override final
  {
    return execute_r(Algorithm::getContext());
  }
  // This will become the normal execute when
  // inheriting from AthReentrantAlgorithm
  StatusCode execute_r(const EventContext& ctx) const;

private:
  /** @brief the Calo Extension tool*/
  ToolHandle<Trk::IParticleCaloExtensionTool> m_particleCaloExtensionTool{
    this,
    "LastCaloExtentionTool",
    "Trk::CaloExtensionBuilderTool"
  };

  /// Manages the track selection. It should be able to handle both pflow and
  /// tau selections
  ToolHandle<InDet::IInDetTrackSelectionTool> m_TrkSelection{
    this,
    "TrkSelection",
    "TrkSelectionCaloExtensionBuilder",
    "Tool that handles the track selection"
  };

  /// output particle calo extension collection
  SG::WriteHandleKey<CaloExtensionCollection> m_ParticleCacheKey{
    this,
    "ParticleCache",
    "ParticleCaloExtension",
    "Name of the particle measurement extrapolation cache"
  };

  /// input Track collection and vertex
  SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrkPartContainerKey{
    this,
    "TrkPartContainerName",
    "InDetTrackParticles",
    "Container of tracks"
  };

  // For P->T converters of ID tracks with Pixel
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_pixelDetEleCollKey{ this,
                          "PixelDetEleCollKey",
                          "PixelDetectorElementCollection",
                          "Key of SiDetectorElementCollection for Pixel" };
  // For P->T converters of ID tracks with SCT
  SG::ReadCondHandleKey<InDetDD::SiDetectorElementCollection>
    m_SCTDetEleCollKey{ this,
                        "SCTDetEleCollKey",
                        "SCT_DetectorElementCollection",
                        "Key of SiDetectorElementCollection for SCT" };
  // For P->T converters of ID tracks with TRT
  SG::ReadCondHandleKey<InDetDD::TRT_DetElementContainer> m_trtDetEleContKey{
    this,
    "TRTDetEleContKey",
    "TRT_DetElementContainer",
    "Key of TRT_DetElementContainer"
  };
};
}

#endif //
