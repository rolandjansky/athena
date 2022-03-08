/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SIMPLEJETSTREAM_H
#define TRIGHLTJETHYPO_SIMPLEJETSTREAM_H


/*
 * SimpleJetStream is an implementation of IJetStream.

 * On each call to bump(), it steps through its container of jet indices,
 * making the current value
 * available for collection. When it reaches the end, and moves
 * back to the begining of its list. bump() returns the true when it has
 * cycled, otherwise it returns false
 * 
 */

#include "IJetStream.h"
#include <vector>
#include <sstream>
#include <memory>
#include <string>

class SimpleJetStream: public IJetStream {

 public:


  friend std::ostream& operator << (std::ostream&,
				    const SimpleJetStream&);
  friend std::stringstream& operator << (std::stringstream&,
					 const SimpleJetStream&);

  SimpleJetStream(const std::vector<std::size_t>& jets,
		  std::unique_ptr<IJetStream> neigh,
		  std::size_t id):
    m_jets(jets),
    m_neigh(std::move(neigh)),
    m_id{id}
  {
    m_valid = !jets.empty();
    if(m_valid) {
      m_data = m_jets.at(0);
      m_ind = 0;
    }
  }
  
  
  virtual std::vector<std::size_t> get() override {

    auto result = m_neigh ? m_neigh->get() : std::vector<std::size_t>();
    result.push_back(m_data);

    return result;
  }
  
  virtual bool bump() override {
    // if there is a neighbor, try bumping it.
    bool cycled{false};
    if (m_neigh) {
      bool neigh_cycled = m_neigh->bump();

      if (!neigh_cycled) {return false;}
      
      // neighbor has cycled as a result of bumping.
	++m_ind;
	cycled = m_ind == m_jets.size();
	
	if (cycled) {m_ind = 0;}
	
	m_data = m_jets.at(m_ind);
	return cycled;
    }  else {
    
      // no neighbor

      ++m_ind;

      cycled = m_ind == m_jets.size();
  
      if(cycled) {
	m_ind = 0;
      }
      m_data = m_jets.at(m_ind);
      return cycled;
    }
  }



  virtual bool valid() const override {
    if (!m_valid) {return false;}
    
    if (m_neigh) {return m_neigh->valid();}
    return true;
  }

  virtual std::string dump() const override {
    std::stringstream ss;

    auto result = m_neigh ? m_neigh->dump() : "";

    ss<< *this << '\n';
    result += ss.str();
    
    return result;
  }

private:
  std::vector<std::size_t> m_jets;
  std::size_t m_ind{0};
  std::unique_ptr<IJetStream> m_neigh{nullptr};
  std::size_t m_id;
  std::size_t m_data;
  bool m_valid{false};

};

std::ostream& operator << (std::ostream& os ,
			   const SimpleJetStream& js);
std::stringstream& operator << (std::stringstream& os ,
				const SimpleJetStream& js);

#endif
