/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTProcessingOfBarrelHits_hh
#define TRT_G4_SD_TRTProcessingOfBarrelHits_hh

#include "CxxUtils/checker_macros.h"

class TRTParameters;
class TRTSensitiveDetector;

class G4Step;

class ATLAS_NOT_THREAD_SAFE TRTProcessingOfBarrelHits // Thread unsafe TRTParameters and TRTSensitiveDetector classes are used.
{
  friend class TRTParametersForBarrelHits;

public:
  TRTProcessingOfBarrelHits(TRTSensitiveDetector*);
  ~TRTProcessingOfBarrelHits();

  bool ProcessHit(G4Step*);

private:
  TRTProcessingOfBarrelHits (const TRTProcessingOfBarrelHits&);
  TRTProcessingOfBarrelHits& operator= (const TRTProcessingOfBarrelHits&);
  void Initialize();
  int GetLayerID1(const int&, int&) const;
  int GetLayerID2(const int&, int&) const;
  void DeleteArrays();

  int m_printMessages; //FIXME obsolete?

  int m_barrelIdentifier;
  int m_testLocalCoordinatesOfHits;

  int m_numberOfStrawLayersA;
  int m_numberOfStrawLayersB;
  int m_numberOfStrawLayersC;

  int m_numberOfStrawsA;
  int m_numberOfStrawsB;
  int m_numberOfStrawsC;

  int* m_integralDistributionOfStrawsA;
  int* m_integralDistributionOfStrawsB;
  int* m_integralDistributionOfStrawsC;

  int* m_strawIDToLayerIDA;
  int* m_strawIDToLayerIDB;
  int* m_strawIDToLayerIDC;

  TRTParameters* m_pParameters;
  TRTSensitiveDetector* m_pSensitiveDetector;

  int m_verboseLevel;
};

#endif //TRT_G4_SD_TRTProcessingOfBarrelHits_hh
