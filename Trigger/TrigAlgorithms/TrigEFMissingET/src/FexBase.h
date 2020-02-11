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
#define TRIGEFMISSINGET_FEXBASE_H

#include "TrigEFMissingET/METMonitor.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "xAODTrigMissingET/TrigMissingET.h"

#include <vector>
#include <string>
#include <tuple>

// Foward declares
namespace HLT {
  class TriggerElement;
}


/******************************************************************************
 * @class FexBase 
 *
 * Base class for HLT MET Fex algorithms.
 * Derived classes should implement the TE algorithm interface (hltInitialize,
 * and hltFinalize).
 *****************************************************************************/
namespace HLT { namespace MET {
  class FexBase : public HLT::AllTEAlgo {
    public:
      /// Constructor
      FexBase(const std::string& name, ISvcLocator* pSvcLocator);
      /// Destructor
      virtual ~FexBase() override;

      /// Execute with given inputs
      virtual HLT::ErrorCode hltExecute(
          std::vector<std::vector<HLT::TriggerElement*>>& tes_in,
          unsigned int type_out) override;


    protected:

      /************************************************************************
       * Properties
       ***********************************************************************/
      /// The output container name
      std::string m_metOutputKey;
      /// The maximum MET/SumEt ratio per component (for flags)
      float m_maxComponentMetSumEtRatio;
      /// The maximum MET/SumEt ratio for the global value (for flags)
      float m_maxGlobalMetSumEtRatio;


      /************************************************************************
       * Data members (accessible from the derived classes)
       ***********************************************************************/
      /// The names of the components
      std::vector<std::string> m_componentNames;
      /// The monitor
      std::unique_ptr<IMETMonitor> m_monitor;

      /************************************************************************
       * Internal functions
       ***********************************************************************/
      /**
       * @brief Use the input TEs to fill the MET value
       * @param met The MET to fill.
       * @param tes_in The input TEs
       */
      virtual HLT::ErrorCode fillMET(
          xAOD::TrigMissingET& met,
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in) = 0;
      /**
       * @brief Initialize this base class object.
       * @param componentNames The names of the MET components produced by this
       * algorithm
       * @param monitor The IMETMonitor instance to use. If none is provided a
       * default one is set up.
       *
       * Note, the muon component is set by a property on this class. i.e. the
       * derived class should not provide when this function is called.
       * This must be called by the hltInitialize function of the derived class.
       */
      HLT::ErrorCode initializeBase(
          const std::vector<std::string>& componentNames,
          std::unique_ptr<IMETMonitor> monitor = nullptr);

      /**
       * @brief Apply a baseline set of flags to each component of the provided
       * MET object.
       */
      virtual HLT::ErrorCode flagMET(xAOD::TrigMissingET& met);

    private:
      /************************************************************************
       * Data members
       ***********************************************************************/
      /// Has the base class been initialized
      bool m_baseInitialised{false};
      /// The algorithm timer
      TrigTimer* m_timer{nullptr};

      /************************************************************************
       * Internal functions
       ***********************************************************************/
      /**
       * @brief Create the EDM object
       */
      std::unique_ptr<xAOD::TrigMissingET> makeEDMObject();

      /**
       * @brief Write the EDM object to the output TE
       */
      HLT::ErrorCode makeOutputTE(
          const std::vector<std::vector<HLT::TriggerElement*>>& tes_in,
          unsigned int type_out,
          std::unique_ptr<xAOD::TrigMissingET> met);

  }; //> end class FexBase
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_FEXBASE_H
