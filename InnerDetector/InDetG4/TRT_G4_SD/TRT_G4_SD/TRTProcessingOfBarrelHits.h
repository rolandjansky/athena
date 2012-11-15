/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTProcessingOfBarrelHits_hh
#define TRTProcessingOfBarrelHits_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

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

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const  { return m_msg.get().level() <= lvl; }

  private:
    TRTProcessingOfBarrelHits (const TRTProcessingOfBarrelHits&);
    TRTProcessingOfBarrelHits& operator= (const TRTProcessingOfBarrelHits&);
    void Initialize();
    int GetLayerID1(const int&, int&) const;
    int GetLayerID2(const int&, int&) const;
    void DeleteArrays();

    int printMessages;

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

    mutable Athena::MsgStreamMember m_msg;

};

#endif
