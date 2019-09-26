/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

/******************************************************************************
 * @package Trigger/TrigAlgorithms/TrigEFMissingET
 * @class   FexSum
 *
 * @brief Algorithm that sums TrigMissingET objects
 * @author Jon Burr
 *****************************************************************************/

#ifndef TRIGEFMISSINGET_FEXSUM_H
#define TRIGEFMISSINGET_FEXSUM_H

#include "TrigInterfaces/AllTEAlgo.h"

/******************************************************************************
 * @class SumFex
 *
 * Algorithm that sums TrigMissingET objects
 *****************************************************************************/
namespace HLT { namespace MET {
  class FexSum : public HLT::AllTEAlgo {
    public:
      /// Constructor
      FexSum(const std::string& name, ISvcLocator* pSvcLocator);

      virtual HLT::ErrorCode hltInitialize() override;
      virtual HLT::ErrorCode hltFinalize() override;

      /// Execute 
      virtual HLT::ErrorCode hltExecute(
          std::vector<std::vector<HLT::TriggerElement*>>& tes_in,
          unsigned int type_out) override;
    private:
      /************************************************************************
       * Properties
       ***********************************************************************/
      /// The output TE name
      std::string m_teName;
      /// Also sum the components (sum components with the same name)
      bool m_sumComponents;
  }; //> end class FexSum
} } //> end namespace HLT::MET

#endif //> !TRIGEFMISSINGET_FEXSUM_H
