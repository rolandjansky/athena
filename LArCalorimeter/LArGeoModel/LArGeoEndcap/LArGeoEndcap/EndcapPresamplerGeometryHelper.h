/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EndcapPresamplerGeometryHelper.h
// Prepared 16-Jul-2003 Bill Seligman

// This is a separate geometry helper class for the endcap presampler.
// It provides any separate calculations or constants for the endcap
// presampler geometry.

#ifndef LARGEOENDCAP_ENDCAPPRESAMPLERGEOMETRYHELPER_H
#define LARGEOENDCAP_ENDCAPPRESAMPLERGEOMETRYHELPER_H

namespace LArGeo {

  // Forward declarations.
  class VDetectorParameters;

  class EndcapPresamplerGeometryHelper 
  {
  public:

    // Accessor for pointer to the singleton.
    static EndcapPresamplerGeometryHelper* GetInstance();

    // "zShift" is the z-distance (cm) that the EM endcap is shifted
    // (due to cabling, etc.)
    float zShift() const { return m_zShift; }

    // 15-Jan-2002 WGS: A "lookup" functions for detector measurements,
    // sizes, and other values.
    enum kValue {
      rMinEndcapPresampler,
      rMaxEndcapPresampler,
      zEndcapPresamplerFrontFace,
      zEndcapPresamplerBackFace,
      EndcapPresamplerHalfThickness,
      EndcapPresamplerZpositionInMother
    };
    double GetValue(const kValue);

  protected:
    // The constructor is protected according to the singleton design
    // pattern.
    EndcapPresamplerGeometryHelper();

  private:
    
    // Pointer to source of detector parameters.
    VDetectorParameters* m_parameters;
    
    // Store the endcap offset from the description:
    float m_zShift;
  };

} // namespace LArGeo

#endif // __EndcapPresamplerGeometryHelper_H__
