/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLURL.hpp>

#include "FileCatalog/URIParser.h"
#include "PersistentDataModel/Guid.h"
#include "CoralBase/MessageStream.h"
#include "PoolXMLFileCatalog.h"
#include "XMLDOMErrorMessanger.h"
#include "DTDRedirect.h"
#include "XMLQueryHandle.h"

#include <vector>
#include <fstream>
#include <stdexcept>

using namespace xercesc;

const std::string pool::PoolXMLFileCatalog::MainNode  = "POOLFILECATALOG";

const std::string pool::PoolXMLFileCatalog::METANode        = "META";
const std::string pool::PoolXMLFileCatalog::ParentNodeName  = "File";
const std::string pool::PoolXMLFileCatalog::DaughtPNodeName = "physical";
const std::string pool::PoolXMLFileCatalog::DaughtLNodeName = "logical";
const std::string pool::PoolXMLFileCatalog::GranDaPNodeName = "pfn";
const std::string pool::PoolXMLFileCatalog::GranDaLNodeName = "lfn";

const std::string pool::PoolXMLFileCatalog::FileAtt_type    = "type";
const std::string pool::PoolXMLFileCatalog::FileAtt_ID      = "ID";
const std::string pool::PoolXMLFileCatalog::FileAtt_name    = "name";
const std::string pool::PoolXMLFileCatalog::FileAtt_fitype  = "filetype";

const std::string pool::PoolXMLFileCatalog::MetaNode        = "metadata";
const std::string pool::PoolXMLFileCatalog::FileAtt_attname = "att_name";
const std::string pool::PoolXMLFileCatalog::FileAtt_attvalu = "att_value";

int pool::PoolXMLFileCatalog::NumberOfIstances = 0;

namespace pool {
  inline std::string _toString(const XMLCh *toTranscode)
  {
    char * buff = XMLString::transcode(toTranscode);
    std::string tmp(buff);
    XMLString::release(&buff);
    return tmp;
  }
  
  
  inline XMLCh*  _toDOMS( std::string temp ){
    XMLCh* buff = XMLString::transcode(temp.c_str());    
    return  buff;
  }


  inline std::string addpref(const std::string& /*pref*/, const std::string& name){
    //    if (name.substr(0,pref.size()) == pref ) 
    //  return name;
    //   else
    //return pref + name;
    return name;
  }

  inline std::string nopref(const std::string& /*pref*/, const std::string& name){
    //    if (name.substr(0,pref.size()) == pref ) 
    //  return name.substr(pref.size(),name.npos);
    //else
    return name;
  }
}




bool 
pool::PoolXMLFileCatalog::finding(XMLQueryHandle& qhandle, 
                                  unsigned int i){
  bool tostore = false;
  if (metadata.find(qhandle.get_attrib(i)) != metadata.end()){
    switch (qhandle.is_wild(i))
      {
      case XMLQueryHandle::NONE:
        {
          if (metadata[qhandle.get_attrib(i)] == qhandle.get_value(i) )
            tostore = true;
          break;
        }
      case XMLQueryHandle::START:
        {
          if (metadata[qhandle.get_attrib(i)].
              substr(0,qhandle.get_value(i).length()) ==   
              qhandle.get_value(i))
            tostore = true;
          break;
        }
      case XMLQueryHandle::MIDDLE:
        {
          if (metadata[qhandle.get_attrib(i)].find(qhandle.get_value(i)) != 
              metadata[qhandle.get_attrib(i)].npos)
            tostore = true;
          break;
        }
      case XMLQueryHandle::END:
        {
          unsigned int lennam = qhandle.get_value(i).length();
          unsigned int lenatt = metadata[qhandle.get_attrib(i)].length();
          if (lenatt >= lennam && 
              metadata[qhandle.get_attrib(i)].substr(lenatt-lennam,lennam)==
              qhandle.get_value(i))
            tostore = true;
          break;
        }
      }
  }
  return tostore;
}




pool::PoolXMLFileCatalog::PoolXMLFileCatalog(const std::string& contactstring)
  :
  read_only(false),
  update(false),
  doc(0),
  parser(0),
  errMessanger(0),
  index_l(0),
  m_file(contactstring),
  nmeta(0),
  imeta(0),
  fmeta(true)
{
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  xmllog << ( NumberOfIstances? coral::Debug : coral::Info )
         << "Xerces-c initialization Number " << NumberOfIstances << coral::MessageStream::endmsg;
  if( NumberOfIstances==0 ) try { 
     XMLPlatformUtils::Initialize();  
  }
  catch (const XMLException& e) {
     xmllog<<coral::Fatal << "Xerces-c error in initialization \n"<< "Exception message is:  \n"<< pool::_toString(e.getMessage()) << coral::MessageStream::endmsg;
     throw std::runtime_error( "Standard pool exception : Fatal Error on pool::PoolXMLFileCatalog" );
  }
  ++NumberOfIstances;
}



pool::PoolXMLFileCatalog::~PoolXMLFileCatalog()
{
   delete parser;   parser = 0;
   delete errMessanger;   errMessanger = 0;

   --NumberOfIstances;

   coral::MessageStream xmllog( "PoolXMLFileCatalog" );
   xmllog << ( NumberOfIstances? coral::Debug : coral::Info )
          << "XercesC termination number "  << NumberOfIstances << coral::MessageStream::endmsg;

   if (NumberOfIstances == 0)
      XMLPlatformUtils::Terminate();
}


void
pool::PoolXMLFileCatalog::StartXMLFileCatalog() 
{  
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  xmllog<<coral::Verbose<<"Start parsing the catalog" <<coral::MessageStream::endmsg;
  std::string xmlFile;
  try{
    char* back = getenv("POOL_XMLBACKUP");
    bool dobackup = true;
    if (back){
      std::stringstream os;
      os << back;
      os >> dobackup;
    }
    xmlFile = this->getfile(dobackup);
  }
  catch ( std::exception& poole )
  {
    throw poole;
  }
  try{
    if (parser)
    {
      //doc->release();
      delete parser;
      parser=0;
    }    

    if( errMessanger )
    {
      delete errMessanger;
      errMessanger = 0;
    }

    mem_pfn.clear();
    mem_lfn.clear();
    mem_fid.clear();

    parser = new XercesDOMParser;     
    parser->setValidationScheme(XercesDOMParser::Val_Auto);
    parser->setDoNamespaces(false);      
    DTDRedirect dtdinmem;
    parser->setEntityResolver(&dtdinmem);
    errMessanger = new XMLDOMErrorMessanger();
    parser->setErrorHandler(errMessanger); 
    parser->parse(xmlFile.c_str());
    doc = parser->getDocument();

    this->getMeta();    
  }
  catch (const XMLException& e) {
    xmllog<<coral::Fatal<<"xml parse error in file " << xmlFile << "\n"<< "Exception message is:  \n"<< pool::_toString(e.getMessage()) << coral::MessageStream::endmsg;
    throw( std::runtime_error( "Standard pool exception : Fatal Error on pool::PoolXMLFileCatalog" ) );
  }
  catch (const DOMException& e) {
    xmllog<<coral::Fatal<<"xml parse error in file " << xmlFile << "\n"<<" DOMException code is: "<<e.code <<coral::MessageStream::endmsg;
    throw( std::runtime_error( "Standard pool exception : Fatal Error on PoolXMLFileCatalog" ) );
  }
  catch (...)
  {
    xmllog<<coral::Fatal<<"xml parse error in file " << xmlFile << coral::MessageStream::endmsg;
    throw( std::runtime_error( "Standard pool exception : Fatal Error on PoolXMLFileCatalog" ) );
  }
}




