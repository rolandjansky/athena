/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonNSWAsBuilt/Element.h"
#include <list>

using namespace NswAsBuilt;

//===============================================================================
Element::Element(std::unique_ptr<ElementModel> deformationModel)
: m_deformationModel(std::move(deformationModel))
,  m_nominalPar(m_deformationModel->nParameters())
,  m_correctedPar(m_deformationModel->nParameters())
{ }


//===============================================================================
Element::ParameterVector& Element::parameterVector(ParameterClass iclass) 
{
  return (iclass == ParameterClass::CORRECTION) ? m_correctedPar : m_nominalPar;
}


//===============================================================================
const Element::ParameterVector& Element::parameterVector(ParameterClass iclass) const 
{
  return (iclass == ParameterClass::CORRECTION) ? m_correctedPar : m_nominalPar;
}


//===============================================================================
void Element::transform(ParameterClass iclass, VectorSetRef local) const 
{
  m_deformationModel->transform(parameterVector(iclass), local);
}


//===============================================================================
void Element::transformToFrame(ParameterClass iclass, VectorSetRef local, Element* frame) const 
{
  const Element* el = this;
  while ((el!=nullptr) && (el!=frame)) {
    el->transform(iclass, local);
    el = el->m_mother;
  }
}


//===============================================================================
void Element::cacheTransforms() 
{
  m_deformationModel->cacheTransform(m_correctedPar);
  m_deformationModel->cacheTransform(m_nominalPar);
}


//===============================================================================
double Element::getParameter(ParameterClass iclass, ipar_t ipar) const 
{
  const auto& parvec = parameterVector(iclass).parameters;
  if (ipar>=parvec.size()) {
    throw std::runtime_error("Element::getParameter: Parameter exceeds parvec size "+std::to_string(ipar));
  }
  return parvec[ipar];
}


//===============================================================================
void Element::setParameter(ParameterClass iclass, ipar_t ipar, double value) 
{
  auto& parvec = parameterVector(iclass).parameters;
  if (ipar>=parvec.size()) {
    throw std::runtime_error("Element::setParameter: Parameter exceeds parvec size "+std::to_string(ipar));
  }
  parvec[ipar] = value;
  parameterVector(iclass).transformCacheValid = false;
}


//===============================================================================
void Element::setParametersFromMap(ParameterClass iclass, const std::map<std::string, double>& values) 
{
  for (auto p : values) {
    setParameter(iclass, m_deformationModel->getParameterIndex(p.first), p.second);
  }
  m_deformationModel->cacheTransform(parameterVector(iclass));
}


//===============================================================================
void Element::addDaughter(std::unique_ptr<Element> daughter) 
{
  if (daughter->m_mother) {
    throw std::runtime_error("Element has a defined mother already");
  }
  daughter->m_mother = this;
  m_daughters.push_back(std::move(daughter));
}


//===============================================================================
void Element::traverseTree(std::function<void(Element&)> callback) 
{
  struct tree_t {
    daughterVec_t::iterator it;
    daughterVec_t::iterator end;
  };
  std::list<tree_t> dtree;
  callback(*this);
  dtree.push_back({m_daughters.begin(), m_daughters.end()});
  while (!dtree.empty()) {
    auto& it = dtree.back().it;
    if (it != dtree.back().end) {
      Element& el = **it;
      callback(el);
      dtree.push_back({el.m_daughters.begin(), el.m_daughters.end()});
      ++it;
    } else {
      dtree.pop_back();
    }
  }
}

