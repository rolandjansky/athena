/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERMENUANALYSIS_HLTOBJECTSINROI_H
#define TRIGGERMENUANALYSIS_HLTOBJECTSINROI_H
/*
  HLTObjectsInRoI.h
*/
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "GaudiKernel/MsgStream.h"
#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Conditions.h"
#include "TriggerMenuNtuple/TrigStatus.h"


class HLTObjectsInRoI {
public:
  static HLTObjectsInRoI* create();

private:
  class HltFeature {
  public:
    HltFeature() {}
    virtual ~HltFeature();
    virtual const void* objectAddress() const = 0;
    virtual int getObject(const HLT::TriggerElement* te, 
			  HLT::NavigationCore* navitool)=0;
    virtual int getObject(const Trig::Combination& comb, bool& passed) = 0;
    virtual int getRoIObject(const Trig::TrigDecisionTool& tdt, 
			     const HLT::TriggerElement* te) = 0;

    void setName(const std::string& name) { m_name = name; }
    void addChain(const std::string& chain_name, bool passed);
    const std::string& name() const { return m_name; }
    const std::vector<TrigStatus>& chains() const { return m_chains; }
    std::vector<std::string> getAssociatedChains() const;
    std::vector<std::string> getPassedChains() const;
    virtual const HLT::TriggerElement* getTEforObj(const Trig::Combination& comb) const=0;
    virtual const HLT::TriggerElement* getTEforObj(const Trig::FeatureContainer& fc) const=0;
  protected:
    std::string m_name;
    std::vector<TrigStatus> m_chains;
  };

  template<class T>
  class HltFeatureT : public HltFeature {
  public:
    HltFeatureT(const T* &obj, int size) : 
      HltFeature(), m_obj(obj), m_size(size) {}
    ~HltFeatureT() {}
    const void* objectAddress() const { return m_obj; }
    int getObject(const HLT::TriggerElement* te, 
 		  HLT::NavigationCore* navitool);
    int getObject(const Trig::Combination& comb, bool& passed);
    int getRoIObject(const Trig::TrigDecisionTool& tdt, 
		     const HLT::TriggerElement* te);
    const HLT::TriggerElement* getTEforObj(const Trig::Combination& comb) const;
    const HLT::TriggerElement* getTEforObj(const Trig::FeatureContainer& fc) const;
  private:
    const T* &m_obj;
    int m_size;
  };

  template<class T>
  class L1FeatureT : public HltFeature {
  public:
    L1FeatureT(const T* &obj, int size) : 
      HltFeature(), m_obj(obj), m_size(size) {}
    ~L1FeatureT() {}
    const void* objectAddress() const { return m_obj; }
    int getRoIObject(const Trig::TrigDecisionTool& tdt, 
		     const HLT::TriggerElement* te);

    int getObject(const HLT::TriggerElement* /*te*/, 
		  HLT::NavigationCore* /*navitool*/) { return -1; }
    int getObject(const Trig::Combination& /*comb*/, bool& /*passed*/) {
      return -1;
    }
    const HLT::TriggerElement* getTEforObj(const Trig::Combination& /*comb*/) const {
      return 0;
    }
    const HLT::TriggerElement* getTEforObj(const Trig::FeatureContainer& /*fc*/) const {
      return 0;
    }

  private:
    const T* &m_obj;
    int m_size;
  };

public:
  HLTObjectsInRoI();
  virtual ~HLTObjectsInRoI();

  std::vector<std::string> getAssociatedChains(void* feature) const;
  std::vector<std::string> getPassedChains(void* feature) const;
  std::vector<TrigStatus> getChains(const void* feature) const;

  bool isAssociated(const std::string& chain_name, void* feature) const;
  bool isPassed(const std::string& chain_name, void* feature) const;

  virtual bool runBySameFex(const  HLTObjectsInRoI* x) const;
  virtual bool setDataFromTE(int isig, const HLT::TriggerElement* te, 
			     HLT::NavigationCore* navitool, int level);
  int setDataFromCombination(const Trig::Combination& comb, 
			     const std::string& chain_name, 
			     const Trig::TrigDecisionTool& tdt);
  bool addFeatureChainStatus(const void* feature, 
			     const TrigStatus& chain_status);
  int updateChainStatus(const std::string& chain_name, 
			const Trig::Combination& comb);
  int updateChainStatus(const std::string& chain_name, 
			const Trig::FeatureContainer& comb);

