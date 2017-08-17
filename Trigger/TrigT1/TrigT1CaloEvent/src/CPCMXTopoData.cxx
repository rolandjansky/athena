
// Local include(s):
#include "TrigT1CaloEvent/CPCMXTopoData.h"

#include <algorithm>

namespace LVL1 {
  
  /// Static constants
  const unsigned int CPCMXTopoData::s_maxTOBsPerLink;
  const unsigned int CPCMXTopoData::s_maxTOBsPerCpm;


  /** Constructors */
  /** Load complete object in one go */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx, bool overflow, const std::vector< CPTopoTOB >& tobs )
    : m_crate( crate ), m_cmx( cmx ), m_overflow( overflow),
      m_cpm_overflow(false)
  {
      
    m_tobWords.clear();
    for (std::vector<CPTopoTOB>::const_iterator it = tobs.begin(); it != tobs.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it).tobWord() );
      else m_overflow = true;
    }
    checkCpmOverflow();
  }


  /** Load complete object in one go */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx, bool overflow, const std::vector< uint32_t >& tobWords )
    : m_crate( crate ), m_cmx( cmx ), m_overflow( overflow ),
      m_cpm_overflow(false)
  {
      
    m_tobWords.clear();
    for (std::vector<uint32_t>::const_iterator it = tobWords.begin(); it != tobWords.end(); ++it) {
      if (m_tobWords.size() < s_maxTOBsPerLink) m_tobWords.push_back( (*it) );
      else m_overflow = true;
    }
    checkCpmOverflow();    
  }
  

  /** Load complete object in one go */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx, const std::vector< uint32_t >& roiWords )
    : m_crate( crate ), m_cmx( cmx ),
      m_cpm_overflow(false)
  {
      
    m_overflow = false;
    m_tobWords.clear();
    
    for (std::vector<uint32_t>::const_iterator it = roiWords.begin(); it != roiWords.end(); ++it) {
      CPTopoTOB tob((*it));
      if (tob.crate() == m_crate && tob.cmx() == m_cmx) m_tobWords.push_back( tob.tobWord() );
    }
    checkCpmOverflow();
  } 


  /** Create empty object for specified crate and module */
  CPCMXTopoData::CPCMXTopoData( int crate, int cmx )
    : m_crate( crate ), m_cmx( cmx ), m_overflow( false ),
      m_cpm_overflow(false)
  {
     m_tobWords.clear();
  }


  /** default constructor */
  CPCMXTopoData::CPCMXTopoData()
    : m_crate( 0 ), m_cmx( 0 ), m_overflow( false ),
      m_cpm_overflow(false)

  {
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


  /** Add another TOB word to the collection. Note: overflows ignored in this method */
  void CPCMXTopoData::addRoI( uint32_t roiWord ) {
    
    CPTopoTOB tob(roiWord);
    
    if (tob.crate() == m_crate &&  tob.cmx() == m_cmx) m_tobWords.push_back( tob.tobWord() );

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
    if ( m_overflow ||
         (m_tobWords.size() > s_maxTOBsPerLink) ||
         m_cpm_overflow ) return true;
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

CPCMXTopoData& CPCMXTopoData::checkCpmOverflow()
{
    const size_t max_cpm_index = 14; // as indicated in CPTopoTOB::cpm(), but we start from 0, not 1
    std::vector<uint32_t> counters_tob_per_cpm(max_cpm_index, 0);
    for(const uint32_t word : m_tobWords) {
        CPTopoTOB tob(m_crate, m_cmx, word);
        const size_t iCpm = tob.cpm()-1;
        const bool icpmValid = iCpm < counters_tob_per_cpm.size();
        if(icpmValid) {
            counters_tob_per_cpm[iCpm] += 1;
        } else {
            /*
              // cout discouraged in athena, but this occurrence is
              // rare and not deemed to deserve Athena::MsgStreamMember
            cout<<"CPCMXTopoData::checkCpmOverflow :"
                <<" invalid iCpm "<<iCpm<<","
                <<" vector size is "<<counters_tob_per_cpm.size()
                <<endl;
            */
        }
    }
    m_cpm_overflow = (m_cpm_overflow ||
                      std::any_of(counters_tob_per_cpm.begin(),
                                  counters_tob_per_cpm.end(),
                                  [](const uint32_t &c) { return (c >= s_maxTOBsPerCpm); }));
    return *this;
}
  
  
} // namespace bracket
