// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_FeatureContainer_H
#define TRIGGER_DECISION_TOOL_FeatureContainer_H


/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 * @class  : TrigDecision
 *
 * @brief objects of this type hold "synchronized" lists of features
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch> - LAPP Annecy
 * @author Michael Begel   <Michael.Begel@cern.ch>  - BNL
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>    - UC Irvine - AGH-UST Krakow
 * @author Joerg Stelzer   <Joerg.Stelzer@cern.ch>  - DESY
 *
 ***********************************************************************************/
#include "boost/foreach.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/type_traits/is_base_of.hpp"

#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureCollectStandalone.h"
#include "TrigDecisionTool/TypelessFeature.h"

#include "TrigSteeringEvent/TrigPassFlags.h"

#include "TrigNavStructure/TrigNavStructure.h"



#include <vector>
#include <map>

namespace Trig {

  

  class FeatureContainer
  {
  public:
      
    typedef std::vector<Combination>::const_iterator combination_const_iterator;
      
    /// constructors, destructor
    FeatureContainer(const CacheGlobalMemory* cgm = 0): m_combinations_unique(true), m_cgm(cgm) {}
    
    /**
     * @brief returns flattened vector of Features of given type
     * This method is in fact sullied by 3 arguments. 
     * One of them is type, another is object label and another is eventually TE name.
     *
     * @param label is label assigned to feature at HLT algorithm
     * @param teName is name of TE to which the object has to be attached (default is 0 and would suffice for most of applications)
     **/
    template<class T> const std::vector<Trig::Feature<T> > get(const std::string& label = "", unsigned int condition = TrigDefs::Physics,
							       const std::string& teName = "") const;
     

    std::vector<Trig::Feature<xAOD::IParticle> > getIParticle(HLT::class_id_type clid, const std::string& container_name, const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string & teName = "" ) const{
      auto features = typelessGet(clid,label,condition,teName);
      return FeatureAccessImpl::typedGet<xAOD::IParticle,xAOD::IParticleContainer,xAOD::IParticleContainer>(features,
                                                                                                           navigation(),
                                                                                                           m_cgm->store(),
                                                                                                           container_name); 
    }

    template<typename CONTAINER> using ELEMENT_OF = typename CONTAINER::base_value_type;

