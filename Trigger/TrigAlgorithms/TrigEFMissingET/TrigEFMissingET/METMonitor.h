/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   METMonitor
 *
 * @brief Base class for Fex monitoring
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_METMONITOR_H
#define TRIGEFMISSINGET_METMONITOR_H

#include "TrigInterfaces/Algo.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "xAODTrigMissingET/TrigMissingET.h"

#include <vector>
#include <string>
#include <bitset>

namespace HLT { namespace MET {

  /// Abstract base class
  class IMETMonitor {
    public:
      virtual ~IMETMonitor() {}
      /**
       * @brief Initialise all the registered monitoring variables to some
       * default values.
       */
      virtual void reset() = 0;

      /**
       * @brief Fill all the registered monitoring variables from an EDM object.
       * @param met The EDM object to use
       */
      virtual HLT::ErrorCode monitor(const xAOD::TrigMissingET& met) = 0;
  }; //> end class IMETMonitor

  class METMonitor : public AthMessaging, virtual public IMETMonitor {
    public:
      /**
       * @brief Constructor
       * @param parent The object that the monitoring is actually associated to.
       * We also inherit our messaging from this object.
       */
      METMonitor(Algo* parent);

      virtual ~METMonitor() override {}

      /**
       * @brief Initialise all the registered monitoring variables to some
       * default value
       */
      virtual void reset() override;

      /**
       * @brief Fill all the registered monitoring variables from an EDM object.
       * @param met The EDM object to use
       */
      virtual HLT::ErrorCode monitor(const xAOD::TrigMissingET& met) override;
    private:
      // The actual monitored variables
      float m_MExLog;
      float m_MEyLog;
      float m_MEzLog;
      float m_METLog;
      float m_MELog;
      float m_SumEtLog;
      float m_SumELog;
      float m_MExLin;
      float m_MEyLin;
      float m_MEzLin;
      float m_METLin;
      float m_MELin;
      float m_SumEtLin;
      float m_SumELin;
      float m_phi;
      std::vector<int> m_status;
  }; //> end class METMonitor

  /**
   * @brief Monitor class that also includes the monitoring of individual
   * components.
   */
  class METComponentMonitor : public METMonitor {
    public:
      /**
       * @brief Constructor
       * @param parent The object that the monitoring is actually associated to.
       * We also inherit our messaging from this object.
       */
      METComponentMonitor(Algo* parent);

      virtual ~METComponentMonitor() override {}

      /**
       * @brief Initialise all the registered monitoring variables to some
       * default value
       */
      virtual void reset() override;

      /**
       * @brief Fill all the registered monitoring variables from an EDM object.
       * @param met The EDM object to use
       */
      virtual HLT::ErrorCode monitor(const xAOD::TrigMissingET& met) override;

    private:
      // Component variables
      std::vector<float> m_compIndex;
      std::vector<float> m_compExLog;
      std::vector<float> m_compEyLog;
      std::vector<float> m_compEzLog;
      std::vector<float> m_compEtLog;
      std::vector<float> m_compSumEtLog;
      std::vector<float> m_compSumELog;
      std::vector<float> m_compEtLin;
      std::vector<float> m_compSumEtLin;
      std::vector<int> m_compStatus;
  }; 

} } //> end namespace HLT::MET
#endif //> !TRIGEFMISSINGET_METMONITOR_H
