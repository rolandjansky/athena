/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// Implementation file for class Eventbookkeeper
// Author: DavidCote, September 2008 <david.cote@cern.ch>
/////////////////////////////////////////////////////////////////// 
 
#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"

////////////////
/// Constructors
////////////////

EventBookkeeper::EventBookkeeper()
{ 
  SetDefaultDataMemberValues();
}

EventBookkeeper::EventBookkeeper(const std::string &name)
{ 
  SetDefaultDataMemberValues();
  m_name=name;
}

EventBookkeeper::EventBookkeeper(const std::string& name, 
                                 const std::string& description, 
                                 const std::string& logic)
{ 
  SetDefaultDataMemberValues();
  m_name=name;
  m_description=description;
  m_logic=logic;
}

EventBookkeeper::EventBookkeeper( const EventBookkeeper& rhs )
{
  m_name=rhs.m_name;
  m_description=rhs.m_description;
  m_inputstream=rhs.m_inputstream;
  m_outputstream = rhs.m_outputstream;
  m_logic = rhs.m_logic;
  m_nAcceptedEvents=rhs.m_nAcceptedEvents;
  m_nWeightedAcceptedEvents=rhs.m_nWeightedAcceptedEvents;
  m_cycle=rhs.m_cycle;
  m_parentIndex=rhs.m_parentIndex;
  //Make a new deep copy of the children, as these will be owned by this
  m_childrenEB = new std::vector<EventBookkeeper*>; 
  m_childrenEB->reserve(rhs.m_childrenEB->size());
  for(unsigned int i=0; i<rhs.m_childrenEB->size(); i++){ 
    EventBookkeeper* child=new EventBookkeeper(*rhs.m_childrenEB->at(i));
    m_childrenEB->push_back(child); 
  }
  m_childrenIndices = new std::vector<unsigned int>;
  m_childrenIndices->reserve(rhs.m_childrenIndices->size());
  for(unsigned int i=0; i<rhs.m_childrenIndices->size(); i++){ 
    m_childrenIndices->push_back(rhs.m_childrenIndices->at(i));
  }
  m_declaredChildFilter=rhs.m_declaredChildFilter;
  m_declaredTopFilter=rhs.m_declaredTopFilter;
}

EventBookkeeper&
EventBookkeeper::operator=(const EventBookkeeper& rhs)
{
  if (this != &rhs) {
    m_name=rhs.m_name;
    m_description=rhs.m_description;
    m_inputstream=rhs.m_inputstream;
    m_outputstream = rhs.m_outputstream;
    m_logic = rhs.m_logic;
    m_nAcceptedEvents=rhs.m_nAcceptedEvents;
    m_nWeightedAcceptedEvents=rhs.m_nWeightedAcceptedEvents;
    m_cycle=rhs.m_cycle;
    m_parentIndex=rhs.m_parentIndex;
    //Make a new deep copy of the children, as these will be owned by this
    m_childrenEB = new std::vector<EventBookkeeper*>; 
    m_childrenEB->reserve(rhs.m_childrenEB->size());
    for(unsigned int i=0; i<rhs.m_childrenEB->size(); i++){ 
      EventBookkeeper* child=new EventBookkeeper(*rhs.m_childrenEB->at(i));
      m_childrenEB->push_back(child); 
    }
    m_childrenIndices = new std::vector<unsigned int>;
    m_childrenIndices->reserve(rhs.m_childrenIndices->size());
    for(unsigned int i=0; i<rhs.m_childrenIndices->size(); i++){ 
      m_childrenIndices->push_back(rhs.m_childrenIndices->at(i));
    }
    m_declaredChildFilter=rhs.m_declaredChildFilter;
    m_declaredTopFilter=rhs.m_declaredTopFilter;
  }
  return *this;
}

void 
EventBookkeeper::SetDefaultDataMemberValues()
{
  m_name="";
  m_description="";
  m_inputstream="unkwn";
  m_outputstream="unkwn";
  m_logic="unkwn";
  m_nAcceptedEvents=0; 
  m_nWeightedAcceptedEvents=0.0; 
  m_cycle=-1;
  m_childrenEB = new std::vector<EventBookkeeper*>; 
  m_parentIndex=-1;
  m_childrenIndices = new std::vector<unsigned int>;
  m_declaredChildFilter=false;
  m_declaredTopFilter=false;
  return;
}

EventBookkeeper* 
EventBookkeeper::DeepCopyForFlatStructure(EventBookkeeperCollection* collFLAT)
{
  EventBookkeeper* book = new EventBookkeeper();
  book->m_name = m_name;
  book->m_description = m_description;
  book->m_inputstream = m_inputstream;
  book->m_outputstream = m_outputstream;
  book->m_logic = m_logic;
  book->m_nAcceptedEvents = m_nAcceptedEvents; 
  book->m_nWeightedAcceptedEvents = m_nWeightedAcceptedEvents; 
  book->m_cycle = m_cycle;
  book->m_declaredChildFilter = m_declaredChildFilter;
  book->m_declaredTopFilter = m_declaredTopFilter;

  //now deal here with the flat indices from collFLAT. Only the direct children are filled, NOT the whole family.
  for(unsigned int i=0; i<m_childrenEB->size(); i++){ 
    bool notFound=true;
    for(unsigned int j=0; (j<collFLAT->size() && notFound); j++){ 
      if(m_childrenEB->at(i)==collFLAT->at(j)){ 
        book->m_childrenIndices->push_back(j); 
        notFound=false;
      }
    }
  }
  //parentIndex is dealt in a separate function of EventBookkeeperCollection
  book->m_parentIndex=m_parentIndex;

  //the caller now owns book
  return book;
}

