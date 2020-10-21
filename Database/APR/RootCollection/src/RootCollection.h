/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTCOLLECTION_ROOTCOLLECTION_H
#define ROOTCOLLECTION_ROOTCOLLECTION_H

#include "RootCollectionDataEditor.h"
#include "RootCollectionSchemaEditor.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/CollectionDescription.h"

#include "FileCatalog/IFileCatalog.h"
#include "CoralBase/MessageStream.h"

#include "GaudiKernel/IFileMgr.h"
#include "Gaudi/PluginService.h"

#include <string>
#include <vector>

class TTree;
class TFile;
class IFileMgr;


namespace pool {

   class ISession; 

   namespace RootCollection {

      // compressed data size for buffer auto-resize (negative == bytes)
      const int TTREE_AUTO_FLUSH = -1*1024*1024;
      
      const int TBRANCH_DEF_BUFSIZE = 8*1024;

      // increase TTree size limit to 100GB
      const unsigned long long TTREE_MAX_SIZE = 100*1024*1024*1024ULL;

      class Attribute;
      class AttributeSpecification;
      class RootCollectionMetadata;
  
      /**
         @brief Collection (and CollectionProxy) implementation based on ROOT trees
  
         Implementation details: 
         - Token and meta data attributes are stored in a simple TTree 
         - Tokens are stored as (compressed) C-string
         - Each attribute is written to a separate branch of the TTree  
         - You can "play" with the collection tree in an interactive ROOT session 
         - The TTree is stored to the ROOT file: collectionName.root 
         - Pool FileCatalog can be utilized to locate/register the collection files
         (see: RootCollection::RootCollection)
         - Selection is performed using TTreePlayer 
         - Description of query syntax can be found at http://root.cern.ch/root/htmldoc/TTree.html#TTree:Draw
         - It is possible to read only subsets of the AttributeList 
         - This feature is controlled via the "option" parameter of the RootCollection::select method 
         - e.g. RootCollection::select( primaryQuery, secondaryQuery ,"ATTRIBUTE_LIST attribute23 attribute42") 
         - Support for remote collection access via rootd 
         - (experimental) 
         - Support for a simple server side selection 
         - (experimental) 
         . 
         . 
         ROOT documentation can be found at http://root.cern.ch/ 
      */
      class RootCollection :  public ICollection {
    
     public:
	typedef Gaudi::PluginService::Factory<ICollection*( const ICollectionDescription*, ICollection::OpenMode, ISession*)> Factory;
    
        /// Constructor
        /// @param session If you want to access the referenced objects you have to provide an ISession
        /// @param connection The location of the collection file is uniquely defined by the parameters name and connection
        /// @param name The location of the collection file is uniquely defined by the parameters name and connection
        /// @param mode The open mode of the collection
        ///
        /// - Without use of FileCatalog:
        ///   - The path to the collection file is simply created by the following concatenation:\n
        ///     connection+name+".root"
        ///   - name: Name of the collection file
        ///   - connection:
        ///     - It can be a relative or absolute path
        ///     - In case of an empty connection string it is assumed that the file is located in the current directory
        ///     - Remote access via rootd: e.g. "root://pcepsft02.cern.ch:9090//localdisk/ \n 
        ///       Further documentation can be found in the class description of TNetFile
        ///       (http://root.cern.ch/root/html/TNetFile.html)
        /// .
        /// .
        /// .
        /// - Utilization of FileCatalog:
        ///   - This mode is triggered if the name parameter starts with one of the following prefixes
        ///     "PFN:", "FID:" or "LFN:". 
        ///   - According to the prefix the name is interpreted as 
        ///     Physical File Name, unique File ID or Logical File Name
        ///   - The connection string is interpreted as URI of the FileCatalog. 
        ///     The collection retrieves the FileCatalog defined by the given URI from FileCatalogMap.
        ///     A default file catalog (empty connection string) can be defined there.
        ///   - This mode works also in the context of MultiCollection. 
        ///     If a collection created by a MultiCollection iterator is a RootCollection using  FileCatalog,
        ///     it will use the same catalog as its multi collection parent. 
        ///     On the other side the connection string defining
        ///     the FileCatalog of a RootCollection is ignored when a RootCollection using FileCatalogs is
        ///     added to a MultiCollection 
        /// .
        /// .

    
        RootCollection(  const pool::ICollectionDescription* description,
                         pool::ICollection::OpenMode mode,
                         pool::ISession* );

