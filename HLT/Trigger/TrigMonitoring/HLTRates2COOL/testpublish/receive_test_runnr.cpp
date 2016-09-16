/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream.h>
#include <ostream.h>


#include "is/infoany.h"
#include <is/infoT.h>
#include <is/infodictionary.h>
#include <is/infoiterator.h>
#include <is/inforeceiver.h>
#include <is/serveriterator.h>
#include <ipc/partition.h>
#include <is/infodocument.h>

#include <cmdl/cmdargs.h>
#include <ipc/core.h>
#include <owl/timer.h>


int getRunNumber(std::string partition_name,std::string server){

  try{
    IPCPartition partition( partition_name );
    ISInfoDictionary dict(partition);
    
    
    ISInfoInt runnumber;
    
    ISInfoIterator ii( partition, server,ISCriteria( ".*" ));
    
    
    int run=-99;
    while( ii() )
      {
	
	ISInfoAny isa;
	ii.value( isa );
	
	
	std::string  object_name(ii.name());  
	std::string::size_type loc = object_name.find( "LB_CTP_", 0 );
	std::string helpstr=object_name.substr(loc+7).c_str();
	std::string::size_type loc2= helpstr.find("_",0);
	int irun=atoi(helpstr.substr(0,loc2).c_str());
	run =  irun > run ? irun : run;
	
      }
    cout<<"highest published  runnumber "<<run<<endl;
    return run;
  }catch(...){
    cout<<" could not receive runnumber ! "<<endl;
    return -99;
  }
}
using namespace std;
int main( int argc, char ** argv ){
  
  IPCCore::init( argc, argv );
    
  // Get command line arguments
  CmdArgStr partition_name( 'p', "partition", "partition_name",
			    "Partition name");
  CmdArgStr server_name( 's', "server", "server_name",
			 "OH (IS) Server name");
  
  CmdLine cmd( *argv, &partition_name, &server_name, 0);
  
  CmdArgvIter arg_iter( --argc, ++argv );
  cmd.description( "OH histogram publisher utility" );
  cmd.parse( arg_iter );
  

  //partition MyTestPartition
  //server_name MyTest2.LB_CTP 

  if( partition_name.isNULL( ) ) partition_name = "MyTestPartition";
  if( server_name.isNULL( ) ) server_name = "MyTest2.LB_CTP";
  
  cout<<" get latest Run Number published to    "<<partition_name<<"  "<<server_name<<endl;
  std::string testPartition((const char*) partition_name);
  std::string server((const char*) server_name);
  
  int runnr=getRunNumber(testPartition,server);
  return 0;
}