std::vector<pool::PoolXMLFileCatalog::poolFNs>
pool::PoolXMLFileCatalog::getAllpfns(const std::string& FileID)
{
  coral::MessageStream xmllog("PoolXMLFileCatalog" );
  xmllog <<coral::Verbose
         <<" Loading all physical file names for FileID "
         <<FileID<<coral::MessageStream::endmsg;
  std::vector<poolFNs> files;

  DOMNode* FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FIDNode){
    xmllog<<coral::Debug << "FileID "<<FileID
          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return files;
  }
  
  DOMNode* DaNode = this->getChild(FIDNode,DaughtPNodeName);
  while (!DaNode){
    xmllog<<coral::Warning<< "FileID "<<FileID<<" has not the <"
          <<DaughtPNodeName<<"> node"<<coral::MessageStream::endmsg;
    return files;
  }
  
  DOMNode* MyNode = this->getChild(DaNode,GranDaPNodeName);
  
  while (MyNode){
    int nodetype = MyNode->getNodeType();
    if ( nodetype == DOMNode::ELEMENT_NODE) {
      std::string pfn = nopref("pfn_",this->getAttr(MyNode,FileAtt_name));
      std::string typ = this->getAttr(MyNode,FileAtt_fitype);
      std::string fid = FileID;
      files.push_back(poolFNs(pfn,fid,typ));   
    }
    MyNode=MyNode->getNextSibling();
  }
  return files;
}




std::vector<pool::PoolXMLFileCatalog::poolFNs> 
pool::PoolXMLFileCatalog::getAllpfnsbyq(const std::string& query,
                                        unsigned int init, 
                                        unsigned int ntot)
{
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );  
  xmllog <<coral::Verbose
         <<coral::MessageStream::endmsg;
  std::vector<poolFNs> files;
  pool::XMLQueryHandle qhandle;
  // Deal with simple query guid=...
  qhandle.init(query);
  if (qhandle.nqueries() == 1 && 
      qhandle.get_attrib(0) == "guid" && 
      qhandle.get_comman(0) == "=") {
    std::vector<poolFNs> buffs = this->getAllpfns(qhandle.get_value(0));
    unsigned iend = (init+ntot > buffs.size()) ? buffs.size() : init+ntot;
    for (unsigned int i=init; i<iend ;i++)
      files.push_back(buffs[i]);
    return files; 
  }
  

  bool lfnused=false;
  bool metaused=false;
  for(unsigned int i=0;i<qhandle.nqueries();i++){
    if(qhandle.get_attrib(i) == "lfn" || qhandle.get_attrib(i) == "lfname")
      lfnused=true;
    if(metaGuid.find(qhandle.get_attrib(i)) != metaGuid.end())
      metaused=true;
  }

  XMLCh* name = pool::_toDOMS(GranDaPNodeName);
  unsigned int Num_of_Tags  = 
    doc->getElementsByTagName(name)->getLength();
  if (init==0) 
    index_l=0;
  if (ntot==0)
    ntot = Num_of_Tags;
  unsigned int nfound=0;
  while(nfound<ntot && index_l < Num_of_Tags){
    DOMNode* MyNode = 
      doc->getElementsByTagName(name)->item(index_l);

    std::string pfn = nopref("pfn_",(this->getAttr(MyNode,FileAtt_name)));
    std::string typ = this->getAttr(MyNode,FileAtt_fitype);
    DOMNode* IDNode = MyNode->getParentNode()->getParentNode();
    std::string fid = this->getAttr(IDNode, FileAtt_ID);
    
    metadata.clear();
    metadata["pfn"]=pfn;
    metadata["pfname"]=pfn;
    metadata[FileAtt_name]=pfn;
    metadata[FileAtt_fitype]=typ;
    metadata["guid"]=fid;
    
    std::vector<poolFNs> lfns;
    if (lfnused) 
      lfns = this->getAlllfns(fid);
    
    std::vector<poolMeta> metas;
    if(metaused)
      metas = this->getAllMetaData(fid);
    
    for (std::vector<poolMeta>::iterator im=metas.begin(); 
         im != metas.end(); im++){
      metadata[im->first]=im->second;
    }    

    bool good = true;
    
    for (unsigned int i=0; i<qhandle.nqueries(); i++){
      bool tostore = false;
      if (qhandle.get_attrib(i) == "lfn" || 
          qhandle.get_attrib(i) == "lfname"){
        for (std::vector<poolFNs>::iterator ilf = lfns.begin(); 
             ilf != lfns.end(); ilf++){          
          metadata["lfn"]    = ilf->fname;
          metadata["lfname"]    = ilf->fname;
          if (this->finding(qhandle, i)) 
            tostore = true;
        }
      }else{          
        tostore = this->finding(qhandle, i);
      }
      good = good && tostore;        
    }    
    if (good) {
      files.push_back(poolFNs(pfn,fid,typ));   
      nfound++;
    }
    index_l++;
  }
  XMLString::release(&name);
  return files;
}




