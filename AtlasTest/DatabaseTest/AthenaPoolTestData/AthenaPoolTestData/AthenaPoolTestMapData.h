/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestMapData.h 
 * 
 * @brief Test class for embedded collections
 * 
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 * 
 * $Id: AthenaPoolTestMapData.h,v 1.1 2005-02-04 16:12:19 schaffer Exp $
 */

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAPDATA_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAPDATA_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaPoolTestData/AthenaPoolTestMapDataLink.h"
#include <vector>

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class AthenaPoolTestMapData 
 * 
 * @brief
 * 
 */

class AthenaPoolTestMapData 
{
    
public:
    // A particle link, and the energy (charge) which its hits
    // contribute to the current RDO.
    typedef std::pair< AthenaPoolTestMapDataLink, float > Deposit; 

    AthenaPoolTestMapData();
    AthenaPoolTestMapData (const std::vector< Deposit >& deposits, int simDataWord = 0);
    virtual ~AthenaPoolTestMapData();

    int word() const;                           // Get the packed simdata word

    void deposits(std::vector<Deposit>& deposits) const; // Get the Deposits
    //std::vector< Deposit > getdeposits() const; // for some reason I can't get this to return by reference! It won't compile. EJWM 

    const std::vector< Deposit >& getdeposits() const;

private:

    int m_word;   
    //    Deposit* m_p_deposits; but use vector meantime, needs more work
    std::vector<Deposit> m_deposits;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline int AthenaPoolTestMapData::word() const
{
    return m_word & 0x1fffffff;
}

inline const std::vector< AthenaPoolTestMapData::Deposit >& AthenaPoolTestMapData::getdeposits() const
{
  return m_deposits;
}

inline void AthenaPoolTestMapData::deposits(std::vector< AthenaPoolTestMapData::Deposit>& deposits) const
{
  // I really don't understand what the point of this is ... it does EXACTLY the same as the getdeposits() method, just in a different way. EJWM
    deposits = m_deposits;
    return;
}

#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTMAPDATA_H
