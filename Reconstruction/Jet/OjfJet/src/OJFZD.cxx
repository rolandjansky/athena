/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFZD.cxx,v 1.5 2008-02-15 14:31:34 seuster Exp $


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


// This module is about minimization of a function defined in a simplex.
// No jet specifics.

// References:
// [1] D. Yu. Grigoriev, E. Jankowski, F. V. Tkachov,
//     Comput. Phys. Commun. 155, 42 (2003) [hep-ph/0301226].
// [2] S. Chumakov, E. Jankowski, F. V. Tkachov, 
//     the accompanying Comput. Phys. Commun. paper [paper.pdf].


#include "OjfJet/OJFZD.h"
#include <cassert>
#include <cmath>


using CLHEP::RandFlat;


//RS using namespace std;

//#define NDEBUG


namespace OptimalJetFinder {

  //const double eps_sum = 1.0E-8;
  //const double eps_round = 1.0E-6;


  const double Position::eps_snap = 1.0E-3; 
  const double Position::eps_sum = 1.0E-8;
  const double Position::eps_sum0 = 1.0E-6;
  const double Position::eps_sum1 = 1.0E-4;
  const double Position::eps_round = 1.0E-6;


  const double Direction::eps_sum = 1.0E-8;
  const double Direction::eps_round = 1.0E-6;
  const double Direction::INF =  1.0E+30;


  const double ProblemOfZD::min_dist = 1.0E-6;
  const double ProblemOfZD::stepFactor = 0.3666667;

  /* switch to ATLAS random engine
     
     const unsigned OJFRandom::m = 259200;
     const unsigned OJFRandom::b = 7141;
     const unsigned OJFRandom::c = 54773;
     const double OJFRandom::z = 1/double( m + 1 );
     
     
     
     unsigned OJFRandom::ojf_random = 12345;  
     
     
     
     double OJFRandom::Random() {
     
     ojf_random = ojf_random % m;
     ojf_random = ( ojf_random * b + c ) % m;
     double res = ( ojf_random + 1 ) * z;
     assert(  res > 0   &&   res < 1  );
     return res;
     
     }
  */

  double OJFRandom::Random() {
    return RandFlat::shoot();
  }
  
  
  namespace {
    
    double max( double x, double y ) { return x>y ? x : y; }

    
    double min( double x, double y ) { return x<y ? x : y; }
    
  }

  

  // This class represents a position in the simplex z(0,1,2,...,dim).

  Position::Position( unsigned Dim ) :
    z( new double[Dim+1] ), len_z( Dim+1 ), dim( Dim ) {
    
    for( unsigned j = 0; j <= dim; j++  ) 
      z[j] = 0;

  }



  Position::Position( const Position* zz ) :
    z( new double[zz->GetDim()+1] ),
    len_z( zz->GetDim()+1 ), dim( zz->GetDim() ) {
    
    
    for( unsigned j = 0; j <= dim; j++  ) 
      z[j] = zz->At( j );

  }



  // Copies the value of z(0,1,2,...,dim) from another Position.

  void Position::Copy( const Position* zz ) {

    assert( dim == zz->GetDim() );
    
    for( unsigned j = 0; j <= dim; j++  ) 
      z[j] = zz->At( j );
    
  }

  
  
  Position::~Position() {
    delete[] z;
  }



  // Returns the i-th component of z(0,1,2,...,dim). 

  double Position::At( unsigned i ) const { 
    //assert(  i >= 0   &&   i <= dim  );
    assert(  i <= dim  );
    return z[i];
  }



  // Verifies that z(0,1,2,...,dim) is a proper point of the simplex
  // and verifies that the snapping works properly.

  void Position::Validate() const {
    
    assert( dim <= len_z - 1 );
    double s [[maybe_unused]] = 0;
    double zj;  
    for( unsigned j = 0; j <= dim; j++ ) {
      zj = z[j];  assert(  zj >= eps_snap   ||   zj == 0  );
      s += zj;
    }
    assert( fabs( s - 1 ) < eps_sum1 );
    
  }


  
  // A point of the simplex that is close enough to the simplex boundary
  // is snapped to the boundary, i.e. z(j) < eps_snap => z(j) = 0.

