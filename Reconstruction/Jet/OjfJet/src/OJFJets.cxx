/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFJets.cxx,v 1.2 2007-06-09 14:42:00 seuster Exp $


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


// This module contains operations on an entire jet configuration.

// References:
// [1] D. Yu. Grigoriev, E. Jankowski, F. V. Tkachov,
//     Comput. Phys. Commun. 155, 42 (2003) [hep-ph/0301226].
// [2] S. Chumakov, E. Jankowski, F. V. Tkachov, 
//     the accompanying Comput. Phys. Commun. paper [paper.pdf].


#include "OjfJet/OJFJets.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
//RS using namespace std;

//#define NDEBUG


namespace OptimalJetFinder {



  const double Jets::eps_radius = 1.0E-3;



  // This class represents a particle (that takes part in formation of jets).
  // The class is composite and contains
  // (a pointer to) a ParticleOfKinematics object.

  Particle::Particle( unsigned Label, Kinematics kk, const Event* PP ) 
    : next( 0 ), label( Label ), a( 0 ), z( 0 ), xenergy( 0 ), P( PP ) {
    
    assert (  kk == sphere   ||   kk == cylinder  );
    
    if( kk == sphere ) {
      a = new SParticle();
    }
    else {
      a = new CParticle();
    }
    
  }
  

  
  Particle::~Particle() {

    if( a ) delete a;
    if( z ) delete z;
    
  }

  

  // For the spherical kinematics, it returns the fraction of the energy
  // of the particle that does not belong to any jet.
  // For the cylindrical kinematics, it returns the fraction
  // of the transverse energy of the particle that does not belong to any jet.
  // In both cases,
  // the return value is in normalized units ( see Event::Normalize() ).

  double Particle::GetESoft() const {
    return  z->At( 0 ) * xenergy;
  }



  // Returns the energy of the particle in the same units as used in the input.

  double Particle::GetE() const {
    return Get_a()->Get_p()->GetE() * GetEvent()->GetXEnergy();
  }


  
  // Returns the x-component of the momentum of the particle
  // in the same units as used in the input.

  double Particle::GetPx() const {
    return Get_a()->Get_p()->GetX() * GetEvent()->GetXEnergy();
  }



  // Returns the y-component of the momentum of the particle
  // in the same units as used in the input.

  double Particle::GetPy() const {
    return Get_a()->Get_p()->GetY() * GetEvent()->GetXEnergy();
  }



  // Returns the z-component of the momentum of the particle
  // in the same units as used in the input.

  double Particle::GetPz() const {
    return Get_a()->Get_p()->GetZ() * GetEvent()->GetXEnergy();
  }



  // Returns the energy of the particle (for spherical kinematics)
  // or transverse energy of the particle (for the cylindrical kinematics)
  // in the same units as used in the input.

  double Particle::GetXEnergy() const {
    return Get_a()->GetXEnergy() * GetEvent()->GetXEnergy();
  }

  
  
  // Returns the standard angle theta in DEGREES for the spherical kinematics
  // or pseudorapidity eta for the cylindrical kinematics.

  double Particle::GetXEta() const {
    return Get_a()->GetXEta();
  }


  
  // Returns the standard angle phi in DEGREES.

  double Particle::GetPhi() const {
    return Get_a()->GetPhi();
  }



  // Returns the fraction of the particle that belongs to the j-th jet.

  double Particle::GetFractionInJet( unsigned j ) const {
    if( ! GetPosition() ) {
      // RS what shall we do here ?
      //    this should never be called ...
      std::cerr << "OptimalJetFinder::Particle::"
		<< "GetFractionInJet( unsigned ) called "
		<< "before jets are initialized."
		<< std::endl
		<< "Certainly, not a good time to ask for this!"
		<< std::endl;
      exit( 1 );
    }
    assert( GetPosition() );
    //assert(  j >= 0   &&   j <= GetPosition()->GetDim()  );
    assert(  j <= GetPosition()->GetDim()  );
    return GetPosition()->At( j );
  }
      
  
  
  // This class represents the jet finding problem as the minimization
  // of a function defined in a simplex (class ProblemOfZD). 

  ProblemOfJets::ProblemOfJets( unsigned njets, Jets* QQ ) :
    ProblemOfZD( njets ), Q( QQ ), cache( new Position( njets ) ), a( 0 ) {

  }
  


  ProblemOfJets::~ProblemOfJets() {
    if( cache ) { 
      delete cache;
    }
  }

  
  
