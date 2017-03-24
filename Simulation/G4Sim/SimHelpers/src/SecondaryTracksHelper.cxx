/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "SimHelpers/SecondaryTracksHelper.h"



const G4TrackingManager* SecondaryTracksHelper::s_theTrackingManager=0;

SecondaryTracksHelper::SecondaryTracksHelper()
{
	m_currentNrOfSecondaries=0;
}
SecondaryTracksHelper::SecondaryTracksHelper(const G4TrackingManager *tm)
{
	s_theTrackingManager=tm;
	m_currentNrOfSecondaries=0;
}
void SecondaryTracksHelper::SetTrackingManager(const G4TrackingManager *tm)
{
	s_theTrackingManager=tm;
}
const G4TrackingManager *SecondaryTracksHelper::GetTrackingManager()
{
	return s_theTrackingManager;
}
int SecondaryTracksHelper::NrOfNewSecondaries()
{
	int value=0;
//	std::cout<<" NrOfNewSecondaries "<<m_currentNrOfSecondaries << 
//	         " "<<CurrentNrOfSecondaries()<<std::endl;
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
	int iSize=s_theTrackingManager->GimmeSecondaries()->size();
	for (int i=iSize-1;i>iSize-ntracks-1;i--) 
		vTrack.push_back((*(s_theTrackingManager->GimmeSecondaries()))[i]);
	return vTrack;
}
