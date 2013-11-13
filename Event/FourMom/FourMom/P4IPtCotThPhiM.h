/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4IPTCOTTHPHIM_H
#define FOURMOM_P4IPTCOTTHPHIM_H

#include "FourMom/P4IPtCotThPhiMBase.h"
#include <cmath>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

/** 
   @class P4IPtCotThPhiM

   P4IPtCotThPhiM is a concrete class with 4-momentum behavior
for which 1/Pt, cottan(tehta), phi and M are data members.

    
   @author David Rousseau rousseau@lal.in2p3.fr
 */


class P4IPtCotThPhiM : public virtual P4IPtCotThPhiMBase

{
 public:
  /** constructor with all data members */
  P4IPtCotThPhiM(const double iPt, const double cotTh, const double phi, const double m);

  /** default constructor */
  P4IPtCotThPhiM();

  /** constructor from hlv to allow conversion from hlv */
  P4IPtCotThPhiM(const CLHEP::HepLorentzVector & theHlv);
  
  /** constructor from any I4Momentum reference */
  P4IPtCotThPhiM(const I4Momentum & theI4M );

  /** constructor from any I4Momentum pointer */
  P4IPtCotThPhiM(const I4Momentum  * const pI4M );

  /** get inverse pT data member  */
  virtual double iPt() const;
  /** get cot(theta) data member  */
  virtual double cotTh() const;
  /** get phi data member  */
  virtual double phi() const;
  /** get m data member  */
  virtual double m() const;

  /** set inverse pT data member  */
  virtual void setIPt(double theIPt) ;
  /** set cot(theta) data member  */
  virtual void setCotTh(double theCotTh) ;
  /** set phi data member  */
  virtual void setPhi(double thePhi) ;
  /** set m data member  */
  virtual void setM(double theM) ;

  /** set all 4-mom from another I4Momentum reference  */
  virtual void set4Mom (const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */
  virtual void set4Mom (const I4Momentum * const theI4Mom );
  /** set all 4-mom from CLHEP HepLorentzVector  */
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv );

  virtual const I4MomentumError* errors() const;

 protected:

  double m_iPt;
  double m_cotTh;
  double m_phi;
  double m_m;
  
};

inline P4IPtCotThPhiM::P4IPtCotThPhiM() :
  P4IPtCotThPhiMBase( ),
  m_iPt   ( 0./CLHEP::GeV ),
  m_cotTh ( 0. ),
  m_phi   ( 0. ),
  m_m     ( 0.*CLHEP::GeV )
{}

inline P4IPtCotThPhiM::P4IPtCotThPhiM(const CLHEP::HepLorentzVector & theHlv)
{
  this->set4Mom(theHlv);
}

inline P4IPtCotThPhiM::P4IPtCotThPhiM(const I4Momentum & theI4Mom ) 
{this->set4Mom(theI4Mom); }

inline P4IPtCotThPhiM::P4IPtCotThPhiM(const I4Momentum  * const  theI4Mom )
{this->set4Mom(*theI4Mom); }


inline double P4IPtCotThPhiM::iPt() const 
{ return m_iPt;}

inline double P4IPtCotThPhiM::cotTh() const
{ return m_cotTh;}

inline double P4IPtCotThPhiM::phi() const
{ return m_phi;}

inline double P4IPtCotThPhiM::m() const
{ return m_m;}

inline void P4IPtCotThPhiM::setIPt(double theIPt) 
{ m_iPt=theIPt;}

inline void P4IPtCotThPhiM::setCotTh(double theCotTh) 
{ m_cotTh=theCotTh;}

inline void P4IPtCotThPhiM::setPhi(double thePhi) 
{ m_phi=thePhi;}

inline void P4IPtCotThPhiM::setM(double theM) 
{ m_m=theM;}


inline void P4IPtCotThPhiM::set4Mom(const I4Momentum & theI4Mom )
{
  m_iPt=theI4Mom.iPt();
  m_cotTh=theI4Mom.cotTh();
  m_phi=theI4Mom.phi();
  m_m=theI4Mom.m();
}

inline void P4IPtCotThPhiM::set4Mom(const I4Momentum * const theI4Mom )
{
  this->set4Mom(*theI4Mom);
}


inline void P4IPtCotThPhiM::set4Mom(const CLHEP::HepLorentzVector & theHlv)
{
  m_iPt=1./theHlv.perp();
  m_cotTh=theHlv.pz()*m_iPt;
  m_phi=theHlv.phi();
  m_m=theHlv.m();
}


#endif
