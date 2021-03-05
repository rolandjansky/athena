/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator> // remove it (it is here to help with debugging)

#include "GaudiKernel/IConversionSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "AthenaKernel/DataBucketBase.h"

#include "TrigConfHLTData/HLTUtils.h"

#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/TrigEDMSizes.h"
#include "TrigNavigation/RoICacheHistory.h"
#include "TrigNavigation/NavigationCore.h"

#include "TrigSerializeResult/StringSerializer.h"
#include "TrigSerializeCnvSvc/TrigStreamAddress.h"

#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x

using namespace HLT;
using namespace HLTNavDetails;

template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v)
{
  s<< "[";for(auto i:v){s << i << ", ";} s << "]";
  return s;
}

NavigationCore::NavigationCore()
  : TrigNavStructure(), m_log(0),
    m_serializerSvc(0),
    m_storeGate(0),
    m_objectsKeyPrefix("HLT"),
    m_objectsIndexOffset(0),
    m_holderfactory(0),
    m_referenceAllClasses(true) {

  m_log = new MsgStream(Athena::getMessageSvc(), "NavigationCore");
}

NavigationCore::~NavigationCore() {
  MLOG(VERBOSE) << "~NavigationCore: cleaning static type information" << endmsg;
  {
    HLT::TypeMaps::CLIDtoTypeProxyMap::iterator it;
    for ( it = HLT::TypeMaps::proxies().begin(); it != HLT::TypeMaps::proxies().end(); ++it ) {
      delete it->second; it->second = 0;
    }
    HLT::TypeMaps::proxies().clear();
  }
  {
    HLT::TypeMaps::CLIDtoHolderMap::iterator it;
    for ( it = HLT::TypeMaps::holders().begin(); it != HLT::TypeMaps::holders().end(); ++it ) {
      delete it->second; it->second = 0;
    }
    HLT::TypeMaps::holders().clear();
  }

  delete m_log;
}

