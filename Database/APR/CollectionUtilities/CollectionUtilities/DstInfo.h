/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_DSTINFO
#define UTILITIES_COLLECTION_DSTINFO

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/**********************************************************

DstInfo is an concretization of a CmdLineArgs2
   to contain output collection info.

Qualifiers: -dst, -dstconnect

**********************************************************/

namespace pool
{
   
   class DstInfo : public CmdLineArgs2
   {
  public:

     /// Constructors
     DstInfo( );

     virtual ~DstInfo() {}

     ///
     bool evalArgs(std::vector<std::string>& argv);

     std::string connect() {return m_connection;}

     std::pair<std::string,std::string> nameAndType(unsigned int);

     std::string name(unsigned int);
     std::string type(unsigned int);

     unsigned int nDst();

  private:
     std::string m_connection;
     std::vector<std::string> m_NameVec;
     std::vector<std::string> m_TypeVec;

   };

   DstInfo::DstInfo( ) : CmdLineArgs2("DstInfo") 
   {
      QualList markers;
      markers.insert( std::make_pair("-dst",       ArgQual(2,true,true)) );
      markers.insert( std::make_pair("-dstconnect",ArgQual()) );
      markers["-dst"].desc << "<output collection name> <output collection type> " << std::endl
			   << "List of name-type pairs is space separated; "
			   << "Choices for collection types are " << std::endl;
      for (std::vector<std::string>::iterator at = this->allowedTypes().begin(); 
	   at != this->allowedTypes().end(); 
	   ++at)
      {
	 markers["-dst"].desc << *at << std::endl;
      }
      markers["-dstconnect"].desc << "[Output database connection string] " << std::endl
				  << "(Note: The default value is \"\" and this argument MUST "
				  << "be specified for RelationalCollection." << std::endl
				  << "The connection string will be looked up in $CORAL_DBLOOKUP_PATH/dblookup.xml" << std::endl
				  << "Database user authentication is done using CORAL_AUTH_PATH/authentication.xml" << std::endl
				  << " or CORAL_AUTH_USER and CORAL_AUTH_PASSWORD variables" << std::endl;
      this->setArgQuals(markers);
      m_NameVec.clear();
      m_TypeVec.clear();
   }

   inline bool
   DstInfo::evalArgs(std::vector<std::string>& argv)
   {
      bool retc = true;
      retc = this->CmdLineArgs2::evalArgs(argv);
      if (!retc) return retc;

      if ( this->hasQual("-dstconnect") ) {
	 int ifirst = (*this)["-dstconnect"].first;
	 m_connection = argv[ifirst];
      }
        
      if ( this->hasQual("-dst") ) {
	 int ifirst = (*this)["-dst"].first;
	 int ilast  = (*this)["-dst"].second;
	 std::string dstCollName = "";
	 std::string dstCollType = "";
	 int i = ifirst;
	 while( i < ilast ) {
	    dstCollName = std::string(argv[i]); ++i;
	    dstCollType = std::string(argv[i]);
	    // First check if it is a RelationalCollection
	    // Or is it a file based or logical type collection
	    // Or is it gibberish, and they need to try again.
	    if ( std::find(this->allowedTypes().begin(),
			   this->allowedTypes().end(),
			   dstCollType) == this->allowedTypes().end() ) {
	       std::cerr << " ERROR Collection type " << dstCollType 
			 << " is not presently handled by this program." 
			 << std::endl;
	       retc = false;
	    }
	    m_NameVec.push_back( dstCollName );
	    m_TypeVec.push_back( dstCollType );
	    ++i;
	 } // end of m_nameAndTypeVec loop
      } // end of -dst Qual

      // Check that connection is available for any RDB collections
      if (std::find(m_TypeVec.begin(),m_TypeVec.end(),"RelationalCollection") != m_TypeVec.end() )
      {
	 if (m_connection.size()==0) {
	    std::cerr << " ERROR Found at least one RDB output collection, "
		      << "but -dstconnect is not set" << std::endl;
	    retc = false;
	    m_argsfine = false;
	 }
      }

      return retc;

   }

   inline std::pair<std::string,std::string>
   DstInfo::nameAndType(unsigned int ind)
   {
      if ( ind < m_TypeVec.size() ) 
	 return std::pair<std::string,std::string>(m_NameVec[ind],m_TypeVec[ind]);
      else { 
	 std::cerr << "Out of range request for dst (name,type) pair" << std::endl;
	 return std::pair<std::string,std::string>("BAD","BAD");
      }
   }

   inline std::string
   DstInfo::name(unsigned int ind)
   {
      if ( ind < m_NameVec.size() ) return m_NameVec[ind];
      else { 
	 std::cerr << "Out of range request for dst name" << std::endl;
	 return std::string("BAD");
      }
   }

   inline std::string
   DstInfo::type(unsigned int ind)
   {
      if ( ind < m_TypeVec.size() ) return m_TypeVec[ind];
      else { 
	 std::cerr << "Out of range request for dst type" << std::endl;
	 return std::string("BAD");
      }
   }

   inline unsigned int
   DstInfo::nDst()
   {
      //return this->nArgs("-dst")/this->getArgQuals()["-dst"].argsize;
      return this->nArgs("-dst");
   }

} // end pool namespace

#endif  // UTILITIES_COLLECTION_DSTINFO

