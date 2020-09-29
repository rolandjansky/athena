/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/Route.h"

#include "InDetServMatGeoModel/ServiceVolume.h"

#include <iostream>
using namespace std;

ServiceVolume* Route::entryVolume( double pos, bool ascending, Athena::MsgStreamMember& msg) const 
{
  msg << MSG::DEBUG  << "entering entryVolume with ascending = " << ascending 
       << " and pos = " << pos << endmsg;

  if (volumes().empty()) return 0;

  //  if (volumes().front()->contains(pos)) return volumes().front();
  //  else if (volumes().back()->contains(pos)) return volumes().back();
  //  else {
  //should iterate to find exit volume
  if ( ascending) {
    for (VolumeContainer::const_iterator i=volumes().begin(); i!=volumes().end(); ++i) {

      msg << MSG::DEBUG  << "Comparing " << pos << " and " << (**i).position() << endmsg;

      if (pos < (**i).position()) {
	msg << MSG::DEBUG  << "volume at pos " << (**i).radius() 
	     << ", " << (**i).zPos() << " contains exit point" << endmsg;
	return *i;
      }
      else {
	msg << MSG::DEBUG  << "volume at pos " << (**i).radius() 
	     << ", " << (**i).zPos() << " does not contain exit point" << endmsg;
      }
    }
    return volumes().front();  // catch-all
  } 
  else {
    for (VolumeContainer::const_reverse_iterator i=volumes().rbegin(); i!=volumes().rend(); ++i) {
      if ((**i).contains(pos)) return *i;
    }
    return volumes().back(); // catch-all
  }
  //}
}

ServiceVolume* Route::exitVolume( bool ascending, Athena::MsgStreamMember& msg) const 
{

  msg << MSG::DEBUG  << "entering exitVolume with ascending = " << ascending 
       << " and route.exit() = " << exit() << endmsg;

  if (m_exitVolume != 0) return m_exitVolume;

  // returns the volume closest to the exit point, coming from the given direction
  if (volumes().empty()) return 0;
  if ( ascending) {
    for (VolumeContainer::const_iterator i=volumes().begin(); i!=volumes().end(); ++i) {
      if ((**i).contains(exit())) return *i;
      else {
	msg << MSG::DEBUG  << "volume at pos " << (**i).radius() 
	     << ", " << (**i).zPos() << " does not contain exit point" << endmsg;
      }
    }
    return volumes().back();  // catch-all
  } 
  else {
    for (VolumeContainer::const_reverse_iterator i=volumes().rbegin(); i!=volumes().rend(); ++i) {
      if ((**i).contains(exit())) return *i;
    }
    return volumes().front(); // catch-all
  }
}

