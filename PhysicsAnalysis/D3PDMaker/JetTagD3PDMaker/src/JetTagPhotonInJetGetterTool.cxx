/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagPhotonInJetGetterTool.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

JetTagPhotonInJetGetterTool::JetTagPhotonInJetGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent)
{

  declareProperty("JetCollections", m_jetCollections);
  m_created=false;
  m_objects=0;
}

StatusCode JetTagPhotonInJetGetterTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}

const std::type_info& JetTagPhotonInJetGetterTool::typeinfo() const
{
  return typeid(std::vector<const JetCollection*>);
}


const std::type_info& JetTagPhotonInJetGetterTool::elementTypeinfo() const
{
  return typeid(Analysis::Photon);
}

std::vector<const JetCollection*>* JetTagPhotonInJetGetterTool::createObject(bool allowMissing){ 
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

const std::vector<const JetCollection*>* JetTagPhotonInJetGetterTool::get (bool allowMissing){


  if(m_created) return m_objects;
  return createObject(allowMissing);
  

}

 void JetTagPhotonInJetGetterTool::releaseContainers (){

  m_created=false;
  m_inserted.clear();
  if(m_objects) delete m_objects;
  m_objects=0;

}


size_t JetTagPhotonInJetGetterTool::sizeHint(bool )
{

  // to be obtimized
  return 3;  

}

StatusCode JetTagPhotonInJetGetterTool::reset(bool allowMissing)
{

  const std::vector<const JetCollection*>* allcoll = get(allowMissing);

  if (!allcoll) {
    m_colItr=m_colEnd;
    m_jetItr=m_jetEnd;
    m_phItr=m_phEnd;
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
    m_phItr=m_phEnd;
    return StatusCode::SUCCESS;
  }

  m_jetItr = (*m_colItr)->begin();
  m_jetEnd = (*m_colItr)->end();

  if(m_jetItr == m_jetEnd){
    m_phItr=m_phEnd;
    return StatusCode::SUCCESS;
  }

  const Analysis::PhotonAssociation* pa = (*m_jetItr)->getAssociation<Analysis::PhotonAssociation>("Photons");

  if(!pa){
    m_phItr=m_phEnd;
    ATH_MSG_WARNING("no PhotonAssociation in jets");
    return StatusCode::SUCCESS;
  }

  m_phItr = pa->begin();
  m_phEnd = pa->end();

  return StatusCode::SUCCESS;
}



const void* JetTagPhotonInJetGetterTool::nextUntyped()
{


  if(m_phItr == m_phEnd){
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
      const Analysis::PhotonAssociation* pa = (*m_jetItr)->getAssociation<Analysis::PhotonAssociation>("Photons");
      if(!pa){
	m_phItr=m_phEnd;
	ATH_MSG_WARNING("no PhotonAssociation in jets");
	return nextUntyped();
      }       
      m_phItr = pa->begin();
      m_phEnd = pa->end();
      return nextUntyped();

    }/// last jet in collection
    ++m_jetItr;
    if(m_jetItr == m_jetEnd)return nextUntyped();
    const Analysis::PhotonAssociation* pa = (*m_jetItr)->getAssociation<Analysis::PhotonAssociation>("Photons");
    if(!pa){
      m_phItr=m_phEnd;
      ATH_MSG_WARNING("no PhotonAssociation in jets");
      return nextUntyped();
    }
    m_phItr = pa->begin();
    m_phEnd = pa->end();

    if(m_phItr == m_phEnd) return nextUntyped();
  } // last phtron in jet

  const Analysis::Photon* ph = (*m_phItr);

  ++m_phItr;

  if(!ph){
    ATH_MSG_WARNING("iterator to a null pointer, photon collection mismatch");
    return nextUntyped();
  }

  if(!selectPhoton(ph))return nextUntyped();
  return ph;
}


bool JetTagPhotonInJetGetterTool::selectPhoton(const Analysis::Photon* ph){

  if( m_inserted.insert(ph).second ) return true;
  return false;

}

}
