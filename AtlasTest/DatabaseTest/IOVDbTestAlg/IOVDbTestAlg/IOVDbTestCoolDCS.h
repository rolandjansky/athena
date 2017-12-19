/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVDBTEST_TESTCOOLDCS_H
#define IOVDBTEST_TESTCOOLDCS_H
// IOVDbTestCoolDCS.h
// a simple algorithm to demonstrate reading of DCS data stored in COOL
// into an Athena algorithm
// Richard Hawkings, started 9/9/05

#include "AthenaBaseComps/AthAlgorithm.h"
#include <vector>
#include <string>

class StoreGateSvc;

class IOVDbTestCoolDCS: public AthAlgorithm
{
 public:
    IOVDbTestCoolDCS(const std::string& name, ISvcLocator* pSvcLocator);
    ~IOVDbTestCoolDCS();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

private:
  StoreGateSvc* p_evtstore;
  StoreGateSvc* p_detstore;
  // list folders to be read as AthenaAttributeList
  std::vector<std::string> m_par_atrlist;   
  // list folders to be read as CondAttrListCollection*
  std::vector<std::string> m_par_atrcollist;
};

#endif // IOVDBTEST_TESTCOOLDCS_H
