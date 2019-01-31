/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiCommonItems.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETREADOUTGEOMETRY_SICOMMONITEMS_H
#define INDETREADOUTGEOMETRY_SICOMMONITEMS_H

class AtlasDetectorID;

// Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

namespace InDetDD {

    /** @class SiCommonItems
    
        Helper class to concentrate common items, such as the pointer to the IdHelper,
        the lorentzAngle service or the information about the solenoidal frame.
        
        To be used for Pixel and SCT.
        
        @author: Grant Gorfine
        mondified & maintained: Nick Styles, Andreas Salzburger
        */

    class SiCommonItems: public RCBase 
    {
    
        public:
        
          SiCommonItems(const AtlasDetectorID* const idHelper);
          
          const AtlasDetectorID* getIdHelper() const;
          const HepGeom::Transform3D & solenoidFrame() const;
          void setSolenoidFrame(const HepGeom::Transform3D & transform) const; 
          void setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool);
          const ISiLorentzAngleTool * lorentzAngleTool() const;

          //Declaring the Message method for further use
          MsgStream& msg (MSG::Level lvl) const { return m_msg.get() << lvl; }
        
          //Declaring the Method providing Verbosity Level
          bool msgLvl (MSG::Level lvl) const { return m_msg.get().level() <= lvl; }
        
        private:
        
          //Declaring private message stream member.
          mutable Athena::MsgStreamMember m_msg;
          
          const AtlasDetectorID* m_idHelper; 
          mutable HepGeom::Transform3D m_solenoidFrame;
          const ISiLorentzAngleTool *m_lorentzAngleTool;
        
    };
    
    
    inline const AtlasDetectorID* SiCommonItems::getIdHelper() const
    {
      return m_idHelper;
    }
    
    
    inline const HepGeom::Transform3D & SiCommonItems::solenoidFrame() const
    {
      return m_solenoidFrame;
    }
    
    

} // End namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SICOMMONITEMSS_H
