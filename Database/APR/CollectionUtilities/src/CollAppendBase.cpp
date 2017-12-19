/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollAppend.cpp
 * @brief  Base class for utilities that copy or merge POOL collections
 * @author Marcin.Nowak@cern.ch
 * $Id:
 */

#include "CollectionUtilities/CollAppendBase.h"
#include "CollectionUtilities/MetaInfo.h"
#include "CollectionUtilities/MaxEventsInfo.h"

#include "CoralBase/Attribute.h"

#include "POOLCore/Exception.h"
#include "POOLCore/boost_tokenizer_headers.h"

#include <ctime>
#include <sstream>
#include <iostream>

#include <memory> 

#define corENDL coral::MessageStream::endmsg

using namespace std;
using namespace pool;


CollAppendBase::CollAppendBase(std::string name) :
      m_thisProgram( name ),
      m_provName("SourceFID_ref"),
      m_provCLID("6F6A12A0-FEEF-484B-9691-94B82B90CDBA"),
      m_provCollExists(false),
      m_noAttrib(false),
      m_numEvents(-1),
      m_numEventsPerCommit(static_cast<unsigned int>(-1)),
      m_numEventsPerPrint(-1),
      m_numRowsCached(0),
      m_extendProv(false),
      m_ignoreSchemaDiff( false ),
      m_evtCounterTotal( 0 ),
      m_committed( false ),
      m_collectionService(new pool::CollectionService()),
      m_log( name ),
      m_argsVec( name, true, &m_log ),
      m_initialized( false )
{
   m_metainfo = new MetaInfo();

   // attempt to shut up Coverity, MN
   time(&m_starttime); 
   time(&m_endtime);
}

CollAppendBase::~CollAppendBase()
{
   delete m_metainfo;
   delete m_collectionService;
}

void CollAppendBase::setMetaInfo( MetaInfo* minfo )
{
   if( m_initialized ) {
      m_log << coral::Error
            << "CollAppendBase::setMetaInfo() can not be used after init()"
            <<  coral::MessageStream::endmsg;
      exit(51);
   }
   delete m_metainfo;
   m_metainfo = minfo;
}


int
CollAppendBase::execute( std::vector<std::string> argv_v )
{
   // Primary try block
   try {
      if( !init( argv_v ) ) return 3;

      time(&m_starttime); 
      copyData();
      readMetadata(); 
      time(&m_endtime);
      
      addMetadata( );
      writeMetadata();
      
      closeCollections();
      finalize();
      return 0;
   }
   catch( pool::Exception& poolException )
   {
      if( m_committed ) cout << "At least one COMMIT has occurred" << endl;
      std::cerr << "pool::Exception: " << poolException.what() << std::endl;;
      return 1;
   }
   catch( std::exception& exception )
   {
      if( m_committed ) cout << "At least one COMMIT has occurred" << endl;
      std::cerr << "std::exception: " << exception.what() << std::endl;;
      return 1;
   }
   catch( ... )
   {
      if( m_committed ) cout << "At least one COMMIT has occurred" << endl;
      std::cerr << "Unknown exception caught in main()." << std::endl;;
      return 1;
   }
}



