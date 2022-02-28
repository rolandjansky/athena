/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   NNHLTFex
 *
 * @brief Fex class for the Neural netowrk algorithm that combines HLT inputs
 * @author Zhelun Li
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_NNHLTFEX_H
#define TRIGEFMISSINGET_NNHLTFEX_H

#include "FexBase.h"
#include "xAODPFlow/PFOContainer.h"

// Externals
#include "lwtnn/LightweightGraph.hh"
#include "lwtnn/NanReplacer.hh"
#include "lwtnn/parse_json.hh"



namespace HLT::MET {
  /****************************************************************************
   * @class NNHLTFex
   *
   * Class to create output from the HLT NN algorithm
   ***************************************************************************/
  
  class NNHLTFex : public FexBase
  {
    public:
      /// Constructor
      NNHLTFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/

      SG::ReadHandleKeyArray<xAOD::TrigMissingETContainer> m_triggerMETs{
      this, "TriggerMETs", {"HLT_MET_tcpufit","HLT_MET_cell","HLT_MET_trkmht","HLT_MET_pfopufit","HLT_MET_mhtpufit_pf_subjesgscIS","HLT_MET_pfsum_vssk"}, "TriggerMETs"};
      Gaudi::Property<std::string> m_filename{
          this, "InputFileName","/eos/atlas/atlascerngroupdisk/asg-calib/TrigEFMissingET/20220211/NNsingleLayerRed.json", "The NN input files in json, used to build NN in C++"};
      // lightweight graph and preprocessor
      lwt::GraphConfig m_configNN;
      std::unique_ptr<lwt::LightweightGraph> m_graphNN;

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
  }; //> end class NNHLTFex
} //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_NNHLTFex_H
