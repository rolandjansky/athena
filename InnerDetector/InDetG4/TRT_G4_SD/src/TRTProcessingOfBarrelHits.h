/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTProcessingOfBarrelHits_hh
#define TRT_G4_SD_TRTProcessingOfBarrelHits_hh

class TRTParameters;
class TRTSensitiveDetector;

class G4Step;

class TRTProcessingOfBarrelHits
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

  int printMessages; //FIXME obsolete?

  int barrelIdentifier;
  int testLocalCoordinatesOfHits;

  int numberOfStrawLayersA;
  int numberOfStrawLayersB;
  int numberOfStrawLayersC;

  int numberOfStrawsA;
  int numberOfStrawsB;
  int numberOfStrawsC;

  int* integralDistributionOfStrawsA;
  int* integralDistributionOfStrawsB;
  int* integralDistributionOfStrawsC;

  int* strawIDToLayerIDA;
  int* strawIDToLayerIDB;
  int* strawIDToLayerIDC;

  TRTParameters* pParameters;
  TRTSensitiveDetector* pSensitiveDetector;

  int verboseLevel;
};

#endif //TRT_G4_SD_TRTProcessingOfBarrelHits_hh
