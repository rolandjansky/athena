/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define TBBDEBUG
// STL includes
#include <vector>
#include <string>
#include <sstream>
using namespace std;

#include "TBBExamples/IHelloTool.h"
#include "HelloParForAlg.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

HelloParForAlg::HelloParForAlg(const string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), m_nIters(0),
  m_myPrivateHelloTool("HelloTool",this), m_myPublicHelloTool("HelloTool")
{
  
  // Part 2: Declare the properties
  declareProperty("NIters", m_nIters);

  declareProperty("MyPrivateHelloTool", m_myPrivateHelloTool, "private IHelloTool");
  declareProperty("MyPublicHelloTool", m_myPublicHelloTool, "public, shared IHelloTool");

}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloParForAlg::initialize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("initialize()");

  ATH_MSG_INFO 
    ("  " << m_myPrivateHelloTool.propertyName() 
     << " = " << m_myPrivateHelloTool.type() 
     << endreq
     << "  " << m_myPublicHelloTool.propertyName() 
     << " = " << m_myPublicHelloTool.type());


  // Part 3: Retrieve the tools using the ToolHandles
  if ( m_myPrivateHelloTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_myPrivateHelloTool.propertyName() << ": Failed to retrieve tool "
       << m_myPrivateHelloTool.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO 
      (m_myPrivateHelloTool.propertyName() << ": Retrieved tool " 
       << m_myPrivateHelloTool.type());
  }
  if ( m_myPublicHelloTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL 
      (m_myPublicHelloTool.propertyName() << ": Failed to retrieve tool "
       << m_myPublicHelloTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO (m_myPublicHelloTool.propertyName() << ": Retrieved tool "
		  << m_myPublicHelloTool.type());
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
class Filler {
private:
  string m_initial;
  vector<string>& m_toFill;
public:
  Filler(const std::string& initial, vector<string>& toFill) :
    m_initial(initial), m_toFill(toFill) {}
  void operator() (const tbb::blocked_range<size_t>& r) const { 
#ifdef TBBDEBUG
    cout << "Filler grainsize " << r.end() - r.begin();
#endif
    for( size_t i=r.begin(); i!=r.end(); ++i ) {
      stringstream ss;
      ss << m_initial << ' ' << i << ':' <<ends;
      m_toFill[i] = ss.str();
    } 
  }
};

class SayTools {
private:
  SayTools();
  HelloParForAlg& m_host;
  mutable int m_called; //this is because operator() in parallel_for is const
  vector<string>& m_messages;
public:
  SayTools(HelloParForAlg& alg, vector<string>& m):  
    m_host(alg), m_called(0), m_messages(m) {}
  void operator() (const tbb::blocked_range<size_t>& r) const { 
#ifdef TBBDEBUG
    cout << "SayTools grainsize " << r.end() - r.begin();
#endif
    for( size_t i=r.begin(); i!=r.end(); ++i ) {
      // Part 1a: Let publicly declared tool say something
      StatusCode sc1 = m_host.m_myPublicHelloTool->saySomething(m_messages[i]);
      // Part 1b: Let privately declared tool say something
      StatusCode sc2 = m_host.m_myPrivateHelloTool->saySomething(m_messages[i]);
      if ( sc1.isFailure() || sc2.isFailure() ) {
	throw("bad news");
      }
    }
    ++m_called;
  }
};

StatusCode HelloParForAlg::execute() {

  // Part 1: print where you are
  ATH_MSG_INFO ("execute()");

  vector<string> messages(m_nIters);
  Filler f("Counting in element ", messages);
  tbb::parallel_for(tbb::blocked_range<size_t>(0,m_nIters), f);
  SayTools s(*this, messages);
  tbb::parallel_for(tbb::blocked_range<size_t>(0,m_nIters), s);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloParForAlg::finalize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("finalize()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloParForAlg::beginRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("beginRun()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloParForAlg::endRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("endRun()");

  return StatusCode::SUCCESS;
}
