/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelChargeSmearer.h
//   Header file for class PixelChargeSmearer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to simulate the diode-level thermal noise for Pixel and SCT
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// -RndmSvc		Random Number Service used in Pixel digitization
// -RndmEngine		Random engine name
// -ThermalNoise	Thermal noise amplitude
//
///////////////////////////////////////////////////////////////////

/** @class PixelChargeSmearer
 * @brief smears charges in the given collection
 * @author David Calvet
 * @author Fredrik Tegenfeldt
 *
 * \b Description
 *
 * This class smears a charges using a gaussian with a mean=0 and a sigma given by the user.
 * The charge is flaged as noise and added to the total charge of the pixel.
 *
 */

#ifndef PIXELDIGITIZATION_PIXELCHARGESMEARER_H
#define PIXELDIGITIZATION_PIXELCHARGESMEARER_H

#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class IAtRndmGenSvc;

namespace CLHEP {
  class HepRandomEngine;
}

class PixelChargeSmearer : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

public:

  // Constructor:
  PixelChargeSmearer( const std::string& type, const std::string& name,const IInterface* parent);

  /** Destructor */
  virtual ~PixelChargeSmearer();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;


  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** process the collection of charged diodes */
  virtual void process(SiChargedDiodeCollection &collection) const;
   
private:
  /** empty constructor made private */
  PixelChargeSmearer();

  double                   m_amplitude;  /**< noise amplitude */
 ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
 std::string	m_rndmEngineName;
 CLHEP::HepRandomEngine* m_rndmEngine;
    };

#endif // SITHERMALDIODENOISEGENERATOR_H