std::vector<pool::PoolXMLFileCatalog::poolFNs> 
pool::PoolXMLFileCatalog::getAlllfnsbyq(const std::string& query,
                                        unsigned int init, 
                                        unsigned int ntot)
{  
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  xmllog <<coral::Verbose
         <<" Loading all logical file names for query"<<coral::MessageStream::endmsg;
  std::vector<poolFNs> files;
  pool::XMLQueryHandle qhandle;
  qhandle.init(query);
  if (qhandle.nqueries() == 0) return files;
  // Deal with simple query guid=...
  if (qhandle.nqueries() == 1 && 
      qhandle.get_attrib(0) == "guid" && 
      qhandle.get_comman(0) == "=") {
    std::vector<poolFNs> buffs = this->getAlllfns(qhandle.get_value(0));
    unsigned iend = (init+ntot > buffs.size()) ? buffs.size() : init+ntot;
    for (unsigned int i=init; i<iend ;i++)
      files.push_back(buffs[i]);
    return files; 
  }

  bool pfnused=false;
  bool metaused=false;
  for(unsigned int i=0;i<qhandle.nqueries();i++){
    if(qhandle.get_attrib(i) == "pfn" || qhandle.get_attrib(i) == "pfname")
      pfnused=true;
    if(metaGuid.find(qhandle.get_attrib(i)) != metaGuid.end())
      metaused=true;
  }        
  XMLCh* name = pool::_toDOMS(GranDaLNodeName);
  unsigned int Num_of_Tags  = 
    doc->getElementsByTagName(name)->getLength();
  if (init==0) 
    index_l=0;
  if (ntot==0)
    ntot = Num_of_Tags;
  unsigned int nfound=0;
  while(nfound<ntot && index_l < Num_of_Tags){
    DOMNode* MyNode = 
      doc->getElementsByTagName(name)->item(index_l);

    std::string lfn = nopref("lfn_",(this->getAttr(MyNode,FileAtt_name)));
    DOMNode* IDNode = MyNode->getParentNode()->getParentNode();
    std::string fid = this->getAttr(IDNode, FileAtt_ID);
    
    metadata.clear();
    metadata["lfn"]=lfn;
    metadata["lfname"]=lfn;
    metadata[FileAtt_name]=lfn;
    metadata["guid"]=fid;
    
    std::vector<poolFNs> pfns;
    if (pfnused) 
        pfns = this->getAllpfns(fid);    
    std::vector<poolMeta> metas;
    if(metaused)
      metas = this->getAllMetaData(fid);
    
    for (std::vector<poolMeta>::iterator im=metas.begin(); 
         im != metas.end(); im++){
      metadata[im->first]=im->second;
    }
      
    bool good = true;
    for (unsigned int i=0; i<qhandle.nqueries(); i++){      
      bool tostore = false;
      if(qhandle.get_attrib(i) == "pfn" || 
         qhandle.get_attrib(i) == "pfname"){
        for (std::vector<poolFNs>::iterator ilp = pfns.begin(); 
             ilp != pfns.end(); ilp++){          
          metadata["pfn"]    = ilp->fname;
          metadata["pfname"]    = ilp->fname;
          metadata[FileAtt_fitype]=ilp->filetype;
          if (this->finding(qhandle, i)) 
            tostore = true;
        }
      }else{
        tostore = this->finding(qhandle, i);
      }
      good = good && tostore;        
    }
    if (good) {
      files.push_back(poolFNs(lfn,fid));   
      nfound++;
    }
    index_l++;
  }
  XMLString::release(&name);
  return files;
}




std::vector<std::vector<pool::PoolXMLFileCatalog::poolMeta> >
pool::PoolXMLFileCatalog::getAllMetaDatabyq(const std::string& query,
                                            unsigned int init, 
                                            unsigned int ntot)
{  
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  xmllog <<coral::Verbose
         <<" Loading all Meta Data for query"
         <<coral::MessageStream::endmsg;
  metadata.clear();
  std::vector<std::vector<poolMeta> > attrs;
  pool::XMLQueryHandle qhandle;
  qhandle.init(query);
  if (qhandle.nqueries() == 0) return attrs;

  // Deal with simple query guid=...
  if (qhandle.nqueries() == 1 && 
      qhandle.get_attrib(0) == "guid" && 
      qhandle.get_comman(0) == "=") {
    if (init == 0 ) {
      std::vector<poolMeta> buffs = this->getAllMetaData(qhandle.get_value(0));
      attrs.push_back(buffs);
    }
    return attrs; 
  }
  bool pfnused=false;
  bool lfnused=false;
  for(unsigned int i=0;i<qhandle.nqueries();i++){
    if(qhandle.get_attrib(i) == "pfn" || qhandle.get_attrib(i) == "pfname")
      pfnused=true;
    if(qhandle.get_attrib(i) == "lfn" || qhandle.get_attrib(i) == "lfname")
      lfnused=true;
  }

  XMLCh* name = pool::_toDOMS(ParentNodeName);
  unsigned int Num_of_Tags  = 
    doc->getElementsByTagName(name)->getLength();
  if (init==0) 
    index_l=0;
  if (ntot==0)
    ntot = Num_of_Tags;
  unsigned int nfound=0;
  while(nfound<ntot && index_l < Num_of_Tags){
    DOMNode* MyNode = 
      doc->getElementsByTagName(name)->item(index_l);
    std::string fid = this->getAttr(MyNode,FileAtt_ID);
    std::vector<poolMeta> metas = this->getAllMetaData(fid);
    if (metas.size() > 0) {
      for (std::vector<poolMeta>::iterator im=metas.begin(); 
           im != metas.end(); im++){
        metadata[im->first]=im->second;
      }

      std::vector<poolFNs> pfns;
      if (pfnused) 
        pfns = this->getAllpfns(fid);
      
      std::vector<poolFNs> lfns;
      if (lfnused) 
        lfns = this->getAlllfns(fid);

      bool good = true;
      for (unsigned int i=0; i<qhandle.nqueries(); i++){        
        bool tostore = false;        
        if (qhandle.get_attrib(i) == "lfn" || 
            qhandle.get_attrib(i) == "lfname"){
          for (std::vector<poolFNs>::iterator ilf = lfns.begin(); 
               ilf != lfns.end(); ilf++){            
            metadata["lfn"]    = ilf->fname;
            metadata["lfname"]    = ilf->fname;
            if (this->finding(qhandle, i)) 
              tostore = true;
          }
        }else if(qhandle.get_attrib(i) == "pfn" || 
                 qhandle.get_attrib(i) == "pfname"){          
          for (std::vector<poolFNs>::iterator ilp = pfns.begin(); 
               ilp != pfns.end(); ilp++){
            
            metadata["pfn"]    = ilp->fname;
            metadata["pfname"]    = ilp->fname;
            metadata[FileAtt_fitype]=ilp->filetype;            
            if (this->finding(qhandle, i)) 
              tostore = true;
          }
        }else {
          tostore = this->finding(qhandle, i);
        }
        good = good && tostore;        
      }        
      if (good) {
        attrs.push_back(metas);   
        nfound++;
      }
    }else{
      xmllog<<coral::Verbose<<"No metadata for FileID "
            <<fid<<coral::MessageStream::endmsg;
    }
    index_l++;
  }
  XMLString::release(&name);
  return attrs;
}
  



