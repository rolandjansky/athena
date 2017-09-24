#ifndef GENERATOR_PHOTOSPP_H
#define GENERATOR_PHOTOSPP_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

class IAtRndmGenSvc;

extern "C" double phoranc_(int *idum);

class Photospp_i : public AthAlgorithm{
  
public:
  
  /// Standard Athena algorithm constructor
  Photospp_i(const std::string &name, ISvcLocator *pSvcLocator);
  
  /// Initialise the Photospp_i algorithm and required services
  StatusCode initialize();
  
  /// Run Photos on one event
  /// Will require a pre-existing HepMC event in Storegate
  StatusCode execute();
  
  /// Finalize the algorithm
  /// This probably doesn't do much
  StatusCode finalize();
  
  /// Set up the Photos class
  /// This may be called in the initialize method or directly before the 
  /// generation of the first event
  void setupPhotos();
  
  /// This external fortran function is the PHOTOS++ random number generator
  /// We make it a friend so it can access our AtRndmGenSvc
  friend double ::phoranc_(int *idum);
  
private:
  
  /// Rather than a static pointer to the AtRndmGenSvc, we use a static member function.
  /// This allows access to the extern fortran function above, and is anyway nicer since
  /// it follows the "construct on first use" idiom for static members (although in this case the 
  /// service is useless before an instance of Photospp_i has been initialised) and provides nicer 
  /// encapsulation than a global static pointer.
  static IAtRndmGenSvc* &atRndmGenSvc();
  
  /// This is the name of the random number stream.  Static function returning reference for the same reasons as above
  static std::string &photospp_stream();
  
  /// The GenEvent StoreGate key (default "GEN_EVENT")
  std::string m_genEventKey;
  
  /// Whether to use exponentiation mode (default = yes)
  bool m_exponentiation;
  
  /// Whether to create history entries (default = yes)
  bool m_createHistory;

  /// Whether to stop on critical error (default = no)
  bool m_stopCritical;
  
  /// Delay initialisation until just before first event execution (default = no)
  bool m_delayInitialisation;
  
  /// Whether to apply ME correction to Z decays (default = no, until validated)
  bool m_ZMECorrection;
 
  /// Whether to apply ME correction to W decays (default = no, until validated)
  bool m_WMECorrection;
  
  /// Whether to include photon splitting
  bool m_photonSplitting;
  
  /// 
  double m_infraRedCutOff;
  ///
  double m_maxWtInterference;
  
  /// Value of alpha_QED
  double m_alphaQED;
  
};
#endif
