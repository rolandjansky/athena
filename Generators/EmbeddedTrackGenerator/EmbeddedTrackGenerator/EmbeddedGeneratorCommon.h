/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMBEDDEDGENERATORCOMMON_H
#define EMBEDDEDGENERATORCOMMON_H

#include "GeneratorModules/GenModule.h"
#include "PileUpTools/PileUpStream.h"

class IEvtSelector;


/// Common code for generators that need reconstructed event data inputs.
/// 
/// @author Andrei Gaponenko <andrei.gaponenko@cern.ch>, 2006-2008
class EmbeddedGeneratorCommon : public GenModule {
public:

  EmbeddedGeneratorCommon(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EmbeddedGeneratorCommon() {}

  /// This method is called by GenModule.  
  /// Calls embeddedGenInitialize().
  virtual StatusCode genInitialize();

  /// Initialization of the child class.
  virtual StatusCode embeddedGenInitialize() = 0;

  /// This method is called by GenModule.  
  /// It preloads the input data event and calls 
  /// fillEmbeddedEvent().
  virtual StatusCode fillEvt(HepMC::GenEvent* evt); 
  
  /// Actual generation happens here.
  virtual StatusCode embeddedFillEvt(HepMC::GenEvent *evt) = 0;
  
protected:

  PileUpStream m_pileUpStream;
  ServiceHandle<IEvtSelector> m_pileUpSelector;

};

#endif /* EMBEDDEDGENERATORCOMMON_H*/
