/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator> // remove it (it is here to help with debugging)



//#include <boost/cstdint.hpp>
//#include <stdint.h>
#include <boost/lexical_cast.hpp>
#include "GaudiKernel/IConversionSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "SGTools/DataBucketBase.h"


#include "TrigNavigation/TriggerElement.h"

#include "TrigConfHLTData/HLTUtils.h"

#include "TrigNavigation/TrigEDMSizes.h"
#include "TrigNavigation/RoICacheHistory.h"

#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include "TrigSerializeResult/StringSerializer.h"


#include "TrigNavigation/NavigationCore.h"


//#define MLOG(x)   if (m_log->level()<=MSG::x+m_msgLvlOffset) *m_log << (m_msgLvlOffset+MSG::x)
#define MLOG(x)   if (m_log->level()<=MSG::x) *m_log << MSG::x

using namespace HLT;
using namespace HLTNavDetails;


template<typename T>
std::ostream& operator<<( std::ostream& s, const std::vector<T>& v)
{
  s<< "[";for(auto i:v){s << i << ", ";} s << "]";
  return s;
}




std::string NavigationCore::m_unspecifiedLabel = "";
const TriggerElement* NavigationCore::m_unspecifiedTE = 0;



NavigationCore::NavigationCore()
  : TrigNavStructure(), m_log(0), m_serializerSvc("TrigSerializeCnvSvc","NavigationCore"),
    m_storeGate(0),
    //m_serializeSpaceLimit(8000000),
    m_objectsKeyPrefix("HLT"),
    m_objectsIndexOffset(0),
    m_referenceAllClasses(true) {

  m_log = new MsgStream(Athena::getMessageSvc(), "NavigationCore");

  m_stringSerializer = new StringSerializer();
}

NavigationCore::~NavigationCore() {


  MLOG(VERBOSE) << "~NavigationCore: cleaning static type information" << endreq;
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

  delete m_stringSerializer;
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


bool NavigationCore::addBlob(std::vector<uint32_t>& output,
			     const std::vector<uint32_t>& blob1,
			     const std::vector<uint32_t>& blob2) const {
  output.push_back(blob1.size()+blob2.size());
  output.insert(output.end(), blob1.begin(), blob1.end());
  output.insert(output.end(), blob2.begin(), blob2.end());
  //  copy(blob.begin(), blob.end(), std::ostream_iterator<int>(std::cerr, " A "));
  //  std::cerr << std::endl;
  return true;
}


bool NavigationCore::extractBlob(const std::vector<uint32_t>& input, std::vector<uint32_t>::const_iterator& it, std::vector<uint32_t>& blob) const {
  blob.clear();
  if ( it == input.end() )
    return false;

  unsigned sizeOfBlob = *it;
  //  MLOG(DEBUG) << "blob size is: " << sizeOfBlob << endreq;
  std::vector<uint32_t>::const_iterator begin = it;
  ++begin;
  std::vector<uint32_t>::const_iterator end   = it;
  ++end;
  advance(end, sizeOfBlob);

  if ( end <= input.end()) {
    blob.reserve(end-begin);
    blob.insert(blob.end(), begin, end);
    //    copy(blob.begin(), blob.end(), std::ostream_iterator<int>(std::cerr, " X "));
    //    std::cerr << std::endl;
    it = end; // update iterator
    return true;
  }
  return false; // failed to get blob
}

bool doSingleHolderSerialization(std::vector<uint32_t>& headerBlob, std::vector<uint32_t>& dataBlob, HLTNavDetails::IHolder* holder, const ServiceHandle<IConversionSvc>& serializerSvc, bool dataSerialization ) {
  holder->serialize( headerBlob );
  
  if (dataSerialization) {
    DataObject* dobj = holder->getDataObject();
    TrigStreamAddress* addr(0);
    if ( serializerSvc->createRep(dobj, *pp_cast<IOpaqueAddress>(&addr) ).isSuccess() ) {
      REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "serialization of feature (object) successful, blob size: "  << addr->get().size();
      dataBlob.push_back(addr->get().size());
      dataBlob.insert(dataBlob.end(), addr->get().begin(), addr->get().end());
      // above should be optimized (need to know if in case of failed serialization addr->get() contains empty data vector)
    }
    else{
      REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR,"NavigationCore") << "Container serialization Failure" << endreq;
      return false;
    }
    //dobj->addRef();
    DataBucketBase* dobjBase = static_cast<DataBucketBase*>(dobj);
    if ( dobjBase ) {
      dobjBase->relinquish();
      REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "serialization giving up the ownership";
    }
    delete dobj;
    delete addr;
    
    DataObject* dobjaux = holder->getAuxDataObject();
    TrigStreamAddress* auxaddr(0);
    if(dobjaux){
      REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "AuxStore serialization" << endreq;
      if(serializerSvc->createRep(dobjaux,*pp_cast<IOpaqueAddress>(&auxaddr) ).isSuccess() ){
	REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "aux conversion success! aux blob has size: " << auxaddr->get().size();
	dataBlob.push_back(auxaddr->get().size());
	dataBlob.insert(dataBlob.end(), auxaddr->get().begin(), auxaddr->get().end());
      }
      else{
	REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR,"NavigationCore") << "AuxStore serialization Failure" << endreq;
	return false;
      }
      DataBucketBase* dobjBaseAux = static_cast<DataBucketBase*>(dobjaux);
      if ( dobjBaseAux ) {
	dobjBaseAux->relinquish();
	REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "serialization giving up the ownership of Aux";
      }
      delete dobjaux;
      delete auxaddr;
    }
  }

  REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "finished serializing dumping blobs. header size: " 
							     << headerBlob.size() << " data size: " << dataBlob.size();
  // std::cout << "[";for(auto i:headerBlob){std::cout << i << ", ";} std::cout << "]" << std::endl;
  // std::cout << "[";for(auto i:dataBlob){std::cout << i << ", ";} std::cout << "]" << std::endl;

  return true;
}

