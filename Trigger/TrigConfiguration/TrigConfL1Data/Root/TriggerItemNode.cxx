/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/TriggerItemNode.h"
#include "TrigConfL1Data/TriggerThreshold.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <cstdlib>
#include <inttypes.h>
#include <stdexcept>
#include <sys/types.h>

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace TrigConf;


TrigConf::TriggerItemNode::TriggerItemNode(NodeType type) :
   m_NodeType(type), 
   m_ThresholdName(""),
   m_Position(0), 
   m_Multiplicity(0),
   m_Threshold(0), 
   m_InternalTrigger(InternalType_t(L1DataDef::UNDEF,0))
{}


TrigConf::TriggerItemNode::~TriggerItemNode() {
   for(TriggerItemNode * node : m_Children)
      delete node;
}

TrigConf::TriggerItemNode::NodeType
TrigConf::TriggerItemNode::typeFromString(const std::string& type) {
   if(type=="AND") return TriggerItemNode::AND;
   if(type=="OR")  return TriggerItemNode::OR;
   if(type=="NOT") return TriggerItemNode::NOT;
   if(type=="OBJ") return TriggerItemNode::OBJ;
   if(type=="TriggerCondition") return TriggerItemNode::OBJ;
   if(type=="InternalTrigger") return TriggerItemNode::OBJ;
   return TriggerItemNode::UNDEF;
}

TriggerItemNode::NodeType
TriggerItemNode::typeFromChar(const char& c) {
   if(c=='&') return TriggerItemNode::AND;
   if(c=='|') return TriggerItemNode::OR;
   if(c=='!') return TriggerItemNode::NOT;
   return TriggerItemNode::UNDEF;
}

std::string
TrigConf::TriggerItemNode::typeAsString(NodeType type) {
   static const std::string typelabel[] = {"OBJ", "NOT", "AND", "OR", "UNDEF"};
   return typelabel[type];
}


bool
TrigConf::TriggerItemNode::isThreshold() const {
   return m_InternalTrigger.first == L1DataDef::UNDEF;
}

bool
TrigConf::TriggerItemNode::isInternalTrigger() const {
   return m_InternalTrigger.first != L1DataDef::UNDEF;
}

void
TrigConf::TriggerItemNode::setTriggerThreshold(TriggerThreshold* thr ) {
   if(thr->isInternal()) {
      // from the database internal triggers are also loaded as TriggerThresholds
      setInternalTrigger(thr->name());
   } else {
      m_InternalTrigger.first = L1DataDef::UNDEF;
      m_InternalTrigger.second = 0;
   }
   m_Threshold = thr;
   m_ThresholdName = thr->name();
}

void
TrigConf::TriggerItemNode::setInternalTrigger(L1DataDef::TriggerType x,
                                              unsigned int thresholdNumber) {
   m_InternalTrigger.first = x;
   m_InternalTrigger.second = thresholdNumber;
   m_ThresholdName = L1DataDef::typeConfig(x).name + boost::lexical_cast<string,unsigned int>(thresholdNumber);
   m_Threshold = 0;
}

void
TrigConf::TriggerItemNode::setInternalTrigger(const string& name) {

   string::size_type pos = name.find_first_of("0123456789");

   L1DataDef::TriggerType tt = L1DataDef::stringAsType(name.substr(0,pos));

   if( ! L1DataDef::typeConfig(tt).internal ) {
      cerr << "TriggerItemNode::setInternalTrigger: type " << name << " is not an internal trigger" << endl;
      throw runtime_error("TriggerItemNode::setInternalTrigger: type is not an internal trigger");
   }
   m_InternalTrigger.first = tt;
   m_InternalTrigger.second = boost::lexical_cast<unsigned int,string>(name.substr(pos));
   m_ThresholdName = name;
   m_Threshold = 0;
}


void
TrigConf::TriggerItemNode::addChild(TriggerItemNode* node) {
   if(type() == OBJ)
      throw std::runtime_error("TriggerItemNode::addChild: trying to add child to leaf node");
   m_Children.push_back(node);
}

