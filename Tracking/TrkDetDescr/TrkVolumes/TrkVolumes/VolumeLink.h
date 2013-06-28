/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VolumeLink.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_VOLUMELINK_H
#define TRKVOLUMES_VOLUMELINK_H

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
            
        /** update method for link-setting */
        void updateLink(const Volume* vol) const;
        
        const Volume* volume( void ) const;
          
      private:    
        mutable const Volume* m_linkVolume;         
    };

  // update the link
  inline void  VolumeLink::updateLink(const Volume* vol) const
    { m_linkVolume = vol; };    

  // forward the link
  inline const Volume* VolumeLink::volume( void ) const
    { return m_linkVolume; };    
            
} // end of namespace

#endif // TRKVOLUMES_VOLUMELINK_H