bool doSingleHolderDeserialization(const std::vector<uint32_t>& dataBlob, HLTNavDetails::IHolder* holder, const ServiceHandle<IConversionSvc>& serializerSvc, StoreGateSvc* /*storegate*/, const std::string& sgkey, CLID clid, int version){
  REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "deserializing a data blob of size " << dataBlob.size() << " navi version is " << version;
  
  typedef std::vector<uint32_t>::const_iterator it_type;
  it_type it = std::begin(dataBlob);
  
  if(!dataBlob.size()) return false;


  std::vector<uint32_t> first;
  std::vector<uint32_t> second;

  if(version == 4){
    auto firstsize = *(it++);
    REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "first part has size: " << firstsize;

    first = std::vector<uint32_t>(it,it+firstsize);
  
    std::advance(it,firstsize);  
  
    if(!(it!=dataBlob.end())){
      REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "this datablob only has a first part (non xAOD case)";
    }
    else{
      auto secondsize = *(it++);
      REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "second part has size: " << secondsize;
      second = std::vector<uint32_t>(it,it+secondsize);
    }
  }
  else{
    first = std::vector<uint32_t>(dataBlob.begin(),dataBlob.end());
  }

  TrigStreamAddress addr(clid, sgkey, "", 0, 0);
  addr.add(first);
  DataObject* dobj(0);

  if (serializerSvc->createObj((IOpaqueAddress*)&addr, dobj).isFailure() ){
    REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,"NavigationCore") << "deserialize main: failed" << std::endl;
    return false;
  }

  bool setcontainer = holder->setDataObject(dobj);
  if(!setcontainer) return false;

  DataObject* dobjaux(0);
  if(!second.empty()){
    std::string sgkeyaux = sgkey+"Aux.";
    REPORT_MESSAGE_WITH_CONTEXT(MSG::VERBOSE,"NavigationCore") << "aux clid was deduced to be: " << holder->auxClidOrZero();
    TrigStreamAddress auxaddr(holder->auxClidOrZero(), sgkeyaux, "", 0, 0);
    auxaddr.add(second);

    if (serializerSvc->createObj((IOpaqueAddress*)&auxaddr, dobjaux).isFailure() ){
      REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,"NavigationCore") << "Aux Store deserialization failed";
      return false;
    }
    bool setaux = holder->setAuxDataObject(dobjaux);
    if(!setaux) return false;
  }



  return true;  
}


bool NavigationCore::serialize( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts ) const {
  std::vector<std::pair<CLID, std::string> > clid_name;
  clid_name.clear();
  return serialize(output, cuts, clid_name);
}

