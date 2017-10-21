/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HECConstructionH62004


#ifndef __HECConstructionH62004_H__
#define __HECConstructionH62004_H__

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"



// Forward declarations

namespace LArGeo {

  class HECConstructionH62004 
  {
  public:

    // Constructor;
    HECConstructionH62004();

    // Destructor:
    virtual ~HECConstructionH62004();

    // Get the envelope containing this detector.
    GeoVFullPhysVol* GetEnvelope();  // h6Phys is GeoVPhysVol   


  private: 

    // It is illegal to copy a HECConstructionH62004:
    HECConstructionH62004 (const HECConstructionH62004 &);

    // It is illegal to assign a HECConstructionH62004:
    HECConstructionH62004 & operator= (const HECConstructionH62004 &);

    
    // volumes that are private member variables:
    GeoFullPhysVol*  m_h6Phys;

    //static LArGeo::VDetectorParameters* m_parameters;
    //static VDetectorParameters* HECConstructionH62004::m_parameters;


  };

 

}  // namespace LArGeo

#endif // __HECConstructionH62004_H__
