/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////
//    create_info.cc
//
//    Test application for the IS library
//
//    Sergei Kolos,  January 2002
//
//    description:
//          Test the functionality of the ISNamedInfo class
//      
////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <ers/ers.h>
#include <cmdl/cmdargs.h>
#include <ipc/core.h>

#include <is/PersonNamed.h>
#include <is/EmployeeNamed.h>
#include <is/infodynany.h>
//#include "TestRunNumber.h"
//#include "IS_LB.h"
#include "TestRunNumber2.h"


void print_dynany( std::ostream & out, const ISInfoDynAny & any )
{
    for ( size_t i = 0; i < any.getAttributesNumber(); ++i )
    {
	if ( any.isAttributeArray( i )  )
	{
	    switch ( any.getAttributeType( i ) )
	    {
		case ISType::Boolean	: { std::vector<bool> value = any.getAttributeValue<std::vector<bool> >( i ); out << value << " "; break; }
		case ISType::S8		: { std::vector<char> value = any.getAttributeValue<std::vector<char> >( i ); out << value << " "; break; }
		case ISType::U8		: { std::vector<unsigned char> value = any.getAttributeValue<std::vector<unsigned char> >( i ); out << value << " "; break; }
		case ISType::S16	: { std::vector<short> value = any.getAttributeValue<std::vector<short> >( i ); out << value << " "; break; }
		case ISType::U16	: { std::vector<unsigned short> value = any.getAttributeValue<std::vector<unsigned short> >( i ); out << value << " "; break; }
		case ISType::S32	: { std::vector<int> value = any.getAttributeValue<std::vector<int> >( i ); out << value << " "; break; }
		case ISType::U32	: { std::vector<unsigned int> value = any.getAttributeValue<std::vector<unsigned int> >( i ); out << value << " "; break; }
		case ISType::S64	: { std::vector<int64_t> value = any.getAttributeValue<std::vector<int64_t> >( i ); out << value << " "; break; }
		case ISType::U64	: { std::vector<uint64_t> value = any.getAttributeValue<std::vector<uint64_t> >( i ); out << value << " "; break; }
		case ISType::Float	: { std::vector<float> value = any.getAttributeValue<std::vector<float> >( i ); out << value << " "; break; }
		case ISType::Double	: { std::vector<double> value = any.getAttributeValue<std::vector<double> >( i ); out << value << " "; break; }
		case ISType::String	: { std::vector<std::string> value = any.getAttributeValue<std::vector<std::string> >( i ); out << value << " "; break; }
		case ISType::Date	: { std::vector<OWLDate> value = any.getAttributeValue<std::vector<OWLDate> >( i ); out << value << " "; break; }
		case ISType::Time	: { std::vector<OWLTime> value = any.getAttributeValue<std::vector<OWLTime> >( i ); out << value << " "; break; }
		case ISType::InfoObject : { std::vector<ISInfoDynAny> value = any.getAttributeValue<std::vector<ISInfoDynAny> >( i );
                			  for( size_t j = 0; j < value.size(); ++j ) print_dynany( out, value[j] ); break; }
		default			: ERS_ASSERT( false ); //must not happen
	    }
	}
	else
	{
	    switch ( any.getAttributeType( i ) )
	    {
		case ISType::Boolean	: { bool value = any.getAttributeValue<bool>( i ); out << value << " "; break; }
		case ISType::S8		: { char value = any.getAttributeValue<char>( i ); out << value << " "; break; }
		case ISType::U8		: { unsigned char value = any.getAttributeValue<unsigned char>( i ); out << value << " "; break; }
		case ISType::S16	: { short value = any.getAttributeValue<short>( i ); out << value << " "; break; }
		case ISType::U16	: { unsigned short value = any.getAttributeValue<unsigned short>( i ); out << value << " "; break; }
		case ISType::S32	: { int value = any.getAttributeValue<int>( i ); out << value << " "; break; }
		case ISType::U32	: { unsigned int value = any.getAttributeValue<unsigned int>( i ); out << value << " "; break; }
		case ISType::S64	: { int64_t value = any.getAttributeValue<int64_t>( i ); out << value << " "; break; }
		case ISType::U64	: { uint64_t value = any.getAttributeValue<uint64_t>( i ); out << value << " "; break; }
		case ISType::Float	: { float value = any.getAttributeValue<float>( i ); out << value << " "; break; }
		case ISType::Double	: { double value = any.getAttributeValue<double>( i ); out << value << " "; break; }
		case ISType::String	: { std::string value = any.getAttributeValue<std::string>( i ); out << value << " "; break; }
		case ISType::Date	: { OWLDate value = any.getAttributeValue<OWLDate>( i ); out << value << " "; break; }
		case ISType::Time	: { OWLTime value = any.getAttributeValue<OWLTime>( i ); out << value << " "; break; }
		case ISType::InfoObject : { ISInfoDynAny value = any.getAttributeValue<ISInfoDynAny>( i ); print_dynany( out, value ); break; }
		default			: ERS_ASSERT( false ); //must not happen
	    }
	}
    }
    out << std::endl;
}

