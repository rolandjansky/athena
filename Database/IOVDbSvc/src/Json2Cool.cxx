/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "Json2Cool.h"

#include "IOVDbStringFunctions.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/Record.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "boost/regex.hpp"
#include <stdexcept>
#include <iostream>

using json = nlohmann::json;
using namespace cool;
using namespace IOVDbNamespace;


namespace {


const std::map<std::string, cool::StorageType::TypeId> typeCorrespondance={ 
    //http://cool-doxygen.web.cern.ch/COOL-doxygen/classcool_1_1_storage_type.html
      {"Bool", StorageType::Bool},
      {"UChar",StorageType::UChar},
      {"Int16", StorageType::Int16},
      {"UInt16", StorageType::UInt16},
      {"Int32", StorageType::Int32},
      {"UInt32", StorageType::UInt32},
      {"UInt63",StorageType::UInt63},
      {"Int64", StorageType::Int64},
      {"Float", StorageType::Float},
      {"Double", StorageType::Double},
      {"String255", StorageType::String255},
      {"String4k", StorageType::String4k},
      {"String64k", StorageType::String64k},
      {"String16M", StorageType::String16M},
      {"Blob64k", StorageType::Blob64k},
      {"Blob16M", StorageType::Blob16M}
    };


} // anonymous namespace
  
  
namespace IOVDbNamespace{


Json2Cool::Json2Cool(std::istream & stream, BasicFolder & b):m_basicFolder(b){
   init(stream);
  }
  
  void
  Json2Cool::init(std::istream & s){
    if (not s.good()){
      const std::string msg("Json2Cool constructor; Input is invalid and could not be opened.");
      throw std::runtime_error(msg);
    } else {
      json j;
      try{
        s>>j; //read into json
      }catch (const std::exception& e) {
        std::cout<<"ERROR AT LINE "<<__LINE__<<" of "<<__FILE__<<std::endl;
        std::cout<<e.what()<<std::endl; //typically a parsing error
      }
      const std::string specString=j["folder_payloadspec"];
      const std::string description=j["node_description"];
      m_isVectorPayload =(description.find("CondAttrListVec") != std::string::npos);
      m_sharedSpec = parsePayloadSpec(specString);
      const auto & payload=j["data_array"];//payload is an array in any case
      m_basicFolder.setVectorPayloadFlag(m_isVectorPayload);
      const auto & iovFromFile=j["iov"];//iov is a two-element array
      const std::pair<cool::ValidityKey, cool::ValidityKey> iov(iovFromFile[0], iovFromFile[1]);
      m_basicFolder.setIov(iov);
      if (m_isVectorPayload){
        for (json::const_iterator k=payload.begin();k!=payload.end();++k){ //k are {"0":}
          const json f=k.value(); //channel id
          std::vector<coral::AttributeList> tempVector;//can optimise this by pre constructing it and using 'clear'
          for (json::const_iterator i=f.begin();i!=f.end();++i){
            const std::string keyString=i.key();
            const long long key=std::stoll(keyString);
            auto & val=i.value();
            for (const auto & aList:val){        
              auto r=createAttributeList(m_sharedSpec,aList);
              const auto & attList=r.attributeList();
              tempVector.push_back(attList);
            }
            m_basicFolder.addChannelPayload(key, tempVector);
          }
          //add payload with channelId here
        }
      } else {
        for (json::const_iterator i=payload.begin();i!=payload.end();++i){
          const json f=i.value();
          for (json::const_iterator k=f.begin();k!=f.end();++k){
            const std::string keyString=k.key();
            const long long key=std::stoll(keyString);
            auto & val=k.value();
            auto r=createAttributeList(m_sharedSpec,val);
            const auto & attList=r.attributeList();
            m_basicFolder.addChannelPayload(key, attList);
          }
        }
      }
    }
  }
  
  //parsing something like
  // "folder_payloadspec": "crate: UChar, slot: UChar, ROB: Int32, SRCid: Int32, BCIDOffset: Int16, slave0: Int32, slave1: Int32, slave2: Int32, slave3: Int32"
  cool::RecordSpecification *
  Json2Cool::parsePayloadSpec(const std::string & stringSpecification){
    if (stringSpecification.empty()) return nullptr;
    std::string input(stringSpecification);
    auto spec = new cool::RecordSpecification();
    
    std::string regex=R"delim(([^\s]*):\s?([^\s,]*),?)delim";
    boost::regex expression(regex);
    boost::smatch what;
    
    bool match=boost::regex_search(input, what, expression);
    while (match){
      std::string n(what[1]);
      std::string t(what[2]);
      //todo: need to catch error if type not found, also
      spec->extend(n, typeCorrespondance.find(t)->second);
      input = what.suffix();
      match=boost::regex_search(input, what, expression);
    }
    return spec;
  }
  
  cool::Record 
  Json2Cool::createAttributeList(cool::RecordSpecification * pSpec, const nlohmann::json & j){
    cool::Record a(*pSpec);
    unsigned int s=a.size();
    json::const_iterator it = j.begin();
    for (unsigned int i(0);i!=s;++i){
      auto & f=a[i];
      const auto & v = it.value();
      ++it;
      try{
        auto & att=const_cast<coral::Attribute&>(a.attributeList()[i]);
        if (v.is_null()){
          att.setNull();
          continue;
        }
        if (v.is_string()){
          const std::string & thisVal=v;
          att.setValue<std::string>(thisVal);
        } else {
          auto & thisVal=v;
          //nasty
        
          switch (f.storageType().id()){
            case (StorageType::Bool):
            {
              const bool newVal=thisVal;
              att.setValue<bool>(newVal);
            }
            break;
            case (StorageType::UChar):
            {
              const unsigned char newVal=thisVal;
              att.setValue<unsigned char>(newVal);
            }
            break;
            case (StorageType::Int16):
            {
              const short newVal=thisVal;
              att.setValue<short>(newVal);
            }
            break;
            case (StorageType::UInt16):
            {
              const unsigned short newVal=thisVal;
              att.setValue<unsigned short>(newVal);
            }
            break;
          
            case (StorageType::Int32):
            {
              const int newVal=thisVal;
              att.setValue<int>(newVal);
            }
            break;
            case (StorageType::UInt32):
            {
              const unsigned int newVal=thisVal;
              att.setValue<unsigned int>(newVal);
            }
            break;
            case (StorageType::UInt63):
            {
              const unsigned long long newVal=thisVal;
              att.setValue<unsigned long long>(newVal);
            }
            break;
            case (StorageType::Int64):
            {
              const  long long newVal=thisVal;
              att.setValue< long long>(newVal);
            }
            break;
            case (StorageType::Float):
            {
              const  float newVal=thisVal;
              att.setValue<float>(newVal);
            }
            break;
            case (StorageType::Double):
            {
              const  double newVal=thisVal;
              att.setValue<double>(newVal);
            }
            break;
            
            case (StorageType::String255):
            case (StorageType::String4k):
            case (StorageType::String64k):
            case (StorageType::String16M):
            {
              const  std::string newVal=thisVal;
              att.setValue<std::string>(newVal);
            }
            break;
            default:
            //nop
            break;
          }
        }
      } catch (json::exception& e){
        std::cout<<e.what()<<std::endl;
      }
    }
    return a;
  }

  
  Json2Cool::~Json2Cool(){
  }

}//end of namespace

