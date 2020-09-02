/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  CBNode.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *
 *  Tree node structure for callback function trigger tree
 *
 *****************************************************************************/

#include "CBNode.h"

#include "SGTools/DataProxy.h"

unsigned int CBNode::s_serial = 0;

CBNode::CBNode(const std::string& name, CBNode* parent): 
  m_name(name), m_proxy(0), m_fcn(0), m_trig(false), m_flag(false) {
  m_serial = ++s_serial;
  if (parent != 0) {
    m_level = parent->level() + 1;
    addParent( parent );
    parent->addChild( this );
  } else {
    m_level = 0;
  }
}

CBNode::CBNode(const SG::DataProxy* proxy, const std::string& name, 
               CBNode* parent): 
  m_name(name), m_proxy(proxy), m_fcn(0), m_trig(false), m_flag(false) {
  m_serial = ++s_serial;
  if (parent != 0) {
    m_level = parent->level() + 1;
    addParent( parent );
    parent->addChild( this );
  } else {
    m_level = 0;
  }
}

CBNode::CBNode(BFCN* fcn, const CallBackID& cb, CBNode* parent): 
  m_proxy(0), m_fcn(fcn), m_cbid(cb), m_trig(false), m_flag(false) {
  m_serial = ++s_serial;
  
  m_name = cb.name();

  if (parent != 0) {
    m_level = parent->level() + 1;
    addParent( parent );
    parent->addChild( this );
  } else {
    m_level = 0;
  }
}

CBNode::~CBNode() {

}

void CBNode::addParent( CBNode* parent ) {
  assert( parent);
  m_parents.insert( parent );
}

void CBNode::addChild( CBNode* child ) {
  assert(child);
  m_children.insert( child );
}

bool CBNode::delParent( CBNode* par ) {
  std::set<CBNode*>::iterator itr = m_parents.find(par);
  if (itr != m_parents.end()) {
    m_parents.erase( itr );
    return true;
  }
  return false;
}

bool CBNode::delChild( CBNode* child ) {
  std::set<CBNode*>::iterator itr = m_children.find(child);
  if (itr != m_children.end()) {
    m_children.erase( itr );
    return true;
  }
  return false;
}

void CBNode::setProxy( const SG::DataProxy* p) {
  assert(0 != p);
  m_proxy=p;
  m_name=p->name();
}
