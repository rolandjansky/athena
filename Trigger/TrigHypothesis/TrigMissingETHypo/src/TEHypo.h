/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigHypothesis/TrigMissingETHypo
 * @class   TEHypo
 *
 * @brief Algorithm to make a cut decision on the total energy
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGMISSINGETHYPO_TEHYPO_H
#define TRIGMISSINGETHYPO_TEHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigEFMissingET/METMonitor.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class TEHypo
   *
   * TE Hypo class
   ***************************************************************************/
  class TEHypo : public HLT::HypoAlgo {
    public:
      TEHypo(const std::string& name, ISvcLocator* pSvcLocator);

      virtual HLT::ErrorCode hltInitialize() override;
      virtual HLT::ErrorCode hltFinalize() override;
      virtual HLT::ErrorCode hltExecute(
          const HLT::TriggerElement* outputTE, bool& pass) override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// The label of the TE feature to select on
      std::string m_label;
      /// The cut value
      double m_cut;
      /// The monitor
      METMonitor m_monitor;
  }; //> end class TEHypo
} } //> end namespace HLT::MET

#endif //> !TRIGMISSINGETHYPO_TEHYPO_H

