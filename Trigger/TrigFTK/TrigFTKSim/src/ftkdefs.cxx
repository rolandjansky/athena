/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/ftkdefs.h"
#include <stdint.h> // fuer uint8_t/uint32_t
#include <stddef.h> // fuer size_t
#include <TObjArray.h>
#include <TObjString.h>

namespace ftk {

   uint32_t adler32chksum(const void *buf, size_t buflength) {
      const uint8_t *buffer = (const uint8_t*)buf;
      uint32_t s1 = 1;
      uint32_t s2 = 0;
      for (size_t n = 0; n < buflength; n++) {
	 s1 = (s1 + buffer[n]) % MOD_ADLER;
	 s2 = (s2 + s1) % MOD_ADLER;
      }
      return (s2 << 16) | s1;
   }

   TString StripFileName(TString path) {
      TObjArray* as = path.Tokenize("/");
      TObjString* asend = (TObjString*)as->At(as->GetEntries()-1);
      TString filename = asend->GetString();
      delete as;
      return filename;
   }

}

