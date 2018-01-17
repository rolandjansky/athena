/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// dSFMTEngine.h
// base source code copied from CLHEP 1.9.4.7 version of MTwistEngine, than
// adopted to use dSFMT
// author Michael Duehrssen

#ifndef dSFMTEngine_h
#define dSFMTEngine_h

#include <stdexcept>
#include "CLHEP/Random/defs.h"
#include "CLHEP/Random/RandomEngine.h"

#include <stdint.h> 
// forward declare the base data struct of the dSFMT method
// dSFMT internally uses dsfmt_t, which is
// typedef struct DSFMT_T dsfmt_t;
// unfortunately, typedefs can't be forward declared
struct DSFMT_T;

namespace CLHEP {

/**
 * @author
 * @ingroup random
 */
class dSFMTEngine : public HepRandomEngine {

public:
  dSFMTEngine();
  dSFMTEngine( long seed );
  dSFMTEngine( const long * seeds );
  virtual ~dSFMTEngine();
  // Constructors and destructor.

  dSFMTEngine( const dSFMTEngine & p );
  dSFMTEngine & operator=( const dSFMTEngine & p );
  // Copy constructor and operator=.

  double flat();
  // Returns a pseudo random number between 0 and 1 (excluding the end points).

  void flatArray(const int size, double* vect);
  // Fills an array "vect" of specified size with flat random values.

  void setSeed(long seed, int k=0);
  // Sets the state of the algorithm according to seed.

  void setSeeds(const uint32_t * seeds, int k=0);
  // Sets the state of the algorithm according to the zero terminated
  // array of seeds. It is allowed to ignore one or many seeds in this array.

  void setSeeds(const long * seeds, int k=0);
  // Sets the state of the algorithm according to the zero terminated
  // array of seeds. It is allowed to ignore one or many seeds in this array.

  void saveStatus( const char filename[] = "MTwist.conf") const;
  // Saves the current engine status in the named file

  void restoreStatus( const char filename[] = "MTwist.conf" );
  // Reads from named file the the last saved engine status and restores it.

  void showStatus() const;
  // Dumps the current engine status on the screen.

  operator float();     // returns flat, without worrying about filling bits
  operator unsigned int(); // 32-bit flat, quickest of all

  virtual std::ostream & put (std::ostream & os) const;
  virtual std::istream & get (std::istream & is);
  static  std::string beginTag ( );
  virtual std::istream & getState ( std::istream & is );

  std::string name() const;
  static std::string engineName() {return "MTwistEngine";}

  std::vector<unsigned long> put () const;
  bool get (const std::vector<unsigned long> & v);
  bool getState (const std::vector<unsigned long> & v);

private:
  DSFMT_T* m_dsfmt;
  void init_dsfmt();
  
  static int numEngines;
  static int maxIndex;
}; // dSFMTEngine

}  // namespace CLHEP

#ifdef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
using namespace CLHEP;
#endif

#endif // dSFMTEngine_h