std::vector<std::string >
pool::PoolXMLFileCatalog::getAllGuidbyq(const std::string& query,
                                        unsigned int init, 
                                        unsigned int ntot)
{ 
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  std::vector<std::string>  guids;
  pool::XMLQueryHandle qhandle;
  qhandle.init(query);
  if (qhandle.nqueries() == 0) return guids;
  
  // Deal with simple query guid=...
  if (qhandle.nqueries() == 1 && 
      qhandle.get_attrib(0) == "guid" && 
      qhandle.get_comman(0) == "=") {
    if (init == 0 ) {
      guids.push_back(qhandle.get_value(0));
    }
    return guids; 
  }
  bool pfnused=false;
  bool lfnused=false;
  bool metaused=false;
  for(unsigned int i=0;i<qhandle.nqueries();i++){
    if(qhandle.get_attrib(i) == "pfn" || qhandle.get_attrib(i) == "pfname")
      pfnused=true;
    if(qhandle.get_attrib(i) == "lfn" || qhandle.get_attrib(i) == "lfname")
      lfnused=true;
    if(metaGuid.find(qhandle.get_attrib(i)) != metaGuid.end())
      metaused=true;
  }

  XMLCh* name = pool::_toDOMS(ParentNodeName);
  unsigned int Num_of_Tags  = 
    doc->getElementsByTagName(name)->getLength();
  if (init==0) 
    index_l=0;
  if (ntot==0)
    ntot = Num_of_Tags;
  
  unsigned int nfound=0;
  while(nfound<ntot && index_l < Num_of_Tags){
    DOMNode* MyNode = 
      doc->getElementsByTagName(name)->item(index_l);
    
    std::string fid = this->getAttr(MyNode,FileAtt_ID);
    metadata["guid"]=fid;
    
    if (metaused){
      std::vector<poolMeta> metas = this->getAllMetaData(fid);
      if (metas.size() > 0) {
        for (std::vector<poolMeta>::iterator im=metas.begin(); 
             im != metas.end(); im++){
          metadata[im->first]=im->second;
        }
      }
    }
    
    std::vector<poolFNs> pfns;
    std::vector<poolFNs> lfns;
    if (pfnused) 
      pfns = this->getAllpfns(fid);  
    if (lfnused) 
      lfns = this->getAlllfns(fid);
    
    bool good = true;
    for (unsigned int i=0; i<qhandle.nqueries(); i++){        
      bool tostore = false;        
      if (qhandle.get_attrib(i) == "lfn" || 
          qhandle.get_attrib(i) == "lfname"){
        for (std::vector<poolFNs>::iterator ilf = lfns.begin(); 
             ilf != lfns.end(); ilf++){            
          metadata["lfn"]    = ilf->fname;
          metadata["lfname"]    = ilf->fname;
          if (this->finding(qhandle, i)) 
            tostore = true;
        }
      }else if(qhandle.get_attrib(i) == "pfn" || 
               qhandle.get_attrib(i) == "pfname"){          
        for (std::vector<poolFNs>::iterator ilp = pfns.begin(); 
             ilp != pfns.end(); ilp++){
          
          metadata["pfn"]    = ilp->fname;
          metadata["pfname"]    = ilp->fname;
          metadata[FileAtt_fitype]=ilp->filetype;            
          if (this->finding(qhandle, i)) 
            tostore = true;
        }
      } else {
        tostore = this->finding(qhandle, i);
      }
      good = good && tostore;        
    }        
    if (good) {
      guids.push_back(fid);   
      nfound++;
    }
    index_l++;
  }
  XMLString::release(&name);
  return guids;
}




std::vector<pool::PoolXMLFileCatalog::poolFNs> 
pool::PoolXMLFileCatalog::getAlllfns(const std::string& FileID)
{  
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  xmllog <<coral::Verbose<<" Loading all logical file names for FileID "
         <<FileID<<coral::MessageStream::endmsg;
  std::vector<poolFNs> files;
  
  DOMNode* FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FIDNode){
    xmllog<<coral::Warning << "FileID "<<FileID
          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return files;
  }
  
  DOMNode* DaNode = this->getChild(FIDNode,DaughtLNodeName);
  if (!DaNode){
    xmllog<<coral::Warning<< "FileID "<<FileID<<" has not the <"
          <<DaughtLNodeName<<"> node"<<coral::MessageStream::endmsg;
    return files;
  }
  
  DOMNode* FNNode = this->getChild(DaNode,GranDaLNodeName);
  while (FNNode){
    int nodetype = FNNode->getNodeType();
    if ( nodetype == DOMNode::ELEMENT_NODE) {
      std::string lfn = nopref("lfn_",this->getAttr(FNNode, FileAtt_name));
      files.push_back(poolFNs(lfn,FileID));
    }
    FNNode=FNNode->getNextSibling();
  }
  return files;
}




std::vector<pool::PoolXMLFileCatalog::poolMeta> 
pool::PoolXMLFileCatalog::getAllMetaData(const std::string& FileID)
{  
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );
  xmllog <<coral::Verbose<<" Loading all  MetaData for FileID "<<
    FileID<<coral::MessageStream::endmsg;

  std::vector<pool::PoolXMLFileCatalog::poolMeta> myattrs;
  DOMNode* FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FIDNode){
    xmllog<<coral::Warning << "FileID "<<FileID
          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return myattrs;
  }
  
  DOMNode* FNNode = this->getChild(FIDNode,MetaNode);
  while (FNNode){
    int nodetype = FNNode->getNodeType();
    if ( nodetype == DOMNode::ELEMENT_NODE) {
      std::string name = this->getAttr(FNNode, FileAtt_attname);
      std::string valu = this->getAttr(FNNode, FileAtt_attvalu);
      poolMeta coppia(name,valu);
      myattrs.push_back(coppia);      
    }
    FNNode=FNNode->getNextSibling();
  }
  if ( myattrs.size() > 0 ) {
    poolMeta coppia("guid",FileID);
    myattrs.push_back(coppia);
  }
  return myattrs;
}




std::string 
pool::PoolXMLFileCatalog::getFile_FROM_FileId (const pool::PoolXMLFileCatalog::
                                               FileNameType& fntype,
                                               const std::string & FileID)
{  
  /// Find the first available PFN
  DOMNode* FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FIDNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Debug << "FileID "<<FileID
          <<" is not in the catalog"<<coral::MessageStream::endmsg;
    return std::string();
  }
    
  std::string daughter,grandaug;
  this->selectType(fntype,daughter,grandaug);
  
  DOMNode* DaNode = this->getChild(FIDNode,daughter);
  if (!DaNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Debug<< "FileID "<<FileID<<" has not the <"
          <<daughter<<"> node"<<coral::MessageStream::endmsg;
    return std::string();
  }
  
  DOMNode* FNNode = this->getChild(DaNode,grandaug);
  if (!FNNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog <<coral::Debug <<"No "<<daughter<<" filename for FileID "
           <<FileID<<coral::MessageStream::endmsg;
    return std::string();
  }  
  return nopref(grandaug+std::string("_"),this->getAttr(FNNode,FileAtt_name));
}




std::string 
pool::PoolXMLFileCatalog::getFileID_FROM_File(const pool::PoolXMLFileCatalog::
                                              FileNameType & fntype, 
                                              const std::string& FName)
{
  // Find the FIleID. Assuming unique PFNames
  std::string daughter,grandaug;
  this->selectType(fntype,daughter,grandaug);
  
  DOMNode* FNNode = this->getNode(grandaug,FileAtt_name,FName);
  if (!FNNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Debug <<"filename "<<FName
          <<" does not exist"<<coral::MessageStream::endmsg; 
    return std::string();
  }  
  DOMNode* FIDNode = FNNode->getParentNode()->getParentNode();
  if (!FIDNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Debug << "FileID is missing for physical filename "
          <<FName<<coral::MessageStream::endmsg;
    return std::string();
  }
  return this->getAttr(FIDNode,FileAtt_ID);
}




// Deleters
void 
pool::PoolXMLFileCatalog::delFile(const pool::PoolXMLFileCatalog::
                                  FileNameType &fntype, 
                                  const std::string& FName)
{
  std::string daughter,grandaug;
  this->selectType(fntype,daughter,grandaug);
  
  DOMNode* FNNode = this->getNode(grandaug,FileAtt_name,FName);
  if (!FNNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Warning<<daughter<<" filename "<<FName
          <<" does not exist"<<coral::MessageStream::endmsg;
    return;
  }
  
  DOMNode* parentNode = FNNode->getParentNode();
  parentNode->removeChild(FNNode);        
  
  if ( fntype == pool::PoolXMLFileCatalog::PFNAME) {
    if (mem_pfn.find(FName) != mem_pfn.end()){
      mem_pfn.erase(FName);
    }
  }else{
    if (mem_lfn.find(FName) != mem_lfn.end()){
      mem_lfn.erase(FName);
    }
  }
}




