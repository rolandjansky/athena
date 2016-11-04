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

#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x 

/*****************************************************************************
 *
 * INNER CLASS (fatures handling)
 *
 *****************************************************************************/
IHolder::IHolder()
{
}

IHolder::IHolder(const std::string& prefix, const std::string& label, uint16_t idx )
  : m_prefix(prefix),
    m_label(label),
    m_subTypeIndex(idx)
{
}


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

  MLOG(DEBUG) << "Holder<T> serialize, " << *this  << endreq;
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
    MLOG(ERROR) << " header serialization failed " << endreq;
    return false;
  }
  std::vector<uint32_t> payload;
  bool payload_ser = serializePayload(payload);
  if(!payload_ser){
    MLOG(ERROR) << " payload serialization failed " << endreq;
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
  MLOG(DEBUG) << "IHolder::generateAliasKey generated key: " << st << endreq;
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

// only construct the regex once
namespace HLTNavDetails {  
  boost::regex rx1("_v[0-9]+$");
}

std::string HLTNavDetails::formatSGkey(const std::string& prefix, const std::string& containername, const std::string& label){
  // Memoize already used keys
  static std::map<std::string,std::string> memo;
  std::string key = prefix+containername+label;
  if (memo.count(key)) return memo[key];

  // Remove version
  std::string ret = boost::regex_replace(containername,rx1,std::string(""));
  
  // Replace :: with __
  size_t pos = ret.find("::");
  if (pos!=std::string::npos) ret.replace(pos,2,"__");

  // Add prefix and label
  ret = prefix + "_" + ret;
  if (label == "Aux.")
    ret += label;
  else if (!label.empty())
    ret += ("_" + label);

  memo[key] = ret;
  return ret;
}

bool HLTNavDetails::IHolder::deserializePayload(const std::vector<uint32_t>& dataBlob, int version){
  const std::string sgkey = key();
  ::HLT::class_id_type feature_clid = typeClid();
  ::HLT::class_id_type container_clid = containerClid();
  const std::string container_typename = collectionName();

  if ( m_storeGate->transientContains(container_clid, sgkey) ) {
    MLOG(VERBOSE) << "deserialize: while working on: " << container_typename << " and key: " << sgkey
                  << " from serialized form found it in SG already, sync with it" << endreq;
    syncWithSG();
  }

  MLOG(VERBOSE) << "deserializing a data blob of size " << dataBlob.size() << " navi version is " << version;
  MLOG(DEBUG) << "holder type clid " << feature_clid<< " collection clid: " << container_clid << endreq;

  if (container_clid<1){
    MLOG(WARNING) << "holder type clid " << feature_clid << " collection clid: " << container_clid << endreq;
  }  
  
  typedef std::vector<uint32_t>::const_iterator it_type;
  it_type it = std::begin(dataBlob);
  
  if(!dataBlob.size()) return false;

  std::vector<uint32_t> first, second;

  if(version == 4){
    auto firstsize = *(it++);
    MLOG(VERBOSE) << "first part has size: " << firstsize;
  
    first = std::vector<uint32_t>(it,it+firstsize);
  
    std::advance(it,firstsize);  
  
    if(!(it!=dataBlob.end())){
      MLOG(VERBOSE) << "this datablob only has a first part (non xAOD case)";
    }
    else{
      auto secondsize = *(it++);
      MLOG(VERBOSE) << "second part has size: " << secondsize;
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
    MLOG(WARNING) << "deserialize main: failed" << std::endl;
    return false;
  }

  bool setcontainer = setDataObject(dobj);
  if (DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (dobj)) {
    dbb->relinquish();
    delete dbb;
  }
  if(!setcontainer) return false;

  DataObject* dobjaux(0);
  if(!second.empty()){
    std::string sgkeyaux = sgkey+"Aux.";
    MLOG(VERBOSE) << "aux clid was deduced to be: " << auxClidOrZero();
    TrigStreamAddress auxaddr(auxClidOrZero(), sgkeyaux, "", 0, 0);
    auxaddr.add(second);

    if (m_objectserializerSvc->createObj((IOpaqueAddress*)&auxaddr, dobjaux).isFailure() ){
      MLOG(WARNING) << "Aux Store deserialization failed";
      return false;
    }
    bool setaux = setAuxDataObject(dobjaux);
    if (DataBucketBase* dbb = dynamic_cast<DataBucketBase*> (dobjaux)) {
      dbb->relinquish();
      delete dbb;
    }
    if(!setaux) return false;
  }
  return true;  
}

bool HLTNavDetails::IHolder::serializePayload(std::vector<uint32_t>& dataBlob) {
  DataObject* dobj = getDataObject();
  
  if(!dobj){
    MLOG(WARNING) << "data object points to null. can't serialize" << endreq;
  }

  MLOG(VERBOSE) << "creating serialized representation for " << dobj << endreq;


  TrigStreamAddress* addr(0);
  if ( m_objectserializerSvc->createRep(dobj, *pp_cast<IOpaqueAddress>(&addr) ).isSuccess() ) {
    MLOG(VERBOSE) << "serialization of feature (object) successful, blob size: "  << addr->get().size() << endreq;
    dataBlob.push_back(addr->get().size());
    dataBlob.insert(dataBlob.end(), addr->get().begin(), addr->get().end());
    // above should be optimized (need to know if in case of failed serialization addr->get() contains empty data vector)
  }
  else{
    MLOG(ERROR) << "Container serialization Failure" << endreq;
    return false;
  }
  DataBucketBase* dobjBase = static_cast<DataBucketBase*>(dobj);
  if ( dobjBase ) {
    dobjBase->relinquish();
    MLOG(VERBOSE) << "serialization giving up the ownership" << endreq;;
  }
  delete dobj;
  delete addr;
  
  DataObject* dobjaux = getAuxDataObject();

  MLOG(VERBOSE) << "aux object: " << dobjaux << endreq;;

  TrigStreamAddress* auxaddr(0);
  if(dobjaux){
    MLOG(VERBOSE) << "AuxStore serialization" << endreq;
    if(m_objectserializerSvc->createRep(dobjaux,*pp_cast<IOpaqueAddress>(&auxaddr) ).isSuccess() ){
      MLOG(VERBOSE) << "aux conversion success! aux blob has size: " << auxaddr->get().size() << endreq;
      dataBlob.push_back(auxaddr->get().size());
      dataBlob.insert(dataBlob.end(), auxaddr->get().begin(), auxaddr->get().end());
    }
    else{
      MLOG(ERROR) << "AuxStore serialization Failure" << endreq;
      return false;
    }
    DataBucketBase* dobjBaseAux = static_cast<DataBucketBase*>(dobjaux);
    if ( dobjBaseAux ) {
      dobjBaseAux->relinquish();
      MLOG(VERBOSE) << "serialization giving up the ownership of Aux" << endreq;
    }
    delete dobjaux;
    delete auxaddr;
  }
  MLOG(VERBOSE) << "finished serializing payload. size: " << dataBlob.size() << endreq;

  return true;
}
