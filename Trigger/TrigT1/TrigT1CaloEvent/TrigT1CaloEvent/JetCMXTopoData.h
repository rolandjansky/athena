/* **********************************************************
*
* Emulation of TOB link from one CP CMX to L1Topo
* Author: Alan Watson <Alan.Watson@CERN.CH>
*
********************************************************** */
#ifndef TRIGT1CALO_JETCMXTOPODATA_H
#define TRIGT1CALO_JETCMXTOPODATA_H

// STL include(s):
#include <vector>
#include <string>

// Local include(s):
#include "TrigT1CaloEvent/JetTopoTOB.h"

namespace LVL1 {

  class JetCMXTopoData {

  public:
    /** Constructor with Crate number, overflow flag and vector of TOB words*/
    JetCMXTopoData( int crate, bool overflow, const std::vector< uint32_t >& tobWords );
    /** Constructor with Crate number, overflow flag and TOB vector */
    JetCMXTopoData( int crate, bool overflow, const std::vector< JetTopoTOB >& tobs );
    /** Constructor with Crate number only */
    JetCMXTopoData( int crate );
    /** Default constructor */
    JetCMXTopoData();
    ~JetCMXTopoData();

    /** Return crate number (0-1) */
    int crate() const;
    /** Return overflow flag */
    bool overflow() const;
    /** Return vector of TOBs */
    void tobs(std::vector< JetTopoTOB >& tobs) const;
    /** Return vector of TOB words */
    const std::vector< uint32_t >& tobWords() const;

   /** Add TOB to record */
   void addTOB( const JetTopoTOB tob);
   void addTOB( uint32_t tobWord);
   
   /** Set overflow flag */
   void setOverflow( bool overflow );

  private:
    /** Data members */
    int m_crate;
    bool m_overflow;
    std::vector< uint32_t > m_tobWords;              
    
    static const unsigned int s_maxTOBsPerLink = 32;

  }; // class JetCMXTopoData


} // namespace bracket

#ifndef JetCMXTopoData_ClassDEF_H
#include "TrigT1CaloEvent/JetCMXTopoData_ClassDEF.h"
#endif

#endif // TRIGT1CALO_JETCMXTOPODATA_H
