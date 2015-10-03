/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_PROGRESS
#define UTILITIES_COLLECTION_PROGRESS

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/**********************************************************

Progress indicator options and implementation
Qualifiers: -progress <filename>

**********************************************************/

#include <fstream>

namespace pool
{
   
   class Progress : public CmdLineArgs2
   {
  public:

     /// Constructors
     Progress( );

     virtual ~Progress() {}

     ///
     bool evalArgs(std::vector<std::string>& argv);

     void print( const std::string& action, int percentage );

  
  private:
     bool		m_valid;
     bool		m_stdout;
     std::string	m_outFName;
     std::ofstream	m_file;
   };

} // end pool namespace

#endif  