bool NavigationCore::serialize( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name ) const {

  const std::vector<TriggerElement*>& fullList =  m_factory.listOfProduced();
  std::vector<TriggerElement*> all;
  for(std::vector<TriggerElement*>::const_iterator iter = fullList.begin(); iter != fullList.end(); ++iter) {
    if(! (*iter)->transient() ) {
      all.push_back( *iter );
    }
  }

  cuts.clear();
  clid_name.clear();

  // version
  unsigned int version=4;
  output.push_back(version);

  MLOG(DEBUG) << "NavigationCore::serialize: serializing with version " << version << endreq;

  unsigned int totalSizeIndex = output.size();
  output.push_back(0); // reserve one word (accessible under the index totalSize), it is 0 if there was truncation here
                       // and != output.size() if truncation using cuts

  cuts.push_back(output.size()); // mark a cut place

  unsigned int endSizeIndex = output.size();
  output.push_back(0); // reserve one word (accessible under the index startSize)


  // reserve space (in order to be efficient it should not be resized any more, this can be studied probably later to find optimum)
  output.reserve(10*all.size());

  // SERIALZE NAVIGATION STRUCTURE
  // size (measured in elements)
  output.push_back(all.size());

  // helper keys
  std::map<TriggerElement*, uint16_t> keys;

  const TriggerElement* previous = 0;
  std::vector<TriggerElement*>::const_iterator it;
  uint16_t indexForTe = 0;
  for ( it = all.begin(); it != all.end(); ++it ) {
    // first we stream pointer as it (this is already an unique key for this TE)
    //    output.push_back((unsigned int)(*it));
    (*it)->serialize(output, keys, previous);
    previous = *it;
    keys[*it] =  indexForTe;
    indexForTe++;
  }
  unsigned int endSize = output.size();

  MLOG(DEBUG) << "serialize: Sizes start: " << endSizeIndex << " end: " << endSize <<
    " TEs: " << m_factory.listOfProduced().size() << endreq;
  output[endSizeIndex] = endSize; // one can use it for cutting out all features

  cuts.push_back(output.size()); // mark a cut place


  // SERIALIZE FEATURES
  std::vector<uint32_t> headerBlob;
  std::vector<uint32_t> dataBlob;


  if ( m_classesToPayload.empty() ) { // this is offline case
    FeaturesStructureLabelIndexed::const_iterator clidIt;
    for ( clidIt = m_featuresByLabel.begin(); clidIt != m_featuresByLabel.end(); ++clidIt ) {
      std::map<std::string, IHolder*>::const_iterator holderIt;
      for ( holderIt = clidIt->second.begin(); holderIt != clidIt->second.end(); ++holderIt ) {

        MLOG(DEBUG) << "serialization of featue attempting : " << clidIt->first << " " << holderIt->first << endreq;

        IHolder *holder = holderIt->second;
        if ( ! holder ) {
          MLOG(DEBUG) << "serialization of feature skipped, nothing know on this objects"  << endreq;
          continue;
        }
        MLOG(DEBUG) << "serialization of feature: " << holder->typeClid() << " label: " << holder->label()
                    << " size of payload: " << output.size() << endreq;

        headerBlob.clear();
        dataBlob.clear();
        bool status = doSingleHolderSerialization(headerBlob, dataBlob, holder, m_serializerSvc, false );
        addBlob( output, headerBlob, dataBlob );
	clid_name.push_back(std::pair < CLID, std::string> (holder->typeClid(), holder->label()));
        cuts.push_back(output.size()); // mark truncation point
        MLOG(DEBUG) << "serialization of feature: " << holder->typeClid() << " label: " << holder->label()
                    << " size of blob: " << headerBlob.size()+dataBlob.size() << " done with status: " << status  << endreq;
      }
    }
  } else { // this is online case when list of classes to payload is not empty
    // prepare sizes object
    //
    std::string name_TrigEDMSizes("TrigEDMSizes");

    // check if SG contains a previously registered TrigEDMSizes object
    // reuse it and update it
    // this is an issue when running L2 and EF together in athena
    TrigEDMSizes* sizes(0);
    if ( m_storeGate->transientContains(ClassID_traits<HLT::TrigEDMSizes>::ID(), name_TrigEDMSizes) ) {
      MLOG(DEBUG) << "A previously registered object of type = (HLT::TrigEDMSizes) and name = " << name_TrigEDMSizes << " was found in SG." << endreq;
      const TrigEDMSizes* sizesSG(0) ;
      if ( m_storeGate->retrieve(sizesSG, name_TrigEDMSizes).isFailure() ) {
        MLOG(WARNING) << "There was an error when retrieving the old object of type = (HLT::TrigEDMSizes) and name = " << name_TrigEDMSizes << " from SG." << endreq;
      } else {
        sizes = const_cast<TrigEDMSizes*>(sizesSG);
      }
    } else {
      if ( m_storeGate->contains(ClassID_traits<HLT::TrigEDMSizes>::ID(), name_TrigEDMSizes) ) {
        MLOG(DEBUG) << "An object of type = (HLT::TrigEDMSizes) and name = " << name_TrigEDMSizes << " accessible through SG was found." << endreq;
        const TrigEDMSizes* sizesSG(0) ;
        if ( m_storeGate->retrieve(sizesSG, name_TrigEDMSizes).isFailure() ) {
          MLOG(WARNING) << "There was an error when retrieving the object contained in SG of type = (HLT::TrigEDMSizes) and name = " << name_TrigEDMSizes << "." << endreq;
        } else {
          sizes = const_cast<TrigEDMSizes*>(sizesSG);
        }
      } else {
        sizes = new TrigEDMSizes();
        m_storeGate->record(sizes, name_TrigEDMSizes).setChecked(); //
        MLOG(DEBUG) << "A new object of type = (HLT::TrigEDMSizes) and name = " << name_TrigEDMSizes << " was registered in SG." << endreq;
      }
    }
    
    MLOG(DEBUG) << "serialization: number of classes to payload: " << m_classesToPayload.size() << endreq;

    std::vector<std::pair<CLID, std::string> >::const_iterator cl;
    for ( cl =  m_classesToPayload.begin();  cl != m_classesToPayload.end(); ++cl ) {
      CLID clid = cl->first;
      std::string key = cl->second;
      MLOG(DEBUG) << "serialization (ordered) of featue attempting : " << clid << " " << key << endreq;
      FeaturesStructureLabelIndexed::const_iterator clidIt = m_featuresByLabel.find(clid);
      if ( clidIt != m_featuresByLabel.end() ) {
        std::map<std::string, IHolder*>::const_iterator holderIt = clidIt->second.find(key);
        if ( holderIt != clidIt->second.end() ) {
          IHolder *holder = holderIt->second;
          if ( ! holder ) {
            MLOG(DEBUG) << "serialization (ordered) of feature skipped, nothing know on this objects"  << endreq;
            continue;
          }
          MLOG(DEBUG) << "serialization (ordered) of feature: " << holder->typeClid() << " label: " << holder->label()
                      << " size of payload: " << output.size() << endreq;
          headerBlob.clear();
          dataBlob.clear();
          bool status = doSingleHolderSerialization(headerBlob, dataBlob, holder, m_serializerSvc, true );
          addBlob( output, headerBlob, dataBlob );
	  clid_name.push_back(std::pair < CLID, std::string> (holder->typeClid(), holder->label()));
          cuts.push_back(output.size()); // mark truncation point
          MLOG(DEBUG) << "serialization of feature: " << holder->typeClid() << " label: " << holder->label()
                      << " size of blob: " << headerBlob.size()+dataBlob.size() << " done with status: " << status << endreq;
          if (sizes) sizes->addObject(holder->collectionName(), holder->label(), dataBlob.size(), output.size() );
        }
      }
    }
  }

  MLOG(DEBUG) << "serialization done" << endreq;

  output[totalSizeIndex] = output.size();
  return true;
}

bool NavigationCore::serialize_DSonly( std::vector<uint32_t>& output, std::vector<unsigned int>& cuts, std::vector<std::pair<CLID, std::string> >& clid_name ) const {
  
  cuts.clear();
  clid_name.clear();
  
  cuts.push_back(output.size()); // mark a cut place
  
  std::vector<uint32_t> headerBlob;
  std::vector<uint32_t> dataBlob;

  std::vector<std::pair<CLID, std::string> >::const_iterator cl_DS;
  for ( cl_DS =  m_classesToPayload_DSonly.begin();  cl_DS != m_classesToPayload_DSonly.end(); ++cl_DS ) {
    CLID clid = cl_DS->first;
    std::string key = cl_DS->second;
    MLOG(DEBUG) << "serialization (ordered) of feature attempting : " << clid << " " << key << endreq;
    FeaturesStructureLabelIndexed::const_iterator clidIt = m_featuresByLabel.find(clid);
    if ( clidIt != m_featuresByLabel.end() ) {
      std::map<std::string, IHolder*>::const_iterator holderIt = clidIt->second.find(key);
      if ( holderIt != clidIt->second.end() ) {
	IHolder *holder = holderIt->second;
	if ( ! holder ) {
	  MLOG(DEBUG) << "serialization (ordered) of feature skipped, nothing know on this objects"  << endreq;
	  continue;
	}
	MLOG(DEBUG) << "serialization (ordered) of feature: " << holder->typeClid() << " label: " << holder->label()
		    << " size of payload: " << output.size() << endreq;
	headerBlob.clear();
	dataBlob.clear();
	bool status = doSingleHolderSerialization(headerBlob, dataBlob, holder, m_serializerSvc, true );
	addBlob( output, headerBlob, dataBlob );
	clid_name.push_back(std::pair < CLID, std::string> (holder->typeClid(), holder->label()));
	cuts.push_back(output.size()); // mark truncation point
	MLOG(DEBUG) << "serialization of feature: " << holder->typeClid() << " label: " << holder->label()
		    << " size of blob: " << headerBlob.size()+dataBlob.size() << " done with status: " << status << endreq;
	//if (sizes) sizes->addObject(holder->collectionName(), holder->label(), dataBlob.size(), output.size() );
      }
    }
  }
  return true;
}


