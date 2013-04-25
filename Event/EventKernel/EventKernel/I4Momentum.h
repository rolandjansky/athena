///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTKERNEL_I4MOMENTUM_H
#define EVENTKERNEL_I4MOMENTUM_H 1

// STL includes
#include <ostream>

#include "CLHEP/Vector/LorentzVector.h"

class I4MomentumError;

/** 
   @class I4Momentum

   I4Momentum is an abstract base class providing 4-momentum behavior.
   Concrete base classes and some implementation are provided in package 
   Event/FourMom

    
   @author David Rousseau  rousseau@lal.in2p3.fr
   @author Sebastien Binet binet@cern.ch
 */


class I4Momentum 
{
 public:
  enum Kind { P4EETAPHIM, P4IPTCOTTHPHIM, P4PTETAPHIM, P4PXPYPZE, UNKNOWN };


  /** virtual destructor needed */
  virtual ~I4Momentum();
  /** x component of momentum */
  virtual double px() const = 0;
  /** y component of momentum */
  virtual double py() const = 0 ;
  /** z component of momentum */
  virtual double pz() const = 0 ;
  /** mass */
  virtual double m() const = 0 ;
  /** mass squared */
  virtual double m2() const = 0 ;
  /** momentum magnitude */
  virtual double p() const = 0 ;
  /** square of momentum magnitude */
  virtual double p2() const = 0 ;
  /** pseudo rapidity */
  virtual double eta() const = 0 ;
  /** rapidity */
  virtual double rapidity() const = 0 ;
  /** phi in [-pi,pi[ */
  virtual double phi() const = 0 ;
  /** energy */
  virtual double e() const = 0 ;
  /** transverse energy defined to be e*sin(theta) */
  virtual double et() const = 0 ;   
  /** transverse momentum */
  virtual double pt() const = 0 ;
  /** inverse of transverse momentum */
  virtual double iPt() const = 0;
  /** cosinus phi */
  virtual double cosPhi() const = 0 ;
  /** sinus phi */
  virtual double sinPhi() const = 0 ;
  /** cosinus theta */
  virtual double cosTh() const = 0 ;
  /** sinus theta */
  virtual double sinTh() const = 0 ;
  /** cottan theta */
  virtual double cotTh() const = 0 ;
  /** tan theta */
  virtual double tanTh() const = 0 ;
  /** CLHEP HepLorentzVector */
  virtual CLHEP::HepLorentzVector hlv() const = 0 ;

  /** Access to errors, if available; returns 0 if no errors*/
  virtual const I4MomentumError* errors() const = 0;

  /** tells what kind of P4XYZT this is */
  virtual Kind kind() const = 0;


  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const I4Momentum * const theI4Mom ) = 0;

  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const I4Momentum & theI4Mom) = 0;

  /** set 4Momentum (will throw exception if cannot be implemented) */
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv ) = 0;
 

  /// Print @c I4Momentum content
  virtual std::ostream& dump( std::ostream& out ) const = 0;

};

std::ostream& operator<<( std::ostream& out, const I4Momentum& p4 );

inline std::ostream& operator<<( std::ostream& out, const I4Momentum& p4 )
{
  return p4.dump( out );
}

// MsgStream& operator<<( MsgStream& msg, const I4Momentum& p4 );
// inline MsgStream& operator<<( MsgStream& msg, const I4Momentum& p4 )
// {
//  std::stringstream out;
//  p4.dump( out );
//  msg << out.str();
//  return msg;
// }

#endif
