/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTConditionsTestAlgMT_H
#define MDTConditionsTestAlgMT_H

// STL
#include <sstream>
#include <string>

// Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"

// Athena
#include "Identifier/Identifier.h"
#include "MuonCondData/MdtCondDbData.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"
#include "MuonCondSvc/MuonHierarchy.h"

// Forward declarations
class ISvcLocator;
class StatusCode;
class MdtCondDbData;

/// Example class to show calling the SCT_ConditionsSummarySvc
class MDTConditionsTestAlgMT : public AthAlgorithm {
public:
    MDTConditionsTestAlgMT(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~MDTConditionsTestAlgMT();

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

private:
    SG::ReadCondHandleKey<MdtCondDbData> m_readKey{this, "ReadKey", "MdtCondDbData", "Key of MdtCondDbData"};

};  // end of class

#endif
