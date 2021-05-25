/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_ARGS2CONTAINER
#define UTILITIES_COLLECTION_ARGS2CONTAINER

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/***************************************************************

Args2Container is a simple extension of a vector of CmdLineArgs2

It has a name and a desc. Name is normally the program name.
It also adds three utility methods.

printHelp(verbose) :
   - (verbose = false) print out names of ArgQuals 
     for all contained pointers
   - (verbose = true) print out names _and_ desc of ArgQuals 
     for all contained pointers

checkValid() : Loop over pointers and call valid() method

evalArgs(argc,argv) : save time/errors by calling evalArgs 
   on the container rather than the individual CmdLineArg2's

bool doXML() : whether to write out an xml file with the argv 
   results classified by CmdLineArgs2

WIP pieces for XML reading/state recovery
-------------------------------------------
void useXMLInput(Args2Container::XMLOption)

XMLOption actions:

	OVERRIDE (NI):	evalArgs
			parse XML
			Pt CLA2 in XML at non-argv
	APPEND (NI):	evalArgs
			parse XML
			if CLA2 missing pt it at non-argv
	FILEONLY:	parse XML
			pt CLA2 in XML at non-argv

* Note that the minimum unit is CLA2, e.g. you can't take -src 
from the command line and -srcconnect from XML for SrcInfo

***************************************************************/

#include "CoralBase/MessageStream.h"

namespace pool
{
   class Args2Container : public std::vector<CmdLineArgs2*>
   {
  public: 
     enum XMLVersion {V1=0,V2,NUM_VERSIONS}; 
     enum XMLOption {FILEONLY=0,OVERRIDE,APPEND}; 

     Args2Container(const std::string& name="Args2Container",
		    bool doxml = false,
		    coral::MessageStream* log = 0); 

     void init();

     void printHelp(bool verbose) const;
     bool checkValid() const;

     // Pass argv to all cliarg's
     bool evalArgs(std::vector<std::string>& argv);
     bool evalArgs( int argc, const char *argv[] );

     // Pass argv to specified cliarg
     bool evalArgs(const std::string& cliarg, std::vector<std::string>& argv);

     bool doXML() const {return m_xml;}
     void setXMLOption(Args2Container::XMLOption a) {
	m_xmlOpt = a;
	m_xml=true; // if they're setting it, assume they're using it.
     }

     std::stringstream desc;

     coral::MessageStream  &m_log;
     
  private:

     // input xml methods   
     std::vector<std::string> fillCmdLineArgsFromXML(std::string file);

     // output xml methods
     void writeXMLContent(std::vector<std::string>& argv);

     std::string	m_name;
     bool		m_xml;
     XMLOption		m_xmlOpt;
     XMLVersion		m_ver;
     CmdLineArgs2	m_a2c_cla2;
  
     std::string	m_xFileName;

     std::vector< std::string > m_argv;
   };
   
}// end pool namespace

#endif  // UTILITIES_COLLECTION_ARGS2CONTAINER









