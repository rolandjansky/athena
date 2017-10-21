/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  Concrete implementation of the XML Meta Data catalog. 
  Auhor: Maria Girone
*/
#include "XMLCatalog/XMLMetaDataCatalog.h"
#include "XMLCatalog/XMLFileCatalog.h"

#include "FileCatalog/FCSystemTools.h"
#include "FileCatalog/FCException.h"

#include "PoolXMLFileCatalog.h"

#include "CoralBase/AttributeListException.h"

#include <set>
#include <exception>

   
pool::XMLMetaDataCatalog::XMLMetaDataCatalog(FCImpl* fc) :
      FCMetaImpl(fc),
      AthMessaging( 0, "APR.XMLMetaDataCatalog" )
{
   this->setLevel( FCSystemTools::GetOutputLvl() ); 
   x_fc=static_cast<XMLFileCatalog*>(m_fc);  
   XMLcon = 0;
}


pool::XMLMetaDataCatalog::~XMLMetaDataCatalog()
{
}

void
pool::XMLMetaDataCatalog::createMetaDataSpec(MetaDataEntry& meta) const 
{
  MetaDataEntry m_spec;
  this->getMetaDataSpec(m_spec);
  if ( m_spec.size() > 1 ){    
    throw FCduplicatemetadataspecException
      ("XMLMetaDataCatalog::createMetaDataSpec");
    return;
  }
  
  try{
    x_fc->ClassHandle();
    PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
    //coral::AttributeListSpecification schemadef = meta.spec();
    coral::AttributeList::const_iterator isp=meta.attrs().begin();  
    coral::AttributeList::const_iterator isq=meta.attrs().end();  
    std::vector<std::string> meta_attri; 
    std::vector<std::string> meta_type; 
    for ( ;isp!=isq; ++isp){
      if ( isp->specification().name() != "guid" ) {
        meta_attri.push_back(isp->specification().name());
        meta_type.push_back(isp->specification().typeName());
      }
    }
    _xmlcatalog->setMeta(meta_attri,meta_type);
    x_fc->to_be_updated = true;
  }
  catch( std::exception& er){
    ATH_MSG_FATAL( "Cannot create metadata schema: " << er.what() );
    throw FCbackendException("XMLMetaDataCatalog::createMetaDataSpec", er.what());        
  }
}

void 
pool::XMLMetaDataCatalog::updateMetaDataSpec(MetaDataEntry& meta,
                                         const FileCatalog::MetaUpdateMode metamode) const
{
  try{
    x_fc->ClassHandle();
    //coral::AttributeListSpecification schemadef = meta.spec();
    MetaDataEntry oldmeta;
    this->getMetaDataSpec(oldmeta);
    if( oldmeta.attrs()==meta.attrs() ){
       ATH_MSG_INFO("old schema equals the new one, do nothing");
      return;
    }

    PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);

    if( metamode==FileCatalog::DELETE_REDUNDANT ){
      for( coral::AttributeList::iterator it=oldmeta.attrs().begin(); 
           it!=oldmeta.attrs().end(); ++it){
        std::string colname=it->specification().name();
	try{
	  meta.attrs()[colname].specification();
	}catch( const coral::AttributeListException&){
	  _xmlcatalog->dropGuidMetas(colname);        
	}
      }
    }
    
    std::vector<std::string> meta_attri; 
    std::vector<std::string> meta_type; 
    meta.attrs().merge(oldmeta.attrs());

    for( coral::AttributeList::iterator it=meta.attrs().begin(); 
         it!=meta.attrs().end(); ++it){
      meta_attri.push_back(it->specification().name());
      meta_type.push_back(it->specification().typeName());
    }
    if (meta_attri.size() > 0 ){
      x_fc->to_be_updated = true;
      _xmlcatalog->setMeta(meta_attri,meta_type);
    }
    x_fc->to_be_updated = true;
  }

  catch( std::exception& er)
  {
     ATH_MSG_FATAL("Cannot update metadata schema: " << er.what() );
     throw FCbackendException("XMLMetaDataCatalog::updateMetaDataSpec", er.what());        
  }
}

