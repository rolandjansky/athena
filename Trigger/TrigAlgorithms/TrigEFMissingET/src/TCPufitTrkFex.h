/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TCPufitTrkFex
 *
 * @author Rui Zou, Ben Carlson, Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TCPUFITTRKFEX_h
#define TRIGEFMISSINGET_TCPUFITTRKFEX_h

#include "FexBase.h"
#include "AsgTools/ToolHandle.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TH2.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TCPufitTrkFex
   *
   * pufittrk FEX class. Calculates MET using topoclusters applying a fit to remove
   * pileup contributions. Jets passing JVT selections are used to select which
   * towers are used as hard scatter.
   ***************************************************************************/
  class TCPufitTrkFex : public FexBase {
    public:
      /// Constructor
      TCPufitTrkFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~TCPufitTrkFex() override;

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the fex
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// minimum pT cut for central jets
      float m_minJetPtRpT;
      /// maximum pT cut to apply RpT (above this always accept)
      float m_maxJetPtRpT;
      /// pT cut for forward jets
      float m_forwardPtCut;
      /// RPT cut
      float m_RpTCut;
      /// pT cut for tracks entering into the RpT calculation
      float m_trackPtCut;
      /// The track selection tool
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;
      /// The eta range to use
      double m_maxEta;
      /// The number of eta bins
      std::size_t m_nEtaBins;
      /// The number of phi bins
      std::size_t m_nPhiBins;
      /// The trim fraction to use
      double m_trimFraction;
      /// The noise term in the calorimeter noise estimate
      double m_caloNoise;
      /// The stochastic term in the calorimeter noise estimate
      double m_caloStoch;
      /// The relative importance of the different constraints
      double m_constraintImportance;
      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;
  }; //> end class TCPufitTrkFex
} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_TCPUFITTRKFEX_h
