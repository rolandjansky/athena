/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelDigitization.h
//   Header file for class PixelDigitization
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Top algorithm class for Pixel digitization
///////////////////////////////////////////////////////////////////
// Version 2.1 10/07/2001 David Calvet
// Revisited version. 04-04-03 Davide Costanzo
//   - Moved to the the SiDigitization
//   - Added PixelID
//   - Added createRDO class
//   - creates RDOs instead of digits
// Revisited version. 06-12-06 Marian Zdrazil
//   - Modified PixelDigitization to accomodate needs of the Pixel
//     EndCap C Cosmic Test. We need to check the status of each
//     module, whether it was in a readout and if it has any bad
//     channels. The code should be further developed to access the
//     conditions database (COOL) just like SCT does.
// Revisited version 04/07/2006 Fredrik Tegenfeldt
//   - Made the TOT parameters available via jobOpts
// Restructured/rewritten version 29/08/2013 Matthew Klein
//   - Eliminated dependence on SiDigitization
//   - Added separate class for each technology to account for addition
//     of IBL
//   - Functions to choose technology and detect presence of IBL
//   - Other structural/minor changes
///////////////////////////////////////////////////////////////////

/** @class PixelDigitization
 * @brief The main pixel digitization class
 * @author David Calvet
 * @author Davide Costanzo
 * @author Fredrik Tegenfeldt
 * @author Matthe Klein
 *
 * \b Description
 *
 * This is the main class for the pixel digitization process. That is, for each event it
 * reads the Geant 4 produced hits and simulates the detector respons.
 *
 * The current design uses so called 'charge processors' for each simulated
 * process that affects the digitization.
 * Such a processor inherits from SiDigitization::SiChargedDiodesProcessor.
 * The number of processors is not explicitly limited apart from performance issues.
 *
 *
 * <b>Job options</b>
 *
 * <table border="1">
 * <tr><td><b>Name</b></td>         <td><b>Description</b></td></tr>
 * <tr><td>DigitizationTool</td>    <td>Pixel Digtization Tool Name</td></tr>
 * </table>
 */
	
#ifndef PIXELDIGITIZATION_PIXELDIGITIZATION_H
#define PIXELDIGITIZATION_PIXELDIGITIZATION_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
// Data members
#include "GaudiKernel/ToolHandle.h"

#include "PileUpTools/PileUpToolBase.h"
class PixelDigitizationTool;
class PixelDigitization : public AthAlgorithm {

public:

  /** Constructor with parameters */
  PixelDigitization(const std::string &name,ISvcLocator *pSvcLocator);

  /** Destructor */
  ~PixelDigitization();

  /** Algorithm::initialize() */
  virtual StatusCode initialize();
   
  /** Algorithm::execute() */
  virtual StatusCode execute();
   
  /** Algorithm::finalize() */
  virtual StatusCode finalize();

private:
  ToolHandle<PixelDigitizationTool> m_pixelDigitizationTool{this, "DigitizationTool", "PixelDigitizationTool", "PixelDigitizationTool name"};
};
	
#endif // PIXELDIGITIZATION_PIXELDIGITIZATION_H
