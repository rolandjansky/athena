/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelCellDiscriminator.cxx 
//   Implementation file for class PixelCellDiscriminator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
/////////////////////////////////////////////////////////////////// 
//
// Configurable Parameters:
// -RndmSvc		Random Number Service used in Pixel digitization
// -RndmEngine		Random engine name
//
//////////////////////////////////////////////////////////////////


/** @class PixelCellDiscriminator
 * @brief simulates the discriminator, including the timing
 * @author Francesco De Lorenzi
 *
 * \b Description
 *
 * The flow of the algorithm is (for each charged pixel):
 * -# simulate the threshold using a gaussian
 * -# smear the obtained threshold with a gaussian with mean=0
 * -# calculate the intime threshold
 * -# discriminate : check if charge is larger than obtained threshold
 * -# if charge \e ABOVE threshold
 *    - if main charge from track: calculate timing (bunch) using the bunch calculator
 *    - else : set the bunch to a random number [0..TimeBCN-1] - hit is just noise
 *    - if timing is outside allowed range ([0,TimeBCN-1] then pixel is set to 'below threshold'
 * -# if charge \e BELOW threshold
 *    - flag pixel as 'below threshold'
 *
 */
#ifndef PIXELDIGITIZATION_PIXELCELLDISCRIMINATOR_H
#define PIXELDIGITIZATION_PIXELCELLDISCRIMINATOR_H

// Base class
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

namespace CLHEP {
  class HepRandomEngine;
}
class TimeSvc;
class IPixelCalibSvc;

class IAtRndmGenSvc;
class PixelCellDiscriminator : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

public:

  //Constructor:
  PixelCellDiscriminator( const std::string& type, const std::string& name,const IInterface* parent);
			 

  /** Destructor */
  virtual ~PixelCellDiscriminator();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;


  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** process the collection of charged pixels */
  virtual void process(SiChargedDiodeCollection &collection) const;

private:   

  /** empty constructor made private */
  PixelCellDiscriminator();
  
  ServiceHandle<TimeSvc> m_TimeSvc;
  ServiceHandle<IPixelCalibSvc> m_pixelCalibSvc;
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string m_rndmEngineName;
  CLHEP::HepRandomEngine* m_rndmEngine;
  int m_timingTune;
  bool m_disableTimeSim;
};

#endif
