/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/**********************************************************

CmdLineArgs2 is an extension of a map intended to map 
   qualifiers (e.g. -help) of a CLI to the index of 
   the argv[]. It's state is determined by a list of 
   ArgQual's/QualList. It then uses evalArgs to apply 
   those qualifiers to an argv[].

 - Qualifier -help is added in constructor
 - methods described in comments in definition

Example of Usage:

      Args2Container argsVec(thisProgram);

      QualList markers;
      markers.insert( make_pair("-somequal", ArgQual()) );
      markers["-somequal"].desc << "this is a qualifier with default=this";

      CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      cmdLineArgs.evalArgs(argc,argv);
      argsVec.push_back(&cmdLineArgs);  // Add it to the list

      ...

**********************************************************/
using namespace std;
using namespace pool;

#include <iostream>

CmdLineArgs2::CmdLineArgs2( const std::string& name ) 
  : m_argsfine( false ),
    m_ignoreMissingArgs( false ),
    m_id(name)
{	
   initTypes();
}


CmdLineArgs2::CmdLineArgs2( QualList& quals, const std::string& name ) 
  : m_argsfine( false ),
    m_ignoreMissingArgs( false ),
    m_quals(quals),
    m_id(name)
{	
   initTypes();
}

void
CmdLineArgs2::initTypes()
{
   // Is there a way to do this with static something?
   m_allowedTypes.push_back("RelationalCollection");
   m_allowedTypes.push_back("RootCollection");
   m_allowedTypes.push_back("PHYSICAL_NAME");
   m_allowedTypes.push_back("LOGICAL_NAME");
   m_allowedTypes.push_back("GUID");
}


bool 
CmdLineArgs2::hasQual( const std::string& arg ) const
{
  return ( find(arg) != end() );
}


bool
CmdLineArgs2::getOpt( const std::string& opt, std::string &optval ) const
{
   const_iterator i = find(opt);
   if( i == end() )
      return false;
   optval = m_argv[i->second.first];
   return true;
}

bool
CmdLineArgs2::getOpt( const std::string& opt, int &optval ) const
{
   const_iterator i = find(opt);
   if( i == end() )
      return false; 
   istringstream  str( m_argv[i->second.first] );
   str >> optval;
   return true;
}

bool
CmdLineArgs2::getOpts( const std::string& opt, std::vector<std::string> &optvalvec ) const
{
   const_iterator iter = find(opt);
   if( iter == end() )
      return false;
   optvalvec.clear();
   for( int i = iter->second.first; i<iter->second.second; i++ ) {
      optvalvec.push_back( m_argv[i] );
   }
   return true;
}


bool
CmdLineArgs2::evalArgs(std::vector<std::string>& argv) 
{
   m_argv = argv;
   m_argsfine = true;
   int argc=argv.size();
   // Markers are info stored in the base class map
   // Look for markers (anything beginning with '-') in argv and record their position
   vector< std::pair<int, string> > markPos;
   vector<string> marks;
   for (int i=1; i<argc; ++i) {
      if (argv[i][0]=='-') {
         markPos.push_back(std::make_pair(i,argv[i]));
         marks.push_back(argv[i]);
      }
   }

   // sort the marks by their position
   std::sort(markPos.begin(),markPos.end());

   // Check for required args
   for( QualList::iterator quit = m_quals.begin(); quit != m_quals.end(); ++quit ) {
      if( quit->second.required && 
          std::find(marks.begin(),marks.end(),quit->first) == marks.end() )
      {
	 if( !m_ignoreMissingArgs )
	    std::cerr << "Must specify value for " << quit->first << std::endl;
         m_argsfine = false;
      }
   }

   // build a map of the begin,end positions for all marks
   vector< std::pair<int, string> >::iterator itmarks=markPos.begin();
   while (itmarks != markPos.end()) {
      std::string markerName = itmarks->second;
      int first = itmarks->first+1;
      ++itmarks;
      int last = (itmarks==markPos.end())? argc : itmarks->first;
      insert( make_pair( markerName, make_pair(first,last) ) );
   }

   // Now check that they obey the rules and set flag accordingly
   iterator mit = begin();
   while( mit != end() ) {
      string qual = mit->first;
      // check only options that are in the qual list
      if( m_quals.find(qual) != m_quals.end() ) {
         int npars   = m_quals[qual].argsize;
         int diff = (*this)[qual].second - (*this)[qual].first;
         if (m_quals[qual].multiarg) {
            if ( abs(diff) < npars || diff%npars != 0) {
               std::cerr << endl << " ERROR Wrong number of parameters" << std::endl;
               std::cerr << qual << " needs " << npars << " parameters"
                         << ", Found " << diff << std::endl;
               std::cerr << "MORE INFO" << std::endl;
               m_quals[qual].print(std::cerr);
               std::cerr << std::endl;
               m_argsfine = false;
            }
         }
         else {
            if (abs(diff)!=npars) {
               std::cerr << endl << " ERROR Wrong number of parameters" << std::endl;
               std::cerr << qual << " needs " << npars << " parameters"
                         << ", Found " << diff << std::endl;
               std::cerr << "MORE INFO" << std::endl;
               m_quals[qual].print(std::cerr);
               std::cerr << std::endl;
               m_argsfine = false;
            }
         }
      }
      //else std::cout << "CmdLineArgs: ERROR Unknown qualifier " << markerName << std::endl;      
      ++mit;
   }

   return m_argsfine;
}

unsigned int
CmdLineArgs2::nArgs(std::string name)
{
   if( !valid() ) {
      std::cerr << "nArgs called for invalid CmdLineArgs2 class" << std::endl;
      return 0;
   }
   CmdLineArgs2::iterator it = find(name);
   if( it == end() )
      return 0;
   return abs(it->second.second - it->second.first)/m_quals[name].argsize;
}

