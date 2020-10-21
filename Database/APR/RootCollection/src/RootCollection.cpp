/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "RootCollection.h"
#include "RootCollectionQuery.h"
#include "RootCollectionMetadata.h"

#include "PersistentDataModel/Token.h"
#include "POOLCore/Exception.h"

#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionFragment.h"
#include "CollectionBase/CollectionBaseNames.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IFileMgr.h"
#include "GaudiKernel/IService.h"

#include "TFile.h"
#include "TNetFile.h"
#include "TTree.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TDirectory.h"

#define corENDL coral::MessageStream::endmsg

#include <map>
#include <vector>
#include <ctype.h>


using namespace std;

namespace pool {

  namespace RootCollection { 

    // MN: recreate replaced by update to not overwrite the file
    const char* const RootCollection::poolOptToRootOpt[] = {"CREATE","UPDATE","UPDATE","UPDATE","READ"};

    const Io::IoFlags RootCollection::poolOptToFileMgrOpt[] = { Io::WRITE|Io::CREATE, Io::WRITE|Io::APPEND, Io::WRITE|Io::APPEND, Io::WRITE|Io::APPEND, Io::READ };

    const char* const RootCollection::c_tokenBranchName = "Token";
    const char* const RootCollection::c_attributeListLayoutName = "Schema"; 
    const char* const RootCollection::c_treeName = "POOLCollectionTree";
    const char* const RootCollection::c_oldTreeName = "CollectionTree";


     RootCollection::RootCollection(
        const pool::ICollectionDescription* description,
        pool::ICollection::OpenMode mode,
        pool::ISession* )
      : m_description( *description ),
      m_name( description->name() ),
      m_fileName( description->name() + ".root" ),
      m_mode( mode ),
      m_tree( 0 ),
      m_file( 0 ),
      m_session( 0 ),
      m_open( false ),
      m_readOnly( mode == ICollection::READ ? true : false ),
      m_poolOut( "RootCollection"),
      m_schemaEditor( 0 ),
      m_dataEditor( 0 ),
	m_metadata( 0 ),
	m_fileMgr( 0 )
    {
       open();
    }

     
     RootCollection::~RootCollection()
     {
        if( m_open ) close();
        else cleanup();
     }


     void  RootCollection::delayedFileOpen( const std::string& method )
     {
        if( m_open && !m_file && m_session && m_mode != ICollection::READ ) {
           m_file = TFile::Open(m_fileName.c_str(), poolOptToRootOpt[m_mode] );
           if(!m_file || m_file->IsZombie()) {
              throw pool::Exception( string("ROOT cannot \"") + poolOptToRootOpt[m_mode] + "\" file " + m_fileName,
                                     std::string("RootCollection::") + method,
                                     "RootCollection" );
           }
           m_poolOut << coral::Info << "File " << m_fileName << " opened in " << method <<  coral::MessageStream::endmsg;
 
           m_tree->SetDirectory(gDirectory);
           m_schemaEditor->writeSchema();
        }
     }


     TTree*  RootCollection::getCollectionTree()
     {
        TTree *tree( NULL );
        if( m_file ) {
           tree = dynamic_cast<TTree*>(m_file->Get(c_treeName));
           if( !tree && m_file->Get(c_attributeListLayoutName) ) {
              tree = dynamic_cast<TTree*>(m_file->Get(c_oldTreeName));
              if( tree )
                 m_poolOut << coral::Info << "Found old Collection TTree name: "
                           << c_oldTreeName << coral::MessageStream::endmsg;
           }
           if( tree )
              m_poolOut << coral::Debug << "Retrieved Collection TTree  \""
                        << tree->GetName() << "\" from file " << m_fileName
                        << coral::MessageStream::endmsg;
        }
        return tree;
     }
        
        
     