  // Returns the value of the function being minimized, 
  // which is in this case Omega function, according to eq. (20) in ref. [1].

  double ProblemOfJets::Value() const {
    return Q->GetOmega();
  }

  
  
  // Performs one shift in the simplex.

  void ProblemOfJets::Shift() {
    assert( cache );

    Position* res = cache;
    cache = GetPosition();
    a->SetPosition( res );
    
    GetPosition()->SetToShifted( GetDirection(), GetStep() );
    
    Jet* j = Q->GetFirst();
    while( j ) {
      j->UpdateFor( a, cache );
      j = j->GetNext();
    }
    Q->UpdateEsoftFor( a, cache );
  }
  

  
  // Restores the previous position in the simplex (before the shift),
  // i.e. backs up the shift.

  void ProblemOfJets::BackUp() {
    assert( cache );
    assert( a->GetPosition() );
    
    Position* t = a->GetPosition();
    a->SetPosition( cache );
    cache = t;
    
    Jet* j = Q->GetFirst();
    while( j ) {
      j->BackUp();
      j = j->GetNext();
    }
    Q->BackUpEsoft();
  }

  
  
  // Returns the gradient of the function being minimized,
  // in this case: the omega function of eq. (20) in ref. [1]

  double ProblemOfJets::Gradient( unsigned j ) const {
    return Q->DerivzOmega( a, j );
  }
  
  

  // This class represents a HEP event: a collection
  // of input particles (calorimeter cells, preclusters, etc.).

  Event::Event( Kinematics kk ) : k( kk ), first( 0 ), last( 0 ), normalized( false ), xenergy( 0 ) {}
  

  
  Event::~Event() {
    Clear();
  }
  

  
  // Creates a new particle in the event.
  // RS returns int
  int Event::AddParticle0() {
    if( last == 0 ) {
      first = new Particle( 1, k, this ); 
      last = first;
    }
    else {
      Particle* a = new Particle( last->GetLabel()+1, k, this );
      last->SetNext( a );
      last = a;
    }
    // 'last' is this particle
    return last->GetLabel();
  }

  
  
  // Adds a new particle to the event.
  // E is the energy of the particle in arbitrary units
  // and the standard angles theta and phi in DEGREES
  // describe the direction of the particle. 

  void Event::AddParticle( double E, double theta, double phi ) {
    assert( !normalized );
    AddParticle0();
    last->Get_a()->Set( E, theta, phi );
  }

  

  // Removes all particles from that event and clears storage.

  void Event::Clear() {
    normalized = false;
    Particle *next;
    while( first != 0 ) {
      next = first->GetNext();
      delete first;
      first = next;
    }
    last = 0;
  }

  
  
  // Adds a new particle to the event.
  // px, py, pz are the components of the momentum
  // of the particle in arbitrary units.
  // RS returns int
  int Event::AddParticleRaw( double px, double py, double pz ) {
    assert( !normalized );
    AddParticle0();
    last->Get_a()->SetRaw( px, py, pz );
    // 'last' is this particle
    return last->GetLabel();    
  }

  
  
  // Has to be called before jets are searched;
  // it normalizes the 4-momenta of the particles
  // so that the sum of all energies or transverse energies
  // of all particles is equal to 1.

  void Event::Normalize() {
    Particle* a;  double e;
    
    assert( !normalized );
    
    a = first;  e = 0;
    while( a != 0 ) {  
      e += a->Get_a()->GetXEnergy();  
      a = a->GetNext();  
    }
    assert( e > 0 );
    xenergy = e;
    
    a = first;
    while( a != 0 ) {  
      a->Get_a()->NormalizeBy( e );
      a->SetXEnergy( a->Get_a()->GetXEnergy() );
      a = a->GetNext();  
    }
    
    normalized = true;
  }
  

  
  // Returns the number of particles.

  unsigned Event::GetNumber() const {
    Particle* a; unsigned n;
    
    assert( normalized );
    a = first;  n = 0;
    while( a != 0 )  {  n++;  a = a->GetNext();  }
    return  n;
  }
  
  

  // This class represents a jet (as a part of the jet configuration).
  // The class is composite
  // and contains (a pointer to) a JetOfKinematics object. 

