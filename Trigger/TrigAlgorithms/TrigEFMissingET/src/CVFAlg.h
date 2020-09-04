/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   CVFAlg
 * @brief   Algorithm class to calculate CVF
 * @author  Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_CVFALG_H
#define TRIGEFMISSINGET_CVFALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AsgTools/ToolHandle.h"
#include "TrigEFMissingET/IExtendTrackToLayerTool.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TrackVertexAssociationTool/ITrackVertexAssociationTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "Gaudi/Property.h"

namespace HLT
{
  namespace MET
  {
    class CVFAlg : public ::AthReentrantAlgorithm
    {
    public:
      /// Constructor
      CVFAlg(const std::string &name, ISvcLocator *pSvcLocator);

      /// Initialise the algorithm
      virtual StatusCode initialize() override;

      /// Run the algorithm
      virtual StatusCode execute(const EventContext &ctx) const override;

    private:
      ToolHandle<IExtendTrackToLayerTool> m_extensionTool{
          this, "ExtensionTool", "", "The extension tool"};
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool{
          this, "TrackSelectionTool", "", "An optional track selection tool to filter tracks"};
      ToolHandle<CP::ITrackVertexAssociationTool> m_tvaTool{
          this, "TVATool", "", "Track -> vertex associationTool"};
      SG::ReadHandleKey<xAOD::CaloClusterContainer> m_inputClusterKey{
          this, "InputClusterKey", "", "Input cluster container"};
      SG::ReadHandleKey<xAOD::TrackParticleContainer> m_inputTrackKey{
          this, "InputTrackKey", "", "Input track container"};
      SG::ReadHandleKey<xAOD::VertexContainer> m_inputVertexKey{
          this, "InputVertexKey", "", "Input vertex container"};
      SG::WriteDecorHandleKey<xAOD::CaloClusterContainer> m_outputCVFKey{
          this, "OutputCVFKey", "", "Output CVF name"};
      Gaudi::Property<double> m_clusterMatchDR{
          this, "ClusterMatchDR", 0.1, "Max DR to match clusters to extrapolated tracks"};
      Gaudi::Property<bool> m_useCompatible{
          this, "UseCompatible", true,
          "Use the track -> vertex compatibility over unique matching."};

      // Internal
      /// Is a track selection tool being used?
      bool m_useTrackSelTool{false};
    }; //> end class CVFAlg
  }    // namespace MET
} // namespace HLT

#endif //> !TRIGEFMISSINGET_CVFALG_H