// Loop over destination collections and check if they exist
void
CollAppendBase::chkExistingDst(vector<bool>& existVec)
{
   // initialize the exist vector
   existVec.resize( m_dstinfo.nDst() );

   for( unsigned int i=0; i < m_dstinfo.nDst(); ++i ) {
      m_log << "Testing collection '" << m_dstinfo.name(i)
	  << "' of type " << m_dstinfo.type(i) <<  coral::MessageStream::endmsg;
      // testing for collection existence
      try {
	 bool readOnly( true );
	 std::unique_ptr<pool::ICollection> dstCollection( m_collectionService->handle( m_dstinfo.name(i), m_dstinfo.type(i), m_dstinfo.connect(), readOnly ) );

	 std::unique_ptr<pool::ICollectionQuery> dquery( dstCollection->newQuery() );
	 pool::ICollectionCursor& cursor = dquery->execute();
	 if( !cursor.next() ) {
	    m_log << coral::Info
		<< "Destination collection `" + m_dstinfo.name(i) + "' is empty."
		<<  coral::MessageStream::endmsg;
	 }
	 existVec[i] = true; 
	 if( m_metainfo->copyMode() != ICollMetaHandler::merge ) {
	    m_log << coral::Error << "Destination collection '" << m_dstinfo.name(i)
		<< "' exists, and the copy mode was not 'merge'. "
		<< "Remove the existing collection first"
		<< coral::MessageStream::endmsg;
	    exit(1);
	 }
      }
      catch ( pool::Exception& ) {
	 if( m_metainfo->copyMode() == ICollMetaHandler::merge )
	    m_log << coral::Warning << "Destination Collection `" << m_dstinfo.name(i)
		  << "' does not exist; A new one will be created; "  << corENDL;
	 existVec[i] = false; 
      }
   }
}      


// Open all destination collections, creating them if not existing
std::vector< pool::ICollection* >
CollAppendBase::openDestCollections( pool::CollectionDescription& destDescription )
{
   vector<bool>	destCollExistVec;
   chkExistingDst( destCollExistVec );

   vector< ICollection* > destCollections( m_dstinfo.nDst() );
   for( unsigned int i=0; i<m_dstinfo.nDst(); i++ ) {
      if( destCollExistVec[i] ) {
	 // open existing
         int nDst = m_dstinfo.nDst();
	 m_progress.print("Opening destination collections", nDst ? 100*i/nDst : 0);
	 destCollections[i] = openDestCollection( m_dstinfo.name(i), 
						  m_dstinfo.type(i), 
						  m_dstinfo.connect() );
	 // Check whether an opened dst collection has an appropriate description 
	 if( !destDescription.isSubsetOf( destCollections[i]->description() ) ) {
	    m_log << coral::Error << " Destination collection '"
		  << destCollections[i]->description().name()
		  << "' does not have all the required columns to perform append"
                  << " (or column types do not match)" << corENDL;
	    exit(1);
	 }
      } else {
	 // create missing destination collections
         int nDst = m_dstinfo.nDst();
         if (nDst < 1) nDst = 1;
	 m_progress.print("Creating destination collections", 100*i/nDst);
	 destDescription.setName( m_dstinfo.name(i) );
	 destDescription.setType( m_dstinfo.type(i) ); 
	 destDescription.setConnection( m_dstinfo.connect() );
	 destCollections[i] = createDestCollection( destDescription );
      }
      // set Row Cache size for ALL collections here
      if( m_numRowsCached )
	 destCollections[i]->dataEditor().setRowCacheSize( m_numRowsCached );
   }
   return destCollections;
}



pool::ICollection*
CollAppendBase::openDestCollection( const std::string& name,
				    const std::string& type,
				    const std::string& connect )
{
   bool	readOnly(false);
   pool::ICollection* collection = m_collectionService->handle( name, type, connect, readOnly );
   if( m_catinfo.useCC() ) {
      // Try to register it in the catalog
      try { 
	 m_log << coral::Debug
	     << "----  Registering destination collection " << name << corENDL;
	 m_collectionService->registerExisting( name, type, connect );
      }catch( pool::Exception& e) {
	 m_log << coral::Warning << "Registration of collection "
	     << name << " failed with error: " << e.what() << corENDL;
      }
   }
   return collection;
} 


