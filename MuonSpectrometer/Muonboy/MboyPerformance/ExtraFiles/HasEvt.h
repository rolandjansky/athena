/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HasEvt_h
#define HasEvt_h
#include "AANTEvent.h"
#include "AANTMisc.h"

class HasEvt {
public: 
  HasEvt();
  virtual ~HasEvt( );

  static void setEvt(AANTEvent* pevt); 

public:
///////////////////////////////////
// Data
//  The event
    static AANTEvent* p_evt ;

};
#endif // 
