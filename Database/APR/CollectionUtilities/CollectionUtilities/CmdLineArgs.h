/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_CMDLINEARGS
#define UTILITIES_COLLECTION_CMDLINEARGS

#include <string>
#include <vector>
#include <algorithm>

namespace pool
{

   class CmdLineArgs
   {
  public:

     /// Constructor.	
     CmdLineArgs( int argc, const char* argv[] );

     /// Check if argument is in command line.
     bool hasQual( const std::string arg ) const;

     /// Get argument from command line using qualifier.
     std::string getArg( const std::string arg );

     /// Get argument from command line using position.
     std::string getArg( std::vector<std::string>::const_iterator itr );

     /// Get position on command line.
     std::vector<std::string>::const_iterator getPos() const;

  private:

     std::vector<std::string> argvVector;
     std::vector<std::string>::const_iterator vecItr;
   };


   inline CmdLineArgs::CmdLineArgs( int argc, const char* argv[] ) 
	 : argvVector(argc)
   {	
      for( int i=0; i < argc; ++i ) 
	 argvVector[i].assign( argv[i] );
      vecItr = argvVector.begin();
   }


   inline bool CmdLineArgs::hasQual( const std::string arg ) const
   {
      return ( std::find( argvVector.begin(), argvVector.end(), arg ) !=
	       argvVector.end() );
   }


   inline std::string CmdLineArgs::getArg( const std::string arg )
   {
      std::vector<std::string>::const_iterator itr
	 = std::find( argvVector.begin(), argvVector.end(), arg );
      return ( getArg( itr ) );
   }


   inline std::string CmdLineArgs::getArg( 
      std::vector<std::string>::const_iterator itr )
   {
      vecItr = itr;
      if ( vecItr != argvVector.end() ) 
      {
	 vecItr++;
	 if ( vecItr != argvVector.end() )
	 { 
	    return *vecItr;
	 }
	 else
	 {
	    return ("");
	 } 
      }
      else
      {
	 return ("");
      }
   }


   inline std::vector<std::string>::const_iterator CmdLineArgs::getPos() const
   {
      return vecItr;
   }

} // end pool namespace

#endif  // UTILITIES_COLLECTION_CMDLINEARGS