pool::ICollection*
CollAppendBase::createDestCollection( const pool::ICollectionDescription& destDescription ) 
{
   string name = destDescription.name();
   string type = destDescription.type();
   string connect = destDescription.connection();
   ICollection* collection = 0;
   m_log << coral::Info << "Creating destination collection " << name
       << " of type " << type <<  coral::MessageStream::endmsg;
   try { 
      bool overwrite( false );  // don't overwrite existing collection
      collection = m_collectionService->create( destDescription, overwrite );
      // created the collection - now try to register it
      if(m_catinfo.useCC()) {
	 bool reg_ok = false;
	 m_log << coral::Debug
	     <<  "----  Registering destination collection " << name << corENDL;
	 try {
	    bool overwrite( true );  // overwrite existing catalog entry
	    reg_ok = m_collectionService->registerExisting( collection, overwrite, name ); 
	 } catch(pool::Exception& e) {
	    m_log << coral::Error << e.what() << coral::MessageStream::endmsg;
	 }
	 if( !reg_ok ) {
	    m_log << coral::Warning << "Registration of destination collection " << name
		<< " FAILED!" << coral::MessageStream::endmsg;
	 }
      }	
   } catch( pool::Exception& ) {
      m_log << coral::Info <<"Could not create destination collection " << name
	  << " - testing again if the collection exists already " << coral::MessageStream::endmsg;
      if( m_collectionService->exists( name, type, connect ) ) {
	 m_log << coral::Info << "Opening destination collection " << name
	     << " in update mode" << coral::MessageStream::endmsg;
	 bool forUpdate(false);
	 collection = m_collectionService->handle( name, type, connect, forUpdate );
      } else {  
	 throw;
      }
   }
   return collection;
}



// Create Description of the destination collection
// use only selected columns if this option was specified
//  - updates m_removedTokens and m_provCollExists when processing provenance
pool::CollectionDescription 
CollAppendBase::buildDstDesc(const pool::ICollectionDescription& sourceDesc
                             ,const pool::TokenList &tokens
			     ,const coral::AttributeList& attribs
                             ,const string queryopt
			     )
{
   // find which will be the Primary Token in the output collection
   typedef boost::tokenizer<boost::char_separator<char> > Tizer;
   const boost::char_separator<char> sep(" ,");
   Tizer        tizer( queryopt, sep );
   string       destPrimaryTok;
   int          pos = 1, prev_pos = 1000000;
   for( TokenList::const_iterator token = tokens.begin(); token != tokens.end(); ++token ) {
      const string name( token.tokenName() );
      if( name == sourceDesc.eventReferenceColumnName() ) {
         // dest Primary will be the same as source
         destPrimaryTok = name;
         break;
      }
      // check which posistion this token name has on the queryopt list, take the first
      for( Tizer::iterator col = tizer.begin(); col != tizer.end(); ++col, ++pos ) {
         if( *col == name ) {
            if( pos < prev_pos ) {
               prev_pos = pos;
               destPrimaryTok = name;
            }
            break;
         }
      }
   }
   m_log << (destPrimaryTok==sourceDesc.eventReferenceColumnName()? coral::Debug : coral::Info )
         <<"Primary Token in the output collection will be '" << destPrimaryTok << "'" << corENDL;
   
   CollectionDescription	destDescription( sourceDesc.name(), "destCollType" );
   for( TokenList::const_iterator token = tokens.begin(); token != tokens.end(); ++token ) {
      string name( token.tokenName() );
      if( name == destPrimaryTok ) {
	 if( name == "Token" ) {
	    /* MN: hack - "Token" column name taken from RootCollection */
	    // remap back the RootCollection default event ref name to the new default
	    name = pool::CollectionBaseNames::defaultEventReferenceColumnName();
	 } else {
	    // source had user defined name, copy it
	    destDescription.setEventReferenceColumnName( name );
	 }
      }
      const ICollectionColumn& column = sourceDesc.column( name );
      // Do not copy PROV columns if adding a new PROV columns
      if( column.annotation().find("PROV") == string::npos || !m_extendProv ) {
	 destDescription.insertTokenColumn( name, column.annotation(),
					    column.collectionFragmentName() );
      } else {
	 m_log << coral::Debug << "Ignoring source collection provenance Token attribute: "
	     << name << corENDL;
	 m_removedTokens.insert( name );
      }
   }
   // add the new provenance Token
   if( m_extendProv ) try {
      destDescription.insertTokenColumn( m_provName, "PROV",
					 destDescription.collectionFragment(0).name() );
      m_provCollExists = true;
   }catch( pool::Exception &e ) {
      m_log << coral::Error << "Failed to add provenance Token attribute " << m_provName
	  << "; " << e.what() << corENDL;
   }
   // add new References from the -addattrib option   
   for( TokenList::const_iterator token = m_addedReferences.begin();
        token != m_addedReferences.end(); ++token ) {
      destDescription.insertTokenColumn( token.tokenName() );
   }
   
   // add requested ATTRIBUTES
   for( coral::AttributeList::const_iterator attrib = attribs.begin();
	attrib != attribs.end();  ++attrib ) {
      const string& name = attrib->specification().name();
      const ICollectionColumn& column = sourceDesc.column( name );
      destDescription.insertColumn( name, column.type(), column.annotation(),
				    column.collectionFragmentName(),
				    column.maxSize(), column.sizeIsFixed() );
   }
   // add new attributes from the -addattrib option
   for( coral::AttributeList::const_iterator attrib = m_addedAttributes.begin();
	attrib != m_addedAttributes.end();  ++attrib ) {
      const string& name = attrib->specification().name();
      const string& type = attrib->specification().typeName();
      const ICollectionColumn* column = destDescription.columnPtr(name);
      if( !column ) {
         // only add not existing columns
         destDescription.insertColumn( name, type );
      } else {
         if( type != column->type() ) {
            string err = "Type mismatch between existing attribute '" + column->type() + " " + name + " and added attribute type: '" + type + "'";
            throw pool::Exception(err, "buildDstDesc", "CollAppend");
         }
      }
   }
   
   return destDescription;
}


