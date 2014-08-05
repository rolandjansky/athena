// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_DecisionAccess_H
#define TRIGGER_DECISION_TOOL_DecisionAccess_H

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : DecisionAccess
 *
 * @brief access to information about the trigger decision
 *
 * @author Michael Begel   <michael.begel@cern.ch>  - Brookhaven National Laboratory
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch> - LAPP Annecy
 * @author Tomasz Bold     <Tomasz.Bold@cern.ch>    - UC Irvine - AGH Krakow
 *
 ***********************************************************************************/
#include <string>
#include <boost/algorithm/string/predicate.hpp>


#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"
#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"


#include "GaudiKernel/MsgStream.h"
//#include "TrigNavigation/NavigationCore.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigNavigation/Holder.h"
#include "TrigNavigation/TrigFeatureLink.h"
#include "TrigDecisionEvent/TrigDecision.h"
#include "TrigDecisionTool/Logger.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/ChainGroup.h"
//#include "TrigDecisionTool/ClassTraits.h"

#include "TrigStorageDefinitions/EDM_TypeInfoMethods.h"


namespace HLT {
  class Chain;
}
namespace TrigConf {
  class HLTChain;
  class HLTTriggerElement;
}

class LVL1_ROI;

namespace Trig {

  class CacheGlobalMemory;
   
  class DecisionAccess : public virtual Logger {
  public:      

    DecisionAccess();
    virtual ~DecisionAccess();

    StatusCode unpackDecision(const TrigDec::TrigDecision* dec);

    /**
     * @brief true if given group of chains passed
     * @see Trig::ChainGroup
     * @see TrigDefs::Conditions
     **/ 
    bool isPassed(const Trig::ChainGroup* chaingroup, 
                  unsigned int condition = TrigDefs::Physics) const;
    /**
     * @brief true if given chain passed
     * @see Trig::ChainGroup
     * @see TrigDefs::Conditions
     **/ 
    bool isPassed(const std::string& chain, 
                  unsigned int condition) const;

    bool isPassed(const std::string& chain) const;
    /**
     * Return expert-level information about the various trigger flags as a
     * bit mask. Note: this does not accept a conditions flag -- the
     * prescription is always enforceLogicalFlow. The mask can be comprehended
     * using the following constants from TrigDefs::
     * EF_passedRaw
     * EF_passThrough
     * EF_prescaled
     * EF_resurrected
     * L2_passedRaw
     * L2_passThrough
     * L2_prescaled
     * L2_resurrected
     * L1_isPassedAfterPrescale
     * L1_isPassedBeforePrescale
     * L1_isPassedAfterVeto
     **/
    unsigned int isPassedBits(const Trig::ChainGroup* chaingroup) const;
    unsigned int isPassedBits(const std::string& chain) const;
    
    /**
     * @brief get the bunch group code
     * bit X=1: BG X fired (X=0..7)
     * use: bool BGXfired = BGCode & (0x1<<X)
     */
    char getBGCode() const;
    
    /**
     * @brief returns all features related to given chain group
     **/
    const FeatureContainer features(const ChainGroup* group, 
                                    unsigned int condition = TrigDefs::Physics) const;

    /**
     * @brief returns features related to given chain
     **/
    const FeatureContainer features(const std::string& chainName = "EF_.*", 
                                    unsigned int condition = TrigDefs::Physics) const;

    /**
     * @brief gives back feature matching (by seeding relation)
     * @parm te - is trigger element to start with, not that thanks to conversion operators
     * Feature object can be given here as well
     **/
    template<class T> 
    const Feature<T> ancestor(const HLT::TriggerElement* te, std::string label = "") const;

    /**
     * @brief gives back features matching (by seeding relation) - will also flatten
     *        containers if necessary
     * @parm te - is trigger element to start with, not that thanks to conversion operators
     * Feature object can be given here as well
     **/
    template<class T>
    const std::vector<Trig::Feature<T> > ancestors(const HLT::TriggerElement* te, std::string label = "", unsigned int condition=TrigDefs::Physics, const std::string& teName = "") const;


    template<class T>
    const T* featureLink2Object(const TrigFeatureLink& ) const;

    template<class T>
    ElementLink<typename Object2Container<T>::type> featureLink2ElementLink(const TrigFeatureLink& ) const; 

  protected:

    friend class Combination;    


  private:

    virtual Trig::CacheGlobalMemory* cgm() const =0; 

    //    template<class T> 
    //    const T* getL1RoI(const std::vector<T>& rois, unsigned int id) const;