  void Position::Snap() {
    
    double s = 0;
    unsigned nzeros = 0;
    double zj;
    for( unsigned j = 0; j <= dim; j++ ) {
      zj = z[j];
      assert(  zj > -eps_round   &&   zj < 1+eps_round  );
      if( zj < eps_snap ) {
	zj = 0;
	nzeros++;
      }
      else  s += zj;
      z[j] = zj;
    }
    assert( fabs(s - 1) < nzeros * eps_snap + eps_sum );
    if( nzeros == dim ) {
      unsigned j = 0;  
      while( z[j] == 0 ) j++;
      z[j] = 1;
    }
    else  
      for( unsigned j = 0; j <= dim; j++ )  z[j] /= s;
    Validate();	
  }


  
  // An arbitrary z(0,1,...,dim) is forced to be a point of the simplex.

  void Position::ForceToSimplex() {
    
    double s = 0;
    for( unsigned j = 0; j <= dim; j++ ) {
      z[j] = max( z[j], 0.0 );  s += z[j];
    }
    if( s < eps_sum0 ) {
      s = 1 / (double)(dim + 1);
      for( unsigned j = 0; j <= dim; j++ )  z[j] = s;
    }
    else
      for( unsigned j = 0; j <= dim; j++ )  z[j] /= s;

    Snap();
  }


  
  namespace {

    // Sorts a vector in increasing order.

    void Sort( unsigned dim, double a[], unsigned len_a ) {
      unsigned i, j;  double t;
    
      assert( dim <= len_a - 1 );
      if( dim <= len_a - 1 )
	//std::cout << "Weird. Shouldn't be here - OjfJet (Sort)\n";
      for( j = 0; j <= dim - 1; j++ ) {
	t = a[j + 1];
	i = j;
	if( a[j] > t ) {
	  do {
	    a[i + 1] = a[i];
	    i--;
	    // RS unsigned always non-negative ! } while( ! (  i < 0U   ||   a[i] <= t  ) ); 
	  } while( ! ( a[i] <= t ) ); 
	  a[i + 1] = t;
	}
      }
      for( j = 0; j <= dim - 1; j++ )  assert( a[j] <= a[j + 1] );
    }
    
  }



  // Sets z(0,1,...,dim) to a random position in the simplex.

  void Position::SetRandom() {
    
    z[0] = 0;
    for( unsigned j = 1; j <= dim; j++ ) z[j] = OJFRandom::Random();
    Sort( dim, z, len_z );
    for( unsigned j = 0; j <= dim - 1; j++ ) z[j] = z[j + 1] - z[j];
    z[dim] = 1 - z[dim];
    
    for( unsigned j = 0; j <= dim; j++ )  assert( z[j] >= 0 );
    
    Snap();
  }


   
  // Sets z(0,1,...,dim) to a given value and then enforces
  // the position to belong to the simplex.

  void Position::SetTo( const double* z0, unsigned len_z0 ) {
    assert( len_z0 - 1 >= dim );
    if( len_z0 - 1 >= dim )
      //std::cout << "Weird. Shouldn't be here - OjfJet (SetTo)\n";
    for( unsigned j = 0; j <= dim; j++ ) z[j] = z0[j];
    ForceToSimplex();
  }
  


  // Shifts the position to a new point in the simplex
  // based on the position and direction contained in d.

  void Position::SetToShifted( const Direction *d, double step ) {
    assert(  step > 0   &&   step <= d->GetMaxStep()  );
    for( unsigned j = 0; j <= d->GetDim(); j++ )
      z[j] = d->GetPosition()->At(j) + step * d->At(j);
    Snap();
  }

  
  
  // This class represent a direction vector d(0,1,...,dim)
  // in which a point on the simplex should be moved
  // to decrease the value of the function that is being minimized.

  Direction::Direction( unsigned Dim ) : dim(Dim), z(0),
				    d( new double[Dim + 1] ), 
				    f( new double[Dim + 1] ), 
				    New(new double[Dim + 1] ),
				    maxStep(0), norm(0) {
        
  }


  
  Direction::~Direction() {
    
    delete[] d;
    delete[] f;
    delete[] New;
    
  }

  
  
  // Returns the i-th component of d(0,1,...,dim).