     void RootCollection::commit( bool )
     {
        delayedFileOpen("commit");

        if( m_open ) {

	  if (m_tree->GetCurrentFile() == 0) {
	    m_poolOut << coral::Debug << "setting TFile for " 
		      << m_tree->GetName() << " to " << m_file->GetName()
		      << coral::MessageStream::endmsg;
	    m_tree->SetDirectory(m_file);
	  }

           m_poolOut << coral::Debug
                     << "Commit: saving collection TTree to file: " << m_tree->GetCurrentFile()->GetName()
                     << coral::MessageStream::endmsg;

           Long64_t bytes = m_tree->AutoSave();

           m_poolOut << "   bytes written to TTree " << (size_t)bytes
                     << coral::MessageStream::endmsg;
           if( m_metadata )
              m_metadata->commit();
        }
     }

     
    void RootCollection::rollback()
    {
      if( m_open ) {
         // reopen
        open();
      }
    }

     

    void RootCollection::close()
    {
       m_poolOut << coral::Info << "Closing " << (m_open? "open":"not open")
                 << " collection '" << m_fileName << "'" << coral::MessageStream::endmsg;
       if(m_open) {
          delayedFileOpen("close");
              
          if( m_mode == ICollection::CREATE || m_mode == ICollection::CREATE_AND_OVERWRITE ) {
             TObject* tree = getCollectionTree();
             if( tree )
                m_mode = ICollection::UPDATE;
             else {
                // probably supposed to unregister if the collection was not created
                m_mode = ICollection::CREATE_AND_OVERWRITE;
                if(m_fileCatalog){
                   m_fileCatalog->start();
                   m_fileCatalog->deleteFID( m_fileCatalog->lookupPFN(m_fileName) );
                   m_fileCatalog->commit();
                }
             }
          }
          if( m_mode != ICollection::READ ) {
             m_schemaEditor->writeSchema();
             // m_tree->Print();
             // m_file->Write( "0", TObject::kOverwrite );
          }
          cleanup();
       }
    }

     
    void RootCollection::cleanup()
    {
       if( m_file ) {
          int n = 0;
          if( m_fileMgr ) {
             n = m_fileMgr->close(m_file, "RootCollection");
          } else {
             m_file->Close();
          }
          if( n==0 ) delete m_file; 
          m_file = 0;
       }
       m_tree = 0;
       m_open = false;
       delete m_schemaEditor;   m_schemaEditor = 0;
       delete m_dataEditor;   m_dataEditor = 0;
       delete m_metadata;  m_metadata = 0;
    }       
       
     
    void RootCollection::open()  try
    {
      const string myFileType = "RootCollectionFile";

      if( m_open ) close();

      if( !m_fileCatalog
        && m_fileName.find("PFN:") == 0
        && m_description.connection().empty() )
      {
        // special case with no catalog and PFN specified
        // create the collection with exactly PFN file name
        m_fileName = m_description.name().substr(4);   // remove the PFN: prefix
      }
      else if( fileCatalogRequired() ) {
        m_fileName = "";

        if(!m_fileCatalog)
           m_fileCatalog = make_unique<pool::IFileCatalog>();
        
        if( m_mode == ICollection::CREATE ){
          string fid = retrieveFID();
          if(fid!="")
            throw pool::Exception( "Cannot CREATE already registered collections",
            "RootCollection::open", 
            "RootCollection");
          else{
            m_fileName = retrievePFN();
            FileCatalog::FileID dummy;
            m_fileCatalog->start();
            m_fileCatalog->registerPFN( m_fileName, myFileType, dummy);
            m_fileCatalog->commit();
          }
        }

        else if(m_mode == ICollection::CREATE_AND_OVERWRITE){
          string fid = retrieveFID();
          if(fid!="")
            m_fileName = retrieveUniquePFN(fid);
          else{
            m_fileName = retrievePFN();
            FileCatalog::FileID dummy;
            m_fileCatalog->start();
            m_fileCatalog->registerPFN( m_fileName, myFileType, dummy);
            m_fileCatalog->commit();
          }
        }

        else if(m_mode == ICollection::UPDATE){
          string fid = retrieveFID();
          if(fid!="")
            m_fileName = retrieveUniquePFN(fid);
          else
            throw pool::Exception( "Cannot UPDATE non registered collections",
            "RootCollection::open", 
            "RootCollection");
        }

        else if(m_mode == ICollection::READ) {
          string fid = retrieveFID();
          if(fid!="") {
             string dummy;
             m_fileCatalog->start();
             m_fileCatalog->getFirstPFN(fid, dummy, dummy);
             m_fileCatalog->commit();
          }else
             throw pool::Exception( "Cannot READ non registered collections",
                                    "RootCollection::open", 
                                    "RootCollection");
        }
      }

      TDirectory *dirsav = gDirectory;

      if( m_session == 0 || m_mode == ICollection::READ || m_mode == ICollection::UPDATE ) {
         // first step: Try to open the file
         m_poolOut << coral::Info << "Opening Collection File " << m_fileName << " in mode: "
                   << poolOptToRootOpt[m_mode] << coral::MessageStream::endmsg;
         bool fileExists = !gSystem->AccessPathName( m_fileName.c_str() );
         m_poolOut << coral::Debug << "File " << m_fileName
		   << (fileExists? " exists." : " does not exist." ) << corENDL;
         // open the file if it exists, or create if requested
         if( !fileExists && m_mode != ICollection::CREATE && m_mode != ICollection::CREATE_AND_OVERWRITE )
            m_file = 0;
         else {
            const char* root_mode = poolOptToRootOpt[m_mode];
	    Io::IoFlags io_mode = poolOptToFileMgrOpt[m_mode];

            if( fileExists && (m_mode == ICollection::CREATE
                               || m_mode == ICollection::CREATE_AND_OVERWRITE ) ) {
               // creating collection in an existing file
               root_mode = "UPDATE";
	       io_mode = (Io::WRITE | Io::APPEND);
            }
	    
	    if (m_fileMgr == 0) {
	      IService *is(0);

	      if (Gaudi::svcLocator()->getService("FileMgr",is,true).isFailure()) {
		m_poolOut << coral::Error 
			  << "unable to get the FileMgr, will not manage TFiles"
			  << coral::MessageStream::endmsg;
	      } else {
		m_fileMgr = dynamic_cast<IFileMgr*>(is);
	      }

	    }

	    // FIXME: hack to avoid issue with setting up RecExCommon links
	    if (m_fileMgr != 0 && 
		m_fileMgr->hasHandler(Io::ROOT).isFailure()) {
	      m_poolOut << coral::Info 
			<< "Unable to locate ROOT file handler via FileMgr. "
			<< "Will use default TFile::Open" 
			<< coral::MessageStream::endmsg;
	      m_fileMgr = 0;
	    }

	    if (m_fileMgr == 0) {
	      m_file = TFile::Open(m_fileName.c_str(), root_mode);
	    } else {
	      void *vf(0);
	      // open in shared mode only for writing
	      bool SHARED(false);
	      if (io_mode.isWrite()) {
		SHARED = true;
	      }	       
	      int r = m_fileMgr->open(Io::ROOT,"RootCollection",m_fileName,io_mode,vf,"TAG",SHARED);
	      if (r < 0) {
		m_poolOut << coral::Error << "unable to open \"" << m_fileName
			  << "\" for " << root_mode
			  << coral::MessageStream::endmsg;
	      } else {      
		m_file = (TFile*)vf;
	      }
	    }
         }
         if(!m_file || m_file->IsZombie()) {
            throw pool::Exception( string("ROOT cannot \"") + poolOptToRootOpt[m_mode] + "\" file " + m_fileName,
                                   "RootCollection::open", 
                                   "RootCollection" );
         }
         m_poolOut << coral::Info << "File " << m_fileName << " opened" << coral::MessageStream::endmsg;
      }

      if( m_mode == ICollection::READ || m_mode == ICollection::UPDATE ) {
         // retrieve the TTree from file 
         m_tree = getCollectionTree();
         if( !m_tree ) {
	   // m_file->Write();
	   
	   int n(0);
	   if (m_fileMgr == 0) {
	     m_file->Close();
	   } else {
	     n = m_fileMgr->close(m_file,"RootCollection");
	   }

	   if (n == 0) delete m_file; 
	   m_file=0;
            throw pool::Exception( string("POOL Collection TTree not found in file ") + m_fileName,
                                   "RootCollection::open", 
                                   "RootCollection" );
         }
         m_schemaEditor = new RootCollectionSchemaEditor( *this, m_description, m_tree );
         m_schemaEditor->readSchema();
      }

      if( m_mode == ICollection::CREATE || m_mode == ICollection::CREATE_AND_OVERWRITE ) {
        // create a new TTree
        if( 0 && m_mode == ICollection::CREATE_AND_OVERWRITE ) {
          m_poolOut << coral::Warning <<  "Cleaning previous collection object from the file..." << coral::MessageStream::endmsg;
          std::string treeName(c_treeName); treeName += ";*";
          m_file->Delete(treeName.c_str());
          m_file->Delete("Schema;*");
        }
        m_tree = new TTree(c_treeName, m_name.c_str());
        m_poolOut << coral::Debug << "Created Collection TTree. Collection file will be " << m_fileName << coral::MessageStream::endmsg;
        m_schemaEditor = new RootCollectionSchemaEditor( *this, m_description, m_tree );
        m_schemaEditor->createTreeBranches();
      }

      m_dataEditor = new RootCollectionDataEditor( m_description, m_tree, m_poolOut );
      m_poolOut << coral::Info <<  "Root collection opened, size = " << m_tree->GetEntries() << corENDL;
      TTree::SetMaxTreeSize(TTREE_MAX_SIZE);
      m_tree->SetAutoFlush(TTREE_AUTO_FLUSH);

      if( m_session && m_mode == ICollection::UPDATE ) {
        m_tree->SetDirectory(dirsav);

	int n(0);
	if (m_fileMgr == 0) {
	  m_file->Close();
	} else {
	  n = m_fileMgr->close(m_file,"RootCollection");
	}

        if (n == 0) delete m_file; 
	m_file =0;
      }
      m_open = true;
    }
    catch( std::exception &e ) {
       m_poolOut << coral::Debug << "Open() failed with expception: " << e.what() << corENDL;
       cleanup();
       throw;
    }

     
    bool RootCollection::isOpen() const{
      return m_open;
    }

     
    ICollection::OpenMode RootCollection::openMode() const
    {
      return m_mode;
    }

     
    bool RootCollection::fileCatalogRequired() const
    {
      return m_name.find("PFN:")==0 || 
        m_name.find("FID:")==0 || 
        m_name.find("LFN:")==0; 
    }

     
    string RootCollection::retrievePFN() const {
      if (m_name.substr (0, 4) != "PFN:")
        throw pool::Exception( "In CREATE mode a PFN has to be provided",
        "RootCollection::open", 
        "RootCollection");
      return m_name.substr(4,string::npos);
    }

     
    string  RootCollection::retrieveFID() const {

      FileCatalog::FileID fid="";
      string fileType="";        

      if (m_name.substr (0, 4) == "PFN:") {
        string pfn = m_name.substr(4,string::npos);
        m_fileCatalog->start();
        m_fileCatalog->lookupFileByPFN(pfn,fid,fileType);
        m_fileCatalog->commit();
      }
      else if (m_name.substr (0, 4) == "LFN:") {
        string lfn = m_name.substr(4,string::npos);
        m_fileCatalog->start();
        m_fileCatalog->lookupFileByLFN(lfn,fid);
        m_fileCatalog->commit();
      }
      else if (m_name.substr (0, 4) == "FID:") {
        fid = m_name.substr(4,string::npos);
      }else
        throw pool::Exception( "A FID, PFN or and LFN has to be provided",
        "RootCollection::retrieveFID", 
        "RootCollection");
      return fid;
    }


