/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4PTETAPHIM_H
#define FOURMOM_P4PTETAPHIM_H

#include "FourMom/P4PtEtaPhiMBase.h"
//#include "SealBase/DebugAids.h"
#include <cmath>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

/** 
   @class P4PtEtaPhiM

   P4PtEtaPhiM is a class  with 4-momentum behavior, 
for which Pt, Eta, Phi and M are data members.
    
   @author Olga Igonkina olya@cern.ch
 */

class P4PtEtaPhiM : public virtual P4PtEtaPhiMBase
{
 public:

  /** constructor with all data members */
  P4PtEtaPhiM(const double pt, const double eta, const double phi, const double m)
    : m_pt(pt), m_eta(eta), m_phi(phi), m_m(m) {};

  /** default constructor */
  P4PtEtaPhiM();

  /** constructor from hlv to allow conversion from hlv */
  P4PtEtaPhiM(const CLHEP::HepLorentzVector & theHlv);

  /** constructor from any 4Momentum reference */
  P4PtEtaPhiM(const I4Momentum & theI4M );

  /** constructor from any 4Momentum pointer */
  P4PtEtaPhiM(const I4Momentum  * const pI4M );
  
  
  /** get pt data member */
  virtual double pt() const;
  /** get eta data member */
  virtual double eta() const;
  /** get phi data member */
  virtual double phi() const;
  /** get mass data member */
  virtual double m() const;

  /** set pt data member */
  virtual void setPt (double thePt);
  /** set eta data member */
  virtual void setEta (double theEta);
  /** set phi data member */
  virtual void setPhi (double thePhi);
  /** set mass data member */
  virtual void setM (double theM);

  /** set all 4-mom from another I4Momentum reference  */  
  virtual void set4Mom (const I4Momentum & theI4Mom );
  /** set all 4-mom from another I4Momentum pointer  */  
  virtual void set4Mom (const I4Momentum * const theI4Mom );
  /** set all 4-mom from a CLHEP HepLorentzVector */  
  virtual void set4Mom (const CLHEP::HepLorentzVector & theHlv );  
  
  virtual const I4MomentumError* errors() const;

 protected:



  double m_pt;
  double m_eta;
  double m_phi;
  double m_m;
  
};

inline P4PtEtaPhiM::P4PtEtaPhiM() :
  P4PtEtaPhiMBase( ),
  m_pt  ( 0.*CLHEP::GeV ),
  m_eta ( 0. ),
  m_phi ( 0. ),
  m_m   ( 0.*CLHEP::GeV )
{}

inline P4PtEtaPhiM::P4PtEtaPhiM(const CLHEP::HepLorentzVector & theHlv)
{
  this->set4Mom(theHlv);
}

inline P4PtEtaPhiM::P4PtEtaPhiM(const I4Momentum & theI4Mom ) 
{this->set4Mom(theI4Mom); }

inline P4PtEtaPhiM::P4PtEtaPhiM(const I4Momentum  * const  theI4Mom )
{this->set4Mom(*theI4Mom); }


inline double P4PtEtaPhiM::pt() const 
{ return m_pt;}

inline double P4PtEtaPhiM::eta() const
{ return m_eta;}

inline double P4PtEtaPhiM::phi() const
{ return m_phi;}

inline double P4PtEtaPhiM::m() const
{ return m_m;}

inline void P4PtEtaPhiM::setPt(double thePt) 
{ m_pt = thePt;}

inline void P4PtEtaPhiM::setEta(double theEta)
{ m_eta = theEta;}

inline void P4PtEtaPhiM::setPhi(double thePhi)
{ m_phi = thePhi;}

inline void P4PtEtaPhiM::setM(double theM)
{ m_m = theM;}

inline void P4PtEtaPhiM::set4Mom(const I4Momentum & theI4Mom )
{ 
  m_pt=theI4Mom.pt();
  m_eta=theI4Mom.eta();
  m_phi=theI4Mom.phi();
  m_m=theI4Mom.m();
}

inline void P4PtEtaPhiM::set4Mom(const I4Momentum * const theI4Mom )
{ 
  this->set4Mom(*theI4Mom);
}

inline void P4PtEtaPhiM::set4Mom(const CLHEP::HepLorentzVector & theHlv)
{
  m_pt=theHlv.perp();
  m_eta=theHlv.eta();
  //FIXME protect against negative energy
  //assert(m_e >= 0,"P4PtEtaPhiM::set4Mom cannot have negative energy");
  //assert(m_e >= 0);
  // FIXME of the FIXME in fact it is not necessary to prtoect against negative energy
  // and besides Seal assert does not work
  //  ASSERT( m_e >= 0 );
  m_phi=theHlv.phi();
  m_m=theHlv.m();


}


#endif
