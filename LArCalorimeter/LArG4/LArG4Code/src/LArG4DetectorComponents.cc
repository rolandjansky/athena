/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArG4DetectorComponents
// Maintain a list of detector components.
// 05-Jun-2003 Bill Seligman

#include "LArG4Code/LArG4DetectorComponents.h"
#include "LArG4Code/LArVolumeInterface.h"

#include <map>

// Standard implementation of a singleton pattern.

LArG4DetectorComponents* LArG4DetectorComponents::GetInstance()
{
  static LArG4DetectorComponents instance;
  return &instance;
}

G4bool LArG4DetectorComponents::m_zNeg = false;

LArG4DetectorComponents::LArG4DetectorComponents() {;}

LArG4DetectorComponents::~LArG4DetectorComponents() {;}


// Add a detector component to the list.
void LArG4DetectorComponents::SetDetectorComponent( const EDetectorComponent e, LArVolumeInterface* v )
{
  m_detectorMap[e] = v;
}


// Fetch a component from the list.  If we can't find it, just return
// a null pointer.
LArVolumeInterface* LArG4DetectorComponents::GetDetectorComponent( const EDetectorComponent e ) const
{
  m_detectorMap_ptr i = m_detectorMap.find(e);
  
  if ( i == m_detectorMap.end() )
    return 0;

  // Reminder:
  // i == a pointer to one entry in detectorMap
  // (*i) = a pair<EDetectorComponent, LArVolumeInterface*>
  // (*i).second = a LArVolumeInterface*

  return (*i).second;
}
