/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TCFex
 *
 * @brief Fex class for the basic topocluster algorithm
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TCFEX_H
#define TRIGEFMISSINGET_TCFEX_H

#include "FexBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TCFex
   *
   * Class to create output from the topocluster algorithm
   *
   * tc calculates the MET with a sum over trigger-level topoclusters.
   ***************************************************************************/
  class TCFex : public FexBase
  {
    public:
      /// Constructor
      TCFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input clusters
      SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterKey{
        this, "ClusterName", "clusters", "Input cluster collection"};
      // TODO - for this next one I'm not sure what the best option is. We could
      // keep this logic inside the algorithm, *or* we could use the approach
      // standard within the jet slice, which is to use different cluster
      // collections to represent the EM and LCW calibrations (therefore the
      // differences between calibrated and uncalibrated inputs is at the
      // configuration level).
      /// Whether to use the uncalibrated clusters or not
      Gaudi::Property<bool> m_useUncalibrated{
        this, "UseUncalibrated", false,
        "Whether to use the uncalibrated clusters in the calculation"};

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
  }; //> end class TCFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_TCFEX_H
