/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetEvent/JetCollection.h"
#include "StoreGate/StoreGateSvc.h"

JetCollection::JetCollection( SG::OwnershipPolicy own)  : 
    DataVector<Jet>(own), 
    m_ordered(Random),
    m_jetAuthor(Jet::s_defaultJetAuthor),
    m_nextId(Jet::s_defaultJetId+1),
    m_keyDescInstance(false),
    m_ownMap(false),
    m_fastjetClustSeq(0)
{
  // by default the keyDescInstance points to the global s_instance's stores .
  // The JetCollection cnv replaces that by the store read from file.
  if( ! JetKeyDescriptorInstance::s_instance.m_Stores ) JetKeyDescriptorInstance::s_instance.createKeyStore(); // this may happen inside Trigger navigation system
  m_keyDescInstance.m_Stores = JetKeyDescriptorInstance::s_instance.m_Stores;
}
JetCollection::~JetCollection(){

  //std::cout << " JetCollection deleting " << m_jetAuthor <<std::endl;
  if(m_ownPolicy == SG::OWN_ELEMENTS){    
    // remove myself from my jets. Otherwise, they might invoke me
    // in their destructors.
    iterator it = begin();
    iterator itE = end();
    for(;it != itE; ++it){
      (*it)->m_collection = NULL;
    }
  }
  
  if( (m_ownMap) && m_momentMapLink.isValid() ) {
    delete m_momentMapLink.cptr();
  }

}

JetKeyDescriptorInstance* JetCollection::keyDesc() const {
  return &m_keyDescInstance;
}

std::string JetCollection::getMomentMapName() const{
  std::string mapname = author();
  mapname +="MomentMap";
  return mapname;
}

const JetMomentMap * JetCollection::getMomentMap() const {

  //std::cout << "asking map in "<< author() << std::endl;
  if ( !m_momentMapLink.isValid() ) {
    // we first try to retrieve it :
    std::string mapname = getMomentMapName();
    m_momentMapLink.toIdentifiedObject ( mapname); // by name, because we don't use DataLink_p2 class yet

    //m_momentMapLink->access(); // this hsould work whe we use DataLink_p2 P class.

    //std::cout << "Datalink invalid set  "<< mapname << std::endl;
    if ( !m_momentMapLink.isValid() ) {
      //std::cout << "Datalink invalid create map  "<< mapname << std::endl;
      // Nothing available from SG/disk. We create the map
      JetMomentMap* map = new JetMomentMap();
      map->m_ownPolicy = m_ownPolicy;
      m_momentMapLink.toStorableObject( *map );
      // This means we own it !
      m_ownMap = true;
    }
  }

  return m_momentMapLink.cptr();
}

size_t JetCollection::getNextId(){
  return m_nextId++; // this returns m_nextId *then* increments it
}

void JetCollection::push_back(Jet *j){
  acquireJet(j);
  DataVector<Jet>::push_back(j);
}


JetCollection::iterator JetCollection::insert(JetCollection::iterator position, Jet* j){
  acquireJet(j);
  return DataVector<Jet>::insert(position,j);  
}

void JetCollection::clear()
{
  DataVector<Jet>::clear();
  m_nextId = Jet::s_defaultJetId+1;
}

void JetCollection::clear(SG::OwnershipPolicy ownPolicy)
{
  DataVector<Jet>::clear(ownPolicy);
  m_nextId = Jet::s_defaultJetId+1;
}

void JetCollection::acquireJet(Jet* j){
  // If the jet is orphan, adopt it 
  //  this means :  acquire its moments if they exist and give it an Id
  // otherwise, we do nothing.

  if ( j->parentCollection() && (m_ownPolicy == SG::OWN_ELEMENTS )) return ; // this case is very bad anyway, there will be problems, we can't do anything.
    
  // In the case this collection has no author, simply acquire those from the jet.
  if( (m_jetAuthor == Jet::s_defaultJetAuthor) && (j->m_jetAuthor != Jet::s_defaultJetAuthor) ) {
    std::string viewS = "";
    if( m_ownPolicy == SG::VIEW_ELEMENTS ) viewS ="View"; // We want to distinguish the name from the jet's original collection.
    m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,j->jetAuthor()+viewS,true); // use the real jetAuthor() (m_jetAuthor index
  }
  //  std::cout << " acquireJet jid="<<j->id() <<"  "<<author()<<std::endl;

  size_t oldId = j->id();
  if( m_ownPolicy == SG::OWN_ELEMENTS ) j->setJetId( getNextId() );  
  if( oldId !=  Jet::s_defaultJetId ){
    // jet might have moments to be tranfered
    const JetMomentMap *jmap = j->getMomentMap(); 
    // The moment map associated to this collection has the same m_ownPolicy, 
    // so the call knows how to deal the moments (i.e. copy or move)
    if( jmap ){ getMomentMap()->transferRecord( jmap, oldId, j->id() ); }
    
  } 
  // set myself as this jet parent now (not before !) if needed
  if( m_ownPolicy == SG::OWN_ELEMENTS ) j->m_collection = this;
  
}


std::string JetCollection::author() const 
{ 
  return keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);  
}
void JetCollection::setAuthor(std::string author){
  m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,author,true); 
}

void JetCollection::recordedInSG(std::string key){
  //std::cout << "JetCollection Recorded "<< m_momentMapLink.cptr() << std::endl;
  m_ownMap = false;
  if( key == author() ){
    key = getMomentMapName();
    m_momentMapLink.toIdentifiedObject ( key);
  }else {
    // this should never happen, so I let the raw std::cout
    std::cout <<  " JetCollection ERROR attempt to record in SG with key ="<< key << "  while author ="<< author() << " both should be the same."<< std::endl;
  }

 
    //

}

void JetCollection::resetJetId(std::size_t id_) {

  //make sure the jet belongs to us
  if (m_ownPolicy == SG::OWN_ELEMENTS) {
     if ( this->at(id_)->m_collection != this) return;
  }
  
  //this allows changing the jet id of a view collection...not ideal
 
  this->at(id_)->setJetId(id_+1);
  
}
