/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_COMBINATIONSJETSTREAM_H
#define TRIGHLTJETHYPO_COMBINATIONSJETSTREAM_H

/*
 * CompoundJetStream is an implementation of IJetStream.
 * on every call to bump(), it makes available a vector of k
 * indices chosen from a vector of n indices.
 *
 * The positions in the k chosen jets in the jet indices list
 * is claculated by an instance of CombinationsGenerator, held
 * as attribute of this class.
 *
 * On each call to bump(), the right neighbor is checked for
 * having cycled. If this is rthe case, or if there is no such neighbor,
 * the instance will bump itself by
 * asking its  CombinationsGenerator which positions to 
 * use, and makes these values availble for collection.
 * When all combinations have been exhausted by succesive calls to 
 * bump, the bump() returns true, and the cycle is restarted.
 * 
 */


#include "IJetStream.h"
#include "CombinationsGenerator.h"
#include <vector>
#include <sstream>
#include <memory>
#include <string>

class CombinationsJetStream: public IJetStream {

 public:
  friend std::ostream& operator << (std::ostream&,
				    const CombinationsJetStream&);
  friend std::stringstream& operator << (std::stringstream&,
					 const CombinationsJetStream&);

  CombinationsJetStream(const std::vector<std::size_t>& jets,
		  std::unique_ptr<IJetStream> neigh,
			std::size_t k,
			std::size_t id):
    m_jets(jets),
    m_neigh(std::move(neigh)),
    m_k(k),
    m_id{id}
  {
    auto n = m_jets.size();

    m_valid = k <= n and !jets.empty();
    if (m_valid) {
      m_combgen.reset(new CombinationsGenerator(n, k));
      auto indices = m_combgen->get();
      m_data.clear();
      for (const auto i : indices) {m_data.push_back(m_jets.at(i));}
    }
  }
  
  virtual std::vector<std::size_t> get() override {


    auto result = m_neigh ? m_neigh->get() : std::vector<std::size_t>();
    result.insert(result.end(), m_data.begin(), m_data.end());
    return result;
  }

  
  virtual bool bump() override {

    // if there is a neighor, bump it. If it returns
    // true, it has cycled, in which case try bumping this stream

    bool cycled{false};
    if (m_neigh) {
      bool neigh_cycled = m_neigh->bump();
      if (!neigh_cycled) {return false;}

      cycled = m_combgen->bump();

      auto indices = m_combgen->get();
      m_data.clear();
      for (const auto i : indices) {m_data.push_back(m_jets.at(i));}
      return cycled;
    } else {
      // no neighbor
      cycled = m_combgen->bump();

      auto indices = m_combgen->get();
      m_data.clear();
      for (const auto& i : indices) {
	m_data.push_back(m_jets.at(i));
      }
      return cycled;
    }
  }

  
  virtual bool valid() const override {
    if (!m_valid){return false;}
    
    if (m_neigh) {return m_neigh->valid();}
    return true;
  }

  virtual std::string dump() const override {

    auto result = m_neigh ? m_neigh->dump() : "";

    std::stringstream ss;
    ss << *this;
    result += ss.str();
    
    return result;
  }

private:
  std::vector<std::size_t> m_jets;
  std::unique_ptr<IJetStream> m_neigh{nullptr};
  std::size_t m_k;  // n choose k...
  std::size_t m_id;
  std::vector<std::size_t> m_data;
  std::unique_ptr<CombinationsGenerator> m_combgen{nullptr};
  bool m_valid{false};
   
 
  
  bool empty() const {
    return m_jets.empty();
  }

  };

std::ostream& operator << (std::ostream&,
			   const CombinationsJetStream&);

std::stringstream& operator << (std::stringstream&,
				const CombinationsJetStream&);

 #endif
