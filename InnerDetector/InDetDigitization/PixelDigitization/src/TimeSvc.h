/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////
// TimeSvc.h
//   Header file for class TimeSvc
// Class to compute the time walk for Pixel
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// -RndmSvc		Random Number Service used in Pixel digitization
// -RndmEngine		Random engine name
// -TimePerBCO		Time per BCO - should be 25ns
// -TimeBCN		Number of BCID
// -TimeJitter		Time jitter
// -TimeZero		Time zero...?
//
//////////////////////////////////////////////////////////////////

/** @class TimeSvc
 * @brief Generate the timing for pixel events
 * @author David Calvet
 * @author Francesco De Lorenzi (2007 - new timing parametrization and algorithm)
 * @author Fredrik Tegenfeldt (2006-2007 - revision and documentation)
 *
 * \b Description
 *
 * This class generates the timing for the charged diode collection.
 * It is calculated in TimeSvc::relativeBunch() and returned as a
 * relative bunch number.
 *
 * This is calculated as follows for a given charge:
 * -# Evaluate \e overdrive = \f$Q_{intime} - Q_{threshold}\f$
 * -# Evaluate \e curvature = \f$7.6\cdot 10^7\cdot overdrive - 2.64\cdot 10^{10}\f$
 * -# Evaluate \e divergence = \f$-1.6\cdot overdrive + 942.0\f$
 * -# Evaluate \e timewalk = \f$curvature/(Q - divergence)^{2.5}\f$
 * -# Draw the 'observed' timewalk from a Gauss with <em>mean = timewalk</em> and <em>sigma = timewalk/5</em>
 * -# Time jitter is drawn from a flat distribution of width TimeJitter and centered around the observed timewalk
 * -# The final time is calculated as <em>TimeZero + observed timewalk + jitter + G4 time - ComTime</em>
 * -# Bunch cross number is then calculated as <em>time/TimePerBCO</em>
 *
 * The figure below illustrates the timing of a charge deposition.
 * @image html timingfig.png
 *
 * <b>Used jobOptions from PixelDigitConfig</b>
 *
 * - DiscrThresh
 * - IntimeThresh
 * - TimeJitter
 * - TimeZero
 * - TimePerBCO
 * - TimeBCN
 * - ComTime
 *
 * The below are obsolete. They are retained for comparision between the old and new parametrizations.
 *
 * - TWMode (selects which parametrization to use)
 * - Overcharges
 * - TimeWalks
 * - TimeWalksVar
 */
#ifndef PIXELDIGITIZATION_TIMESVC_H
#define PIXELDIGITIZATION_TIMESVC_H

// STL classes
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"

class PixelCellDiscriminator;
class SiTotalCharge;

class IAtRndmGenSvc;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ITimeSvc("TimeSvc",1,0); 

class TimeSvc : public AthService, virtual public IInterface {

 public:

  //Constructor:
  TimeSvc( const std::string& name, ISvcLocator* svc);

  /** Destructor */
  ~TimeSvc();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );
  /** compute the relative bunch using the NEW parametrisation
   * @param threshold the given threshold of the pixel
   * @param intimethreshold the given intime-threshold of the pixel
   * @param totalCharge its total charge
   * @return relative bunch number, integer >= 0
   */  
  int relativeBunch(const double threshold,
                    const double intimethreshold,
                    const SiTotalCharge &totalCharge, bool CTW) const;

  /** sets the ComTime */
  void setComTime(const double cT ) { m_comTime = cT;  }
  double getTimePerBCO() {return m_timePerBCO;}
  double getTimeBCN() {return m_timeBCN;}
 private:

  /** Empty constructor */
  TimeSvc();

  double m_comTime;     /**< cosmics timing ofs */
  double m_timePerBCO;
  double m_timeBCN;
  double m_timeZero;
  double m_timeJitter; 
  double getG4Time(const SiTotalCharge &totalCharge) const;
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string m_rndmEngineName;
  CLHEP::HepRandomEngine* m_rndmEngine;

};
inline const InterfaceID& TimeSvc::interfaceID(){
	return IID_ITimeSvc;
}
#endif // PIXELDIGITIZATION_TIMESVC_H
