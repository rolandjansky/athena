/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKROADKDTREE_H
#define FTKROADKDTREE_H

#include "FTKRoad.h"

#include <list>

/** this class describes a KD-Tree to simplify
    the NN search for the road, used to improve the RW
    performances */
class FTKRoadKDTree {
private:
  int m_dim; // dimension of the space
  int m_depth; // depth level, 0 is the top level
  int m_splitplane; // coordinate compated at this level
  FTKRoad *m_node; // element
  int m_pos; // position
  FTKRoadKDTree *m_parent; // parent element
  FTKRoadKDTree *m_left; // left sub-tree
  FTKRoadKDTree *m_right; // right sub-tree

  int m_maxdepth; // max depth
  int m_maxdepth_l; 
  int m_maxdepth_r; 

public:
  FTKRoadKDTree(int, FTKRoad*, int, int depth=0, FTKRoadKDTree *parent=0);
  ~FTKRoadKDTree();

  FTKRoadKDTree* addNode(FTKRoad*, int);
  FTKRoad* getNode() const { return m_node; }
  int getPosition() const { return m_pos; }
  const int &getDepth() const { return m_depth; }
  const int &getSplitPlane() const { return m_splitplane; }
  const int &getMaxDepth() const { return m_maxdepth; }
  const int &getMaxDepthLeft() const { return m_maxdepth_l; }
  const int &getMaxDepthRight() const { return m_maxdepth_r; }
  int findSimilar(const FTKRoad*, unsigned int, std::list<FTKRoadKDTree*> &);
};



#endif // FTKROADKDTREE_H
