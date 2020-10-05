/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTProcessingOfEndCapHits_hh
#define TRT_G4_SD_TRTProcessingOfEndCapHits_hh

#include "CxxUtils/checker_macros.h"

class TRTParameters;
class TRTSensitiveDetector;

class G4Step;

class TRTProcessingOfEndCapHits
{
  friend class TRTParametersForEndCapHits;

public:
  TRTProcessingOfEndCapHits(TRTSensitiveDetector*);
  ~TRTProcessingOfEndCapHits();

  bool ProcessHit(G4Step*);

private:
  TRTProcessingOfEndCapHits (const TRTProcessingOfEndCapHits&);
  TRTProcessingOfEndCapHits& operator= (const TRTProcessingOfEndCapHits&);
  void Initialize();
  void DeleteArrays();

  int m_printMessages; //FIXME obsolete?

  int m_sectorsABC;
  int m_testLocalCoordinatesOfHits;

  int m_numberOfStrawsInPlanesAB;
  int m_numberOfStrawsInPlaneC;

  int m_numberOfWheelsAB;

  int m_numberOfStrawsInSectorsAB;
  int m_numberOfStrawsInSectorC;
  int m_initialStrawIDInSectorsAB;
  int m_initialStrawIDInSectorC;

  int m_numberOfStrawsInIDSectorsAB;
  int m_numberOfStrawsInIDSectorC;

  const TRTParameters* m_pParameters;
  TRTSensitiveDetector* m_pSensitiveDetector;

  int m_verboseLevel;
};

#endif // TRT_G4_SD_TRTProcessingOfEndCapHits_hh
