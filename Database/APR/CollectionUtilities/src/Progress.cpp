/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CollectionUtilities/Progress.h"

#include <fstream>
using namespace std;
using namespace pool;

const std::string progress_opt("-progress");

Progress::Progress()
      : CmdLineArgs2( "ProgressMeter" )
      , m_valid( false )
      , m_stdout( false )
{
   QualList markers;
   markers.insert( std::make_pair(progress_opt, ArgQual(1, false, false)) );
   markers[progress_opt].desc << "Write out progress indicator to a given file ( 'stdout' prints to stdout )";
   setArgQuals(markers);
}


bool Progress::evalArgs(std::vector<std::string>& argv)
{
   bool retc = CmdLineArgs2::evalArgs(argv);
   if( retc && hasQual(progress_opt) ) {
      int ifirst = (*this)[progress_opt].first;
      m_outFName = argv[ifirst];
   }
   if( m_outFName == "stdout" ) {
      m_stdout = true;
   }
   m_valid = true;
   return retc;
}
     

void Progress::print( const std::string& action, int percentage )
{
   if( !m_valid )  return;

   std::streambuf *buf;
   if( !m_stdout ) {
      m_file.open( m_outFName.c_str(), ios_base::trunc | ios_base::out );
      buf = m_file.rdbuf();
   } else {
      buf = std::cout.rdbuf();
   }
   std::ostream out(buf);

   out << percentage << ";" << action << endl;

   if( !m_stdout )  m_file.close();
}
