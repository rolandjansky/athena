/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <iterator>

#include "AthExHelloWorld/IHelloTool.h"
#include "HelloAlg.h"

/////////////////////////////////////////////////////////////////////////////
// FIXME Looks like we need operator<<( ostream&, pair<double, double > ) for gcc41
std::ostream& operator<<( std::ostream& s, std::pair<double, double > p )
{
  s << p.first << " " << p.second;
  return s;
}

HelloAlg::HelloAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_myTable()
{
  
  // Part 2: Declare the properties
  // some default values;
  m_myDict["Bonjour"]      = "Guten Tag";
  m_myDict["Good Morning"] = "Bonjour";
  m_myDict["one"]          = "uno";

  declareProperty("MyTable", m_myTable, "A table of <double,double>" );
  // some default values
  m_myTable.push_back( std::make_pair( 1., 1. ) );
  m_myTable.push_back( std::make_pair( 2., 2.*2. ) );
  m_myTable.push_back( std::make_pair( 3., 3.*3. ) );

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloAlg::initialize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("initialize()");

  // Part 2: Print out the property values
  ATH_MSG_INFO
    (   "  MyInt =    "    << (int) m_myInt << endmsg
        << "  MyBool =   " << (int) m_myBool << endmsg
        << "  MyDouble = " << (double) m_myDouble);

  for (unsigned int i=0; i<m_myStringVec.size(); i++) {
    ATH_MSG_INFO ("  MyStringVec[" << i << "] = " << m_myStringVec[i]);
  }

  for ( Dict_t::const_iterator itr = m_myDict.begin();
	itr != m_myDict.end();
	++itr ) {
    ATH_MSG_INFO
      ("  MyDict['" << itr->first << "'] = '" << itr->second << "'");
  }
  for ( Table_t::const_iterator itr = m_myTable.begin();
	itr != m_myTable.end();
	++itr ) {
    ATH_MSG_INFO 
      ("  MyTable['" << itr->first << "'] = '" << itr->second << "'");
  }
  for (unsigned int i=0; i<m_myMatrix.size(); i++) {
    msg(MSG::INFO) << "  MyMatrix[" << i << "] = [ ";
    std::copy( m_myMatrix[i].begin(), m_myMatrix[i].end(), 
	       std::ostream_iterator<double>(msg().stream(), " ") );
    msg() << "]" << endmsg;
  }

  ATH_MSG_INFO 
    ("  " << m_myPrivateHelloTool.propertyName() 
     << " = " << m_myPrivateHelloTool.type() 
     << endmsg
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

StatusCode HelloAlg::execute() {

  // Part 1: print where you are
  ATH_MSG_INFO ("execute()");

  // Part 1: Print out the different levels of messages
  ATH_MSG_DEBUG   ("A DEBUG message");
  ATH_MSG_INFO    ("An INFO message");
  ATH_MSG_WARNING ("A WARNING message");
  ATH_MSG_ERROR   ("An ERROR message");
  ATH_MSG_FATAL   ("A FATAL error message");

  // Part 1a: Let publicly declared tool say something
  ATH_MSG_INFO ("Let the tool " << m_myPublicHelloTool.propertyName() 
		<< " say something:");
  StatusCode sc1 = m_myPublicHelloTool->saySomething();

    // Part 1b: Let privately declared tool say something
  ATH_MSG_INFO ("Let the tool " << m_myPrivateHelloTool.propertyName()
		<< " say something:");
  StatusCode sc2 = m_myPrivateHelloTool->saySomething();

  if ( sc1.isFailure() || sc2.isFailure() ) {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloAlg::finalize() {

  // Part 1: print where you are
  ATH_MSG_INFO ("finalize()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloAlg::beginRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("beginRun()");

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode HelloAlg::endRun() {
  // Part 1: print where you are
  ATH_MSG_INFO ("endRun()");

  return StatusCode::SUCCESS;
}
