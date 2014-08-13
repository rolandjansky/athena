/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_SRCINFO
#define UTILITIES_COLLECTION_SRCINFO

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/**********************************************************

SrcInfo is an concretization of a CmdLineArgs2
   to contain input collection info.

Qualifiers: -src, -srcconnect

**********************************************************/

namespace pool
{

   class SrcInfo : public CmdLineArgs2
   {
  public:

     /// Constructors
     SrcInfo( );

     virtual ~SrcInfo() {}

     ///
     bool evalArgs(std::vector<std::string>& argv);

     std::string connect() {return m_connection;}

     std::pair<std::string,std::string> nameAndType(unsigned int);

     std::string name(unsigned int);
     std::string type(unsigned int);

     unsigned int nSrc();

  private:
     std::string m_connection;
     std::vector<std::string> m_NameVec;
     std::vector<std::string> m_TypeVec;

   };


   
   inline
   SrcInfo::SrcInfo( ) : CmdLineArgs2("SrcInfo") 
   {
      QualList markers;
      markers.insert( std::make_pair("-src",       ArgQual(2,true,true)) );
      markers.insert( std::make_pair("-srcconnect",ArgQual()) );
      markers["-src"].desc << "<input collection name> <input collection type> " << std::endl
			   << "List of name-type pairs is space separated; "
			   << "Choices for collection types are " << std::endl;
      for (std::vector<std::string>::iterator at = this->allowedTypes().begin(); 
	   at != this->allowedTypes().end(); 
	   ++at)
	 markers["-src"].desc << *at << std::endl;
      markers["-srcconnect"].desc << "[Input database connection string] " << std::endl
				  << "(Note: The default value is \"\" and this argument MUST "
				  << "be specified for RelationalCollection." << std::endl
				  << "The connection string will be looked up in $CORAL_DBLOOKUP_PATH/dblookup.xml" << std::endl
				  << "Database user authentication is done using CORAL_AUTH_PATH/authentication.xml" << std::endl
				  << " or CORAL_AUTH_USER and CORAL_AUTH_PASSWORD variables" << std::endl;
      this->setArgQuals(markers);
      m_NameVec.clear();
      m_TypeVec.clear();
   }

   inline bool	 SrcInfo::evalArgs(std::vector<std::string>& argv)
   {
      // Call underlying parser in the base class
      bool retc = this->CmdLineArgs2::evalArgs(argv);
      if(!retc) return retc;

      if ( this->hasQual("-srcconnect") ) {
	 int ifirst = (*this)["-srcconnect"].first;
	 m_connection = argv[ifirst];
      }
        
      if ( this->hasQual("-src") ) {
	 int ifirst = (*this)["-src"].first;
	 int ilast  = (*this)["-src"].second;
	 std::string srcCollName = "";
	 std::string srcCollType = "";
	 int i = ifirst;
	 while( i < ilast ) {
	    srcCollName = std::string(argv[i]); ++i;
	    srcCollType = std::string(argv[i]);
	    // First check if it is a RelationalCollection
	    // Or is it a file based or logical type collection
	    // Or is it gibberish, and they need to try again.
	    if ( std::find(this->allowedTypes().begin(),
			   this->allowedTypes().end(),
			   srcCollType) == this->allowedTypes().end() ) {
	       std::cerr << " ERROR Collection type " << srcCollType 
			 << " is not presently handled by this program." 
			 << std::endl;
	       retc = false;
	    }
	    m_NameVec.push_back( srcCollName );
	    m_TypeVec.push_back( srcCollType );
	    ++i;
	 } // end of m_nameAndTypeVec loop
      } // end of -src Qual

      // Check that connection is available for any RDB collections
      if(std::find(m_TypeVec.begin(),m_TypeVec.end(),"RelationalCollection") != m_TypeVec.end() )
      {
	 if(m_connection.size()==0) {
	    std::cerr << " ERROR Found at least one RDB source collection, "
		      << "but -srcconnect is not set" << std::endl;
	    retc = false;
	    m_argsfine = false;
	 }
      }

      return retc;

   }

   inline std::pair<std::string,std::string>
      SrcInfo::nameAndType(unsigned int ind)
   {
      if ( ind < m_TypeVec.size() ) 
	 return std::pair<std::string,std::string>(m_NameVec[ind],m_TypeVec[ind]);
      else {
	 std::cerr << "Out of range request for src (name,type) pair" << std::endl;
	 return std::pair<std::string,std::string>("BAD","BAD");
      }
   }

   inline std::string
      SrcInfo::name(unsigned int ind)
   {
      if ( ind < m_NameVec.size() ) return m_NameVec[ind];
      else {
	 std::cerr << "Out of range request for src name" << std::endl;
	 return std::string("BAD");
      }
   }

   inline std::string
      SrcInfo::type(unsigned int ind)
   {
      if ( ind < m_TypeVec.size() ) return m_TypeVec[ind];
      else {
	 std::cerr << "Out of range request for src type" << std::endl;
	 return std::string("BAD");
      }
   }

   inline unsigned int
      SrcInfo::nSrc()
   {
      return this->nArgs("-src");
   }

} // end pool namespace
      
#endif  // UTILITIES_COLLECTION_SRCINFO







