/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Concrete implementation of the XML catalog. 
  Auhor: Maria Girone
*/
#include "XMLCatalog/XMLFileCatalog.h"
#include "CoralBase/MessageStream.h"
#include "FileCatalog/FCException.h"
#include "PoolXMLFileCatalog.h"

#include <cstdlib>
#include <set>
#include <stdexcept>

using namespace pool;

// declare types provided by this Catalog plugin
#ifdef HAVE_GAUDI_PLUGINSVC
DECLARE_COMPONENT_WITH_ID(XMLFileCatalog, "xmlcatalog")
#else
#include "Reflex/PluginService.h"
PLUGINSVC_FACTORY_WITH_ID(XMLFileCatalog, std::string("xmlcatalog"), FCImpl*())
#endif

//DD quick hack to work around NULL pointer passed in from PersistencySvc
static pool::PoolXMLFileCatalog *lastConnection  =0;




pool::XMLFileCatalog::XMLFileCatalog(): is_started(false),
                                        to_be_updated(false),
                                        dontstart(false)
{  
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
    coral::MessageStream xmllog( "XMLFileCatalog" );
    xmllog << coral::Info<< "Connecting to the catalog"<<coral::MessageStream::endmsg;
    if ( !m_transactionsta.test(1) ){ //if connection flag is not set
      m_transactionsta.set(1);//set connection flag to 1, print state
    }else{
      throw FCTransactionException("XMLFileCatalog::connect",
                                   "catalog is already connected");
    }
    XMLcon = new PoolXMLFileCatalog(m_url);
    lastConnection = XMLcon;    
  }
  catch( std::exception& er )
  {
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
    coral::MessageStream xmllog( "XMLFileCatalog" );
    xmllog << coral::Info << "Disconnected" << coral::MessageStream::endmsg;  
  }
  catch( std::exception& er)
  {
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  try{
    if (! is_started ) {
      is_started = true;
      if (!dontstart) 
        _xmlcatalog->StartXMLFileCatalog();    
    }
    dontstart = false;
  } 
  catch ( std::exception& er ) {
    xmllog << coral::Fatal << "Cannot start the catalog"<<coral::MessageStream::endmsg;
    throw FCTransactionException("XMLFileCatalog::start",er.what());
  }
  catch (...)
  {
    xmllog << coral::Fatal << "Unknown error"<<coral::MessageStream::endmsg;
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  
  m_transactionsta.reset(0);//unset Intransaction flag
  is_started = false;      
  
  if (_xmlcatalog->isitReadOnly()) {
    //   xmllog << coral::Warning 
    //   << "Cannot store on read-only system"<<coral::MessageStream::endmsg;
  }else if (!to_be_updated){
    xmllog << coral::Debug
           << "There were no updates in the catalog"<<coral::MessageStream::endmsg;      
  }else{ 
    xmllog << coral::Info << "Updating the catalog"<<coral::MessageStream::endmsg;
    try{
      _xmlcatalog->storeOn();
    }
    catch ( std::exception& er )
    {
      throw FCTransactionException("XMLFileCatalog::commit",er.what());
    }
    catch (...){
      std::string message("XMLFileCatalog exception: cannot store");
      xmllog << coral::Fatal << message << coral::MessageStream::endmsg;
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
    coral::MessageStream xmllog( "XMLFileCatalog" );
    std::string message("Cannot rollback the XML File Catalog");
    xmllog << coral::Fatal << message << coral::MessageStream::endmsg;
    throw FCTransactionException("XMLFileCatalog::rollback",er.what());
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

  coral::MessageStream xmllog( "XMLFileCatalog" );    

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
      xmllog << coral::Error <<"Physical Name "<<pfn
             <<" is already registered"<<coral::MessageStream::endmsg;  
      throw FCduplicatePFNException("XMLFileCatalog::registerFile"); 
      break;  
    }
  }  
}




void
pool::XMLFileCatalog::registerLFN(const std::string& pfn, 
                                  const std::string& lfn) const
{
  this->ClassHandle();
  PoolXMLFileCatalog* _xmlcatalog = castConnectionHandle(XMLcon);    
  coral::MessageStream xmllog( "XMLFileCatalog" );

  if ( pfn.empty() && !lfn.empty() ) {
    xmllog << coral::Info 
           <<"Logical filenames must be associated to physical filenames" 
           <<coral::MessageStream::endmsg;
    return;
  }
  if (lfn.empty() ) {
    xmllog<< coral::Info
          << "You must specify a valid logical file name"
          <<coral::MessageStream::endmsg;
    return;
  }
  if ( _xmlcatalog->existsLFN(lfn) ){
    xmllog<< coral::Error
          <<"Logical filename "<< lfn <<" already in the catalog"
          <<coral::MessageStream::endmsg;
    throw FCduplicateLFNException("XMLFileCatalog::registerFilename");
    return;
  }
  PoolXMLFileCatalog::PFNStatus rc = _xmlcatalog->statusPFN(pfn);
  switch (rc) 
    {
    case PoolXMLFileCatalog::NOTEXISTS: {
      xmllog<<coral::Error<<"Physical filename "<<pfn
            <<" is not registered"<<coral::MessageStream::endmsg;
      throw FCnonexistentFileException
        ("XMLFileCatalog::registerFilename", 
         pfn);
      break;
    }
    case PoolXMLFileCatalog::REGISTERED: {
      FileCatalog::FileID fid = 
        _xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME, 
                                         pfn);
      _xmlcatalog->setFile_onFileID(PoolXMLFileCatalog::LFNAME,
                                    fid,lfn,std::string(""));
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  
  if ( pfn.empty() ) {
    xmllog<<coral::Info <<"Null physical filename to be replicated" 
          <<coral::MessageStream::endmsg;
    return;    
  }
  if ( rpf.empty() ) {
    xmllog <<coral::Info<<"Null replica filename" <<coral::MessageStream::endmsg;
    return;
  } 
  if ( _xmlcatalog->statusPFN(rpf) != PoolXMLFileCatalog::NOTEXISTS){
    xmllog<<coral::Info <<"Replicated Filename is already registered" 
          <<coral::MessageStream::endmsg;
    throw FCduplicatePFNException("XMLFileCatalog::addReplicaPFN "+rpf); 
  }
  
  PoolXMLFileCatalog::PFNStatus rc = _xmlcatalog->statusPFN(pfn);
  switch (rc) 
  {
    case PoolXMLFileCatalog::NOTEXISTS: {
      throw FCnonexistentFileException("XMLFileCatalog::addReplicaPFN",
                                       pfn);
      break;
    }
    case PoolXMLFileCatalog::REGISTERED: {
      FileCatalog::FileID fid = 
        _xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME, pfn);
      std::string typ = _xmlcatalog->getfiletype(pfn);
      _xmlcatalog->setFile_onFileID(PoolXMLFileCatalog::PFNAME,
                                    fid,rpf,typ);
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  
  if ( pf.empty() ) {
    xmllog<<coral::Info <<"addPFNtoGuid: empty PFN specified" 
          <<coral::MessageStream::endmsg;
    return;    
  }
  if ( guid.empty() ) {
    xmllog<<coral::Info <<"addPFNtoGuid: add PFN to a null guid" 
          <<coral::MessageStream::endmsg;
    return;
  }
  
  if ( !_xmlcatalog->existsFileID(guid) )
    throw FCnonexistentFileException("XMLFileCatalog::addPFNtoGuid",guid);

  PoolXMLFileCatalog::PFNStatus rc = _xmlcatalog->statusPFN(pf);
  if ( rc != PoolXMLFileCatalog::NOTEXISTS){
    xmllog<<coral::Info <<"Replicated Filename is already registered" 
          <<coral::MessageStream::endmsg;
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  fid=_xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::PFNAME,pfn);
  if (fid.empty() ) {
    xmllog<<coral::Debug <<"lookupFilebyPFN: Empty File ID found"
          <<coral::MessageStream::endmsg;
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  fid=_xmlcatalog->getFileID_FROM_File(PoolXMLFileCatalog::LFNAME,lfn);
  if (fid.empty() ) {
    xmllog<<coral::Debug 
          <<"lookupFilebyLFN: Empty File ID found" <<coral::MessageStream::endmsg;
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
  coral::MessageStream xmllog( "XMLFileCatalog" );

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

  pfn = _xmlcatalog->getFile_FROM_FileId(PoolXMLFileCatalog::PFNAME, 
                                         fid);
  if (pfn.empty() ) {
    xmllog << coral::Debug << "empty physical filename found" 
           <<pfn<<coral::MessageStream::endmsg;   
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
  coral::MessageStream xmllog( "XMLFileCatalog" );
  if ( !is_started) {
    xmllog<< coral::Error 
          <<"Catalog not started, please use XMLFileCatalog::start()"
          <<coral::MessageStream::endmsg;
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

