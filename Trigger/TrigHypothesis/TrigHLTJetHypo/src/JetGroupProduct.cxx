/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "./JetGroupProduct.h"
#include "./JetStreamer.h"
#include "./make_jetstream.h"

#include <set>
#include <string>

JetGroupProduct::JetGroupProduct(const std::vector<std::size_t>& siblings,
				 const CondInd2JetGroupsInds& satisfiedBy,
				 const std::vector<std::size_t>& condMult) {

  m_valid = !siblings.empty() or satisfiedBy.size() != condMult.size();
  if (m_valid) {init(siblings, satisfiedBy, condMult);}
}

void JetGroupProduct::init(const std::vector<std::size_t>& siblings,
		      const CondInd2JetGroupsInds& satisfiedBy,
		      const std::vector<std::size_t>& condMult) {

  // create a jetstreamer object to cycle through the
  // possoble combinations. The streamer may form 
  // vectors of indices by performing external products, and
  // these may have duplicates, and vectors that differ only in
  // order, which will need to be removed.

  
  std::vector<std::vector<std::size_t>> condIndices;
  condIndices.reserve(siblings.size());
  std::vector<std::size_t> repeats;
  condIndices.reserve(siblings.size());
  
  for(const auto& isib : siblings){
    auto mult = condMult[isib];  // jet groups indices of satisying jgs.
    repeats.push_back(mult);
    condIndices.push_back(satisfiedBy.at(isib));

    // find the greatest jet index we will deal with
    const auto& sibjets = satisfiedBy.at(isib);
    m_jetEnd = std::max(m_jetEnd,
			   *std::max_element(sibjets.begin(),
					     sibjets.end()));
    ++m_jetEnd;; // to be used as an end marker
    m_jetMask.reserve(m_jetEnd);
    
  }


  auto streamer = make_jetstream(condIndices, repeats, 0);
  m_jetstreamer.reset(new JetStreamer(std::move(streamer)));
}
  
std::vector<std::size_t> JetGroupProduct::next(const Collector& collector){
  // Produce an ordered set sequence of jet indices, one from each Condition
  // with no duplicate indices.
  //
  // Ask ProductGen for the next set of indices into the condIndices arrays
  // use these to create a sequences of jet indices. The process is blocked
  // if there is a duplicate jet index using m_jetMask
  //
  // finally, m_seenIndices keeps track of jet index seqiences that have
  // already been generated. If this is the first time the sequece has
  // been generated, it will be added to m_seenJetIndices, and then requrned
  // to the caller. If it has already been seen, it will be abandoned.

  if (!m_valid) {return std::vector<std::size_t>();}
  
  unsigned int ipass{0};
  std::vector<std::size_t> jg_indices;
  while(true){

    m_jetMask.assign(m_jetEnd, false);
    
    if(collector){
      collector->collect("JetGroupProduct::next()",
                         "loop start pass " + std::to_string(ipass++));
    }
      

    bool blocked{false};
    auto jet_indices = m_jetstreamer->next();
    if (jet_indices.empty()) {
      if(collector){
      collector->collect("JetGroupProduct::next()",
                         "end of iteration ");
      }
      return jet_indices;
    }
    for (const auto& ind : jet_indices) {
      if (m_jetMask[ind]) {
	blocked = true;
	break;
      }
      m_jetMask[ind] = true;
    }

    if (blocked){continue;}

    jg_indices.clear();
    for(std::size_t i = 0; i != m_jetEnd; ++i) {
      if (m_jetMask[i]) {
	jg_indices.push_back(i);
      }
    }
      
    if (std::find(m_seenIndices.begin(),
		  m_seenIndices.end(),
		  jg_indices) == m_seenIndices.end()){
      
      m_seenIndices.push_back(jg_indices);
      return jg_indices;
    }
  }		       
}

bool JetGroupProduct::valid() const {return m_valid;}

