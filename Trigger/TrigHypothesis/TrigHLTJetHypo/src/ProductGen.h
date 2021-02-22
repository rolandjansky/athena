/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_PRODUCTGEN_H
#define TRIGHLTJETHYPO_PRODUCTGEN_H

#include <optional>
#include <vector>

// For a vector of vectors, repeated calls to next() returns
// indices that point to elements in the inner vectors.
// The calls will step through this data structure.
class ProductGen{
 public:
  ProductGen();
  
  // ends contains the sizes of the inner vectors
  explicit ProductGen(const std::vector<std::size_t>& ends);
  std::optional<std::vector<std::size_t>>  next();

 private:
  std::vector<std::size_t> m_ends;
  std::vector<std::size_t> m_counters;
  std::size_t m_ncounters;
  bool m_done{true};
  bool atEnd(){return (m_counters == m_ends);}
};

#endif
