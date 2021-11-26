/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// dSFMTEngine.h
// base source code copied from CLHEP 1.9.4.7 version of MTwistEngine, than
// adopted to use dSFMT
// author Michael Duehrssen

#ifndef dSFMTEngine_h
#define dSFMTEngine_h

#include <stdexcept>
#include <atomic>
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

  virtual double flat() override;
  // Returns a pseudo random number between 0 and 1 (excluding the end points).

  virtual void flatArray(const int size, double* vect) override;
  // Fills an array "vect" of specified size with flat random values.

  virtual void setSeed(long seed, int k=0) override;
  // Sets the state of the algorithm according to seed.

  void setSeeds(const uint32_t * seeds, int k=0);
  // Sets the state of the algorithm according to the zero terminated
  // array of seeds. It is allowed to ignore one or many seeds in this array.

  virtual void setSeeds(const long * seeds, int k=0) override;
  // Sets the state of the algorithm according to the zero terminated
  // array of seeds. It is allowed to ignore one or many seeds in this array.

  virtual void saveStatus( const char filename[] = "MTwist.conf") const override;
  // Saves the current engine status in the named file

  virtual void restoreStatus( const char filename[] = "MTwist.conf" ) override;
  // Reads from named file the the last saved engine status and restores it.

  virtual void showStatus() const override;
  // Dumps the current engine status on the screen.

  using HepRandomEngine::operator double;
  virtual operator float() override;     // returns flat, without worrying about filling bits
  virtual operator unsigned int() override; // 32-bit flat, quickest of all

  virtual std::ostream & put (std::ostream & os) const override;
  virtual std::istream & get (std::istream & is) override;
  static  std::string beginTag ( );
  virtual std::istream & getState ( std::istream & is ) override;

  virtual std::string name() const override;
  static std::string engineName() {return "MTwistEngine";}

  virtual std::vector<unsigned long> put () const override;
  virtual bool get (const std::vector<unsigned long> & v) override;
  virtual bool getState (const std::vector<unsigned long> & v) override;

private:
  DSFMT_T* m_dsfmt;
  void init_dsfmt();
  
  static std::atomic<int> numEngines;
  static constexpr int maxIndex = 215;
}; // dSFMTEngine

}  // namespace CLHEP

#ifdef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
using namespace CLHEP;
#endif

#endif // dSFMTEngine_h
