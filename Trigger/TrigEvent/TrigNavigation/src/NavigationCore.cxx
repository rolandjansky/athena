/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>

#include "GaudiKernel/IConversionSvc.h"
#include "AthenaKernel/DataBucketBase.h"
#include "AthenaKernel/SlotSpecificObj.h"
#include "CxxUtils/crc64.h"

#include "TrigConfHLTUtils/HLTUtils.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/TrigEDMSizes.h"
#include "TrigNavigation/RoICacheHistory.h"
#include "TrigNavigation/NavigationCore.h"

#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include <unordered_set>


using namespace HLT;
using namespace HLTNavDetails;

template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v)
{
  s<< "[";for(auto i:v){s << i << ", ";} s << "]";
  return s;
}

NavigationCore::NavigationCore(MsgStream& log)
  : TrigNavStructure(),
    m_serializerSvc(nullptr),
    m_storeGate(nullptr),
    m_objectsKeyPrefix("HLT"),
    m_objectsIndexOffset(0),
    m_holderfactory(nullptr),
    m_log(log)
{
}

NavigationCore::~NavigationCore() {
  ATH_MSG_VERBOSE("~NavigationCore: cleaning static type information");

  for ( auto [clid, proxy] : HLT::TypeMaps::proxies() ) delete proxy;
  HLT::TypeMaps::proxies().clear();

  for ( auto [clid, holder] : HLT::TypeMaps::holders() ) delete holder;
  HLT::TypeMaps::holders().clear();
}

/*****************************************************************************
 *
 * PRETTY PRINTING
 *
 *****************************************************************************/

MsgStream& HLT::operator<< ( MsgStream& m, const NavigationCore& nav ) {
  std::string str;
  nav.printASCIIArt(str);
  
  m << str;
  return m;
}

bool NavigationCore::extractBlob(const std::vector<uint32_t>& input, std::vector<uint32_t>::const_iterator& it, std::vector<uint32_t>& blob) const {
  blob.clear();
  if ( it == input.end() )
    return false;

  unsigned sizeOfBlob = *it;
  std::vector<uint32_t>::const_iterator begin = it;
  ++begin;
  std::vector<uint32_t>::const_iterator end   = it;
  ++end;
  advance(end, sizeOfBlob);
  if ( end <= input.end()) {
    blob.reserve(end-begin);
    blob.insert(blob.end(), begin, end);
    it = end; // update iterator
    return true;
  }
  return false; // failed to get blob
}


bool NavigationCore::serialize( std::vector<uint32_t>& output) const {
  std::vector<unsigned int> cuts;
  return serialize(output,cuts);
}

bool NavigationCore::serialize( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts ) const {
  std::vector<std::pair<CLID, std::string> > clid_name;
  clid_name.clear();
  return serialize(output, cuts, clid_name);
}

HLT::TrigEDMSizes* NavigationCore::retrieveOrCreateTrigEDMSizes(const std::string& name) const {
  // check if SG contains a previously registered TrigEDMSizes object
  // reuse it and update it
  // this is an issue when running L2 and EF together in athena
  TrigEDMSizes* sizes(0);
  if ( m_storeGate->transientContains(ClassID_traits<HLT::TrigEDMSizes>::ID(), name )) {
    ATH_MSG_DEBUG("A previously registered object of type = (HLT::TrigEDMSizes) and name = " << name << " was found in SG.");
    const TrigEDMSizes* sizesSG(0) ;
    if ( m_storeGate->retrieve(sizesSG, name).isFailure() ) {
      ATH_MSG_WARNING("There was an error when retrieving the old object of type = (HLT::TrigEDMSizes) and name = " << name << " from SG.");
    } else {
      sizes = const_cast<TrigEDMSizes*>(sizesSG);
    }
  } else {
    if ( m_storeGate->contains(ClassID_traits<HLT::TrigEDMSizes>::ID(), name) ) {
      ATH_MSG_DEBUG("An object of type = (HLT::TrigEDMSizes) and name = " << name << " accessible through SG was found.");
      const TrigEDMSizes* sizesSG(0) ;
      if ( m_storeGate->retrieve(sizesSG, name).isFailure() ) {
        ATH_MSG_WARNING("There was an error when retrieving the object contained in SG of type = (HLT::TrigEDMSizes) and name = " << name << ".");
      } else {
        sizes = const_cast<TrigEDMSizes*>(sizesSG);
      }
    } else {
      sizes = new TrigEDMSizes();
      m_storeGate->record(sizes, name).ignore();
      ATH_MSG_DEBUG("A new object of type = (HLT::TrigEDMSizes) and name = " << name << " was registered in SG.");
    }
  }
  return sizes;
}

