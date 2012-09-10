/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CHANFITERRORCODE_H
#define CHANFITERRORCODE_H

#include "TrigT1CaloCalibConditions/AbstractErrorCode.h"

/**
 * ChanFitErrorCode stores information about the calibration fit quality.
 *
 * @author Veit Scharf <veit.scharf@kip.uni-heidelberg.de>
 */

class ChanFitErrorCode : public AbstractErrorCode
{
 public:
    enum ChanFitErrorCodeBits 
    {
      // to be extended
      NoDataBit = 0,
      BadDataBit = 1
    };
    
    ChanFitErrorCode();
    ChanFitErrorCode(unsigned int errorCode);
    ChanFitErrorCode(const ChanFitErrorCode& e);
    
    ChanFitErrorCode& operator=(const ChanFitErrorCode& rhs);
    ChanFitErrorCode& operator|=(const ChanFitErrorCode& rhs);

    using AbstractErrorCode::errorCode;
    
    bool chanValid() const;
    
    inline bool noData() const { return bitIsSet(NoDataBit); }
    inline void noData(bool bad) { setBit(NoDataBit, bad); }

    inline bool badData() const { return bitIsSet(BadDataBit); }
    inline void badData(bool bad) { setBit(BadDataBit, bad); }
};

#endif // CHANFITERRORCODE_H
