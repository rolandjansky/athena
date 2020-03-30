/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TrkMHTFex
 *
 * @brief Fex class for trkmht algorithm
 * @author Jon Burr, Ren-Jie Wang
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TRKMHTFEX_H
#define TRIGEFMISSINGET_TRKMHTFEX_H 1

#include "FexBase.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include <utility>

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TrkMHTFex
   *
   * Class to create output from the trkmht algorithm.
   *
   * trkmht calculates the MET using jets with JVT suppression applied. There is
   * an additional track soft term which adds in any tracks associated to the
   * hard scatter vertex which are not associated to jets passing the JVT
   * selection.
   ***************************************************************************/
  class TrkMHTFex : public FexBase
  {
    // Alias optional to make it clear how we're using it
    template <typename T>
      using deferred_t = std::optional<T>;
    public:
      /// Constructor
      TrkMHTFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialise the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input jets and jvt
      SG::ReadHandleKey<xAOD::JetContainer> m_jetKey{
        this, "JetName", "jets", "Input jet collection"};
      /// Input tracks
      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_trackKey{
        this, "TrackName", "tracks",
        "Input track collection. Should be ghost-associated to the jets"};
      /// Input vertices
      SG::ReadHandleKey<xAOD::VertexContainer> m_vertexKey{
        this, "VertexName", "vertices", "Input vertex collection"};
      /// The track-vertex assocation name
      SG::ReadHandleKey<jet::TrackVertexAssociation> m_tvaKey{
        this, "TVAName", "trackVtxAssoc",
        "The name of the track-vertex association"};
      /// The Jvt selection name
      SG::ReadDecorHandleKey<xAOD::JetContainer> m_jvtKey{
        this, "JvtName", "Jvt", "The name of the JVT decorator"};
      /// The ghost-association aux element name
      Gaudi::Property<std::string> m_trackGAName{
        this, "TrackLinkName", "tracks", "The name of the jet track links"};
      /// Pt selection on forward jets
      Gaudi::Property<float> m_forwardJetPt{
        this, "ForwardJetPt", 0., "The pT cut applied to forward jets"};
      /// Minimum pt selection for JVT on central jets
      Gaudi::Property<float> m_minJvtJetPt{
        this, "MinimumJvtJetPt", 20. * Gaudi::Units::GeV,
        "The minimum pT for central jets for JVT selections to be used"};
      /// Maximum pt selection for JVT on central jets
      Gaudi::Property<float> m_maxJvtJetPt{
        this, "MaximumJvtJetPt", 50. * Gaudi::Units::GeV,
        "The maximum pT for central jets for JVT selections to be used"};
      /// The JVT selection
      Gaudi::Property<float> m_jvtSelection{
        this, "JVTCut", 0.9, "The JVT selection"};
      /// The track selection tool
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool{
        this, "TrackSelTool", "InDet::InDetTrackSelectionTool/TrackSelTool",
        "The track selection tool to use for tracks entering the track soft "
        "term."};
      /// The maximum pT for tracks in the soft term
      Gaudi::Property<float> m_tstPtCeil{
        this, "TrackSoftTermPtCeiling", 20. * Gaudi::Units::GeV,
        "The maximum pT for tracks entering the track soft term."};

      /************************************************************************
       * Internal functions
       ***********************************************************************/
      /**
       * @brief Calculate and fill the output MET value
       * @param met The object to fill
       * @param context The event context
       * @param monitors[out] Any extra monitors to fill
       */
      virtual StatusCode fillMET(
          xAOD::TrigMissingET& met,
          const EventContext& context,
          MonGroupBuilder& monitors) const override;

      /************************************************************************
       * Internal members
       ***********************************************************************/
      deferred_t<SG::AuxElement::ConstAccessor<std::vector<ElementLink<xAOD::IParticleContainer>>>> m_trackGA;
  }; //> end class TrkMHTFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_TRKMHTFEX_H
