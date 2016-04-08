/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HECConstructionH62002
// Insert the LAr FCAL into a pre-defined mother volume.
// Author: Joe Boudreau August 204


#ifndef LARGEOH62002ALGS_HECCONSTRUCTIONH62002_H
#define LARGEOH62002ALGS_HECCONSTRUCTIONH62002_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"



// Forward declarations

namespace LArGeo {

  class HECConstructionH62002 
  {
  public:

    // Constructor;
    HECConstructionH62002();

    // Destructor:
    virtual ~HECConstructionH62002();

    // Get the envelope containing this detector.
    GeoVFullPhysVol* GetEnvelope();  // h6Phys is GeoVPhysVol   


  private: 

    // It is illegal to copy a HECConstructionH62002:
    HECConstructionH62002 (const HECConstructionH62002 &);

    // It is illegal to assign a HECConstructionH62002:
    HECConstructionH62002 & operator= (const HECConstructionH62002 &);

    
    // volumes that are private member variables:
    GeoFullPhysVol*  m_h6Phys;

    //static LArGeo::VDetectorParameters* m_parameters;
    //static VDetectorParameters* HECConstructionH62002::m_parameters;


  };

 

}  // namespace LArGeo

#endif // LARGEOH62002ALGS_HECCONSTRUCTIONH62002_H
