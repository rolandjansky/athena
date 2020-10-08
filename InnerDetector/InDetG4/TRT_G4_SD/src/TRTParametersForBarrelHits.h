/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTParametersForBarrelHits_hh
#define TRT_G4_SD_TRTParametersForBarrelHits_hh

#include "CxxUtils/checker_macros.h"

class TRTParameters;
class TRTProcessingOfBarrelHits;

class TRTParametersForBarrelHits
{
public:
  TRTParametersForBarrelHits(TRTProcessingOfBarrelHits*);
  ~TRTParametersForBarrelHits();

private:
  void DefineParameters() const;
  void PrintParameters() const;

  int m_printMessages; //FIXME not used

  const TRTParameters* m_pParameters;
  TRTProcessingOfBarrelHits* m_pProcessingOfBarrelHits;

  int m_verboseLevel;
};

#endif //TRT_G4_SD_TRTParametersForBarrelHits_hh
