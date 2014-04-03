/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class TileGeoG4SD
// Sensitive detector for TileCal G4 simulations using TileGeoModel
//
// Author: Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>
//
// Major updates: July, 2013 (Sergey)
//
//************************************************************

#ifndef TileGeoG4SD_H
#define TileGeoG4SD_H 

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

using namespace FADS;

class TileGeoG4SDCalc;
class TileGeoG4LookupBuilder;

class G4Step;
class G4HCofThisEvent;
class G4String;

class IMessageSvc;
class MsgStream;



class TileGeoG4SD : public FadsSensitiveDetector
{
public:
    TileGeoG4SD (G4String name);
   ~TileGeoG4SD ();

    void   Initialize  (G4HCofThisEvent*);
    G4bool ProcessHits (G4Step*, G4TouchableHistory*);
    void   EndOfEvent  (G4HCofThisEvent*);

private:
    AthenaHitsCollectionHelper m_hitCollHelp;

    TileGeoG4SDCalc*          m_calc;
    TileGeoG4LookupBuilder*   m_lookup;

    IMessageSvc*  m_msgSvc;
    MsgStream*    m_log;

    bool m_debug;
    bool m_verbose;

};

#endif

