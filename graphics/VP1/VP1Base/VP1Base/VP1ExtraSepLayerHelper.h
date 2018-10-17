/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1EXTRASEPLAYERHELPER_H
#define VP1EXTRASEPLAYERHELPER_H

class SoNode;
class SoSeparator;
class SoMaterial;

//When a large number of nodes are to be placed under a given
//separator ("topsep"), in an order which does not matter, it can
//potentially be much more performant (3D rendering wise) to add a
//layer of extra separators below topsep, each with, say, 32, of the
//nodes. Caching works a lot better this way.
//
//NB: It is assumed that you never call
//addChild/removeChild/removeAllChildren directly on topsep (at least
//not before the instance of VP1ExtraSepLayerHelper is deleted again).

class VP1ExtraSepLayerHelper {
public:

  VP1ExtraSepLayerHelper( SoSeparator* topsep, int numberOfNodesPerExtraSep = 32 );
  ~VP1ExtraSepLayerHelper();

  SoSeparator * topSeparator() const;
  int numberOfNodesPerExtraSep() const;

  //For efficiency these next two do NOT do any error checking in opt builds!
  void addNode(SoNode*); //Adds node as a grandchild of topsep (under relevant extra separator)
  void removeNode(SoNode*);//Removes node (and reshuffles contents of extra separators as necessay).

  //To avoid propagating notifications needlessly (always call first ...Begin then ...End):
  void largeChangesBegin();//Disables notifications on topsep
  void largeChangesEnd();//Restores previous notifications enabled state on topsep and touches it.

  //By using the following methods, the node is ensured to be placed in
  //the tree such that the passed material is the last material node
  //in front of it (the point is that several nodes can share a
  //material while the material is only placed once in the graph -
  //thus leading to potentially large gains in rendering performance.
  void addNodeUnderMaterial(SoNode*,SoMaterial*);
  void removeNodeUnderMaterial(SoNode*,SoMaterial*);
  void moveNodeToNewMaterial(SoNode*,SoMaterial* oldmat,SoMaterial* newmat);

private:

  // It is illegal to copy/assign a VP1ExtraSepLayerHelper:
  VP1ExtraSepLayerHelper(const VP1ExtraSepLayerHelper & );
  VP1ExtraSepLayerHelper & operator= (const VP1ExtraSepLayerHelper & );

  class Imp;
  Imp * m_d;

};

#endif