bool NavigationCore::serialize( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name ) const {
  std::vector<uint32_t> holderdata;
  std::vector<unsigned int> holderblobsizes;

  std::lock_guard<std::recursive_mutex> lock(getMutex());
  const TrigHolderStructure& holderstorage = getHolderStorage();

  bool status = false;
  if ( m_classesToPayload.empty() ) { // this is offline case
    status = serializeHoldersWithoutPayload(holderstorage.getAllHolders<IHolder>(),holderdata, holderblobsizes, clid_name);
  } else { // this is online case when list of classes to payload is not empty
    status = serializeHoldersWithPayload(m_classesToPayload,holderdata,holderblobsizes,clid_name,true);
  }

  if(!status){
    ATH_MSG_WARNING("holder serialization failed");
    return false;
  }
  
  status = serializeWithHolderSection(holderdata,holderblobsizes,output,cuts,clid_name);

  if(!status){
    ATH_MSG_WARNING("full serialization failed");
    return false;
  }

  ATH_MSG_DEBUG("total size of serialized navigation: " << output.size());

  return true;
}

bool NavigationCore::serialize_DSonly( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name ) const {
  std::vector<uint32_t> holderdata;
  std::vector<unsigned int> holderblobsizes;

  bool status = serializeHoldersWithPayload(m_classesToPayload_DSonly,holderdata,holderblobsizes,clid_name,true);
  if(!status){
    ATH_MSG_WARNING("holder serialization failed");
    return false;
  }

  status = serializeWithHolderSection(holderdata,holderblobsizes,output,cuts,clid_name);
  if(!status){
    ATH_MSG_WARNING("full serialization failed");
    return false;
  }
  
  ATH_MSG_DEBUG("total size of serialized navigation (DS only): " << output.size());

  return true;
}


/*****************************************************************************
 *
 * DESERIALIZATION
 *
 *****************************************************************************/

bool NavigationCore::deserialize( const std::vector<uint32_t>& input ) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  TrigHolderStructure& holderstorage = getHolderStorage();

  std::vector<uint32_t>::const_iterator inputIt = input.begin();

  ATH_MSG_DEBUG("deserialize: deserializing input of size:  " << input.size());


  if (input.size()==0) {
    ATH_MSG_WARNING("Cannot work on empty payload");
    return false;
  }
  unsigned int  version = *inputIt++; // ignore version

  ATH_MSG_DEBUG("deserialize: the serialized input has versions " << version);

  if ( version != 3 and version !=4  ) {
    ATH_MSG_WARNING("No backward compatibility beyond version 3 possible; data was serialized with V: " << version
                << " while we are in version 4");
    return true;
  }
  ATH_MSG_DEBUG("deserialize: deserialization of Navigation version: " << version);

  unsigned int totalSize =  *inputIt++; // total size
  if ( totalSize != input.size() )
    ATH_MSG_WARNING("deserialize: the navigation is truncated: " << input.size()
                  << " while should be: " << totalSize );

  if ( input.size() == 2 ) { // there was only space for version and size
    ATH_MSG_WARNING("deserialize: the navigation is truncated badly, no recovery possible " );
    return false;
  }

  bool tesDeserializationStatus = deserializeTEs(inputIt,input.size());
  ATH_MSG_DEBUG("deserialize: TEs structure unpacked, status: " << tesDeserializationStatus );


  ATH_MSG_DEBUG("do we have holder payload? " << (inputIt != input.end()));

  // Keep track of blobs we've deserialized for each event.
  struct DeserializedMemo
  {
    std::mutex m_mutex;
    EventContext::ContextEvt_t m_evt;
    std::unordered_set<uint64_t> m_hashes;
  };
  static SG::SlotSpecificObj<DeserializedMemo> memos;

  const EventContext& ctx = Gaudi::Hive::currentContext();
  DeserializedMemo& memo = *memos.get (ctx);
  std::scoped_lock memolock (memo.m_mutex);
  if (memo.m_evt != ctx.evt()) {
    memo.m_hashes.clear();
    memo.m_evt = ctx.evt();
  }
  
  // EOF TEs deserialization
  // deserialize Features
  std::vector<uint32_t>::const_iterator it  = inputIt; 
  std::vector<uint32_t> blob;
  while ( extractBlob(input, it, blob) ) {
    // Skip this blob if we've already deserialized it.
    // See ATLASRECTS-6278 and ATEAM-734.
    uint64_t hash = CxxUtils::crc64 ((const char*)blob.data(),
                                     blob.size()*sizeof(*blob.data()));
    if (!memo.m_hashes.insert (hash).second) {
      ATH_MSG_DEBUG("blob with size/hash " << blob.size() << "/" << hash
                  << " already deserialized; skipping");
      continue;
    }
                                     
    ATH_MSG_DEBUG("deserializing holder blob of size: " << blob.size());
    auto holder = std::shared_ptr<HLT::BaseHolder>(m_holderfactory->fromSerialized(version,blob.begin(),blob.end()));
    if (! holder ) continue;   // either error or class was skipped during deserialization

    if(!holderstorage.registerHolder(holder)){
      ATH_MSG_WARNING("deserialize: holder registration for holder with clid: " << holder->typeClid() << " and label: " << holder->label() << " failed.");
    }
  }
  return true;
}

