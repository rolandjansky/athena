/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_CMDLINEARGS2
#define UTILITIES_COLLECTION_CMDLINEARGS2

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "CollectionUtilities/ArgQual.h"

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

namespace pool
{
   class Args2Container;
   
   class CmdLineArgs2 : public std::map<std::string,std::pair<int,int> >
   {
  public:
     /// Constructor.	
     CmdLineArgs2( std::string name="General" );
     CmdLineArgs2( QualList&, std::string name="General");
 
     virtual ~CmdLineArgs2() {}

     /// init
     virtual void 	init();

     /// Check if argument was found in the argv[]
     bool 		hasQual( const std::string arg ) const;

     /// check if option was present and return its (first) value in optval(!)
     bool 		getOpt( const std::string opt, std::string &optval ) const;

     /// check if option was present and return its (first) numerical value in optval (!)
     bool 		getOpt( const std::string opt, int &optval ) const;

     /// check if option was present and return its values in optvalvec(!)
     bool 		getOpts( const std::string opt, std::vector<std::string> &optvalvec ) const;

     /// Apply the criteria in the QualList to the argv[]
     virtual bool 	evalArgs(std::vector<std::string>& argv);

     /// Second pass options processing
     virtual bool 	evalArgsPass2(std::vector<std::string>&, pool::Args2Container&) { return true; }

     /// Returns true if evalArgs has been called with no errors 
     bool 		valid() const {return m_argsfine;}

     /// identifier
     std::string 	name() const {return m_id;}

     /// 
     ArgQual& 		getArgQual(const std::string a) {return m_quals[a];}
     QualList& 		getArgQuals( ) {return m_quals;}

     /// Hack to bring in allowed types of Collections
     std::vector<std::string>& allowedTypes() {return m_allowedTypes;}

     /// Number of arguments for a given qualifer, e.g. nArgs("-src")
     unsigned int 	nArgs(std::string);

     /// set the QualList
     void 		setArgQuals( QualList& quals) {m_quals=quals;}

     /// set flag to suppress error messages about missing arguments e.g. when -xmlInput is present
     void		ignoreMissingArgs( bool ignore ) { m_ignoreMissingArgs = ignore; }
     
  protected:

     bool 		m_argsfine;
     bool		m_ignoreMissingArgs;
     std::vector<std::string>   m_argv;

  private:

     /// Fill the vector m_allowedTypes
     void initTypes();

     QualList m_quals;
     std::string m_id;
     std::vector<std::string> m_allowedTypes;
   };


   inline void    CmdLineArgs2::init()
   {
      this->clear();
      m_argsfine = false;
      m_ignoreMissingArgs = false;
   }

   
} // end pool namespace

#endif  // UTILITIES_COLLECTION_CMDLINEARGS2

