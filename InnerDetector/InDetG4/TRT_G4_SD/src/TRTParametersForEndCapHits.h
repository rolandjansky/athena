/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTParametersForEndCapHits_hh
#define TRT_G4_SD_TRTParametersForEndCapHits_hh

#include "CxxUtils/checker_macros.h"

class TRTParameters;
class TRTProcessingOfEndCapHits;

class ATLAS_NOT_THREAD_SAFE TRTParametersForEndCapHits // Thread unsafe TRTParameters and TRTProcessingOfEndCapHits classes are used.
{
public:
  TRTParametersForEndCapHits(TRTProcessingOfEndCapHits*);
  ~TRTParametersForEndCapHits();

private:
  void DefineParameters() const;
  void PrintParameters() const;

  int m_printMessages; // FIXME not used?

  TRTParameters* m_pParameters;
  TRTProcessingOfEndCapHits* m_pProcessingOfEndCapHits;

  int m_verboseLevel;

};

#endif //TRT_G4_SD_TRTParametersForEndCapHits_hh

