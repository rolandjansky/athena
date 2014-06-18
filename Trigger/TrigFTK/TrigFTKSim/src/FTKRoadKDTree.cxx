/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKRoadKDTree.h"

#include <iostream>
using namespace std;

/** the constructor should know the dimensione of the space */
FTKRoadKDTree::FTKRoadKDTree(int dim, FTKRoad *road, int pos,
			     int depth, FTKRoadKDTree *parent) :
  m_dim(dim), m_depth(depth), m_node(road), m_pos(pos),
  m_parent(parent), m_left(0), m_right(0),
  m_maxdepth(0), m_maxdepth_l(0), m_maxdepth_r(0)
{
  // determine the SS that splits the hyper-space
  m_splitplane = m_depth % m_dim;  
}


/** destructor of the Tree, recurivelly destroy all the level */
FTKRoadKDTree::~FTKRoadKDTree()
{
  if (m_left) delete m_left;
  if (m_right) delete m_right;
}


/** find an empty leaf to place this road in the kd-tree*/
FTKRoadKDTree* FTKRoadKDTree::addNode(FTKRoad *newroad, int pos)
{
  FTKRoadKDTree *newnode(0);

  // determine the direction
  if (newroad->getSSID(m_splitplane) <= m_node->getSSID(m_splitplane) ) {
    // is the left part the axis
    if (!m_left) {
      // create a new sub-tree
      m_left = new FTKRoadKDTree(m_dim,newroad,pos,m_depth+1,this);
      newnode = m_left;
    }
    else {
      // continue the search
      newnode = m_left->addNode(newroad,pos);
    }
  }
  else {
    // is on the right part of the axis
    if (!m_right) {
      // create a new sub-tree
      m_right = new FTKRoadKDTree(m_dim,newroad,pos,m_depth+1,this);
      newnode = m_right;
    }
    else {
      // continue the search
      newnode = m_right->addNode(newroad,pos);
    }
  }

  int reldep = newnode->getDepth()-m_depth;
  if (reldep>m_maxdepth) m_maxdepth = reldep;
  if (reldep>m_maxdepth_l) m_maxdepth_l = reldep;
  if (reldep>m_maxdepth_r) m_maxdepth_r = reldep;

  return newnode;
} 


/** this function walk in the tree to find if there are
    roads with all the SSID, except one, shared with the road
    that heads this sub-tree */
int FTKRoadKDTree::findSimilar(const FTKRoad *refroad, 
			       unsigned int unconstrmask,
			       list<FTKRoadKDTree*> &nodes)
{
  int nsims(0);

  // evaluate if the constrained coordinates are te same
  bool match(true);
  for (int ip=0;ip!=m_dim && match==true;++ip) {
    if (refroad->getSSID(ip)!=m_node->getSSID(ip) && 
	(unconstrmask & (1<<ip))==0) match = false;
  }
  if (match && refroad!=m_node) {
    // this node is similar, with respect the mask
    nodes.push_back(this);
    nsims += 1;
  }

  // prevent the serach if there is any node or if is the direction where all started
  if (m_left) {
    // if the coordinate partioned by this level is in the unconstrmask,
    // or if it's constrained but the similar roads could be in this part
    // continue the search
    if ( (refroad->getSSID(m_splitplane)<=m_node->getSSID(m_splitplane)) ||
	 (unconstrmask & (1<<m_splitplane)) ) {
      
      nsims += m_left->findSimilar(refroad,unconstrmask,nodes);
    }    
  }

  if (m_right) {
    // if the coordinate partioned by this level is in the unconstrmask,
    // or if it's constrained but the similar roads could be in this part
    // continue the search
    if ( (refroad->getSSID(m_splitplane)>getNode()->getSSID(m_splitplane)) ||
	 (unconstrmask & (1<<m_splitplane)) ) {
      
      nsims += m_right->findSimilar(refroad,unconstrmask,nodes);
    }    
  }

  return nsims;
}
