/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////
//                                                     
//NAME:     JobOpttriggerapp.cpp 
//PACKAGE:  TrigConfStorage
//                                                     
//AUTHOR:   J.Haller (CERN)	Johannes.Haller@cern.ch 
//CREATED:  06. Feb. 2006   
//                                                     
//PURPOSE:
//
//
//////////////////////////////////////////////////////////////////////

#include "TrigConfStorage/IJobOptionTableLoader.h"

#include "TrigConfStorage/JobOptionTableLoader.h"

#include "TrigConfJobOptData/JobOptionTable.h"

#include "TrigConfStorage/StorageMgr.h"
#include "TrigConfHLTData/HLTFrame.h"

#include <iostream>
#include <memory>
#include <ctime>

using std::cout;
using std::endl;

int main( int, char** )

{

  TrigConf::StorageMgr* sm = new TrigConf::StorageMgr("mysql://pcatr13.cern.ch/TriggerTestLST", "triguser", "trigconfig");



  try
  {
    clock_t start,finish;
    double time =0;

    start = clock();

    TrigConf::JobOptionTable jot;
    jot.setSuperMasterTableId(1);
    sm->jobOptionTableLoader().load( jot );
    jot.print(); 

    finish = clock();
    time = (double(finish)-double(start))/CLOCKS_PER_SEC;
    std::cout << "Leaving triggerapp1 " << std::endl;
    
    std::cout << "time needed\t" << time << std::endl;
  }

  catch( const std::exception& e ) {
      std::cerr << "Caught standard C++ exception: " << e.what() << std::endl;
      return 1;
  }

  catch( ... ) {
      std::cerr << "Caught unknown C++ exception!" << std::endl;
      return 2;
  }


  try {

    delete sm;
  
    sm = new TrigConf::StorageMgr("mysql://pcatr13.cern.ch/TriggerTestLST", "triguser", "trigconfig");

    TrigConf::HLTFrame& hltFrame = * TrigConf::HLTFrame::instance();
    hltFrame.setUseNextRun( false );
    hltFrame.set_superMaster_id( 1 );

    sm->hltFrameLoader().load( hltFrame );
  }
  catch( const std::exception& e ) {
      std::cerr << "Caught standard C++ exception: " << e.what() << std::endl;
      return 1;
  }

  catch( ... ) {
      std::cerr << "Caught unknown C++ exception!" << std::endl;
      return 2;
  }

  return true; 
}