void
pool::XMLMetaDataCatalog::getMetaDataSpec(MetaDataEntry& meta) const
{
  try{
    x_fc->ClassHandle();
    PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);

    std::map<std::string,std::string> guidMeta = 
      _xmlcatalog->getMeta();
    std::map<std::string,std::string>::iterator iMeta;

    if(!guidMeta.empty()){
      for (iMeta = guidMeta.begin(); iMeta!=guidMeta.end(); iMeta++){
        if (iMeta->first!="guid")
          meta.addAttributeSpec(iMeta->first,iMeta->second);
      }
    }
  }
  catch( std::exception& er ){
     ATH_MSG_FATAL("Cannot get metadata schema: " << er.what() );
     throw FCbackendException("XMLMetaDataCatalog::getMetaDataSpec", er.what());
  }  
}

void pool::XMLMetaDataCatalog::dropMetaDataSpec() const
{
  x_fc->ClassHandle();
  PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
  _xmlcatalog->dropGuidMetas();        
  x_fc->to_be_updated = true;

}

void
pool::XMLMetaDataCatalog::insertMetaData(MetaDataEntry& mentry) const
{
  try{    
    x_fc->ClassHandle();
    PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
    FileCatalog::FileID fid = mentry.guid();
    coral::AttributeList arow = mentry.attrs();
    if (arow.size()==0) return;
    for(coral::AttributeList::const_iterator it=arow.begin(); it!=arow.end(); ++it){      
      if (it->specification().name() != "guid"){
        if ( _xmlcatalog->getGuidMeta(fid,it->specification().name()).empty() ){
	  if(it->specification().typeName()=="string"){
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),it->data<std::string>());
	  }else if(it->specification().typeName()=="short"){
	    short value=it->data<short>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<short>(value));
	  }else if(it->specification().typeName()=="unsigned short"){
	    unsigned short value=it->data<unsigned short>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<unsigned short>(value));
	  }else if(it->specification().typeName()=="int"){
	    int value=it->data<int>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<int>(value));
	  }else if(it->specification().typeName()=="unsigned int"){
	    unsigned int value=it->data<unsigned int>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<unsigned int>(value));
	  }else if(it->specification().typeName()=="long"){
	    long value=it->data<long>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<long>(value));
	  }else if(it->specification().typeName()=="unsigned long"){
	    unsigned long value=it->data<unsigned long>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<unsigned long>(value));
	  }else if(it->specification().typeName()=="long long"){
	    long long value=it->data<long long>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<long long>(value));
	  }else if(it->specification().typeName()=="unsigned long long"){
	    unsigned long long value=it->data<unsigned long long>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<unsigned long long>(value));
	  }else if(it->specification().typeName()=="float"){
	    float value=it->data<float>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<float>(value));
	  }else if(it->specification().typeName()=="double"){
	    double value=it->data<double>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<double>(value));
	  }else if(it->specification().typeName()=="long double"){
	    long double value=it->data<long double>();
	    _xmlcatalog->setGuidMeta(fid,it->specification().name(),pool::fc::to_string<long double>(value));
	  }else{
	    throw FCbackendException("XMLMetaDataCatalog::insertMetaData ", std::string("unsuported type ")+it->specification().typeName());
	  }
	}else{
	  throw FCduplicatemetadataException("XMLCatalog::insertMetaData"); 
	}
      }
    }
    x_fc->to_be_updated = true;
  }catch( pool::FCduplicatemetadataException& /* m */ ){
    throw FCduplicatemetadataException("XMLCatalog::insertMetaData"); 
  }
  catch( std::exception& er)    {
     ATH_MSG_FATAL("Cannot insert MetaData: " << er.what() );
     throw FCbackendException("XMLMetaDataCatalog::insertMetaData", er.what());
  }
}


void
pool::XMLMetaDataCatalog::deleteMetaData(const FileCatalog::FileID& fid) const
{
  x_fc->ClassHandle();
  PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
  if ( !_xmlcatalog->existsFileID(fid) )
    throw FCnonexistentFileException("XMLMetaDataCatalog::deleteMetaData",fid);
 
  _xmlcatalog->dropMetas(fid);
  x_fc->to_be_updated = true;
}


