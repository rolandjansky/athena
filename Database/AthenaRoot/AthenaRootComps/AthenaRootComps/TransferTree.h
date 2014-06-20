/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRANSFERTREE_H
#define TRANSFERTREE_H
#include "TTree.h"
#include "SGTools/CLASS_DEF.h"

class TransferTree
{
public: 
  TransferTree() : m_tree(0) {}
  TransferTree(const TTree* t) : m_tree(t) {}
  virtual ~TransferTree() {m_tree=0;}
  void setTree(const TTree* t) {m_tree=t;}
  const TTree* tree() const {return m_tree;}
private:
  const TTree* m_tree;
};

CLASS_DEF(TransferTree,777000777,1)

#endif
