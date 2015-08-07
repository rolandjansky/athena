/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_G4_SD_TRTProcessingOfEndCapHits_hh
#define TRT_G4_SD_TRTProcessingOfEndCapHits_hh

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

  int printMessages; //FIXME obsolete?

  int sectorsABC;
  int testLocalCoordinatesOfHits;

  int numberOfStrawsInPlanesAB;
  int numberOfStrawsInPlaneC;

  int numberOfWheelsAB;

  int numberOfStrawsInSectorsAB;
  int numberOfStrawsInSectorC;
  int initialStrawIDInSectorsAB;
  int initialStrawIDInSectorC;

  int numberOfStrawsInIDSectorsAB;
  int numberOfStrawsInIDSectorC;

  TRTParameters* pParameters;
  TRTSensitiveDetector* pSensitiveDetector;

  int verboseLevel;
};

#endif // TRT_G4_SD_TRTProcessingOfEndCapHits_hh