/// open one of the source collectinos
/// * throws exceptions in case of failure
pool::ICollection*
CollAppendBase::openSrcCollection( const std::string& name,
				   const std::string& type,
				   const std::string& connect )
{
   m_log << coral::Info
       << "Opening source collection " << name << " of type " << type << corENDL;
   bool readOnly( true );
   pool::ICollection *srcCollection = m_collectionService->handle( name, type, connect, readOnly );

   //Try to register it in the catalog
   if (m_catinfo.useCC()) {
      try {
	 m_progress.print("Registering source collection", 0);
	 m_log << coral::Debug
	     <<  "----  Registering source collection " << name << corENDL;
	 m_collectionService->registerExisting( name, type, connect, name);
      } catch(pool::Exception& e) {
	 m_log << coral::Info << " Source collection registration failed: " << e.what()
	     << corENDL;
      }
   }

   m_log <<  coral::Debug << "Opened source collection " << name << corENDL;
   return srcCollection;
}


std::string
CollAppendBase::readCollectionGUID( pool::ICollection* collection )
{
   if( (!m_extendProv && m_metainfo->noMetadata() )
       || !collection->metadata().existsKey(CollectionBaseNames::CollIDMdataKey()) )
      return string("NOTFOUND") ;
   string GUID = collection->metadata().getValueForKey(CollectionBaseNames::CollIDMdataKey());
   m_log << coral::Debug << "Collection " << collection->description().name() << " GUID= "
       << GUID << corENDL;
   return GUID;
}