  Jet::Jet( unsigned Label, Jets* QQ, Kinematics kk ) 
    : next( 0 ), Q( QQ ), j( 0 ), jold( 0 ), label( Label ) {
    
    assert (  kk == sphere   ||   kk == cylinder  );
    
    if( kk == sphere ) {
      j = new SJet; 
      jold = new SJet; 
    }
    else { 
      j = new CJet;
      jold = new CJet;
    }
    
  }

  
  
  Jet::~Jet() {
    if( j ) delete j;
    if( jold ) delete jold;
  }


  
  // Evaluates the kinematic parameters of the jet. 

  void Jet::ReevalLinear() {
    Get_j()->Nullify();
    Particle* a = Q->GetEvent()->GetFirst();
    
    while( a ) {
      j->AddProduct( a->GetPosition()->At( label ), a->Get_a() );
      a = a->GetNext();
    }
  }

  
  
  // Updates the kinematic parameters of the jet for the change in zaj.

  void Jet::UpdateFor( Particle* a, Position* old ) {
    double dzaj = a->GetPosition()->At( label ) - old->At( label );

    JetOfKinematics* t = j;
    j = jold;
    jold = t;

    j->SumProduct( jold, dzaj, a->Get_a() );
    j->EvalQtilde();
  }



  // Returns the energy of the jet in the same units as used in the input.

  double Jet::GetE() const {
    return Get_j()->Get_q()->GetE() * GetJets()->GetEvent()->GetXEnergy();
  }



  // Returns the x-component of the momentum of the jet
  // in the same units as used in the input.

  double Jet::GetPx() const {
    return Get_j()->Get_q()->GetX() * GetJets()->GetEvent()->GetXEnergy();
  }
  


  // Returns the y-component of the momentum of the jet
  // in the same units as used in the input.

  double Jet::GetPy() const {
    return Get_j()->Get_q()->GetY() * GetJets()->GetEvent()->GetXEnergy();
  }



  // Returns the z-component of the momentum of the jet
  //  in the same units as used in the input.

  double Jet::GetPz() const {
    return Get_j()->Get_q()->GetZ() * GetJets()->GetEvent()->GetXEnergy();
  }



  // Returns the energy of the jet for spherical kinematics
  // or transverse energy of the jet for cylindrical kinematics.

  double Jet::GetXEnergy() const {
    return Get_j()->GetXEnergy() * GetJets()->GetEvent()->GetXEnergy();
  }


  
  // Returns the standard angle theta of the jet direction (in DEGREES)
  // for the spherical kinematics
  // or the pseudorapidity eta of the jet for the cylindrical kinematics.

  double Jet::GetXEta() const {
    return Get_j()->GetXEta();
  }



  // Returns the standard angle phi of the jet direction in DEGREES.

  double Jet::GetPhi() const {
    return Get_j()->GetPhi();
  }

  
  
  // Restores the jet to the configuration before the shift.

  void Jet::BackUp() {
    JetOfKinematics* t = j;
    j = jold;
    jold = t;
  }
  
  

  // This class represents a configuration of jets.
  // Njets is the number of jets.
  // PP points to the HEP event (where the input data is).
  // RR is the radius parameter of eq. (20) in [1].

  Jets::Jets( unsigned Njets, const Event* PP, double RR ) 
    : P( PP ), R( RR ), njets( Njets ), first( 0 ),
      movesAttempted( 0 ), movesFailed( 0 ),
      pb (new ProblemOfJets(njets, this) ),
      Esoft(0), EsoftOld(0) {
        
    assert(  P   &&   P->IsNormalized()  );
    assert( R > 0 );
    assert( njets > 0 );

    if ( R <= eps_radius ) {
      std::cerr << "In Jets::Jets( unsigned, const Event*, double ), " 
		<< "Radius too small!" << std::endl;
      exit( 1 );
    }
    
    Jet* j;
    Jet* last = 0;
    
    for( unsigned nj = 1; nj <= njets; nj++ ) {
      j = new Jet( nj, this, P->GetKinematics() );
      
      if( nj == 1 ) {
	first = j;
	last = j;
      } 
      else {
	last->SetNext( j );
	last = j;
      }
    }
    
    Particle* a = P->GetFirst();
    while( a ) {
      if( a->GetPosition() ) delete a->GetPosition();
      a->SetPosition( new Position( njets ) );
      a = a->GetNext();
    }

  
  }


  
  Jets::~Jets() {
    
    Jet* next;
    while ( first != 0 ) {
      next = first->GetNext();
      delete first;
      first = next;
    }
     
    if( pb ) delete pb;

  }
  

  
  // Evaluates the soft energy (energy that does not belong to any jet).

