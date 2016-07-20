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

  int m_printMessages; // FIXME not used?

  TRTParameters* m_pParameters;
  TRTProcessingOfEndCapHits* m_pProcessingOfEndCapHits;

  int m_verboseLevel;

};

#endif //TRT_G4_SD_TRTParametersForEndCapHits_hh

