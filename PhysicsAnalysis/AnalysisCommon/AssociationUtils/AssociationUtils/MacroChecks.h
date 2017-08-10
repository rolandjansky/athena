#ifndef ASSOCIATIONUTILS_MACROCHECKS_H
#define ASSOCIATIONUTILS_MACROCHECKS_H

#include "AsgTools/Check.h"

// Checks that pointer is non-null, prints error.
// Can only be used in context where the ATH_MSG_XXX macros work
// and where we return a StatusCode from this scope.
#define ORT_POINTER_CHECK( ptr )                        \
  do {                                                  \
    if(ptr == nullptr) {                                \
      ATH_MSG_ERROR(#ptr << " pointer is null!");       \
      return StatusCode::FAILURE;                       \
    }                                                   \
  } while(0)

#endif // ASSOCIATIONUTILS_MACROCHECKS_H