  void dump() const;

  // OLD METHODS (with TrigDecisionTool 14)

  //  const HLT::TriggerElement* getRoITE() const { return mRoITE; }
  std::vector<const HLT::TriggerElement*> getRoITEs() const {
    return m_RoITEs;
  }
  std::vector<const HLT::TriggerElement*> getTEs() const { return m_TEs; }
 
  bool isAssociated(const std::string& cname) const;
  bool isPassed(const std::string& cname) const;
  inline std::vector<std::string> getAssociatedChains() const;
  inline std::vector<std::string> getPassedChains() const;

  //  void setRoITE(const HLT::TriggerElement* te) { mRoITE = te; }
  void addRoITE(const HLT::TriggerElement* te);
  void addRoITEs(const std::vector<HLT::TriggerElement*>& tes);
  void addTE(const HLT::TriggerElement* te);
  void addAssociatedChain(const std::string& cname);
  void addPassedChain(const std::string& cname);

  bool isInSameRoI(const HLTObjectsInRoI* x, HLT::NavigationCore* navitool) const;
  bool isInSameRoI(const HLT::TriggerElement* te, 
		   HLT::NavigationCore* navitool) const;

private:
  void dumpFeature(const HltFeature* feature) const;

protected:
  class LevelSigId {
  public:
    LevelSigId(int ilevel=0, int isig=0) : level(ilevel), sig(isig) {}
    int level;
    int sig;
    bool operator<(const LevelSigId& y) const;
  };

  template<class T>
  void defineHltFeature(const T* &obj, int level, int isig, 
			const std::string& name="", int n_objs=1);
  template<class T>
  void defineL1RoI(const T* &obj, 
		   const std::string& name="", int n_objs=1);

  const HltFeature* findFeature(const void *obj) const;
  HltFeature* findFeature(const void *obj);

protected:
  std::vector<const HLT::TriggerElement*> m_RoITEs;
  std::vector<const HLT::TriggerElement*> m_TEs;
  std::vector<std::string> m_associatedChains;
  std::vector<std::string> m_passedChains;

  MsgStream& log() const;

private:
  static MsgStream* s_log;
  std::map<LevelSigId, std::vector<HltFeature*> > m_hltFeatureDefs;
};

std::vector<std::string> HLTObjectsInRoI::getAssociatedChains() const {
  return m_associatedChains;
}

std::vector<std::string> HLTObjectsInRoI::getPassedChains() const {
  return m_passedChains;
}

// Template methods

template<class T>
int HLTObjectsInRoI::HltFeatureT<T>::getObject(const Trig::Combination& comb, bool& passed) {
   passed = false;
   const std::vector<Trig::Feature<T> > features = 
      comb.get<T>("", TrigDefs::alsoDeactivateTEs);
   if (m_size == 1 && features.size() == 1) {
      // assume only one object is attached to the TriggerElement
      if (m_obj==0) m_obj = features[0].cptr();
      if (m_obj) {
         const HLT::TriggerElement* te = features[0].te();
         unsigned int nj = te->getFeatureAccessHelpers().size();
         //unsigned int clid = 0;
         for (unsigned int j=0; j<nj; ++j) {
            //clid =
            te->getFeatureAccessHelpers()[j].getCLID();
            //	std::cout << "clid of the feature found: " << clid << std::endl;
         }
         if (te->getActiveState()) passed = true;
      }
   }
   if (m_obj) return 0;
   else return -1;
}

template<class T>
int HLTObjectsInRoI::HltFeatureT<T>::getObject(const HLT::TriggerElement* te, 
					       HLT::NavigationCore* navitool) {
  std::vector<const T*> tmp;
  navitool->getFeatures(te, tmp);
  if (m_size == 1 && tmp.size() == 1) {
    // assume only one object is attached to the TriggerElement
    m_obj = tmp[0];
  }
  if (m_obj) return 0;
  else return -1;
}

