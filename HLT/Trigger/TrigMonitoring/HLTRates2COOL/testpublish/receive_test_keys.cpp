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

#include <cmdl/cmdargs.h>
#include <ipc/core.h>
#include <owl/timer.h>


int getRunNumber(std::string partition_name,std::string server,std::string crit){

  int superMasterKey=-99;
  int l1MasterKey=-99;
  int l1PrescaleKey=-99;
    
  try{
    IPCPartition partition( partition_name );
    ISInfoDictionary dict(partition);
    ISInfoIterator ii( partition, server,ISCriteria( crit));
    
    
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
		
		//for ( size_t ii = 0; ii < value.size(); ii++){
		// std::cout <<" attribute loop "<<i<<"  attribute array loop "<<ii<<"  : "<< value[ii] << " ";
		//}
	      }
	    else
	      {
		int value;
		isa >> value;
		std::cout <<" attribute loop "<<i<<"  value "<< value;//<<" name ? "<<isa.name();
		if(i==0)superMasterKey=value;
		if(i==1)l1MasterKey=value;
		if(i==2)l1PrescaleKey=value;
	      }
	    std::cout << std::endl;
	    
	    break;
	    //default:
	    //std::cout<<" Attribute Type is not what I want "<<isa.getAttributeType()<<std::endl;
	  }
	}
      }
    cout<<"return from here...superMasterKey "<<superMasterKey<<endl;
    return superMasterKey;
  }catch(...){
    cout<<" could not receive TrigConfKeys ! "<<endl;
    return superMasterKey;
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
  CmdLine cmd( *argv, &partition_name, &server_name, &is_criteria, 0);
  
  CmdArgvIter arg_iter( --argc, ++argv );
  cmd.description( "OH histogram publisher utility" );
  cmd.parse( arg_iter );
  

  //partition MyTestPartition
  //server_name MyTest2.LB_CTP 

  if( partition_name.isNULL( ) ) partition_name = "MyTestPartition";
  if( server_name.isNULL( ) ) server_name = "MyTest2.LB_CTP";
  if( is_criteria.isNULL( ) ) is_criteria = "TrigConfKeys.*";
  cout<<" try to find TrigConfKeys in     "<<partition_name<<"  "<<server_name<<endl;
  std::string testPartition((const char*) partition_name);
  std::string server((const char*) server_name);
  std::string crit((const char*) is_criteria);
  int runnr=getRunNumber(testPartition,server,crit);
  cout<<" returned ! "<<runnr<<endl;
  return 0;
}


