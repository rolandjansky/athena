/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file EndcapPresamplerConstruction.h
 *
 * @brief Declaration of EndcapPresamplerConstruction class
 *
 * $Id: EndcapPresamplerConstruction.h,v 1.5 2007-07-08 18:51:26 tsulaia Exp $
 */

#ifndef LARGEOENDCAP_ENDCAPPRESAMPLERCONSTRUCTION_H
#define LARGEOENDCAP_ENDCAPPRESAMPLERCONSTRUCTION_H

#include "GeoModelKernel/GeoFullPhysVol.h"
#include "CLHEP/GenericFunctions/FunctionNoop.hh"

// Forward declarations

/** @class EndcapPresamplerConstruction
    @brief GeoModel description of the LAr Endcap Presampler geometry
 */
class EndcapPresamplerConstruction
{	
 public:
  
  // Constructor;
  EndcapPresamplerConstruction( bool imb = false );
  
  // Destructor:
  virtual ~EndcapPresamplerConstruction();
  
  // Get the envelope containing this detector.
  GeoFullPhysVol* Envelope();
  
 private: 
  
  // It is illegal to copy a EndcapPresamplerConstruction:
  EndcapPresamplerConstruction (const EndcapPresamplerConstruction &);
  
  // It is illegal to assign a EndcapPresamplerConstruction:
  EndcapPresamplerConstruction & operator= (const EndcapPresamplerConstruction &);
  
  // volumes that are private member variables:
  GeoFullPhysVol*  m_psPhysical;

  // Flag for module only presampler ( TB )
  bool m_isModule;
    
};

#endif // LARGEOENDCAP_ENDCAPPRESAMPLERCONSTRUCTION_H


