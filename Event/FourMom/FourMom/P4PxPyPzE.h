/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4PXPYPZE_H
#define FOURMOM_P4PXPYPZE_H

#include "FourMom/P4PxPyPzEBase.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <cmath>


/** 
   @class P4PxPyPzE

   P4PxPyPzE is a class with 4-momentum behavior, 
for which Px, Py, Pz and M are data members.

    
   @author David Rousseau rousseau@lal.in2p3.fr
 */


class P4PxPyPzE  : public virtual P4PxPyPzEBase
{

 public:

  /** constructor with all data members */
  P4PxPyPzE(const double px,const  double py,const  double pz,const  double e);
  /** constructor from hlv to allow conversion from hlv */
  P4PxPyPzE(const CLHEP::HepLorentzVector & theHlv);
  /** constructor from any I4Momentum reference */
  P4PxPyPzE(const I4Momentum & theI4M );
  /** constructor from any I4Momentum pointer */
  P4PxPyPzE(const I4Momentum  * const theI4M );

  /** default constructor */
  P4PxPyPzE();

  /** get px data member */
  virtual double px() const;
  /** get py data member */
  virtual double py() const;
  /** get pz data member */
  virtual double pz() const;
  /** get energy data member */
  virtual double e() const;

  /** set Px data member */
  virtual void setPx(double thePx) ;
  /** set Py data member */
  virtual void setPy(double thePy) ;
  /** set Pz data member */
  virtual void setPz(double thePz) ;
  /** set energy data member */
  virtual void setE(double theE) ;

  /** set all 4-mom from another I4Momentum reference  */
  virtual void set4Mom (const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */
  virtual void set4Mom (const I4Momentum * const theI4Mom );
  /** set all 4-mom from CLHEP HepLorentzVector  */
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv );

  virtual const I4MomentumError* errors() const;

  /** Experimental : inlined function to get  px without going through virtual inheritance */
  double pxi() const;
  /** Experimental : inlined function to get  py without going through virtual inheritance */
  double pyi() const;
  /** Experimental : inlined function to get  pz without going through virtual inheritance */
  double pzi() const;
  /** Experimental : inlined function to get  e without going through virtual inheritance */
  double ei() const;
  /** Experimental : inlined function to get pt without going through virtual inheritance */
  double pti() const;


 protected:




  double m_px;
  double m_py;
  double m_pz;
  double m_e;
  
};

inline P4PxPyPzE::P4PxPyPzE( ) :
  P4PxPyPzEBase( ),
  m_px ( 0.*CLHEP::GeV ),
  m_py ( 0.*CLHEP::GeV ),
  m_pz ( 0.*CLHEP::GeV ),
  m_e  ( 0.*CLHEP::GeV )
{}

inline P4PxPyPzE::P4PxPyPzE(const double px, const double py, const double pz, const double e):
  m_px(px),m_py(py),m_pz(pz),m_e(e)
{
}

inline P4PxPyPzE::P4PxPyPzE(const CLHEP::HepLorentzVector & theHlv)
{
  this->set4Mom(theHlv);
}

inline P4PxPyPzE::P4PxPyPzE(const I4Momentum & theI4Mom ) 
{this->set4Mom(theI4Mom); }

inline P4PxPyPzE::P4PxPyPzE(const I4Momentum  * const  theI4Mom )
{this->set4Mom(*theI4Mom); }



inline double P4PxPyPzE::px() const 
{ return m_px;}


inline double P4PxPyPzE::py() const
{ return m_py;}

inline double P4PxPyPzE::pz() const
{ return m_pz;}

inline double P4PxPyPzE::e() const
{ return m_e;}

inline void P4PxPyPzE::setPx(double thePx) 
{ m_px=thePx;}

inline void P4PxPyPzE::setPy(double thePy) 
{ m_py=thePy;}

inline void P4PxPyPzE::setPz(double thePz) 
{ m_pz=thePz;}

inline void P4PxPyPzE::setE(double theE) 
{ m_e=theE;}

inline void P4PxPyPzE::set4Mom(const I4Momentum & theI4Mom )
{
  m_px=theI4Mom.px();
  m_py=theI4Mom.py();
  m_pz=theI4Mom.pz();
  m_e=theI4Mom.e();
}

inline void P4PxPyPzE::set4Mom(const I4Momentum * const theI4Mom )
{
  this->set4Mom(*theI4Mom);
}


inline void P4PxPyPzE::set4Mom(const CLHEP::HepLorentzVector & theHlv)
{
  m_px=theHlv.px();
  m_py=theHlv.py();
  m_pz=theHlv.pz();
  m_e=theHlv.e();
}


 inline double P4PxPyPzE::pxi() const 
{ return m_px;}

inline double P4PxPyPzE::pyi() const
{ return m_py;}

inline double P4PxPyPzE::pzi() const
{ return m_pz;}

inline double P4PxPyPzE::ei() const
{ return m_e;}

inline double P4PxPyPzE::pti() const
{ const double thePx=pxi();
 const double thePy=pyi();
 return sqrt(thePx*thePx+thePy*thePy);}


#endif
