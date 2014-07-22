/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OJFJets.h,v 1.1.1.1 2007-05-10 09:02:21 seuster Exp $


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

// See the comments in the source code OJFJets.cpp for more information.


#ifndef OJFJETS_H
#define OJFJETS_H

#include "OjfJet/OJFZD.h"
#include "OjfJet/OJFKinematics.h"


namespace OptimalJetFinder {


  class Event;
  class Jet;
  class Jets;
  class Particle;
  class ProblemOfJets;


  
  class Event {
    Event( const Event& );
    Event& operator=( const Event& );
  public:
    Event( Kinematics kk );
    ~Event();
    Kinematics GetKinematics() const { return k; }
    Particle* GetFirst() const { return first; }
    bool IsNormalized() const { return normalized; } 
    double GetXEnergy() const { return xenergy; }
    void AddParticle( double E, double theta, double phi );
    // RS make return parameter int to store the label in map
    int AddParticleRaw( double px, double py, double pz );
    void Normalize();
    unsigned GetNumber() const;
    void Clear();
  private:
    const Kinematics k;
    Particle* first;
    Particle* last;
    bool normalized;
    double xenergy; // energy or transverse energy, depending on kinematics
    // RS make return parameter int to store the label in map
    int AddParticle0();
  };


  
  class Jet {
    Jet( const Jet& );
    Jet& operator=( const Jet& );
  public:
    Jet( unsigned Label, Jets* QQ, Kinematics kk );
    ~Jet();
    double GetE() const;       // energy ( 0-component of 4-momentum )
    double GetPx() const;      // x-component of 4-momentum  
    double GetPy() const;      // y-component of 4-momentum
    double GetPz() const;      // z-component of 4-momentum
    double GetXEnergy() const; // energy or transverse energy
                               // depending on the kinematics used
    double GetXEta() const;    // theta (degrees) or eta
                               // depending on the kinematics used
    double GetPhi() const;     // phi (degrees)
    JetOfKinematics* Get_j() const { return j; }
    unsigned GetLabel() const { return label; }
    Jets* GetJets() const { return Q; }
    Jet* GetNext() const { return next; }
    void SetNext( Jet* Next ) { next = Next; }
    void UpdateFor( Particle* a, Position* old );
    void BackUp();
    void ReevalLinear();
  private:
    Jet* next;
    Jets* const Q;
    JetOfKinematics* j;
    JetOfKinematics* jold;
    const unsigned label;
  };



  class Jets {
    Jets( const Jets& );
    Jets& operator=( const Jets& );
  public:
    Jets( unsigned Njets, const Event* PP, double RR );
    ~Jets();
    const Event* GetEvent() const { return P; };
    double GetR() const { return R; };
    unsigned GetNumber() const { return njets; };
    Jet* GetFirst() const { return first; };
    unsigned GetMovesAttempted() const { return movesAttempted; };
    unsigned GetMovesFailed() const { return movesFailed; };
    double GetESoft() const { return Esoft; };
    void Reeval();
    void Randomize();
    Jet* GetJet( unsigned n ) const;
    double GetY() const;
    double GetOmega() const;
    bool MinimizeWrt( Particle* a );
    bool MinimizeEachOnce();
    bool Minimize( unsigned maxIter );
    void UpdateEsoftFor ( Particle* a, Position* old );
    void BackUpEsoft();
    double DerivzOmega ( Particle* a, unsigned j ) const;
  private:
    static const double eps_radius;
    const Event* const P;
    const double R;
    const unsigned njets;
    Jet* first;
    unsigned movesAttempted, movesFailed;
    ProblemOfJets* const pb;
    double Esoft, EsoftOld;
    void EvalEsoft();
  };


  
  class Particle {
    Particle( const Particle& );
    Particle& operator=( const Particle& );
  public:
    Particle( unsigned Label, Kinematics kk, const Event* P );
    ~Particle();
    double GetE() const;
    double GetPx() const;
    double GetPy() const;
    double GetPz() const;
    double GetXEnergy() const;
    double GetXEta() const;
    double GetPhi() const;
    double GetESoft() const;
    double GetFractionInJet( unsigned j ) const;
    const Event* GetEvent() const { return P; }
    ParticleOfKinematics* Get_a() const { return a; }
    Position* GetPosition() const { return z; }
    void SetPosition( Position* pos ) { z = pos; } 
    unsigned GetLabel() const { return label; }
    double GetXEnergyNormalized() const { return xenergy; }
    Particle* GetNext() const { return next; }
    void SetNext( Particle* Next ) { next = Next; }
    void SetXEnergy( double Xenergy ) { xenergy = Xenergy; }
  private:
    Particle* next;
    const unsigned label;
    ParticleOfKinematics* a;
    Position* z;
    double xenergy; //energy or transverse energy, depending on the kinematics
    const Event* const P;
  };


  
  class ProblemOfJets : public ProblemOfZD {
    ProblemOfJets( const ProblemOfJets& );
    ProblemOfJets& operator=( const ProblemOfJets& );
  public:
    ProblemOfJets( unsigned njets, Jets* QQ );
    ~ProblemOfJets();
    Position* GetPosition() const { return a->GetPosition(); }
    void SetPosition( Position* pos ) { a->SetPosition( pos ); }
    void SetParticle( Particle* aa ) { a = aa; }
    double Value() const;
    void Shift();
    void BackUp();
    double Gradient( unsigned j ) const;
  private:
    Jets* const Q;
    Position* cache;
    Particle* a;  // the particle we deal with
  };

  
}

#endif //OJFJETS_H