bool NavigationCore::merge(const NavigationCore& l2) {
  std::lock_guard<std::recursive_mutex> l2lock(l2.getMutex());
  const TrigHolderStructure& l2Holderstorage = l2.getHolderStorage();

  // we need to pick the holders which are at L2 and move them to EF
  for(auto l2holder : l2Holderstorage.getAllHolders<IHolder>()){
    ATH_MSG_DEBUG("will add holder " << *l2holder);
    std::string label(l2holder->label());
    IHolder* efholder = getHolder(l2holder->typeClid(), label);
    if ( efholder != 0 ) {
      if ( (efholder->label() != l2holder->label())
	   || (efholder->subTypeIndex() != l2holder->subTypeIndex()) ) {
	
        ATH_MSG_DEBUG("fixing bug for " << *efholder);
        // bug which we need to fix (some week of data in autumn 2008)
        // we have to delete current holder
        // and replace it by L2 one
        //
        // efholder->setSubTypeIndex(l2holder->subTypeIndex());
	
        ATH_MSG_DEBUG("after fixing   " << *efholder);
      }
    } else {
      ATH_MSG_DEBUG("cloning the holder from L2 " << *l2holder);

      bool status = createHolder(efholder, l2holder->typeClid(),l2holder->label(), l2holder->subTypeIndex());
      if(!status){
        ATH_MSG_WARNING("in merge could not create EF holder");
      }
      
      //not sure if there is any possibility for this case but check anyways -Lukas
      if(efholder->key()!=l2holder->key()){
        ATH_MSG_WARNING("in merge the created EF holder has different SG access key than L2 holder we tried to copy");
      }
      
      registerHolder(efholder);
    }
  }
  return true;
}

/*****************************************************************************
 *
 * very important RESET
 *
 *****************************************************************************/
void NavigationCore::reset(bool inFinalize) {
  TrigNavStructure::reset(inFinalize);

  ATH_MSG_DEBUG("Navigation reset done");
}

uint16_t NavigationCore::nextSubTypeIndex(CLID clid, const std::string& /*label*/) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  TrigHolderStructure& holderstorage = getHolderStorage();

  auto holders = holderstorage.getHoldersOfClid(clid);

  if ( holders.empty() )
    return m_objectsIndexOffset;

  std::vector<sub_index_type> sub_indices(holders.size());
  std::transform(holders.begin(), holders.end(), sub_indices.begin(),
		 [](BaseHolder* h) -> sub_index_type { return h->subTypeIndex(); });

  return (*std::max_element(sub_indices.begin(),sub_indices.end()))+1;
}

HLTNavDetails::IHolder* NavigationCore::prepareOneHolder(CLID clid, const std::string& label) {

  ATH_MSG_VERBOSE("NavigationCore::prepare preregistering objects of clid: " << clid << " label: " << label);
  IHolder *holder = getHolder(clid, label);
  if ( holder ) {    
    ATH_MSG_VERBOSE("NavigationCore::prepare preregistering objects  not executed as it already exists " << *holder);
    return holder;
  }
  
  uint16_t index =  nextSubTypeIndex(clid, label);
  ATH_MSG_VERBOSE("NavigationCore::prepare creating handler for type (CLID): " << clid
                  << " label: " << label << " index: " << index);
  
  if ( !createHolder(holder, clid, label, index) ) {
    ATH_MSG_INFO("NavigationCore::prepare Can't create storage for objects of CLID: " << clid << " as it is requested by configuration");
    return nullptr;
  }

  ATH_MSG_VERBOSE("Holder created, registering " << holder << " " << *holder);

  if ( !registerHolder(holder) ) {
    ATH_MSG_WARNING("Holder registration failed " << holder << " " << *holder);
    return nullptr;
  }
  if ( !holder->syncWithSG() ) {
    ATH_MSG_WARNING("Holder SG sync failed" << holder << " " << *holder);
    return nullptr;
  }
  
  return holder;
}