        /// Destructor
        ~RootCollection();
    
        /// Return openMode
        virtual ICollection::OpenMode openMode() const; 

        /// Explicitly re-opens the collection after it has been closed.
        virtual void open();
    
        /// Checks if the collection is open.
        virtual bool isOpen() const;

        /// Commits the last changes made to the collection
        virtual void commit( bool restartTransaction = false );
    
        /// Aborts the last changes made to the collection
        virtual void rollback();
    
        /// Explicitly closes the collection
        virtual void close();
    
        virtual bool exists( const std::string& fragmentName,
                             bool setForUpdate = false,
                             bool checkChildFragments = false ) const;

        virtual bool drop( const std::string& fragmentName,
                           bool dropChildFragments = false,
                           bool ignoreExternalDependencies = false );


        virtual bool rename( const std::string& oldName,
                             const std::string& newName );


        virtual bool grantToUser( const std::string& userName,
                                  pool::ICollection::Privilege privilege,
                                  const std::string& fragmentName = "",
                                  bool grantForChildFragments = false );


        virtual bool revokeFromUser( const std::string& userName,
                                     pool::ICollection::Privilege privilege,
                                     const std::string& fragmentName = "",
                                     bool revokeForChildFragments = false );

        virtual bool grantToPublic( const std::string& fragmentName = "",
                                    bool grantForChildFragments = false );

        virtual bool revokeFromPublic( const std::string& fragmentName = "",
                                       bool revokeForChildFragments = false );
 


        /// Returns an object used to describe the collection properties.
        virtual const ICollectionDescription& description() const;

        /// Returns an object used to modify the collection schema.
        virtual ICollectionSchemaEditor& schemaEditor();

        /// Returns an object used to add, update or delete rows of the collection.
        virtual ICollectionDataEditor&         dataEditor();

        /// Returns an object used to query the collection.
        virtual ICollectionQuery*         newQuery() const;

        /// Returns an object used to access collection metadata
        virtual ICollectionMetadata&        metadata();
        
     private:
    
        /// copying unimplemented in this class.
        RootCollection(const RootCollection &);
        /// copying unimplemented in this class.
        RootCollection & operator = (const RootCollection &);
    
        void delayedFileOpen( const std::string& method );
        TTree* getCollectionTree();
        void setupTree() const;
        void readAttributeListSpecification() const;
        void writeAttributeListSpecification();

        bool fileCatalogRequired()const;
        std::string retrievePFN()const;
        std::string retrieveFID()const;
        std::string retrieveUniquePFN(const FileCatalog::FileID& fid)const;
        std::string retrieveBestPFN(const FileCatalog::FileID& fid)const;  
        void retrieveFileCatalog()const;

        void cleanup();
        
     public:
        
        static const unsigned int c_maxLengthOfStrings = 5000;
        static const char* const poolOptToRootOpt[];
	static const Io::IoFlags poolOptToFileMgrOpt[];
        static const char* const c_tokenBranchName;// = "Token";
        static const char* const c_attributeListLayoutName;// = "Schema"; 
        static const char* const c_oldTreeName;// = "CollectionTree";
        static const char* const c_treeName;// = "POOLCollectionTree";

     private:

        mutable CollectionDescription        m_description;
        
        std::string                         m_name;
        std::string                         m_fileName;
        ICollection::OpenMode                 m_mode;
        mutable TTree*                         m_tree;
        mutable TFile*                         m_file;
        mutable ISession*                    m_session;
        bool                                 m_open;
        bool                                 m_readOnly;
        
        mutable std::unique_ptr<pool::IFileCatalog> m_fileCatalog;
        mutable coral::MessageStream        m_poolOut;

        RootCollectionSchemaEditor*        m_schemaEditor;
        RootCollectionDataEditor*        m_dataEditor;

        RootCollectionMetadata*                m_metadata;

	IFileMgr*                         m_fileMgr;

      };
   }
}
#endif



