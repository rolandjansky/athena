/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFKinematics.cxx,v 1.1.1.1 2007-05-10 09:02:22 seuster Exp $


//  +---------------------------------------------------------+
//  |                                                         |
//  |  OPTIMAL JET FINDER v1.0                                |
//  |                                                         |
//  |  (C) 2005 S. Chumakov, E. Jankowski and F. V. Tkachov   |
//  |                                                         |
//  |  Permission is granted for anyone to use this software  |       
//  |  for any purpose on any computer. The authors are not   |
//  |  responsible for the consequences of such use.          |        
//  |  This software is meant to be freely distributed.       |
//  |                                                         |
//  +---------------------------------------------------------+


// This module deals with the kinematic properties of
// single jets and particles.

// References:
// [1] D. Yu. Grigoriev, E. Jankowski, F. V. Tkachov,
//     Comput. Phys. Commun. 155, 42 (2003) [hep-ph/0301226].
// [2] S. Chumakov, E. Jankowski, F. V. Tkachov, 
//     the accompanying Comput. Phys. Commun. paper [paper.pdf].


#include "OjfJet/OJFKinematics.h"
#include "OjfJet/OJFZD.h"
#include <cmath>
#include <cassert>
//RS using namespace std;

//#define NDEBUG


namespace OptimalJetFinder {


  const double eps_round = 1.0E-6;

  const double JetOfKinematics::eps_norm = 1.0E-6;

  const double CJet::eps_Et = 1.0E-6;



  // Returns the (Lorentz +---) product of the two 4-vectors 
  // OR zero depending on what is larger. 

  double PosProduct( const Vector& v, const Vector& w ) {
    
    double res = v.GetE() * w.GetE() 
      - v.GetX() * w.GetX() 
      - v.GetY() * w.GetY() 
      - v.GetZ() * w.GetZ();
    assert( res >= -eps_round );
    return res>0 ? res : 0;
    
  }


  
  // Sets the spacial components of the 4-vector to the given numbers.
  // The temporal component is set in such a way
  // that the vector is light-like (zero mass).

  void Vector::SetRaw( double px, double py, double pz ) {
    
    E = sqrt( px*px + py*py + pz*pz );
    x = px;
    y = py;
    z = pz;
    
  }
  

  
  // Sets the components of the 4-vector from the given energy and direction.
  // theta and phi are in DEGREEs.

  void Vector::Set( double En, double theta, double phi ) {

    assert(  En >= 0   &&   theta >= 0   &&
	     theta <= 180   &&   fabs(phi) <= 360  );

    theta = theta * pi180;  phi = phi * pi180;

    E = En;
    x = En * sin( theta ) * cos( phi );
    y = En * sin( theta ) * sin( phi );
    z = En * cos( theta );

  }
  

  
  // Normalizes (divides) the 4-vector by the number c.

  void Vector::NormalizeBy( double c ) {

    assert( c != 0 );

    E /= c;
    x /= c;
    y /= c;
    z /= c;
  }

  
  
  // v += c*w

  void Vector::AddProduct( double c, const Vector& w ) {
    E += c * w.E;
    x += c * w.x;
    y += c * w.y;
    z += c * w.z;	
  }
  

  
  // v = old + c*w

  void Vector::SumProduct( const Vector& old, double c, const Vector& w ) {
    E = old.E + c * w.E;
    x = old.x + c * w.x;
    y = old.y + c * w.y;
    z = old.z + c * w.z;
  }
  

  
  // Returns the standard angle phi in DEGREEs
  // (of the direction of the 4-vector).

  double Vector::GetPhi() const {
    double res = atan2( y, x ) / pi180;
    //if( res < 0 )  res += 360;
    if ( res > 180 ) res -= 360;
    return res;
  }



  // Returns the standard angle theta in DEGREEs
  // (of the direction of the 4-vector).

  double Vector::GetTheta() const {
    return atan2(  sqrt( x*x + y*y ),  z  ) / pi180;
  }

  
  
  // Returns the transverse energy.

  double Vector::GetEt() const {
    return  sqrt( x*x + y*y );
  }

  
  inline double arcsh( double x ) {
    //return log( x + sqrt( x*x + 1 ) ); // and change to long double
    if ( x >= 0 ) return log( x + sqrt( x*x + 1 ) );
    return - log( fabs(x) + sqrt( x*x + 1 ) );
  }

  
  
  //Returns the pseudorapidity.

