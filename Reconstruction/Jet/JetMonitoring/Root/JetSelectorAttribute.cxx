/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetMonitoring/JetSelectorAttribute.h"
#include <limits>

namespace {
  
  template <class T>
  struct ValueRetriever : public JetSelectorAttribute::SelValueRetriever {
    ValueRetriever(const std::string &n) : acc(n) {}
    float value(const xAOD::Jet& j) { return acc(j);}
    SG::AuxElement::Accessor<T> acc;
  };

  template <class T>
  struct VecValueRetriever : public JetSelectorAttribute::SelValueRetriever {
    VecValueRetriever(const std::string &n, int ind) : acc(n), index(ind) {}
    float value(const xAOD::Jet& j) { return acc(j)[index];}
    SG::AuxElement::Accessor<std::vector<T> > acc;
    int index;
  };

  
}

JetSelectorAttribute::JetSelectorAttribute(const std::string &t) 
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

JetSelectorAttribute:: ~JetSelectorAttribute(){
  if(m_vretriever) delete m_vretriever;
}

StatusCode JetSelectorAttribute::initialize() {
  if(m_attName=="") {
    ATH_MSG_ERROR("Please specify an attribute name");
    return StatusCode::FAILURE;
  }

  if(m_attType=="float") m_vretriever = new ::ValueRetriever<float>(m_attName);
  else if(m_attType=="int") m_vretriever = new ::ValueRetriever<int>(m_attName);
  else if(m_attType=="vector<float>") m_vretriever = new ::VecValueRetriever<float>(m_attName, m_vectorAttIndex);
  else {
    ATH_MSG_ERROR("Unsupported attribute type : "<< m_attType);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(" will select on attribute of type : "<< m_attType<< ".  "<<m_min << " < "<< m_attName << " < "<< m_max);
  return StatusCode::SUCCESS;
}

int JetSelectorAttribute::keep(const xAOD::Jet& jet) const {
  float v = m_vretriever->value(jet);
  return (m_min < v ) && (v<m_max);
}
