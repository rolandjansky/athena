/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigHypothesis/TrigMissingETHypo
 * @class   XSHypo
 *
 * @brief Algorithm to make the XS cut decision
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGMISSINGETHYPO_XSHYPO_H
#define TRIGMISSINGETHYPO_XSHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigEFMissingET/METMonitor.h"

namespace HLT { namespace MET {
  /****************************************************************************
   * @class XSHypo
   *
   * XS Hypo class
   ***************************************************************************/
  class XSHypo : public HLT::HypoAlgo {
    public:
      XSHypo(const std::string& name, ISvcLocator* pSvcLocator);

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
      // Significance parameters
      /// Offset
      double m_xsOffset;
      /// Linear term
      double m_xsLinear;
      /// Quadratic term
      double m_xsQuadratic;
      /// MET value below which we always fail
      double m_minMET;
      /// Sum Et value below which we always fail
      double m_minSumEt;
      /// MET value above which we always pass
      double m_maxMET;
      /// Sum Et value above which we always pass
      double m_maxSumEt;
      /**
       * @brief Accept the monitor class to include the significance
       */
      class Monitor : public METMonitor {
        public:
          Monitor(Algo* parent);

          ~Monitor() override {}

          /**
           * @brief Initialise all the registered monitoring variables to some
           * default value
           */
          void reset() override;

          /// Set the internal xs value
          void setXS(float xs);
        private:
          /// The monitored value
          float m_xs;
          float m_xs2;
      }; //> end class Monitor
      Monitor m_monitor;
  }; //> end class XSHypo
} } //> end namespace HLT::MET

#endif //> !TRIGMISSINGETHYPO_XSHYPO_H
