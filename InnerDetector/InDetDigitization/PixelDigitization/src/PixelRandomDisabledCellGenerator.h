/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelRandomDisabledCellGenerator.h 
//   Header file for class PixelRandomDisabledCellGenerator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
//////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// -RndmSvc		Random Number Service used in Pixel digitization
// -RndmEngine		Random engine name
// -TotalBadChannels	m_disableProbability
//
//////////////////////////////////////////////////////////////////

/** @class PixelRandomDisabledCellGenerator
 * @brief This can be used for disabling, disconnecting, and flagging bad_tot
 * @author
 *
 * \b Description
 *
 *
 */
#ifndef PIXELDIGITIZATION_PIXELRANDOMDISABLEDCELLGENERATOR_H
#define PIXELDIGITIZATION_PIXELRANDOMDISABLEDCELLGENERATOR_H

#include "SiDigitization/ISiChargedDiodesProcessorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "SiDigitization/SiChargedDiodeCollection.h"


class IAtRndmGenSvc;

namespace CLHEP {
  class HepRandomEngine;
}

class PixelRandomDisabledCellGenerator : public AthAlgTool, virtual public ISiChargedDiodesProcessorTool {

public:

  /**  constructor */
  PixelRandomDisabledCellGenerator( const std::string& type, const std::string& name,const IInterface* parent);
			 
  /** Destructor */
  virtual ~PixelRandomDisabledCellGenerator();

  /** AlgTool InterfaceID */
  static const InterfaceID& interfaceID() ;

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** process the collection of charged pixels */
  virtual void process(SiChargedDiodeCollection &collection) const;

private:   

  void (*m_myfunc)(SiChargedDiode& chDiode, bool flag1, bool flag2);
  ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
  std::string	m_rndmEngineName;
  CLHEP::HepRandomEngine* m_rndmEngine;

  double m_disableProbability;   // probability that a cell is disabled
};

#endif
