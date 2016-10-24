/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Concrete implementation of the XML catalog. 
  Auhor: Maria Girone
*/

#include "FileCatalog/FCException.h"
#include "FileCatalog/FCSystemTools.h"
#include "XMLCatalog/XMLFileCatalog.h"
#include "PoolXMLFileCatalog.h"

#include <cstdlib>
#include <set>
#include <stdexcept>

//#include <iostream>
//using namespace std;

//DD quick hack to work around NULL pointer passed in from PersistencySvc
static pool::PoolXMLFileCatalog *lastConnection  =0;


pool::XMLFileCatalog::XMLFileCatalog() :
      AthMessaging( 0, "APR.PFC.XML" ),
      is_started(false),
      to_be_updated(false),
      dontstart(false)
{  
   msg().setLevel( FCSystemTools::GetOutputLvl() );
   XMLcon=0;
}


pool::XMLFileCatalog::~XMLFileCatalog()
{
   if(XMLcon){
      delete XMLcon;
      XMLcon=0;
   }
}


void 
pool::XMLFileCatalog::connect() 
{
   try{
      ATH_MSG_DEBUG("Connecting");
      if ( !m_transactionsta.test(1) ){    //if connection flag is not set
         m_transactionsta.set(1);    //set connection flag to 1, print state
      }else{
         throw FCTransactionException("XMLFileCatalog::connect",
                                      "catalog is already connected");
      }
      XMLcon = new PoolXMLFileCatalog(m_url);
      lastConnection = XMLcon;    
   }
   catch( std::exception& er )  {
      throw FCconnectionException("XMLFileCatalog::connect",er.what());
   }
}




void
pool::XMLFileCatalog::disconnect() const
{
   try{
      if( m_transactionsta.test(1) ){ //if connection flag is set
         m_transactionsta.reset(1);//reset connection flag to 0
      }else {
         throw FCTransactionException( "XMLFileCatalog::disconnect",
                                       "catalog is already disconnected");
      }
    
      lastConnection = 0;
      PoolXMLFileCatalog* _xmlcatalog = this->castConnectionHandle(XMLcon);
      _xmlcatalog->setUpdate(to_be_updated);
      delete XMLcon;
      XMLcon = 0;
      ATH_MSG_DEBUG("Disconnected");

   }
   catch( std::exception& er)  {
      throw FCconnectionException("XMLFileCatalog::disconnect", er.what());
   }
}




void
pool::XMLFileCatalog::start() const
{
   if( m_transactionsta.test(1) ){ //if connected
      if( !m_transactionsta.test(0) ) { //if not already intransaction
         m_transactionsta.set(0); //set transaction flag
      }else{ 
         throw FCTransactionException
            ("XMLFileCatalog::start",
             std::string("catalog is already in transaction, ") + 
             std::string("cannot start another transaction"));
      }
   } else { //if not connected
      throw FCTransactionException
         ("XMLFileCatalog::start",
          "catalog is not connected, cannot start transaction");
   }

   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   try{
      if (! is_started ) {
         is_started = true;
         if (!dontstart) 
            _xmlcatalog->StartXMLFileCatalog();    
      }
      dontstart = false;
   } 
   catch ( std::exception& er ) {
      ATH_MSG_WARNING("Cannot start the catalog");
      throw FCTransactionException("XMLFileCatalog::start",er.what());
   }
   catch (...)  {
      ATH_MSG_WARNING("Unknown error when starting");
      throw FCTransactionException("XMLFileCatalog::start", 
                                   "Cannot start the catalog");
   }
}




void
pool::XMLFileCatalog::commit( pool::FileCatalog::CommitMode cm) const
{
   //check if in transaction 
   if( !m_transactionsta.test(0) || !m_transactionsta.test(1) ){
      throw FCTransactionException
         ("XMLFileCatalog::commit",
          " not in transaction state, cannot commit");
   }   
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   m_transactionsta.reset(0);//unset Intransaction flag
   is_started = false;      
  
   if (_xmlcatalog->isitReadOnly()) {
      //   xmllog << coral::Warning 
      //   << "Cannot store on read-only system"<<coral::MessageStream::endmsg;
   }else if (!to_be_updated){
      ATH_MSG_VERBOSE("Commit: nothing to update");
   }else{
      ATH_MSG_DEBUG("Committing updates");
      try{
         _xmlcatalog->storeOn();
      }
      catch ( std::exception& er )    {
         throw FCTransactionException("XMLFileCatalog::commit", er.what());
      }
      catch (...) {
         std::string message("Unknown error during commit");
         ATH_MSG_WARNING(message);
         throw FCTransactionException("XMLFileCatalog::commit",message);
      }      
   }  
   switch (cm) 
   {
    case FileCatalog::REFRESH: {
       dontstart = false;
       break;
    }
    case FileCatalog::ONHOLD: {
       dontstart = true;
       break;
    }
   }
}




