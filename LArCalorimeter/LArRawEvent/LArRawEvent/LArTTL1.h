/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// * author : Fabienne Ledroit                                                     *
// * date of creation : 09/01/2003                                                 *
// * last change 05 april 2004 (FL): added offline id and changed double to float  *


#ifndef LARTTL1_H
#define LARTTL1_H
#include <vector>

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"


/**
@class LArTTL1
@brief Liquid Argon TT L1 sum base class 

 * It's a TES object which is readonly 

@author Fabienne Ledroit
 *
 * sampleValues contains 7 samplings. 
 */

class LArTTL1  {

public:


    /** Constructor <br><br>
     * Beware that sampleValues is a reference to a STL vector of float's <br>
     * which is filled by a series of push_back calls .<br> 
     * Using another construction method of this vector will fail . <br>
     * Now carries both online and offline Id. One is to be removed when we decide which one <br><br> */
    LArTTL1(const HWIdentifier& onlineId, const Identifier& offlineId, const std::vector<float>& sampleValues); 

    /**  return LArTTChannelID (online Id)   */
    const HWIdentifier & ttOnlineID() const; 

    /** return TT offline Identifier */
    const Identifier & ttOfflineID() const; 

    /** return number of samples */
    short nsamples() const; 

    /** return a reference to a stl vector containing the sample values */
    const std::vector<float> & samples() const; 

    /** Destructor */
    virtual ~LArTTL1();

    /** default constructor for persistency */
    LArTTL1();

private:

    /** online Id  */
    HWIdentifier  m_onlineId ;

    /** offline Id (CaloLVL1_ID) */
    Identifier m_offlineId;

    /** vector of samples */
    std::vector <float> m_samples;

};

#endif //LARTTL1_H

