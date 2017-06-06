/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FCALConstructionH62004
// Insert the LAr FCAL into a pre-defined mother volume.
// Author: Joe Boudreau August 204

#ifndef __FCALConstructionH62004_H__
#define __FCALConstructionH62004_H__

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

// Forward declarations
class ISvcLocator;

namespace LArGeo {

  class FCALConstructionH62004 
  {
  public:

    // Constructor;
    FCALConstructionH62004();

    // Destructor:
    virtual ~FCALConstructionH62004();

    // Get the envelope containing this detector.
    GeoVFullPhysVol* GetEnvelope();

    // Set a limit on cell number (for Visualization only);
    void setFCALVisLimit(int maxCell) {m_VisLimit    = maxCell;}

  private: 

    // It is illegal to copy a FCALConstructionH62004:
    FCALConstructionH62004 (const FCALConstructionH62004 &);

    // It is illegal to assign a FCALConstructionH62004:
    FCALConstructionH62004 & operator= (const FCALConstructionH62004 &);

    
    // full physical volumes for absorbers
    GeoFullPhysVol* m_absPhysical1, * m_absPhysical2, * m_absPhysical3;

    int m_VisLimit;

    ISvcLocator*      m_svcLocator;
    IRDBRecordset_ptr m_fcalElectrode;
    IRDBRecordset_ptr m_fcalMod;
  };

 

}  // namespace LArGeo

#endif // __FCALConstructionH62004_H__