void
pool::XMLFileCatalog::rollback() const
{   
   if( !m_transactionsta.test(0)|| !m_transactionsta.test(1) ){
      throw FCTransactionException
         ("XMLFileCatalog::rollback",
          "not in transaction state, cannot rollback");
   }
   try{
      this->ClassHandle();
      PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
      _xmlcatalog->getback();
   }
   catch ( std::exception& er )   {
      ATH_MSG_WARNING("Cannot rollback the XML File Catalog");
      throw FCTransactionException("XMLFileCatalog::rollback", er.what());
   }
   m_transactionsta.reset(0);//unset Intransaction flag
   is_started=false;
}




void
pool::XMLFileCatalog::registerPFN(const std::string& pfn, 
                                  const std::string& filetype,
                                  FileCatalog::FileID& fid)  const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = this->castConnectionHandle(XMLcon);

   ATH_MSG_DEBUG("Register PFN: " << pfn << " with GUID: " << fid << " in " << _xmlcatalog->connection());
//   cout << "MN: this=" << this << endl;
   
   PoolXMLFileCatalog::PFNStatus pfnsta = _xmlcatalog->statusPFN(pfn);
   switch(pfnsta)
   {
    case PoolXMLFileCatalog::NOTEXISTS: {
       if (fid.empty())
       {
          fid = _xmlcatalog->generateFileID();
       }
       _xmlcatalog->registerFile(fid,pfn,filetype);
       to_be_updated = true;
       break;
    }
    case PoolXMLFileCatalog::REGISTERED: {
       ATH_MSG_WARNING("Physical Name " << pfn << " is already registered");
       throw FCduplicatePFNException("XMLFileCatalog::registerFile"); 
       break;  
    }
   }  
}




void
pool::XMLFileCatalog::registerLFN(const std::string& pfn,                                 const std::string& lfn) const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);    

   if ( pfn.empty() && !lfn.empty() ) {
      ATH_MSG_INFO("Logical filenames must be associated to physical filenames"); 
      return;
   }
   if (lfn.empty() ) {
      ATH_MSG_INFO("You must specify a valid logical file name");
      return;
   }
   if ( _xmlcatalog->existsLFN(lfn) ){
      ATH_MSG_WARNING("Logical filename "<< lfn <<" already in the catalog");
      throw FCduplicateLFNException("XMLFileCatalog::registerFilename");
   }
   PoolXMLFileCatalog::PFNStatus rc = _xmlcatalog->statusPFN(pfn);
   switch (rc) 
   {
    case PoolXMLFileCatalog::NOTEXISTS: {
       ATH_MSG_WARNING("Physical filename "<< pfn <<" is not registered");
       throw FCnonexistentFileException("XMLFileCatalog::registerFilename", pfn);
    }
    case PoolXMLFileCatalog::REGISTERED: {
       FileCatalog::FileID fid = 
          _xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME, pfn);
       _xmlcatalog->setFile_onFileID(PoolXMLFileCatalog::LFNAME, fid, lfn, "");
       to_be_updated = true;
       break;     
    }
   }
}








void
pool::XMLFileCatalog::addReplicaPFN(const std::string& pfn, 
                                    const std::string& rpf) const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   if( pfn.empty() ) {
      ATH_MSG_INFO("Null physical filename to be replicated");
      return;    
   }
   if ( rpf.empty() ) {
      ATH_MSG_INFO("Null replica filename");
      return;
   } 
   if ( _xmlcatalog->statusPFN(rpf) != PoolXMLFileCatalog::NOTEXISTS){
      ATH_MSG_INFO("Replicated Filename is already registered");
      throw FCduplicatePFNException("XMLFileCatalog::addReplicaPFN "+rpf); 
   }
  
   PoolXMLFileCatalog::PFNStatus rc = _xmlcatalog->statusPFN(pfn);
   switch (rc) 
   {
    case PoolXMLFileCatalog::NOTEXISTS: 
       throw FCnonexistentFileException("XMLFileCatalog::addReplicaPFN", pfn);
    case PoolXMLFileCatalog::REGISTERED: {
       FileCatalog::FileID fid = 
          _xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME, pfn);
       std::string typ = _xmlcatalog->getfiletype(pfn);
       _xmlcatalog->setFile_onFileID(PoolXMLFileCatalog::PFNAME, fid, rpf, typ);
       to_be_updated = true;
       break; 
    }
   }
}




