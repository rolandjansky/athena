/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "GoogleTestTools/InitGaudiGoogleTest.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"

#include <iostream>
#include <fstream>

namespace Athena_test {

  InitGaudiGoogleTest::InitGaudiGoogleTest( MSG::Level level ) :
    // create a new ApplicationMgr w/o singleton behaviour
    theApp( Gaudi::createApplicationMgrEx( "GaudiCoreSvc", 
					   "ApplicationMgr" ) ),
    propMgr( theApp ),
    svcLoc( theApp ),
    svcMgr( theApp ),
    m_msgLevel( level ) {
    EXPECT_TRUE( theApp != nullptr );
    EXPECT_TRUE( propMgr.isValid() );
    EXPECT_TRUE( svcLoc.isValid() );
    EXPECT_TRUE( svcMgr.isValid() );
    // set the new ApplicationMgr as instance in Gaudi
    Gaudi::setInstance( theApp );
    EXPECT_TRUE( propMgr->setProperty( "JobOptionsType", "NONE" ).isSuccess() );
    EXPECT_TRUE( propMgr->setProperty( "OutputLevel", 
				       std::to_string( m_msgLevel ) ).isSuccess() );
    bool lconfig= false;
    // Configure ApplicationMgr
    if( m_msgLevel == MSG::ALWAYS ) {
      // Redirect cout to /dev/null around configure() to get rid of Gaudi 
      // messages in unit tests
      std::streambuf* coutbuf= std::cout.rdbuf(); // save std::cout buf
      std::ofstream out( "/dev/null" );
      std::cout.rdbuf( out.rdbuf() ); // redirect std::cout
      lconfig= theApp->configure().isSuccess();
      std::cout.rdbuf( coutbuf ); // restore std::cout
    }
    else {
      // just configure()
      lconfig= theApp->configure().isSuccess();
    }
    EXPECT_TRUE( lconfig ) << "InitGaudiGoogleTest: theApp->configure() failure";
    EXPECT_TRUE( theApp->initialize().isSuccess() );
  }

  // Finalize and terminate Gaudi, more to make sure that things
  // went clean than anything else because we are throwing away the
  // ApplicationMgr anyway
  InitGaudiGoogleTest::~InitGaudiGoogleTest() {
    EXPECT_TRUE( theApp->finalize().isSuccess() );
    EXPECT_TRUE( theApp->terminate().isSuccess() );
  }

}

