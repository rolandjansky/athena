/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETSTEAMER_H
#define TRIGHLTJETHYPO_JETSTEAMER_H

/*
 * JetStreamer owns a linked list  of IJetStream objects.
 * These provide vectors of indices according to the conrete types.
 * The stream objects cycle, which allows for stepping through
 * all possible combinations of jets. On each call to bump an element 
 * in the list informs its left neigbor if the fact. The
 * the cycle state is relayed back to JetStreamer: when all elements
 * in the list have cycled, the iteration stops.
 */

#include "./IJetStream.h"
#include "./SimpleJetStream.h"
#include "./CombinationsJetStream.h"

#include <sstream>
#include <ostream>

class JetStreamer {

 public:

  friend std::ostream& operator << (std::ostream&, const JetStreamer&);
  friend std::ostream& operator << (std::stringstream&, const JetStreamer&);

  
 JetStreamer(std::unique_ptr<IJetStream>&& stream) :
   m_stream(std::move(stream))
  {
    m_valid  = m_stream != nullptr and  m_stream->valid();
  }
  
  
  std::vector<std::size_t> next() {

    if (!m_valid) {
      return std::vector<std::size_t>();
    }

    if (m_done) {return  std::vector<std::size_t>();}
    auto result = m_stream->get(); // stream always as legal data

    m_done = m_stream->bump();
    
    return result;
  }

  bool isValid() const {return m_valid;}
  
 private:
  std::unique_ptr<IJetStream> m_stream;
  bool m_done{false};
  bool m_valid{false};
};

std::ostream& operator << (std::ostream&, const JetStreamer&);

#endif
