/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "Cool2Json.h"
#include "CoolKernel/IFolderSpecification.h"

#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IRecord.h"
#include "CoolKernel/IRecordIterator.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/Blob.h"
#include "TStopwatch.h"

#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoraCool/CoraCoolObjectIter.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttrListAddress.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "AthenaPoolUtilities/CondAttrListVecAddress.h"
#include "FolderTypes.h"
#include "IOVDbStringFunctions.h"
#include "Base64Codec.h"

namespace {
  std::string     
  spec2String(const cool::IFolderPtr & pFolder){
    //open bracket, close bracket for json objects
    const std::string ob="";
    const std::string cb="";
    const std::string objName="\"folder_payloadspec\": \"";
    std::string result=ob+objName;
    const auto & rspec = pFolder->payloadSpecification();
    std::string sep{""};
    for (unsigned int i(0); i<rspec.size();++i){
      if (i==1) sep=", ";
      const auto & f = rspec[i];
      result+=sep;
      result+=f.name();
      result+=": ";
      result+=f.storageType().name();
    }
    result+='\"';
    result+=cb;
    return result;
  }
}

namespace IOVDbNamespace {
  Cool2Json::Cool2Json(const cool::IFolderPtr & pFolder, 
      const cool::ValidityKey & since, 
      const cool::ValidityKey & until, 
      const cool::ChannelSelection & chansel, 
      const std::string& folderTag): 
      m_pFolder(pFolder),
      m_start(since),
      m_stop(until),
      m_chansel(chansel),
      m_tag(folderTag), 
      m_desc(pFolder->description()),
      m_spec(spec2String(pFolder)), 
      m_nchans(0){
    const auto & channums=pFolder->listChannels();
    //find channels in the chansel which are chan nums
    //theres no 'size' method in the ChannelSelection class
    for (const auto &i:channums){
      m_nchans+=m_chansel.inSelection(i);//bool casts to 0 or 1
    }
  }
  
  std::string 
  Cool2Json::description() const{
    std::string saneXml=sanitiseXml(m_desc);
    std::string out = "\"node_description\" : \"";
    out += saneXml;
    out += '\"';
    return out;
  }

  std::string 
  Cool2Json::payloadSpec() const{
    return m_spec;
  }
  
  std::string 
  Cool2Json::open(){
    return "{";
  }
  
  std::string 
  Cool2Json::close(){
    return "}";
  }
  
  std::string 
  Cool2Json::delimiter(){
    return ", ";
  }
  
  std::string 
  Cool2Json::payload() {
    std::string result("\"data_array\" : [");
    cool::IObjectIteratorPtr itr=m_pFolder->browseObjects(m_start,m_stop,m_chansel,m_tag);
    IOVDbNamespace::FolderType ftype = determineFolderType(m_pFolder);
    std::string sep="";
    while (itr->goToNext()){
      const cool::IObject& ref=itr->currentRef();
      result+=sep;
      const long long cId=ref.channelId();
      result+="{ "+quote(std::to_string(cId))+" : ";
      switch  (ftype){
      case IOVDbNamespace::CoolVector:
        result+=formatCvp(itr);
        break;
      case IOVDbNamespace::AttrList:
        result +=formatAttrList(itr);
        break;
      case IOVDbNamespace::AttrListColl:
        result +=formatAttrList(itr);
        break;
      case IOVDbNamespace::PoolRef:
        result +=formatPoolRef(itr);
        break;
      case IOVDbNamespace::PoolRefColl:
        result += formatAttrList(itr);
        break;
      case IOVDbNamespace::CoraCool:
        result += " CoraCool";
        break;
      default:
        result+=" a_data_value";
      }
      if (sep.empty()) sep=",";
      result+='}';
    }
    result+=']';
    itr->close();
    return result;
  }
  
  unsigned int 
  Cool2Json::nchans() const{
    return m_nchans;
  }
  
  std::string 
  Cool2Json::iovBase() const{
    std::string result("");
    //run-lumi and run-event can both be found in our database, but the meaning is run-lumi
    if (m_desc.find("<timeStamp>run-lumi</timeStamp>") != std::string::npos) result = "run-lumi";
    if (m_desc.find("<timeStamp>run-event</timeStamp>") != std::string::npos) result = "run-lumi";
    if (m_desc.find("<timeStamp>time</timeStamp>") != std::string::npos) result = "time";
    return result;
  }
  
  std::string 
  Cool2Json::tag() const{
    return m_tag;
  }
  
  std::string 
  Cool2Json::formatCvp(const cool::IObjectIteratorPtr & itr){
    std::string os;
    const cool::IObject& ref=itr->currentRef();
    cool::IRecordIterator& pitr=ref.payloadIterator();
    auto pvec=pitr.fetchAllAsVector();
    std::string sep="";
    os+='[';//vector of vectors
    for (const auto & vitr:*pvec){
      os+=sep;
      const coral::AttributeList& atrlist=(vitr)->attributeList();
      os+=jsonAttributeList(atrlist);
      if (sep.empty()) sep =", ";
    }
    os+=']';
    return os;
  }
  
  std::string 
  Cool2Json::formatAttrList(const cool::IObjectIteratorPtr & itr){
    const cool::IObject& ref=itr->currentRef();
    const coral::AttributeList& atrlist=ref.payload().attributeList();
    std::string sep="";
    return jsonAttributeList(atrlist);
  }
  
  std::string 
  Cool2Json::formatPoolRef(const  cool::IObjectIteratorPtr & itr){
    const cool::IObject& ref=itr->currentRef();
    const coral::AttributeList& atrlist=ref.payload().attributeList();
    std::ostringstream os;
    atrlist[0].toOutputStream(os);
    auto res=os.str();
    const std::string sep(" : ");
    const auto separatorPosition = res.find(sep);
    const std::string payloadOnly=res.substr(separatorPosition+3);
    return quote(payloadOnly);
  }
  
  std::string
  Cool2Json::iov() const{
    std::string iovString("\"iov\" : ");
    return iovString+"["+std::to_string(m_start)+", "+std::to_string(m_stop)+"]";
  }
  
  std::ostream &operator<<(std::ostream &o, const Cool2Json &c){
    o << c.description() << std::endl;
    return o;
  }
  
  std::string
  Cool2Json::jsonAttribute(const coral::Attribute & attr){
    std::ostringstream os;
    attr.toOutputStream(os);
    const std::string native=os.str();
    const bool stringPayload=(native.find(" (string) ") != std::string::npos);
    const bool blobPayload=(native.find(" (blob) ") != std::string::npos);
    //take away anything between brackets in the original
    const std::string regex=R"delim( \(.*\))delim";
    const std::string deleted= deleteRegex(native,regex);
    const std::string sep(" : ");
    const auto separatorPosition = deleted.find(sep);
    const std::string payloadOnly=deleted.substr(separatorPosition+3);
    if (stringPayload) return quote(sanitiseJsonString(payloadOnly));
    if (blobPayload){
      return quote(IOVDbNamespace::base64Encode(attr.data<coral::Blob>()));
    }
    std::string result(payloadOnly);
    if (result=="NULL"){
      result="null";
    }
    
    return result;
  }
  
  std::string 
  Cool2Json::jsonAttributeList(const coral::AttributeList& atrlist){
    std::string os("[");
    const unsigned int nelement=atrlist.size();
    std::string delimiter(" ");
    for (unsigned int i(0);i!=nelement;++i){
      if (i==1) delimiter = ", ";
      os+=delimiter;
      os+=jsonAttribute(atrlist[i]);
    }
    os+="]";
    return os;
  }
}
