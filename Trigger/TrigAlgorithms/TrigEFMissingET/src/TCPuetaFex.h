/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TCPuetaFex
 *
 * @brief TC Fex class. Calculate MET from the negative vector sum over HLT
 * clusters.
 * @author Randall Sobie, Justin Chiu, Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TCPUETAFEX_H
#define TRIGEFMISSINGET_TCPUETAFEX_H

#include "FexBase.h"
#include "xAODCaloEvent/CaloCluster.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TCPuetaFex
   *
   * TC pu-eta Fex class. Calculates MET from a sum over clusters with a ring
   * based pileup subtraction.
   ***************************************************************************/
  class TCPuetaFex : public FexBase {
    public:
      /// Constructor
      TCPuetaFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the fex
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// The number of rings to use
      std::size_t m_nRings;
      /// The number of standard deviations away from the ring mean to allow in
      /// the correction
      float m_pileupNSigma;
      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;

      /// The cluster state to use. This is not configurable but it could be.
      /// It was configurable in the old algorithm but we only ever used it in
      /// LCW mode
      xAOD::CaloCluster::State m_clusState;


  }; //> end class TCPuetaFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_TCPUETAFEX_H

