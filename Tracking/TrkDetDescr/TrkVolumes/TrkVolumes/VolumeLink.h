/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VolumeLink.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_VOLUMELINK_H
#define TRKVOLUMES_VOLUMELINK_H

#include "CxxUtils/checker_macros.h"

namespace Trk {

class Volume;

/** 
  @class VolumeLink
  LinkUtitlity for Volumes with update possibility 

  @author Andreas.Salzburger@cern.ch  
  */

class VolumeLink {
public:
  /** Default constructor */
  VolumeLink(){}
  /** Constructor with already existing Link */

  VolumeLink(const Volume* vol) :
    m_linkVolume(vol){}

  /** Copy Constructor*/
  VolumeLink(const VolumeLink& vlink) :
    m_linkVolume(vlink.m_linkVolume){}

  /** Destructor */
  ~VolumeLink(){}

  /** update method for link-setting const not thread safe */
  void  VolumeLink::updateLink(const Volume* vol) { 
    m_linkVolume = vol; 
  }    
  // forward the link
  const Volume* VolumeLink::volume( void ) const{ 
    return m_linkVolume; 
  }    

private:    
  const Volume* m_linkVolume;         
};
} // end of namespace

#endif // TRKVOLUMES_VOLUMELINK_H
