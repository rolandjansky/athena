// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H
#define XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

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
  class TrigNavigationCnvAlg : public AthAlgorithm {

  public:
    /// Regular Algorithm constructor
    TrigNavigationCnvAlg( const std::string& name, ISvcLocator* svcLoc );

    /// Function initialising the algorithm
    virtual StatusCode initialize();
    /// Function executing the algorithm
    virtual StatusCode execute();

  private:
    /// StoreGate key of the input object
    std::vector<std::string> m_aodKeys;
    /// StoreGate key for the output object
    std::string m_xaodKey;

    /// Handle to the converter tool
    ToolHandle< ITrigNavigationCnvTool > m_cnvTool;

  }; // class TrigNavigationCnvAlg

} // namespace xAODMaker

#endif // XAODTRIGGERCNV_TRIGNAVIGATIONCNVALG_H
