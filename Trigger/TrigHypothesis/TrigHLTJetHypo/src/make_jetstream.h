/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MAKE_JETSTREAM_H
#define TRIGHLTJETHYPO_MAKE_JETSTREAM_H

/*
 * make_jetstream creates a linled list of IJetStreams. 
 * The first arguement of type  vector<vector<std::size>> contains 
 * jet indices that pass a RepeatedCondition. There is one entry in the outer
 * vector per Condition being considered.
 *
 * The vector<std:size_t> contains the repeat values of the RepeatedConditions.
 *
 * The concrete types in the list
 * will be a SimpleJetStream if the repeat value for the corresponding
 * Condition is 1, and a CombinationsJetStream otherwise.
 */

#include "SimpleJetStream.h"
#include "CombinationsJetStream.h"
#include "JetStreamer.h"
#include <memory>
#include <vector>

std::unique_ptr<IJetStream>
make_jetstream(std::vector<std::vector<std::size_t>> indices,
	    std::vector<std::size_t> repeats,
	    std::size_t sid) {
  if (indices.size()==1) {
    auto null_stream  = std::unique_ptr<IJetStream>{nullptr};
    auto base_case = std::unique_ptr<IJetStream>(nullptr);
    auto repeat = repeats.at(0);
    if (repeat == 1) {
      base_case.reset(new SimpleJetStream(indices.at(0),
					  std::move(null_stream),
					  sid));
    } else {
      base_case.reset(new CombinationsJetStream(indices.at(0),
						std::move(null_stream),
						repeat,
						sid));
    }
    return base_case;
		    
  }

  auto inds = indices.back();
  indices.pop_back();

  auto repeat = repeats.back();
  repeats.pop_back();
  
  auto n_sid = sid;
  auto right_stream =  make_jetstream(indices, repeats, ++n_sid);
  auto stream = std::unique_ptr<IJetStream>(nullptr);
  if (repeat == 1) {
    stream.reset(new SimpleJetStream(inds,
				     std::move(right_stream),
				     sid));
  } else {
        stream.reset(new CombinationsJetStream(inds,
					       std::move(right_stream),
					       repeat,
					       sid));
  }
  
  return stream;
}
					     
#endif
