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

#include "MyInfo.h"
#include "IS_LB.h"

using namespace std;
int main( int argc, char ** argv )
{

  string testPartition = "MyTestPartition";
  string subscription = "MyTest.LB_";

  
  cout << "Publishing data for new IS_LB 2007/08/27" << endl;
  IPCCore::init( argc, argv );

  IPCPartition partition( testPartition );

  ISInfoDictionary dict( partition );
  
  char pubname[100];

  
  IS_LB lvl1info;
  ISInfo *ptrlvl1;
  ptrlvl1= & lvl1info;

	for ( int lb=0; lb<1000000; lb++ )
	{

		lvl1info.setLB( lb );
	
		cout << "LB: " << lb << endl;

		try
		{
			sprintf( pubname, "%s%d", subscription.c_str(), lb );
			dict.insert( pubname , * ptrlvl1 );
			cout << " published lvl1 example " << pubname << endl;
   		}
	    catch(...)
		{
			sprintf( pubname, "%s%d", subscription.c_str(), lb );
			dict.update( pubname , * ptrlvl1 );
			cout<<" lvl1 example was already published, update it!" << pubname << endl;
	    }

		int updateTimes = random()%10 + 1;
		for(int i=0;i<updateTimes;i++)
		{
			cout << endl << "loopp update no. " << i << endl;
		
			//update is_lb info
			lvl1info.increase_counters( 10 );
	
			sprintf( pubname, "%s%d", subscription.c_str(), lb );
			dict.update( pubname , * ptrlvl1, true );
			cout << " update lvl1 example " << pubname << endl;

			sleep( 2 );
		}
	}

  return 0;
}
