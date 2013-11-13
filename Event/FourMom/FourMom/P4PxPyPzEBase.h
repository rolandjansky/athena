///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4PXPYPZEBASE_H
#define FOURMOM_P4PXPYPZEBASE_H
#include "EventKernel/I4Momentum.h"
#include "CLHEP/Vector/LorentzVector.h"
#include <ostream>

/** 
   @class P4PxPyPzEBase

   P4PxPyPzEBase is a base class for classes with 4-momentum behavior, 
for which Px, Py, Pz and M are natural parameters.
   Any class deriving from it should implement px(), py(), pz(), e().
    
   @author David Rousseau rousseau@lal.in2p3.fr
 */


class P4PxPyPzEBase : public virtual I4Momentum

{
 public:

  /**virtual destructor needed by pool */
  virtual ~P4PxPyPzEBase();

  virtual double m() const ;
  virtual double m2() const ;
  virtual double eta() const ;
  virtual double phi() const ;
  virtual double p() const;
  virtual double p2() const;
  virtual double pt()  const;
  virtual double et()  const;
  virtual double iPt() const;
  virtual double rapidity() const;
  virtual double cosPhi() const;
  virtual double sinPhi() const;
  virtual double tanTh() const;
  virtual double cotTh() const;
  virtual double cosTh() const;
  virtual double sinTh() const;
  virtual CLHEP::HepLorentzVector hlv() const;

  virtual const I4MomentumError* errors() const;

  /** set all 4-mom from another I4Momentum reference DUMMY IMPLEMENTATION  */  
  virtual void set4Mom (const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  DUMMY IMPLEMENTATION */  
  virtual void set4Mom (const I4Momentum * const theI4Mom );
  /** set all 4-mom from a CLHEP HepLorentzVector DUMMY IMPLEMENTATION */  
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv );  

  virtual I4Momentum::Kind kind() const { return I4Momentum::P4PXPYPZE; };

  virtual std::ostream& dump( std::ostream& out ) const;
};


#endif
