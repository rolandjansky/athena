// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H
#define XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "TrigSteeringEvent/HLTResult.h"
#include "xAODTrigger/TrigNavigation.h"
#include "xAODTrigger/TrigNavigationAuxInfo.h"

// Local include(s):
#include "xAODTriggerCnv/ITrigNavigationCnvTool.h"

namespace xAODMaker {

  /**
   *  @brief Algorithm for creating xAOD::TrigNavigation objects
   *
   *         This algorithm can be used to translate existing
   *         navigation part of HLT::HLTResult objects into xAOD::TrigNavigation ones.
   *
   * @author Tomasz Bold
   */
  class TrigNavigationCnvAlg : public AthReentrantAlgorithm {

  public:
    /// Regular Algorithm constructor
    TrigNavigationCnvAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize() override;
    /// Function executing the algorithm
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:

    /// StoreGate key of the input object
    SG::ReadHandleKey<HLT::HLTResult> m_aodKey{this, "AODKey", "HLTResult_HLT", "HLTResult input key"};

    /// StoreGate key for the output object
    SG::WriteHandleKey<xAOD::TrigNavigation> m_xaodKey{this, "xAODKey", "TrigNavigation", "xAOD Navigation output key"};

    ToolHandle< ITrigNavigationCnvTool > m_cnvTool{this, "CnvTool", "xAODMaker::TrigNavigationCnvTool/TrigNavigationCnvTool", "Handle to the converter tool"};

  }; // class TrigNavigationCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H