template<class T>
const HLT::TriggerElement* 
HLTObjectsInRoI::HltFeatureT<T>::getTEforObj(const Trig::Combination& comb) const {
  const std::vector<Trig::Feature<T> > features = 
    comb.get<T>("", TrigDefs::alsoDeactivateTEs);
  //  std::cout << "N features for comb: " << features.size() << std::endl;
  for (unsigned int i=0; i<features.size(); ++i) {
    //    std::cout << "Feature: cptr=" 
    //	      << std::hex << features[i].cptr() << std::dec
    //	      << " te=" << std::hex << features[i].te() << std::dec << std::endl;
    if (features[i].cptr() == m_obj) {
      return features[i].te();
    }
  }
  return 0;
}

template<class T>
const HLT::TriggerElement* 
HLTObjectsInRoI::HltFeatureT<T>::getTEforObj(const Trig::FeatureContainer& fc) const {
  const std::vector<Trig::Feature<T> > features = 
    fc.get<T>("", TrigDefs::alsoDeactivateTEs);
  for (unsigned int i=0; i<features.size(); ++i) {
    if (features[i].cptr() == m_obj) {
      return features[i].te();
    }
  }
  return 0;
}

template<class T>
int HLTObjectsInRoI::HltFeatureT<T>::getRoIObject(const Trig::TrigDecisionTool& tdt, 
						 const HLT::TriggerElement* te) {
  if (te==0) {
    return -2;
  }
  const Trig::Feature<T> feature = tdt.ancestor<T>(te);
  if (m_obj==0) m_obj = feature.cptr();
  //  std::cout << "RoI TE: " << feature.te() << ", obj=" << m_obj << std::endl;
  if (m_obj) {
    return 0;
  } else {
    return -1;
  }
}

template<class T>
int HLTObjectsInRoI::L1FeatureT<T>::getRoIObject(const Trig::TrigDecisionTool& tdt, 
						 const HLT::TriggerElement* te) {
  const Trig::Feature<T> feature = tdt.ancestor<T>(te);
  if (m_obj==0) m_obj = feature.cptr();
  //  std::cout << "RoI TE: " << feature.te() << ", obj=" << m_obj << std::endl;
  if (m_obj) {
    return 0;
  } else {
    return -1;
  }
}


template<class T>
void HLTObjectsInRoI::defineHltFeature(const T* &obj, 
				       int level, int isig, 
				       const std::string& name, 
				       int n_objs) {
  LevelSigId lsid(level, isig);
  std::map<LevelSigId, std::vector<HltFeature*> >::iterator p;
  if ( (p=m_hltFeatureDefs.find(lsid)) == m_hltFeatureDefs.end()) {
    std::vector<HltFeature*> tmp;
    HltFeature* feature = new HltFeatureT<T>(obj, n_objs);
    feature->setName(name);
    tmp.push_back(feature);
    m_hltFeatureDefs.insert(make_pair(lsid, tmp));
  } else {
    HltFeature* feature = new HltFeatureT<T>(obj, n_objs);
    feature->setName(name);
    p->second.push_back(feature);
  }
}

template<class T>
void HLTObjectsInRoI::defineL1RoI(const T* &obj, 
				  const std::string& name, int n_objs) {
  int level = 2;
  int isig = 0;
  LevelSigId lsid(level, isig);
  std::map<LevelSigId, std::vector<HltFeature*> >::iterator p;
  if ( (p=m_hltFeatureDefs.find(lsid)) == m_hltFeatureDefs.end()) {
    std::vector<HltFeature*> tmp;
    HltFeature* feature = new L1FeatureT<T>(obj, n_objs);
    feature->setName(name);
    tmp.push_back(feature);
    m_hltFeatureDefs.insert(make_pair(lsid, tmp));
  } else {
    HltFeature* feature = new L1FeatureT<T>(obj, n_objs);
    feature->setName(name);
    p->second.push_back(feature);
  }
}


#endif // TRIGGERMENUANALYSIS_HLTOBJECTSINROI_H
