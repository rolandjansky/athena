/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_RANGEFILTER_H
#define TRIGHLTJETHYPO_RANGEFILTER_H

#include "./IHypoJetVectorFilter.h"
#include <sstream>
#include <ostream>

class RangeFilter: public IHypoJetVectorFilter  {
 public:

  RangeFilter(){};
  RangeFilter(std::size_t begin, std::size_t end);

  // find the subset of jets which satisfy a sequence of ranges
  virtual std::pair<HypoJetCIter, HypoJetCIter>
  filter (const HypoJetCIter& b,
	  const HypoJetCIter& e,
	  const std::unique_ptr<ITrigJetHypoInfoCollector>&
	  ) override;

  virtual std::string toString() const override;  
 private:
  std::size_t m_begin{0};
  std::size_t m_end{0};
  long int m_njets{0};
  
  HypoJetVector m_filtered{};  
};

std::ostream& operator<<(std::ostream&, const RangeFilter&);


#endif