EventBookkeeper::~EventBookkeeper()
{
  //std::cout<<"Destructing "<<m_name<<std::endl;
  for(unsigned int i=0; i<m_childrenEB->size(); i++){ delete m_childrenEB->at(i); }
  m_childrenEB->clear(); 
  delete m_childrenEB;

  m_childrenIndices->clear();
  delete m_childrenIndices;
}


void
EventBookkeeper::setName(const std::string& name)
{ 
  m_name=name; 
}

void
EventBookkeeper::setDescription(const std::string& description)
{ 
  m_description=description;
}

void
EventBookkeeper::setInputStream(const std::string& inputstream)
{ 
  m_inputstream=inputstream;
}

void
EventBookkeeper::setOutputStream(const std::string& outputstream)
{ 
  m_outputstream=outputstream;
}

void
EventBookkeeper::setOutputStreamOfFamily(const std::string &outputstream)
{
  setOutputStream(outputstream);
  for(unsigned int i=0; i<m_childrenEB->size(); i++){
    m_childrenEB->at(i)->setOutputStreamOfFamily(outputstream);
  }
  return;
}

void
EventBookkeeper::setLogic(const std::string& logic)
{ 
  m_logic=logic;
}

void
EventBookkeeper::setNAcceptedEvents(uint64_t nEvents)
{ 
  m_nAcceptedEvents=nEvents;
}

void
EventBookkeeper::addNAcceptedEvents(uint64_t nEvents)
{ 
  m_nAcceptedEvents+=nEvents;
}

void
EventBookkeeper::setNWeightedAcceptedEvents(double nWeightedEvents)
{ 
  m_nWeightedAcceptedEvents=nWeightedEvents;
}

void
EventBookkeeper::addNWeightedAcceptedEvents(double nWeightedEvents)
{ 
  m_nWeightedAcceptedEvents+=nWeightedEvents;
}

void
EventBookkeeper::updateAcceptedEventsIncludingFamily(const EventBookkeeper* eb)
{
  addNAcceptedEvents( eb->getNAcceptedEvents() );
  addNWeightedAcceptedEvents( eb->getNWeightedAcceptedEvents() );

  std::vector<EventBookkeeper*>* eb_children = eb->getChildrenEventBookkeepers();
  for(unsigned int i=0; i<m_childrenEB->size(); i++){
    m_childrenEB->at(i)->updateAcceptedEventsIncludingFamily(eb_children->at(i));
  }
  return;
}

void
EventBookkeeper::setCycle(int cycle)
{ 
  m_cycle=cycle;
}

void
EventBookkeeper::setChildrenEventBookkeepers(std::vector<EventBookkeeper*>* childrenEB)
{ 
  m_childrenEB=childrenEB;
}

void
EventBookkeeper::AddChild(EventBookkeeper* eb)
{
  //synchronize contextual info with the parent
  eb->setInputStream( m_inputstream );
  eb->setCycle( m_cycle );
  eb->setOutputStream(m_outputstream);
  bool notFound=true;
  for(unsigned int i=0; (i<m_childrenEB->size()) && notFound; i++){ 
    if(m_childrenEB->at(i)==eb){
      notFound=false;
    } 
  }
  if(notFound){ m_childrenEB->push_back(eb); }
  //else{ ATH_MSG_WARNING("Two requests for the same child. Second one ignored."); }
  return;
}
void EventBookkeeper::AddChildren( std::vector<EventBookkeeper*>* children ){
  for(unsigned int i=0; i<children->size(); i++){
    AddChild( children->at(i) );
  }
  return;
}

EventBookkeeper* EventBookkeeper::AddNewChild(std::string name, std::string description){
  EventBookkeeper* eb = new EventBookkeeper(name,description,"Child");
  AddChild(eb);
  return eb;
}


bool EventBookkeeper::isEqualTo( const EventBookkeeper *eb ){
  //This function is mostly a placeholder in case we decide to complexify later.
  return (m_name==eb->getName() && m_cycle==eb->getCycle());
}

void
EventBookkeeper::PrintToMsg(MsgStream &log, const std::string& indent)
{
  if(log.level()==MSG::DEBUG){
    log<<MSG::DEBUG<<indent<<m_name<<" "<<m_inputstream<<" "<<m_outputstream<<" "<<m_logic<<" "<<m_cycle<<" ("<<m_childrenEB->size()<<" children) "<<": "<<m_description<<" ==> "<<m_nAcceptedEvents<<" "<<m_nWeightedAcceptedEvents<<endreq;
  }
  else{
    log<<MSG::INFO<<indent<<m_name<<": "<<m_description<<" ==> "<<m_nAcceptedEvents<<endreq;
  }
  
  return;
}

void 
EventBookkeeper::Print(const std::string& indent, std::ostream &out)
{
  out<<indent<<m_name<<" "<<m_inputstream<<" "<<m_outputstream<<" "<<m_logic<<" "<<m_cycle<<" ("<<m_childrenEB->size()<<" children) "<<": "<<m_description<<" ==> "<<m_nAcceptedEvents<<" "<<m_nWeightedAcceptedEvents<<std::endl;
  return;
}

void
EventBookkeeper::PrintFamily(const std::string& indent)
{
  Print(indent);
  std::string indent_ = indent + "     ";
  for(unsigned int i=0; i<m_childrenEB->size(); i++){
    m_childrenEB->at(i)->PrintFamily(indent_);
  }
  return;
}

void
EventBookkeeper::fillWithWholeFamily(EventBookkeeperCollection* family)
{

  //family does not include self
  for(unsigned int i=0; i<m_childrenEB->size(); i++){
    family->push_back(m_childrenEB->at(i));
    m_childrenEB->at(i)->fillWithWholeFamily(family);
  }
  
  return;
}
