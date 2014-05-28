/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* print_algorithms.cpp
     Author Haleh Hadavand
*/
#include <boost/program_options.hpp>

#include <dqm_core/LibraryManager.h>
#include <dqm_core/AlgorithmManager.h>
#include <iostream>
#include <ers/ers.h>

int main(int argc, char ** argv) 
{
    boost::program_options::options_description description( "Options" );
    
    description.add_options()
	( "help,h", "produce help message" )
	( "library,l", boost::program_options::value<std::string>()->default_value( "libdqm_algorithms.so", "libdqm_algorithms.so" ), "names of DQM library" );
    
    boost::program_options::variables_map arguments;
    try {
	boost::program_options::store( boost::program_options::parse_command_line( argc, argv, description ), arguments );
	boost::program_options::notify( arguments );
    }
    catch ( boost::program_options::error & ex )
	{
	    std::cerr << ex.what() << std::endl;
	    description.print( std::cout );
	    return 1;
	}
    
    if ( arguments.count("help") ) {
	std::cout << "Test application of the 'dqm' package" << std::endl;
	description.print( std::cout );
	return 0;
    }
    
    if ( !arguments.count("library") ) {
	std::cerr << "'library' option is not given" << std::endl;
	description.print( std::cout );
	return 2;
    }
    
    std::string library	= arguments["library"].as<std::string>();
    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // Uuu-f ... at last we have finished with parsing the command line
    //////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    try
    {
	dqm_core::LibraryManager::instance().loadLibrary( library );
    }
    catch( dqm_core::Exception & ex )
    {
    	ers::fatal( ex );
        return -1;
    }
	int count=0;
    std::map<std::string, dqm_core::Algorithm*> algslist = dqm_core::AlgorithmManager::instance().getAlgorithmMap();
    std::map<std::string,dqm_core::Algorithm*>::iterator iter;
    for (iter=algslist.begin();iter!=algslist.end();iter++){
        ++count;
        std::cout<<"*********************************************************************************************"<<std::endl;
	std::cout<<"Algorithm Name '"<<iter->first<<"'"<<std::endl;
        std::cout<<"---------------------------------------------------------------------------------------------"<<std::endl;
        iter->second->printDescription();
    }
    std::cout<<"There are "<<count<<" algorithm(s) in lib "<<library<<std::endl;
}

