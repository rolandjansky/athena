/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CPTOOLTESTS_ERRORCHECK_H
#define CPTOOLTESTS_ERRORCHECK_H

#define CHECK(ARG)                                                \
    do {                                                          \
        const bool result = ARG;                                  \
        if (!result) {                                            \
            ::Error(APP_NAME, "Failed to execute: \"%s\"", #ARG); \
            return 1;                                             \
        }                                                         \
    } while (false)

#endif  // CPTOOLTESTS_ERRORCHECK_H
