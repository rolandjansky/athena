// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/*! \file Herwig7.h
 *  \brief Athena interface for the Herwig 7 generator
 *  \author Daniel Rauch (daniel.rauch@desy.de)
 */

#ifndef GENERATORMODULES_HERWIG7_H
#define GENERATORMODULES_HERWIG7_H

#include "GeneratorModules/GenModule.h"

#include "ThePEG/Repository/EventGenerator.h"
#include "ThePEG/EventRecord/Event.h"


class Herwig7 : public GenModule {
public:

  /// Constructor.
  Herwig7(const std::string& name, ISvcLocator* pSvcLocator);

  /// Initialize the generator.
  StatusCode genInitialize();

  /// Run the generator for one event.
  StatusCode callGenerator();

  /// Convert the generated event into the HepMC format.
  StatusCode fillEvt(HepMC::GenEvent* evt);

  /// Close down the generator.
  StatusCode genFinalize();

private:

  /// @name Properties
  //@{

  /// Typedef to hold Herwig repository commands
  typedef std::vector<std::string> CommandVector;

  /// Commands to set up Herwig from jobOptions.
  CommandVector m_herwigCommandVector;

  /// Run name, used in the Herwig/ThePEG config system.
  std::string m_runname;

  /// File to dump an equivalent Herwig .in file to, if specified.
  std::string m_infiledump;

  /// ME PDF name, stored for AMI capture at finalize.
  std::string m_pdfname_me;

  /// PS and MPI PDF name, stored for AMI capture at finalize.
  std::string m_pdfname_mpi;

  /// Scale integrated cross section by a factor for MetaData output
  double m_xsscale;

  //@}

  /// ThePEG generator object.
  ThePEG::EGPtr m_hw;

  /// ThePEG event object.
  ThePEG::EventPtr m_event;

};

#endif
