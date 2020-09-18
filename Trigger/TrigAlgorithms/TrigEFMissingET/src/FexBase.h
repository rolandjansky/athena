/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   FexBase
 *
 * @brief Base class for MET Trigger Fexs
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_FEXBASE_H
#define TRIGEFMISSINGET_FEXBASE_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "Gaudi/Property.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "MonGroupBuilder.h"

#include <string>
#include <vector>


namespace HLT { namespace MET {
  /****************************************************************************
  * @class FexBase 
  *
  * Base class for HLT MET Fex algorithms.
  *
  * Responsible for creating the MET object
  ***************************************************************************/
  class FexBase : public ::AthReentrantAlgorithm {
    public:
      /// Constructor
      FexBase(const std::string& name, ISvcLocator* pSvcLocator);

      /// Run the algorithm
      virtual StatusCode execute(const EventContext& context) const override;

    protected:
      /************************************************************************
       * Data members (accessible from derived classes)
       ***********************************************************************/
      /// The names of the output MET components
      std::vector<std::string> m_componentNames;

      /************************************************************************
       * Internal functions
       ***********************************************************************/
      /**
       * @brief Initialize the base class
       * @param componentNames The names of any components in the output MET
       * object.
       *
       * The xAOD::TrigMissingET writes out a main 'met' value but can also
       * write out extra MET components - either for debugging, monitoring or
       * further use offline. The object has to be initialized with this names,
       * they cannot be extended after this, so this information must be
       * available for the base class to use.
       */
      StatusCode initializeBase(
          const std::vector<std::string>& componentNames);

      /**
       * @brief Calculate and fill the output MET value
       * @param met The object to fill
       * @param context The event context
       * @param monitors Extra variables to be monitored
       *
       * The builder passed in to the monitors variable can be filled with any
       * variables that the particular algorithm wishes to monitor. Extra
       * variables that can be calculated directly from the xAOD::TrigMissingET
       * object can also be added using the monitor function.
       */
      virtual StatusCode fillMET(
          xAOD::TrigMissingET& met,
          const EventContext& context,
          MonGroupBuilder& monitors) const = 0;

      /**
       * @brief Add monitor variables from an xAOD::TrigMissingET object
       * @param met The object to fill from
       * @param monitors Extra variables to be monitored
       */
      virtual StatusCode monitor(
          const xAOD::TrigMissingET& met,
          MonGroupBuilder& monitors) const;

      /**
       * @brief Flag suspicious values in the output MET
       * @param met The xAOD::TrigMissingET object to flag
       */
      virtual StatusCode flagMET(xAOD::TrigMissingET& met) const;

    private:
      /************************************************************************
       * Properties (Not accessible from derived classes)
       ***********************************************************************/
      /// The output MET object
      SG::WriteHandleKey<xAOD::TrigMissingETContainer> m_metContainerKey {
        this, "METContainerKey", "HLT_MET", "Name of the output MET object"};
      /// The monitoring tool
      ToolHandle<GenericMonitoringTool> m_monTool{
        this, "MonTool", "", "Monitoring tool"};
      Gaudi::Property<float> m_maxComponentMETSumEtRatio{
        "MaxComponentMETSumEtRatio", 1.,
        "The maximum MET/SumEt ratio per component"};
      Gaudi::Property<float> m_maxGlobalMETSumEtRatio{
        "MaxGlobalMETSumEtRatio", 1.,
        "The maximum MET/SumEt ratio for the total value"};

      /************************************************************************
       * Data members (Not accessible from derived classes)
       ***********************************************************************/
      /// Whether or not this class has been correctly initialised.
      bool m_baseInitialised{false};
  }; //> end class FexBase
} } //> end namespace HTL::MET
#endif //> !TRIGEFMISSINGET_FEXBASE_H