    //unsigned int initRoIWord(const HLT::TriggerElement* te) const;

  };


//   template<> 
//   const Feature<Muon_ROI> DecisionAccess::ancestor<Muon_ROI>(const HLT::TriggerElement* te, std::string /*label*/ ) const ;

//   template<> 
//   const Feature<Jet_ROI> DecisionAccess::ancestor<Jet_ROI>(const HLT::TriggerElement* te, std::string /*label*/ ) const ;

//   template<> 
//   const Feature<EmTau_ROI> DecisionAccess::ancestor<EmTau_ROI>(const HLT::TriggerElement* te, std::string /*label*/ ) const ;

} // End of namespace







template<class T> 
const Trig::Feature<T> Trig::DecisionAccess::ancestor(const HLT::TriggerElement* te, std::string label) const {
  Trig::Feature<T> f;
  std::vector<Trig::Feature<T> > data;
  FeatureAccessImpl::collect<T>(te, data, label, TrigDefs::alsoDeactivateTEs, "", cgm()->navigation_feature());

  BOOST_FOREACH( Feature<T>& f, data ) {
    if ( f.owned() ) {
      cgm()->deleteAtTheEndOfEvent( const_cast<T*>( f.cptr() ) );
    }
  }

  if (data.size() == 1)
    f = data[0];  
  return f;
}


template<class T>
const std::vector<Trig::Feature<T> > Trig::DecisionAccess::ancestors(const HLT::TriggerElement* te, std::string label, unsigned int condition, const std::string& teName) const {
  std::vector<Trig::Feature<T> > data;
  FeatureAccessImpl::collect<T>(te, data, label, condition, teName, cgm()->navigation_feature());
  BOOST_FOREACH( Feature<T>& f, data ) {
    if ( f.owned() ) {
      cgm()->deleteAtTheEndOfEvent(const_cast<T*>( f.cptr() ));
    }
  }
  return data;
}


template<class T>
const T* Trig::DecisionAccess::featureLink2Object(const TrigFeatureLink& flink ) const {
  return const_cast<HLT::NavigationCore*>(cgm()->navigation())->featureLink2Object<T>(flink);
}


namespace DecisionAccessDetails {

  template<class F1, class F2> 
  struct getHolderImp {
    static  const HLTNavDetails::IHolder* do_it(HLT::NavigationCore* nav, const TrigFeatureLink& flink) {    
      if ( ClassID_traits<F1>::ID() == flink.clid() )
	return nav->getHolder<F1>(flink.subTypeIndex());
      if ( ClassID_traits<F2>::ID() == flink.clid() )
	return nav->getHolder<F2>(flink.subTypeIndex());
      return 0;
    }
  };
  
  
  template<class F1> 
  struct getHolderImp< F1, HLT::TypeInformation::nil> {
    static  const HLTNavDetails::IHolder* do_it(HLT::NavigationCore* nav, const TrigFeatureLink& flink) {    
      if ( ClassID_traits<F1>::ID() == flink.clid() )
	return nav->getHolder<F1>(flink.subTypeIndex());    
      return 0;
    }
  };
  
  template<class list>
  struct first {
    typedef typename list::first type;
  };
  
  template<>
  struct first<HLT::TypeInformation::nil> {
    typedef typename HLT::TypeInformation::nil type;
  };
}

template<class T>
ElementLink<typename Object2Container<T>::type>
Trig::DecisionAccess::featureLink2ElementLink(const TrigFeatureLink& flink ) const {
  
  using namespace HLT::TypeInformation;
  using namespace DecisionAccessDetails;
  typedef typename Object2Container<T>::type container;
  typedef typename Object2Features<T>::type list_of_possible_features;

  typedef typename first<list_of_possible_features>::type feature0;
  typedef typename first<typename list_of_possible_features::rest>::type feature1;

  // I expected this to work actually this to work
  //  typedef typename at<list_of_possible_features, 0>::type feature0;
  //  typedef typename at<list_of_possible_features, 1>::type feature1;
  // but it failed recursing to -99
    
  
  // two cases need to be sorted out, because object can come from the feasures exisinging as a single (2) or features which are collections (2)
  // we distinguish the two cases by looking at the feature at the index 1, it is the "nil" type then we have  (1) otherwise (2)
  const HLTNavDetails::IHolder* holder = getHolderImp<feature0, feature1>
    ::do_it( const_cast<HLT::NavigationCore*>(cgm()->navigation()), flink);  

  // it may well be 0 of CLIDs are not matching
  if ( holder != 0 ) { 
    ElementLink<container> el( holder->key(), flink.index() );
    return el;
  }
  return ElementLink<container>();
}


#endif
