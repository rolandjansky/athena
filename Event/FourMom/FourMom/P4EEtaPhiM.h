/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FOURMOM_P4EETAPHIM_H
#define FOURMOM_P4EETAPHIM_H

#include "FourMom/P4EEtaPhiMBase.h"
//#include "SealBase/DebugAids.h"
#include <cmath>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

/** 
   @class P4PxPyPzE

   P4PxPyPzE is a class  with 4-momentum behavior, 
for which Px, Py, Pz and M are data members.
    
   @author David Rousseau rousseau@lal.in2p3.fr
 */

class P4EEtaPhiM : public virtual P4EEtaPhiMBase
{
 public:

  /** constructor with all data members */
  P4EEtaPhiM(const double e, const double eta, const double phi, const double m);

  /** default constructor */
  P4EEtaPhiM();

  /** constructor from hlv to allow conversion from hlv */
  P4EEtaPhiM(const CLHEP::HepLorentzVector & theHlv);

  /** constructor from any 4Momentum reference */
  P4EEtaPhiM(const I4Momentum & theI4M );

  /** constructor from any 4Momentum pointer */
  P4EEtaPhiM(const I4Momentum  * const pI4M );
  
  
  /** get energy data member */
  virtual double e() const;
  /** get eta data member */
  virtual double eta() const;
  /** get phi data member */
  virtual double phi() const;
  /** get mass data member */
  virtual double m() const;

  /** set energy data member */
  virtual void setE (double theE);
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



  double m_e;
  double m_eta;
  double m_phi;
  double m_m;
  
};

inline P4EEtaPhiM::P4EEtaPhiM() :
  P4EEtaPhiMBase(),
  m_e   ( 0.*CLHEP::GeV ),
  m_eta ( 0. ),
  m_phi ( 0. ),
  m_m   ( 0.*CLHEP::GeV )
{}

inline P4EEtaPhiM::P4EEtaPhiM(const CLHEP::HepLorentzVector & theHlv)
{
  this->set4Mom(theHlv);
}

inline P4EEtaPhiM::P4EEtaPhiM(const I4Momentum & theI4Mom ) 
{this->set4Mom(theI4Mom); }

inline P4EEtaPhiM::P4EEtaPhiM(const I4Momentum  * const  theI4Mom )
{this->set4Mom(*theI4Mom); }


inline double P4EEtaPhiM::e() const 
{ return m_e;}

inline double P4EEtaPhiM::eta() const
{ return m_eta;}

inline double P4EEtaPhiM::phi() const
{ return m_phi;}

inline double P4EEtaPhiM::m() const
{ return m_m;}

inline void P4EEtaPhiM::setE(double theE) 
{ m_e = theE;}

inline void P4EEtaPhiM::setEta(double theEta)
{ m_eta = theEta;}

inline void P4EEtaPhiM::setPhi(double thePhi)
{ m_phi = thePhi;}

inline void P4EEtaPhiM::setM(double theM)
{ m_m = theM;}

inline void P4EEtaPhiM::set4Mom(const I4Momentum & theI4Mom )
{ 
  m_e=theI4Mom.e();
  m_eta=theI4Mom.eta();
  m_phi=theI4Mom.phi();
  m_m=theI4Mom.m();
}

inline void P4EEtaPhiM::set4Mom(const I4Momentum * const theI4Mom )
{ 
  this->set4Mom(*theI4Mom);
}

inline void P4EEtaPhiM::set4Mom(const CLHEP::HepLorentzVector & theHlv)
{
  m_e=theHlv.e();
  m_eta=theHlv.eta();
  //FIXME protect against negative energy
  //assert(m_e >= 0,"P4EEtaPhiM::set4Mom cannot have negative energy");
  //assert(m_e >= 0);
  // FIXME of the FIXME in fact it is not necessary to prtoect against negative energy
  // and besides Seal assert does not work
  //  ASSERT( m_e >= 0 );
  m_phi=theHlv.phi();
  m_m=theHlv.m();


}


#endif
