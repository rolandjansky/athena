/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   PFOPrepAlg
 *
 * @brief Algorithm to prepare PFOs for the PUSplit pufit alg
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_PFOPREPALG_H
#define TRIGEFMISSINGET_PFOPREPALG_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "AsgTools/ToolHandle.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include <utility>

namespace HLT
{
  namespace MET
  {
    /**************************************************************************
     * @class PFOPrepAlg
     * 
     * Class to prepare the PFOs for the PUSplit pufit alg.
     * 
     * The PUSplit alg expects a single input container, which has a decoration
     * classifying each object according to the @see PUClassification enum. This
     * algorithm reads in the input charged and neutral PFO containers and
     * combines them.
     * 
     * The classification either uses the 'matchedToPV' decoration from the cPFO
     * creation, or a TVA tool, (optional) track selection tool and vertex
     * container to determine the classification.
     **************************************************************************/
    class PFOPrepAlg : public AthReentrantAlgorithm
    {
      // Alias optional to make it clear how we're using it
      template <typename T>
      using deferred_t = std::optional<T>;

    public:
      /// Constructor
      PFOPrepAlg(const std::string &name, ISvcLocator *pSvcLocator);

      virtual StatusCode initialize() override;
      /// Run the algorithm
      virtual StatusCode execute(const EventContext &context) const override;

    private:
      /// The input neutral container
      SG::ReadHandleKey<xAOD::PFOContainer> m_inputNeutralKey{
          this, "InputNeutralKey", "", "Input neutral container"};
      /// The input charged container
      SG::ReadHandleKey<xAOD::PFOContainer> m_inputChargedKey{
          this, "InputChargedKey", "", "Input charged container"};
      /// The input vertex container
      SG::ReadHandleKey<xAOD::VertexContainer> m_inputVertexKey{
          this, "InputVertexKey", "",
          "Input vertex container (only needed if TVA tool is supplied."};
      /// The output combined container
      SG::WriteHandleKey<xAOD::PFOContainer> m_outputKey{
          this, "OutputKey", "", "Output combined container"};
      /// The output classification
      SG::WriteDecorHandleKey<xAOD::PFOContainer> m_outputCategoryKey{
          this, "OutputCategoryKey", "", "Output category name"};
      /// Track -> vertex association
      ToolHandle<CP::ITrackVertexAssociationTool> m_tvaTool{
          this, "TVATool", "",
          "Track -> vertex association tool for the cPFOS. "
          "If not provided the matchedToPV decoration is used."};
      /// Track selection tool
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool{
          this, "TrackSelTool", "",
          "The track selection tool (optional). Only used if a TVA tool is provided"};
      /// Choose between the unique (many-to-one) track->vertex association and
      /// just checking compatibility with the PV (many-to-many).
      /// Studies with offline objects show a fairly clear preference for the
      /// 'compatible' approach so that is the default
      Gaudi::Property<bool> m_useCompatible{
          this, "UseCompatible", true,
          "Use the track -> vertex compatibility over unique matching. "
          "This only takes effect if a TVA tool is supplied"};

      /// Whether to perform track -> vertex matching manually.
      bool m_manualTVA{false};

      // Internal
      deferred_t<SG::AuxElement::Decorator<int>> m_decCategory;
    };
  } // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_PFOPREPALG