void NavigationCore::prepare() {
  if ( msgLvl(MSG::VERBOSE) ) {
    for ( const auto& [clid, holder] : HLT::TypeMaps::holders() ) {
      ATH_MSG_VERBOSE("NavigationCore::prepare Compile time known types : " << *holder);
    }
  }
  
  ATH_MSG_VERBOSE("NavigationCore::prepare Preregistering objects #:" <<  m_classesToPreregister.size());

  // populate structure with "must have" features
  for ( const CSPair& conf : m_classesToPreregister ) {
    CLID clid = conf.first;
    std::string label  = conf.second;
    if ( prepareOneHolder(clid, label) == 0 ) {
      ATH_MSG_WARNING("NavigationCore::prepare failed preparing the holder for CLID: " << clid << " and label " << label);
    }
  }
  ATH_MSG_DEBUG("NavigationCore::prepare Navigation structure prepared for next event");
}

bool NavigationCore::registerHolder(IHolder* holder) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  TrigHolderStructure& holderstorage = getHolderStorage();

  auto shared_holder = std::shared_ptr<HLT::BaseHolder>(holder);
  holderstorage.registerHolder(shared_holder);
  ATH_MSG_DEBUG("registerHolder for OK " << *holder);
  return true;
}

bool NavigationCore::createHolder( IHolder*& holder,  CLID clid, const std::string& label, uint16_t index) {
  ATH_MSG_DEBUG("createHolder: creating holder for CLID: " << clid  << " label: " << label << " and index: " << index);
  //reset holder
  holder = 0;
  auto baseholder = m_holderfactory->createHolder(clid,label, index);
  if(!baseholder){
    ATH_MSG_ERROR("createHolder: creation of holder for CLID: " << clid  << " label: " << label << " and index: " << index << " failed");
  }

  holder = dynamic_cast<IHolder*>(baseholder);
  
  if(!holder){
    ATH_MSG_ERROR("createHolder: cast to IHolder* failed");
  }

  return true;
}

IHolder* NavigationCore::getHolder(CLID clid, uint16_t subTypeIndex) const {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  const TrigHolderStructure& holderstorage = getHolderStorage();

  return holderstorage.getHolder<IHolder>(clid, subTypeIndex);
}

IHolder* NavigationCore::getHolder(CLID clid, const std::string& label) const {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  const TrigHolderStructure& holderstorage = getHolderStorage();

  return holderstorage.getHolder<IHolder>(clid, label);
}

uint32_t  NavigationCore::string2hash( const std::string& s, const std::string& category) {
  return TrigConf::HLTUtils::string2hash(s, category);
}

void NavigationCore::getAllOfType ( const std::string& id, std::vector< HLT::TriggerElement* >& output,
                                    const bool activeOnly) const {
  if ( id == "" )
    return getAll(output, activeOnly);
  
  return TrigNavStructure::getAllOfType( string2hash(id, "TE"), output, activeOnly);
}


bool NavigationCore::getFeatureAccessors( const TriggerElement* te, class_id_type clid,
					  const index_or_label_type& index_or_label,
					  bool only_single_feature,
					  TriggerElement::FeatureVec& features,
					  bool with_cache_recording,
					  bool travel_backward_recursively,
					  const TriggerElement*& source,
					  std::string& sourcelabel) const {
  
  NavigationCore* non_const_this = const_cast<NavigationCore*>(this);
  

  //if query was via subindex we don't cache the query (no support yet)
  //note that the instantiation of this object has side effects (with calls to the caching singleton etc...)
  HLT::RoICacheHistory::QuestionScope qscope( with_cache_recording && (index_or_label.which() == 1)?
					      HLT::RoICacheHistory::QuestionScope(te, clid, boost::get<std::string>(index_or_label), non_const_this, only_single_feature) :
					      HLT::RoICacheHistory::QuestionScope() );
  
  return TrigNavStructure::getFeatureAccessors(te,clid,index_or_label,only_single_feature,features,with_cache_recording,travel_backward_recursively,source,sourcelabel);
} 