void 
pool::PoolXMLFileCatalog::delFileID(const std::string & FileID)
{
  DOMNode* FileNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FileNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Warning<< "FileID "<<FileID
          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return;
  }
  DOMNode* parentNode = FileNode->getParentNode();
  parentNode->removeChild(FileNode);
  //Found in the memory and delete pfns and lfns from the maps
  if ( mem_fid.find(FileID) != mem_fid.end()){
    
    std::vector<poolFNs> files = this->getAlllfns(FileID);
    std::vector<poolFNs>::iterator ifil;
    for (ifil = files.begin(); ifil < files.end(); ifil++){ 
      std::string lfn    = (*ifil).fname;
      if ( mem_lfn.find(lfn) != mem_lfn.end() ) {
        mem_lfn.erase(lfn);
      }
    }
    files = this->getAllpfns(FileID);
    for (ifil = files.begin(); ifil < files.end(); ifil++){ 
      std::string pfn    = (*ifil).fname;
      if ( mem_pfn[pfn] != mem_pfn[pfn] ) mem_pfn.erase(pfn);
    }
    mem_fid.erase(FileID);
  }
}




// Setters
std::string 
pool::PoolXMLFileCatalog::generateFileID()
{
  Guid myuid;
  myuid.create(myuid);
  return std::string(myuid.toString());
}




void 
pool::PoolXMLFileCatalog::setFile_onFileID(const pool::PoolXMLFileCatalog::
                                           FileNameType& fntype, 
                                           const std::string & FileID, 
                                           const std::string & FName, 
                                           const std::string & filetype)
{
  DOMNode* FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FIDNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Warning<<"FileID "<<FileID
          <<" is not in the xml file"<<coral::MessageStream::endmsg;return;
  }
  
  std::string daughter,grandaug;
  this->selectType(fntype,daughter,grandaug);
  
  DOMNode* FNNode = this->getChild(FIDNode,daughter);
  if (!FNNode){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Warning<<"FileID "<<FileID
          <<" has not the <"<<daughter<<"> node"<<coral::MessageStream::endmsg;return;
  }
  XMLCh* name = pool::_toDOMS(grandaug);
  DOMElement* fnelem = doc->createElement(name);
  XMLString::release(&name);

  FNNode->appendChild(fnelem);
  this->setAttr(fnelem,FileAtt_name,addpref(grandaug+std::string("_"),FName));
  
  if (fntype == pool::PoolXMLFileCatalog::PFNAME) {
    mem_pfn[FName]=fnelem;
    this->setAttr(fnelem,FileAtt_fitype,filetype);
  }else{
    mem_lfn[FName]=fnelem;
  }
}


  

void 
pool::PoolXMLFileCatalog::insertFile(const std::string& FileID, 
                                     const std::string& fname, 
                                     const std::string& ftype)
{
  /// It creates a new node File with name = FileID in the XML file catalog   
  DOMNode* FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  if (!FIDNode){
    // Create it !
    XMLCh* name = pool::_toDOMS("*");
    DOMNode* fde      = doc->getElementsByTagName(name)->item(0);
    XMLString::release(&name);

    name = pool::_toDOMS(ParentNodeName);
    DOMElement* filelem = doc->createElement(name);
    XMLString::release(&name);

    this->setAttr(filelem, FileAtt_ID, FileID);
    mem_fid[FileID]=filelem;
    fde->appendChild(filelem);
    
    name = pool::_toDOMS(DaughtPNodeName);
    DOMElement* phyelem = doc->createElement(name);
    filelem->appendChild(phyelem);
    XMLString::release(&name);
    
    name = pool::_toDOMS(DaughtLNodeName);
    DOMElement* logelem = doc->createElement(name); 
    XMLString::release(&name);

    filelem->appendChild(logelem);
    FIDNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  }
  DOMNode* FNNode = this->getChild(FIDNode,DaughtPNodeName);
  
  XMLCh* name = pool::_toDOMS(GranDaPNodeName);
  DOMElement* fnelem = doc->createElement(name);
  XMLString::release(&name);

  this->setAttr(fnelem, FileAtt_name, addpref(std::string("pfn_"),fname));
  mem_pfn[fname]=fnelem;  
  this->setAttr(fnelem, FileAtt_fitype, ftype);

  if (!FNNode)
    throw( std::runtime_error( "Can't find daughter node: " + DaughtPNodeName));
  FNNode->appendChild(fnelem);
}
  



void 
pool::PoolXMLFileCatalog::registerFile(const std::string& FileID, 
                                       const std::string& fname, 
                                       const std::string& ftype)
{
  /// It creates a new node File with name = FileID in the XML file catalog 
 
  XMLCh* name = pool::_toDOMS("*");
  DOMNode* fde      = doc->getElementsByTagName(name)->item(0);
  XMLString::release(&name);
 
  name = pool::_toDOMS(ParentNodeName);
  DOMElement* filelem = doc->createElement(name);
  XMLString::release(&name);
  this->setAttr(filelem, FileAtt_ID, FileID);
  mem_fid[FileID]=filelem;
  
  fde->appendChild(filelem);
  
  name = pool::_toDOMS(DaughtPNodeName);
  DOMElement* phyelem = doc->createElement(name);
  XMLString::release(&name);

  filelem->appendChild(phyelem);
  
  name = pool::_toDOMS(DaughtLNodeName);
  DOMElement* logelem = doc->createElement(name);
  XMLString::release(&name);
  filelem->appendChild(logelem);
  
  name = pool::_toDOMS(GranDaPNodeName);
  DOMElement* fnelem = doc->createElement(name);
  XMLString::release(&name);
  phyelem->appendChild(fnelem);
  this->setAttr(fnelem, FileAtt_name, addpref(std::string("pfn_"),fname));
  mem_pfn[fname] = fnelem;

  this->setAttr(fnelem, FileAtt_fitype, ftype);

  this->setUpdate(true);
}




std::string 
pool::PoolXMLFileCatalog::registerFile(const std::string& fname)
{  
  DOMNode* PFNNode = this->getNode(GranDaPNodeName, FileAtt_name,fname);
  if (!PFNNode)
    throw( std::runtime_error( "Can't find node: " + GranDaPNodeName + " " + fname));
  DOMNode* FIDNode = PFNNode->getParentNode()->getParentNode();    
  std::string fid = this->getAttr(FIDNode,FileAtt_ID);
  return fid;
}




