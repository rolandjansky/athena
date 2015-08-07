/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTParametersForEndCapHits_hh
#define TRT_G4_SD_TRTParametersForEndCapHits_hh

class TRTParameters;
class TRTProcessingOfEndCapHits;

class TRTParametersForEndCapHits
{
public:
  TRTParametersForEndCapHits(TRTProcessingOfEndCapHits*);
  ~TRTParametersForEndCapHits();

private:
  void DefineParameters() const;
  void PrintParameters() const;

  int printMessages;

  TRTParameters* pParameters;
  TRTProcessingOfEndCapHits* pProcessingOfEndCapHits;

  int verboseLevel;

};

#endif //TRT_G4_SD_TRTParametersForEndCapHits_hh

