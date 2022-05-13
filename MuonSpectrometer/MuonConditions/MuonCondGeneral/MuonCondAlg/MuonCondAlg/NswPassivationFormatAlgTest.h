/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NswPassivationFormatAlgTest_H
#define NswPassivationFormatAlgTest_H

// STL
#include <stdio.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "vector"

// Gaudi
#include "CoralBase/Blob.h"
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaKernel/IOVInfiniteRange.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CoralUtilities/blobaccess.h"
#include "StoreGate/ReadCondHandleKey.h"

// Forward declarations
class ISvcLocator;
class StatusCode;

namespace coral {
    class Blob;
}

class NswPassivationFormatAlgTest : public AthReentrantAlgorithm {
public:
    NswPassivationFormatAlgTest(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~NswPassivationFormatAlgTest() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &) const override;
    virtual StatusCode finalize() override;

private:
    StatusCode retrieve(const EventContext &, std::chrono::duration<double> &) const;
    static std::string timestamp() ;

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/MDT/MM/PASSIVATION",
                                                                  "Key to passivation conditions folder"};

};  // end of class

#endif