/*****************************************************************************
 *
 * DESERIALIZATION
 *
 *****************************************************************************/

bool NavigationCore::deserialize( const std::vector<uint32_t>& input ) {
  using namespace std;
  std::vector<uint32_t>::const_iterator inputIt = input.begin();

  if (input.size()==0) {
    MLOG(WARNING) << "Cannot work on empty payload" << endreq;
    return false;
  }
  unsigned int  version = *inputIt++; // ignore version

  MLOG(DEBUG) << "deserialize: the serialized input has versions " << version << endreq;


  if ( version != 3 and version !=4  ) {
    MLOG(WARNING) << "No backward compatibility beyond version 3 possible; data was serialized with V: " << version
                << " while we are in version 4" << endreq;
    return true;
  }
  //  version += 0; // this is to shut up gcc
  MLOG(DEBUG) << "deserialize: deserialization of Navigation version: " << version << endreq;

  unsigned int totalSize =  *inputIt++; // total size
  if ( totalSize != input.size() )
    MLOG(WARNING) << "deserialize: the navigation is truncated: " << input.size()
                  << " while should be: " << totalSize  << endreq;

  if ( input.size() == 2 ) { // there was only space for version and size
    MLOG(WARNING) << "deserialize: the navigation is truncated badly, no recovery possible "  << endreq;
    return false;
  }

  unsigned int endOfChunk  = *inputIt++; // size of the chunk
  //  std::cerr << "Deserialization ot the chunk: " << endOfChunk  << std::endl;

  unsigned int size = *inputIt++;
  MLOG(DEBUG) << "deserialize: Number of TEs: " << size  << endreq;
  TriggerElement* previous = 0;
  std::map<uint16_t, TriggerElement* > keys;

  for ( unsigned int i = 0; i < size; ++i ) {

    // create new TE

    TriggerElement* te = m_factory.produce(TriggerElement::enquireId(inputIt)); //
    te->deserialize(inputIt, keys, previous);
    previous = te;
    // keys table for deserialization of other TEs
    keys[i] = te;

  }

  if ( not m_factory.empty() ) {

    // rebuild  sameRoI relations (this can't be done by TEs deserialization)
    TriggerElement* initialNode = getInitialNode();
    std::vector<TriggerElement*>::const_iterator roiTEit;
    for ( roiTEit = getDirectSuccessors(initialNode).begin(); 
	  roiTEit != getDirectSuccessors(initialNode).end(); ++roiTEit ) {
      fillSameRoIRelation((*roiTEit), (*roiTEit));
    }
  } else {
    MLOG(INFO) << "deserialize: the navigation TEs structure is empty, initial Node is missing" << endreq;
    return false;
  }
  

  // EOF TEs deserialization
  // deserialize Features
  std::vector<uint32_t>::const_iterator it=input.begin();
  advance(it, endOfChunk );
  std::vector<uint32_t> blob;
  while ( extractBlob(input, it, blob) ) {

    CLID        clid;
    uint16_t    idx;
    std::string label;
    //    IHolder::SerializationMethod sm;
    std::vector<uint32_t>::const_iterator blobIt= blob.begin();

    IHolder::enquireSerialized(blob, blobIt, clid, label, idx);
    MLOG(VERBOSE) << "deserialize: extracted blob CLID: " << clid << " of size: " << blob.size()
                  << " SubTypeIndex: " << idx << " Label: " << label << endreq;

    
    IHolder *holder(0);
    if (! createHolder(holder, clid, label, idx) ) {
      MLOG(ERROR) << "deserialize: Could not create holder for CLID " << clid
                  << " this probably means that the package holding the class for this CLID was not compiled against the TrigNavigation package in use!"
                  << endreq;
      continue;
      //return false;
    }
    
    registerHolder(holder); // ??? check status

    // now we have the blobIt pointing to the place where objects may start
    // if so we ought to do deserialization
    if ( blob.end() != blobIt ) {
      if ( m_storeGate->transientContains(holder->containerClid(), holder->key()) ) {
	
        MLOG(DEBUG) << "deserialize: while working on: " << holder->collectionName()
                    << " and key: " << holder->key()
                    << " from serialized form found it in SG already, OK if running transient BS job, SG has priority" << endreq;

        // remove obj from SG
        holder->syncWithSG();
        holder->clearSG();
      }
 
      {
        std::vector<uint32_t>::const_iterator constEnd = blob.end();
        std::vector<uint32_t> blobpart2(blobIt, constEnd);
        //      std::vector<uint32_t>::const_iterator bit;

        //      for ( bit = blobIt; bit != blob.end(); ++it ) {
        //	blobpart2.push_back(*bit);
        //      }
        MLOG(DEBUG) << "deserialize: going to deserialize clid: " << clid << " from its collection: "
                    << holder->containerClid() << endreq;

        if (holder->containerClid()<1)
          MLOG(WARNING) << "deserialize: cannot map to a collection type by holder->containerClid()" << endreq;

	doSingleHolderDeserialization(blobpart2,holder,m_serializerSvc,m_storeGate,holder->key(),holder->containerClid(),version);
      }
      if ( !holder->syncWithSG() ) {
        MLOG(WARNING) << "deserialize: after deserialization obejct not accessible though SG"  << endreq;
      }
    }


  }
  return true;
}






