void
TrigConf::TriggerItemNode::getAllFinalNodes(std::vector<const TriggerItemNode*>& vec) const {
   if(type() == OBJ) {
      vec.push_back(this);
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getAllFinalNodes(vec);
   }
}

void
TrigConf::TriggerItemNode::getAllThresholds(std::vector<TriggerThreshold*>& vec) const {
   if(type() == OBJ) {
      if(!isInternalTrigger()) {
         if(m_Threshold) vec.push_back(m_Threshold);
      }
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getAllThresholds(vec);
   }
}

// returns a bit-pattern for the bunchgroups
void
TrigConf::TriggerItemNode::getAllBunchGroups(std::vector<bool>& vec) const {
   unsigned int max_bgrp = L1DataDef::typeConfig(L1DataDef::BGRP).max;
   if(vec.size() < max_bgrp) vec.resize( max_bgrp );
   if(type() == OBJ) {
      if(internalTriggerType() == L1DataDef::BGRP)
         vec[m_InternalTrigger.second] = true;
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getAllBunchGroups(vec);
   }
}

// returns a bit-pattern for the just as the previous one but as uint16
void
TrigConf::TriggerItemNode::getBunchGroupsMask(uint16_t & bgmask) const {
   if(type() == OBJ) {
      if(internalTriggerType() == L1DataDef::BGRP) {
         uint16_t mask(0x1);
         bgmask |= (mask << m_InternalTrigger.second);
      }
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getBunchGroupsMask(bgmask);
   }
}

void
TrigConf::TriggerItemNode::getAllBunchGroups(std::vector<unsigned int>& vec) const {
   if(type() == OBJ) {
      if(internalTriggerType() == L1DataDef::BGRP)
         vec.push_back(m_InternalTrigger.second);
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getAllBunchGroups(vec);
   }
}

void
TrigConf::TriggerItemNode::getAllRandomTriggers(std::vector<unsigned int>& vec) const {
   if(type() == OBJ) {
      if(internalTriggerType() == L1DataDef::RNDM)
         vec.push_back(m_InternalTrigger.second);
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getAllRandomTriggers(vec);
   }
}

void
TrigConf::TriggerItemNode::getAllPrescaledClockTriggers(std::vector<unsigned int>& vec) const {
   if(type() == OBJ) {
      if(internalTriggerType() == L1DataDef::PCLK)
         vec.push_back(m_InternalTrigger.second);
   } else {
      for(TriggerItemNode * node : m_Children)
         node->getAllPrescaledClockTriggers(vec);
   }
   return;
}

void
TrigConf::TriggerItemNode::writeXML(std::ostream & xmlfile, int indentLevel, int indentWidth, bool /*omitDelimiter*/) const {
   if (m_NodeType == OBJ) {

      if(isInternalTrigger()) {
         indent(xmlfile, indentLevel, indentWidth)
            << "<InternalTrigger name=\"" << m_ThresholdName << "\"/>" << endl;
      } else {
         indent(xmlfile, indentLevel, indentWidth)
            << "<TriggerCondition multi=\"" << m_Multiplicity 
            << "\" name=\"" << m_ThresholdName << "_x" << m_Multiplicity 
            << "\" triggerthreshold=\"" << m_ThresholdName << "\"/>" << endl;
      }
      
   } else if (m_NodeType == AND || m_NodeType == OR) {

      std::string logic = (m_NodeType==AND?"AND":"OR");
      indent(xmlfile, indentLevel, indentWidth) << "<" << logic << ">" << endl;
      for(TriggerItemNode *node : children())
         node->writeXML(xmlfile, indentLevel+1, indentWidth);
      indent(xmlfile, indentLevel, indentWidth) << "</"<<logic<<">" << endl;
      
   } else if (m_NodeType == NOT) {

      indent(xmlfile, indentLevel, indentWidth) << "<NOT>" << endl;
      children()[0]->writeXML(xmlfile, indentLevel+1, indentWidth);
      indent(xmlfile, indentLevel, indentWidth) << "</NOT>" << endl;

   }
}


