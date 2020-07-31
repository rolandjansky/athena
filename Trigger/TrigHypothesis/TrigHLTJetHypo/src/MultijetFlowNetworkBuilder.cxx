/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./MultijetFlowNetworkBuilder.h"
#include <map>
#include <algorithm>
#include <sstream>

MultijetFlowNetworkBuilder::MultijetFlowNetworkBuilder(ConditionsMT conditions):
  FlowNetworkBuilderBase(std::move(conditions)){
}

std::optional<std::vector<std::shared_ptr<FlowEdge>>>
MultijetFlowNetworkBuilder::make_flowEdges(const HypoJetGroupCIter& groups_b,
                                           const HypoJetGroupCIter& groups_e, 
                                           const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                           int& V,
                                           std::map<int, pHypoJet>& nodeToJet) const{


  auto matches = conditionGroupMatches(groups_b, groups_e, collector);
  if(!matches.has_value()){
    // there is  a condition without any  matches.
    return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
  }

  // add edges from condition to matching job group
  auto icond{0};
  const auto jg_offset = m_conditions.size() + 1;
  
  auto edges = getSourceToConditionsEdges();
  
  // matches: std::optional<std::vector<std::vector<int>>>
  for(const auto& jg_nodes : *matches){
    // icond is m_conditons index
    double cap = m_conditions[icond]->capacity();

    auto  dest  = std::back_inserter(edges);
    std::transform(jg_nodes.begin(),
                   jg_nodes.end(),
                   dest, 
                   [icond, jg_offset, cap](const auto& g) {
                     return std::make_shared<FlowEdge>(icond+1,
                                                       g + jg_offset,
                                                       cap);
                   });
    ++icond;
  }
    
  //contains jets: jet - node for jets found in matching groups (nb type order)
  // note that more than one jet group will contain a jet. use this map
  // to avoid allocating more than one jet node to a jet.
  
  std::map<pHypoJet, int> jets; 

  std::size_t jnode{m_conditions.size() + (groups_e-groups_b)};

  // now add the jetgroup - jet edges
  std::size_t sinkCapacity{0};
  constexpr double unitCapacity = 1.0;


  // *matches: conditions index     jetGroup indcies
  //                0                jg1, jg2, jg2
  //                1                jg1, jg4, jg5
  //  etc
  // note: all conditions are present with non-zero lists of job groups
  // the entries are indicies into arrays and _not_ node numbers (add offsets
  // to get node numbers).

  //obtain a list of job groups without duplicits:

  std::set<int> u_jgroups;
  for(const auto& jgis: *matches){u_jgroups.insert(jgis.begin(), jgis.end());}

  // a jet may appear inmore than one matched job group. Ensure it recieves
  // one node number.
    
  for(const auto& jgi : u_jgroups){
    //jgi is a job group index, use it to retrieve the job group
    auto jgroup = *(groups_b + jgi);
      
    // loop over jets in group and identify group-jet edges
    for(const auto& j : jgroup){
      // j is a HypoJet*
        
      auto iter = jets.find(j);
      if (iter == jets.end()){
	// first time this jet is seen. add it to jets.
	jets[j] = ++jnode; 
	edges.push_back(std ::make_shared<FlowEdge>(jgi + jg_offset,
						    jnode,
						    unitCapacity));
	if(collector){
	  std::stringstream ss;
	  ss << "new edge " << jgi + jg_offset << "->" << jgi << '\n';
	  collector->collect("MultijetFlowNetworkBuilder", ss.str());
	}
      } else {
	// this jet has already been senn in a previous group
	auto jnode = (*iter).second;
	edges.push_back(std::make_shared<FlowEdge>(jgi + jg_offset,
						   jnode,
						   unitCapacity));
      }
    }
  }

  auto sourceCapacity = 0.;
  for(const auto& c : m_conditions){sourceCapacity += c->capacity();}
  sinkCapacity = jets.size();
  if(sinkCapacity < sourceCapacity){
    
    // no hope of matching
    if(collector){
      std::string msg =
        "source capacity > sink capacity: " +
        std::to_string(sourceCapacity) + " " +
        std::to_string(sinkCapacity);
      collector->collect("MultijetFlowNetworkBuilder", msg);
    }
    return std::optional<std::vector<std::shared_ptr<FlowEdge>>>();
  }
  
  if(collector){
    std::stringstream ss;
    ss << " No of matched jets in matched groups: " << jets.size() << " " 
       << static_cast<int>(jets.size()) << "\n";
						      
	 
	  collector->collect("MultijetFlowNetworkBuilder", ss.str());
  }
      
  V = jnode + 2;  // now we have all the nodes, can index the sink

  // finally add the jet - sink edges.

  for(const auto& j: jets){
      edges.push_back(std::make_shared<FlowEdge>(j.second,
						 V-1,
						 unitCapacity));
      if(collector){
        std::stringstream ss;
        ss <<"new edge " << j.second << "->" << V-1 << '\n';
        collector->collect("MultijetFlowNetworkBuilder", ss.str());
      }

      // fill the node to jet map - to be used to identify jets
      // contributing to hypo success.
      //
      // NB: relies on the having no duplicates  in the values
      nodeToJet[j.second] = j.first;

  }

  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);

}


std::string MultijetFlowNetworkBuilder::toString() const {
  std::stringstream ss;
  ss << "MultijetFlowNetworkBuilder:\n";
  ss << FlowNetworkBuilderBase::toString();
  return ss.str();
}