bool NavigationCore::getFeatureAccessorsSingleTE( const TriggerElement* te, CLID clid,
						  const index_or_label_type& index_or_label,
						  bool only_single_feature,
						  TriggerElement::FeatureVec& features,
						  bool with_cache_recording,
						  const TriggerElement*& source,
						  std::string& sourcelabel ) const {

  bool status = TrigNavStructure::getFeatureAccessorsSingleTE(te,clid,index_or_label,only_single_feature, features,with_cache_recording,source,sourcelabel);
  
  //if query was via subindex we don't cache the query (no support yet)
  if(index_or_label.which() == 0) return status;
  
  if ( with_cache_recording ) {
    for(auto& fea : features){
      HLT::RoICacheHistory::instance().addAnswer(te, fea);
    }
  }
  
  return status;
}

bool NavigationCore::serializeWithHolderSection(const std::vector<uint32_t>& holderdata, const std::vector<unsigned int>& holderblobsizes, std::vector<uint32_t>& output,
                                                std::vector<unsigned int>& cuts ,std::vector<std::pair<CLID, std::string> >& /*clid_name*/) const {
  cuts.clear();
  // clid_name.clear(); Don't reset this vector here since the vector is not remade. Otherwise datascouting stops working.

  unsigned int version=4;
  ATH_MSG_DEBUG("NavigationCore::serialize: serializing with version " << version);

  output.push_back(version);

  unsigned int totalSizeIndex = output.size();
  output.push_back(0); // reserve one word (accessible under the index totalSize), it is 0 if there was truncation here
                       // and != output.size() if truncation using cuts

  cuts.push_back(output.size()); // mark a cut place

  bool tesSerializationStatus = serializeTEs(output);

  ATH_MSG_DEBUG("serializes: TE serialization status: " << tesSerializationStatus << " size: " << output.size());

  cuts.push_back(output.size()); // mark a cut place

  output.insert(output.end(),holderdata.begin(),holderdata.end());

  for(auto hc : holderblobsizes){cuts.push_back(cuts.back()+hc);}

  output[totalSizeIndex] = output.size();

  ATH_MSG_DEBUG("serialization done");
  return true;
}

bool NavigationCore::serializeHoldersWithPayload(const std::vector<CSPair>& payload, std::vector<uint32_t>& output,
						 std::vector<uint32_t>& holderblobsizes,
						 std::vector<std::pair<CLID, std::string> >& clid_name, bool recordSizes) const {

  ATH_MSG_DEBUG("serialization: number of classes to payload: " << payload.size());
  for ( auto& cl :  payload) {
    ATH_MSG_DEBUG("serialization (ordered) of featue attempting : " <<  cl.first << " " <<  cl.second);
    
    IHolder *holder = getHolder(cl.first,cl.second);
    if ( ! holder ) {
      ATH_MSG_DEBUG("serialization (ordered) of feature skipped, nothing know on this objects" );
      continue;
    }
    ATH_MSG_DEBUG("serialization (ordered) of feature: " << holder->typeClid() << " label: " << holder->label()
		<< " size of payload up to now: " << output.size());
    
    std::vector<uint32_t> holderblob;
    size_t payloadsize = 0;
    bool status = holder->serializeWithPayload(cl.sel,holderblob,payloadsize);
    if(!status){
      ATH_MSG_WARNING("problem serializing holder: " << *holder);
      return false;
    }

    output.push_back(holderblob.size()); //leading bit indicates size
    output.insert(output.end(),holderblob.begin(),holderblob.end());
    
    holderblobsizes.push_back(1+holderblob.size()); //one for leading size bit 
    clid_name.push_back(std::pair < CLID, std::string> (holder->typeClid(), holder->label()));

    TrigEDMSizes* sizes = retrieveOrCreateTrigEDMSizes("TrigEDMSizes");
    if (sizes && recordSizes){
      sizes->addObject(holder->collectionName(), holder->label(), payloadsize, output.size() );
    }
  }

  return true;
}

bool NavigationCore::serializeHoldersWithoutPayload(const std::vector<IHolder*>& holders, std::vector<uint32_t>& output, std::vector<uint32_t>& holderblobsizes, std::vector<std::pair<CLID, std::string> >& clid_name) const {
  for(auto& holder : holders){
    ATH_MSG_DEBUG("serialization of feature: " << holder->typeClid() << " label: " << holder->label()
		<< " size of payload: " << output.size());
    
    std::vector<uint32_t> holderblob;
    bool status = holder->serialize(holderblob);
    if(!status){
      ATH_MSG_WARNING("problem serializing holder: " << *holder);
      return false;
    }
    output.push_back(holderblob.size()); //leading bit indicates size
    output.insert(output.end(),holderblob.begin(),holderblob.end());
    
    holderblobsizes.push_back(1+holderblob.size()); //one for leading size bit 
    clid_name.push_back(std::pair < CLID, std::string> (holder->typeClid(), holder->label()));
  }
  return true;
}
