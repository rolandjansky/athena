/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFKinematics.h,v 1.1.1.1 2007-05-10 09:02:21 seuster Exp $


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


// This module deals with the kinematic properties 
// of single jets and particles.

// References:
// [1] D. Yu. Grigoriev, E. Jankowski, F. V. Tkachov,
//     Comput. Phys. Commun. 155, 42 (2003) [hep-ph/0301226].
// [2] S. Chumakov, E. Jankowski, F. V. Tkachov, 
//     the accompanying Comput. Phys. Commun. paper [paper.pdf].

// See the comments in the source code OJFKinematics.cpp for more information.


#ifndef OJFKINEMATICS_H
#define OJFKINEMATICS_H

#include <cmath>


namespace OptimalJetFinder {


  enum Kinematics { sphere, cylinder };


  const double pi = 3.141592653589793; 
  const double pi180 = pi / 180;

  
  class JetOfKinematics;
  class ParticleOfKinematics;
  class Vector;

  class SParticle;
  class SJet;
  
  class CParticle;
  class CJet;



  class Vector {
    Vector( const Vector& );
    Vector& operator=( const Vector& );
  public:
    Vector() : E( 0 ), x( 0 ), y( 0 ), z( 0 ) {}
    ~Vector() {}
    double GetE() const { return E; };
    double GetX() const { return x; };
    double GetY() const { return y; };
    double GetZ() const { return z; };
    void SetE( double En ) { E = En; };
    void SetX( double X ) { x = X; };
    void SetY( double Y ) { y = Y; };
    void SetZ( double Z ) { z = Z; };
    void SetRaw( double px, double py, double pz );
    void Set( double En, double theta, double phi );
    double GetPhi() const;
    double GetTheta() const;
    double GetEt() const;
    double GetEta() const;
    void NormalizeBy( double c );
    void Nullify() { E = 0; x = 0; y = 0; z = 0; };
    void AddProduct ( double c, const Vector& w );
    void SumProduct ( const Vector& old, double c, const Vector& w );
  private:
    double E, x, y, z;
  };


  
  class JetOfKinematics {
    JetOfKinematics( const JetOfKinematics& );
    JetOfKinematics& operator=( const JetOfKinematics& ); 
  public:
    JetOfKinematics();
    virtual ~JetOfKinematics();
    virtual void Nullify() = 0;
    virtual void AddProduct( double c, ParticleOfKinematics* a ) = 0;
    virtual void SumProduct( JetOfKinematics* jold,
			     double c,
			     ParticleOfKinematics* a ) = 0;
    virtual void EvalQtilde() = 0;
    virtual double GetXEnergy() const = 0; // energy or transverse energy
    virtual double GetXEta() const = 0; // polar angle (degrees) 
                                        // or pseudorapidity
    double GetPhi() const;
    double GetY() const;
    virtual double DerivzY( ParticleOfKinematics* a ) const = 0;
    Vector* Get_q() const { return q; }
    Vector* Get_qtilde() const { return qtilde; }
  private:
    Vector* const q;
    Vector* const qtilde;
  protected:
    static const double eps_norm;
  };

  
  
  class ParticleOfKinematics {
    ParticleOfKinematics( const ParticleOfKinematics& );
    ParticleOfKinematics& operator=( const ParticleOfKinematics& );
  public:
    ParticleOfKinematics();
    virtual ~ParticleOfKinematics();
    Vector* Get_p() const { return p; }
    double GetPhi() const { return phi; };
    virtual void NormalizeBy( double c ) = 0;
    virtual void Set( double E, double theta, double phi ) = 0;
    virtual void SetRaw( double px, double py, double pz ) = 0;
    virtual double GetXEnergy() const = 0;
    virtual double GetXEta () const = 0;
  protected:
    void Set0( double E, double Theta, double Phi );
    void SetRaw0( double px, double py, double pz );
    double GetTheta() const { return theta; };
  private:
    Vector* const p;
    double theta, phi;
  };
  


  class SParticle : public ParticleOfKinematics {
    SParticle( const SParticle& );
    SParticle& operator=( const SParticle& );
  public:
    SParticle() {}
    ~SParticle() {}
    void Set( double E, double theta, double phi );
    void SetRaw( double px, double py, double pz );
    void NormalizeBy( double c );
    double GetXEnergy() const { return Get_p()->GetE(); } 
    double GetXEta() const { return GetTheta(); }
  };
  

  
  class SJet : public JetOfKinematics {
    SJet( const SJet& );
    SJet& operator=( const SJet& );
    void EvalQtilde();
    void Nullify();
    void AddProduct( double c, ParticleOfKinematics* a );
    void SumProduct( JetOfKinematics* jold,
		     double c,
		     ParticleOfKinematics* a );
    double GetXEnergy() const { return Get_q()->GetE(); }
    double GetXEta() const { return std::acos( Get_qtilde()->GetZ() )/pi180; }
    double DerivzY( ParticleOfKinematics* a ) const;
  public:
    SJet() {}
    ~SJet() {}
  };


  
  class CParticle : public ParticleOfKinematics {
    CParticle( const CParticle& );
    CParticle& operator=( const CParticle& ); 
  public:
    CParticle() : Et( 0 ), eta( 0 ), Eteta( 0 ) {}
    ~CParticle() {}
    void Set( double E, double theta, double phi );
    void SetRaw( double px, double py, double pz );
    void NormalizeBy( double c );
    double GetXEnergy() const { return Et; }
    double GetXEta() const { return eta; }
  private:
    void SetC();
    double Et, eta, Eteta;
  };


  
  class CJet : public JetOfKinematics {
    CJet( const CJet& );
    CJet& operator=( const CJet& );
    static const double eps_Et;
    double Et, eta, Eteta;
    void Nullify();
    void AddProduct( double c, ParticleOfKinematics* a );
    void SumProduct( JetOfKinematics* jold,
		     double c,
		     ParticleOfKinematics* a );
    void SetToZero();
    void AddMul( double c, ParticleOfKinematics* a );
    void EvalQtilde();
    double GetXEnergy() const { return Et; }
    double GetXEta() const { return eta; }
    double GetEteta() const { return Eteta; }
    double DerivzY( ParticleOfKinematics* a ) const;
  public:
    CJet() : Et( 0 ), eta( 0 ), Eteta( 0 ) {}
    ~CJet() {}
  };

  

  double PosProduct( const Vector& v, const Vector& w );


}

#endif //OJFKINEMATICS_H
