/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EndcapPresamplerGeometryHelper
// 27-Dec-2002 Bill Seligman

// This singleton class provides detector-description information and
// calculations for the Geant4 simulation. 

// 2-July-2003 Mikhail Leltchouk: local coordinates for determination
// of etaBin, phiBin at any Endcap Presamplerposition. 

#include "GaudiKernel/SystemOfUnits.h"
#include "LArGeoEndcap/EndcapPresamplerGeometryHelper.h"
#include "LArGeoCode/VDetectorParameters.h"

#include <cmath>
#include <string>
#include <iostream>


// Standard implementation of a singleton pattern.

LArGeo::EndcapPresamplerGeometryHelper* LArGeo::EndcapPresamplerGeometryHelper::GetInstance()
{
  static EndcapPresamplerGeometryHelper instance;
  return &instance;
}


LArGeo::EndcapPresamplerGeometryHelper::EndcapPresamplerGeometryHelper()
{
  // Constructor initializes the geometry helper information.

  // Access source of detector parameters.
  m_parameters = VDetectorParameters::GetInstance();

  // Get the endcap shift and out-of-time cut from the detector
  // parameters routine.
  m_zShift = m_parameters->GetValue("LArEMECZshift");
}


double LArGeo::EndcapPresamplerGeometryHelper::GetValue(const kValue a_valueType)
{
  // Look up a value based on name.
  switch (a_valueType)
    {
    case rMinEndcapPresampler:
      //return 1231.74 * mm;
      return  m_parameters->GetValue("LArEMECPreMinRadius");
      break;
    case rMaxEndcapPresampler:
      //return 1701.98 * mm;
      return  m_parameters->GetValue("LArEMECPreMaxRadius");
      break;
      // At nominal (zShift=0) endcap position absolute z-coordinates: 
      // of the faces of the EndcapPresampler
    case zEndcapPresamplerFrontFace:
      //return 3622. * mm;
      return (m_parameters->GetValue("LArEMECPreNomPos")
	      - GetValue(EndcapPresamplerHalfThickness));
      break;
    case zEndcapPresamplerBackFace:
      //return 3626. * mm;
      return (m_parameters->GetValue("LArEMECPreNomPos")
	      + GetValue(EndcapPresamplerHalfThickness)); 
      break;
    case EndcapPresamplerHalfThickness:
      //return ( GetValue(zEndcapPresamplerBackFace) - GetValue(zEndcapPresamplerFrontFace) ) / 2.;
      return  (m_parameters->GetValue("LArEMECPreThickness")/2.);
      break;
    case EndcapPresamplerZpositionInMother:
      // between cold wall center and presampler center which is at
      // 3624 Gaudi::Units::mm nominal (zShift=0) absolute position
      return 30.5 * Gaudi::Units::mm;
      break;
    default:
      std::cerr << "EndcapPresamplerGeometryHelper::GetValue -- type '"
		<< a_valueType
		<< "' not recognized; using zero" << std::endl;
      return 0.;
    }
}