/// copy all data rows.
/// Opens source and destination collections, leaves them open and uncommitted
void
CollAppendBase::copyData()
{
   // loop over all source collections
   size_t k=0;
   CollectionDescription destDescription( m_srcinfo.name(0), "type" );
   m_log << coral::Info << "Number of input collections: " << m_srcinfo.nSrc() << corENDL;
   for( unsigned int sCollN=0; sCollN < m_srcinfo.nSrc(); sCollN++ )
   {
      int nSrc = m_srcinfo.nSrc();
      if (nSrc < 1) nSrc = 1;
      m_progress.print("Opening source collection", 100*sCollN / nSrc);
      pool::ICollection *srcCollection = openSrcCollection( m_srcinfo.name(sCollN), m_srcinfo.type(sCollN), m_srcinfo.connect() );
      m_srcCollections.push_back( srcCollection );
      m_srcCountVec.push_back(0);
	 
      m_log << "Creating query for the source collection" <<  coral::MessageStream::endmsg;
      std::unique_ptr<ICollectionQuery>  srcQuery( srcCollection->newQuery() );
      srcQuery->setRowCacheSize( 1000 );
      srcQuery->setCondition( m_queryinfo.query(sCollN) );
      if( m_queryinfo.queryOptions().size() ) {
         // add attributes requested on the command line
	 srcQuery->addToOutputList( m_queryinfo.queryOptions() );
         // -noattrib option adds all tokens on top of that
	 if( m_noAttrib )
	    srcQuery->selectAllTokens();
         srcQuery->skipEventReference();
      } else {
	 if( m_noAttrib )
	    srcQuery->selectAllTokens();
	 else
	    srcQuery->selectAll();
      }
      m_log << "Executing query for the source collection" <<  coral::MessageStream::endmsg;
      m_progress.print("Querying source collection", 0);
      pool::ICollectionCursor& cursor = srcQuery->execute();	 
 
      // Prepare destination collections at the first source iteration
      if( m_evtCounterTotal == 0 ) {
	 destDescription = buildDstDesc( m_srcCollections[sCollN]->description(),
					 cursor.currentRow().tokenList(), cursor.currentRow().attributeList(),
                                         m_queryinfo.queryOptions() );
      } else {
	 // Source Collection 2+
         //   build attr list for input collection for check with destination
         CollectionDescription desc( m_srcinfo.name(sCollN), "type" );
         desc = buildDstDesc( m_srcCollections[sCollN]->description(),
                              cursor.currentRow().tokenList(), cursor.currentRow().attributeList(),
                              m_queryinfo.queryOptions() );         
         if( ! desc.isSubsetOf( destDescription ) ) {
            m_log << coral::Error << "Source collection '"
		  << srcCollection->description().name()
		  << "' has attributes not existing in the destination collection and can not be copied."
		  << " EXITING" << corENDL;
	    exit(2);
	 }
	 // For each source collection get clean dest. coll. row buffers
	 for( unsigned int i=0; i<m_dstinfo.nDst(); i++ ) {
	    m_destCollections[i]->dataEditor().clearRowBuffers();
	 }
      }
         
      m_log << "Iterating over the source collection" <<  coral::MessageStream::endmsg;
      long long evtCounter = 0;
      const std::string& srcRefName = srcCollection->description().eventReferenceColumnName();
      m_provToken.setDb( readCollectionGUID(srcCollection) );
      size_t		srcSize = 1;  // the size of the query result set is not known - todo sometime
      size_t		progressStep = (srcSize>99) ? srcSize/100 : 1;

      // loop over input collection and copy all rows to all output collections
      while( cursor.next() && (m_numEvents < 0 || m_numEvents > evtCounter) ) {
	 if (m_destCollections.empty()) m_destCollections = openDestCollections( destDescription );
	 evtCounter++;
	 m_evtCounterTotal++;
	 if( (evtCounter % progressStep) == 0)
	    m_progress.print("Copying collection(s)", (long)(100*evtCounter/srcSize));

	 for( unsigned int i=0; i<m_dstinfo.nDst(); i++ )
	 {
	    // Get row buffer for adding data to collection.
	    CollectionRowBuffer &rowBuffer = m_destCollections[i]->dataEditor().rowBuffer();
            if( evtCounter == 1 ) {
               // first row: 
               //cout << "XXXXXXXX    recreating the buffer" << endl;
               //cout << "   rowBuffer: ";   rowBuffer.attributeList().toOutputStream(cout);  cout << endl;
               //cout << "   input row: ";   cursor.currentRow().attributeList().toOutputStream(cout);  cout << endl;
               //cout << "   new attribs: ";   m_addedAttributes.toOutputStream(cout);  cout << endl;
               // insert default values for the new attributes requested with -addattr option
               // use temporary attribute list to avoid changing the defaults through merge
               coral::AttributeList     newAttribsTmp( m_addedAttributes );
               rowBuffer.attributeList().merge( newAttribsTmp );

               // add new References from the -addattrib option   
               for( TokenList::const_iterator token = m_addedReferences.begin();
                    token != m_addedReferences.end(); ++token ) {
                  token->setData( &rowBuffer.tokenList()[ token.tokenName() ] );
               }
            }  

	    // copy values from source
	    rowBuffer.attributeList().merge( cursor.currentRow().attributeList() );
            
	    // copy tokens
	    const std::string& dstRefName = m_destCollections[i]->description().eventReferenceColumnName();
	    const TokenList&  tokens = cursor.currentRow().tokenList();
	    for( TokenList::const_iterator ti = tokens.begin(); ti != tokens.end(); ++ti ) {
	       if( !m_extendProv || m_removedTokens.find( ti.tokenName() ) == m_removedTokens.end() ) {
		  if( ti.tokenName() == srcRefName ) {
		     ti->setData( &rowBuffer.tokenList()[ dstRefName ] );
		  } else {
		     ti->setData( &rowBuffer.tokenList()[ ti.tokenName() ] );
		  }
	       }
	    }
            
	    if( m_provCollExists ) {
	       m_provToken.setData( &rowBuffer.tokenList()[m_provName] );
	    }
	    m_destCollections[i]->dataEditor().insertRow( rowBuffer );

	    if( !(m_evtCounterTotal % m_numEventsPerCommit) ) {
	       m_destCollections[i]->commit();
	       m_committed = true;
	    }

	    if ( (m_numEventsPerPrint >= 0) && !(m_evtCounterTotal % m_numEventsPerPrint) ) {
	       m_log << m_evtCounterTotal << " events appended" << corENDL;
	    }
	 }
      }
      if( k < m_srcCountVec.size() ) m_srcCountVec[k]=(long)evtCounter;
      k++;
   }
}


