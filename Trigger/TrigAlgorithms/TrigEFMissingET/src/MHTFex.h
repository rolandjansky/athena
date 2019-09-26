/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   MHTFex
 *
 * @brief MHT Fex class. Calculate MET from the negative vector sum over HLT
 * jets.
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_MHTFEX_H
#define TRIGEFMISSINGET_MHTFEX_H

#include "FexBase.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class MHTFex
   *
   * MHT Fex class. Calculates MET from a sum over jets.
   ***************************************************************************/
  class MHTFex : public FexBase {
    public:
      /// Constructor
      MHTFex(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~MHTFex() override;

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
      /// Whether or not to apply the tile-gap 3 correction
      bool m_applyTG3Corr;
      /// pT cut for central jets
      float m_centralPtCut;
      /// pT cut for forward jets
      float m_forwardPtCut;

      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;
      /// The eta bounds of our components
      std::vector<std::pair<float, float>> m_bounds;
  }; //> end class MHTFex
} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_MHTFEX_H
