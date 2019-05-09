#include "./MultijetFlowNetworkBuilder.h"
#include <map>
#include <algorithm>
#include <sstream>

MultijetFlowNetworkBuilder::MultijetFlowNetworkBuilder(const ConditionsMT& conditions):
FlowNetworkBuilderBase(conditions){
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
  
  std::vector<std::shared_ptr<FlowEdge>> edges(m_initialEdges.begin(),
                                               m_initialEdges.end());
  
  // matches: std::optional<std::vector<std::vector<int>>>
  for(const auto& jg_nodes : *matches){
    // icond is m_conditons index
    double cap = m_conditions[icond].capacity();

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
  std::map<pHypoJet, int> jets; 

  std::size_t jnode{m_conditions.size() + (groups_e-groups_b)};

  // now add the jetgroup - jet edges
  std::size_t sinkCapacity{0};
  constexpr double unitCapacity = 1.0;
  
  for(const auto& jgnodes : *matches){
    // jgnodes - vector of jet group indices
    for (auto jgnode : jgnodes){
      // convert from (a job group index to a job group.
      auto jgroup = *(groups_b + jgnode);
      
      // loop over jets in group and identify group-jet edges
      for(const auto& j : jgroup){
        // do not allow duplicates
        
        auto iter = jets.find(j);
        if (iter == jets.end()){
          jets[j] = ++jnode;
          edges.push_back(std::make_shared<FlowEdge>(jgnode + jg_offset,
                                                     jnode,
                                                     unitCapacity));
          if(collector){
            std::stringstream ss;
            ss << jgnode << "->" << jnode << '\n';
            collector->collect("MatcherMT", ss.str());
          }
        }
        /*
        else {
        edges.push_back(std::make_shared<FlowEdge>(jgnode +jg_offset,
        iter->second,
                                                     unitCapacity));
          if(collector){
            std::stringstream ss;
            ss << jgnode + jg_offset<< "->" << iter->second << '\n';
            collector->collect("MatcherMT", ss.str());
          }
        }
        */
      }
    }
  }

  auto sourceCapacity = 0.;
  for(const auto& c : m_conditions){sourceCapacity += c.capacity();}
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
    collector->collect("MultijetFlowNetworkBuilder - fn before",
		       "Νο of matched jets in matched groups: " + std::to_string(jets.size())+ "\n");
  }
  
  V = jnode + 2;  // now we have all the nodes, can index the sink

  // finally add the jet - sink edges.

  for(const auto j: jets){
      edges.push_back(std::make_shared<FlowEdge>(j.second,
						 V-1,
						 unitCapacity));

      // fill the node to jet map - to be used to identify jets
      // contirbuting to hypo success.
      //
      // NB: relies on the having no duplicates  in the values
      nodeToJet[j.second] = j.first;
      if(collector){
        std::stringstream ss;
        ss << j.first << "->" << V-1 << '\n';
        collector->collect("MatcherMT", ss.str());
      }
  }

  return std::make_optional<std::vector<std::shared_ptr<FlowEdge>>>(edges);

}