    template<typename CONTAINER>
    std::vector<Feature<ELEMENT_OF<CONTAINER> > > elementFeature(const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const{
      auto features = typelessGet(ClassID_traits<ELEMENT_OF<CONTAINER> >::ID(),label,condition,teName);
      return FeatureAccessImpl::typedGet<ELEMENT_OF<CONTAINER>,ELEMENT_OF<CONTAINER>,CONTAINER>(features,navigation(),m_cgm->store());
    }
    
    template<typename CONTAINER>
    std::vector<Feature<CONTAINER> > containerFeature(const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const{
      auto features = typelessGet(ClassID_traits<CONTAINER>::ID(),label,condition,teName);
      return FeatureAccessImpl::typedGet<CONTAINER,CONTAINER,CONTAINER>(features,navigation(),m_cgm->store());
    }
    
    template<typename CONTAINER>
    std::vector<Feature<ELEMENT_OF<CONTAINER> > > flattenedContainerFeature(const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const{
      auto features = typelessGet(ClassID_traits<CONTAINER>::ID(),label,condition,teName);
      return FeatureAccessImpl::typedGet<ELEMENT_OF<CONTAINER>,CONTAINER,CONTAINER>(features,navigation(),m_cgm->store());
    }


    /**
     * @brief typeless version of get
     * @param clid must be pointing directly to stored type (no flattening supported)
     */
    const std::vector<Trig::TypelessFeature> typelessGet(HLT::class_id_type clid, const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const;


    /**
     * @brief access to TrigPassFlags for given trigger object or container in given TE
     *
     * @param f is the Feature that holds the trigger object (container)
     * @param flagsLabel is the label of the TrigPassFlags object
     **/
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
    template<class T> TrigPassFlags getFlags(const Trig::Feature<T> & f, const std::string & flagsLabel = "") const;
#endif
      
    /**
     * @brief gives back reference to combinations collected though append
     **/
    const std::vector<Trig::Combination>& getCombinations() const;
      
     
    /**
     * @brief add new combination to the container checking for overlap
     **/    
    bool addWithChecking(const Combination& newComb);
    void append(const FeatureContainer& other);

  private:
    const Trig::CacheGlobalMemory* cgm() const { return m_cgm; }

    HLT::TrigNavStructure* navigation() const;

    // data
    mutable std::vector<Trig::Combination> m_combinations;
    std::vector<Trig::Combination>         m_nonunique_combinations;
    mutable bool                           m_combinations_unique;
    const Trig::CacheGlobalMemory*         m_cgm;

    // helper class to support ordered set of features
    //      class ordering_by_objects_attached {
    //      public:
    //        bool operator()(const HLT::TriggerElement* a, const HLT::TriggerElement* b);
    //      };

    // helper function for
    class ordering_by_objects_attached2 {
    public:
      template<class T>
      bool operator()(Feature<T> a, Feature<T> b) const;
      bool weakOrder(const HLT::TriggerElement* te_a, const HLT::TriggerElement* te_b, void* obj_a, void* obj_b) const;
    };

  };
  
} // End of namespace


template<class T> 
bool
Trig::FeatureContainer::ordering_by_objects_attached2::operator()(Feature<T> a, Feature<T> b) const {
  bool ret = weakOrder(a.te(), b.te(), (void*) a.cptr(), (void*) b.cptr());
  //std::cout << " ==> " << (ret?"TRUE":"FALSE") << std::endl;
  return ret;
}

template<class T> const std::vector<Trig::Feature<T> >
Trig::FeatureContainer::get(const std::string& label, unsigned int condition, const std::string& teName) const
{
  if ( condition != TrigDefs::Physics && condition != TrigDefs::alsoDeactivateTEs ) {
    throw std::runtime_error("Only two flags can be supplied to features");
  }

  // uniquify combinations first
  getCombinations();

  //std::cout << " in FC::get, after call to getCombinations(), now looping over them" << std::endl;

  std::set<Trig::Feature<T>, Trig::FeatureContainer::ordering_by_objects_attached2 > uniqnessHelper;

  
  BOOST_FOREACH(const Trig::Combination& comb, m_combinations ) {
    //std::cout << "   ---- combination has size " << comb.tes().size() << " tes[0] " << comb.tes()[0] << std::endl;
    //  for (std::vector<Trig::Combination>::const_iterator it = m_combinations.begin(); it != m_combinations.end(); ++it){
    if (condition == TrigDefs::Physics) {
      if (!comb.active()) continue;
    }
    std::vector<Trig::Feature<T> > features = comb.get<T>(label, condition, teName);

    //typedef std::vector< Trig::Feature<T> > FTV;
    //typename FTV::const_iterator fIt;
    //for (fIt = features.begin(); fIt != features.end(); ++fIt ) {
    BOOST_REVERSE_FOREACH(const Trig::Feature<T>& f,  features) {      
      //std::cout << "in FC::get, insertion of feature " << std::endl;
      uniqnessHelper.insert(f);

    }
  }

  //std::cout << " in FC::get, at the end: uniqnessHelper size = " << uniqnessHelper.size() << std::endl;

  return std::vector<Trig::Feature<T> >(uniqnessHelper.begin(), uniqnessHelper.end());

}



/**
 * @brief access to TrigPassFlags for given trigger object or container in given TE
 *
 * @param f is the Feature that holds the trigger object (container)
 * @param flagsLabel is the label of the TrigPassFlags object
 **/
#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
template<class T> TrigPassFlags
Trig::FeatureContainer::getFlags(const Trig::Feature<T> & f, const std::string & flagsLabel) const {

  TrigPassFlags pf;

  // get the trigPassFlag (only works if the flags are created in the same TE as the objects
  const TrigPassFlags* flags(0);
  const HLT::TriggerElement* sourceTE(0);
  std::string sourceLabel;

  auto fullnavi = dynamic_cast<HLT::NavigationCore*>(navigation());

  bool success = fullnavi->getRecentFeature(f.te(), flags, flagsLabel, sourceTE, sourceLabel);

  if(!success || !flags) // navigation failed or did not return a flags
    return pf;

    
  return FeatureAccessImpl::getFlags(f, flags, fullnavi);

}
#endif

#endif
