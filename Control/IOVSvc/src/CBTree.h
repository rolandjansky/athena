/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_CBTREE_H
#define IOVSVC_CBTREE_H

/*****************************************************************************
 *
 *  CBTree.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *
 *  Callback function trigger tree
 *
 *****************************************************************************/

#include "CBNode.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include <set>
#include <string>

namespace SG {
  class DataProxy;
}

class CBTree {
public:

  typedef std::set<CBNode*, CBNode::nodeOrder> nodeSet;
  typedef IOVSvcCallBackFcn BFCN;

  CBTree();
  ~CBTree();

  CBTree (const CBTree&) = delete;
  CBTree& operator= (const CBTree&) = delete;

  CBNode* addNode(const std::string& name, CBNode* parent);
  CBNode* addNode(const SG::DataProxy* proxy, const std::string& name);
  CBNode* addNode(BFCN* fcn, const CallBackID& cb, 
                  const SG::DataProxy* parent_proxy);
  CBNode* addNode(BFCN* fcn, const CallBackID& cb, BFCN* parent_fcn);

  CBNode* replaceProxy(const SG::DataProxy* pOld, const SG::DataProxy* pNew);

  void connectNode(CBNode* node, CBNode* parent);
  void connectNode(const std::string& name, CBNode* parent);

  CBNode* findNode(const std::string& name) const;
  CBNode* findNode(const std::string& name, CBNode* start) const;

  CBNode* findNode(const SG::DataProxy* proxy) const;
  CBNode* findNode(const SG::DataProxy* proxy, CBNode* start) const;

  CBNode* findNode(BFCN* fcn) const;
  CBNode* findNode(BFCN* fcn, CBNode* start) const;

  bool delNode(const SG::DataProxy* prx);

  void printTree() const;
  void printTree( CBNode* start ) const;

  int maxLevel() const;
  void adjustLevels( CBNode* start );

  void listNodes() const;
  void listNodes( const int& level, 
                  nodeSet::const_iterator& start, 
                  nodeSet::const_iterator& end ) const;

  void cascadeTrigger(const bool b, CBNode* start) const;
  void cascadeTrigger(const bool b, BFCN* fcn) const;
  void cascadeTrigger(const bool b, const SG::DataProxy* proxy) const;

  void clearTrigger() const;

  void cascadeFlag(const bool b, CBNode* node) const;
  void clearFlag() const;

  void traverse( void (*pF) (CBNode*) ) const;
  void traverse( CBNode*, void (*pF) (CBNode*) ) const;
  void traverseR( CBNode*, void (*pF) (CBNode*) ) const;

  CBNode* traverse( CBNode* (*pF) (CBNode*) ) const;
  CBNode* traverse( CBNode*, CBNode* (*pF) (CBNode*) ) const;
  CBNode* traverseR( CBNode*, CBNode* (*pF) (CBNode*) ) const;

  void traverse( void (*pF) (CBNode*, CBNode*) ) const;
  void traverse( CBNode*, CBNode*, void (*pF) (CBNode*, CBNode*) ) const;
  void traverseR( CBNode*, CBNode*, void (*pF) (CBNode*, CBNode*) ) const;

private:
  
  static void _printTree( CBNode*, CBNode* );

  CBNode* m_root;
  nodeSet m_allNodes;

};

#endif
