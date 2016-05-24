/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * SCT_TimeWalkGenerator.h
 * Header file for class SCT_TimeWalkGenerator
 * Class to compute the time walk for SCT base on the 
 * SCT_TimeWalkGenerator
 * (c) ATLAS Detector software
 * Version 1.0 24/04/2001 David Calvet
 * Rev. 23/08/2007 Kondo Gnanvo
*/

#ifndef SCT_DIGITIZATION_SCT_TIMEWALKGENERATOR_H
#define SCT_DIGITIZATION_SCT_TIMEWALKGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_Digitization/ISCT_TimeWalkGenerator.h"

class ComTime;

namespace CLHEP {
  class HepRandomEngine;
}


class SCT_TimeWalkGenerator : public AthAlgTool, virtual public ISCT_TimeWalkGenerator {

///////////////////////////////////////////////////////////////////
// Public methods:
///////////////////////////////////////////////////////////////////
 public:
 
  /**  constructor */
  SCT_TimeWalkGenerator(const std::string& type, const std::string& name, const IInterface* parent ) ;
  /** Destructor */
  virtual ~SCT_TimeWalkGenerator();
  /** AlgTool initialize */
  virtual StatusCode initialize();
  /** AlgTool finalize */
  virtual StatusCode finalize();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // compute the relative bunch for a given threshold and charge:
  int relativeBunch(const double threshold, const double intimethreshold, const SiTotalCharge &totalCharge) const;
  void setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) {  m_rndmEngine = rndmEngine; };

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////

 private:

  double m_timeZero;
  double m_timeJitter;
  double m_timePerBCO;                     //!< time per BCO - should be 25ns

  double m_comTime;                        //!< use cosmics time for timing 

  bool      m_useComTimeFlag;              //!< Flag to decide the use of cosmics time for timing
  ComTime*  m_ComTime ;                    //!< Tool to retrieve commissionning timing info from SG

  CLHEP::HepRandomEngine *m_rndmEngine;    //!< random number generation engine

};

#endif //SCT_DIGITIZATION_SCT_TIMEWALKGENERATOR_H
