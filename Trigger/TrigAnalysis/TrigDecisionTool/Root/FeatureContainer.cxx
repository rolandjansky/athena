/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : FeatureContainer
 *
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 * 
***********************************************************************************/

#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Combination.h"
    

void Trig::FeatureContainer::addWithChecking(const Combination& newComb)
{
  /* Keep unique combinations and preserve insertion order to guarantee
     reproducibility. We achieve this by an additional set (faster than doing
     a linear search in the vector each time). */
  if (m_combinations_unique.insert(newComb).second) { // true if element was inserted (not known yet)
    m_combinations.push_back(newComb);
  }
}

void Trig::FeatureContainer::append(const FeatureContainer& other)
{  
  for(const Trig::Combination& comb : other.m_combinations)
    addWithChecking(comb);
}


HLT::TrigNavStructure*
Trig::FeatureContainer::navigation() const {
  return const_cast<HLT::TrigNavStructure*>(m_cgm->navigation());
}



bool 
Trig::FeatureContainer::ordering_by_objects_attached::weakOrder(const HLT::TriggerElement* a, const HLT::TriggerElement* b,
                                                                const void* obj_a, const void* obj_b) const {
  // returning False -> no insertion into set
  // returning True -> insertion into set
  // each pair of candidates is tested twice (if test returns False twice, then no insertion into set)

  using namespace HLT;
  if (a==b && obj_a==obj_b) {
    return false; // if TE and object pointers are the same, then we don't insert the second cand at all
  }

  if(a==b && obj_a!=obj_b) {
    // if different objects but the same TE, then we always insert, so the order in the set is the insertion order
    // this guaranties reproducibility only if the order of the objects in the TE is reproducible
    return true;
  }

  // if the TEs are different we follow the old procedure which does guaranty reproducibily (the order of features is always the same)
  const TriggerElement::FeatureVec& a_features = a->getFeatureAccessHelpers();
  const TriggerElement::FeatureVec& b_features = b->getFeatureAccessHelpers();

  if ( a_features.size() !=  b_features.size() )
     return a_features.size() < b_features.size();
  
  for ( unsigned f = 0, sz = a_features.size(); f < sz; ++f ) {
     const TriggerElement::FeatureAccessHelper& a_f = a_features[f];
     const TriggerElement::FeatureAccessHelper& b_f = b_features[f];
     if ( a_f.getCLID() == b_f.getCLID()) {
        if ( a_f.getIndex() == b_f.getIndex()) continue;
        return a_f.getIndex() < b_f.getIndex();
     }
     return a_f.getCLID() < b_f.getCLID();
  }
  return false;
}

//this is a helper method to weakly order feature access helpers.
class order_by_clid_and_index {
public:
  bool operator()(const Trig::TypelessFeature& a,const Trig::TypelessFeature& b) const {
    if ( a.accessHelper().getCLID() == b.accessHelper().getCLID()) {
      return a.accessHelper().getIndex() < b.accessHelper().getIndex();
    }
    return a.accessHelper().getCLID() < b.accessHelper().getCLID();
  }
    };


const std::vector<Trig::TypelessFeature> Trig::FeatureContainer::typelessGet(HLT::class_id_type clid, const std::string& label, unsigned int condition, const std::string& teName) const {
  if ( condition != TrigDefs::Physics && condition != TrigDefs::alsoDeactivateTEs ) {
    throw std::runtime_error("Only two flags can be supplied to features");
  }

  std::set<Trig::TypelessFeature,::order_by_clid_and_index> uniqnessHelper;
  for(auto& comb : m_combinations ) {
    if (condition == TrigDefs::Physics) {
      if (!comb.active()) continue;
    }

    std::vector<Trig::TypelessFeature> features = comb.typelessGet(clid, label, condition, teName);
    for(auto& f : features) {      
      uniqnessHelper.insert(f);
    }
  }
  return std::vector<Trig::TypelessFeature>(uniqnessHelper.begin(), uniqnessHelper.end());
}
