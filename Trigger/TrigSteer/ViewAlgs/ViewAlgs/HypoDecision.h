/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ViewAlgs_HypoDecision_h
#define ViewAlgs_HypoDecision_h
#include <set>
#include "xAODTrigger/TrigComposite.h"

class HypoDecision {
 public:
  /// constructs HypoDecision aux class rom the TrigComposite (encoding of the decision hidden here)
  HypoDecision(){}
  HypoDecision( const xAOD::TrigComposite& src );
  static bool containsDecision( const xAOD::TrigComposite& src );

  // attaches the decision information to the TrigComposite
  void store( xAOD::TrigComposite& destination ) const;

  /// added info about the decision taken by an algo/tool given its instance name
  void addDecision( const std::string& hypo_name, bool decision );
  /// added info about the decision taken by an algo/tool given its hashed ID
  void addDecision( unsigned hypo_id, bool decision );

  /// return true if the decision of a hypo given by the ID was ever recorded in this object
  bool known( unsigned hypo_id ) const { 
    return failed(hypo_id) or passed(hypo_id); 
  }

  /// return true if the hypo decision was positive ( use like this: known(id) and passing(id) ) 
  bool passed( unsigned hypo_id ) const {  
    if (find(m_passed.begin(), m_passed.end(), hypo_id) != m_passed.end()) {
      return true; 
    }
    return false;
  }
  /// return true if the hypo decision was negative
  bool failed( unsigned hypo_id ) const { 
    if (find(m_failed.begin(), m_failed.end(), hypo_id) != m_failed.end()) {
      return true; 
    }
    return false;
  }
  const std::vector<unsigned>& passed() const { return m_passed; }
  const std::vector<unsigned>& failed() const { return m_failed; }

 private:
  std::vector<unsigned> m_passed;
  std::vector<unsigned> m_failed;
};


#endif