void
pool::XMLFileCatalog::addPFNtoGuid(const FileCatalog::FileID& guid, 
                                   const std::string& pf, const std::string& filetype ) const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   if ( pf.empty() ) {
      ATH_MSG_INFO("addPFNtoGuid: empty PFN specified");
      return;    
   }
   if ( guid.empty() ) {
      ATH_MSG_INFO("addPFNtoGuid: add PFN to a null guid");
      return;
   }
  
   if ( !_xmlcatalog->existsFileID(guid) )
      throw FCnonexistentFileException("XMLFileCatalog::addPFNtoGuid",guid);

   PoolXMLFileCatalog::PFNStatus rc = _xmlcatalog->statusPFN(pf);
   if ( rc != PoolXMLFileCatalog::NOTEXISTS){
      ATH_MSG_INFO("Replicated Filename is already registered");
      throw FCduplicatePFNException("XMLFileCatalog::addPFNtoGuid "+pf); 
   }
  
   _xmlcatalog->setFile_onFileID(PoolXMLFileCatalog::PFNAME,
                                 guid,pf,filetype);
   to_be_updated = true;
}




void
pool::XMLFileCatalog::renamePFN(const std::string& pfn, 
                                const std::string& newpfn) const
{
   this->addReplicaPFN(pfn,newpfn);
   this->deletePFN(pfn);
}




void
pool::XMLFileCatalog::lookupFileByPFN(const std::string& pfn, 
                                      FileCatalog::FileID& fid, 
                                      std::string& filetype) const
{

   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   ATH_MSG_DEBUG("Lookup PFN: " << pfn << " in " << _xmlcatalog->connection());
//   cout << "MN: this=" << this << endl;

   fid = _xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME, pfn);
   if (fid.empty() ) {
      ATH_MSG_DEBUG("lookupFilebyPFN: Empty File ID found");
      return;    
   }
   filetype = _xmlcatalog->getfiletype(pfn);
}




void
pool::XMLFileCatalog::lookupFileByLFN(const std::string& lfn, 
                                      FileCatalog::FileID& fid) const 
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   fid=_xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::LFNAME,lfn);
   if (fid.empty() ) {
      ATH_MSG_DEBUG("lookupFilebyLFN: Empty File ID found");
      return;
   }
}




void
pool::XMLFileCatalog::lookupBestPFN(const FileCatalog::FileID& fid, 
                                    const FileCatalog::FileOpenMode& omode, 
                                    const FileCatalog::FileAccessPattern& amode,
                                    std::string& pfn,
                                    std::string& filetype) const
{  
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);

   switch (omode){
    case FileCatalog::READ:{break;}
    case FileCatalog::WRITE:{break;}
    case FileCatalog::UPDATE:{break;}
   }
   switch (amode){
    case FileCatalog::SEQUENTIAL:{break;}
    case FileCatalog::RANDOM:{break;}
    case FileCatalog::PRANDOM:{break;}
   }

   pfn = _xmlcatalog->getFile_FROM_FileId(PoolXMLFileCatalog::PFNAME, fid);
   if (pfn.empty() ) {
      ATH_MSG_DEBUG("lookupBestPFN: empty physical filename found");
   }
   filetype = _xmlcatalog->getfiletype(pfn);
} 




void
pool::XMLFileCatalog::insertPFN(PFNEntry& pentry) const
{  
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
  
   std::string pfn         = pentry.pfname();
   FileCatalog::FileID fid = pentry.guid();
   std::string typ         = pentry.filetype();

   if (_xmlcatalog->statusPFN(pfn) == PoolXMLFileCatalog::NOTEXISTS){
      _xmlcatalog->insertFile(fid,pfn,typ);
      to_be_updated = true;
   }else{
      throw FCduplicatePFNException("XMLFileCatalog::insertPFN "+pfn);
   }    
}




void
pool::XMLFileCatalog::insertLFN(LFNEntry& lentry) const
{  
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon); 
  
   std::string lfn         = lentry.lfname();
   FileCatalog::FileID fid = lentry.guid();
   if ( !_xmlcatalog->existsLFN(lfn) ) {
      _xmlcatalog->setFile_onFileID(PoolXMLFileCatalog::LFNAME,
                                    fid,lfn,std::string(""));
      to_be_updated = true;
   }else{
      throw FCduplicateLFNException("XMLFileCatalog::insertLFN "+lfn);
   }
}