bool
CollAppendBase::init( std::vector<std::string> argv_v )
{
   m_provToken.setCont( "POOLCollection" );
   m_provToken.setClassID( Guid(m_provCLID) );
   m_provToken.setTechnology( 0 );
     
   m_argsVec.desc << "Takes one or more collections (of varying formats) and "
                  << "merges them and writes the results to one or more output "
                  << "collections (of varying formats). " << endl
                  << "Infile Metadata handling depends on -copy, -extract and -merge options." << endl;
 
   // list of CollAppend *specific* cli keys and their argument properties
   QualList markers;
   markers["-noattrib"] = ArgQual("Copy only Tokens. Attributes specified with -queryopt will still be copied as well", 0);
   markers["-nevtpercommit"].desc << "[Max. number of events to process between "
				  << "collection commits (DEFAULT = -1)]";
   markers["-nevtperprint"].desc << "Number of events to process before "
				 << "each print of status to the screen.";
   markers["-nevtcached"].desc << "size of the insert buffer for bulk operations "
			       << "DEFAULT = 0 (no bulk operations)";
   // markers["-ignoreschemadiff"].desc << "attempts to copy collections even if schema seems incompatible "
   //                                  << "(use at your own risk)";
   markers["-extendprov"].desc << "Add GUIDs of the input collections to the new Token attribute <arg>. "
			       << "If <arg> is \"default\" the default name will be used: " << m_provName ;

   markers["-addattr"] = ArgQual("Add new attributes to output collection", 3, true);
   markers["-addattr"].desc << " in the form: NAME TYPE VALUE."
                            << " accepted types: char,short,int,int64,uchar,ushort,uint,uint64,float,double,string,token";
      
   CmdLineArgs2 cmdLineArgs;
   cmdLineArgs.setArgQuals(markers);
   m_argsVec.push_back(&cmdLineArgs);  // Add it to the list

   // Classes with shared cli keys and their argument properties
   // Add them to the list
   m_argsVec.push_back(&m_catinfo);
   m_argsVec.push_back(&m_queryinfo);
   m_argsVec.push_back(&m_srcinfo);
   m_argsVec.push_back(&m_dstinfo);
   m_argsVec.push_back(m_metainfo);
   m_argsVec.push_back(&m_progress);
   MaxEventsInfo maxEvents;
   m_argsVec.push_back(&maxEvents);

   // Check that all cmd line args are valid
   if( !m_argsVec.evalArgs(argv_v) ) return false;
   if( !m_argsVec.checkValid() )     return false;

   if( maxEvents.specified() )
      m_numEvents = maxEvents.get();
   m_noAttrib = cmdLineArgs.hasQual("-noattrib");
   //m_ignoreSchemaDiff  = cmdLineArgs.hasQual("-ignoreschemadiff");
   cmdLineArgs.getOpt("-nevtpercommit", m_numEventsPerCommit);
   cmdLineArgs.getOpt("-nevtcached", m_numRowsCached);
   string val;
   if( cmdLineArgs.getOpt("-nevtperprint", val) ) {
      m_numEventsPerPrint = ( val=="all"? -1 : atoi(val.c_str()) );
   }
   if( cmdLineArgs.getOpt("-extendprov", val) ) {
      m_extendProv = true;
      if( val != "default" )
         m_provName = val;  // + "_ref";
   }
   vector<string>       addAttributesVec;
   if( cmdLineArgs.getOpts("-addattr", addAttributesVec) ) {
      for(size_t i=0; i<addAttributesVec.size(); i+=3) {
         const string   attrname = addAttributesVec[i];
         const string   attrtype = addAttributesVec[i+1];
         istringstream    val( addAttributesVec[i+2] );
         if( attrtype == "int" ) {
            m_addedAttributes.extend(attrname, attrtype);
            val >> m_addedAttributes[attrname].data<int>();
         } else if( attrtype == "uint" ) {
            m_addedAttributes.extend(attrname, "unsigned int");
            val >> m_addedAttributes[attrname].data<unsigned int>();
         } else if( attrtype == "char" ) {
            m_addedAttributes.extend(attrname, attrtype);
            val >> m_addedAttributes[attrname].data<char>();
         } else if( attrtype == "uchar" ) {
            m_addedAttributes.extend(attrname, "unsigned char");
            val >> m_addedAttributes[attrname].data<unsigned char>();
         } else if( attrtype == "short" ) {
            m_addedAttributes.extend(attrname, attrtype);
            val >> m_addedAttributes[attrname].data<short>();
         } else if( attrtype == "ushort" ) {
            m_addedAttributes.extend(attrname, "unsigned short");
            val >> m_addedAttributes[attrname].data<unsigned short>();
         } else if( attrtype == "float" ) {
            m_addedAttributes.extend(attrname, attrtype);
            val >> m_addedAttributes[attrname].data<float>();
         } else if( attrtype == "double" ) {
            m_addedAttributes.extend(attrname, attrtype);
            val >> m_addedAttributes[attrname].data<double>();
         } else if( attrtype == "string" ) {
            m_addedAttributes.extend(attrname, attrtype);
            // val >> string  stops at the first space!  MN
            m_addedAttributes[attrname].data<string>() = val.str();
         } else if( attrtype == "int64" ) {
            m_addedAttributes.extend(attrname, "long long");
            val >> m_addedAttributes[attrname].data<long long>();
         } else if( attrtype == "uint64" ) {
            m_addedAttributes.extend(attrname, "unsigned long long");
            val >> m_addedAttributes[attrname].data<unsigned long long>();
         } else if( attrtype == "token" ) {
            m_addedReferences.extend(attrname);
            if( val.str().length() > 1 ) {
               m_addedReferences[attrname].fromString( val.str() );
            }
         } else {
            markers["-addattr"].print(std::cerr);
            throw pool::Exception(string("Unsupported attribute type for -addattr option: ") + attrtype,
                                  "init", "CollAppend");
         }
      }
   }
   if( m_metainfo->copyMode() == ICollMetaHandler::copy && m_srcinfo.nSrc() > 1 ) {
      m_log << coral::Error << "Copy mode: 'copy' supports only one source collection" << corENDL;
      return false;
   }
   
   m_catinfo.setCatalogs( m_collectionService );
   m_initialized = true;
   return true;
}


