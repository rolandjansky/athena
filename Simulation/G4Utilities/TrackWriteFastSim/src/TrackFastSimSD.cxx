/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackWriteFastSim/TrackFastSimSD.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "MCTruth/TrackHelper.h"

#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4DynamicParticle.hh"

#include <cmath>

static  FADS::SensitiveDetectorEntryT<TrackFastSimSD> trackfastsd("TrackFastSimSD");

void TrackFastSimSD::Initialize(G4HCofThisEvent*)
{
  m_trackRecordCollection = m_hitCollHelp.RetrieveNonconstCollection<TrackRecordCollection>(m_colName);
}

void TrackFastSimSD::WriteTrack(const G4Track* track, const bool originPos, const bool originMom)
{
  if (!track) { ATH_MSG_ERROR ( "the track pointer was zero" ); return; }

  G4VPhysicalVolume *preVol=track->GetVolume();

  int pdgcode = (track->GetDefinition())?track->GetDefinition()->GetPDGEncoding():0;

  G4ThreeVector pos = originPos?track->GetVertexPosition():track->GetPosition();
  double ener=originMom?(track->GetVertexKineticEnergy()+track->GetDynamicParticle()->GetMass()):track->GetTotalEnergy();
  G4ThreeVector mom = track->GetMomentum();
  if (originMom){
    double mommag = std::sqrt(std::pow(ener,2)-std::pow(track->GetDynamicParticle()->GetMass(),2));
    mom = track->GetVertexMomentumDirection()*mommag;
  }

  double time=track->GetGlobalTime();
  TrackHelper trHelp(track);
  int barcode = trHelp.GetBarcode();

  //create the TimedTrackRecord
  TrackRecord *rec=new TrackRecord(pdgcode,ener,mom,pos,time,barcode,preVol?preVol->GetName():"Unknown");

  if (!m_trackRecordCollection){
    ATH_MSG_ERROR ( "No collection" );
    delete rec;
  } else m_trackRecordCollection->Insert(rec);
}

void TrackFastSimSD::EndOfEvent(G4HCofThisEvent* )
{
  if (m_trackRecordCollection) {
    if(!m_allowMods) {
      m_hitCollHelp.SetConstCollection<TrackRecordCollection>(m_trackRecordCollection);
    }
  }
}

void TrackFastSimSD::SetCollectionName(const std::string s)
{
  m_colName = s;
  if (m_trackRecordCollection) m_trackRecordCollection->setName(s);
}