  double Vector::GetEta() const {
    return arcsh( z / GetEt() );
  }
    

  
  // This class deals with general kinematic properties of
  // a particle (calorimeter cell, precluster etc.).
  // (No jet specific properties are touched at this level.)

  ParticleOfKinematics::ParticleOfKinematics() 
    : p( new Vector ), theta( 0 ), phi( 0 ) {
  }

  
  
  ParticleOfKinematics::~ParticleOfKinematics() {
    delete p;
  }

  

  // The next couple methods set the particle's 4-momentum,
  // angles (transverse energy and pseudorapidity) from the given arguments.
  
  void ParticleOfKinematics::Set0( double E, double Theta, double Phi ) {
    //theta = Theta;
    //phi = Phi;
    //if ( phi > 180 ) phi -= 360;
    //p->Set( E, theta, phi );
    p->Set( E, Theta, Phi );
    phi = p->GetPhi();
    theta = p->GetTheta();
  }

  
    
  void CParticle::SetC() {
    Et = Get_p()->GetEt();
    eta = Get_p()->GetEta();
    Eteta = Et * eta;
  }

  

  void SParticle::Set( double En, double Theta, double Phi ) {
    Set0( En, Theta, Phi );
  }
  

  
  void CParticle::Set( double En, double Theta, double Phi ) {
    Set0( En, Theta, Phi );
    SetC();
  }

  
  
  void ParticleOfKinematics::SetRaw0( double px, double py, double pz ) {
    p->SetRaw( px, py, pz );
    theta = acos( p->GetZ() / p->GetE() ) / pi180;
    phi = p->GetPhi();
  }

  
  
  void SParticle::SetRaw( double px, double py, double pz ) {
    SetRaw0( px, py, pz );
  }
  

  
  void CParticle::SetRaw( double px, double py, double pz ) {
    SetRaw0( px, py, pz );
    SetC();
  }

  
 
  // Divides the particle's 4-momentum by the given number c.

  void SParticle::NormalizeBy( double c ) {
    Get_p()->NormalizeBy( c );
  }

  
  
  // Divides the particle's 4-momentum by the given number c
  // and updates the transverse energy and pseudorapidity.

  void CParticle::NormalizeBy( double c ) {
    Get_p()->NormalizeBy( c );
    //SetC();
    Et /= c;
    Eteta /= c;
  }
  
  

  // This class deals with general kinematic properties of a jet.

  JetOfKinematics::JetOfKinematics() 
    : q( new Vector ), qtilde( new Vector ) {
  }


  
  JetOfKinematics::~JetOfKinematics() {
    delete q;
    delete qtilde;
  }

  

  // Assigns (0,0,0,0) to the jet's 4-momentum.

  void SJet::Nullify() {
    Get_q()->Nullify();
  }
  

  
  // Assigns (0,0,0,0) to the jet's 4-momentum
  // and updates the transverse energy and pseudorapidity accordingly.

  void CJet::Nullify() {
    Get_q()->Nullify();
    Et = 0;
    eta = 0;
    Eteta = 0;
  }
  


  // To the 4-momentum of the jet, it adds the 4-momentum
  // of the particle that the argument points to
  // times the given factor c.
  // "q += c * (a->p)"

  void JetOfKinematics::AddProduct( double c, ParticleOfKinematics* a ) {
    q->AddProduct( c, *a->Get_p() );
  }

  

  // As above for the spherical kinematics.

  void SJet::AddProduct( double c, ParticleOfKinematics* a ) {
    JetOfKinematics::AddProduct( c, a );
  }


  
  // As above for the cylindrical kinematics.
  // Transverse energy and (transverse energy)*(pseudorapidity)
  // are updated accordingly.

  void CJet::AddProduct( double c, ParticleOfKinematics* a ) {
    JetOfKinematics::AddProduct( c, a );
    Et += c * a->GetXEnergy();
    Eteta += c * a->GetXEnergy() * a->GetXEta();
  }


  
  // The 4-momentum of the jet is assigned the sum 
  // of the 4-momentum of the argument jet jold
  // and the 4-momentum of the particle a points to
  // times the given factor c.
  // "q = (jold->q) + c * (a->p)"

  void JetOfKinematics::SumProduct( JetOfKinematics* jold,
				    double c,
				    ParticleOfKinematics* a ) {

    q->SumProduct( *jold->Get_q(), c, *a->Get_p() );

  }
  

  
  // As above for the spherical kinematics.

  void SJet::SumProduct( JetOfKinematics* jold,
			 double c,
			 ParticleOfKinematics* a ) {

    JetOfKinematics::SumProduct( jold, c, a );

  }

  

