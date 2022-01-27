/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFZD.h,v 1.3 2008-02-15 14:31:34 seuster Exp $


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


#ifndef OJFZD_H
#define OJFZD_H

// for ATLAS random engine
#include "CLHEP/Random/RandFlat.h"

namespace OptimalJetFinder {


  class Direction;
  class Position;
  class ProblemOfZD;



  // this class represents a position in the simplex z(0,1,2,...,dim)

  class Position {
    Position( const Position& ); // not used
    Position& operator=( const Position& ); // not used 
  public:
    // a constructor; requires the dimension of z(0,1,...,dim) 
    Position( unsigned Dim );

    // a copy-constructor (copies the value of the position from the argument
    Position( const Position* );
     
    ~Position();
    
    // copies the value of z(0,1,2,...,dim) from another Position
    // pointed to by the argument 
    void Copy ( const Position* ); 

    // returns the i-th component of z(0,1,2,...,dim)
    double At( unsigned i ) const;

    // returns the dimension of z(0,1,2,...,dim)
    unsigned GetDim() const { return dim; };

    // sets z(0,1,...,dim) to a random position in the simplex
    void SetRandom();

    // sets z(0,1,...,dim) to a given value and then
    // enforces the position to belong to the simplex
    void SetTo( const double *z0, unsigned len_z0 );
                                                
    // shifts the position to a new point in the simplex
    // based on the position and direction contained in d
    void SetToShifted( const Direction* d, double step );
                                                          
                                                          
  private:
    static const double eps_snap;
    static const double eps_sum;
    static const double eps_sum0;
    static const double eps_sum1; 
    static const double eps_round;
    double* const z;
    const unsigned len_z;
    const unsigned dim;

    // verifies that the shift in the direction d(0,1,...,dim) will not
    // move the point out of the simplex (for appropriately small step)
    void Validate() const;
                 
    // a point of the simplex that is close enough to the simplex boundary
    // is snapped to the boundary, i.e. z(j) < eps_snap => z(j) = 0
    void Snap(); 
                 
    // an arbitrary z(0,1,...,dim) is forced to be a point of the simplex
    void ForceToSimplex();
  };



  // this class represent a direction vector d(0,1,...,dim)
  // in which a point on the simplex should be moved
  // to decrease the value of the function that is being minimized

  class Direction {
    Direction( const Direction& ); // not used
    Direction& operator=( const Direction& ); // not used
  public:
    // a constructor; requires the dimension of d(0,1,...,dim)
    Direction( unsigned Dim );

    ~Direction(); 

    // returns the dimension of d(0,1,...,dim)
    unsigned GetDim() const { return dim; }

    // returns the pointer to the Position
    Position* GetPosition() const { return z; }

    // returns the i-th component of d(0,1,...,dim)
    double At( unsigned i ) const; 

    // returns the maximal value of step that will not lead out of
    // the simplex according to (28) in ref. [1]
    double GetMaxStep() const { return maxStep; } 
    
    // returns the norm of the direction d(0,1,...,dim)
    // according to eq. (27) in [1]
    double GetNorm() const { return norm; }
                                   
    // chooses the best direction to move within the simplex,
    // as described in section 2.2 of ref. [1]
    void SetOptimal( ProblemOfZD* pb );
                                        
  private:
    static const double eps_sum;
    static const double eps_round;
    static const double INF;
    const unsigned dim;
    Position* z;
    double *d, *const f, *New; 
    double maxStep, norm;
    
    // returns the norm of the direction d(0,1,...,dim)
    // that d points to, according to eq. (27) in [1]
    double Norm( const double* d ) const;
                                        
    // verifies that the shift in the direction d(0,1,...,dim) will not
    // move the point out of the simplex (for appropriately small step)
    void Validate() const;
                
    // as described in section 2.2 of [1], out of the possible
    // valid directions the one is chosen that has maximal norm
    void Snap();
                 
    // returns the maximal value of step that will not lead out of
    // the simplex according to (28) in ref. [1]
    double MaxStep() const;
                            
  };

  
  
  // this is an abstract class
  // that represents the minimization problem (one iteration)

  class ProblemOfZD {
    ProblemOfZD( const ProblemOfZD& ); // not used
    ProblemOfZD& operator=( const ProblemOfZD& ); // not used
  public:
    // a constructor; requires the dimension of the simplex
    ProblemOfZD( unsigned Dim );

    virtual ~ProblemOfZD() = 0;

    // returns the dimension of the simplex
    unsigned GetDim() const { return dim; }

    // returns the current step
    double GetStep() const { return step; }

    // returns the pointer to the direction
    Direction* GetDirection() const { return d; }

    // returns true and performs one move
    // that reduces the value of the function being minimized
    // or returns false if already in a local minimum
    // (and no move has been performed)
    bool DescendOnce();
                        
    // returns the value of the function being minimized 
    virtual double Value() const = 0;

    // returns the pointer to the Position
    virtual Position* GetPosition() const = 0;

    // returns the j-th component of the gradient
    // of the function being minimized 
    virtual double Gradient( unsigned j ) const = 0;

    // performs one shift in the simplex    
    virtual void Shift() = 0;

    // restores the previous position in the simplex (before the shift)
    // i.e. backs up the shift 
    virtual void BackUp() = 0;
                               
  private:
    static const double min_dist;
    static const double stepFactor;
    const unsigned dim;
    Direction* const d;
    double step;
  };



  // random number generator that OJF uses
  /* 
     class OJFRandom {
     static const unsigned m;
     static const unsigned b;
     static const unsigned c;
     static const double z;
     static unsigned ojf_random;
     public:
     static double Random();
     static void SetSeed( unsigned seed ) { ojf_random = seed % m; }
     static unsigned GetSeed() { return ojf_random; }
     }; */
  
  class OJFRandom {
  public:
    static double Random();
    static void SetSeed( unsigned /* seed */ ) { }
    static unsigned GetSeed() { return 1; }
  };
  
}

#endif //OJFZD_H
