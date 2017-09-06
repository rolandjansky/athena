/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SimHelpers/SecondaryTracksHelper.h"
#include "G4EventManager.hh"


// Default constructor delegates to 2nd constructor and retrieves the
// tracking manager automatically from the global event manager
SecondaryTracksHelper::SecondaryTracksHelper()
  : SecondaryTracksHelper( G4EventManager::GetEventManager()->GetTrackingManager() )
{
}

SecondaryTracksHelper::SecondaryTracksHelper(const G4TrackingManager *tm)
{
  m_theTrackingManager=tm;
  m_currentNrOfSecondaries=0;
}

void SecondaryTracksHelper::SetTrackingManager(const G4TrackingManager *tm)
{
  m_theTrackingManager=tm;
}

const G4TrackingManager *SecondaryTracksHelper::GetTrackingManager()
{
  return m_theTrackingManager;
}

int SecondaryTracksHelper::NrOfNewSecondaries()
{
  int value=0;
  //std::cout<<" NrOfNewSecondaries "<<m_currentNrOfSecondaries <<
  //           " "<<CurrentNrOfSecondaries()<<std::endl;
  int curr=CurrentNrOfSecondaries();
  if (curr>m_currentNrOfSecondaries)
  {
    value=curr-m_currentNrOfSecondaries;
    m_currentNrOfSecondaries=curr;
  }
  return value;
}

std::vector<G4Track*> SecondaryTracksHelper::GetSecondaries(int ntracks) const
{
  std::vector<G4Track*> vTrack;
  int iSize=m_theTrackingManager->GimmeSecondaries()->size();
  for (int i=iSize-1;i>iSize-ntracks-1;i--)
    vTrack.push_back((*(m_theTrackingManager->GimmeSecondaries()))[i]);
  return vTrack;
}
