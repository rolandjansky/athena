/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/MatchingTool.h"
#include "TriggerMatchingTool/MatchingImplementation.h"
#include "xAODBase/IParticle.h"

#include "FourMomUtils/xAODP4Helpers.h"

namespace Trig {

MatchingTool::MatchingTool(const std::string& name) : 
  asg::AsgTool(name),
  m_impl(new Trig::MatchingImplementation(*this)),
  m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool"),
  m_matchingThreshold(0)
{
  declareProperty( "TrigDecisionTool", m_trigDecTool);

#ifndef XAOD_STANDALONE
//  declareProperty( "DefaultMatchingThreshold", m_matchingThreshold = 0.4)->declareUpdateHandler(&MatchingTool::updateThreshold, this );
   auto props = getProperties();
 	for( Property* prop : props ) {
      if( prop->name() != "OutputLevel" ) {
         continue;
      }
      prop->declareUpdateHandler( &MatchingTool::updateOutputLevel, this );
      break;
   }
#else
//   declareProperty( "DefaultMatchingThreshold", m_matchingThreshold = 0.4);
#endif

}

#ifndef XAOD_STANDALONE
void MatchingTool::updateOutputLevel(Property& p) {
   this->msg_update_handler(p); //calls original handler
   impl()->msg().setLevel(AthMessaging::msg().level()); //pass on our message level to the matchingimplementation
}

void MatchingTool::updateThreshold(Property& /*p*/) {
   ATH_MSG_DEBUG("Matching Threshold is updated to:" << m_matchingThreshold);
   impl()->setThreshold( m_matchingThreshold );
}
#endif

MatchingTool::~MatchingTool(){
  delete m_impl;
}

StatusCode MatchingTool::initialize() {
  impl()->setThreshold( m_matchingThreshold );

  ATH_CHECK( m_trigDecTool.retrieve() );

  return StatusCode::SUCCESS;
}

bool MatchingTool::matchCombination(const std::vector<const xAOD::IParticle*>& recoObjects, Trig::Combination& comb){
  std::map<xAOD::Type::ObjectType,std::vector<const xAOD::IParticle*> > type_separated;
  
  for(const auto& obj : recoObjects){
    if(type_separated.find(obj->type()) == type_separated.end()){
      std::vector<const xAOD::IParticle*> typevec;
      type_separated[obj->type()] = typevec;
    }
    type_separated[obj->type()].push_back(obj);
  }
  
  ATH_MSG_DEBUG("found: " << type_separated.size() << " unique objects types to match");
  for(auto& type_vec : type_separated){
    ATH_MSG_DEBUG("type: " << type_vec.first << "(" << type_vec.second.size() << " elements)");
  }
  
  bool overall_status = true;
  std::map<xAOD::Type::ObjectType,bool> status;
  for(auto& type_vec : type_separated){
    auto single_status =  matchSingleType(type_vec.second, comb);
    ATH_MSG_DEBUG("type: " << type_vec.first << " status: " << single_status);
    status[type_vec.first] = single_status;
    overall_status = overall_status && single_status;
  }
  
  return overall_status;
}

bool MatchingTool::matchSingleType(const std::vector<const xAOD::IParticle*>& recoObjects, Trig::Combination& comb){
  ATH_MSG_DEBUG("matching combination with " << comb.tes().size() << " TEs");
  
  auto recoType = recoObjects.at(0)->type();
  ATH_MSG_DEBUG("reco type is " << recoType);

  HLT::class_id_type clid = 0;
  std::string container_typename("");

  if(m_typeMap.isKnown(recoType)){
    auto clid_container = m_typeMap.get(recoType);
    clid = clid_container.first;
    container_typename = clid_container.second;
    ATH_MSG_DEBUG("getting trigger features (clid: " << clid << " and type: " << container_typename << ")");
  }
  else{
    ATH_MSG_WARNING("could not find corresponding trigger type, can't match");
    return false;
  }
  
  auto iparticle_feats = comb.getIParticle(clid,container_typename);
  
  ATH_MSG_DEBUG("found: " << iparticle_feats.size() << " xAOD::IParticle");
  
  for(auto& feat : iparticle_feats){
    ATH_MSG_DEBUG(" ==> pt: " << feat.cptr()->pt() << " and eta: " << feat.cptr()->eta() << " address: " << feat.cptr());
  }

  if(recoObjects.size() > iparticle_feats.size()){
    ATH_MSG_WARNING("more reco objects (" << recoObjects.size() << ") than trigger object (" << iparticle_feats.size() << "). no hope of matching!");
    return false;
  }
  
  ATH_MSG_DEBUG("now matching: " << recoObjects.size() << " reco objects to " << iparticle_feats.size() << " trigger objects");
  
  std::vector<const xAOD::IParticle*> trigObjects;
  for(auto& feat : iparticle_feats){trigObjects.push_back(feat.cptr());}

  auto distance_matrix = distanceMatrix(recoObjects,trigObjects);

  ATH_MSG_DEBUG("made distance matrix");

  bool match_result = impl()->matchDistanceMatrix(distance_matrix);

  ATH_MSG_DEBUG("got matching result: " << match_result);
  
  return match_result;
}

