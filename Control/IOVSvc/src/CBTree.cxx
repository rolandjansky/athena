/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 *
 *  CBTree.cxx
 *  IOVSvc
 *
 *  Author: Charles Leggett
 *
 *  Callback function trigger tree
 *
 *****************************************************************************/

#include "CBTree.h"

#include "SGTools/DataProxy.h"

#include <iostream>

using namespace std;

//
///////////////////////////////////////////////////////////////////////////
//

CBTree::CBTree() {
  m_root = new CBNode("root",0);
  m_allNodes.insert( m_root );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBTree::~CBTree() {
  for(std::set<CBNode*>::iterator itr=m_allNodes.begin();
      itr != m_allNodes.end(); ++itr) {
    delete *itr;
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::addNode(const SG::DataProxy* proxy, const std::string& name) {

  CBNode* n = new CBNode(proxy, name, m_root);

  m_allNodes.insert( n );

  return n;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::replaceProxy(const SG::DataProxy* pOld, const SG::DataProxy* pNew) {
  assert (0 != pOld);
  assert (0 != pNew);
  CBNode* n(findNode(pOld));
  if (0 != n) n->setProxy(pNew);
  return n;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::addNode(const std::string& name, CBNode* parent) {
  if (parent == 0) {
    parent = m_root;
  }
  CBNode* n = new CBNode(name, parent);

  m_allNodes.insert( n );

  return n;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::addNode(BFCN* fcn, const CallBackID& cb, 
                        const SG::DataProxy* proxy) {

  CBNode* parent = findNode(proxy);

  if (parent == 0) {
    cout << "ERROR: no parent proxy found in tree for " << proxy->name()
         << endl;
    return 0;
  }

  CBNode* n = new CBNode(fcn, cb, parent);

  m_allNodes.insert( n );

  return n;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::addNode(BFCN* fcn, const CallBackID& cb, BFCN* parent_fcn) {

  CBNode* parent = findNode(parent_fcn);

  if (parent == 0) {
    cout << "ERROR: no parent function found in tree for " << parent_fcn
         << endl;
    return 0;
  }

  CBNode* n = new CBNode(fcn, cb, parent);

  m_allNodes.insert( n );

  return n;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

bool CBTree::delNode(const SG::DataProxy* prx) {
  CBNode *n = findNode(prx);
  if (n == 0) {
    cout << "ERROR: no node with DataProxy " << prx->name() << " found in tree"
         << endl;
    return false;
  }
  
  bool b(true);
  for ( auto p : n->parents() ) {
    if (!p->delChild( n )) {
      cout << "ERROR: CBTree::delNode : unable to delete child " 
           << n->name() << " from parent " << p->name() << endl;
      b = false;
    }
  }
  
  for (auto c : n->children()) {
    if (!c->delParent( n )) {
      cout << "ERROR: CBTree::delNode : unable to delete parent " 
           << n->name() << " from child " << c->name() << endl;
      b = false;
    }
  }
  
  return b;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::connectNode(CBNode* node, CBNode* parent) {

  // check to see if we're creating a loop
  cascadeFlag(true, node);
  if (parent->flag()) {
    cout << "ERROR: cannot connect " << node->name() << " to "
         << parent->name() << " as a loop would be formed"
         << endl;
    return;
  }
  clearFlag();

  node->addParent( parent );
  parent->addChild( node );
  if (node->level() < (parent->level() + 1)) {
    // we need to erase it first and then reinsert it as the ordering
    // critereon has changed. Do this for all children too.
    m_allNodes.erase( m_allNodes.find( node ) );
    node->setLevel( parent->level() + 1);
    m_allNodes.insert( node );

    adjustLevels( node );

  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::connectNode(const std::string& name, CBNode* parent) {
  CBNode* n = findNode(name);
  if (n == 0) {
    addNode(name,parent);
  } else {
    connectNode(n, parent);
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::findNode(const SG::DataProxy* proxy) const {

  return findNode(proxy, m_root);

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::findNode(const SG::DataProxy* proxy, CBNode* start) const {

  if ( start->proxy() == proxy ) {
    return start;
  } else {

    for ( auto c : start->children() ) {
      c = findNode(proxy, c);
      if (c != 0) {
        return c;
      }
    }
  }

  return 0;
  
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::findNode(BFCN* fcn) const {
  return findNode(fcn, m_root);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::findNode(BFCN* fcn, CBNode* start) const {

  if (start->fcn() == fcn) {
    return start;
  } else {

    CBNode *c;
    std::set<CBNode*>::const_iterator citr = start->children().begin();
    for (; citr != start->children().end(); ++citr) {
      c = findNode(fcn,*citr);
      if (c != 0) {
        return c;
      }
    }
  }

  return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::findNode(const std::string& name) const {
  return findNode(name, m_root);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CBNode* CBTree::findNode(const std::string& name, CBNode* start) const {

  if (start->name() == name) {
    return start;
  } else {

    CBNode *c;
    std::set<CBNode*>::const_iterator citr = start->children().begin();
    for (; citr != start->children().end(); ++citr) {
      c = findNode(name,*citr);
      if (c != 0) {
        return c;
      }
    }
  }

  return 0;
}
    
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::printTree() const {
  cout << "total entries: " << m_allNodes.size() << "  max levels: "
       << maxLevel() << endl;
  traverse( m_root, 0, &CBTree::_printTree );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::printTree( CBNode* start ) const {

  for (int i=0; i<start->level(); ++i) {
    cout << "  ";
  }

  if (start->trigger()) {
    cout << "[93;1m";
  }
  cout << start->name();
  if (start->trigger()) {
    cout << "[m";
  }
  cout << endl;

  std::set<CBNode*>::const_iterator citr;
  for (citr=start->children().begin(); citr!=start->children().end(); ++citr) {
    printTree( *citr );
  }

}

void CBTree::_printTree( CBNode* current, CBNode* parent ) {
  std::string np;
  int lp;
  if (parent == 0) {
    np = "";
    lp = 0;
  } else {
    np = parent->name();
    lp = parent->level();
  }
    
  for (int i=0; i<lp; ++i) {
    cout << "  ";
  }

  for (int i=0; i<(current->level()-lp); ++i) {
    if ( i == 0 ) {
      //      cout << "";
      cout << "+-";
    } else {
      //      cout << "";
      cout << "--";
    }
  }

  if (current->trigger()) {
    cout << "[93;1m" << current->name() << "[m";
  } else {
    cout << current->name();
  }
  cout << endl;
  
} 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int CBTree::maxLevel() const {
  nodeSet::const_iterator itr = m_allNodes.end();
  --itr;
  return (*itr)->level();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::listNodes() const {
  nodeSet::const_iterator citr = m_allNodes.begin();
  for (; citr != m_allNodes.end(); ++citr) {
    cout << (*citr)->name()  << "  " << *citr << "  " << (*citr)->level() 
         << "  " << (*citr)->trigger() << endl;
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::listNodes(const int& level, 
                       nodeSet::const_iterator& start, 
                       nodeSet::const_iterator& end) const {

  bool s = false;

  start = m_allNodes.end();
  end = m_allNodes.end();

  nodeSet::const_iterator citr = m_allNodes.begin();
  for (; citr != m_allNodes.end(); ++citr) {
    if (!s && (*citr)->level() == level) {
      s = true;
      start = citr;
    }

    if (s && (*citr)->level() != level) {
      end = citr;
      break;
    }
  }

  return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::cascadeTrigger(const bool b, CBNode* start) const {
  if (start == 0) {
    start = m_root;
  }

  start->setTrigger(b);

  std::set<CBNode*>::iterator citr = start->children().begin();
  for (;citr!=start->children().end(); ++citr) {
    cascadeTrigger(b, *citr);
  }
}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::cascadeTrigger(const bool b, BFCN* fcn) const {

  CBNode* start = findNode(fcn);
  if (start == 0) {
    cout << "ERROR cascading trigger from fcn: " << fcn << endl;
    return;
  }

  cascadeTrigger(b, start);

}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::cascadeTrigger(const bool b, const SG::DataProxy* proxy) const {

  CBNode* start = findNode(proxy);
  if (start == 0) {
    cout << "ERROR cascading trigger from proxy: " << proxy->name() << endl;
    return;
  }

  cascadeTrigger(b, start);

}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::clearTrigger() const {

  nodeSet::const_iterator itr;
  for (itr=m_allNodes.begin(); itr != m_allNodes.end(); ++itr) {
    (*itr)->setTrigger(false);
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::cascadeFlag(const bool b, CBNode* start) const {
  if (start == 0) {
    start = m_root;
  }

  start->setFlag(b);

  std::set<CBNode*>::iterator citr = start->children().begin();
  for (;citr!=start->children().end(); ++citr) {
    cascadeFlag(b, *citr);
  }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CBTree::clearFlag() const {
  nodeSet::const_iterator itr;
  for (itr=m_allNodes.begin(); itr != m_allNodes.end(); ++itr) {
    (*itr)->setFlag(false);
  }
}
 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// adjust levels of all children to this node
void CBTree::adjustLevels(CBNode* start) {
  
  int level = start->level();

  std::set<CBNode*>::iterator citr = start->children().begin();
  for (;citr != start->children().end(); ++citr) {
    m_allNodes.erase( m_allNodes.find( *citr ) );
    (*citr)->setLevel( level+1 );
    m_allNodes.insert( *citr );
    adjustLevels( *citr );
  }
}
 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Traverse full tree, applying function   void fnc(current)   to each node

void CBTree::traverse(void(*pf) (CBNode*)) const {
  
  CBNode* current = m_root;

  traverse(current, pf);

}

void CBTree::traverse(CBNode* current, void(*pf) (CBNode*)) const {

  if (current == 0) { return; }
  
  (*pf)(current);
  
  std::set<CBNode*>::const_iterator citr = current->children().begin();
  for (; citr != current->children().end(); ++citr) {
    traverse( *citr, pf);
  }

}

void CBTree::traverseR(CBNode* current, void(*pf) (CBNode*)) const {
  // traverse tree in reverse

  if (current == 0) { return; }
  
  (*pf)(current);
  
  std::set<CBNode*>::const_iterator citr = current->parents().begin();
  for (; citr != current->parents().end(); ++citr) {
    traverseR( *citr, pf);
  }

}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Traverse tree, applying function  void fnc(current, parent)  to each node.

void CBTree::traverse(void(*pf) (CBNode*,CBNode*)) const {
  
  CBNode* current = m_root;

  traverse(current,0, pf);

}

void CBTree::traverse(CBNode* current, CBNode* parent, 
                      void(*pf) (CBNode*, CBNode*)) const {

  if (current == 0) { return; }
  
  (*pf)(current,parent);
  
  std::set<CBNode*>::const_iterator citr = current->children().begin();
  for (; citr != current->children().end(); ++citr) {
    traverse( *citr, current, pf);
  }

}

void CBTree::traverseR(CBNode* current, CBNode* child, 
                       void(*pf) (CBNode*, CBNode*)) const {

  if (current == 0) { return; }
  
  (*pf)(current,child);
  
  std::set<CBNode*>::const_iterator citr = current->parents().begin();
  for (; citr != current->parents().end(); ++citr) {
    traverseR( *citr, current, pf);
  }

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Traverse tree, stopping when function  CBNode* fnc( current )  returns
// a non-null. Good for searches.

CBNode* CBTree::traverse(CBNode* (*pf) (CBNode*)) const {

  return traverse(m_root, pf);

}

CBNode* CBTree::traverse(CBNode* current, CBNode* (*pf) (CBNode*)) const {

  CBNode *n = (*pf)(current);

  if ( n != 0 ) {
    return n;
  } else {

    CBNode *c;
    std::set<CBNode*>::const_iterator citr = current->children().begin();
    for (; citr != current->children().end(); ++citr) {
      c = traverse(*citr, pf );
      if (c != 0) {
        return c;
      }
    }
  }

  return 0;
    
}

CBNode* CBTree::traverseR(CBNode* current, CBNode* (*pf) (CBNode*)) const {

  CBNode *n = (*pf)(current);

  if ( n != 0 ) {
    return n;
  } else {

    CBNode *c;
    std::set<CBNode*>::const_iterator citr = current->parents().begin();
    for (; citr != current->parents().end(); ++citr) {
      c = traverseR(*citr, pf );
      if (c != 0) {
        return c;
      }
    }
  }

  return 0;
    
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