void
TrigConf::TriggerItemNode::buildLogic(std::vector<std::string> & conditionList,
                                      std::string & logic) const {

   if (m_NodeType == OBJ) {
      std::string condition("");
      if (m_Threshold) {
         if( m_Threshold->type() != L1DataDef::rndmType() &&
             m_Threshold->type() != L1DataDef::pclkType() &&
             m_Threshold->type() != L1DataDef::bgrpType()) {
            std::string name = m_Threshold->name();
            name += "_x" + std::to_string(m_Multiplicity);
            condition += '0' + m_Multiplicity;
            condition += "," + name;
            condition += "," + m_Threshold->name();
         } else {
            condition = m_Threshold->name();
         }
      } else {
         condition = thresholdName();
      }
      uint32_t pos = conditionList.size()+1;
      if(pos>9) logic += '0'+pos/10;
      logic += '0'+pos%10;
      conditionList.push_back(condition);
   } else if (m_NodeType == AND || m_NodeType == OR) {
      logic += "(";
      bool first = true;
      for(TriggerItemNode *node : children()) {
         if(!first) { logic += (m_NodeType==AND?"&":"|"); } else { first=false; }
         node->buildLogic(conditionList, logic);
      }
      logic += ")";
   } else if (m_NodeType == NOT) {
      logic += "!";
      children()[0]->buildLogic(conditionList, logic);
   }

}


void
TrigConf::TriggerItemNode::print(const std::string& indent, unsigned int detail) const {
   cout << indent << "TriggerItemNode: "      << endl; 
   cout << indent << "  type        : " << (type()==OBJ?"OBJ":(type()==AND?"AND":(type()==OR?"OR":"NOT"))) << endl;
   cout << indent << "  position    : " << m_Position << endl;

   if (m_NodeType == OBJ) {
      cout << indent << "  multiplicity: " << m_Multiplicity << endl;
      if (m_Threshold) {
         cout << indent << "  threshold   : " << m_Threshold->name() << endl;
         m_Threshold->print(indent + indent, detail);
      } else if(isInternalTrigger()) {
         cout << indent << "  InternalTrigger: "
              << thresholdName() << endl;
      } else {
         cout << indent << thresholdName() << endl;
      }
   }
   else {
      for(TriggerItemNode * node : m_Children) {
         cout << indent << "  subnode : "               << endl;
         node->print(indent + indent);
      }
   }
}

std::ostream& TrigConf::TriggerItemNode::indent(std::ostream& o, int lvl, int size) const {
   if(lvl*size==0) return o;
   o << std::setw(lvl*size) << " ";
   return o;
}


std::ostream &
TrigConf::operator<<(std::ostream & o, const TrigConf::TriggerItemNode & node) {
   o << "        TriggerItemNode: " << endl; 
   o << "           type        : " << (node.type()==TriggerItemNode::OBJ? "OBJ" : (node.type()==TriggerItemNode::AND?"AND":(node.type()==TriggerItemNode::OR?"OR":"NOT"))) << endl;
   o << "           position    : " << node.position() << endl;

   if (node.type() == TriggerItemNode::OBJ) {
      o << "  multiplicity: " << node.multiplicity() << endl;
      if ( node.triggerThreshold() ) {
         o << "  threshold   : " << node.thresholdName() << endl
           << node.triggerThreshold();
      } else if( node.isInternalTrigger() ) {
         o << "  InternalTrigger: " << node.thresholdName() << endl;
      } else {
         o << node.thresholdName() << endl;
      }
   }
   else {
      for(TriggerItemNode* subnode: node.children()) {
         o << "           subnodes : " << *subnode << endl;
      }
   }
   return o;
}

string
TriggerItemNode::__str__() const {
   stringstream s;
   s << *this;
   return s.str();
}
