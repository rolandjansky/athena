// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOALG_H
#define ATHEXHELLOWORLD_HELLOALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include <utility>
#include <map>

/////////////////////////////////////////////////////////////////////////////

class IHelloTool;

class HelloAlg : public AthAlgorithm {
public:
  HelloAlg( const std::string& name, ISvcLocator* pSvcLocator );
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode beginRun();
  StatusCode endRun();
  
private:
  int    m_myInt;
  bool   m_myBool;
  double m_myDouble;
  std::vector< std::string > m_myStringVec;

  ToolHandle< IHelloTool > m_myPrivateHelloTool;
  ToolHandle< IHelloTool > m_myPublicHelloTool;

  typedef std::map<std::string, std::string> Dict_t;
  Dict_t m_myDict;
  typedef std::vector<std::pair<double, double> > Table_t;
  Table_t m_myTable;
  typedef std::vector<std::vector<double> > Matrix_t;
  Matrix_t m_myMatrix;
};

#endif // ATHEXHELLOWORLD_HELLOALG_H
