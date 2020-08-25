// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
   *  @short Algorithm for creating xAOD::TrigNavigation objects
   *
   *         This algorithm can be used to translate existing
   *         navigation part of HLT::HLTResult objects into xAOD::TrigNavigation ones.
   *
   * @author Tomasz Bold
   *
   * $Revision: $
   * $Date:  $
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

    Gaudi::Property<bool> m_doL2{this, "doL2", true, "flag whether or not to consider L2 trigger information"};
    Gaudi::Property<bool> m_doEF{this, "doEF", true, "flag whether or not to consider L3 (EF) trigger information"};
    Gaudi::Property<bool> m_doHLT{this, "doHLT", true, "flag whether or not to consider merged L2EF=HLT trigger information"};

    /// StoreGate key of the input object
    SG::ReadHandleKey<HLT::HLTResult> m_aodKeyL2{this, "AODKeyL2", "HLTResult_L2", "Input L2 HLTResult"};
    SG::ReadHandleKey<HLT::HLTResult> m_aodKeyEF{this, "AODKeyEF", "HLTResult_EF", "Input EF HLTResult"};
    SG::ReadHandleKey<HLT::HLTResult> m_aodKeyHLT{this, "AODKeyHLT", "HLTResult_HLT", "Input HLT (single level) HLTResult"};
    /// StoreGate key for the output object
    SG::WriteHandleKey<xAOD::TrigNavigation> m_xaodKey{this, "xAODKey", "TrigNavigation", "Input HLT (single level) HLTResult"};

    ToolHandle< ITrigNavigationCnvTool > m_cnvTool{this, "CnvTool", "xAODMaker::TrigNavigationCnvTool/TrigNavigationCnvTool", "Handle to the converter tool"};

  }; // class TrigNavigationCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H
