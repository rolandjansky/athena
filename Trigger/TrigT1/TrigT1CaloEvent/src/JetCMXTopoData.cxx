/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "TrigT1CaloEvent/JetCMXTopoData.h"


namespace LVL1 {
  
  /// Static constants
  const unsigned int JetCMXTopoData::s_maxTOBsPerLink;
  const unsigned int JetCMXTopoData::s_maxTOBsPerJem;

  /** Constructors */
  JetCMXTopoData::JetCMXTopoData( int crate, bool overflow, const std::vector< uint32_t >& tobWords )
    : m_crate( crate ), m_overflow( overflow ),
      m_jem_overflow(false)
  {    

    m_tobWords.clear();
    for (std::vector<uint32_t>::const_iterator it = tobWords.begin(); it != tobWords.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it) );
      else m_overflow = true;
    }
    checkJemOverflow();
  }


  JetCMXTopoData::JetCMXTopoData( int crate, bool overflow, const std::vector< JetTopoTOB >& tobs )
    : m_crate( crate ), m_overflow( overflow ),
      m_jem_overflow(false)
  {
    
    m_tobWords.clear();
    for (std::vector<JetTopoTOB>::const_iterator it = tobs.begin(); it != tobs.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it).tobWord() );
      else m_overflow = true;
    }
    checkJemOverflow();
  }


  JetCMXTopoData::JetCMXTopoData( int crate, const std::vector< uint32_t >& roiWords )
    : m_crate( crate ),
      m_jem_overflow(false)
  {    

    m_overflow = false;
    m_tobWords.clear();
    
    for (std::vector<uint32_t>::const_iterator it = roiWords.begin(); it != roiWords.end(); ++it) {
      JetTopoTOB tob((*it));
      if (tob.crate() == m_crate) m_tobWords.push_back( tob.tobWord() );
    }
    checkJemOverflow();
  }


  JetCMXTopoData::JetCMXTopoData( int crate )
    : m_crate( crate ), m_overflow( false ),
      m_jem_overflow(false)
  {
     m_tobWords.clear();
  }


  JetCMXTopoData::JetCMXTopoData()
    : m_crate( 0 ), m_overflow ( false ),
      m_jem_overflow(false)
 {
     m_tobWords.clear();
  }


  /** Destructor */
  JetCMXTopoData::~JetCMXTopoData() {
  }


  /** Add another TOB to the collection */
  void JetCMXTopoData::addTOB( const JetTopoTOB tob ) {
    if (m_tobWords.size() < s_maxTOBsPerLink)
      m_tobWords.push_back(tob.tobWord());
    else m_overflow = true;
  }


  /** Add another TOB word to the collection */
  void JetCMXTopoData::addTOB( uint32_t tobWord ) {
    if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( tobWord );
    else m_overflow = true;
  }

  /** Add another TOB word to the collection. Note: overflows ignored in this method */
  void JetCMXTopoData::addRoI( uint32_t roiWord ) {
    JetTopoTOB tob(roiWord);
    if (tob.crate() == m_crate) m_tobWords.push_back( tob.tobWord() );
  }


  /** Set overflow flag */
  void JetCMXTopoData::setOverflow( bool overflow ) {
    m_overflow = overflow;
  }


  /** Access methods */
  int JetCMXTopoData::crate() const {
    return m_crate;
  }


  bool JetCMXTopoData::overflow() const {
    if ( m_overflow ||
         (m_tobWords.size() > s_maxTOBsPerLink) ||
        m_jem_overflow ) return true;
    return false;
  }


  const std::vector< uint32_t >& JetCMXTopoData::tobWords() const {
    return m_tobWords;
  }


  void JetCMXTopoData::tobs(std::vector< JetTopoTOB >& tobs) const {
    tobs.clear();
    
    std::vector< uint32_t >::const_iterator it = m_tobWords.begin();
    for ( ; it != m_tobWords.end(); ++it) 
       tobs.push_back(JetTopoTOB(m_crate, (*it)));
    
    return;
  }

JetCMXTopoData& JetCMXTopoData::checkJemOverflow()
{
    const size_t max_jem_index = 15; // as indicated in JetTopoTOB::jem()
    std::vector<uint32_t> counters_tob_per_jem(max_jem_index, 0);
    for(const uint32_t word : m_tobWords) {
        JetTopoTOB tob(m_crate, word);
        const size_t iJem = tob.jem();
        counters_tob_per_jem[iJem] += 1;
    }
    m_jem_overflow = (m_jem_overflow ||
                      std::any_of(counters_tob_per_jem.begin(),
                                  counters_tob_per_jem.end(),
                                  [](const uint32_t &c) { return (c >= s_maxTOBsPerJem); }));
    return *this;
}
  
  
} // namespace bracket
