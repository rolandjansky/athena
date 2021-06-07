/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <boost/regex.hpp>

#include "TrigNavigation/TypeMaps.h"
#include "TrigNavigation/Holder.h"
#include "TrigAuxStore.h"
#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSerializeResult/ITrigSerializerToolBase.h"
#include "xAODCore/AuxContainerBase.h"
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthContainersRoot/getDynamicAuxID.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include "AthenaKernel/CLIDRegistry.h"
#include "SGTools/StlVectorClids.h"
#include "GaudiKernel/IConversionSvc.h"
#include "RootUtils/Type.h"
#include "TClass.h"
#include "TROOT.h"
#include "TVirtualCollectionProxy.h"


using namespace HLTNavDetails;


/*****************************************************************************
 *
 * INNER CLASS (fatures handling)
 *
 *****************************************************************************/
IHolder::IHolder()
  : m_serializer("TrigTSerializer/TrigTSerializer")
{}

IHolder::IHolder(const std::string& prefix, const std::string& label, uint16_t idx )
  : m_prefix(prefix),
    m_label(label),
    m_subTypeIndex(idx),
    m_serializer("TrigTSerializer/TrigTSerializer")
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

  ATH_MSG_DEBUG("Holder<T> serialize, " << *this );
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

bool IHolder::serializeWithPayload(const xAOD::AuxSelection& sel,
                                   std::vector<uint32_t>& output,
                                   size_t& payloadsize) {
  bool header_ser = serialize(output);
  if(!header_ser){
    ATH_MSG_ERROR(" header serialization failed ");
    return false;
  }
  std::vector<uint32_t> payload;
  bool payload_ser = serializePayload(payload, sel);
  if(!payload_ser){
    ATH_MSG_ERROR(" payload serialization failed ");
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
  ATH_MSG_DEBUG("IHolder::generateAliasKey generated key: " << st);
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
    ATH_MSG_VERBOSE("deserialize: while working on: " << container_typename << " and key: " << sgkey
                    << " from serialized form found it in SG already, sync with it");
    syncWithSG();
  }

  ATH_MSG_VERBOSE("deserializing a data blob of size " << dataBlob.size() << " navi version is " << version);
  ATH_MSG_DEBUG("holder type clid " << feature_clid<< " collection clid: " << container_clid);

  if (container_clid<1){
    ATH_MSG_WARNING("holder type clid " << feature_clid << " collection clid: " << container_clid);
  }  
  
  typedef std::vector<uint32_t>::const_iterator it_type;
  it_type it = std::begin(dataBlob);
  
  if(!dataBlob.size()) return false;

  std::vector<uint32_t> first, second;

  if(version == 4){
    auto firstsize = *(it++);
    ATH_MSG_VERBOSE("first part has size: " << firstsize);
  
    first = std::vector<uint32_t>(it,it+firstsize);
  
    std::advance(it,firstsize);  
  
    if(!(it!=dataBlob.end())){
      ATH_MSG_VERBOSE("this datablob only has a first part (non xAOD case)");
    }
    else{
      auto secondsize = *(it++);
      ATH_MSG_VERBOSE("second part has size: " << secondsize);
      second = std::vector<uint32_t>(it,it+secondsize);
      std::advance(it,secondsize);  
    }
  }
  else{
    first = std::vector<uint32_t>(dataBlob.begin(),dataBlob.end());
  }

  TrigStreamAddress addr(container_clid, sgkey, "", 0, 0);
  addr.add(first);
  DataObject* dobj(0);

  if (m_objectserializerSvc->createObj((IOpaqueAddress*)&addr, dobj).isFailure() ){
    ATH_MSG_WARNING("deserialize main: failed");
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
    ATH_MSG_VERBOSE("aux clid was deduced to be: " << auxClidOrZero());
    TrigStreamAddress auxaddr(auxClidOrZero(), sgkeyaux, "", 0, 0);
    auxaddr.add(second);

    if (m_objectserializerSvc->createObj((IOpaqueAddress*)&auxaddr, dobjaux).isFailure() ){
      ATH_MSG_WARNING("Aux Store deserialization failed");
      return false;
    }

    // Handle dynamic variables.
    if (it != dataBlob.end()) {
      DataBucketBase* dobjBaseAux = static_cast<DataBucketBase*>(dobjaux);

      SG::IAuxStore* istore = nullptr;
      if (xAOD::AuxContainerBase* auxstore =
          dobjBaseAux->template cast<xAOD::AuxContainerBase> (nullptr, false))
      {
        istore = auxstore;
      }

      if (!istore || !deserializeDynVars (dataBlob,
                                          it - dataBlob.begin(),
                                          *istore))
      {
        ATH_MSG_WARNING("Aux Store dynamic var deserialization failed");
        return false;
      }
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

bool HLTNavDetails::IHolder::serializePayload(std::vector<uint32_t>& dataBlob,
                                              const xAOD::AuxSelection& sel) {
  DataObject* dobj = getDataObject();
  
  if(!dobj){
    ATH_MSG_WARNING("data object points to null. can't serialize");
  }

  ATH_MSG_VERBOSE("creating serialized representation for " << dobj);


  TrigStreamAddress* addr(0);
  if ( m_objectserializerSvc->createRep(dobj, *pp_cast<IOpaqueAddress>(&addr) ).isSuccess() ) {
    ATH_MSG_VERBOSE("serialization of feature (object) successful, blob size: "  << addr->get().size());
    dataBlob.push_back(addr->get().size());
    dataBlob.insert(dataBlob.end(), addr->get().begin(), addr->get().end());
    // above should be optimized (need to know if in case of failed serialization addr->get() contains empty data vector)
  }
  else{
    ATH_MSG_ERROR("Container serialization Failure");
    return false;
  }
  DataBucketBase* dobjBase = static_cast<DataBucketBase*>(dobj);
  if ( dobjBase ) {
    dobjBase->relinquish();
    ATH_MSG_VERBOSE("serialization giving up the ownership");
  }
  delete dobj;
  delete addr;
  
  DataObject* dobjaux = getAuxDataObject();

  ATH_MSG_VERBOSE("aux object: " << dobjaux);

  TrigStreamAddress* auxaddr(0);
  if(dobjaux){
    ATH_MSG_VERBOSE("AuxStore serialization");
    if(m_objectserializerSvc->createRep(dobjaux,*pp_cast<IOpaqueAddress>(&auxaddr) ).isSuccess() ){
      ATH_MSG_VERBOSE("aux conversion success! aux blob has size: " << auxaddr->get().size());
      dataBlob.push_back(auxaddr->get().size());
      dataBlob.insert(dataBlob.end(), auxaddr->get().begin(), auxaddr->get().end());
    }
    else{
      ATH_MSG_ERROR("AuxStore serialization Failure");
      return false;
    }
    DataBucketBase* dobjBaseAux = static_cast<DataBucketBase*>(dobjaux);

    const SG::IAuxStoreIO* iio = nullptr;
    if (const xAOD::AuxContainerBase* auxstore =
        dobjBaseAux->template cast<xAOD::AuxContainerBase> (nullptr, true))
    {
      iio = auxstore;
    }

    dobjBaseAux->relinquish();
    ATH_MSG_VERBOSE("serialization giving up the ownership of Aux");
    delete dobjaux;
    delete auxaddr;

    // Handle dynamic variables.
    if (iio) {
      if (!serializeDynVars (*iio, sel, dataBlob)) {
        ATH_MSG_ERROR("Serialization of dynamic aux variables failed.");
        return false;
      }
    }
  }
  ATH_MSG_VERBOSE("finished serializing payload. size: " << dataBlob.size());

  return true;
}


/**
 * @brief Serialize all selected dynamic variables for an xAOD object.
 * @param iio The auxiliary store for the xAOD object.
 * @param sel Selection object for variables to write.
 * @param[out] dataBlob Serialized data.
 *
 * Returns true on success, false on failure.
 *
 * Each selected dynamic variable is concatenated to @c dataBlob as follows.
 *
 *   - Length serialized name data (1 word).
 *   - Serialized name data.
 *   - Length of serialized type name (1 word).
 *   - Serialized type name data.
 *   - Length of serialized variable data (1 word).
 *   - Serialized data for the variable, as produced by TrigTSerializer.
 */
bool IHolder::serializeDynVars (const SG::IAuxStoreIO& iio,
                                const xAOD::AuxSelection& sel,
                                std::vector<uint32_t>& dataBlob)
{
  // Loop over all selected variables.
  const SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();
  SG::auxid_set_t auxids = sel.getSelectedAuxIDs (iio.getDynamicAuxIDs());
  for (SG::auxid_t id : auxids) {
    // Get the TClass of the object being saved.
    const std::type_info* tinfo = iio.getIOType (id);
    if (!tinfo) {
      ATH_MSG_ERROR("serialize aux dyn vars failed: can't get type_info.");
      return false;
    }
    TClass* cls = TClass::GetClass (*tinfo);
    if (!cls) {
      ATH_MSG_ERROR("serialize aux dyn vars failed: can't get TClass.");
      return false;
    }

    // Serialize the object data to a temp buffer.
    std::vector<uint32_t> serialized =
      m_serializer->serialize (cls->GetName(),
                               const_cast<void*>(iio.getIOData (id)));

    // Concatenate header header information to the output buffer:
    // attribute name and data type.
    StringSerializer ss;
    std::vector<uint32_t> serializedName;
    ss.serialize (r.getName(id), serializedName); // FIXME: handle class part?
    dataBlob.push_back (serializedName.size());
    dataBlob.insert(dataBlob.end(), serializedName.begin(), serializedName.end());

    std::vector<uint32_t> serializedType;
    ss.serialize (cls->GetName(), serializedType);
    dataBlob.push_back (serializedType.size());
    dataBlob.insert(dataBlob.end(), serializedType.begin(), serializedType.end());

    // Add the object data itself to the output buffer.
    dataBlob.push_back (serialized.size());
    dataBlob.insert(dataBlob.end(), serialized.begin(), serialized.end());
  }

  return true;
}


/**
 * @brief Read dynamic auxiliary variables from a serialized buffer.
 * @param dataBlob Serialized data.
 * @param offs Offset in @c dataBlob where dynamic variables start.
 * @param[out] store Auxiliary store for the object being read.
 *
 * @c dataBlob should be formatted as described in the documentation
 * for @c serializeDynVars starting at @c offs.
 * @c store should be convertable to @c IAuxStoreHolder.
 * A new dynamic store will be created and registered with @c store
 * via @c setStore.
 *
 * Returns true on success, false on failure.
 */
bool IHolder::deserializeDynVars (const std::vector<uint32_t>& dataBlob,
                                  size_t offs,
                                  SG::IAuxStore& store)
{
  SG::IAuxStoreHolder* holder = dynamic_cast<SG::IAuxStoreHolder*> (&store);
  if (!holder) {
    ATH_MSG_ERROR("deserialize aux dyn vars failed: can't convert to IAuxStoreHolder.");
    return false;
  }
  
  std::unique_ptr<SG::IAuxStore> newstore = deserializeDynVars (dataBlob, offs);
  if (!newstore) return false;
  holder->setStore (newstore.release());
  return true;
}


namespace {


// Change instances of std::vector<...> to vector<...> in the input string,
// for comparing class names.
std::string stripStdVec (const std::string& s_in)
{
  std::string s = s_in;
  std::string::size_type pos;
  while ((pos = s.find ("std::vector<")) != std::string::npos) {
    s.erase (pos, 5);
  }
  return s;
}


} // anonymous namespace


/**
 * @brief Read dynamic auxiliary variables from a serialized buffer.
 * @param dataBlob Serialized data.
 * @param offs Offset in @c dataBlob where dynamic variables start.
 * @param sz Number of elements in the xAOD container.
 *
 * @c dataBlob should be formatted as described in the documentation
 * for @c serializeDynVars starting at @c offs.
 * Returns a new dynamic store.
 */
std::unique_ptr<SG::IAuxStore>
IHolder::deserializeDynVars (const std::vector<uint32_t>& dataBlob,
                             size_t offs)
{
  StringSerializer ss;
  SG::AuxTypeRegistry& r = SG::AuxTypeRegistry::instance();

  // New dynamic store.
  auto store = std::make_unique<HLTNavDetails::TrigAuxStore>();

  // Read through the buffer starting at @c offs.
  std::vector<uint32_t>::const_iterator it = dataBlob.begin() + offs;
  while (it < dataBlob.end()) {
    // Read the attribute and type names from the buffer.
    size_t name_size = *it++;
    std::string name;
    ss.deserialize (it, it+name_size, name);
    it += name_size;

    size_t tname_size = *it++;
    std::string tname;
    ss.deserialize (it, it+tname_size, tname);
    it += tname_size;

    // Object size.
    size_t var_size = *it++;

    const static char* packed_pref = "SG::PackedContainer<";
    const static unsigned int packed_preflen = strlen(packed_pref);
    std::string vecname = tname;
    if (strncmp (vecname.c_str(), packed_pref, packed_preflen) == 0)
      vecname.replace (0, packed_preflen, "vector<");

    // Handle schema evolution.  If we're expecting a different
    // type for the variable, then use that name instead.
    SG::auxid_t id = r.findAuxID (name);
    if (id != SG::null_auxid && stripStdVec(r.getVecTypeName(id)) != stripStdVec(vecname)) {
      vecname = r.getVecTypeName (id);
      tname = vecname;
    }

    // Handle the case where we don't have a registered type for this
    // auxiliary variable.
    if (id == SG::null_auxid)
    {
      std::string elementTypeName;
      const std::type_info* elt_tinfo = getElementType (vecname, elementTypeName);
      if (elt_tinfo)
        id = SG::getDynamicAuxID (*elt_tinfo, name, elementTypeName, tname, false);
      if (id == SG::null_auxid) {
        ATH_MSG_ERROR("deserialize aux dyn var failed; can't find auxid for:" << name);
        return std::unique_ptr<SG::IAuxStore> (std::move(store));
      }
    }

    // Read the object from the buffer.
    std::vector<uint32_t> buf (it, it+var_size);
    void* obj = m_serializer->deserialize (tname, buf);
    it += var_size;

    bool isPacked = (strncmp (tname.c_str(), packed_pref, packed_preflen) == 0);
    std::unique_ptr<SG::IAuxTypeVector> vec
      (r.makeVectorFromData (id, obj, isPacked, true));
    store->addVector (id, std::move(vec), false);
  }

  return std::unique_ptr<SG::IAuxStore> (std::move(store));
}


/**
 * @brief Find the type of an element of a vector.
 * @param tname_in The name of the type to analyze.
 * @param[out] elementTypeName The name of the type of an element of the vector.
 *
 * Returns the @c type_info for an element of the vector.
 */
const std::type_info*
IHolder::getElementType (const std::string& tname,
                         std::string& elementTypeName) const
{
  TClass* cls = TClass::GetClass (tname.c_str());
  if (!cls) return nullptr;
  TVirtualCollectionProxy* prox = cls->GetCollectionProxy();
  if (!prox) return nullptr;
  if (prox->GetValueClass()) {
     elementTypeName = prox->GetValueClass()->GetName();
     return prox->GetValueClass()->GetTypeInfo();
  }
  RootUtils::Type typ (prox->GetType());
  elementTypeName = typ.getTypeName();
  return typ.getTypeInfo();
}
