/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_StripDiscriminator.cxx 
 * Implementation file for class SCT_StripDiscriminator 
 * (c) ATLAS Detector software 
 * Version 1.0 01/08/2007   Kondo.Gnanvo@cern.ch 
 * New Version of CellDiscriminator: 
 * Implemented different timing for hit from track (timewalk+jitter) 
 * and for noise hit (random bunch crossing assignment) 
 */

#ifndef SCT_DIGITIZATION_SCT_STRIPDISCRIMINATOR_H
#define SCT_DIGITIZATION_SCT_STRIPDISCRIMINATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_StripDiscriminator.h"

// Gaudi
#include "GaudiKernel/ToolHandle.h"

class SiChargedDiodeCollection;
class ISCT_TimeWalkGenerator;

namespace CLHEP {
  class HepRandomEngine;
}

class SCT_StripDiscriminator: public AthAlgTool, virtual public ISCT_StripDiscriminator {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  /**  constructor */
  SCT_StripDiscriminator(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** Destructor */
  virtual ~SCT_StripDiscriminator();
  /** AlgTool initialize */
  virtual StatusCode initialize();
  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** process the collection of pre digits */
  virtual void process(SiChargedDiodeCollection &collection) const;
  void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) ;


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

private:

  int m_discrThresh;                                      //!< discriminator threshold value
  int m_discrThreshSigma;                                 //!< discriminator threshold sigma value
  int m_intimeThresh;                                     //!< intime threshold
  int m_timeBCN;                                          //!< time per BCO - should be 25ns

  CLHEP::HepRandomEngine *m_rndmEngine;                   //!< random number generation engine 
  ToolHandle<ISCT_TimeWalkGenerator>  m_sct_TimeWalkGen;   //1< Handling the Time Walk tool

};

#endif
