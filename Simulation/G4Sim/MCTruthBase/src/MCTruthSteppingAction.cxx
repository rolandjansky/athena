/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthBase/MCTruthSteppingAction.h"
#include "MCTruth/TrackHelper.h"
#include "TrackWriteFastSim/TrackFastSimSD.h"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"

#include <map>
#include <iostream>

MCTruthSteppingAction::MCTruthSteppingAction(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),sde(0),m_needToInitialize(true)
{

}

void MCTruthSteppingAction::AddRecordingEnvelope(std::string name, int lev,
                                                 std::string sdName)
{
  //G4cout << "MCTruthSteppingAction::AddRecordingEnvelope Name: " << name << ", Level: " << lev << ", sdName: " << sdName << G4endl;
  if (sdName.empty()) { sdName=name; }
  m_volumeMap[name]=lev;
  m_sdNameMap[name]=sdName;
}

void MCTruthSteppingAction::GetSensitiveDetectors()
{
  G4SDManager *SDM=G4SDManager::GetSDMpointer();
  //G4cout << "MCTruthSteppingAction::GetSensitiveDetectors m_sdNameMap.size() = " << m_sdNameMap.size() << G4endl;
  const VolName2SDNameMap::iterator endOfSDs = m_sdNameMap.end();
  for(VolName2SDNameMap::iterator sdItr = m_sdNameMap.begin();sdItr!=endOfSDs;++sdItr)
    {
      G4VSensitiveDetector *sd=SDM->FindSensitiveDetector(sdItr->second);
      if(sd)
        {
          m_sdPtrMap[sdItr->first]=sd;
          //G4cout << "MCTruthSteppingAction::UserSteppingAction adding entry in m_sdPtrMap for volume called " << sdItr->first << "!" << G4endl;
        }
      else
        {
          G4ExceptionDescription description;
          description << "AddRecordingEnvelope: Can't find SD with name " << sdItr->second << " in G4SDManager.";
          G4Exception("MCTruthSteppingAction", "NoRecEnvelopeSD", FatalException, description);
        }
    }
  m_needToInitialize=false;
  return;
}

void MCTruthSteppingAction::ListRecordingEnvelopes()
{
  VolName2LevelMapIterator it;
  for (it=m_volumeMap.begin();it!=m_volumeMap.end();it++)
    G4cout << "MCTruthSteppingAction::ListRecordingEnvelopes Recording Envelope -> " << it->second << G4endl;
}

void MCTruthSteppingAction::Step(const G4Step* aStep)
{
  if (m_volumeMap.size() == 0) return;

  TrackHelper tHelp(aStep->GetTrack());
  if (tHelp.IsSecondary()) return;

  G4StepPoint *preStep=aStep->GetPreStepPoint();
  G4StepPoint *postStep=aStep->GetPostStepPoint();

  G4VPhysicalVolume *preVol=preStep->GetPhysicalVolume();
  G4VPhysicalVolume *postVol=postStep->GetPhysicalVolume();

  if (preVol==postVol) return;

  G4TouchableHistory *preTHist=(G4TouchableHistory *)preStep->GetTouchable();
  G4TouchableHistory *postTHist=(G4TouchableHistory *)postStep->GetTouchable();
  int nLev1=preTHist->GetHistoryDepth();
  int nLev2=postTHist->GetHistoryDepth();

  if(m_needToInitialize)
    {
      this->GetSensitiveDetectors();
    }

  std::string vname1;
  VolName2LevelMapIterator it;
  for (it=m_volumeMap.begin();it!=m_volumeMap.end();it++)
    {
      int il=it->second;

      if (il<=(nLev1+1))
        {
          vname1=preTHist->GetVolume(nLev1-il+1)->GetName();
          if (vname1!=it->first) continue;

          if (il<=(nLev2+1))
            {
              if (vname1==postTHist->GetVolume(nLev2-il+1)->GetName()) continue;
            }

          G4Step *vStep=const_cast<G4Step *>(aStep);
          const G4VSensitiveDetector* theSD = m_sdPtrMap[vname1];
          if(theSD)
            {
              m_sdPtrMap[vname1]->Hit(vStep);
            }
          else
            {
              G4ExceptionDescription description;
              description << "UserSteppingAction no entry in m_sdPtrMap for " << vname1 << "!";
              G4Exception("MCTruthSteppingAction", "RecEnvSDMissing", FatalException, description);
            }
          break;
        }
    }

}

StatusCode MCTruthSteppingAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
