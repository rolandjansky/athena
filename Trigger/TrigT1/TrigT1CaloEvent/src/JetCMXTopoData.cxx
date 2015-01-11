
// Local include(s):
#include "TrigT1CaloEvent/JetCMXTopoData.h"


namespace LVL1 {
  
  /// Static constants
  const unsigned int JetCMXTopoData::s_maxTOBsPerLink;

  /** Constructors */
  JetCMXTopoData::JetCMXTopoData( int crate, bool overflow, const std::vector< uint32_t >& tobWords )
    : m_crate( crate ), m_overflow( overflow )
  {    

    m_tobWords.clear();
    for (std::vector<uint32_t>::const_iterator it = tobWords.begin(); it != tobWords.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it) );
      else m_overflow = true;
    }

  }


  JetCMXTopoData::JetCMXTopoData( int crate, bool overflow, const std::vector< JetTopoTOB >& tobs )
    : m_crate( crate ), m_overflow( overflow )
  {
    
    m_tobWords.clear();
    for (std::vector<JetTopoTOB>::const_iterator it = tobs.begin(); it != tobs.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it).tobWord() );
      else m_overflow = true;
    }
    
  }


  JetCMXTopoData::JetCMXTopoData( int crate )
    : m_crate( crate ), m_overflow( false )
  {
     m_tobWords.clear();
  }


  JetCMXTopoData::JetCMXTopoData()
    : m_crate( 0 ), m_overflow ( false ) {
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


  /** Set overflow flag */
  void JetCMXTopoData::setOverflow( bool overflow ) {
    m_overflow = overflow;
  }


  /** Access methods */
  int JetCMXTopoData::crate() const {
    return m_crate;
  }


  bool JetCMXTopoData::overflow() const {
    if ( m_overflow || (m_tobWords.size() > s_maxTOBsPerLink) ) return true;
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
  
  
} // namespace bracket
