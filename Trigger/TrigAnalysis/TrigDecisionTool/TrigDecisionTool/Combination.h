// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_DECISION_TOOL_Combination_H
#define TRIGGER_DECISION_TOOL_Combination_H

/**********************************************************************************
 * @Project:
 * @Package: TrigDecision
 * @Class  : TrigDecision
 *
 * @brief combination of TEs storage
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 *
 ***********************************************************************************/


#include "TrigDecisionTool/Feature.h"
#include "TrigDecisionTool/Conditions.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
//#include "TrigDecisionTool/TDTUtilities.h"

//#include "TrigNavigation/TriggerElement.h"
//#include "TrigNavigation/NavigationCore.h"
#include "boost/foreach.hpp"
#include <vector>


namespace Trig {

  class CacheGlobalMemory;

  /**
   * @class Combination is a connector between chains and object
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
     * @brief copy constructor
     **/
    Combination(const Combination& other) :  m_tes(other.m_tes), m_cgm(other.m_cgm) { }


    /**
     * @brief Method used to get objects. 
     * This is main functionality of combination class.
     * @param label the keyword with which the objects were labeled in HLT algorithms (if none given most recent is returned)
     * @param condition switch wether to return only objects from passed RoIs (when Physics) or also from failed (alsoDeactivateTEs)
     * @param teName to get features from TEs of specific TEs (it is to be used in combination with switch alsoDeactivateTEs or composite triggers) super-expert usecase
     **/
     template<class T> const std::vector<Trig::Feature<T> > get(const std::string& label = "", unsigned int condition=TrigDefs::Physics, const std::string& teName = "") const;


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

    // helper functionality for collect using traits class to determine how to access
    // the navigation

    HLT::NavigationCore* navigation() const;

    std::vector<const HLT::TriggerElement*> m_tes;
    const Trig::CacheGlobalMemory* m_cgm;
  };
} // EOF namespace Trig




template <class T>
const std::vector<Trig::Feature<T> > Trig::Combination::get(const std::string& label, unsigned int condition, const std::string& teName) const
{
  std::vector<Feature<T> > data;
  BOOST_FOREACH( const HLT::TriggerElement* te, m_tes )
    FeatureAccessImpl::collect<T>(te, data, label, condition, teName, navigation());
  BOOST_FOREACH( Feature<T>& f, data ) {
    if ( f.owned() ) {
      m_cgm->deleteAtTheEndOfEvent( const_cast<T*>( f.cptr() ) );
    }
  }  
  return data;
}



MsgStream& operator<< ( MsgStream& m, const Trig::Combination& c );


#endif
