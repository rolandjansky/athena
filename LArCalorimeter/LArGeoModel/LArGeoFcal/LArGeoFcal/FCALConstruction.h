/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class FCALConstruction
 *
 * @brief Insert the LAr FCAL into a pre-defined mother volume.
 *
 * @author Joe Boudreau August 2004
 */

#ifndef LARGEOFCAL_FCALCONSTRUCTION_H
#define LARGEOFCAL_FCALCONSTRUCTION_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

// Forward declarations
class ISvcLocator;


namespace LArGeo {

  class FCALConstruction 
  {
  public:

    // Constructor;
    FCALConstruction();

    // Destructor:
    virtual ~FCALConstruction();

    // Get the envelope containing this detector.
    GeoVFullPhysVol* GetEnvelope(bool bPos);

    // Set a limit on cell number (for Visualization only);
    void setFCALVisLimit(int maxCell) {m_VisLimit    = maxCell;}

    // Set fullGeo flag
    void setFullGeo(bool flag) {m_fullGeo = flag;}

  private: 

    // It is illegal to copy a FCALConstruction:
    FCALConstruction (const FCALConstruction &);

    // It is illegal to assign a FCALConstruction:
    FCALConstruction & operator= (const FCALConstruction &);

    
    // volumes that are private member variables:
    GeoFullPhysVol*  m_fcalPhysical;

    // full physical volumes for absorbers
    GeoFullPhysVol* m_absPhysical1, * m_absPhysical2, * m_absPhysical3;

    int m_VisLimit;

    ISvcLocator*         m_svcLocator;
    IRDBRecordset_ptr    m_fcalMod;
    IRDBRecordset_ptr    m_LArPosition;

    bool             m_fullGeo;  // true->FULL, false->RECO
  };

 

}  // namespace LArGeo

#endif // __FCALConstruction_H__