   string RootCollection::retrieveUniquePFN(const FileCatalog::FileID& fid) const
   {
      IFileCatalog::Files       pfns;
      m_fileCatalog->start();
      m_fileCatalog->getPFNs(fid, pfns);
      m_fileCatalog->commit();
      if( pfns.empty() )
         throw pool::Exception( "This exception should never have been thrown, please send a bug report",
                                "RootCollection::retrieveUniquePFN", 
                                "RootCollection"); 
      if( pfns.size() > 1 )
         throw pool::Exception( "Cannot UPDATE or CREATE_AND_OVERWRITE since there are replicas",
                                "RootCollection::retrieveUniquePFN", 
                                "RootCollection");
      return pfns[0].first;
    }

   
    const ICollectionDescription& RootCollection::description() const
    {
      return m_description;
    }

     
    ICollectionSchemaEditor& RootCollection::schemaEditor()
    {
      if ( m_mode == pool::ICollection::READ )   {
        std::string errorMsg = "Cannot modify the schema of a collection in READ open mode.";
        throw pool::Exception( errorMsg,
          "RootCollection::schemaEditor",
          "RootCollection" );
      } 
      return *m_schemaEditor; 
    }

     
    ICollectionDataEditor& RootCollection::dataEditor()
    {
      if( m_mode == pool::ICollection::READ ) {
        throw pool::Exception( "Cannot modify the data of a collection in READ open mode.", "RootCollection::dataEditor", "RootCollection" );
      }
      return *m_dataEditor;
    }

     
    ICollectionQuery* RootCollection::newQuery() const
    {
       if( !isOpen() ) {
          throw pool::Exception( "Attempt to query a closed collection.", "RootCollection::dataEditor", "RootCollection" );
       }
       return new RootCollectionQuery( m_description, m_tree );
    }

     
    ICollectionMetadata&   RootCollection::metadata()
    {
      if( !m_metadata ) {
         delayedFileOpen("metadata");
         m_metadata = new RootCollectionMetadata();
         m_metadata->initialize( m_file, m_mode );
      }
      return *m_metadata;
    }

     
     // Checks for the existence of a fragment in the collection 
     bool
     RootCollection::exists( const std::string& fragmentName,
                             bool /* setForUpdate */,
                             bool /* checkChildFragments  */) const
     {
        try{
           m_description.collectionFragment( fragmentName );
           return true;
        }catch( pool::Exception& ) {
           return false;
        }
     }

     

