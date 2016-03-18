/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <boost/regex.hpp>

#include "TrigNavigation/TypeMaps.h"
#include "TrigNavigation/Holder.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSerializeResult/ITrigSerializerToolBase.h"

#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include "GaudiKernel/IConversionSvc.h"

using namespace HLTNavDetails;

/*****************************************************************************
 *
 * INNER CLASS (fatures handling)
 *
 *****************************************************************************/
IHolder::IHolder()
  : m_serialized(0),
    m_log(0),
    m_objectserializerSvc(0),
    m_storeGate(0),
    m_label(""),
    m_subTypeIndex(0),
    m_aux(0),
    m_uniqueCounter(0)
{}

IHolder::~IHolder() {
  if ( m_aux ) { delete m_aux; }
}

void IHolder::prepare(MsgStream* log, HLT::AccessProxy* sg, IConversionSvc* objSerializer, bool readonly) {
  m_storeGate = sg;
  m_log = log;
  m_objectserializerSvc = objSerializer;
  m_readonly = readonly;
}

bool IHolder::serialize(std::vector<uint32_t>& output)  const {
  if ( m_log->level()<=MSG::DEBUG )
    *m_log << MSG::DEBUG << "Holder<T> serialize, " << *this  << endreq;
  // if feature to be forgotten then indication of it is simply 0 size of serialized vector

  output.push_back( typeClid() );
  output.push_back( m_subTypeIndex );

  // label
  std::vector<uint32_t> serializedLabel;
  StringSerializer ss;
  ss.serialize( m_label, serializedLabel );
  output.push_back( serializedLabel.size() );
  output.insert(output.end(), serializedLabel.begin(), serializedLabel.end());
  return true;
}

bool IHolder::serializeWithPayload(std::vector<uint32_t>& output, size_t& payloadsize) {
  bool header_ser = serialize(output);
  if(!header_ser){
    *m_log << MSG::ERROR << " header serialization failed " << endreq;
    return false;
  }
  std::vector<uint32_t> payload;
  bool payload_ser = serializePayload(payload);
  if(!payload_ser){
    *m_log << MSG::ERROR << " payload serialization failed " << endreq;
    return false;
  }
  payloadsize = payload.size();
  output.insert(output.end(), payload.begin(), payload.end());
  return true;
}

std::string IHolder::generateAliasKey(CLID c, uint16_t sti, const std::string& label, unsigned size) {
  std::ostringstream ss;
  ss <<  "HLTAutoKey_" << label << "_" << c << "_" << sti << "_" << (size == 0xffffffff ? "back" : "to")  << "_" << m_uniqueCounter;
  std::string st = ss.str();
  if ( m_log->level()<=MSG::DEBUG )
    *m_log << MSG::DEBUG << "IHolder::generateAliasKey generated key: " << st << endreq;
  m_uniqueCounter++;
  return st;
}

void IHolder::print(MsgStream& m) const {
  m << "type: " << typeName() << "(" << typeClid() << ")" << " container: " << collectionName() << "(" << containerClid() << ")"
    << "label: \"" << label() << "\" subTypeIndex: " << subTypeIndex();
}

MsgStream& HLTNavDetails::operator<< ( MsgStream& m, const HLTNavDetails::IHolder& h ) {
  h.print (m);
  return m;
}

std::string HLTNavDetails::formatSGkey(const std::string& prefix, const std::string& containername, const std::string& label){
  std::string cleaned = containername;
  boost::regex rx1("_v[0-9]+$");
  cleaned = boost::regex_replace(cleaned,rx1,std::string(""));
  boost::regex rx2("::");
  cleaned = boost::regex_replace(cleaned,rx2,std::string("__"));
  std::string ret = prefix+"_"+cleaned;
  if (label == "Aux.")
    return ret + label;
  else if (!label.empty())
    return ret + "_" + label;
  return ret;
}


