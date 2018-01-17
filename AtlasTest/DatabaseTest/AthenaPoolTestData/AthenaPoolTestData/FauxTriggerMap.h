/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FauxTriggerMap.h
 * @brief Class definition for FauxTriggerMap
 */
/**
 * @class FauxTriggerMap
 * @brief Support class for use in AthenaPoolMultiTest event splitting. <br>
 * Acts as dummy trigger information in event. FOR TESTING ONLY.
 * @author Jack Cranshaw
 * Created May 2004 for DC2 validation
 */
// FauxTriggerMap.h
//  
#ifndef ATHENAPOOLTEST_TRIGGER_H
#define ATHENAPOOLTEST_TRIGGER_H

#include <string>
#include <set>

class TrigPath;

class FauxTriggerMap 
{
public:
    typedef unsigned int NUMBER_TYPE;
	
    FauxTriggerMap();
    virtual ~FauxTriggerMap();
	
    /// Add the trigger path to the current map
    bool        addPath(const TrigPath& tp);

    /// Query whether l1bit is in the map
    bool        hasL1(int l1bit) const;
    /// Return the bits stored in the map
    NUMBER_TYPE bits() const;

private:
    bool        m_l1results[64];
    NUMBER_TYPE m_bits;
};  
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(FauxTriggerMap, 1287802, 0)
#endif