void set_values( const ISInfoDynAny & any )
{
    for ( size_t i = 0; i < any.getAttributesNumber(); ++i )
    {
	if ( any.isAttributeArray( i )  )
	{
	    switch ( any.getAttributeType( i ) )
	    {
		case ISType::Boolean	: { any.setAttributeValue<std::vector<bool> >( i, std::vector<bool>() ); break; }
		case ISType::S8		: { any.setAttributeValue<std::vector<char> >( i, std::vector<char>()); break; }
		case ISType::U8		: { any.setAttributeValue<std::vector<unsigned char> >( i, std::vector<unsigned char>()); break; }
		case ISType::S16	: { any.setAttributeValue<std::vector<short> >( i, std::vector<short>()); break; }
		case ISType::U16	: { any.setAttributeValue<std::vector<unsigned short> >( i, std::vector<unsigned short>()); break; }
		case ISType::S32	: { any.setAttributeValue<std::vector<int> >( i, std::vector<int>()); break; }
		case ISType::U32	: { any.setAttributeValue<std::vector<unsigned int> >( i, std::vector<unsigned int>()); break; }
		case ISType::S64	: { any.setAttributeValue<std::vector<int64_t> >( i, std::vector<int64_t>()); break; }
		case ISType::U64	: { any.setAttributeValue<std::vector<uint64_t> >( i, std::vector<uint64_t>()); break; }
		case ISType::Float	: { any.setAttributeValue<std::vector<float> >( i, std::vector<float>()); break; }
		case ISType::Double	: { any.setAttributeValue<std::vector<double> >( i, std::vector<double>()); break; }
		case ISType::String	: { any.setAttributeValue<std::vector<std::string> >( i, std::vector<std::string>()); break; }
		case ISType::Date	: { any.setAttributeValue<std::vector<OWLDate> >( i, std::vector<OWLDate>() ); break; }
		case ISType::Time	: { any.setAttributeValue<std::vector<OWLTime> >( i, std::vector<OWLTime>() ); break; }
		case ISType::InfoObject : { std::vector<ISInfoDynAny> value = any.getAttributeValue<std::vector<ISInfoDynAny> >( i );
					    for( size_t j = 0; j < value.size(); ++j ) set_values( value[j] ); 
                                            any.setAttributeValue<std::vector<ISInfoDynAny> >( i, value );    
					    break; }
		default			: ERS_ASSERT( false ); //must not happen
	    }
	}
	else
	{
	    switch ( any.getAttributeType( i ) )
	    {
		case ISType::Boolean	: { any.setAttributeValue<bool>( i, true ); break; }
		case ISType::S8		: { any.setAttributeValue<char>( i, i ); break; }
		case ISType::U8		: { any.setAttributeValue<unsigned char>( i, i ); break; }
		case ISType::S16	: { any.setAttributeValue<short>( i, i ); break; }
		case ISType::U16	: { any.setAttributeValue<unsigned short>( i, i ); break; }
		case ISType::S32	: { any.setAttributeValue<int>( i, i ); break; }
		case ISType::U32	: { any.setAttributeValue<unsigned int>( i, i ); break; }
		case ISType::S64	: { any.setAttributeValue<int64_t>( i, i ); break; }
		case ISType::U64	: { any.setAttributeValue<uint64_t>( i, i ); break; }
		case ISType::Float	: { any.setAttributeValue<float>( i, i ); break; }
		case ISType::Double	: { any.setAttributeValue<double>( i, i ); break; }
		case ISType::String	: { any.setAttributeValue<std::string>( i, "TEST SET VALUE" ); break; }
		case ISType::Date	: { any.setAttributeValue<OWLDate>( i, OWLDate() ); break; }
		case ISType::Time	: { any.setAttributeValue<OWLTime>( i, OWLTime() ); break; }
		case ISType::InfoObject : { ISInfoDynAny tmp( any.getAttributeValue<ISInfoDynAny >( i ).type() ); 
                			    set_values( tmp ); any.setAttributeValue<ISInfoDynAny >( i, tmp ); break; }
		default			: ERS_ASSERT( false ); //must not happen
	    }
	}
    }
}

int main(int argc, char ** argv)
{
    try {
        IPCCore::init( argc, argv );
    }
    catch( daq::ipc::Exception & ex ) {
    	is::fatal( ex );
    }
    
    // Declare arguments
    CmdArgStr    partition_name('p', "partition", "partition-name", "partition to work in.");
    CmdArgStr    person_name('P', "names", "name", "name of information to create.", CmdArg::isREQ);
    CmdArgStr    employee_name('E', "names", "name", "name of information to create.", CmdArg::isREQ);
    CmdArgStr    server_name('n', "server", "server-name", "server to work with.",CmdArg::isREQ);
 
    // Declare command object and its argument-iterator
    CmdLine  cmd(*argv, &partition_name, &server_name, &person_name, &employee_name, NULL);
    CmdArgvIter  arg_iter(--argc, ++argv);
 
    cmd.description("This program implements fanctionality tests for the Information Service.\n"
        "It creates, updates and deletes information objects.");
  
    // Parse arguments
    cmd.parse(arg_iter);

    IPCPartition      p(partition_name);
    
    std::string name2(server_name);
    name2 += ".RunParams";
    TestRunNumber2 rr(p,name2);
    rr.runnumber=12345;
    rr.name="run_number";
    
    try
    {
      ISInfoDictionary id( p );
      rr.checkin("hello");
      std::cout << "Checkin successfull " << std::endl;
      
      //
      ISInfoDynAny ida1;
      id.getValue( name2, ida1 );
      std::cout << "getValue for the " << name2 << " successfull " << std::endl << ida1 << std::endl;
      
    }
    
    catch( daq::is::Exception & ex )
      {
    	is::fatal( ex );
    }
    
    return 0;
}
