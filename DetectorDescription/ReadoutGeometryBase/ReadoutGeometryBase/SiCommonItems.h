/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SiCommonItems.h
 */

#ifndef READOUTGEOMETRYBASE_SICOMMONITEMS_H
#define READOUTGEOMETRYBASE_SICOMMONITEMS_H

class AtlasDetectorID;

/**
 * Message Stream Member
 */
#include "AthenaKernel/MsgStreamMember.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthMessaging.h"


#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <mutex>

namespace InDetDD {

  /**
   * @class SiCommonItems
   *
   * Helper class to concentrate common items, such as the pointer to the IdHelper,
   * the lorentzAngle tool or the information about the solenoidal frame.
   *
   * To be used for Pixel and SCT.
   *
   * SiCommonItems is owned by SiDetectorManager.
   * 
   * @author: Grant Gorfine
   * mondified & maintained: Nick Styles, Andreas Salzburger
   */

  class SiCommonItems
    : public AthMessaging
  {
    
  public:
 
    /**
     * Constructor
     */
    SiCommonItems(const AtlasDetectorID* const idHelper);

    /**
     * @name Getter methods
     */
    //@{
    const AtlasDetectorID* getIdHelper() const;
    const ISiLorentzAngleTool* lorentzAngleTool() const;
    //@}

    /**
     * @name Setter methods
     */
    //@{
    void setLorentzAngleTool(const ISiLorentzAngleTool* lorentzAngleTool);
    //@}

  private:
    /**
     * IdHelper
     */
    const AtlasDetectorID* m_idHelper; 

    /**
     * LorentzAngle tool
     */
    const ISiLorentzAngleTool* m_lorentzAngleTool;

  };
    
    
  inline const AtlasDetectorID* SiCommonItems::getIdHelper() const
  {
    return m_idHelper;
  }
    

} // End namespace InDetDD

#endif // READOUTGEOMETRYBASE_SICOMMONITEMS_H
