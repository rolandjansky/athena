// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_Combination_H
#define TRIGGER_DECISION_TOOL_Combination_H

/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 * @class  : TrigDecision
 *
 * @brief combination of TEs storage
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 *
 ***********************************************************************************/


#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/TypelessFeature.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigNavStructure/TrigNavStructure.h"
#include "boost/foreach.hpp"
#include <vector>

#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
#include "TrigDecisionTool/FeatureCollectAthena.h"
#endif

#include "TrigDecisionTool/FeatureCollectStandalone.h"

class HltNaviChecker;

namespace Trig {

  class CacheGlobalMemory;

  /**
   * @class Combination
   * is a connector between chains and object
   * It store single combination of trigger elements. 
   * Thanks to templates methods get()  it is able to 
   * invoke features retrieval and return actual objects (wrapped into Feature class).
   * @warning It only looks simple.
   **/
  class Combination 
  {  
  public:

    Combination();

    /**
     * @brief normal constructor
     **/
    Combination(const std::vector<HLT::TriggerElement*>& tes, const Trig::CacheGlobalMemory* cgm);


    /**
     * @brief Typeless version of get method
     * @param clid must be clid of type as stored. No flattening supported
     */

    const std::vector<Trig::TypelessFeature> typelessGet(HLT::class_id_type clid, const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string & teName = "" ) const;

    /**
     * @brief Method used to get objects. 
     * This is main functionality of combination class.
     * @param label the keyword with which the objects were labeled in HLT algorithms (if none given most recent is returned)
     * @param condition switch wether to return only objects from passed RoIs (when Physics) or also from failed (alsoDeactivateTEs)
     * @param teName to get features from TEs of specific TEs (it is to be used in combination with switch alsoDeactivateTEs or composite triggers) super-expert usecase
     **/
     template<class T>
     const std::vector<Trig::Feature<T> > get(const std::string& label = "", unsigned int condition=TrigDefs::Physics, const std::string& teName = "") const;

    
    template<typename CONTAINER> using ELEMENT_OF = typename CONTAINER::base_value_type;

    /**
     * @brief Experimental flattened get for IParticle types
     * @param clid must be clid of type as stored. No flattening supported
     */

    std::vector<Trig::Feature<xAOD::IParticle> > getIParticle(HLT::class_id_type clid, const std::string& container_name, const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string & teName = "" ) const{
      auto features = typelessGet(clid,label,condition,teName);
      return FeatureAccessImpl::typedGet<xAOD::IParticle,xAOD::IParticleContainer,xAOD::IParticleContainer>(features,
													    navigation(),
													    m_cgm->store(),
													    container_name); 
    }

    /**
     * @brief get element type features (those attached to TE as single instance of a trigger type)
     */
    template<typename CONTAINER>
    std::vector<Feature<ELEMENT_OF<CONTAINER> > > elementFeature(const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const{
      auto features = typelessGet(ClassID_traits<ELEMENT_OF<CONTAINER> >::ID(),label,condition,teName);
      return FeatureAccessImpl::typedGet<ELEMENT_OF<CONTAINER>,ELEMENT_OF<CONTAINER>,CONTAINER>(features,navigation(),m_cgm->store());
    }

    /**
     * @brief get container type features (those attached to TE as container of a trigger type)
     */
    template<typename CONTAINER>
    std::vector<Feature<CONTAINER> > containerFeature(const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const{
      auto features = typelessGet(ClassID_traits<CONTAINER>::ID(),label,condition,teName);
      return FeatureAccessImpl::typedGet<CONTAINER,CONTAINER,CONTAINER>(features,navigation(),m_cgm->store());
    }

    /**
     * @brief get container type features flattened as Features (those attached to TE as container of a trigger type)
     */
    template<typename CONTAINER>
    std::vector<Feature<ELEMENT_OF<CONTAINER> > > flattenedContainerFeature(const std::string& label = "", unsigned int condition = TrigDefs::Physics, const std::string& teName = "") const{
      auto features = typelessGet(ClassID_traits<CONTAINER>::ID(),label,condition,teName);
      return FeatureAccessImpl::typedGet<ELEMENT_OF<CONTAINER>,CONTAINER,CONTAINER>(features,navigation(),m_cgm->store());
    }

    /**
     * @brief Size of combination.
     **/
    unsigned int size() const { return m_tes.size(); }

    /**
     * @brief Returns the state of the combination i.e. if all TEs are passed by respective hypothesis.
     * This method can be used to differentiate between passed/not-passed objects
     **/
    bool active() const;



    /**
     * @brief trigger elements in the combination 
     * can be used directly with ancestor method
     **/
    const std::vector<const HLT::TriggerElement*>& tes() const {return m_tes; }

    /**
     * @brief Method used to get objects. 
     * This is called for each TE in the combination     
     **/      
    template<class T> 
    void collect(const HLT::TriggerElement* te, std::vector<Trig::Feature<T> >& data, 
		 const std::string& label, unsigned int condition, const std::string& teName) const;

    bool operator==(const Combination& other) const;
    bool operator<(const Combination& other) const;
    

  private:
    friend class ::HltNaviChecker;

    // helper functionality for collect using traits class to determine how to access
    // the navigation

    HLT::TrigNavStructure* navigation() const;

    std::vector<const HLT::TriggerElement*> m_tes;
    const Trig::CacheGlobalMemory* m_cgm;
  };
} // EOF namespace Trig




template <class T>
const std::vector<Trig::Feature<T> > Trig::Combination::get(const std::string& label, unsigned int condition, const std::string& teName) const
{
  std::vector<Feature<T> > data;

#if defined(ASGTOOL_ATHENA) && !defined(XAOD_ANALYSIS)
  BOOST_FOREACH( const HLT::TriggerElement* te, m_tes )
    FeatureAccessImpl::collect<T>(te, data, label, condition, teName, navigation());
  BOOST_FOREACH( Feature<T>& f, data ) {
    if ( f.owned() ) {
      m_cgm->deleteAtTheEndOfEvent( const_cast<T*>( f.cptr() ) );
    }
  }  
#else
  std::cerr << "WARNING: Trig::Combination::get is only available in Athena proper" << std::endl;
  (void)label;
  (void)condition;
  (void)teName;
#endif
  return data;
}



MsgStream& operator<< ( MsgStream& m, const Trig::Combination& c );


#endif
