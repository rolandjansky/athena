/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   MHTFex
 *
 * @brief Fex class for the basic mht algorithm
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_MHTFEX_H
#define TRIGEFMISSINGET_MHTFEX_H

#include "FexBase.h"
#include "xAODJet/JetContainer.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class MHTFex
   *
   * Class to create output from the mht algorithm
   *
   * mht calculates the MET with a sum over trigger-level jets.
   ***************************************************************************/
  class MHTFex : public FexBase
  {
    public:
      /// Constructor
      MHTFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual StatusCode initialize() override;

    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Input jets
      SG::ReadHandleKey<xAOD::JetContainer> m_jetKey{
        this, "JetName", "jets", "Input jet colleciton"};
      /// Eta separation
      Gaudi::Property<float> m_etaSeparation{
        this, "EtaSeparation", 2.4,
        "Selection on |eta| to separate the central and forward regions"};
      /// Central pT cut
      Gaudi::Property<float> m_centralPtCut{
        this, "CentralPtCut", 0, "pT cut for central jets"};
      /// Forward pT cut
      Gaudi::Property<float> m_forwardPtCut{
        this, "ForwardPtCut", 0, "pT cut for forward jets"};

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

      /// Is a jet central or forward
      inline bool isCentral(const xAOD::Jet* ijet) const
      { return std::abs(ijet->eta() ) < m_etaSeparation; }
  }; //> end class MHTFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_MHTFEX_H
