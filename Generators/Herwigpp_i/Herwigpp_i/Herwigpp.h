// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_HERWIGPP_H
#define GENERATORMODULES_HERWIGPP_H

#include "GeneratorModules/GenModule.h"

#include "ThePEG/Repository/EventGenerator.h"
#include "ThePEG/EventRecord/Event.h"


/// Interface for the Herwig++ generator
/// @author Andy Buckley
class Herwigpp : public GenModule {
public:

  /// Constructor.
  Herwigpp(const std::string& name, ISvcLocator* pSvcLocator);

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

  // Commands to set up H++ from jobOptions.
  typedef std::vector<std::string> CommandVector;
  CommandVector m_herwigCommandVector;

  /// Run name, used in the H++/ThePEG config system.
  std::string m_runname;

  /// File to dump an equivalent H++ .in file to, if specified.
  std::string m_infiledump;

  /// PDF names, stored for AMI capture at finalize.
  std::string m_pdfname_me, m_pdfname_mpi;

  //@}

  double m_xsscale;

  /// ThePEG generator object.
  ThePEG::EGPtr m_hw;

  /// ThePEG event object.
  ThePEG::EventPtr m_event;


  //Switch that determines whether the readAlpgen function will be executed
  bool m_doAlpgen;

  //Two settings for Alpgen+Herwig++ that can be set by the user
  bool m_useTwoLoopAlphas;
  bool m_useShowerImprovement;
  
  //Function that reads Alpgen inputs
  StatusCode readAlpgen();

  //The following functions are helper functions for readAlpgen()
  double alpgen_sqr(double x);
  int alpgen_nInt(double x);
  int alpgen_ndnsToLHAPDF(int ndns);
  double alpgen_parstrToparval(std::string varName, std::vector<std::string> * parstrPtr, std::vector<double> * parvalPtr);

  StatusCode alpgen_doIndividualHardProcessAssignments(int ihrd, double * nup, std::vector<double> * idup, std::vector<double> * istup, std::vector<double> * mothup1, 
					  std::vector<double> * mothup2, std::vector<double> * icolup1, std::vector<double> * icolup2, 
					  std::vector<std::vector<double> > * pup, std::vector<double> masses, int itopprc);
  bool alpgen_getAlpha_s(int ndns, int& nloop, double& alpha_s);
  StatusCode alpgen_writeHWPPinFile(std::string prefix, int ihrd, int unwev, int lhapdf, /*int idbmup0,int idbmup1,*/ int idwtup, double aqcdup, int nloop, /*std::vector<double> * massesPtr,*/
		     std::vector<double> * parvalPtr, std::vector<std::string> * parstrPtr, double minETclus, double minRclus, double maxETAclus, double exclusive);
  std::string alpgen_trim(std::string theString);

};

#endif
