/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef CLANG_CAN_HANDLE_UNWIND_H

#include "Hephaestus/Hephaestus.h"
#include "Hephaestus/UnwindBacktrace.h"

#include <unwind.h>

/* Note on use: some systems will require pre-loading libunwind.so */


/* - helper for collecting stack trace ------------------------------------- */
struct hhh_TraceCollector {
   void **addresses;
   int size, count;
};


/* _________________________________________________________________________ */
static _Unwind_Reason_Code hhh_traceCallback( struct _Unwind_Context *uc, void *extra )
{
   struct hhh_TraceCollector *tc = 0;

   tc = (struct hhh_TraceCollector*)extra;

   if ( 0 <= tc->count )
      tc->addresses[ tc->count++ ] = (void*)_Unwind_GetIP( uc );
   else
      tc->count += 1;

   if ( tc->count == tc->size )
      return _URC_END_OF_STACK;

   return _URC_NO_REASON;
}


/* _________________________________________________________________________ */
int hhh_UnwindBacktrace( void **array, int size )
{
   struct hhh_TraceCollector tc;

   tc.addresses = array;
   tc.size      = size;
   tc.count     = -1;        /* indicate to skip this trace function */

   _Unwind_Backtrace( hhh_traceCallback, &tc );

   return tc.count;
}

#endif // CLANG_CAN_HANDLE_THIS
