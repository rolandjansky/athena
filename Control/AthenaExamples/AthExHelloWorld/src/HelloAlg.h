// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOALG_H
#define ATHEXHELLOWORLD_HELLOALG_H 1

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "Gaudi/Property.h"

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

  StatusCode start();
  StatusCode stop();
  
private:
  // Properties
  Gaudi::Property<int>    m_myInt    {this, "MyInt", 0, "An Integer"};
  Gaudi::Property<bool>   m_myBool   {this, "MyBool", false, "A Bool"};
  Gaudi::Property<double> m_myDouble {this, "MyDouble", 0., "A Double"};

  Gaudi::Property< std::vector<std::string> > m_myStringVec {this, "MyStringVec", {}, "an entire vector of strings"};

  typedef std::map<std::string, std::string> Dict_t;
  Gaudi::Property<Dict_t> m_myDict {this, "MyDict", {}, "A little dictionary"};

  typedef std::vector<std::vector<double> > Matrix_t;
  Gaudi::Property<Matrix_t> m_myMatrix {this, "MyMatrix", {}, "A matrix of doubles"};

  // legacy style Property
  typedef std::vector<std::pair<double, double> > Table_t;
  Table_t m_myTable;


  // ToolHandles as Properties
  ToolHandle< IHelloTool > m_myPrivateHelloTool {this, "MyPrivateHelloTool", 
      "HelloTool", "private IHelloTool"};

  PublicToolHandle< IHelloTool > m_myPublicHelloTool {this, "MyPublicHelloTool",
      "HelloTool", "public, shared IHelloTool"};
  
};

#endif // ATHEXHELLOWORLD_HELLOALG_H