bool
pool::XMLMetaDataCatalog::retrievePFN(const std::string& query, 
                                      FCBuf<PFNEntry>& buf,
                                      const size_t& start)
{
  x_fc->ClassHandle();
  buf.clear();
  PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
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
pool::XMLMetaDataCatalog::retrieveLFN(const std::string& query,
                                      FCBuf<LFNEntry>& buf,
                                      const size_t& start) 
{
  x_fc->ClassHandle();
  buf.clear();
  PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
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
pool::XMLMetaDataCatalog::retrieveMetaData(const std::string& query,
                                           FCBuf<MetaDataEntry>& buf,
                                           const size_t& start) 
{
  x_fc->ClassHandle();
  buf.clear();
  PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
  std::vector<std::vector<PoolXMLFileCatalog::poolMeta> > files;
  //ZX: begin fix, if query is empty, should retrieve all; should pass buf.maxsize() instead of buf.size(). 
  std::string myquery(query);
  if (query.empty()){
    myquery="guid like %";
  }
  files = _xmlcatalog->getAllMetaDatabyq(myquery,start,buf.maxsize());
  //end fix
  for (unsigned int i = 0 ; i < files.size(); i++){ 
    //ZX: begin fix, mentry should be created inside loop
    MetaDataEntry mentry;
    this->getMetaDataSpec(mentry);
    //ZX: end fix
    std::vector<PoolXMLFileCatalog::poolMeta> mymeta = files[i];
    std::vector<PoolXMLFileCatalog::poolMeta>::iterator idata = mymeta.begin();
    for (unsigned int i=0; i<mymeta.size(); i++){
      try{
        std::string name=idata->first;
	std::string type=(mentry.attrs()[name]).specification().typeName();
        if(name == "guid") {
          mentry.setGuidValue(idata->second);
        }else{
	  if(type=="string"){
	    mentry.setAttributeValue<std::string>(name,idata->second);
	  }else if(type=="int"){
            int value;
	    pool::fc::from_string<int>(value,idata->second,std::dec);
            mentry.setAttributeValue<int>(name,value);
          }else if(type=="unsigned int"){
	    unsigned int value;
	    pool::fc::from_string<unsigned int>(value,idata->second,std::dec);
            mentry.setAttributeValue<unsigned int>(name,value);
	  }else if(type=="short"){
	    short value;
	    pool::fc::from_string<short>(value,idata->second,std::dec);
            mentry.setAttributeValue<short>(name,value);
	  }else if(type=="unsigned short"){
	    unsigned short value;
	    pool::fc::from_string<unsigned short>(value,idata->second,std::dec);
            mentry.setAttributeValue<unsigned short>(name,value);
	  }else if(type=="long"){
	    unsigned long value;
	    pool::fc::from_string<unsigned long>(value,idata->second,std::dec);
            mentry.setAttributeValue<long>(name,value);
	  }else if(type=="unsigned long"){
	    unsigned long value;
	    pool::fc::from_string<unsigned long>(value,idata->second,std::dec);
            mentry.setAttributeValue<unsigned long>(name,value);
	  }else if(type=="long long"){
	    long long value;
	    pool::fc::from_string<long long>(value,idata->second,std::dec);
            mentry.setAttributeValue<long long>(name,value);
	  }else if(type=="unsigned long long"){
	    unsigned long long value;
	    pool::fc::from_string<unsigned long long>(value,idata->second,std::dec);
            mentry.setAttributeValue<unsigned long long>(name,value);
	  }else if(type=="float"){
	    float value;
	    pool::fc::from_string<float>(value,idata->second,std::dec);
            mentry.setAttributeValue<float>(name,value);
	  }else if(type=="double"){
	    double value;
	    pool::fc::from_string<double>(value,idata->second,std::dec);
            mentry.setAttributeValue<double>(name,value);
	  }else if(type=="long double"){
	    long double value;
	    pool::fc::from_string<long double>(value,idata->second,std::dec);
            mentry.setAttributeValue<long double>(name,value);
	  }else{
	    throw FCbackendException("XMLMetaDataCatalog::retrieveMetaData",
				     std::string("unsupported type ")+type);
	  }
	}
      }
      catch( std::exception& e)
      {
        throw e;
      }
      idata++;
    }
    buf.push_back(mentry);    
  }
  return (files.size() > 0);
}  


bool
pool::XMLMetaDataCatalog::retrieveGuid(const std::string& query, 
                                       FCBuf<FileCatalog::FileID>& buf, 
                                       const size_t& start ){
  x_fc->ClassHandle();
  buf.clear();
  PoolXMLFileCatalog* _xmlcatalog = x_fc->castConnectionHandle(x_fc->XMLcon);
  std::string myquery(query);
  std::vector<FileCatalog::FileID> fids;
  if( query.empty() ){
    myquery="guid like %";
  }  
  fids= _xmlcatalog->getAllGuidbyq(myquery,start,buf.maxsize());
  std::copy(fids.begin(),fids.end(),std::back_inserter(buf.getData())); 
  return (buf.size()>0);
}
//end fix

