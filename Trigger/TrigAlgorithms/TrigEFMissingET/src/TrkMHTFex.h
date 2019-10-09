/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TrkMHTFex
 *
 * @brief TrkMHT Fex class. Calculate MET from the negative vector sum over HLT
 * jets, with an additional track soft term and JVT selection.
 * @author Jon Burr, Ren-Jie Wang
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TRKMHTFEX_H
#define TRIGEFMISSINGET_TRKMHTFEX_H

#include "FexBase.h"

#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "TH2.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TrkTrkMHTFex
   *
   * TrkMHT Fex class. Calculates MET from a sum over jets with track soft term
   * and JVT selection.
   ***************************************************************************/
  class TrkMHTFex : public FexBase {
    public:
      /// Constructor
      TrkMHTFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~TrkMHTFex() override {}

      /// Initialize the job
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the job
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Where we count the barrel-endcap split to be in eta
      float m_barrelEndcapEta;
      /// pT cut for central jets
      float m_centralPtCut;
      /// pT cut for forward jets
      float m_forwardPtCut;
      /// pT cut for tracks
      float m_trackPtCut;
      /// JVT cut
      float m_jvtCut;
      /// Minimum pT to apply JVT to
      float m_minJetJVTPt;
      /// Max pT to apply JVT to
      float m_maxJetJVTPt;
      /// The track selection tool
      ToolHandle<InDet::IInDetTrackSelectionTool> m_trackSelTool;
      /// The JVT file name
      std::string m_jvtFile;
      /// The JVT histogram name
      std::string m_jvtHistoName;
      /// The maximum pT for tracks in the TST
      float m_tstPtCeil;

      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;
      /// The eta bounds of our components
      std::vector<std::pair<float, float>> m_bounds;
      /// The JVT LUT. I would prefer to move the JVT calculation out of this
      /// algorithm (at least for master)
      TH2F m_jvtLUT;
  }; //> end class TrkMHTFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_TRKMHTFEX_H
