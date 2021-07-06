/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// VolumeExcluder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKVOLUMES_VOLUMEEXCLUDER_H
#define TRKVOLUMES_VOLUMEEXCLUDER_H

//Trk
#include "TrkDetDescrUtils/AreaExcluder.h"
#include "TrkVolumes/Volume.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MsgStream;

namespace Trk {

  class AreaExcluder;

/** @class VolumeExcluder
    removes explicit dependence of Subtracted*Surface on TrkVolumes          

   @author sarka.todorova@cern.ch 
  */
   
   class VolumeExcluder: public AreaExcluder {
        
      public:
        /** Default constructor */
        VolumeExcluder();
        
        /** Explicit constructor with volume */
        VolumeExcluder(Volume* vol);
        
        /** copy constructor */
        VolumeExcluder(const VolumeExcluder& ex);
        
        /** Destructor */
        virtual ~VolumeExcluder();
        
        /** Assignment operator */
        VolumeExcluder& operator=(const VolumeExcluder &vol);
        
        /** Pseudo-constructor */
        VolumeExcluder* clone() const;
         
        /** First bin from global position */
        bool inside(const Amg::Vector3D& gp, double tol=0.) const;

        /** Acces the subtracted volume */
        const Volume* volume() const;
        Volume* volume();
        /** Output Method for MsgStream, to be overloaded by child classes */
        MsgStream& dump(MsgStream& sl) const ;
        
        /** Output Method for std::ostream, to be overloaded by child classes */
        std::ostream& dump(std::ostream& sl) const;
 
     private:
        Volume* m_vol;

   };      

   inline bool VolumeExcluder::inside(const Amg::Vector3D& gp, double tol) const
    {  return( m_vol->inside(gp,tol) ); }

   inline const Volume* VolumeExcluder::volume() const
    {  return( m_vol ); }

   inline Volume* VolumeExcluder::volume()
    {  return( m_vol ); }


} // end of namespace Trk

#endif // TRKVOLUMES_VOLUMEEXCLUDER