void 
pool::PoolXMLFileCatalog::storeOn()
{  
  std::string xmlfile;
  try{
    xmlfile = this->getfile(false);
  }
  catch ( std::exception& e )
  {
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog << coral::Fatal << "Cannot open output file "<<coral::MessageStream::endmsg;
    throw e;
  }
  catch (...)
  {
    throw( std::runtime_error( "Unknown exception : Fatal Error on PoolXMLFileCatalog" ) );
  }
  try {
    XMLCh *ii = pool::_toDOMS("LS");
    DOMImplementation *impl = 
      DOMImplementationRegistry::getDOMImplementation(ii);
    XMLString::release(&ii);
#if XERCES_VERSION_MAJOR < 3
    DOMWriter         *theSerializer = 
      ((DOMImplementationLS*)impl)->createDOMWriter();
    XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(xmlfile.c_str());
    theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    theSerializer->writeNode(myFormTarget, *doc);
    theSerializer->release();
    delete  myFormTarget;
    theSerializer=0;
    myFormTarget=0;    
#else
    DOMLSSerializer *theSerializer = 
      ((DOMImplementationLS*)impl)->createLSSerializer();
    XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(xmlfile.c_str());
    theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    // See http://xerces.apache.org/xerces-c/program-dom-3.html
    DOMLSOutput* theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
    theOutput->setByteStream(myFormTarget);
    theSerializer->write(doc, theOutput);
    theSerializer->release();
    theOutput->release();
    delete myFormTarget;
    theSerializer=0;
    theOutput=0;
    myFormTarget=0;    
#endif
  }
  catch (...)
  {
    throw ( std::runtime_error( "Unknown exception : Fatal Error on PoolXMLFileCatalog" ) );
  }
}




pool::PoolXMLFileCatalog::PFNStatus 
pool::PoolXMLFileCatalog::statusPFN(const std::string & PFName) 
{
  DOMNode* MyNode = this->getNode(GranDaPNodeName,FileAtt_name,PFName);
  pool::PoolXMLFileCatalog::PFNStatus rc = pool::PoolXMLFileCatalog::NOTEXISTS;
  if ( MyNode ){
    rc = pool::PoolXMLFileCatalog::REGISTERED;
  }else{
    rc = pool::PoolXMLFileCatalog::NOTEXISTS;
  }
  return rc;
}
  



bool 
pool::PoolXMLFileCatalog::existsLFN(const std::string & LFName)
{
  DOMNode* MyNode = this->getNode(GranDaLNodeName,FileAtt_name,LFName);
  return (MyNode!=0);
}




bool 
pool::PoolXMLFileCatalog::existsFileID(const std::string & FileID)
{
  DOMNode* MyNode = this->getNode(ParentNodeName,FileAtt_ID,FileID);
  return (MyNode!=0);
}




bool 
pool::PoolXMLFileCatalog::isitReadOnly()
{
  return read_only;
}




DOMNode* 
pool::PoolXMLFileCatalog::getNode(const std::string& nodename, 
                                  const std::string& attr, 
                                  const std::string& attrname) 
{
  DOMNode* MyNode = 0;
  if (nodename == GranDaPNodeName && attr == FileAtt_name ){
    XMLCh* name = pool::_toDOMS(addpref("pfn_",attrname));
    MyNode = doc->getElementById(name);
    XMLString::release(&name);
    if (!MyNode) {
      if (mem_pfn.find(attrname) != mem_pfn.end()){
        MyNode = mem_pfn[attrname];
        if (!MyNode) {
          // back compatibility with POOL_1_3_3
          XMLCh* attr2 = pool::_toDOMS(attrname);
          MyNode = doc->getElementById(attr2);          
          XMLString::release(&attr2);
        }
      }
    }
    return MyNode;
  }

  if (nodename == GranDaLNodeName && attr == FileAtt_name ){
    XMLCh* name = pool::_toDOMS(addpref("lfn_",attrname));
    MyNode = doc->getElementById(name); 
    XMLString::release(&name);
    if (!MyNode) {
      if (mem_lfn.find(attrname) != mem_lfn.end()){
        MyNode = mem_lfn[attrname];
        // back compatibility with POOL_1_3_3
        if (!MyNode) {
          XMLCh* attr2 = pool::_toDOMS(attrname);
          MyNode = doc->getElementById(attr2);
          XMLString::release(&attr2);
        }
      }
    }
    return MyNode;
  }
  
  if (nodename ==  ParentNodeName && attr == FileAtt_ID ){
    XMLCh* attr2 = pool::_toDOMS(attrname);
    MyNode = doc->getElementById(attr2);
    XMLString::release(&attr2);
    if (!MyNode) {
      if (mem_fid.find(attrname) != mem_fid.end()){
        MyNode = mem_fid[attrname];
      }
    }
    return MyNode;
  }
  
  XMLCh* name = pool::_toDOMS(nodename);
  unsigned int Num_of_Tags  = 
    doc->getElementsByTagName(name)->getLength();
  for ( unsigned int index=0; index < Num_of_Tags ; index++){
    
    MyNode = doc->getElementsByTagName(name)->item(index);
    if ( attrname == this->getAttr(MyNode,attr))
      return MyNode;
  }  
  XMLString::release(&name);

  return MyNode;
}
  



DOMNode* 
pool::PoolXMLFileCatalog::getChild( DOMNode* Parent, 
                                    const std::string & ChildTagName)
{
  return this->getChildwithAttr(Parent,ChildTagName,
                                std::string(),std::string());
}




DOMNode* 
pool::PoolXMLFileCatalog::getChildAttr( DOMNode* Parent, 
                                        const std::string& ChildTagName,
                                        const std::string& ChildAttr)
{
  return this->getChildwithAttr(Parent,ChildTagName,
                                ChildAttr,std::string());
}




DOMNode* 
pool::PoolXMLFileCatalog::getChildwithAttr(DOMNode* Parent, 
                                           const std::string& ChildTagName, 
                                           const std::string & AttName, 
                                           const std::string & AttValue)
{
  if (Parent){
    DOMNode* child = Parent->getFirstChild();
    while( child != 0){
      int nodetype = child->getNodeType();
      if ( nodetype == DOMNode::ELEMENT_NODE) {
        DOMElement* chielem = static_cast<DOMElement *>(child);
        if ( pool::_toString(chielem->getTagName()) == ChildTagName )
          if ( AttName.empty() || this->getAttr(child,AttName) == AttValue ) 
            return child;
      }
      child = child->getNextSibling();        
    }
  }
  return 0;
}

  


void 
pool::PoolXMLFileCatalog::setAttr(DOMNode* Node, 
                                  const std::string& attrib , 
                                  const std::string& attvalue)
{
  DOMElement* nodelem = static_cast<DOMElement *> (Node);
  XMLCh* att = pool::_toDOMS(attrib);
  XMLCh* val = pool::_toDOMS(attvalue);
  nodelem->setAttribute(att,val);
  XMLString::release(&att);
  XMLString::release(&val);
}




std::string 
pool::PoolXMLFileCatalog::getAttr(DOMNode* Node, const std::string & attrib )
{
  DOMElement* nodelem = static_cast<DOMElement *>(Node);
  XMLCh* att = pool::_toDOMS(attrib);
  std::string attr(pool::_toString(nodelem->getAttribute(att)));
  XMLString::release(&att);
  return attr;
}