void
pool::XMLFileCatalog::deletePFN(const std::string& pfn) const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
    
   //check that this is the last pfn and delete the whole node File.
   FileCatalog::FileID fid=
      _xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME, pfn);
   if ( fid.empty() )
      throw FCnonexistentFileException("XMLFileCatalog::deletePFN",pfn);
  
  
   _xmlcatalog->delFile(PoolXMLFileCatalog::PFNAME,pfn);
   to_be_updated = true;
   std::string xpf = 
      _xmlcatalog->getFile_FROM_FileId(PoolXMLFileCatalog::PFNAME, fid);
   if (xpf.empty() )
      _xmlcatalog->delFileID(fid);


} 




void
pool::XMLFileCatalog::deleteLFN(const std::string& lfn) const
{  
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);

   if ( !_xmlcatalog->existsLFN(lfn))
      throw FCnonexistentFileException("XMLFileCatalog::deleteLFN",lfn);
 
   _xmlcatalog->delFile(PoolXMLFileCatalog::LFNAME,lfn);  
   to_be_updated = true;
}




void
pool::XMLFileCatalog::deleteEntry(const FileCatalog::FileID& fid) const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   if ( !_xmlcatalog->existsFileID(fid) )
      throw FCnonexistentFileException("XMLFileCatalog::deleteMetaData",fid);

   _xmlcatalog->delFileID(fid);
   to_be_updated = true;
}




bool
pool::XMLFileCatalog::isReadOnly() const
{
   this->ClassHandle();
   PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);
   return _xmlcatalog->isitReadOnly();
}




bool
pool::XMLFileCatalog::retrievePFN(const std::string& query, 
                                  FCBuf<PFNEntry>& buf,
                                  const size_t& start)
{
   this->ClassHandle();
   buf.clear();
   PoolXMLFileCatalog* _xmlcatalog = this->castConnectionHandle(XMLcon);
   std::vector<PoolXMLFileCatalog::poolFNs> files;
   //ZX: begin fix, if query is empty, should retrieve all; should pass buf.maxsize() instead of buf.size(). 
   std::string myquery(query);
   if (query.empty()){
      myquery="guid like %";
   }
   files = _xmlcatalog->getAllpfnsbyq(myquery,start,buf.maxsize());   
   //end fix
   for (unsigned int i = 0 ; i < files.size(); i++){ 
      buf.push_back(PFNEntry(files[i].fname, files[i].guid, files[i].filetype));
   }
   return (files.size() > 0);
}




bool
pool::XMLFileCatalog::retrieveLFN(const std::string& query,
                                  FCBuf<LFNEntry>& buf,
                                  const size_t& start) 
{
   this->ClassHandle();
   buf.clear();
   PoolXMLFileCatalog* _xmlcatalog = this->castConnectionHandle(XMLcon);
   std::vector<PoolXMLFileCatalog::poolFNs> files;
   //ZX: begin fix, if query is empty, should retrieve all; should pass buf.maxsize() instead of buf.size(). 
   std::string myquery(query);
   if (query.empty()){
      myquery="guid like %";
   }
   files = _xmlcatalog->getAlllfnsbyq(myquery,start,buf.maxsize());
   //end fix
  
  
   for (unsigned int i = 0 ; i < files.size(); i++){ 
      buf.push_back(LFNEntry(files[i].fname, files[i].guid));
   }
   return (files.size() > 0);
}




bool
pool::XMLFileCatalog::retrieveGuid(const std::string& query, 
                                   FCBuf<FileCatalog::FileID>& buf, 
                                   const size_t& start ){
   this->ClassHandle();
   buf.clear();
   PoolXMLFileCatalog* _xmlcatalog = this->castConnectionHandle(XMLcon);
   std::string myquery(query);
   std::vector<FileCatalog::FileID> fids;
   if( query.empty() ){
      myquery="guid like %";
   }  
   fids= _xmlcatalog->getAllGuidbyq(myquery,start,buf.maxsize());
   std::copy(fids.begin(),fids.end(),std::back_inserter(buf.getData())); 
   return (buf.size()>0);
}




void
pool::XMLFileCatalog::ClassHandle() const
{
   if ( !is_started) {
      ATH_MSG_WARNING("Catalog not started, please use XMLFileCatalog::start()");
      throw FCbackendException("XMLFileCatalog::ClassHandle",
                               "start() not called");
   }
}




pool::PoolXMLFileCatalog*
pool::XMLFileCatalog::castConnectionHandle(PoolXMLFileCatalog* con)
{
   PoolXMLFileCatalog* x = (con ? con : lastConnection);
   if (!x) 
      throw FCconnectionException("XMLFileCatalog::castConnectionHandle",
                                  "cannot find a connection");  
   return x;
}

