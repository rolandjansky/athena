/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpecialPixelGenerator.h
//   Header file for class SpecialPixelGenerator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// -SpmNoiseProb		Special Pixels map gen: probability for a noisy pixel
// -SpmNoBumpProb		Special Pixels map gen: probability for pixel with out bump
// -SpmDisableProb		Special Pixels map gen: probability for a disabled pixel
// -SpmBadTOTProb		Special Pixels map gen: probability for corrupted TOT
// -SpecialPixelMapSvc
// -UsePixCondSum		Use PixelConditionsSummarySvc
// -PixelConditionsSummarySvc
// -RndmSvc			Random Number Service used in SCT & Pixel digitization
// -RndmEngine			Random engine name
//
///////////////////////////////////////////////////////////////////

/** @class SpecialPixelGenerator
 * @brief Generates special pixel maps
 * @author Fredrik Tegenfeldt
 *
 * \b Description
 *
 * This class has two main functionalities:
 *
 * -# generate special pixel maps either from simulation or from conditions DB
 * -# simulate/retrieve special pixel maps
 *
 * Special pixel map is simulated by filling the map accoring to some given probabilities.
 * They are:
 *
 * - Noisy pixels
 * - Pixels without bumps
 * - Disabled pixels
 * - Corrupt TOT
 *
 * For a given probability the number of special pixels is drawn from a Poisson with
 * mean = prob*N(pixels). The chosen pixel is not checked if it's already marked as special.
 * Hence, there may be overlap between the various special pixel groups.
 *
 * <b>Used jobOptions from PixelDigitConfig</b>
 *
 * - SpmNoiseProb
 * - SpmNoBumpProb
 * - SpmDisableProb
 * - SpmBadTOTProb
 */
#ifndef PIXELDIGITIZATION_SPECIALPIXELGENERATOR_H
#define PIXELDIGITIZATION_SPECIALPIXELGENERATOR_H

// Base class
#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "PixelConditionsData/SpecialPixelMap.h"
#include "CalibSvc.h"

class Identifier;
class PixelID;

namespace CLHEP {
  class HepRandomEngine;
}

class SpecialPixelGenerator : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //Constructor:
  SpecialPixelGenerator( const std::string& type, const std::string& name, const IInterface* parent ) ;

  // Destructor:
  virtual ~SpecialPixelGenerator();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();
  
  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  virtual void process(SiChargedDiodeCollection &collection) const;
  void maskDead(SiChargedDiodeCollection &collection) const;
  void updatePixelMap();
  void generatePixelMap() const;
  void processDiodes(SiChargedDiodeCollection &collection) const;

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  inline void setUpdate() { m_doUpdate = true; }
  inline void clrUpdate() { m_doUpdate = false; }
  void setnPixTot(unsigned int npixtot ) { m_npixTot = npixtot;  }
  void setnModTot(unsigned int nmodtot ) { m_nmodTot = nmodtot;  }
  void setRandomEngine(CLHEP::HepRandomEngine* rndmEngine) { m_rndmEngine = rndmEngine; }

  ///////////////////////////////////////////////////////////////////
  // const methods:
  ///////////////////////////////////////////////////////////////////

  const DetectorSpecialPixelMap *getDetectorMap() const;

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  SpecialPixelGenerator();
  int fillSpecialPixels( double prob, unsigned int status, bool merge ) const;
  void getID( const Identifier & id, unsigned int & pixID ) const;
  inline unsigned int setPixelStatus( unsigned int modID, unsigned int pixID, unsigned int status );
  inline unsigned int mergePixelStatus( unsigned int modID, unsigned int pixID, unsigned int status );
  inline unsigned int getPixelStatus( unsigned int modID, unsigned int pixID ) const;
  inline bool         pixelUseful( unsigned int modID, unsigned int pixID ) const;

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  mutable DetectorSpecialPixelMap   m_detectorMapGen; /** generated map, if applicable */
  const DetectorSpecialPixelMap*    m_detectorMap;    /** map from DB, if applicable */
  unsigned int                      m_npixTot;        /** number pixels in geometry */
  unsigned int                      m_nmodTot;        /** number of modules */
  mutable bool                      m_doUpdate;       /** flag; if true, update the randomly generated map; not used when using cond DB */
  const PixelID            *m_pixelID;     /**< the ID helper */
  bool                      m_usePixCondSum; /**< if true, use PixelConditionsSummarySvc to mask dead pixels */
  ServiceHandle< IInDetConditionsSvc >    m_pixelConditionsSummarySvc;
  
  double                    m_spmNoiseProb;     /**< probability for noisy pixel generation */
  double                    m_spmNoBumpProb;    /**< probability of missing bump */
  double                    m_spmDisableProb;   /**< probability of disabled cell */
  double                    m_spmBadTOTProb;    /**< probability for ToT corruption */
  ServiceHandle<ISpecialPixelMapSvc> m_specialPixelMapSvc;
  ServiceHandle<IAtRndmGenSvc>  m_rndmSvc;       /** Random number service */ 
  std::string               m_rndmEngineName;/** name of random engine, actual pointer in PixelDigitizationToolBaseAlg */ 
  ServiceHandle<CalibSvc> m_CalibSvc;
  CLHEP::HepRandomEngine* m_rndmEngine;

};

unsigned int SpecialPixelGenerator::setPixelStatus( unsigned int modID, unsigned int pixID, unsigned int status ) {
  unsigned int old = m_detectorMapGen[modID]->pixelStatus(pixID);
  m_detectorMapGen[modID]->setPixelStatus(pixID,status);
  return old;
}

unsigned int SpecialPixelGenerator::mergePixelStatus( unsigned int modID, unsigned int pixID, unsigned int status ) {
  unsigned int newstat = m_detectorMapGen[modID]->pixelStatus(pixID) | status;
  m_detectorMapGen[modID]->setPixelStatus(pixID,newstat);
  return newstat;
}

unsigned int SpecialPixelGenerator::getPixelStatus( unsigned int modID, unsigned int pixID ) const {
  const ModuleSpecialPixelMap *modmap=0;
  if(m_CalibSvc->usePixMapCDB()) modmap = m_detectorMap->module(modID);
  else modmap=m_detectorMapGen.module(modID);
  if ( modmap ) return modmap->pixelStatus(pixID);
  else return 0;
}

bool SpecialPixelGenerator::pixelUseful( unsigned int modID, unsigned int pixID ) const {
  const ModuleSpecialPixelMap *modmap=0;
  if(m_CalibSvc->usePixMapCDB()) modmap = m_detectorMap->module(modID);
  else modmap=m_detectorMapGen.module(modID);
  if ( modmap ) return modmap->pixelUseful(pixID);
  else return true;
}

#endif // PIXELDIGITIZATION_SPECIALPIXELGENERATOR_H