void
CollAppendBase::readMetadata()
{
   m_progress.print("Metadata processing", 0);
   if( m_metainfo->checkMetadata( m_srcCollections, m_destCollections ) ) {
      // only read metadata if "check" did not report any conflicts in the dest collections
      m_metainfo->readMetadata( m_srcCollections );
   }
}


void
CollAppendBase::addMetadata( )
{
   // add extract information
   if( !m_metainfo->noMetadata() && m_metainfo->copyMode() == ICollMetaHandler::extract ) {
      string user; if (getenv("USER")!=NULL) user = getenv("USER");
      string host; if (getenv("HOST")!=NULL) host = getenv("HOST");
      string userhost = user + "@" + host;
      stringstream stimestr, etimestr;

      std::string inputCollections  = m_srcinfo.name(0);
      for( size_t i=1; i<m_srcinfo.nSrc(); i++ ) 
	 inputCollections += ":"+m_srcinfo.name(i);
      std::string outputCollections = m_dstinfo.name(0);
      for( size_t i=1; i<m_dstinfo.nDst(); i++ ) 
	 outputCollections += ":"+m_dstinfo.name(i);
      stimestr << m_starttime; etimestr << m_endtime;

      m_metainfo->addMetaEntry( "TimeBegin", stimestr.str() );
      m_metainfo->addMetaEntry( "TimeEnd", etimestr.str() );
      m_metainfo->addMetaEntry( "ExtractNode", userhost );
      m_metainfo->addMetaEntry( "InputCollections", inputCollections );
      m_metainfo->addMetaEntry( "OutputCollections", outputCollections );
      m_metainfo->addMetaEntry( "Query", (m_queryinfo.query().length() ? m_queryinfo.query() : "1=1") );
   }
}