bool NavigationCore::merge(const NavigationCore& l2) {
  // we need to pick the holders which are at L2 and move them to EF
  FeaturesStructureLabelIndexed::const_iterator clidIt;

  for ( clidIt = l2.m_featuresByLabel.begin(); clidIt != l2.m_featuresByLabel.end(); ++clidIt ) {
    CLID c = clidIt->first;
    std::map<std::string, IHolder*>::const_iterator labelIt;
    for ( labelIt = clidIt->second.begin(); labelIt != clidIt->second.end(); ++labelIt ) {
      IHolder *l2holder = labelIt->second;
      if ( m_log->level()<=MSG::DEBUG )
        *m_log << MSG::DEBUG << "will add holder " << *l2holder << endreq;
      std::string label(l2holder->label());
      IHolder* efholder = getHolder(c, label);
      if ( efholder != 0 ) {
        if ( (efholder->label() != l2holder->label())
             || (efholder->subTypeIndex() != l2holder->subTypeIndex()) ) {

          if ( m_log->level()<=MSG::DEBUG )
            *m_log << MSG::DEBUG << "fixing bug for " << *efholder <<    endreq;
          // bug which we need to fix (some week of data in autumn 2008)
          // we have to delete current holder
          // and replace it by L2 one
          //
          // efholder->setSubTypeIndex(l2holder->subTypeIndex());

          if ( m_log->level()<=MSG::DEBUG )
            *m_log << MSG::DEBUG << "after fixing   " << *efholder <<    endreq;
        }
      } else {
        if ( m_log->level()<=MSG::DEBUG )
          *m_log << MSG::DEBUG << "cloning the holder from L2 " << *l2holder << endreq;
        efholder = HLT::TypeMaps::holders()[c]->clone(l2holder->label(), l2holder->subTypeIndex());
        efholder->prepare(m_log, m_storeGate);
        registerHolder(efholder);
      }
    } // over labels
  } // over CLIDS
  return true;
}









/*****************************************************************************
 *
 * very important RESET
 *
 *****************************************************************************/
void NavigationCore::reset() {
  TrigNavStructure::reset();

  // clear structure after previous event
  FeaturesStructure::iterator featuresIt;
  for ( featuresIt = m_featuresByIndex.begin(); featuresIt != m_featuresByIndex.end(); ++featuresIt ) {

    std::map<uint16_t, IHolder*>::iterator subTypeIt;
    for ( subTypeIt = featuresIt->second.begin(); subTypeIt != featuresIt->second.end(); ++subTypeIt ) {
      if ( m_log->level()<=MSG::DEBUG )
        *m_log << MSG::DEBUG << "Deleting holder for clid:" << featuresIt->first << " and sub type idx " << subTypeIt->first << endreq;
      //	m_log*(subTypeIt->second) << endreq;
      delete subTypeIt->second;
      subTypeIt->second = 0;
    }
    //    featuresIt->second.clear();
  }
  m_featuresByIndex.clear();
  m_featuresByLabel.clear();
  if ( m_log->level()<=MSG::DEBUG )
    *m_log << MSG::DEBUG << "Navigation reset done" << endreq;
}

uint16_t NavigationCore::nextSubTypeIndex(CLID clid, const std::string& /*label*/) {
  // this is tricky (we need to find next free slot (subTypeIndex)
  // For that we look at the last registered holder in the map indexed by type

  // label actually does not matter here
  //
  if ( m_featuresByIndex[clid].size() == 0 )
    return m_objectsIndexOffset;
  return (--m_featuresByIndex[clid].end())->first+1;
}



HLTNavDetails::IHolder* NavigationCore::prepareOneHolder(CLID clid, const std::string& label) {

  MLOG( VERBOSE ) << "NavigationCore::prepare preregistering objects of clid: " << clid << " label: " << label << endreq;
  IHolder *holder = getHolder(clid, label);
  if ( holder ) {    
    MLOG( VERBOSE ) << "NavigationCore::prepare preregistering objects  not executed as it already exists " << *holder << endreq;
    return holder;
  }
  
  uint16_t index =  nextSubTypeIndex(clid, label);
  if ( m_log->level() <= MSG::DEBUG )
    MLOG( VERBOSE ) << "NavigationCore::prepare creating handler for type (CLID): " << clid
			 << " label: " << label << " index: " << index << endreq;
  
  if ( !createHolder(holder, clid, label, index) ) {
    MLOG( INFO ) << "NavigationCore::prepare Can't create storage for objects of CLID: " << clid << " as it is requested by configuration" << endreq;
      return 0;
  }

  MLOG( VERBOSE ) << "Holder created, registering " << holder << " " << *holder << endreq;

  if ( !registerHolder(holder) ) {
    MLOG( WARNING ) << "Holder registration failed " << holder << " " << *holder << endreq;
    return 0;
  }
  if ( !holder->syncWithSG() ) {
    MLOG( WARNING ) << "Holder SG sync failed" << holder << " " << *holder << endreq;
    return 0;
  }
  
  return holder;
}

