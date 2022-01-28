/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfData/Logic.h"

#include <iostream>
#include <regex>

#include "boost/lexical_cast.hpp"

const std::regex re( "([-\\w.]+)(?:\\[x(\\d+)\\])?$" );

TrigConf::Logic::Logic(NodeType nodeType) :
   m_nodeType(nodeType)
{}

TrigConf::Logic::~Logic()
{}

void
TrigConf::Logic::setNegate(bool negate) {
   m_negate = negate;
}

bool
TrigConf::Logic::negate() const {
   return m_negate;
}

void
TrigConf::Logic::setExpression(const std::string & expr) {
   m_expression = expr;
}

const std::string &
TrigConf::Logic::expression() const {
   return m_expression;
}

void
TrigConf::Logic::print(std::ostream & o) const {
   if(m_expression.size()>0) {
      o << m_expression << std::endl;
      o << std::string(m_expression.size(), '-') << std::endl;
   }
   print(o, 4, 0);
}


TrigConf::LogicLeaf::LogicLeaf(const std::string & content) :
   Logic(Logic::LEAF)
{
   setContent(content);
}


void
TrigConf::LogicLeaf::setContent(const std::string & content) {
   m_content = content;
   std::smatch sm;
   std::regex_match(content, sm, re);
   m_name = sm[1];
   m_count = sm[2].length()>0 ? boost::lexical_cast<unsigned int,std::string>(sm[2]) : 1;
}


bool
TrigConf::LogicLeaf::evaluate(const std::map<std::string,bool> & elementsState) const {
   auto stateForThisLeaf = elementsState.find(m_content);
   if( stateForThisLeaf == elementsState.end() ) {
      throw LogicParsingException(std::string("Can't evaluate logic ") + m_content + " as it is missing in the state map");
   }
   return negate() ? !stateForThisLeaf->second : stateForThisLeaf->second;
}


bool
TrigConf::LogicLeaf::evaluate(const std::map<std::string, unsigned int> & elementsCount) const {
   auto stateForThisLeaf = elementsCount.find(m_name);
   if( stateForThisLeaf == elementsCount.end() ) {
      throw LogicParsingException(std::string("Can't evaluate logic ") + m_content + " as " + m_name + " is missing in the counts map");
   }
   bool pass = stateForThisLeaf->second >= m_count;
   return negate() ? !pass : pass;
}

std::map<std::string, bool>
TrigConf::LogicLeaf::elements() const {
   return {{m_name, true}};
}


std::map<std::string, unsigned int>
TrigConf::LogicLeaf::elementsCount() const {
   return {{m_name, m_count}};
}


void
TrigConf::LogicLeaf::print(std::ostream & o, size_t indSize, size_t indLevel) const {
   o << std::string(indSize*indLevel, ' ');
   if( negate() ) { o << "NOT "; }
   o << m_content << std::endl;
}


TrigConf::LogicOPS::LogicOPS(NodeType nodeType) :
   Logic(nodeType)
{}

TrigConf::LogicOPS::LogicOPS(NodeType nodeType, std::unique_ptr<Logic> && left) :
   Logic(nodeType)
{
   m_subs.push_back(std::move(left));
}

void
TrigConf::LogicOPS::addSubLogic(std::unique_ptr<Logic>&& right)
{
   m_subs.push_back(std::move(right));
}

const std::vector<std::unique_ptr<TrigConf::Logic>> &
TrigConf::LogicOPS::subLogics() const {
   return m_subs;
}

std::vector<std::unique_ptr<TrigConf::Logic>>
TrigConf::LogicOPS::takeSubLogics() {
   auto tmp = std::move(m_subs);
   m_subs.clear();
   return tmp;
}

void
TrigConf::LogicOPS::print(std::ostream & o, size_t indSize, size_t indLevel) const {
   o << std::string(indSize*indLevel, ' ');
   if( negate() ) { o << "NOT "; }
   o << (nodeType()==Logic::AND ? "AND (" : "OR (") << std::endl;
   for( auto & subLogic : m_subs ) {
      subLogic->print(o,indSize, indLevel+1);
   }
   o << std::string(indSize*indLevel, ' ') << ")" << std::endl;
}




TrigConf::LogicAND::LogicAND() :
   LogicOPS(Logic::AND)
{}

TrigConf::LogicAND::LogicAND(std::unique_ptr<Logic>&& left) :
   LogicOPS(Logic::AND, std::move(left))
{}

bool
TrigConf::LogicAND::evaluate(const std::map<std::string,bool> & elementsState) const {
   for( auto & subLogic : subLogics() ) {
      if(! subLogic->evaluate(elementsState) ) { // if one evaluates to false, we can stop
         return negate() ? true : false;
      }
   }
   return negate() ? false : true;
}

bool
TrigConf::LogicAND::evaluate(const std::map<std::string, unsigned int> & elementsCount) const {
   for( auto & subLogic : subLogics() ) {
      if(! subLogic->evaluate(elementsCount) ) { // if one evaluates to false, we can stop
         return negate() ? true : false;
      }
   }
   return negate() ? false : true;
}

std::map<std::string, bool>
TrigConf::LogicAND::elements() const {
   std::map<std::string, bool> elements;
   for( auto & subLogic : subLogics() ) {
      for ( const auto& el : subLogic->elements() ) {
         elements.insert(el);
      }
   }
   return elements;
}

std::map<std::string, unsigned int>
TrigConf::LogicAND::elementsCount() const {
   std::map<std::string, unsigned int> elementsCount;
   for( auto & subLogic : subLogics() ) {
      for ( const auto& el : subLogic->elementsCount() ) {
         elementsCount.insert(el);
      }
   }
   return elementsCount;
}

TrigConf::LogicOR::LogicOR() :
   LogicOPS(Logic::OR)
{}

TrigConf::LogicOR::LogicOR(std::unique_ptr<Logic>&& left) :
   LogicOPS(Logic::OR, std::move(left))
{}

bool
TrigConf::LogicOR::evaluate(const std::map<std::string,bool> & elementsState) const {
   for( auto & subLogic : subLogics() ) {
      if( subLogic->evaluate(elementsState) ) { // if one evaluates to true, we can stop
         return negate() ? false : true;
      }
   }
   return negate() ? true : false;
}

bool
TrigConf::LogicOR::evaluate(const std::map<std::string, unsigned int> & elementsCount) const {
   for( auto & subLogic : subLogics() ) {
      if( subLogic->evaluate(elementsCount) ) { // if one evaluates to true, we can stop
         return negate() ? false : true;
      }
   }
   return negate() ? true : false;
}

std::map<std::string, bool>
TrigConf::LogicOR::elements() const {
   std::map<std::string, bool> elements;
   for( auto & subLogic : subLogics() ) {
      for ( const auto& el : subLogic->elements() ) {
         elements.insert(el);
      }
   }
   return elements;
}

std::map<std::string, unsigned int>
TrigConf::LogicOR::elementsCount() const {
   std::map<std::string, unsigned int> elementsCount;
   for( auto & subLogic : subLogics() ) {
      for ( const auto& el : subLogic->elementsCount() ) {
         elementsCount.insert(el);
      }
   }
   return elementsCount;
}
