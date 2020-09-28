/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GlueVolumesDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKGEOMETRY_GLUEVOLUMESDESCRIPTOR_H
#define TRKGEOMETRY_GLUEVOLUMESDESCRIPTOR_H

// Trk include
#include "TrkVolumes/BoundarySurfaceFace.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
// STL
#include <map>
#include <vector>
#include "CxxUtils/checker_macros.h"

namespace Trk {

    class TrackingVolume;

    typedef std::map<BoundarySurfaceFace, std::vector<const TrackingVolume*> >::iterator GlueVolumeIterator;
    typedef std::map<BoundarySurfaceFace, std::vector<const TrackingVolume*> >::const_iterator GlueVolumeConstIterator;
    /** @class GlueVolumesDescriptor 
  
       Descriptor class to hold GlueVolumes of a TrackingGeometry object.
       Should ease the wrapping of a TrackingGeometry object describing one Detector
       by another one.

       @author Andreas.Salzburger@cern.ch
       */

    class GlueVolumesDescriptor {
      public: 
        /**Constructor */
        GlueVolumesDescriptor(){}

        /**Constructor - with arguments*/
        GlueVolumesDescriptor(const std::map<BoundarySurfaceFace, std::vector<const TrackingVolume*> >& glv);


        /**Desctructor */
        ~GlueVolumesDescriptor(){}
 
        /** register the volumes */
        void registerGlueVolumes(BoundarySurfaceFace, std::vector<const TrackingVolume*>&);
        void registerGlueVolumes ATLAS_NOT_THREAD_SAFE (BoundarySurfaceFace, std::vector<const TrackingVolume*>&) const;
     
        /** retrieve them again */
        const std::vector<const TrackingVolume*>& glueVolumes(BoundarySurfaceFace) const;

        /** retrieve the available Glue Faces */
        const std::vector<BoundarySurfaceFace>& glueFaces() const;


     private:
        std::map<BoundarySurfaceFace, std::vector<const TrackingVolume*> > m_glueVolumes;
        std::vector<BoundarySurfaceFace>                                   m_glueFaces;
        static  const std::vector<const TrackingVolume*>                           s_emptyVector;
   };

  inline void GlueVolumesDescriptor::registerGlueVolumes ATLAS_NOT_THREAD_SAFE (BoundarySurfaceFace bSurf, 
                                                                                       std::vector<const TrackingVolume*>& vols) const{

    const_cast<GlueVolumesDescriptor*>(this)->registerGlueVolumes(bSurf,vols);
  }

  inline const std::vector<BoundarySurfaceFace>& GlueVolumesDescriptor::glueFaces() const
  { return m_glueFaces; }

  /**Overload of << operator for both, MsgStream and std::ostream for debug output*/ 
  MsgStream& operator<<( MsgStream& sl, const GlueVolumesDescriptor& mprop);
  std::ostream& operator<<( std::ostream& sl, const GlueVolumesDescriptor& mprop);
     

}

#endif
