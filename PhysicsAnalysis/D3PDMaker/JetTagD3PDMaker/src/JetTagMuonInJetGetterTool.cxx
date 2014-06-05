/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMuonInJetGetterTool.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

JetTagMuonInJetGetterTool::JetTagMuonInJetGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{

  declareProperty("JetCollections", m_jetCollections);
  declareProperty("MuonsName", m_muonsName="Muons");
  m_created=false;
  m_objects=0;
}

StatusCode JetTagMuonInJetGetterTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}

const std::type_info& JetTagMuonInJetGetterTool::typeinfo() const
{
  return typeid(std::vector<const JetCollection*>);
}


const std::type_info& JetTagMuonInJetGetterTool::elementTypeinfo() const
{
  return typeid(Analysis::Muon);
}

std::vector<const JetCollection*>* JetTagMuonInJetGetterTool::createObject(bool allowMissing){ 
  /// create object once in case get is called more than once

  m_created=true;

  if(m_objects){
    ATH_MSG_WARNING("objects not properly deleted - check code");
    delete m_objects;
  }

  m_objects=0;


  if(!allowMissing && m_jetCollections.size() == 0){
    ATH_MSG_WARNING("no jet collections provided");
    return 0;
  }

  std::vector<const JetCollection*>* colls = new std::vector<const JetCollection*>();

  for(unsigned int i=0; i<m_jetCollections.size(); ++i){

    const JetCollection* coll=0;
    if(evtStore()->retrieve(coll, m_jetCollections[i]).isFailure()){
      if(!allowMissing){
	ATH_MSG_WARNING("jet collection " << m_jetCollections[i] << " not found");
	delete colls;
	return 0;
      }
    }
    else{
      colls->push_back(coll);
    }
  }

  m_objects = colls;
  return m_objects;

}

const std::vector<const JetCollection*>* JetTagMuonInJetGetterTool::get (bool allowMissing){

  /// create once in case we need to get it in several methods
  if(m_created) return m_objects;
  return createObject(allowMissing);
  

}

void JetTagMuonInJetGetterTool::releaseContainers (){

  m_created=false;
  m_inserted.clear();

  if(m_objects) delete m_objects;
  m_objects=0;

}


size_t JetTagMuonInJetGetterTool::sizeHint(bool )
{

  // to be obtimized
  return 3;  

}

StatusCode JetTagMuonInJetGetterTool::reset(bool allowMissing)
{

  const std::vector<const JetCollection*>* allcoll = get(allowMissing);

  if (!allcoll) {
    m_colItr=m_colEnd;
    m_jetItr=m_jetEnd;
    m_muItr=m_muEnd;
    if(!allowMissing){
      ATH_MSG_FATAL("no jet collections");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  m_colItr = allcoll->begin();
  m_colEnd = allcoll->end();

  if(m_colItr == m_colEnd){
    m_jetItr=m_jetEnd;
    m_muItr=m_muEnd;
    return StatusCode::SUCCESS;
  }

  m_jetItr = (*m_colItr)->begin();
  m_jetEnd = (*m_colItr)->end();

  if(m_jetItr == m_jetEnd){
    m_muItr=m_muEnd;
    return StatusCode::SUCCESS;
  }

  const Analysis::MuonAssociation* ma = (*m_jetItr)->getAssociation<Analysis::MuonAssociation>(m_muonsName);

  if(!ma){
    m_muItr=m_muEnd;
    ATH_MSG_WARNING("no MuonAssociation " <<m_muonsName << " in jets");
    return StatusCode::SUCCESS;
  }

  m_muItr = ma->begin();
  m_muEnd = ma->end();

  return StatusCode::SUCCESS;
}



const void* JetTagMuonInJetGetterTool::nextUntyped()
{

  if(m_muItr == m_muEnd){
    if(m_jetItr == m_jetEnd){
      if(m_colItr == m_colEnd){
	releaseContainers();
	return 0;
      }/// last collection we are done
      ++m_colItr;
      if(m_colItr == m_colEnd){
	releaseContainers();
	return 0;
      }
      m_jetItr = (*m_colItr)->begin();
      m_jetEnd = (*m_colItr)->end();
      if(m_jetItr == m_jetEnd)return nextUntyped();
      const Analysis::MuonAssociation* ma = (*m_jetItr)->getAssociation<Analysis::MuonAssociation>(m_muonsName);
      if(!ma){
	m_muItr=m_muEnd;
	ATH_MSG_WARNING("no MuonAssociation in jets");
	return nextUntyped();
      }
      m_muItr = ma->begin();
      m_muEnd = ma->end();
      return nextUntyped();

    }/// last jet in collection
    ++m_jetItr;
    if(m_jetItr == m_jetEnd)return nextUntyped();
    const Analysis::MuonAssociation* ma = (*m_jetItr)->getAssociation<Analysis::MuonAssociation>(m_muonsName);
    if(!ma){
      m_muItr=m_muEnd;
      ATH_MSG_WARNING("no MuonAssociation in jets");
      return nextUntyped();
    }
    m_muItr = ma->begin();
    m_muEnd = ma->end();
    if(m_muItr == m_muEnd) return nextUntyped();
  } // last muon in jet


  const Analysis::Muon* mu = (*m_muItr);

  ++m_muItr;

  if(!mu){
    ATH_MSG_WARNING("iterator to a null pointer, muon collection mismatch");
    return nextUntyped();
  }
  if(!selectMuon(mu))return nextUntyped();

  return mu;
}


bool JetTagMuonInJetGetterTool::selectMuon(const Analysis::Muon* mu){

  if( m_inserted.insert(mu).second ) return true;
  return false;

}

}
