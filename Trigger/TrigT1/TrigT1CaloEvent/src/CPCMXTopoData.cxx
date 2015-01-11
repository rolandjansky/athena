
// Local include(s):
#include "TrigT1CaloEvent/CPCMXTopoData.h"


namespace LVL1 {
  
  /// Static constants
  const unsigned int CPCMXTopoData::s_maxTOBsPerLink;


  /** Constructors */
  /** Load complete object in one go */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx, bool overflow, const std::vector< CPTopoTOB >& tobs )
    : m_crate( crate ), m_cmx( cmx ), m_overflow( overflow) {
      
    m_tobWords.clear();
    for (std::vector<CPTopoTOB>::const_iterator it = tobs.begin(); it != tobs.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it).tobWord() );
      else m_overflow = true;
    }
     
  }


  /** Load complete object in one go */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx, bool overflow, const std::vector< uint32_t >& tobWords )
    : m_crate( crate ), m_cmx( cmx ), m_overflow( overflow ) {
      
    m_tobWords.clear();
    for (std::vector<uint32_t>::const_iterator it = tobWords.begin(); it != tobWords.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it) );
      else m_overflow = true;
    }
    
  } 


  /** Create empty object for specified crate and module */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx )
    : m_crate( crate ), m_cmx( cmx ), m_overflow( false ) {
     m_tobWords.clear();
  }


  /** default constructor */
  CPCMXTopoData::CPCMXTopoData()
    : m_crate( 0 ), m_cmx( 0 ), m_overflow( false ) {
     m_tobWords.clear();
  }


  /** Destructor */
  CPCMXTopoData::~CPCMXTopoData() {
  }

  /** Add another TOB to the collection */
  void CPCMXTopoData::addTOB( uint32_t tobWord ) {
    if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back(tobWord);
    else m_overflow = true;
  }


  /** Add another TOB word to the collection */
  void CPCMXTopoData::addTOB( const CPTopoTOB tob ) {
    if (m_tobWords.size() < s_maxTOBsPerLink)
      m_tobWords.push_back( tob.tobWord() );
    else m_overflow = true;
  }


  /** Set overflow flag */
  void CPCMXTopoData::setOverflow( bool overflow ) {
    m_overflow = overflow;
  }


  /** Access methods */
  int CPCMXTopoData::crate() const {
    return m_crate;
  }


  int CPCMXTopoData::cmx() const {
    return m_cmx;
  }


  bool CPCMXTopoData::overflow() const {
    if ( m_overflow || (m_tobWords.size() > s_maxTOBsPerLink) ) return true;
    return false;
  }


  const std::vector< uint32_t >& CPCMXTopoData::tobWords() const {
    return m_tobWords;
  }


  void CPCMXTopoData::tobs(std::vector< CPTopoTOB >& tobs) const {
    tobs.clear();
    
    std::vector< uint32_t >::const_iterator it = m_tobWords.begin();
    for ( ; it != m_tobWords.end(); ++it) 
       tobs.push_back( CPTopoTOB(m_crate, m_cmx, (*it)) );
    
    return;
  }
  
  
} // namespace bracket