bool HLTNavDetails::IHolder::deserializePayload(const std::vector<uint32_t>& dataBlob, int version){
  const std::string sgkey = key();
  ::HLT::class_id_type feature_clid = typeClid();
  ::HLT::class_id_type container_clid = containerClid();
  const std::string container_typename = collectionName();

  if ( m_storeGate->transientContains(container_clid, sgkey) ) {
    *m_log << MSG::VERBOSE << "deserialize: while working on: " << container_typename << " and key: " << sgkey
	   << " from serialized form found it in SG already, sync with it" << endreq;
    syncWithSG();
  }

  *m_log << MSG::VERBOSE << "deserializing a data blob of size " << dataBlob.size() << " navi version is " << version;
  *m_log << MSG::DEBUG << "holder type clid " << feature_clid<< " collection clid: " << container_clid << endreq;

  if (container_clid<1){
    *m_log << MSG::WARNING << "holder type clid " << feature_clid << " collection clid: " << container_clid << endreq;
  }  
  
  typedef std::vector<uint32_t>::const_iterator it_type;
  it_type it = std::begin(dataBlob);
  
  if(!dataBlob.size()) return false;

  std::vector<uint32_t> first, second;

  if(version == 4){
    auto firstsize = *(it++);
    *m_log << MSG::VERBOSE << "first part has size: " << firstsize;
  
    first = std::vector<uint32_t>(it,it+firstsize);
  
    std::advance(it,firstsize);  
  
    if(!(it!=dataBlob.end())){
      *m_log << MSG::VERBOSE << "this datablob only has a first part (non xAOD case)";
    }
    else{
      auto secondsize = *(it++);
      *m_log << MSG::VERBOSE << "second part has size: " << secondsize;
      second = std::vector<uint32_t>(it,it+secondsize);
    }
  }
  else{
    first = std::vector<uint32_t>(dataBlob.begin(),dataBlob.end());
  }

  TrigStreamAddress addr(container_clid, sgkey, "", 0, 0);
  addr.add(first);
  DataObject* dobj(0);

  if (m_objectserializerSvc->createObj((IOpaqueAddress*)&addr, dobj).isFailure() ){
    *m_log << MSG::WARNING << "deserialize main: failed" << std::endl;
    return false;
  }

  bool setcontainer = setDataObject(dobj);
  if(!setcontainer) return false;

  DataObject* dobjaux(0);
  if(!second.empty()){
    std::string sgkeyaux = sgkey+"Aux.";
    *m_log << MSG::VERBOSE << "aux clid was deduced to be: " << auxClidOrZero();
    TrigStreamAddress auxaddr(auxClidOrZero(), sgkeyaux, "", 0, 0);
    auxaddr.add(second);

    if (m_objectserializerSvc->createObj((IOpaqueAddress*)&auxaddr, dobjaux).isFailure() ){
      *m_log << MSG::WARNING << "Aux Store deserialization failed";
      return false;
    }
    bool setaux = setAuxDataObject(dobjaux);
    if(!setaux) return false;
  }
  return true;  
}

bool HLTNavDetails::IHolder::serializePayload(std::vector<uint32_t>& dataBlob) {
  DataObject* dobj = getDataObject();
  
  if(!dobj){
    *m_log << MSG::WARNING << "data object points to null. can't serialize" << endreq;
  }

  *m_log << MSG::VERBOSE << "creating serialized representation for " << dobj << endreq;


  TrigStreamAddress* addr(0);
  if ( m_objectserializerSvc->createRep(dobj, *pp_cast<IOpaqueAddress>(&addr) ).isSuccess() ) {
    *m_log << MSG::VERBOSE << "serialization of feature (object) successful, blob size: "  << addr->get().size() << endreq;
    dataBlob.push_back(addr->get().size());
    dataBlob.insert(dataBlob.end(), addr->get().begin(), addr->get().end());
    // above should be optimized (need to know if in case of failed serialization addr->get() contains empty data vector)
  }
  else{
    *m_log << MSG::ERROR << "Container serialization Failure" << endreq;
    return false;
  }
  DataBucketBase* dobjBase = static_cast<DataBucketBase*>(dobj);
  if ( dobjBase ) {
    dobjBase->relinquish();
    *m_log << MSG::VERBOSE << "serialization giving up the ownership" << endreq;;
  }
  delete dobj;
  delete addr;
  
  DataObject* dobjaux = getAuxDataObject();

  *m_log << MSG::VERBOSE << "aux object: " << dobjaux << endreq;;

  TrigStreamAddress* auxaddr(0);
  if(dobjaux){
    *m_log << MSG::VERBOSE << "AuxStore serialization" << endreq;
    if(m_objectserializerSvc->createRep(dobjaux,*pp_cast<IOpaqueAddress>(&auxaddr) ).isSuccess() ){
      *m_log << MSG::VERBOSE << "aux conversion success! aux blob has size: " << auxaddr->get().size() << endreq;
      dataBlob.push_back(auxaddr->get().size());
      dataBlob.insert(dataBlob.end(), auxaddr->get().begin(), auxaddr->get().end());
    }
    else{
      *m_log << MSG::ERROR << "AuxStore serialization Failure" << endreq;
      return false;
    }
    DataBucketBase* dobjBaseAux = static_cast<DataBucketBase*>(dobjaux);
    if ( dobjBaseAux ) {
      dobjBaseAux->relinquish();
      *m_log << MSG::VERBOSE << "serialization giving up the ownership of Aux" << endreq;
    }
    delete dobjaux;
    delete auxaddr;
  }
  *m_log << MSG::VERBOSE << "finished serializing payload. size: " << dataBlob.size() << endreq;

  return true;
}