void NavigationCore::prepare() {

  if ( m_log->level()<=MSG::DEBUG ) {
    HLT::TypeMaps::CLIDtoHolderMap::const_iterator hIt;
    for ( hIt = HLT::TypeMaps::holders().begin(); hIt != HLT::TypeMaps::holders().end(); ++hIt ) {
      MLOG( VERBOSE ) << "NavigationCore::prepare Compile time known types : " << *(hIt->second) << endreq;
    }
  }
  
  MLOG( VERBOSE ) << "NavigationCore::prepare Preregistering objects #:" <<  m_classesToPreregister.size()<< endreq;
  // populate structure with "must have" features

  std::vector<std::pair<CLID, std::string> >::const_iterator confIt;
  for ( confIt = m_classesToPreregister.begin(); confIt != m_classesToPreregister.end() ; ++confIt ) {
    CLID clid = confIt->first;
    std::string label  = confIt->second;
    if ( prepareOneHolder(clid, label) == 0 ) {
      MLOG( WARNING  )<< "NavigationCore::prepare failed preparing the holder for CLID: " << clid << " and label " << label << endreq;
    }
  }

  MLOG( DEBUG  )<< "NavigationCore::prepare Navigation structure prepared for next event" << endreq;
}

bool NavigationCore::registerHolder(IHolder* holder) {
  /*  if ( syncWithSG )
      if (  ! holder->syncWithSG() ) {
      *m_log << MSG::WARNING << "createHolder: super-container can't be recorded to SG for CLID:" << holder->typeClid()  << " and label: " << holder->label() << " sub type index: " << holder->subTypeIndex() << endreq;
      // return false;
      }
  */
  if ( m_featuresByLabel.count(holder->typeClid()) != 0 || m_featuresByIndex.count(holder->typeClid()) != 0 ) {
    if (m_featuresByIndex[holder->typeClid()].count(holder->subTypeIndex()) != 0
        || m_featuresByLabel[holder->typeClid()].count(holder->label()) != 0 ) {
      *m_log << MSG::WARNING << "registerHolder registration failing, there are already registered ones (in byIndex map): "
             << m_featuresByIndex[holder->typeClid()].count(holder->subTypeIndex())
             << " OR (byLabel map): " << m_featuresByLabel[holder->typeClid()].count(holder->label()) << endreq;
      return false;
    }
  }

  m_featuresByIndex[holder->typeClid()][holder->subTypeIndex()] = holder;
  m_featuresByLabel[holder->typeClid()][holder->label()] = holder;
  m_lookupLabels[holder->typeClid()][holder->subTypeIndex()] = holder->label();
  *m_log << MSG::DEBUG <<  "registerHolder for OK " << *holder << endreq;
  return true;
}



bool NavigationCore::createHolder( IHolder*& holder,  CLID clid, const std::string& label, uint16_t index) {
  if ( HLT::TypeMaps::holders().count(clid) == 0 ) {
    if (m_log->level()<=MSG::ERROR)
      *m_log << MSG::ERROR << "createHolder: holder can't be done, no predefined storage found for CLID: " << clid << endreq;
    //    throw std::runtime_error("Can not find predefined storage of clid: " + boost::lexical_cast<std::string>(clid) +" and label: " + label );
    return false;
  }

  holder = HLT::TypeMaps::holders()[clid]->clone(label, index);
  //  holder->prepare(m_log, m_storeGate, getAux(label));
  holder->prepare(m_log, m_storeGate);
  holder->setObjectsKeyPrefix(m_objectsKeyPrefix);

  if (m_log->level()<=MSG::DEBUG)
    *m_log << MSG::DEBUG << "createHolder: holder prepared " << *holder << endreq;


  return true;
}




IHolder* NavigationCore::getHolder(CLID clid, uint16_t subTypeIndex) const {
  FeaturesStructure::const_iterator clidIt = m_featuresByIndex.find(clid);
  if ( clidIt ==  m_featuresByIndex.end() )
    return 0;
  std::map<uint16_t, IHolder*>::const_iterator indexIt = clidIt->second.find(subTypeIndex);
  if ( indexIt == clidIt->second.end() ) {
    return 0;
  }
  return indexIt->second;
}

IHolder* NavigationCore::getHolder(CLID clid, const std::string& label) const {
  FeaturesStructureLabelIndexed::const_iterator clidIt = m_featuresByLabel.find(clid);
  if ( clidIt ==  m_featuresByLabel.end() )
    return 0;
  std::map<std::string, IHolder*>::const_iterator labelIt = clidIt->second.find(label);
  if ( labelIt == clidIt->second.end() ) {
    return 0;
  }
  return labelIt->second;
}

bool NavigationCore::toBePutToPayload(const HLTNavDetails::IHolder* holder) const {
  std::string option = holder->typeName()+"#"+holder->key();
  //std::string option = holder->typeName();
  std::vector<std::string>::const_iterator it;
  it = find(m_classesToPayloadProperty.begin(), m_classesToPayloadProperty.end(), option);
  if (it != m_classesToPayloadProperty.end() )
    return true;
  return false;
}

void NavigationCore::tweakMsgLvl(int newOffset) {
  //  m_msgLvlOffset = newOffset;
  m_log->setLevel(newOffset);
}

