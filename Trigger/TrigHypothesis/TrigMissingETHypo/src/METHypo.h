/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigHypothesis/TrigMissingETHypo
 * @class   METHypo
 *
 * @brief Algorithm to make the MET cut decision
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGMISSINGETHYPO_METHYPO_H
#define TRIGMISSINGETHYPO_METHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigEFMissingET/METMonitor.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class METHypo
   *
   * MET Hypo class
   ***************************************************************************/
  class METHypo : public HLT::HypoAlgo {
    public:
      METHypo(const std::string& name, ISvcLocator* pSvcLocator);

      virtual HLT::ErrorCode hltInitialize() override;
      virtual HLT::ErrorCode hltFinalize() override;
      virtual HLT::ErrorCode hltExecute(
          const HLT::TriggerElement* outputTE, bool& pass) override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// The label of the MET feature to select on
      std::string m_label;
      /// The cut value
      double m_cut;
      /// The monitor
      METMonitor m_monitor;
  }; //> end class METHypo
} } //> end namespace HLT::MET

#endif //> !TRIGMISSINGETHYPO_METHYPO_H
