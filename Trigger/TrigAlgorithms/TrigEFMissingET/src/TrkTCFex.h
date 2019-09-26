/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   TrkTCFex
 *
 * @brief TrkTC Fex class. Calculate MET from the negative vector sum over
 * clusters with CVF and VSSK suppression applied, with an additional track soft
 * term.
 * @author Jon Burr, Ren-Jie Wang
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_TRKTCFEX_H
#define TRIGEFMISSINGET_TRKTCFEX_H

#include "FexBase.h"
#include "TrigEFMissingET/ICVFTool.h"
#include "JetInterface/IJetConstituentModifier.h"
#include "GaudiKernel/ToolHandle.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TrkTCFex
   *
   * TrkTC Fex class. Calculates MET from a sum over clusters with VSSK and CVF
   * suppression with an additional track soft term.
   ***************************************************************************/
  class TrkTCFex : public FexBase {
    public:
      /// Constructor
      TrkTCFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~TrkTCFex() override;

      /// Initialize the job
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the job
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// Cluster modifier tools (SK and Voronoi)
      ToolHandleArray<IJetConstituentModifier> m_clusModTools;
      /// The CVFTool
      ToolHandle<ICVFTool> m_cvfTool;
      /// CVF threshold
      float m_cvfThreshold;

      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;

      /// The link accessor
      using trackLink_t = ElementLink<xAOD::TrackParticleContainer>;
      using vecTrackLink_t = std::vector<trackLink_t>;
      using accTrackLinks_t = SG::AuxElement::ConstAccessor<vecTrackLink_t>;
      std::unique_ptr<accTrackLinks_t> m_trackClusLinks;

  }; //> end class TrkTCFex
} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_TRKTCFEX_H
