/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISiChargedDiodesProcessorTool.h
 * Header file for abstract base class ISiChargedDiodesProcessorTool
 * (c) ATLAS Detector software
 * Interface for the SiChargedDiode processor classes
 * 23/08/2007 Kondo.Gnanvo@cern.ch, and others   
*/

#ifndef SIDIGITIZATION_ISICHARGEDDIODESPROCESSORTOOL_H
#define SIDIGITIZATION_ISICHARGEDDIODESPROCESSORTOOL_H

#include "GaudiKernel/IAlgTool.h"

class SiChargedDiodeCollection;
class NPtGainSummaryData;
namespace CLHEP {
  class HepRandomEngine;
}

static const InterfaceID IID_ISiChargedDiodesProcessorTool("ISiChargedDiodesProcessorTool",1,0);

class ISiChargedDiodesProcessorTool : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  //Retrieve interface ID
  static const InterfaceID & interfaceID() {return IID_ISiChargedDiodesProcessorTool; }

  // Destructor:
  virtual ~ISiChargedDiodesProcessorTool() {}

  ///////////////////////////////////////////////////////////////////
  // Pure virtual methods:
  ///////////////////////////////////////////////////////////////////

  // process the collection of charged diodes
  virtual void process(SiChargedDiodeCollection &collection, CLHEP::HepRandomEngine * rndmEngine) const =0;
};

#endif // SIDIGITIZATION_ISICHARGEDDIODESPROCESSORTOOL_H
