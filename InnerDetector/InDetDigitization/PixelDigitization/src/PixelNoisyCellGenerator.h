/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelNoisyCellGenerator.h
//   Header file for class PixelNoisyCellGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to generate the noisy cells for Pixel
///////////////////////////////////////////////////////////////////
// 
// Configurable Parameters:
// -NoiseShape		Vector containing noise ToT shape
// -RndmSvc		Random Number Service used in SCT & Pixel digitization
// -RndmEngine		Random engine name	
// -MergeCharge
// -SpmNoiseOccu	Special Pixels map gen: probability for a noisy pixel in SPM
// -RndNoiseProb	Random noisy pixels, amplitude from calib. - NOT special pixels! 
//
///////////////////////////////////////////////////////////////////

/** @class PixelNoisyCellGenerator
 * @brief Generates random noise
 * @author David Calvet
 * @author Davide Costanzo
 * @author Fredrik Tegenfeldt
 *
 * \b Description
 *
 * This class generates random noise for random pixels. Which pixels that will receive noise and how much is
 * determined in the following way:
 * -# Number of pixels is drawn from a Poisson with <em>mean = </em>\f$N_{pixels}\cdot p_{noise}\cdot N_{BC}\f$
 *     - \f$N_{pixels}\f$ is the total number of pixels
 *     - \f$p_{noise}\f$ is the noise probability
 *     - \f$N_{BC}\f$ is the number of bunches; noise will occur at a random time with a flat distribution
 * -# Pixels are then randomly chosen (flat dist)
 * -# Noise charge is drawn from a Gaussian with <em>mean = threshold + 3*thresholdSigma</em> and <em>sigma = thresholdSigma</em>
 * -# If a pixel is already hit:
 *     - if \e mergeCharge is \e true then merge the noise charge with the existing charge
 *     - else, ignore the pixel
 *
 * Note that if looking at the noise in a simulation, the number of noisy pixels per event
 * will not \e quite follow the poisson ditribution with the above mean. The reason for this
 * is of course the treatment of already hit pixels. This will certainly be more eveident in events
 * with many hits.
 *
 */

#ifndef PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H
#define PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H

#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"

class PixelID;
class TimeSvc;
class CalibSvc;

class IAtRndmGenSvc;
namespace CLHEP {
  class HepRandomEngine;
}

class PixelNoisyCellGenerator : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

public:

  //Constructor:
  PixelNoisyCellGenerator( const std::string& type, const std::string& name,const IInterface* parent);

  /** Destructor */
  virtual ~PixelNoisyCellGenerator();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Process the collection of pre digits */
  virtual void process(SiChargedDiodeCollection &collection) const;

  virtual void setNoisyPixel(std::map<unsigned int,std::vector<unsigned int> > *noisyPixelMap) {m_noisyPixel=noisyPixelMap;}

protected:
  std::map<unsigned int,std::vector<unsigned int> > *m_noisyPixel;

private:
  
  /** Empty constructor made private */
  PixelNoisyCellGenerator();

private:
  ServiceHandle<TimeSvc> m_TimeSvc;
  ServiceHandle<CalibSvc> m_CalibSvc;
  bool                     m_mergeCharge;      /**< to merge or not to merge */
  std::vector<double>       m_noiseShape;      /**< ToT shape of noise*/
  void addRandomNoise(SiChargedDiodeCollection &collection, double occupancy) const;
  void addNoisyPixels(SiChargedDiodeCollection &collection, double occupancy) const;
  void addCell(SiChargedDiodeCollection &collection, const InDetDD::PixelModuleDesign *design,
	       int circuit, int column, int row) const;

  double getNoiseToT() const;
  const PixelID            *m_pixelID;     /**< the ID helper */
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string m_rndmEngineName;
  CLHEP::HepRandomEngine* m_rndmEngine;
  double m_spmNoiseOccu;
  double m_rndNoiseProb;
};

#endif // PIXELDIGITIZATION_PIXELNOISYCELLGENERATOR_H
