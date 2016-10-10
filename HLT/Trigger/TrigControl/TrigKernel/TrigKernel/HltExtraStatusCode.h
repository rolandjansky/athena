/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HltExtraStatusCode_H
#define HltExtraStatusCode_H

namespace hltonl {

  /**
   * Status flag bits used to encode the success/failure of HLT updates
   */
  enum ExtraStatusCode {
    HLT_PRESCALE_UPDATE = 1,
    COOL_UPDATE         = 2
    // This is a bitmap. Next value to be used should be '4'.
  };
  
}

#endif