  // As above for the cylindrical kinematics.
  // Transverse energy and (transverse energy)*(pseudorapidity)
  // are updated accordingly.

  void CJet::SumProduct( JetOfKinematics* jold,
			 double c,
			 ParticleOfKinematics* a ) {

     JetOfKinematics::SumProduct( jold, c, a );

     Et = jold->GetXEnergy() + c * a->GetXEnergy();
     Eteta = jold->GetXEta() * jold->GetXEnergy()
           + c * a->GetXEnergy() * a->GetXEta();

  }



  // Evaluates q_tilde according to eq. (13)-(16) in ref. [1]
  // (spherical kinematics).

  void SJet::EvalQtilde() {
    Get_qtilde()->SetE( 1 );
    double norm = sqrt( Get_q()->GetX() * Get_q()->GetX() 
			+ Get_q()->GetY() * Get_q()->GetY() 
			+ Get_q()->GetZ() * Get_q()->GetZ() );
    if( norm >= eps_norm ) {
      Get_qtilde()->SetX( Get_q()->GetX() / norm );
      Get_qtilde()->SetY( Get_q()->GetY() / norm );
      Get_qtilde()->SetZ( Get_q()->GetZ() / norm );	
    }
    else {
      double x = OJFRandom::Random();
      double y = OJFRandom::Random();
      double z = OJFRandom::Random();
      norm = sqrt( x*x + y*y + z*z );
      Get_qtilde()->SetX( x/norm );
      Get_qtilde()->SetY( y/norm );
      Get_qtilde()->SetZ( z/norm );
      //Get_qtilde()->SetX( 1 );
      //Get_qtilde()->SetY( 0 );
      //Get_qtilde()->SetZ( 0 );
    }
  }
  

  
  // Evaluates q_tilde according to eq. (15),(16),(18),(19) in ref. [1]
  // (cylindrical kinematics).

  void CJet::EvalQtilde() {
    
    double norm = sqrt( Get_q()->GetX() * Get_q()->GetX() 
			+ Get_q()->GetY() * Get_q()->GetY() );
    if( norm >= eps_norm ) {
      Get_qtilde()->SetX( Get_q()->GetX() / norm );
      Get_qtilde()->SetY( Get_q()->GetY() / norm );
    }
    else {
      double x = OJFRandom::Random();
      double y = OJFRandom::Random();
      norm = sqrt( x*x + y*y );
      Get_qtilde()->SetX( x/norm );
      Get_qtilde()->SetY( y/norm );
      //Get_qtilde()->SetX( 1 );
      //Get_qtilde()->SetY( 0 );
    }

    if( Et >= eps_Et ) {
      eta = Eteta / Et;
    }
    else {
      //eta = 0;
      eta = ( OJFRandom::Random() - 0.5 ) * 6;
    }

    Get_qtilde()->SetE( cosh( eta ) );
    Get_qtilde()->SetZ( sinh( eta ) );

  }
  

  
  // Returns the standard angle phi of the direction of the jet; in DEGREES. 

  double JetOfKinematics::GetPhi() const {
    return qtilde->GetPhi();
  }
  

  
  // Returns the fuzziness Y of a single jet according to
  // a single element of the sum in eq. (12) in ref. [1].

  double JetOfKinematics::GetY() const {
    return 2 * PosProduct( *q, *qtilde );
  }

  
  
  // Derivative of Y for a single jet with respect to z_a
  // (spherical kinematics) accoding to
  // first term on the right hand side of eq. (29) in ref. [1].

  double SJet::DerivzY( ParticleOfKinematics* a ) const {
    return  2 * PosProduct( *Get_qtilde(), *a->Get_p() );
  }
  

  
  // Derivative of Y for a single jet with respect to z_a
  // (cylindrical kinematics) according to
  // first two terms on the right hand side of eq. (30) in ref. [1].
  // (Factor 2 is missing in the second term on the right hand side
  // of eq. (30) in ref. [1], but it is correct here).

  double CJet::DerivzY( ParticleOfKinematics* a ) const {
    double r, res;
      
    res = 2 * PosProduct( *a->Get_p(), *Get_qtilde() );

    double x = Et;
    if ( Et < eps_Et ) x = eps_Et;

    r = a->GetXEnergy() / x * ( a->GetXEta() - eta );

    r = r * ( Get_q()->GetE() * Get_qtilde()->GetZ() 
	      - Get_q()->GetZ() * Get_qtilde()->GetE() );
    return res + 2 * r;
  }

  
}  
