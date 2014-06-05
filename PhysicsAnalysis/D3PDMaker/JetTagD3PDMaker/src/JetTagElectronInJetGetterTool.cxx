/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagElectronInJetGetterTool.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

JetTagElectronInJetGetterTool::JetTagElectronInJetGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{

  declareProperty("JetCollections", m_jetCollections);
  m_created=false;
  m_objects=0;
}

StatusCode JetTagElectronInJetGetterTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}

const std::type_info& JetTagElectronInJetGetterTool::typeinfo() const
{
  return typeid(std::vector<const JetCollection*>);
}


const std::type_info& JetTagElectronInJetGetterTool::elementTypeinfo() const
{
  return typeid(Analysis::Electron);
}

std::vector<const JetCollection*>* JetTagElectronInJetGetterTool::createObject(bool allowMissing){ 
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
      if(!coll){
	ATH_MSG_WARNING("jet collection " << m_jetCollections[i] << " retrieved but pointer 0");
      }
      else{
	colls->push_back(coll);
      }
    }
  }

  m_objects = colls;
  return m_objects;

}

const std::vector<const JetCollection*>* JetTagElectronInJetGetterTool::get (bool allowMissing){


  if(m_created) return m_objects;
  return createObject(allowMissing);
  

}

 void JetTagElectronInJetGetterTool::releaseContainers (){

  m_created=false;
  m_inserted.clear();
  if(m_objects) delete m_objects;
  m_objects=0;

}


size_t JetTagElectronInJetGetterTool::sizeHint(bool )
{

  // to be obtimized
  return 3;  

}

StatusCode JetTagElectronInJetGetterTool::reset(bool allowMissing)
{

  const std::vector<const JetCollection*>* allcoll = get(allowMissing);

  if (!allcoll) {
    m_colItr=m_colEnd;
    m_jetItr=m_jetEnd;
    m_elecItr=m_elecEnd;
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
    m_elecItr=m_elecEnd;
    return StatusCode::SUCCESS;
  }

  m_jetItr = (*m_colItr)->begin();
  m_jetEnd = (*m_colItr)->end();

  if(m_jetItr == m_jetEnd){
    m_elecItr=m_elecEnd;
    return StatusCode::SUCCESS;
  }

  const Analysis::ElectronAssociation* ea = (*m_jetItr)->getAssociation<Analysis::ElectronAssociation>("Electrons");

  if(!ea){
    m_elecItr=m_elecEnd;
    ATH_MSG_WARNING("no ElectronAssociation in jets");
    return StatusCode::SUCCESS;
  }

  m_elecItr = ea->begin();
  m_elecEnd = ea->end();

  return StatusCode::SUCCESS;
}



const void* JetTagElectronInJetGetterTool::nextUntyped()
{


  if(m_elecItr == m_elecEnd){
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
      const Analysis::ElectronAssociation* ea = (*m_jetItr)->getAssociation<Analysis::ElectronAssociation>("Electrons");
      if(!ea){
	m_elecItr=m_elecEnd;
	ATH_MSG_WARNING("no ElectronAssociation in jets");
	return nextUntyped();
      }       
      m_elecItr = ea->begin();
      m_elecEnd = ea->end();
      return nextUntyped();

    }/// last jet in collection
    ++m_jetItr;
    if(m_jetItr == m_jetEnd)return nextUntyped();
    const Analysis::ElectronAssociation* ea = (*m_jetItr)->getAssociation<Analysis::ElectronAssociation>("Electrons");
    if(!ea){
      m_elecItr=m_elecEnd;
      ATH_MSG_WARNING("no ElectronAssociation in jets");
      return nextUntyped();
    }
    m_elecItr = ea->begin();
    m_elecEnd = ea->end();

    if(m_elecItr == m_elecEnd) return nextUntyped();
  } // last electron in jet

  const Analysis::Electron* elec = (*m_elecItr);

  ++m_elecItr;

  if(!elec){
    ATH_MSG_WARNING("iterator to a null pointer, electron collection mismatch");
    return nextUntyped();
  }

  if(!selectElectron(elec))return nextUntyped();
  return elec;
}


bool JetTagElectronInJetGetterTool::selectElectron(const Analysis::Electron* elec){

  if( m_inserted.insert(elec).second ) return true;
  return false;

}

}
