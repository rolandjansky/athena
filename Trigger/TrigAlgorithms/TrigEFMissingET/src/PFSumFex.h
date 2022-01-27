/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   PFSumFex
 *
 * @brief Fex class for the basic PFlow algorithm
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_PFSUMFEX_H
#define TRIGEFMISSINGET_PFSUMFEX_H

#include "FexBase.h"
#include "xAODBase/IParticleContainer.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class PFSumFex
   *
   * Class to create output from the PFlow algorithm
   *
   * tc calculates the MET with a sum over trigger-level PFOs.
   ***************************************************************************/
  class PFSumFex : public FexBase
  {
    public:
      /// Constructor
      PFSumFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input neutral PFOs
      SG::ReadHandleKey<xAOD::IParticleContainer> m_neutralPFOKey{
        this, "NeutralPFOName", "neutralPFOs", "Input neutral PFO collection"};
      /// Input charged PFOs
      SG::ReadHandleKey<xAOD::IParticleContainer> m_chargedPFOKey{
        this, "ChargedPFOName", "chargedPFOs", "Input charged PFO collection"};
      /// Whether to only include charged PFOs that are matched to the PV vertex
      Gaudi::Property<bool> m_chargedOnlyFromPV{
        this, "ChargedOnlyFromPV", true, "Only use charged PFOs from the PV"};

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
  }; //> end class PFSumFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_PFSUMFEX_H
