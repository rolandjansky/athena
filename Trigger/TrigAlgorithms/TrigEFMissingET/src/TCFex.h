/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TCFex
 *
 * @brief TC Fex class. Calculate MET from the negative vector sum over HLT
 * clusters.
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TCFEX_H
#define TRIGEFMISSINGET_TCFEX_H

#include "FexBase.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TCFex
   *
   * TC Fex class. Calculates MET from a sum over clusters.
   ***************************************************************************/
  class TCFex : public FexBase {
    public:
      /// Constructor
      TCFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~TCFex() override;

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the fex
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Use the uncalibrated (EM) level topoclusters instead
      bool m_saveUncalibrated;
      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;

      /// The cluster state to access
      xAOD::CaloCluster::State m_clusterState;

  }; //> end class TCFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_TCFEX_H
