/* -*- C++ -*- */

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_IBEAMLUMINOSITY_H
#define PILEUPTOOLS_IBEAMLUMINOSITY_H
/** @file IBeamLuminosity.h
 * @brief provides the relative beam luminosity as a function of the
 * bunch xing.
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Ayana Arce - ATLAS Collaboration
 */

#include "GaudiKernel/IService.h"
class IBeamLuminosity : virtual public IService {
public:
  ///a scale-down factor (between 0 and 1) for the beam luminosity at a given
  ///run and lumiblock number
  virtual float scaleFactor(unsigned int run, unsigned int lumi, bool & updated) = 0;

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IBeamLuminosity, 1, 0 );
};
#endif // PILEUPTOOLS_IBEAMLUMINOSITY_H
