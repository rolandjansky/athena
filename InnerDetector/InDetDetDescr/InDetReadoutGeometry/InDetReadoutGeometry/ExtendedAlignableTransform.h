/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ExtendedAlignableTransform.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_EXTENDEDALIGNABLETRANSFORM_H
#define INDETREADOUTGEOMETRY_EXTENDEDALIGNABLETRANSFORM_H

#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"

namespace InDetDD {

    /** @class ExtendedAlignableTransform 
    
    Class to hold alignable transform plus a pointer to the child volume and
    optionally a frame volume.

    @author: Grant Gorfine
    - modified & maintained: Nick Styles & Andreas Salzburger 
    */
    
    class ExtendedAlignableTransform
    {
    
    public:
    
      ExtendedAlignableTransform(GeoAlignableTransform * alignableTransform, 
    			     const GeoVFullPhysVol * child,
    			     const GeoVFullPhysVol * frame = 0)
        : m_alignableTransform(alignableTransform),
          m_child(child),
          m_frame(frame)
      {};
    
      GeoAlignableTransform * alignableTransform() {return m_alignableTransform;}
      const GeoVFullPhysVol * child()  {return m_child;}
      const GeoVFullPhysVol * frame()  {return m_frame;}
    
    private:
      
      GeoAlignableTransform * m_alignableTransform;
      const GeoVFullPhysVol * m_child;
      const GeoVFullPhysVol * m_frame;
    
    };


} // end namespace 

#endif // InDetDDINDETREADOUTGEOMETRY_EXTENDEDALIGNABLETRANSFORM_H