void  
pool::PoolXMLFileCatalog::selectType(pool::PoolXMLFileCatalog::
                                     FileNameType fntype, 
                                     std::string& daughter, 
                                     std::string& grandaug)
{
  switch (fntype)
    {
    case pool::PoolXMLFileCatalog::LFNAME: 
      {
        daughter = DaughtLNodeName;
        grandaug = GranDaLNodeName;
        break;
      }
    case pool::PoolXMLFileCatalog::PFNAME: 
      {
        daughter = DaughtPNodeName;
        grandaug = GranDaPNodeName;
        break;
      }
    }
}




std::string 
pool::PoolXMLFileCatalog::getfile(bool backup)
{  
  // To be done:
  // Improve the parsing of POOL_CATALOG.
  // Be sure that the local file is not read-only because of problems 
  // (missing privileges, disk quota exceeded, etc...)
  // by the hack : cp xmlfile xmlfile.BAK 
  // (the ofstream will tell me if I can write on path)

  std::string xmlfile = m_file;    
  std::string bakfile;
  std::string pathxml,namexml;
  std::string myurl=xmlfile;
  coral::MessageStream xmllog( "PoolXMLFileCatalog" );

  XMLURL xerurl;
  try{
    XMLCh* burl = pool::_toDOMS(myurl);
    xerurl = burl;
    XMLString::release(&burl);
  }
  catch (const XMLException& e ) {
    xmllog << coral::Error << pool::_toString(e.getMessage()) <<coral::MessageStream::endmsg;;
    throw( std::runtime_error( "Standard pool exception : Fatal Error on PoolXMLFileCatalog" ) );
  }  
  
  std::string prol("");
  try{
    prol = pool::_toString(xerurl.getProtocolName());
  }
  catch (const XMLException& e) {
    xmllog<<coral::Fatal << pool::_toString(e.getMessage()) << coral::MessageStream::endmsg;
    throw( std::runtime_error( "Standard pool exception : Fatal Error on PoolXMLFileCatalog" ) );
  }
  
  if      ( prol == "http") {
    xmllog<<coral::Verbose << "http protocol in use"<<coral::MessageStream::endmsg;
    xmllog << coral::Verbose << "URL in use "<< xmlfile <<coral::MessageStream::endmsg;
    read_only= true;
  }
  else if ( prol == "ftp")  {
    xmllog<<coral::Verbose << "ftp protocol in use"<<coral::MessageStream::endmsg;
    xmllog<<coral::Warning << "ftp protocol not supported yet"
          <<coral::MessageStream::endmsg;
    read_only= true;
  }
  else if ( prol == "file") {
    std::string mypath=pool::_toString(xerurl.getPath());
    xmlfile = mypath;
    xmllog << coral::Verbose << "Local file in use "
           << xmlfile <<coral::MessageStream::endmsg;
    read_only = false;
    std::ifstream TheXML(xmlfile.c_str());
    if ( ! TheXML ) {
      xmllog<<coral::Info<< "File "<<xmlfile
            <<" does not exist, a new one is created"<<coral::MessageStream::endmsg;
      TheXML.close();
      std::ofstream NewXML(xmlfile.c_str());
      read_only  = (!NewXML);
      if ( ! NewXML )
      {
        xmllog << coral::Fatal << "Problem creating file " << xmlfile <<coral::MessageStream::endmsg;
        throw( std::runtime_error( "Standard pool exception : Fatal Error on PoolXMLFileCatalog" ) );
      }
      else
      {
        NewXML << "<!-- Edited By POOL -->"<<std::endl;
        NewXML << "<!DOCTYPE POOLFILECATALOG SYSTEM \"InMemory\">"<<std::endl;
        NewXML <<"<POOLFILECATALOG>"<<std::endl;
        NewXML <<"</POOLFILECATALOG>"<<std::endl;
      }    
      NewXML.close();        
    }
    else if (backup)
    {        
      bakfile = xmlfile + std::string(".BAK");
      std::ofstream TheBAK(bakfile.c_str());
      read_only = (!TheBAK);
      if (!TheBAK) {
        xmllog << coral::Info <<"Read-only filesystem"<<coral::MessageStream::endmsg;
      }
      else
      {
        char ch;
        while(TheXML.get(ch)) TheBAK.put(ch);
      }
      TheBAK.close();
      TheXML.close();
    }
    else
    {
      TheXML.close();
    }
  }
  else if ( prol.empty() )
  {
    throw( std::runtime_error( "Standard pool exception : Missing protocol  (file or http)" ) );
  } else
  {
    std::string message = prol + std::string(" protocol not supported. Use file or http protocol");
    throw( std::runtime_error( "Standard pool exception " + message ) );
  }
  return xmlfile;
}




bool 
pool::PoolXMLFileCatalog::getUpdate()
{
  return update;
}




void 
pool::PoolXMLFileCatalog::setUpdate(bool isup)
{
  update=isup;
}




void 
pool::PoolXMLFileCatalog::getback()
{
  //  doc->release();
  //  if ( ! this->isitReadOnly()){
  //  std::string xmlfile = m_file;    
  //  std::ofstream TheXML(xmlfile.c_str());
  //  std::string bakfile = xmlfile + std::string(".BAK");
  //  std::ifstream TheBAK(bakfile.c_str());
    
  //   char ch;
  //  while(TheBAK.get(ch)) TheXML.put(ch);
    
  //   TheBAK.close();
  //  TheXML.close();
    
  //}
  //    doc->release();
}



  
std::string 
pool::PoolXMLFileCatalog::getGuidMeta(const std::string& fid, 
                                      const std::string& attr_name) 
{  
  DOMNode* node = this->getNode(ParentNodeName,FileAtt_ID,fid);
  if (!node){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    // xmllog<<coral::Warning << "FileID "<<fid
    //          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return std::string();
  }
  
  DOMNode* mnod = this->getChildwithAttr(node,MetaNode,
                                         FileAtt_attname,attr_name);
  if (!mnod){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    //xmllog<<coral::Warning << "Metadata  "<<attr_name
    //          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return std::string();
  } 
  return this->getAttr(mnod,FileAtt_attvalu);      
}




void 
pool::PoolXMLFileCatalog::setGuidMeta(const std::string& fid, 
                                      const std::string& attr_name, 
                                      const std::string& attr_value)
{   
  if (metaGuid.find(attr_name) != metaGuid.end()){    
    DOMNode* node = this->getNode(ParentNodeName,FileAtt_ID,fid);
    if (!node) {      
      coral::MessageStream xmllog( "PoolXMLFileCatalog" );
      xmllog<<coral::Warning << "FileID "<<fid
            <<" is not in the xml file"<<coral::MessageStream::endmsg;
      return;
    }
    DOMNode* mnod = 
      this->getChildwithAttr(node,MetaNode,FileAtt_attname,attr_name);
    if (!mnod){
      XMLCh* name = pool::_toDOMS(MetaNode);
      mnod = doc->createElement(name);
      XMLString::release(&name);
      node->appendChild(mnod);
      this->setAttr(mnod,FileAtt_attname,attr_name);    
    }
    this->setAttr(mnod,FileAtt_attvalu,attr_value);        
  }else{
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog << coral::Error<<"Attribute "<<attr_name <<" not found"<<coral::MessageStream::endmsg;
    throw( std::runtime_error( "Not such attribute : Fatal Error on PoolXMLFileCatalog" ) );
  }
}




