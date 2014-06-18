/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKTYPES_H
#define FTKTYPES_H

#define PROTECT_SA

#include "FTKHit.h"

namespace ftk {

  struct hitlist_elm {
     const FTKHit *car;
     struct hitlist_elm *cdr;
  };
  typedef struct hitlist_elm hitlist_elm_t;


  /** definition for the structure containing the SS list */
  struct ss_lookup {
    int ss;
    unsigned char n;
    struct hitlist_elm *begin;
  };
  typedef struct ss_lookup ss_lookup_t;


  // define debug message level, sevr messages cause an abort of the process
  enum message_level {
    debg = 0, info, warn, sevr
  };
  typedef enum message_level message_level_t;

}

#endif // FTKTYPES_H
