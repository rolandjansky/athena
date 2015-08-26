/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ElectronObjectsInRoI_h__
#define __ElectronObjectsInRoI_h__
/*
  ElectronObjectsInRoI.h
*/
#include "TriggerMenuAnalysis/HLTObjectsInRoI.h"
class TrigEMCluster;
class TrigVertexCollection;
class TrigInDetTrackCollection;
class TrigElectronContainer;
class egammaContainer;

namespace HLT {
  class TriggerElement;
  class Navigation;
}

class ElectronObjectsInRoI : public HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

public:
  ElectronObjectsInRoI();
  ~ElectronObjectsInRoI();

  const TrigEMCluster* getEMCluster() const { return mEMCluster; }
  const TrigVertexCollection* getVertexCollection() const {
    return mVertexCollection;
  }
  const TrigInDetTrackCollection* getInDetTrackCollection() const {
    return mInDetTrackCollection;
  }
  const TrigElectronContainer* getTrigElectronContainer() const {
    return mTrigElectronContainer;
  }

  const egammaContainer* getEFegamma() const { return mEFegamma; }

  void setEMCluster(const TrigEMCluster* x) { mEMCluster = x; }
  void setVertexCollection(const TrigVertexCollection* x) { mVertexCollection = x; }
  void setInDetTrackCollection(const TrigInDetTrackCollection* x) { mInDetTrackCollection = x; }
  void setEFegamma(const egammaContainer* x) { mEFegamma = x; }

//   bool runBySameFex(const HLTObjectsInRoI* x) const;
//   bool setDataFromTE(int isig, 
// 		     const HLT::TriggerElement* te, 
// 		     HLT::Navigation* navitool,int level);

protected:
  const TrigEMCluster* mEMCluster;
  const TrigVertexCollection* mVertexCollection;
  const TrigInDetTrackCollection* mInDetTrackCollection;
  const TrigElectronContainer* mTrigElectronContainer;

  const egammaContainer* mEFegamma; 
};

#endif //__ElectronObjectsInRoI_h__