void 
pool::PoolXMLFileCatalog::setGuidMeta(const std::string& attr_name, 
                                      const std::string& attr_value)
{  
  //Look for the next FID with no metadata 
  // go to the last metadata tag, take the fid, go to next fid   
  
  XMLCh* name = pool::_toDOMS(ParentNodeName);
  if (fmeta) {
    fmeta = false;
    nmeta = doc->getElementsByTagName(name)->getLength();  
    imeta = 0;    
  }
  if (imeta < nmeta ){
    DOMNode* finode = doc->getElementsByTagName(name)->item(imeta);    
    imeta++;
    std::string fid = getAttr(finode,FileAtt_ID);    
    this->setGuidMeta(fid,attr_name,attr_value);
  }else{
    fmeta = true;
  }
  XMLString::release(&name);

}



void 
pool::PoolXMLFileCatalog::dropGuidMetas()
{
  XMLCh* name = pool::_toDOMS(ParentNodeName);
  unsigned int Num_of_Metas  = 
    doc->getElementsByTagName(name)->getLength();
  for ( unsigned int index=0; index < Num_of_Metas; index++){
    DOMNode* FNode = doc->getElementsByTagName(name)->item(index);
    
    DOMNode* child = FNode->getFirstChild();
    std::vector<DOMNode *> garbNode;
    while( child != 0){
      int nodetype = child->getNodeType();
      if ( nodetype == DOMNode::ELEMENT_NODE) {
        DOMElement* chielem = static_cast<DOMElement *>(child);
        if ( pool::_toString(chielem->getTagName()) == MetaNode )
          garbNode.push_back(child);
      }
      child = child->getNextSibling();        
    }
    for (std::vector<DOMNode *>::iterator im = garbNode.begin();
         im != garbNode.end(); im++){
      FNode->removeChild(*im);
    }
  }
  XMLString::release(&name);

  name = pool::_toDOMS(MainNode);
  DOMNode* MNode = doc->getElementsByTagName(name)->item(0);
  XMLString::release(&name);
  name=pool::_toDOMS(METANode);
  unsigned int NumMetas = doc->getElementsByTagName(name)->getLength();
  
  std::vector<DOMNode *> garbNode;
  for (unsigned int index = 0; index < NumMetas; index++)
    garbNode.push_back(doc->getElementsByTagName(name)
                       ->item(index));

  XMLString::release(&name);
  for (std::vector<DOMNode *>::iterator im=garbNode.begin();
       im != garbNode.end(); im++)
    MNode->removeChild(*im); 
  
  metaGuid.clear();
}
  



void 
pool::PoolXMLFileCatalog::dropGuidMetas(const std::string& attr_name)
{
  XMLCh* name = pool::_toDOMS(ParentNodeName);
  unsigned int Num_of_Metas  = 
    doc->getElementsByTagName(name)->getLength();
  
  for ( unsigned int index=0; index < Num_of_Metas; index++){
    DOMNode* FNode = doc->getElementsByTagName(name)->item(index);
    DOMNode* MNode = this->getChildwithAttr(FNode, MetaNode, FileAtt_attname, 
                                            attr_name);
    if (MNode) 
      FNode->removeChild(MNode);    
  }
  XMLString::release(&name);

  name = pool::_toDOMS(MainNode);
  DOMNode* MNode = doc->getElementsByTagName(name)->item(0);
  XMLString::release(&name);
  DOMNode* MetaNode = this->getChildwithAttr(MNode,METANode,FileAtt_name,
                                             attr_name);
  if(MetaNode){
    MNode->removeChild(MetaNode);
    metaGuid.erase(attr_name);
  }
}




void 
pool::PoolXMLFileCatalog::dropMetas(const std::string& fid)
{
  
  DOMNode* FNode = this->getNode(ParentNodeName,FileAtt_ID,fid);
  if (!FNode)
    throw( std::runtime_error( "Can't find node: " + ParentNodeName + " " + fid));
  DOMNode* MNode = this->getChild(FNode,MetaNode);
  std::vector<DOMNode*> garbNode;
  while (MNode){
    garbNode.push_back(MNode);
    MNode=MNode->getNextSibling();
  }
  std::vector<DOMNode*>::iterator ig;
  for (ig = garbNode.begin(); ig != garbNode.end(); ig++){
    FNode->removeChild(*ig);    
  }
}




pool::PoolXMLFileCatalog::poolAttr& 
pool::PoolXMLFileCatalog::getMeta()
{
  metaGuid.clear();
  XMLCh* name = pool::_toDOMS(METANode);
  unsigned int nmeta = 
    doc->getElementsByTagName(name)->getLength();
  for ( unsigned int index=0; index < nmeta ; index++){
    DOMNode* MyNode = 
      doc->getElementsByTagName(name)->item(index);    
    std::string name = this->getAttr(MyNode,FileAtt_name);
    std::string type = this->getAttr(MyNode,FileAtt_type);
    metaGuid[name]=type;
  }
  XMLString::release(&name);
  return metaGuid;
}




void 
pool::PoolXMLFileCatalog::setMeta(const std::vector<std::string>& name, 
                                  const std::vector<std::string>& type)
{

  XMLCh* mnod = pool::_toDOMS(MainNode);
  DOMNode* MNode = doc->getElementsByTagName(mnod)->item(0);
  XMLString::release(&mnod);
  mnod = pool::_toDOMS(ParentNodeName);
  DOMNode* PNode = doc->getElementsByTagName(mnod)->item(0);
  XMLString::release(&mnod);
 
  if (!MNode) {
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog << coral::Error <<"I Cannot find the main node"<<coral::MessageStream::endmsg;
    throw( std::runtime_error( "POOLCATALOG node missing : Fatal Error on PoolXMLFileCatalog" ) );
  }
  std::vector<std::string>::const_iterator imeta;
  std::vector<std::string>::const_iterator itype = type.begin();
  
  mnod = pool::_toDOMS(METANode);
  for (imeta=name.begin(); imeta!=name.end();imeta++){
    DOMElement* fnelem = doc->createElement(mnod);
    MNode->insertBefore(fnelem,PNode);
    this->setAttr(fnelem,FileAtt_name,(*imeta));    
    this->setAttr(fnelem,FileAtt_type,(*itype));    
    metaGuid[*imeta]=*itype;
    metadata[*imeta]=*imeta;
    itype++;
  }
  XMLString::release(&mnod);

}




std::string 
pool::PoolXMLFileCatalog::getfiletype(const std::string& pfn)
{
  DOMNode* node = this->getNode(GranDaPNodeName,FileAtt_name,pfn);
  if (!node){
    coral::MessageStream xmllog( "PoolXMLFileCatalog" );
    xmllog<<coral::Debug << "PFname "<<pfn
          <<" is not in the xml file"<<coral::MessageStream::endmsg;
    return std::string();
  }
  return this->getAttr(node,FileAtt_fitype);      
}
