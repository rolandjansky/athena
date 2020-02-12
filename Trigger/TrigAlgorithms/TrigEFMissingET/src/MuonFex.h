/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   MuonFex
 *
 * @brief Calculate the MET coming from the sum over HLT muons. Not intended to
 * be used by itself but rather in combination with some other algorithm.
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_MUONFEX_H
#define TRIGEFMISSINGET_MUONFEX_H

#include "FexBase.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class MuonFex
   *
   * TC Fex class. Calculates MET from a sum over clusters.
   ***************************************************************************/
  class MuonFex : public FexBase {
    public:
      /// Constructor
      MuonFex(const std::string& name, ISvcLocator* pSvcLocator);

      /// Initialize the fex
      virtual HLT::ErrorCode hltInitialize() override;

      /// Finalize the fex
      virtual HLT::ErrorCode hltFinalize() override;
    private:
      /************************************************************************
       * Internals
       ***********************************************************************/
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) override;
  }; //> end class MuonFex
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_MUONFEX_H

