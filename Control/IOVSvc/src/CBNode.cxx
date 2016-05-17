/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  CBNode.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: CBNode.cxx,v 1.3 2007-08-15 21:04:32 leggett Exp $
 *
 *  Tree node structure for callback function trigger tree
 *
 *****************************************************************************/

#ifndef IOVSVC_CBNODE_H
#include "IOVSvc/CBNode.h"
#endif
#ifndef SGTOOLS_DATAPROXY_H
#include "SGTools/DataProxy.h"
#endif

unsigned int CBNode::s_serial = 0;

CBNode::CBNode(std::string name, CBNode* parent): 
  m_name(name), m_proxy(0), m_fcn(0), m_trig(false), m_flag(false) {
  m_serial = ++s_serial;
  addParent( parent );
  if (parent != 0) {
    parent->addChild( this );
    m_level = parent->level() + 1;
  } else {
    m_level = 0;
  }
}

CBNode::CBNode(const SG::DataProxy* proxy, const std::string& name, 
               CBNode* parent): 
  m_name(name), m_proxy(proxy), m_fcn(0), m_trig(false), m_flag(false) {
  m_serial = ++s_serial;
  addParent( parent );
  if (parent != 0) {
    parent->addChild( this );
    m_level = parent->level() + 1;
  } else {
    m_level = 0;
  }
}

CBNode::CBNode(BFCN* fcn, const CallBackID& cb, CBNode* parent): 
  m_proxy(0), m_fcn(fcn), m_cbid(cb), m_trig(false), m_flag(false) {
  m_serial = ++s_serial;
  
  m_name = cb.name();

  addParent( parent );
  if (parent != 0) {
    parent->addChild( this );
    m_level = parent->level() + 1;
  } else {
    m_level = 0;
  }
}

CBNode::~CBNode() {

}

void CBNode::addParent( CBNode* parent ) {
  m_parents.insert( parent );
}

void CBNode::addChild( CBNode* child ) {
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
