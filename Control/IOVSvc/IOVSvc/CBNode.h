/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IOVSVC_CBNODE_H
#define IOVSVC_CBNODE_H

/*****************************************************************************
 *
 *  CBNode.h
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *  $Id: CBNode.h,v 1.3 2007-08-15 21:04:32 leggett Exp $
 *
 *  Tree node structure for callback function trigger tree
 *
 *****************************************************************************/

#include <set>
#include <string>

#ifndef ATHENAKERNEL_IOVSVCDEFS_H
#include "AthenaKernel/IOVSvcDefs.h"
#endif

#ifndef SGTOOLS_CALLBACKID_H
#include "SGTools/CallBackID.h"
#endif

namespace SG {
  class DataProxy;
}

class CBNode {
public:

  typedef IOVSvcCallBackFcn BFCN;

  class nodeOrder {
  public:
    bool operator() ( const CBNode &n1, const CBNode &n2 ) const {
      if ( n1.level() != n2.level() ) {
        return ( n1.level() < n2.level() );
      } else {
        return ( n1.serial() < n2.serial() );
      }
    }

    bool operator() ( const CBNode *n1, const CBNode *n2 ) const {
      if ( n1->level() != n2->level() ) {
        return ( n1->level() < n2->level() );
      } else {
        return ( n1->serial() < n2->serial() );
      }
    }
  };

  typedef std::set<CBNode*, CBNode::nodeOrder> nodeSet;

  CBNode(std::string name, CBNode* parent);
  CBNode(const SG::DataProxy* proxy, const std::string& name, CBNode* parent);
  CBNode(BFCN* fcn, const CallBackID& cb, CBNode* parent);
  ~CBNode();
  
  void addParent(CBNode* parent);
  void addChild(CBNode* child);

  bool delParent(CBNode* parent);
  bool delChild(CBNode* child);

  const nodeSet& children() const { return m_children; }
  const nodeSet& parents() const { return m_parents; }

  std::string name() const { return m_name; }
  int level() const { return m_level; }
  void setLevel(int l) { m_level = l; }

  bool trigger() const { return m_trig; }
  void setTrigger(bool b) { m_trig = b; }

  bool flag() const { return m_flag; }
  void setFlag(bool b) { m_flag = b; }

  const SG::DataProxy* proxy() const { return m_proxy; }
  void setProxy(const SG::DataProxy* p);

  BFCN* fcn() const { return m_fcn; }

  unsigned int serial() const { return m_serial; }

private:
  nodeSet m_parents;
  nodeSet m_children;

  std::string m_name;
  int m_level;

  const SG::DataProxy* m_proxy;
  BFCN* m_fcn;
  CallBackID m_cbid;

  bool m_trig;
  bool m_flag;

  unsigned int m_serial;
  static unsigned int s_serial;
};

#endif
