/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_ELECTRONOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_ELECTRONOBJECTSINROI_H
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

  const TrigEMCluster* getEMCluster() const { return m_EMCluster; }
  const TrigVertexCollection* getVertexCollection() const {
    return m_vertexCollection;
  }
  const TrigInDetTrackCollection* getInDetTrackCollection() const {
    return m_inDetTrackCollection;
  }
  const TrigElectronContainer* getTrigElectronContainer() const {
    return m_trigElectronContainer;
  }

  const egammaContainer* getEFegamma() const { return m_EFegamma; }

  void setEMCluster(const TrigEMCluster* x) { m_EMCluster = x; }
  void setVertexCollection(const TrigVertexCollection* x) { m_vertexCollection = x; }
  void setInDetTrackCollection(const TrigInDetTrackCollection* x) { m_inDetTrackCollection = x; }
  void setEFegamma(const egammaContainer* x) { m_EFegamma = x; }

//   bool runBySameFex(const HLTObjectsInRoI* x) const;
//   bool setDataFromTE(int isig, 
// 		     const HLT::TriggerElement* te, 
// 		     HLT::Navigation* navitool,int level);

protected:
  const TrigEMCluster* m_EMCluster;
  const TrigVertexCollection* m_vertexCollection;
  const TrigInDetTrackCollection* m_inDetTrackCollection;
  const TrigElectronContainer* m_trigElectronContainer;

  const egammaContainer* m_EFegamma; 
};

#endif // TRIGGERMENUANALYSIS_ELECTRONOBJECTSINROI_H
