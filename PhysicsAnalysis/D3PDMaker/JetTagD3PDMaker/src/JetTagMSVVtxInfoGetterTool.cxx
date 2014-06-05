/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMSVVtxInfoGetterTool.h"
#include "JetTagInfo/MultiSVInfoPlus.h"

#include "AthenaKernel/errorcheck.h"

namespace D3PD {

JetTagMSVVtxInfoGetterTool::JetTagMSVVtxInfoGetterTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : Base (type, name, parent),
      m_finder (*this, "MultiSVInfoPlus", true),
      m_iMSVVtx(0),
      m_eMSVVtx(0)
{
  declareProperty("JetCollections", m_jetCollections);
  m_created=false;
  m_objects=0;
}

StatusCode JetTagMSVVtxInfoGetterTool::initialize()
{
  CHECK( Base::initialize() );
  return StatusCode::SUCCESS;
}

const std::type_info& JetTagMSVVtxInfoGetterTool::typeinfo() const
{
  return typeid(std::vector<const JetCollection*>);
}


const std::type_info& JetTagMSVVtxInfoGetterTool::elementTypeinfo() const
{
  return typeid(Analysis::MSVVtxInfo);
}

std::vector<const JetCollection*>* JetTagMSVVtxInfoGetterTool::createObject(bool allowMissing){ 
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

const std::vector<const JetCollection*>* JetTagMSVVtxInfoGetterTool::get (bool allowMissing){


  if(m_created) return m_objects;
  return createObject(allowMissing);
  

}

 void JetTagMSVVtxInfoGetterTool::releaseContainers (){

  m_created=false;
  if(m_objects) delete m_objects;
  m_objects=0;

}


size_t JetTagMSVVtxInfoGetterTool::sizeHint(bool )
{

  // to be obtimized
  if(m_objects){
    return m_objects->size();
  }
  return 3;

}

StatusCode JetTagMSVVtxInfoGetterTool::reset(bool allowMissing)
{

  const std::vector<const JetCollection*>* allcoll = get(allowMissing);

  if (!allcoll) {
    m_colItr=m_colEnd;
    m_jetItr=m_jetEnd;
    m_iMSVVtx = 0;
    m_eMSVVtx = 0;
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
    m_iMSVVtx = 0;
    m_eMSVVtx = 0;
    return StatusCode::SUCCESS;
  }

  m_jetItr = (*m_colItr)->begin();
  m_jetEnd = (*m_colItr)->end();

  if(m_jetItr == m_jetEnd){
    m_iMSVVtx = 0;
    m_eMSVVtx = 0;
    return StatusCode::SUCCESS;
  }

  const Analysis::MultiSVInfoPlus* msvip = getMultiSVInfoPlus(*m_jetItr);

  if(!msvip){
    m_iMSVVtx = 0;
    m_eMSVVtx = 0;
    return StatusCode::SUCCESS;
  }

  m_iMSVVtx = 0;
  m_eMSVVtx = msvip->numVtxInfo();


  return StatusCode::SUCCESS;
}



const void* JetTagMSVVtxInfoGetterTool::nextUntyped()
{


  if(m_iMSVVtx == m_eMSVVtx){
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
      const Analysis::MultiSVInfoPlus* msvip = getMultiSVInfoPlus(*m_jetItr);
      if(!msvip){
	m_iMSVVtx = 0;
	m_eMSVVtx = 0;
	return nextUntyped();
      }

      m_iMSVVtx = 0;
      m_eMSVVtx = msvip->numVtxInfo();
      return nextUntyped();
    }
    ++m_jetItr;
    if(m_jetItr == m_jetEnd)return nextUntyped();
    const Analysis::MultiSVInfoPlus* msvip = getMultiSVInfoPlus(*m_jetItr);
    if(!msvip){
      m_iMSVVtx = 0;
      m_eMSVVtx = 0;
      return nextUntyped();
    }

    m_iMSVVtx = 0;
    m_eMSVVtx = msvip->numVtxInfo();

    if(m_iMSVVtx == m_eMSVVtx) return nextUntyped();

  }

  const Analysis::MultiSVInfoPlus* msvip = getMultiSVInfoPlus(*m_jetItr);
  const Analysis::MSVVtxInfo* info = msvip->getVtxInfo(m_iMSVVtx);

  ++m_iMSVVtx; 

  if(!info){
    return nextUntyped();
  }       
    
  return info;
}

const Analysis::MultiSVInfoPlus*
JetTagMSVVtxInfoGetterTool::getMultiSVInfoPlus(const Jet* jet)
{
  return m_finder.getTagInfo<Analysis::MultiSVInfoPlus> (*jet);
}


}
