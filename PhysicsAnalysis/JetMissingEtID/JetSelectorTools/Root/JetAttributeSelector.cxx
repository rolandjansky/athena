/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSelectorTools/JetAttributeSelector.h"
#include <limits>

namespace {
  
  template <class T>
  struct ValueRetriever : public JetAttributeSelector::SelValueRetriever {
    ValueRetriever(const std::string &n) : acc(n) {}
    float value(const xAOD::Jet& j) { return acc(j);}
    SG::AuxElement::Accessor<T> acc;
  };

  template <class T>
  struct VecValueRetriever : public JetAttributeSelector::SelValueRetriever {
    VecValueRetriever(const std::string &n, int ind) : acc(n), index(ind) {}
    float value(const xAOD::Jet& j) { return acc(j)[index];}
    SG::AuxElement::Accessor<std::vector<T> > acc;
    int index;
  };
  
}


JetAttributeSelector::JetAttributeSelector(const std::string &t) 
  : asg::AsgTool(t)
  , m_min(-std::numeric_limits<float>::max())
  , m_max( std::numeric_limits<float>::max())
  , m_attName("")
  , m_attType("float")
  , m_vectorAttIndex(0)
  , m_vretriever(NULL)
{

  declareProperty("CutMin", m_min );
  declareProperty("CutMax", m_max );
  declareProperty("Attribute",m_attName) ;
  declareProperty("AttributeType",m_attType) ;
  declareProperty("VectorIndex",m_vectorAttIndex) ;

}

JetAttributeSelector:: ~JetAttributeSelector(){
  if(m_vretriever) delete m_vretriever;
}


JetAttributeSelector::SelValueRetriever* JetAttributeSelector::buildValueRetriever(const std::string& type, const std::string& name, int index){
  SelValueRetriever* retriever = NULL;
  if(type=="float") retriever = new ::ValueRetriever<float>(name);
  else if(type=="int") retriever = new ::ValueRetriever<int>(name);
  else if(type=="vector<float>") retriever = new ::VecValueRetriever<float>(name, index);
  else if(type=="vector<int>") retriever = new ::VecValueRetriever<int>(name, index);
  else {
    ATH_MSG_ERROR("Unsupported attribute type : "<< type);
    // will return NULL
  }
  return retriever;
}


StatusCode JetAttributeSelector::initialize() {
  if(m_attName=="") {
    ATH_MSG_ERROR("Please specify an attribute name");
    return StatusCode::FAILURE;
  }
  
  m_vretriever = buildValueRetriever(m_attType, m_attName, m_vectorAttIndex);
  if(m_vretriever==NULL) {
    ATH_MSG_ERROR("Can't retrieve attribute type : "<< m_attType<< " name : "<< m_attName);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(" will select on attribute of type : "<< m_attType<< ".  "<<m_min << " < "<< m_attName << " < "<< m_max);
  return StatusCode::SUCCESS;
}

int JetAttributeSelector::keep(const xAOD::Jet& jet) const {
  float v = m_vretriever->value(jet);
  return (m_min <= v ) && (v<=m_max);
}


JetAbsAttributeSelector::JetAbsAttributeSelector(const std::string &t) : JetAttributeSelector(t) { }

int JetAbsAttributeSelector::keep(const xAOD::Jet& jet) const {
  float v = fabs(m_vretriever->value(jet));
  return (m_min <= v ) && (v<=m_max);  
}

JetAttributeRatioSelector::JetAttributeRatioSelector(const std::string &t) : JetAttributeSelector(t) 
                                                                           , m_attName2("")
                                                                           , m_attType2("float")
                                                                           , m_vectorAttIndex2(0)
                                                                           , m_vretriever2(NULL) {
  declareProperty("Attribute2",m_attName2) ;
  declareProperty("AttributeType2",m_attType2) ;
  declareProperty("VectorIndex2",m_vectorAttIndex2) ;

}
                                                                             

StatusCode JetAttributeRatioSelector::initialize() {
  ATH_CHECK( JetAttributeSelector::initialize() );

  m_vretriever2 = buildValueRetriever(m_attType2, m_attName2, m_vectorAttIndex2);
  if(m_vretriever2==NULL) {
    ATH_MSG_ERROR("Can't retrieve attribute type : "<< m_attType2<< " name : "<< m_attName2);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(" will select on RATIO of attribute of type : "<< m_attType<< " ,  "<< m_attName );
  ATH_MSG_INFO("                    over attribute of type : "<< m_attType2<<  " ,  "<< m_attName << " within  "<<m_min << " < r < "<< m_max);
  return StatusCode::SUCCESS;
  
}
     
int JetAttributeRatioSelector::keep(const xAOD::Jet& jet) const {
  float v1 = m_vretriever->value(jet);
  float v2 = m_vretriever2->value(jet);
  if( v2 == 0.) return false;
  double r = v1/v2;
  return (m_min <= r ) && (r<=m_max);
}
