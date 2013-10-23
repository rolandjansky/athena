/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file publish_test_data.cpp
 * @author Andrzej Dworak andrzej.dworak@cern.ch
 * @date 13/08/2007
 * @brief Dummy data ganarator. Publishing through IS.
 */

#include <iostream.h>
#include <ipc/core.h>

#include "is/infoany.h"
#include <is/infoT.h>
#include <is/infodictionary.h>
#include <is/infoiterator.h>
#include <is/inforeceiver.h>
#include <is/serveriterator.h>
#include <ipc/partition.h>
#include "is/infodocument.h"


using namespace std;
int main( int argc, char ** argv )
{

  string testPartition = "MyTestPartition";
  string subscription0 = "MyTest2.LB_";

  
  cout << "Publishing data for new IS_LB 2007/08/27" << endl;
  IPCCore::init( argc, argv );

  IPCPartition partition( testPartition );
  ISInfoDictionary dict( partition );
  
  char pubname[100];
  int npublish=0;
  
  for ( int irun=102; irun<110; irun++ )
    {
      ISInfoInt runNr(irun);
      for(int lb=0;lb<100;lb++){
	
	cout << " new example runnr  " << runNr <<" lb="<<lb<< endl;
	
	try {
	  sprintf( pubname, "MyTest2.LB_CTP_%d_%d", irun, lb);
	  dict.insert( pubname , runNr );   
	}
	catch(...){
	  sprintf( pubname, "MyTest2.LB_CTP_%d_%d", irun, lb);
	  dict.update( pubname , runNr );   
	}
	npublish++;
	sleep(3);
      }
    }
	
  //test with ISInfoDynAny 
  // IsInfoDynAny ???
  
  return 0;
}