     void methodNotImplemented(const std::string &method)
     {
        throw pool::Exception( std::string("method not implemented"),
                               std::string("RootCollection::") + method,
                               "RootCollection" );
     }

          
    bool
      RootCollection::drop( const std::string& /* fragmentName */,
      bool /* dropChildFragments */,
      bool /* ignoreExternalDependencies */)
    {
      methodNotImplemented("drop");
      return false;
    }

    bool
      RootCollection::rename( const std::string& /* oldName */,
      const std::string& /* newName  */)
    {
      methodNotImplemented("rename");
      return false;
    }

    bool
      RootCollection::grantToUser( const std::string& /* userName */,
      pool::ICollection::Privilege /* privilege */,
      const std::string& /* fragmentName */,
      bool /* grantForChildFragments */)
    {
      methodNotImplemented("grantToUser");
      return false;
    }

    bool
      RootCollection::revokeFromUser( const std::string& /* userName */,
      pool::ICollection::Privilege /* privilege */,
      const std::string& /* fragmentName */,
      bool /* revokeForChildFragments */)
    {
      methodNotImplemented("revokeFromUser");
      return false;
    }

    bool
      RootCollection::grantToPublic( const std::string& /* fragmentName */,
      bool /* grantForChildFragments */)
    {
      methodNotImplemented("grantToPublic");
      return false;
    }

    bool
      RootCollection::revokeFromPublic( const std::string& /* fragmentName */,
      bool /* revokeForChildFragments */)
    {
      methodNotImplemented("revokeFromPublic");
      return false;
    }
  }
}
