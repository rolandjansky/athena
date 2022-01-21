/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: TrigDecisionTool
 * @Package: TrigDecisionTool
 * @Class  : Combination
 *
 * @brief simple container to hold trigger chains
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - LAPP Annecy
 * 
***********************************************************************************/

#include "TrigDecisionTool/Combination.h"
#include "TrigDecisionTool/CacheGlobalMemory.h"
#include "TrigDecisionTool/FeatureCollectStandalone.h"


Trig::Combination::Combination(const std::vector<HLT::TriggerElement*>& tes, const Trig::CacheGlobalMemory* cgm)
  : m_cgm(cgm)
{
  m_tes.insert(m_tes.end(), tes.begin(), tes.end());
}


bool Trig::Combination::active() const {
  for (std::vector<const HLT::TriggerElement*>::const_iterator te = m_tes.begin(); te != m_tes.end(); ++te)
    if ( ! (*te)->getActiveState() )
      return false;
  return true;
}

HLT::TrigNavStructure* Trig::Combination::navigation() const
{
  return const_cast<HLT::TrigNavStructure*>(m_cgm->navigation());
}

const std::vector<Trig::TypelessFeature> Trig::Combination::typelessGet(HLT::class_id_type clid, const std::string& label, unsigned int condition, const std::string & /*teName*/) const {
  //we will query the navigation for all features of this CLID, starting from the TE's in the combination and going up the navigation recursively.
  std::vector<Trig::TypelessFeature> features;
  
  for(auto& te : m_tes){
    FeatureAccessImpl::typelessCollect(te, clid,features, label, condition, navigation()); 
  }
  return features;
}

MsgStream& operator<< ( MsgStream& m, const Trig::Combination& c ) {
   m << "TEs: " << c.size();
   for ( std::vector<const HLT::TriggerElement*>::const_iterator i = c.tes().begin();
	 i != c.tes().end(); ++i ) {
     m << " " << Trig::getTEName(**i);
   }
   return m;
}
