/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RandBinomialFixedP_h
#define RandBinomialFixedP_h 1

#include <vector>

#include "CLHEP/Random/RandBinomial.h"

namespace CLHEP {

/**
 * @author
 * @ingroup random
 */
class RandBinomialFixedP : public RandBinomial {

public:

  inline RandBinomialFixedP ( HepRandomEngine& anEngine, long n=1,
                                                     double p=0.5, long Nmaxlookup=10 );
  inline RandBinomialFixedP ( HepRandomEngine* anEngine, long n=1,
                                                     double p=0.5, long Nmaxlookup=10 );
  // These constructors should be used to instantiate a RandBinomialFixedP
  // distribution object defining a local engine for it.
  // The static generator will be skipped using the non-static methods
  // defined below. Only the non static method run with the fixed p value optimization
  // If the engine is passed by pointer the corresponding engine object
  // will be deleted by the RandBinomialFixedP destructor.
  // If the engine is passed by reference the corresponding engine object
  // will not be deleted by the RandBinomialFixedP destructor.

  virtual ~RandBinomialFixedP();
  // Destructor

  //  Methods using the localEngine to shoot random values, by-passing
  //  the static generator.

  void initLookupTable(long Nmaxlookup, double p);

  inline double fire();

  double fire( long n);

  double fire( HepRandomEngine* anEngine, long n);
  
  void fireArray ( const int size, double* vect);
  void fireArray ( const int size, double* vect,
                   long n);
  inline double operator()();
  inline double operator()( long n );

  std::string name() const;

  static std::string distributionName() {return "RandBinomialFixedP";}  
  // Provides the name of this distribution class
  
  static double shoot() = delete;

  static double shoot( long n, double p ) = delete;

  static void shootArray ( const int size, double* vect, long n=1, double p=0.5 ) = delete;

  static inline double shoot( HepRandomEngine* anEngine ) = delete;

  static double shoot( HepRandomEngine* anEngine, long n, double p ) = delete;

  static void shootArray ( HepRandomEngine* anEngine, const int size, double* vect, long n=1, double p=0.5 ) = delete;
  
protected:

  std::vector< std::vector< double > > LookupTable;
 
};

}  // namespace CLHEP

#ifdef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
using namespace CLHEP;
#endif

#include "RandBinomialFixedP.icc"

#endif
