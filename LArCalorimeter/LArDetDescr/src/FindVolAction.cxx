/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArDetDescr/FindVolAction.h"

FindVolAction::FindVolAction (PVConstLink startPV,
			      std::string volumeName,
			      int identifier)
  :GeoVolumeAction (GeoVolumeAction::TOP_DOWN),
   m_volumeExists(false),
   m_volumeName(volumeName),
   m_identifier(identifier),
   m_volume(0)
{
  //std::cout << " ---> FindVolAction constructed\n";
  startPV->apply(this);
}

FindVolAction::~FindVolAction ()
{
}

PVConstLink FindVolAction::getVolume () const
{
  return m_volume;
}

HepGeom::Transform3D FindVolAction::getGlobalTransform () const
{
  return m_globalTransform;
}

bool FindVolAction::volumeExists() const
{
  return m_volumeExists;
}

void FindVolAction::handleVPhysVol (const GeoVPhysVol *pv) 
{
  GeoTraversalState* state = getState();
  Query<int> Qint = state->getId();
  std::string pvName = state->getName();
  std::string lvName = pv->getLogVol()->getName();
  
  // Print out :
  /*
  std::cout << " VOL ACTION. Visiting = " << pvName
	    << " " << lvName << " ";
  if(Qint.isValid())
    std::cout << Qint;
  else
    std::cout << "xxx";
  std::cout << "\n";
  */
  
  // If there is a copy number use it :
  if ( m_identifier >= 0 ) {
    
    if((pvName==m_volumeName || lvName==m_volumeName) &&
       Qint.isValid() && Qint == m_identifier)
      {
	//std::cout << " ***  FOUND! " << "\n";
	m_volumeExists = true;
	m_globalTransform = state->getAbsoluteTransform();
	m_volume = pv;
	terminate();
      }
  }
  // If not :
  else {
    
    if((pvName==m_volumeName || lvName==m_volumeName) ) {
      //std::cout << " ***  FOUND! " << "\n";
      m_volumeExists = true;
      m_globalTransform = state->getAbsoluteTransform();
      m_volume = pv;
      terminate();
    } 
  }
}


