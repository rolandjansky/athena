/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDiodeCrossTalkGenerator.h
//   Header file for class PixelDiodeCrossTalkGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to generate the diode-level cross-talk for Pixel
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters: 
// -DiodeCrossTalk	Diode cross talk factor
//
///////////////////////////////////////////////////////////////////



/** @class PixelDiodeCrossTalkGenerator
 * @brief Simulates the discriminator, including the timing
 * @author David Calvet (2000)
 * @author Fredrik Tegenfeldt (2005-2007)
 *
 * \b Description
 *
 * This class generates cross talk from a given charge collection.
 * It is assumed that all charges originate from tracks. Therefore it must
 * be called directly \e after the surface charge generator.
 *
 * For a given charged pixel, cross talk charges are generated in the neighboring
 * pixels (not diagonal). The algorithm is as follows:
 * -# make a copy of the original charge list (avoid creating cross talk on cross talk on ...)
 * -# loop over all charged pixels and check if above 3*threshold (the const 3 is hardcoded)
 * -# if yes, generate cross talk in neighboring pixels
 *
*/

#ifndef PIXELDIGITIZATION_PIXELDIODECROSSTALKGENERATOR_H
#define PIXELDIGITIZATION_PIXELDIODECROSSTALKGENERATOR_H

// Base class
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"


class PixelDiodeCrossTalkGenerator : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

public:

  //Constructor:
  PixelDiodeCrossTalkGenerator( const std::string& type, const std::string& name,const IInterface* parent);

  /** Destructor */
  virtual ~PixelDiodeCrossTalkGenerator();

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
  PixelDiodeCrossTalkGenerator();

private:
 double m_diodeCrossTalk; 
};

#endif // PIXELDIGITIZATION_PIXELDIODECROSSTALKGENERATOR_H
