/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// This file contains the driver class for all CppUnit test classes
// and the function to instantiate and initialize the StoreGateSvc
// All registered CppUnit test classes will be run.
// You can also modify the output (text, compiler, XML).
// Authors: S.Binet<binet@cern.ch> (inst. and init. the StoreGateSvc)
//          A.Undrus<undrus@bnl.gov> (adaptation of driver class from
//          the examples of CppUnit Cookbook, generation of default
//          job options for Gaudi)
///////////////////////////////////////////////////////////////////

#include <cppunit/extensions/TestFactoryRegistry.h> 
#include <cppunit/TextTestRunner.h> 
#include <cppunit/CompilerOutputter.h> 
#include <cppunit/TextOutputter.h> 
#include <cppunit/XmlOutputter.h> 

// STL includes
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <list>

// Framework includes
#ifndef NOGAUDI
#include "TestTools/initGaudi.h"
#endif //NOGAUDI
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"

/** Simple function to instantiate and initialize the StoreGateSvc
 */
bool setupStoreGate()
{
  ISvcLocator  * m_svcLoc;
  StoreGateSvc * m_storeGate;

  /* Search for file jobOptions.txt in ../share ../test/ ../cmt */
  const int SZ = 3;
  std::string FileName="";
  std::string fn;
  std::string filename_in[SZ] = {"../cmt/CppUnit_jobOptions.txt", "../test/CppUnit_jobOptions.txt", "../share/CppUnit_jobOptions.txt"};
  std::ifstream inFile[SZ]; 

  for (int i=0; i<SZ; i++)
   {
   fn = filename_in[i];
   try
     {
       inFile[i].open(fn.c_str());
	 if (!inFile[i])
	   throw(fn);
	 FileName=fn;
     }
   catch(std::string fn)
     {
       //       std::cout << "File " << filename_in[i] << "not found" << " FileName " << std::endl;
     }

   inFile[i].close();

   } // for (int i<0 

  //  std::cout << "FileFFFF " <<FileName<< std::endl;

  // Build job options file if CppUnit_jobOptions.txt is unavailable

  if ( FileName == "" )
  {
  FileName = "../run/CppUnit_jobOptions_generated.txt";
  std::system("touch ../run/CppUnit_jobOptions_generated.txt; rm ../run/CppUnit_jobOptions_generated.txt");
  std::ofstream jobopt("../run/CppUnit_jobOptions_generated.txt");

  {    // Build a default list of options.
    std::list<std::string> opts;
    opts.push_back("#pragma print off");
    opts.push_back("ApplicationMgr.Dlls += { \"StoreGate\" };");
    //opts.push_back("ApplicationMgr.Dlls += { \"CLIDSvc\" };");
    opts.push_back("ApplicationMgr.ExtSvc += { \"ClassIDSvc\" };");
    opts.push_back("ApplicationMgr.ExtSvc += { \"StoreGateSvc\", \"StoreGateSvc/DetectorStore\", \"StoreGateSvc/HistoryStore\" };");
    opts.push_back("ApplicationMgr.ExtSvc += { \"ActiveStoreSvc\" };");
    opts.push_back("AuditorSvc.Auditors  += { \"AlgContextAuditor\"};");
    opts.push_back("StoreGateSvc.OutputLevel = 2;");
    opts.push_back("StoreGateSvc.ActivateHistory = false;");
    //opts.push_back("CLIDSvc.OutputLevel = 2;");
    opts.push_back("MessageSvc.useColors = false;");
    opts.push_back("MessageSvc.OutputLevel = 3;");
    opts.push_back("#include \"IOVSvc/IOVSvc.txt\"");
    // Build job options file from list.
    for ( std::list<std::string>::const_iterator iopt=opts.begin();
          iopt!=opts.end(); ++iopt ) 
    {
      jobopt << *iopt << std::endl;
    }
  }
  jobopt.close();
  } //if ( FileName == "" )                                                                              
  try {
    std::string fileName=FileName;
    
    /// Get StoreGateSvc
    m_svcLoc = 0;
    if (!Athena_test::initGaudi(fileName, m_svcLoc)) {
      std::cerr << "This test can not be run" << std::endl;
      return false;
    }  
    CPPUNIT_ASSERT( 0 != m_svcLoc);
    
    m_storeGate = 0;
    static const bool CREATE(true);
    bool sc = ( m_svcLoc->service( "StoreGateSvc", 
				   m_storeGate, CREATE) ).isSuccess();
    CPPUNIT_ASSERT( sc );
    CPPUNIT_ASSERT( 0 != m_storeGate );
    
    if ( false == sc || 0 == m_storeGate ) {
      std::string error = "No valid pointer to StoreGateSvc !!";
      std::cerr << error << std::endl;
      //throw std::runtime_error(error);
      return false;
    } else {
      return true;
    }
  } catch ( std::exception& e ) {
    std::cerr << "Catched : " << e.what() << std::endl;
    return false;
  }
}

/**  Main class for all the CppUnit test classes  
*/


int main( int /*argc*/, char **/* argv */)
 {
   /// Load the StoreGateSvc
   if ( false == setupStoreGate() ) {
     std::string error = "Could not setup StoreGateSvc !!";
     throw std::runtime_error(error);
   }

   /// Get the top level suite from the registry
   CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();

   /// Adds the test to the list of test to run
   CppUnit::TextTestRunner runner;
   runner.addTest( suite );

   // Change the default outputter to a compiler error format outputter 
   // uncomment the following line if you need a compiler outputter.
   //runner.setOutputter(new CppUnit::CompilerOutputter( &runner.result(),
   //                                                     std::cerr ) );

   // Change the default outputter to a xml error format outputter 
   // uncomment the following line if you need a xml outputter.
   //runner.setOutputter( new CppUnit::XmlOutputter( &runner.result(),
   //                                                    std::cerr ) );

   /// Run the tests.
   // bool wasSuccessful = runner.run_spi();
   // If you want to avoid the CppUnit typical output change the line above 
   // by the following one: 
   bool wasSuccessful = runner.run("",false,true,false);

   // Return error code 1 if the one of test failed.
   if (!wasSuccessful) return 1;

   // Uncomment the next line if you want to integrate CppUnit with Oval
   //  std::cout <<"[OVAL] Cppunit-result ="<<!wasSuccessful<<"\n" ;

   return 0;
 }