void NavigationCore::testMsgService() const {
  if (m_log) {
    MLOG(DEBUG) << "MLOG printout" << endreq;
    *m_log << MSG::FATAL << "MSG::FATAL" << MSG::FATAL << endreq;
    *m_log << MSG::ERROR << "MSG::ERROR " << MSG::ERROR << endreq;
    *m_log << MSG::WARNING << "MSG::WARNING " << MSG::WARNING << endreq;
    *m_log << MSG::INFO << "MSG::INFO " << MSG::INFO << endreq;
    *m_log << MSG::DEBUG << "MSG::DEBUG " << MSG::DEBUG << endreq;
    *m_log << MSG::VERBOSE << "MSG::VERBOSE " << MSG::VERBOSE <<endreq;
    *m_log << MSG::INFO << "MSG level()=" << m_log->level() << endreq;
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


/*
std::vector< std::string >
NavigationCore::getFeatureSGLabelAndPosition( std::string featureClassName, const TriggerElement* te,
                                              const std::string& label)
{
  return getFeatureSGLabelAndPosition( HLT::TypeMaps::type2clid()[featureClassName], te, label);
}

std::vector< std::string >
NavigationCore::getFeatureSGLabelAndPosition( unsigned int clid, const TriggerElement* te,
                                              const std::string& label)
{
  std::vector< std::string > result;

  MLOG(DEBUG) << "got CLID: " << clid << endreq;
  // now look into the TriggerElement to get from it AccessHelpers (look only at features reated to this TE)
  // bool found = false;
  std::vector< TriggerElement::FeatureAccessHelper >::const_iterator it;
  for ( it = te->getFeatureAccessHelpers().begin(); it != te->getFeatureAccessHelpers().end(); ++it ) {
    MLOG(DEBUG) << "got Feature ObjectIndex " << *it << endreq;

    if ( it->getCLID() == clid ) { // CLID is matching
      // remember an index (in holder vector)
      //uint16_t subTypeIndex = TriggerElement::ObjectIndex(it->getIndex()).subTypeIndex();

      uint16_t   subIndex = it->getIndex().subTypeIndex();

      MLOG(DEBUG) << "found label = " << m_lookupLabels[clid][subIndex] << endreq;

      if ( label == m_lookupLabels[clid][subIndex] or label =="") {
        // insert all entries:
        for (unsigned int i = it->getIndex().objectsBegin();
             i < it->getIndex().objectsEnd(); ++i ) {
          std::ostringstream ss;
          ss << "HLT_" << m_lookupLabels[clid][subIndex] << "::" << i;
          result.push_back( ss.str() );
        }
        // found = true;
      }

    }
  }
  return result;
}

std::vector< std::string >
NavigationCore::getRecentFeatureSGLabelAndPosition( unsigned int clid, const TriggerElement* te,
                                                    const std::string& label)
{
  // start from given TriggerElement
  std::vector< std::string > result = getFeatureSGLabelAndPosition(clid, te, label);

  if ( !result.empty() ) {
    MLOG(DEBUG) << "getRecentFeatureSGLabelAndPosition: found one in: " << te->getId()  << endreq;
    return result; // end of our search
  }
  if ( isRoINode(te) ) {
    return result;    // do not search deeper
  } else { // here the logic needed if the feature was not found attached to the current TE
    std::vector<TriggerElement*>::const_iterator it;
    for ( it = getDirectPredecessors(te).begin(); it != getDirectPredecessors(te).end(); ++it ) {
      MLOG(DEBUG) << "getRecentFeatureSGLabelAndPosition: searching deeper in TE: " << (*it)->getId()  << endreq;

      std::vector< std::string > result2 = getRecentFeatureSGLabelAndPosition(clid, *it, label );

      if ( !result2.empty() )
        return result2; // found feature in subtree of one of the predecessors
    }
  }
  return result;
}

std::vector< std::string >
NavigationCore::getRecentFeatureSGLabelAndPosition( std::string featureClassName, const TriggerElement* te,
						    const std::string& label)
{
  return getRecentFeatureSGLabelAndPosition( type2clid()[featureClassName], te, label);
}



std::vector< unsigned int >
NavigationCore::getFeatureContainerPosition( unsigned int clid, const TriggerElement* te,
                                             const std::string& label)
{
  std::vector<unsigned int > result;

  MLOG(DEBUG) << "got CLID: " << clid << endreq;
  // now look into the TriggerElement to get from it AccessHelpers (look only at features reated to this TE)
  // bool found = false;
  std::vector< TriggerElement::FeatureAccessHelper >::const_iterator it;
  for ( it = te->getFeatureAccessHelpers().begin(); it != te->getFeatureAccessHelpers().end(); ++it ) {
    MLOG(DEBUG) << "got Feature ObjectIndex " << *it << endreq;

    if ( it->getCLID() == clid ) { // CLID is matching
      // remember an index (in holder vector)
      //uint16_t subTypeIndex = TriggerElement::ObjectIndex(it->getIndex()).subTypeIndex();

      uint16_t   subIndex = it->getIndex().subTypeIndex();

      MLOG(DEBUG) << "found label = " << m_lookupLabels[clid][subIndex] << endreq;

      if ( label == m_lookupLabels[clid][subIndex] ) {
        // insert all entries:
        for (unsigned int i = it->getIndex().objectsBegin();
             i < it->getIndex().objectsEnd(); ++i )
          result.push_back( i );
        // found = true;
      }

    }
  }
  return result;
}

std::vector< unsigned int >
NavigationCore::getFeatureContainerPosition( std::string typeName, const TriggerElement* te,
                                             const std::string& label)
{
  return getFeatureContainerPosition(type2clid()[typeName], te, label);
}


std::vector< HLT::FeatureDescriptor >
NavigationCore::getFeatureContainerPosition( unsigned int clid, const TriggerElement* te)
{
  std::vector< HLT::FeatureDescriptor > result;

  MLOG(DEBUG) << "got CLID: " << clid << endreq;
  // now look into the TriggerElement to get from it AccessHelpers (look only at features reated to this TE)
  // bool found = false;
  std::vector< TriggerElement::FeatureAccessHelper >::const_iterator it;
  for ( it = te->getFeatureAccessHelpers().begin(); it != te->getFeatureAccessHelpers().end(); ++it ) {
    MLOG(DEBUG) << "got Feature ObjectIndex " << *it << endreq;

    if ( it->getCLID() == clid ) { // CLID is matching
      // remember an index (in holder vector)
      //uint16_t subTypeIndex = TriggerElement::ObjectIndex(it->getIndex()).subTypeIndex();
      //      unsigned int index = it->getIndex();
      uint16_t   subIndex = it->getIndex().subTypeIndex( );
      std::string label( m_lookupLabels[clid][subIndex] );

      MLOG(DEBUG) << "found label = " << label << endreq;

      result.push_back( FeatureDescriptor(label, it->getIndex().objectsBegin(),
                                          it->getIndex().objectsEnd()) );
      // found = true;

    }
  }
  return result;
}


std::vector< HLT::FeatureDescriptor >
NavigationCore::getFeatureContainerPosition( std::string typeName, const TriggerElement* te)
{
  return getFeatureContainerPosition(type2clid()[typeName], te);
}


std::pair<std::string, std::string> NavigationCore::getClassContainerNames(unsigned int clid) const
{
  IHolder* h = getHolder(clid, 0); 
  if (h) {
    return std::make_pair( h->typeName(), h->collectionName() );
  } else {
    return std::make_pair( "", "" );
  }
}
*/

void NavigationCore::testStaticMaps()
{
  *m_log << MSG::INFO << "print statis map: CLID -> class name "  << endreq;
  for ( static HLT::TypeMaps::NametoCLIDMap::const_iterator it = HLT::TypeMaps::type2clid().begin();
	it != HLT::TypeMaps::type2clid().end(); ++it) {
    *m_log << MSG::INFO << "entry: " << (*it).first << " -> " << (*it).second << endreq;
  }

  *m_log << MSG::INFO << "print statis map: CLID -> holder "  << endreq;
  for ( static HLT::TypeMaps::CLIDtoHolderMap::const_iterator it = HLT::TypeMaps::holders().begin();
	it != HLT::TypeMaps::holders().end(); ++it) {
    *m_log << MSG::INFO << "entry: " << (*it).first << " -> " << (* (*it).second) << endreq;
  }

}

void NavigationCore::setAccessProxy(AccessProxy* ap) {
  m_storeGate = ap;
}

AccessProxy* NavigationCore::getAccessProxy() const {
  return m_storeGate;
}

void NavigationCore::setObjKeyPrefix(const std::string& k) {
  m_objectsKeyPrefix = k;
}

bool NavigationCore::getFeatureAccessors( const TriggerElement* te, CLID clid, const std::string& label,
                                          bool only_single_feature,
                                          TriggerElement::FeatureVec& features,
                                          bool with_cache_recording,
                                          bool travel_backward_recursively,
                                          const TriggerElement*& source,
                                          std::string& sourcelabel ) {

  MLOG(VERBOSE) << "getFeatureAccessors: looking for:" << (only_single_feature ? "one object" : "many objects" ) << " of CLID: " << clid
                << " label: \"" << label << "\""
                << " starting from TE: " << te->getId() << (with_cache_recording ? " with cache recording" : " without cache recording")<< endreq;
  // start cache

  //std::auto_ptr<HLT::RoICacheHistory::QuestionScope> qscope( with_cache_recording ? new  : 0);
  HLT::RoICacheHistory::QuestionScope qscope( with_cache_recording
                                              ? HLT::RoICacheHistory::QuestionScope(te, clid, label, this, only_single_feature)
                                              : HLT::RoICacheHistory::QuestionScope() );

  // record ending point (not this method is called recursivelly)

  int size = te->getFeatureAccessHelpers().size(), it;

  // loop the feature access helper in order depending of type of request (i.e. if single featyure needed then loop from back, if all then loop from the front)
  for ( it = ( only_single_feature ? size-1 : 0 ); it != (only_single_feature ? -1 : size ); only_single_feature ? it--: it++ ) {
    //for ( it = 0; it < size; ++it ) {
    const TriggerElement::FeatureAccessHelper& fea = te->getFeatureAccessHelpers().at(it);
    MLOG(VERBOSE) << "getFeatureAccessors: in a loop over FeatureAccessHelpers got ObjectIndex " << fea << endreq;


    if ( fea.getCLID() == clid ) { // CLID is matching
      // remember an index (in holder vector)
      TriggerElement::ObjectIndex objectIndex(fea.getIndex());
      HLTNavDetails::IHolder *holder = getHolder(clid, objectIndex.subTypeIndex());
      if  (! holder ) {
        MLOG(WARNING) << "getFeatureAccessors: feature of clid: " << clid <<  " and index: " << objectIndex.subTypeIndex()
                      << " not available. Probably not recorded. Navigation structure inconsistent. " << endreq;
        continue;
      }

      // now do labels matching
      // "" (empty) label means all match
      // "!" in the label means the collection having exactly empty label
      // otherwise if label is provided it needs to match exactly
      if (label.empty() || (label == "!" &&  holder->label().empty()) || label == holder->label() ) {
        sourcelabel = holder->label();       
        source = te;
        features.push_back(fea);
        if ( with_cache_recording )  HLT::RoICacheHistory::instance().addAnswer(te, fea);
	
        MLOG(DEBUG) << "getFeatureAccessors: matching feature found in te: " << *te << " index: " << fea << endreq;
        // now the ending (depends on the "single" flag)
        if ( only_single_feature )
          break;
      }
    }
  } // end of loop over feature access helpers of this TE

  if ( ! travel_backward_recursively ) {
    return true;
  }

  // stop digging deeper if this is an RoI node already
  if ( isRoINode(te) ) {
    return true;
  }

  // return if a feature(s) is/are found
  if ( ! features.empty() ) {
    return true;
  }


  // recurse deeper


  // but recurse only when unambiguous
  //if ( (label.empty() || only_single_feature ) &&  getDirectPredecessors(te).size() > 1  ) {
  bool recursion_status = true;
  for( TriggerElement* predecessor: getDirectPredecessors(te) ) {
     
    TriggerElement::FeatureVec features_in_branch;
    recursion_status = recursion_status && getFeatureAccessors( predecessor, clid, label, only_single_feature, features_in_branch,
                                                                with_cache_recording, travel_backward_recursively, source, sourcelabel);
    features.insert(features.end(),  features_in_branch.begin(), features_in_branch.end());
  }
    
  if ( only_single_feature &&  ( features.size() > 1 || recursion_status == false) ) {
    MLOG(DEBUG) << "getFeatureAccessors: looking for object of CLID: " << clid
                << " label: \"" << label << "\"" << " found several objects matching criteria while can only return back one, this is ambiguous" << endreq;
      
    if ( getDirectPredecessors(te).size() > 1 ) // mark bifurcation point as to where one can start again
      source = te;

    return false;
  }
  return true;
}

#undef MLOG
