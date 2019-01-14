/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODBase/IParticle.h"
#include "FourMomUtils/xAODP4Helpers.h"

#include "TriggerMatchingTool/MatchingTool.h"
#include "TriggerMatchingTool/MatchResult.h"
#include "TriggerMatchingTool/MatchConfig.h"
#include "TriggerMatchingTool/MatchingImplementation.h"

typedef std::vector<std::vector<double> > cost_matrix_type;


namespace Trig {

  MatchingTool::MatchingTool(const std::string& name) : 
    asg::AsgTool(name),
    m_impl(new Trig::MatchingImplementation(*this)),
    m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool")
  {
    declareProperty( "TrigDecisionTool", m_trigDecTool);

    m_condition = TrigDefs::Physics;

#ifndef XAOD_STANDALONE
    auto props = getProperties();
    for( Property* prop : props ) {
      if( prop->name() != "OutputLevel" ) {
	continue;
      }
      prop->declareUpdateHandler( &MatchingTool::updateOutputLevel, this );
      break;
    }
#else
#endif

  }

#ifndef XAOD_STANDALONE
  void MatchingTool::updateOutputLevel(Property& /*p*/) {
    impl()->msg().setLevel(msgLevel()); //pass on our message level to the matchingimplementation
  }
#endif

  MatchingTool::~MatchingTool(){
    delete m_impl;
  }

  StatusCode MatchingTool::initialize() {
    ATH_CHECK( m_trigDecTool.retrieve() );
    return StatusCode::SUCCESS;
  }


  IParticlesByType MatchingTool::separateByType(const std::vector<const xAOD::IParticle*>& recoObjects){
    IParticlesByType type_separated;
    //first separate the different types based on the ::type() method of IParticle to get a map {type:[objects]}
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
    return type_separated;
  }
  
  bool MatchingTool::matchCombination(IParticlesByType& recoByType, Trig::Combination& comb, std::unique_ptr<MatchResult>& result, const IMatchConfig* mc){
    Trig::ComboMatch thiscombo;
    thiscombo.matched = true;
    for(auto& type_vec : recoByType){
      Trig::ComboMatch::SingleTypeMatch single_info;

      
      bool status =  matchSingleType(type_vec.second, comb, single_info, mc->matchThreshold(type_vec.first));
      
      ATH_MSG_DEBUG("type: " << type_vec.first << " status: " << status << " matched: " << single_info.matched);
      
      thiscombo.singlematch_by_type[type_vec.first] = single_info;
      thiscombo.matched = thiscombo.matched && (status && single_info.matched);
    }

    ATH_MSG_DEBUG("pushing a comb result with mathed status: " << thiscombo.matched);
    result->m_combresults.push_back(thiscombo);
    return thiscombo.matched;
  }
  
  bool MatchingTool::matchSingleType(const std::vector<const xAOD::IParticle*>& recoObjects, Trig::Combination& comb, ComboMatch::SingleTypeMatch& result, double threshold){
    auto recoType = recoObjects.at(0)->type();
    ATH_MSG_DEBUG("matching combination with " << comb.tes().size() << " TEs for reco type: " << recoType << " against threshold: " << threshold);
    
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
    
    auto iparticle_feats = comb.getIParticle(clid,container_typename, "", m_condition);
    result.features = iparticle_feats;
    
    
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
    
    auto status_assoc = impl()->matchDistanceMatrix(distance_matrix,threshold);
    result.matched = status_assoc.first;
    result.assignment = status_assoc.second;
    

    //record sum of costs
    for(size_t i = 0; i < result.assignment.size(); ++i){
      result.total_cost += distance_matrix[i][result.assignment[i]];
    }

    ATH_MSG_DEBUG("result: " << result.matched << "total cost of best match in this combo for type " << recoType << " is " << result.total_cost);
    
    return true;
  }
  
  bool MatchingTool::match(const xAOD::IParticle& recoObject, const std::string& chain, double matchThreshold) {
    std::vector<const xAOD::IParticle*> recoObjects(1,&recoObject);
    MatchConfig config({{recoObject.type(), matchThreshold}});
    return match_result(recoObjects,chain,&config)->isMatched();
  }

  bool MatchingTool::match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, const std::map<xAOD::Type::ObjectType, double>& thresholds) {
    MatchConfig config(thresholds);
    return match_result(recoObjects,chain,&config)->isMatched();
  }
  
  bool MatchingTool::match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, double matchThreshold) {
    //construct a MatchConfig Object 
    auto separatedReco = separateByType(recoObjects);
    std::map<xAOD::Type::ObjectType, double> thr;
    for(auto type_objs : separatedReco){
      thr[type_objs.first] = matchThreshold; //all types use same threshold
    }
    MatchConfig config(thr);
    return match_result(recoObjects,chain,&config)->isMatched();
  }

  
  bool MatchingTool::match(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain) {
    return match_result(recoObjects,chain)->isMatched();
  }
  
  //main matching method
  std::unique_ptr<IMatchResult> MatchingTool::match_result(const std::vector<const xAOD::IParticle*>& recoObjects, const std::string& chain, const IMatchConfig* mc) {
    std::unique_ptr<MatchResult> match_result(new MatchResult());

    const MatchConfig defaultconfig;
    
    if(!mc){
      //defaut config
      ATH_MSG_DEBUG("matching using default match configuration");
      mc = &defaultconfig;
    }
    
    match_result->m_original_recolist = recoObjects;
    
    ATH_MSG_DEBUG("matching " << recoObjects.size() << " reco objects to chain: " << chain );
    
    auto chainGroup = impl()->tdt()->getChainGroup(chain);
    bool decision = chainGroup->isPassed();
    ATH_MSG_DEBUG(chain << " is passed: " << decision);
    if(!decision) return std::unique_ptr<IMatchResult>();

    auto separatedReco = separateByType(recoObjects);
    match_result->m_recos_by_type = separatedReco;

    auto featureContainer = chainGroup->features();
    auto combinations = featureContainer.getCombinations();
    
    ATH_MSG_DEBUG("chain has: " << combinations.size() << " combos");
    
    bool result = false;
    for(auto& comb : combinations){

      bool combResult = matchCombination(separatedReco,comb,match_result,mc);
      ATH_MSG_DEBUG("matching result for this combination: " << combResult);
      result = result || combResult;
      if(result) break; //no need to continue if result is true
    }
    
    ATH_MSG_DEBUG("overall matching result: " << result);
    
    match_result->m_matched = result;
    return match_result;
  }

  void MatchingTool::setCondition(unsigned int condition) {
    m_condition = condition;
  }

  Trig::MatchingImplementation* MatchingTool::impl(){
    return m_impl;
  }

  double MatchingTool::IParticleMetric(const xAOD::IParticle* lhs, const xAOD::IParticle* rhs){
    return xAOD::P4Helpers::deltaR(lhs,rhs,false /*use pseudorapidity to avoid calling p4*/);//return lhs->p4().DeltaR(rhs->p4());
  }

  cost_matrix_type MatchingTool::distanceMatrix(const std::vector<const xAOD::IParticle*>& reco,
						const std::vector<const xAOD::IParticle*>& trigger){

    //make distance matrix indexing is matrix[reco_index][trig_index]
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



