/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header
#include "RecordingEnvelope.h"

// Athena headers
#include "CxxUtils/make_unique.h"
#include "MCTruth/TrackHelper.h"

//G4 headers
#include "G4PhysicalVolumeStore.hh"
#include "G4Step.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VSolid.hh"


RecordingEnvelope::RecordingEnvelope(const std::string& envelopeVolumeName, const std::string& trackRecordCollectionName):
  m_level(-1),
  m_logicalVolume(nullptr),
  m_envelopeVolumeName(envelopeVolumeName),
  m_trackRecordCollection(trackRecordCollectionName)
{

}

RecordingEnvelope::~RecordingEnvelope()
{

}

bool RecordingEnvelope::checkDaughters(const G4LogicalVolume *possibleParent, const G4VPhysicalVolume *thePhysicalVolume, int& level) const
{
  if (possibleParent->IsDaughter(thePhysicalVolume))
   {
     ++level;
     return true;
   }
  // Otherwise Loop over the daughters
  const G4int nDaughters(possibleParent->GetNoDaughters());
  for(G4int daughter(0); daughter<nDaughters; ++daughter)
      {
        const G4VPhysicalVolume* daughterPhysVol = possibleParent->GetDaughter(daughter);
        const G4LogicalVolume* daughterLogVol = daughterPhysVol->GetLogicalVolume();
        if (this->checkDaughters(daughterLogVol, thePhysicalVolume,level))
          {
            ++level;
            return true;
          }
      }
   return false;
}
bool RecordingEnvelope::Initialize()
{
  const G4VPhysicalVolume *thePhysicalVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(m_envelopeVolumeName,false);
  m_logicalVolume=thePhysicalVolume->GetLogicalVolume();
  const G4LogicalVolume * logicalWorld = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume();
  m_level=0;
  return this->checkDaughters(logicalWorld, thePhysicalVolume,m_level);
}

void RecordingEnvelope::BeginOfEvent()
{
  if (!m_trackRecordCollection.isValid()) m_trackRecordCollection = CxxUtils::make_unique<TrackRecordCollection>(m_trackRecordCollection.name());
  return;
}
void RecordingEnvelope::AddTrackRecord(const G4Step* aStep)
{
  const std::string pname = aStep->GetTrack()->GetDefinition()->GetParticleName();
  const int pdgcode = (pname=="geantino") ? 999 : aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
  TrackHelper trHelp(aStep->GetTrack());
  const int barcode = trHelp.GetBarcode();

  G4StepPoint *postStep=aStep->GetPostStepPoint();
  G4ThreeVector pos=postStep->GetPosition();
  G4ThreeVector mom=postStep->GetMomentum();
  const double ener=postStep->GetTotalEnergy();
  const double time=postStep->GetGlobalTime();

  G4StepPoint *preStep=aStep->GetPreStepPoint();
  G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();

  m_trackRecordCollection->Emplace(pdgcode,ener,mom,pos,time,barcode,preVol->GetName());

  return;
}
