/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackFastSimSD_H
#define TrackFastSimSD_H

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "TrackRecord/TrackRecordCollection.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include <string>

class G4Track;
class G4HCofThisEvent;
class G4TouchableHistory;

class TrackFastSimSD : public FADS::FadsSensitiveDetector
{
public:
  TrackFastSimSD(std::string name) : FADS::FadsSensitiveDetector(name) , m_trackRecordCollection(0) , m_colName("NeutronBG") {}
  ~TrackFastSimSD() {}

  void   Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) { return false; }
  void   EndOfEvent(G4HCofThisEvent*E);
  void   WriteTrack(const G4Track*, const bool, const bool);
  std::string CollectionName() { return m_colName; }
  void   SetCollectionName(const std::string s);

private:
  TrackRecordCollection *m_trackRecordCollection;
  std::string m_colName;
  AthenaHitsCollectionHelper m_hitCollHelp;

};

#endif
