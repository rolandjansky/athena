/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMatchingTool/MatchResult.h"

namespace Trig{

  MatchResult::MatchResult() : m_matched(false) {

  }
  
  MatchResult::~MatchResult() {
  }
  
  bool MatchResult::isMatched() {
    return m_matched;
  }

  std::vector<const xAOD::IParticle*> alignMatchedObjects(const std::vector<const xAOD::IParticle*>& recoObjects, const ComboMatch& result){
    std::vector<const xAOD::IParticle*> aligned;
    std::map<xAOD::Type::ObjectType,unsigned int> indices_by_type; //reco indices by type so that we can get the matched objects
    for(const auto& obj : recoObjects){
      if(indices_by_type.find(obj->type()) == indices_by_type.end()){
	indices_by_type[obj->type()] = 0;
      }
      else {
	auto current_index = indices_by_type[obj->type()];
	indices_by_type[obj->type()] = current_index + 1;
      }
      
      auto single_match = result.singlematch_by_type.at(obj->type());
      auto assignment = single_match.assignment;
      auto recidx = indices_by_type.at(obj->type());
      int assigned_index = single_match.assignment[recidx];
      const xAOD::IParticle* trigobj = single_match.features[assigned_index].cptr();
      aligned.push_back(trigobj);
    }
    return aligned;
  }
  
  std::vector<const xAOD::IParticle*> MatchResult::bestMatchedObjects(){
    double bestcost = 0;
    const ComboMatch* bestcombo = 0;
    int i = 0;
    int bestindex = -1;
    for (const auto combresult : m_combresults){
      double combocost = 0;
      for (auto type_objs : m_recos_by_type){
	const auto& single_match = combresult.singlematch_by_type.at(type_objs.first);
	combocost += single_match.total_cost;
      }
      if( combresult.matched and (bestindex < 0 || (combocost < bestcost) )){
	bestindex = i;
      }
      i++;
    }
    if(bestindex >= 0){
      return alignMatchedObjects(original_recolist,m_combresults.at(bestindex));
    }
    
    std::cout << "TriggerMatchingTool ERROR. Best matching combination was not found. This should not happen. Is the result matched (isMatched())? " << std::endl;
    return std::vector<const xAOD::IParticle*>();
  }
}
