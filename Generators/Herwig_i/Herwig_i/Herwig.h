/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULESHERWIG_H
#define GENERATORMODULESHERWIG_H

#include "GeneratorModules/GenModule.h"
#include "AcerMC_i/AcerMC_acset.h"
#include "Tauola_i/Atlas_HEPEVT.h"

/**
@class Herwig
@brief Main interface class to Herwig


Allows the user to generate Herwig events and store the result in the Transient Store.

genInitialize() is used to read parameters

callGenerator() makes the event

genFinalize() writes log files etc

fillEvt(GeneratorEvent* evt) passes the event to HepMC

The output will be stored in the transient event store so it can be passed to the simulation.


@author  

 Georgios Stavropoulos January 2003 : Migrate to Herwig 6.5

 Ian Hinchliffe Sept 2000 Modeled after the CDF code by Marge Shapiro, Elena Bucciol

 Borut Paul Kersevan (February 2003) added access to lesatlas common (not part of 
                      Herwig but needed for external process)

 AcerMC 1.x interface added by Borut Paul Kersevan (February 2003)

 TAUOLA/PHOTOS interface added by Borut Paul Kersevan (October 2003)

 Jon Butterworth Merged with Jimmy.h (Feb 2007)

 Judith Katzy added doxygen docu (Jan 2008)

 Andy Buckley: conversion to AthAlgorithm (December 2009)

*/

class Herwig : public GenModule {
public:

  /// @name Construction / destruction
  //@{
  Herwig(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~Herwig();
  //@}

  /// @name Generation loop
  //@{
  virtual StatusCode genInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(HepMC::GenEvent* evt);
  //@}

  void Localpar2Her();
  void Her2Localpar();

  void updateStatusCode( HepMC::GenEvent *evt );

  /// Accessor for external processes
  AcerMC_acset& acermc_acset();

  /// Local name for a list of config strings
  typedef std::vector<std::string> CommandVector;


private:
     
  /// Turn off parton shower
  bool m_noshower_Parm;
  
  /// Turn off hadronization
  bool m_nohadroniz_Parm;

  /// Param for the SUSY file name 
  std::string m_read_Filesusy;

  /// Top decays switch
  int m_itopd;

  /// Version 6510 variable to prevent infinite looping
  int m_ndetry;
 
  /// Commands to set up Herwig from job options
  CommandVector m_herwigCommandVector;

  // multiply x-section by a num. factor 
  // add both in the MetaData print-out 
  double m_Default_xsscale, m_xsscale; 

  // print out manually set x-section from the MetaData 
  double m_Default_SetCrossSection, m_SetCrossSection;

  /// RNG seeds
  std::vector<long int> m_seeds;

  /// Update status code of hadrons
  bool m_updateHadronStatCode;

protected:

  /// @name External process
  //@{
  int m_ExternalProcess;
  AcerMC_acset m_acermc_acset;
  //@}

  /// I/O to HEPEVT
  void store_Atlas_HEPEVT();

  static Atlas_HEPEVT* atlas_HEPEVT;

};

#endif