  double Direction::At( unsigned i ) const {
    //assert(  i >= 0   &&   i <= dim  );
    assert(  i <= dim  );
    return d[i];
  }

  
  
  // Verifies that the shift in the direction d(0,1,...,dim)
  // will not move the point out of the simplex (for appropriately small step).

  void Direction::Validate() const {
    
    assert(  z == 0   ||   dim == z->GetDim()  );
    
    double s [[maybe_unused]] = 0;
    for( unsigned j = 0; j <= dim; j++ ) {
      assert(  z->At(j) != 0   ||   d[j] >= 0  );
      s += d[j];
    }
    
    assert( fabs( s ) < eps_sum );
    
  }
  

  
  // Returns the norm of the direction d(0,1,...,dim)
  // according to eq. (27) in [1].

  double Direction::Norm( const double *dd ) const {
    
    double s = 0;
    for( unsigned j = 0; j <= dim; j++ )
      s = max( s, fabs( *(dd + j) ) );
    return s;
    
  }


  
  // As described in section 2.2 of [1],
  // out of the possible valid directions the one is chosen that
  // has maximal norm. 

  void Direction::Snap() {
    
    double  newNorm, s;  double *t;
    
    unsigned Dim = dim;
    double maxNorm = -1; 
    
    // this->d is the best direction so far;
    // will be replaced at first iteration.
    
    for( unsigned J = 0; J <= Dim; J++ ) {
      if( z->At(J) > 0 ) {
	s = 0;
	for( unsigned j = 0; j <= Dim; j++ ) {
	  New[j] = f[j] - f[J];
	  if( z->At(j) == 0 )  New[j] = max( 0.0, New[j] );
	  if( j != J ) s += New[j];
	}
	New[J] = - s;
	
	s = 0;
	for( unsigned j = 0; j <= Dim; j++ ) s += New[j];
	assert( fabs(s) < eps_sum );
	
	s = 0;
	for( unsigned j = 1; j <= Dim; j++ )  s += f[j] * New[j];
	assert( s > - eps_round );
	
	newNorm = Norm( New ); 
	if( newNorm > maxNorm ) { // Make d the best direction.
	  maxNorm = newNorm;
	  t = New; New = d;  d = t;
	}
      }
    }
    norm = maxNorm;
    Validate();
  }


  
  // Returns the maximal value of step that will not lead out of the simplex
  // according to (28) in ref. [1].

  double Direction::MaxStep() const {
    double step, t;
    
    step = INF;
    for( unsigned j = 0; j <= dim; j++  ) {
      if(  z->At(j) != 0   &&   d[j] != 0  ) {
	t = - z->At(j) / d[j];
	if(  t >= 0  )
	  step = min( step, t );
	// else step is unrestricted in this direction.
      }
    }

    if( step >= INF )  step = 0; 
    return step;
  }

  
  // Chooses the best direction to move within the simplex,
  // as described in section 2.2 of ref. [1].
  void Direction::SetOptimal( ProblemOfZD* pb ) {
    z = pb->GetPosition();
    f[0] = 0;
    for( unsigned j = 1; j <= pb->GetDim(); j++ ) f[j] = - pb->Gradient( j );
    Snap();
    maxStep = MaxStep();
  }

  
  
  // This is an abstract class that represents
  // the minimization problem (one iteration).

  ProblemOfZD::ProblemOfZD( unsigned Dim ) : dim(Dim), d( new Direction(Dim) ), step(0) {
    assert( Dim > 0 );
  }
  
  
  ProblemOfZD::~ProblemOfZD() {
    delete d;
  }



  // Returns true and performs one move
  // that reduces the value of the function being minimized
  // or returns false if already in the local minimum
  // (and no move has been performed).
  // Value(), Shift(), BackUp() are pure virtual functions to be defined
  // in a derived class. 

  bool ProblemOfZD::DescendOnce() {
    
    d->SetOptimal(this);
    step = d->GetMaxStep();

    do {

      if( d->GetNorm() * step < min_dist ) return false;

      double fold = Value();
      Shift();
      double fnew = Value();

      if( fnew < fold ) return true;

      BackUp();

      step *= stepFactor; 

    } while( true );

  }

  
}
