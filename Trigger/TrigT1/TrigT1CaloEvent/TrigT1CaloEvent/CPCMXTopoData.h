/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/* **********************************************************
*
* Emulation of TOB link from one CP CMX to L1Topo
* Author: Alan Watson <Alan.Watson@CERN.CH>
*
********************************************************** */
#ifndef TRIGT1CALO_CPCMXTOPODATA_H
#define TRIGT1CALO_CPCMXTOPODATA_H

// STL include(s):
#include <vector>
#include <string>

// Local include(s):
#include "TrigT1CaloEvent/CPTopoTOB.h"

namespace LVL1 {

  class CPCMXTopoData {

  public:
    /** Constructor with Crate, CMX numbers, overflow flag and vector of TOB words*/
    CPCMXTopoData( int crate, int cmx, bool overflow, const std::vector< uint32_t >& tobWords );
    /** Constructor with Crate, CMX numbers, overflow flag and TOB vector */
    CPCMXTopoData( int crate, int cmx, bool overflow, const std::vector< CPTopoTOB >& tobs );
    /** Constructor for HLT: Crate, CMX numbers and vector of RoI words*/
    CPCMXTopoData( int crate, int cmx, const std::vector< uint32_t >& roiWords );
    /** Constructor with Crate, CMX numbers only */
    CPCMXTopoData( int crate, int cmx );
    /** Default constructor */
    CPCMXTopoData();
    ~CPCMXTopoData();

    /** Return crate number (0-3) */
    int crate() const;
    /** Return CMX number (0 = EM, 1 = TAU) */
    int cmx() const;
    /** Return overflow flag */
    bool overflow() const;
    /** Fill a vector of TOBs */
    void tobs(std::vector< CPTopoTOB >& tobs) const;
    /** Return vector of TOB words */
    const std::vector< uint32_t >& tobWords() const;

   /** Add TOB to record */
   void addTOB( const CPTopoTOB tob );
   void addTOB( uint32_t tobWord );
 
   /** Add TOB from RoI word */
   void addRoI( uint32_t roiWord );
   
   /** Set overflow flag */
   void setOverflow( bool overflow );

   /** @brief count whether any transmission CPM -> CMX had too many TOBs

       In which case toggle m_cpm_overflow
    */
    CPCMXTopoData& checkCpmOverflow();
  private:
    /** Data members */
    int m_crate;                             
    int m_cmx;
    bool m_overflow; ///< overflow on CMX -> L1Topo transmission
    bool m_cpm_overflow; ///< overflow on CPM -> CMX transmission
    std::vector< uint32_t > m_tobWords;
    
    static const unsigned int s_maxTOBsPerLink = 30;
    static const unsigned int s_maxTOBsPerCpm = 5; ///< as specified in BackplaneFormats_v3.xlsx

  }; // class CPCMXTopoData


} // namespace bracket


#ifndef CPCMXTopoData_ClassDEF_H
#include "TrigT1CaloEvent/CPCMXTopoData_ClassDEF.h"
#endif

#endif // TRIGT1CALO_CPCMXTOPODATA_H
