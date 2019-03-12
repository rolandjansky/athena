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
#include "CxxUtils/checker_macros.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"


#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <mutex>

// mutable Athena::MsgStreamMember issues warnings.
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

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
          mutable HepGeom::Transform3D m_solenoidFrame ATLAS_THREAD_SAFE; // Guarded by m_mutex
          const ISiLorentzAngleTool *m_lorentzAngleTool;

          mutable std::mutex m_mutex;
    };
    
    
    inline const AtlasDetectorID* SiCommonItems::getIdHelper() const
    {
      return m_idHelper;
    }
    
    
    inline const HepGeom::Transform3D & SiCommonItems::solenoidFrame() const
    {
      std::lock_guard<std::mutex> lock{m_mutex};
      return m_solenoidFrame;
      // This reference might be changed by setSolenoidFrame.
      // However, it occurrs very rarely.
    }
    
    

} // End namespace InDetDD

#endif // INDETREADOUTGEOMETRY_SICOMMONITEMSS_H
