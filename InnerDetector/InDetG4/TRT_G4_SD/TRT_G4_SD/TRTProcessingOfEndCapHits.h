/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRTProcessingOfEndCapHits_hh
#define TRTProcessingOfEndCapHits_hh

#include "globals.hh"
#include "AthenaKernel/MsgStreamMember.h"

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

    MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
    bool msgLevel (MSG::Level lvl) const  { return m_msg.get().level() <= lvl; }
  

  private:
    TRTProcessingOfEndCapHits (const TRTProcessingOfEndCapHits&);
    TRTProcessingOfEndCapHits& operator= (const TRTProcessingOfEndCapHits&);
    void Initialize();
    void DeleteArrays();

    int printMessages;

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

    mutable Athena::MsgStreamMember m_msg;

};

#endif