/// write all metadata
void
CollAppendBase::writeMetadata( )
{
   m_progress.print("Metadata writing", 0);
   m_metainfo->writeMetadata( m_destCollections );
}


      
void
CollAppendBase::closeCollections( )
{
   for( size_t i=0; i<m_dstinfo.nDst(); i++ ) {
      m_destCollections[i]->commit();
      m_committed = true;
      m_destCollections[i]->close();
      delete m_destCollections[i];
   }
   m_destCollections.resize(0);

   for( size_t i=0; i<m_srcinfo.nSrc(); i++ ) {
      m_srcCollections[i]->close();
      delete m_srcCollections[i];
   }
   m_srcCollections.resize(0);
}



void
CollAppendBase::finalize( )
{
   // List individual collections (with evt counts)
   cout << "CollAppend: Finished appending input collection(s): " << endl;
   for( unsigned int i=0; i<m_srcinfo.nSrc(); i++ ) {
      cout << i+1 << ": `" << m_srcinfo.name(i) << ":" 
	   << m_srcinfo.type(i) << "' (" 
	   << m_srcCountVec[i]    << " events) "
	   << endl;
   }
      
   // List individual output collections
   cout << " to output collection(s): " << endl;
   for( unsigned int i=0; i<m_dstinfo.nDst(); i++ ) {
      cout << i+1 << ": `" << m_dstinfo.name(i) << ":" 
	   << m_dstinfo.type(i) << "' "
	   << endl;
   }
      
   // Note if there was a selection involved
   if (m_queryinfo.query().size() > 0) cout << "Qualified by " << m_queryinfo.query() << endl;
   // Give total for output
   cout << endl;
   cout << " ===> " << m_evtCounterTotal << " total events appended to each output" << endl;
   cout << " ===> " << "Processing took " << difftime(m_endtime, m_starttime) << " seconds" << endl;
   if( m_committed ) cout << "At least one COMMIT has occurred" << endl;
}
