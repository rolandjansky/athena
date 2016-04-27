/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream.h>
#include <ostream.h>



#include <is/infoany.h>
#include <is/infoT.h>
#include <is/infodictionary.h>
#include <is/infoiterator.h>
#include <is/inforeceiver.h>
#include <is/serveriterator.h>
#include <ipc/partition.h>
#include <is/infodocument.h>
#include <is/infodynany.h>

#include <cmdl/cmdargs.h>
#include <ipc/core.h>
#include <owl/timer.h>

int getRunNumber2(std::string partition_name,std::string server){
  int run=-99;
  std::cout<<"  try to getRunNumber2 from "<<partition_name<<"."<<server<<"."<<"  by looking for attribute with name run_number "<<std::endl;
  try{ 
    
    IPCPartition partition( partition_name );
    ISInfoDictionary dict(partition);
    ISInfoDynAny ida2;
    std::string name = server;
    name += ".RunParams";

    dict.getValue( name, ida2 );
    ISInfoDocument::Attribute helpme= ida2.getAttributeDescription(partition,"run_number");
    run  = (int)  ida2.getAttributeValue<unsigned int>( partition,"run_number");
    cout<<"  found  attribute "<<helpme.description()<<"  name "<<helpme.name()<<" value   ?"<<run<<endl;
    
  }catch(...){
    run=-99;
  }
  return run;
}
  
int getRunNumber(std::string partition_name,std::string server,std::string crit){

  std::cout<<"  try to get run number from "<<partition_name<<"."<<server<<"."<<crit<<"  by using the first attribute "<<std::endl;
  try{
    
    IPCPartition partition( partition_name );
    ISInfoDictionary dict(partition);
    
    
    ISInfoInt runnumber;
    
    ISInfoIterator ii( partition, server,ISCriteria( crit));
    
    
    int run=-99;
    while( ii() )
      {
	
	ISInfoAny isa;
	ii.value( isa );
	
	int attr_number=isa.countAttributes();
	
	for (int i = 0; i < attr_number; i++ ) {
	  switch ( isa.getAttributeType() ){
	  case ISType::U32:
	    if ( isa.isAttributeArray() )
	      {
	        std::vector<unsigned long> value;
		isa >> value;
		//for ( size_t ii = 0; ii < value.size(); ii++){}
	      }
	    else
	      {
		int value;
		isa >> value;
		//std::cout <<" attribute loop "<<i<<"  value "<< value;//<<" name ? "<<isa.name();
		if(i==0)run=value;
	      }
	    //std::cout << std::endl;
	    
	    break;
	    //default:
	    //std::cout<<" Attribute Type is not what I want "<<isa.getAttributeType()<<std::endl;
	  }
	}
      }
    std::cout<<"  return from here...with runnr "<<run<<std::endl;
    return run;
  }catch(...){
    std::cout<<"  could not receive runnumber ! "<<std::endl;
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
  CmdArgStr is_criteria( 'c', "criteria","is_criteria", "ISCriteria ");
  CmdArgStr method('m',"method","method","Method");
  
  CmdLine cmd( *argv, &partition_name, &server_name, &is_criteria, &method, 0);
  
  
  CmdArgvIter arg_iter( --argc, ++argv );
  cmd.description( "OH histogram publisher utility" );
  cmd.parse( arg_iter );
  

  //partition MyTestPartition
  //server_name MyTest2.LB_CTP 

  if( partition_name.isNULL( ) ) partition_name = "MyTestPartition";
  if( server_name.isNULL( ) ) server_name = "RunParams";
  if( is_criteria.isNULL( ) ) is_criteria = "RunParams*";
  if(method.isNULL() ) method="1";
  int runNumberMethod = atoi(method);
  
  cout<<"try to find runnumber in     "<<partition_name<<"  "<<server_name<<"  "<<is_criteria<<endl;
  std::string testPartition((const char*) partition_name);
  std::string server((const char*) server_name);
  std::string crit((const char*) is_criteria);
  int runnr=runNumberMethod ==1 ? getRunNumber(testPartition,server,crit) : getRunNumber2(testPartition,server);
  cout<<"returned Run number  "<<runnr<<endl;
  return 0;
}


