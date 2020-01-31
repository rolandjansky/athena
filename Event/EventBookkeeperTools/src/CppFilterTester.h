/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENT_BOOKKEEPER_TOOLS__CPP_FILTER_TESTER
#define EVENT_BOOKKEEPER_TOOLS__CPP_FILTER_TESTER

// *****************************************************************
//
// Algorithm:  CppFilterTester
// Author: Joao Costa <joao.costa@cern.ch>
// Created: July 2010
//
// *********************************************************************

#include <AthenaBaseComps/AthFilterAlgorithm.h>

class CppFilterTester : public AthFilterAlgorithm
{
public:
  CppFilterTester(const std::string &name, ISvcLocator *pSvcLocator);
  virtual StatusCode initialize() final;
  virtual StatusCode execute() final;

private:
  CutIdentifier m_filterCutID{};

  Gaudi::Property<int> m_counter{this, "counter", 0, ""};
  Gaudi::Property<double> m_cut1{this, "cut1", 0., ""};
  Gaudi::Property<double> m_cut2{this, "cut2", 1., ""};
};

#endif