  bool MatchingTool::match(const xAOD::IParticle& recoObject, const std::string& chain, double matchThreshold, bool rerun) {
   impl()->setThreshold( matchThreshold );
   std::vector<const xAOD::IParticle*> recoObjects(1,&recoObject);
   bool out = match(recoObjects, chain, rerun);
   impl()->setThreshold( m_matchingThreshold );
   return out;
}

  bool MatchingTool::match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, double matchThreshold, bool rerun) {
   impl()->setThreshold( matchThreshold );
   bool out = match(recoObjects, chain, rerun);
   impl()->setThreshold( m_matchingThreshold );
   return out;
}

  bool MatchingTool::match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, bool rerun){
  ATH_MSG_DEBUG("matching " << recoObjects.size() << " reco objects to chain: " << chain );

  auto chainGroup = impl()->tdt()->getChainGroup(chain);
  bool decision;
  if (!rerun)
    decision = chainGroup->isPassed();
  else
    decision = chainGroup->isPassed(TrigDefs::Physics | TrigDefs::allowResurrectedDecision);
  ATH_MSG_DEBUG(chain << " is passed: " << decision);
  if(!decision) return false;
  
  auto featureContainer = chainGroup->features();
  auto combinations = featureContainer.getCombinations();
  
  ATH_MSG_DEBUG("chain has: " << combinations.size() << " combos");
  
  bool result = false;
  for(auto& comb : combinations){
    bool combResult = matchCombination(recoObjects,comb);
    ATH_MSG_DEBUG("matching result for this combination: " << combResult);
    result = result || combResult;
    if(result) break; //no need to continue if result is true
  }

  ATH_MSG_DEBUG("overall matching result: " << result);

  return result;
}

Trig::MatchingImplementation* MatchingTool::impl(){
  return m_impl;
}

double MatchingTool::IParticleMetric(const xAOD::IParticle* lhs, const xAOD::IParticle* rhs){
  return xAOD::P4Helpers::deltaR(lhs,rhs,false /*use pseudorapidity to avoid calling p4*/);//return lhs->p4().DeltaR(rhs->p4());
}

std::vector<std::vector<double> > MatchingTool::distanceMatrix(const std::vector<const xAOD::IParticle*>& reco,
							       const std::vector<const xAOD::IParticle*>& trigger){
  std::vector<std::vector<double> > rows;
  for(const auto& rec : reco){
    std::vector<double> distances_to_rec;
    for(const auto& trig : trigger){
      distances_to_rec.push_back(IParticleMetric(rec,trig));
    }
    rows.push_back(distances_to_rec);
  }
  return rows;
}

} //Trig namespace



