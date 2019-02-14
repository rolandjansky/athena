/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTEST_TESTCOOLDCS_H
#define IOVDBTEST_TESTCOOLDCS_H
// IOVDbTestCoolDCS.h
// a simple algorithm to demonstrate reading of DCS data stored in COOL
// into an Athena algorithm
// Richard Hawkings, started 9/9/05

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include <vector>
#include <string>

class StoreGateSvc;

class IOVDbTestCoolDCS: public AthReentrantAlgorithm
{
 public:
    IOVDbTestCoolDCS(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~IOVDbTestCoolDCS();

    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

private:
  // list folders to be read as AthenaAttributeList
  std::vector<std::string> m_par_atrlist;   
  // list folders to be read as CondAttrListCollection*
  std::vector<std::string> m_par_atrcollist;
};

#endif // IOVDBTEST_TESTCOOLDCS_H