/*****************************************************************************
 *
 * PREATY PRINTING
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
    MLOG(DEBUG) << "A previously registered object of type = (HLT::TrigEDMSizes) and name = " << name << " was found in SG." << endmsg;
    const TrigEDMSizes* sizesSG(0) ;
    if ( m_storeGate->retrieve(sizesSG, name).isFailure() ) {
      MLOG(WARNING) << "There was an error when retrieving the old object of type = (HLT::TrigEDMSizes) and name = " << name << " from SG." << endmsg;
    } else {
      sizes = const_cast<TrigEDMSizes*>(sizesSG);
    }
  } else {
    if ( m_storeGate->contains(ClassID_traits<HLT::TrigEDMSizes>::ID(), name) ) {
      MLOG(DEBUG) << "An object of type = (HLT::TrigEDMSizes) and name = " << name << " accessible through SG was found." << endmsg;
      const TrigEDMSizes* sizesSG(0) ;
      if ( m_storeGate->retrieve(sizesSG, name).isFailure() ) {
	MLOG(WARNING) << "There was an error when retrieving the object contained in SG of type = (HLT::TrigEDMSizes) and name = " << name << "." << endmsg;
      } else {
	sizes = const_cast<TrigEDMSizes*>(sizesSG);
      }
    } else {
      sizes = new TrigEDMSizes();
      m_storeGate->record(sizes, name).setChecked(); //
      MLOG(DEBUG) << "A new object of type = (HLT::TrigEDMSizes) and name = " << name << " was registered in SG." << endmsg;
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
    MLOG(WARNING) << "holder serialization failed" << endmsg;
    return false;
  }
  
  status = serializeWithHolderSection(holderdata,holderblobsizes,output,cuts,clid_name);

  if(!status){
    MLOG(WARNING) << "full serialization failed" << endmsg;
    return false;
  }

  MLOG(DEBUG) << "total size of serialized navigation: " << output.size() << endmsg;

  return true;
}

bool NavigationCore::serialize_DSonly( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name ) const {
  std::vector<uint32_t> holderdata;
  std::vector<unsigned int> holderblobsizes;

  bool status = serializeHoldersWithPayload(m_classesToPayload_DSonly,holderdata,holderblobsizes,clid_name,true);
  if(!status){
    MLOG(WARNING) << "holder serialization failed" << endmsg;
    return false;
  }

  status = serializeWithHolderSection(holderdata,holderblobsizes,output,cuts,clid_name);
  if(!status){
    MLOG(WARNING) << "full serialization failed" << endmsg;
    return false;
  }
  
  MLOG(DEBUG) << "total size of serialized navigation (DS only): " << output.size() << endmsg;

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

  MLOG(DEBUG) << "deserialize: deserializing input of size:  " << input.size() << endmsg;


  if (input.size()==0) {
    MLOG(WARNING) << "Cannot work on empty payload" << endmsg;
    return false;
  }
  unsigned int  version = *inputIt++; // ignore version

  MLOG(DEBUG) << "deserialize: the serialized input has versions " << version << endmsg;

  if ( version != 3 and version !=4  ) {
    MLOG(WARNING) << "No backward compatibility beyond version 3 possible; data was serialized with V: " << version
                << " while we are in version 4" << endmsg;
    return true;
  }
  MLOG(DEBUG) << "deserialize: deserialization of Navigation version: " << version << endmsg;

  unsigned int totalSize =  *inputIt++; // total size
  if ( totalSize != input.size() )
    MLOG(WARNING) << "deserialize: the navigation is truncated: " << input.size()
                  << " while should be: " << totalSize  << endmsg;

  if ( input.size() == 2 ) { // there was only space for version and size
    MLOG(WARNING) << "deserialize: the navigation is truncated badly, no recovery possible "  << endmsg;
    return false;
  }

  bool tesDeserializationStatus = deserializeTEs(inputIt,input.size());
  MLOG(DEBUG) << "deserialize: TEs structure unpacked, status: " << tesDeserializationStatus  << endmsg;


  MLOG(DEBUG) << "do we have holder payload? " << (inputIt != input.end()) << endmsg;
  
  // EOF TEs deserialization
  // deserialize Features
  std::vector<uint32_t>::const_iterator it  = inputIt; 
  std::vector<uint32_t> blob;
  while ( extractBlob(input, it, blob) ) {
    MLOG(DEBUG) << "deserializing holder blob of size: " << blob.size() << endmsg;
    auto holder = std::shared_ptr<HLT::BaseHolder>(m_holderfactory->fromSerialized(version,blob.begin(),blob.end()));
    if (! holder ) {
      MLOG(ERROR) << "deserialize: deserialization of holder from serialized blob failed"<< endmsg;
      continue;
    }
 
    if(!holderstorage.registerHolder(holder)){
      MLOG(WARNING) << "deserialize: holder registration for holder with clid: " << holder->typeClid() << " and label: " << holder->label() << " failed." << endmsg;
    }
  }
  return true;
}

bool NavigationCore::merge(const NavigationCore& l2) {
  std::lock_guard<std::recursive_mutex> l2lock(l2.getMutex());
  const TrigHolderStructure& l2Holderstorage = l2.getHolderStorage();

  // we need to pick the holders which are at L2 and move them to EF
  for(auto l2holder : l2Holderstorage.getAllHolders<IHolder>()){
    if ( m_log->level()<=MSG::DEBUG )
        *m_log << MSG::DEBUG << "will add holder " << *l2holder << endmsg;
    std::string label(l2holder->label());
    IHolder* efholder = getHolder(l2holder->typeClid(), label);
    if ( efholder != 0 ) {
      if ( (efholder->label() != l2holder->label())
	   || (efholder->subTypeIndex() != l2holder->subTypeIndex()) ) {
	
	if ( m_log->level()<=MSG::DEBUG )
	  *m_log << MSG::DEBUG << "fixing bug for " << *efholder <<    endmsg;
	// bug which we need to fix (some week of data in autumn 2008)
	// we have to delete current holder
	// and replace it by L2 one
	//
	// efholder->setSubTypeIndex(l2holder->subTypeIndex());
	
	if ( m_log->level()<=MSG::DEBUG )
	  *m_log << MSG::DEBUG << "after fixing   " << *efholder <<    endmsg;
      }
    } else {
      if ( m_log->level()<=MSG::DEBUG )
	*m_log << MSG::DEBUG << "cloning the holder from L2 " << *l2holder << endmsg;

      bool status = createHolder(efholder, l2holder->typeClid(),l2holder->label(), l2holder->subTypeIndex());
      if(!status){
	*m_log << MSG::WARNING << "in merge could not create EF holder" << endmsg;
      }
      
      //not sure if there is any possibility for this case but check anyways -Lukas
      if(efholder->key()!=l2holder->key()){
	*m_log << MSG::WARNING << "in merge the created EF holder has different SG access key than L2 holder we tried to copy" << endmsg;
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

  if ( m_log->level()<=MSG::DEBUG )
    *m_log << MSG::DEBUG << "Navigation reset done" << endmsg;
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

  MLOG( VERBOSE ) << "NavigationCore::prepare preregistering objects of clid: " << clid << " label: " << label << endmsg;
  IHolder *holder = getHolder(clid, label);
  if ( holder ) {    
    MLOG( VERBOSE ) << "NavigationCore::prepare preregistering objects  not executed as it already exists " << *holder << endmsg;
    return holder;
  }
  
  uint16_t index =  nextSubTypeIndex(clid, label);
  if ( m_log->level() <= MSG::DEBUG )
    MLOG( VERBOSE ) << "NavigationCore::prepare creating handler for type (CLID): " << clid
			 << " label: " << label << " index: " << index << endmsg;
  
  if ( !createHolder(holder, clid, label, index) ) {
    MLOG( INFO ) << "NavigationCore::prepare Can't create storage for objects of CLID: " << clid << " as it is requested by configuration" << endmsg;
      return 0;
  }

  MLOG( VERBOSE ) << "Holder created, registering " << holder << " " << *holder << endmsg;

  if ( !registerHolder(holder) ) {
    MLOG( WARNING ) << "Holder registration failed " << holder << " " << *holder << endmsg;
    return 0;
  }
  if ( !holder->syncWithSG() ) {
    MLOG( WARNING ) << "Holder SG sync failed" << holder << " " << *holder << endmsg;
    return 0;
  }
  
  return holder;
}

void NavigationCore::prepare() {
  if ( m_log->level()<=MSG::DEBUG ) {
    HLT::TypeMaps::CLIDtoHolderMap::const_iterator hIt;
    for ( hIt = HLT::TypeMaps::holders().begin(); hIt != HLT::TypeMaps::holders().end(); ++hIt ) {
      MLOG( VERBOSE ) << "NavigationCore::prepare Compile time known types : " << *(hIt->second) << endmsg;
    }
  }
  
  MLOG( VERBOSE ) << "NavigationCore::prepare Preregistering objects #:" <<  m_classesToPreregister.size()<< endmsg;

  // populate structure with "must have" features
  for ( const CSPair& conf : m_classesToPreregister ) {
    CLID clid = conf.first;
    std::string label  = conf.second;
    if ( prepareOneHolder(clid, label) == 0 ) {
      MLOG( WARNING  )<< "NavigationCore::prepare failed preparing the holder for CLID: " << clid << " and label " << label << endmsg;
    }
  }
  MLOG( DEBUG  )<< "NavigationCore::prepare Navigation structure prepared for next event" << endmsg;
}

bool NavigationCore::registerHolder(IHolder* holder) {
  std::lock_guard<std::recursive_mutex> lock(getMutex());
  TrigHolderStructure& holderstorage = getHolderStorage();

  auto shared_holder = std::shared_ptr<HLT::BaseHolder>(holder);
  holderstorage.registerHolder(shared_holder);
  *m_log << MSG::DEBUG <<  "registerHolder for OK " << *holder << endmsg;
  return true;
}

bool NavigationCore::createHolder( IHolder*& holder,  CLID clid, const std::string& label, uint16_t index) {
  *m_log << MSG::DEBUG << "createHolder: creating holder for CLID: " << clid  << " label: " << label << " and index: " << index << endmsg;
  //reset holder
  holder = 0;
  auto baseholder = m_holderfactory->createHolder(clid,label, index);
  if(!baseholder){
    *m_log << MSG::ERROR << "createHolder: creation of holder for CLID: " << clid  << " label: " << label << " and index: " << index << " failed" << endmsg;
  }

  holder = dynamic_cast<IHolder*>(baseholder);
  
  if(!holder){
    *m_log << MSG::ERROR << "createHolder: cast to IHolder* failed" << endmsg;
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

bool NavigationCore::toBePutToPayload(const HLTNavDetails::IHolder* holder) const {
  std::string option = holder->typeName()+"#"+holder->key();
  std::vector<std::string>::const_iterator it;
  it = find(m_classesToPayloadProperty.begin(), m_classesToPayloadProperty.end(), option);
  if (it != m_classesToPayloadProperty.end() )
    return true;
  return false;
}

void NavigationCore::tweakMsgLvl(int newOffset) {
  m_log->setLevel(newOffset);
}

void NavigationCore::testMsgService() const {
  if (m_log) {
    MLOG(DEBUG) << "MLOG printout" << endmsg;
    *m_log << MSG::FATAL << "MSG::FATAL" << MSG::FATAL << endmsg;
    *m_log << MSG::ERROR << "MSG::ERROR " << MSG::ERROR << endmsg;
    *m_log << MSG::WARNING << "MSG::WARNING " << MSG::WARNING << endmsg;
    *m_log << MSG::INFO << "MSG::INFO " << MSG::INFO << endmsg;
    *m_log << MSG::DEBUG << "MSG::DEBUG " << MSG::DEBUG << endmsg;
    *m_log << MSG::VERBOSE << "MSG::VERBOSE " << MSG::VERBOSE <<endmsg;
    *m_log << MSG::INFO << "MSG level()=" << m_log->level() << endmsg;
  } else {
    std::cerr << "ERROR -> No MessageSvc available !" << std::endl;
  }
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

void NavigationCore::testStaticMaps()
{
  *m_log << MSG::INFO << "print statis map: CLID -> class name "  << endmsg;
  for ( static HLT::TypeMaps::NametoCLIDMap::const_iterator it = HLT::TypeMaps::type2clid().begin();
	it != HLT::TypeMaps::type2clid().end(); ++it) {
    *m_log << MSG::INFO << "entry: " << (*it).first << " -> " << (*it).second << endmsg;
  }

  *m_log << MSG::INFO << "print statis map: CLID -> holder "  << endmsg;
  for ( static HLT::TypeMaps::CLIDtoHolderMap::const_iterator it = HLT::TypeMaps::holders().begin();
	it != HLT::TypeMaps::holders().end(); ++it) {
    *m_log << MSG::INFO << "entry: " << (*it).first << " -> " << (* (*it).second) << endmsg;
  }

}

void NavigationCore::setObjKeyPrefix(const std::string& k) {
  m_objectsKeyPrefix = k;
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
  MLOG(DEBUG) << "NavigationCore::serialize: serializing with version " << version << endmsg;

  output.push_back(version);

  unsigned int totalSizeIndex = output.size();
  output.push_back(0); // reserve one word (accessible under the index totalSize), it is 0 if there was truncation here
                       // and != output.size() if truncation using cuts

  cuts.push_back(output.size()); // mark a cut place

  bool tesSerializationStatus = serializeTEs(output);

  MLOG(DEBUG) << "serializes: TE serialization status: " << tesSerializationStatus << " size: " << output.size() << endmsg;

  cuts.push_back(output.size()); // mark a cut place

  output.insert(output.end(),holderdata.begin(),holderdata.end());

  for(auto hc : holderblobsizes){cuts.push_back(cuts.back()+hc);}

  output[totalSizeIndex] = output.size();

  MLOG(DEBUG) << "serialization done" << endmsg;
  return true;
}

bool NavigationCore::serializeHoldersWithPayload(const std::vector<CSPair>& payload, std::vector<uint32_t>& output,
						 std::vector<uint32_t>& holderblobsizes,
						 std::vector<std::pair<CLID, std::string> >& clid_name, bool recordSizes) const {

  MLOG(DEBUG) << "serialization: number of classes to payload: " << payload.size() << endmsg;
  for ( auto& cl :  payload) {
    MLOG(DEBUG) << "serialization (ordered) of featue attempting : " <<  cl.first << " " <<  cl.second << endmsg;
    
    IHolder *holder = getHolder(cl.first,cl.second);
    if ( ! holder ) {
      MLOG(DEBUG) << "serialization (ordered) of feature skipped, nothing know on this objects"  << endmsg;
      continue;
    }
    MLOG(DEBUG) << "serialization (ordered) of feature: " << holder->typeClid() << " label: " << holder->label()
		<< " size of payload up to now: " << output.size() << endmsg;
    
    std::vector<uint32_t> holderblob;
    size_t payloadsize = 0;
    bool status = holder->serializeWithPayload(cl.sel,holderblob,payloadsize);
    if(!status){
      MLOG(WARNING) << "problem serializing holder: " << *holder << endmsg;
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
    MLOG(DEBUG) << "serialization of feature: " << holder->typeClid() << " label: " << holder->label()
		<< " size of payload: " << output.size() << endmsg;
    
    std::vector<uint32_t> holderblob;
    bool status = holder->serialize(holderblob);
    if(!status){
      MLOG(WARNING) << "problem serializing holder: " << *holder << endmsg;
      return false;
    }
    output.push_back(holderblob.size()); //leading bit indicates size
    output.insert(output.end(),holderblob.begin(),holderblob.end());
    
    holderblobsizes.push_back(1+holderblob.size()); //one for leading size bit 
    clid_name.push_back(std::pair < CLID, std::string> (holder->typeClid(), holder->label()));
  }
  return true;
}
#undef MLOG