  void Jets::EvalEsoft() {
    
    Particle* a = P->GetFirst();
    assert( P->IsNormalized() );
    double s = 0; 
    while( a )  {
      s += a->GetESoft();
      a = a->GetNext();
    }
    Esoft = s;
    
  }
  

  
  // Updates the soft energy after the change in zaj.

  void Jets::UpdateEsoftFor( Particle* a, Position* old ) {
    double dzaj = a->GetPosition()->At( 0 ) - old->At( 0 );
    EsoftOld = Esoft;
    Esoft = Esoft + dzaj * a->GetXEnergyNormalized();
  }
  

  
  // Restores the value of soft energy before the shift.

  void Jets::BackUpEsoft() {
    Esoft = EsoftOld;
  }

  
  
  // Evaluates the parameters of the jets and soft energy.

  void Jets::Reeval() {
    Jet* j = first;
    while( j ) {
      j->ReevalLinear();
      j->Get_j()->EvalQtilde();
      j = j->GetNext();
    }
    EvalEsoft();
  }



  // Set random configuration of jets (initial position 
  // for iterative minimization).

  void Jets::Randomize() {
    movesAttempted = 0;
    movesFailed = 0;
    
    Particle* a = P->GetFirst();
    while( a != 0 )  {
      a->GetPosition()->SetRandom();
      a = a->GetNext();
    }
    Reeval();
  }

  
  
  // Returns the pointer to the n-th jet.

  Jet* Jets::GetJet( unsigned n ) const {
    assert(  n > 0   &&   n <= njets  );
    Jet* j = first;
    while( j->GetLabel() != n )
      j = j->GetNext();
    return  j;
  }



  // Returns the value of Y (fuzziness) according to eq. (12) in ref. [1].

  double Jets::GetY() const {
    assert( njets > 0 );
    Jet* j = first;
    double s = 0;
    while( j )  {
      s += j->Get_j()->GetY();
      j = j->GetNext();
    }
    return  s;
  }

  
  
  // Returns the value of Omega according to eq. (20) in ref. [1].

  double Jets::GetOmega() const {
    return  1 / R / R * GetY() + Esoft;
  }


  
  // Returns the derivative of Omega with respect to zaj
  // according to eq. (29) and (30) in ref. [1].
  // (Factor 2 is missing in the second term on the right hand side 
  // of eq. (30) in ref. [1], but it is correct here.)

  double Jets::DerivzOmega( Particle* a, unsigned j ) const {
    return  1 / ( R * R ) * GetJet(j)->Get_j()->DerivzY( a->Get_a() ) - a->GetXEnergyNormalized();
  }

  
  
  // Moves a single particle within the simplex (a part of one iteration).
  // If the value of Omega from eq. 20 in ref. [1] is reduced,
  // the particle is moved and true is returned.
  // If the value of Omega cannot be reduced (already in a local minimum),
  // the particle is not moved and false is returned.

  bool Jets::MinimizeWrt( Particle* a ) {
    movesAttempted++;
    pb->SetParticle( a );    
    bool success = pb->DescendOnce();
    if( !success ) movesFailed++;
    pb->SetParticle( 0 );
    return success;
  }


  
  // Performs the above MinimizeWrt( a ) for all particles (one iteration).
  // If at least one particle is moved,
  // 'true' is returned ( and Omega is decreased ).
  // If no particle is moved,
  // 'false' is returned ( and Omega stays the same - local minimum ).

  bool Jets::MinimizeEachOnce() {
    bool success = false;
    Particle* a = P->GetFirst();

    while( a ) {
      success = MinimizeWrt( a ) || success;
      a = a->GetNext();
    }

    Reeval();
    return success;
  }



  // Performs the whole iterative minimization.
  // The above Jets::MinimizeEachOnce() performs ONE iteration.
  // Here, all iterations are done.
  // The maximal number of iterations enters as parameter maxIter.
  // If a local minimum is not found within maxIter iterations,
  // 'false' is returned
  // and the minimization is terminated (for safety reasons).
  // If successful, it returns 'true' (how original!).

  bool Jets::Minimize( unsigned maxIter ) {
    
    bool success = false;
    unsigned iter = 0;

    while(  iter < maxIter   &&   ! success  ) {
      success = ! MinimizeEachOnce() ;
      iter++;
    }

    return success;

  }


}
