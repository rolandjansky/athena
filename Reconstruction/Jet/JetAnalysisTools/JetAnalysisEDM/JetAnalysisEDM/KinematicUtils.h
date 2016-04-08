// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CodeGenerator_v2_constants.h 510583 2012-07-18 09:12:16Z krasznaa $
#ifndef JETANALYSISEDM_KINEMATICUTILS_H
#define JETANALYSISEDM_KINEMATICUTILS_H

#include "Math/VectorUtil.h"
#include "JetAnalysisEDM/Particle.h"

namespace JetAnalysisEDM {

  /// Some vector kinematic functions.
  ///  Provides a similar interface to Athena's P4Helpers
  namespace P4Helpers {
    namespace VU = ROOT::Math::VectorUtil ;

    inline double deltaEta(const Particle & p1, const Particle & p2) {return p1.eta() - p2.eta() ;}
    inline double deltaPhi(const Particle & p1, const Particle & p2) { return VU::DeltaPhi(p1,p2);}
    inline double deltaR(const Particle & p1, const Particle & p2)   { return VU::DeltaR(p1,p2);}
    inline double deltaR2(const Particle & p1, const Particle & p2)  { return VU::DeltaR2(p1,p2);}
    
  }



  class AtomicSelector ;
  class ParticleSelector {
    /// ParticleSelector allows to filter an input collection into an other one.
    /// System provides a way to easily build  usual selections based on kinematics or Particle attributes.
    ///  It tries to be as efficient as possible by internally using ParticleAccessor and avoiding as much as possible 
    ///  attribute strings look-up.
  public:

    
    ~ParticleSelector();
    /// Fills the 'out' container from 'in' using selectors defined within this.
    void filter(const ParticleContainer &in , ParticleContainer& out, bool preClear=true);
    

    /// adds a standard cut. Cuts are combined with an AND operator.
    ///  for example addAtomicSel( SelectPt< Above > (30*GeV) ); 
    ///   See other standard atomic selector below.
    template<typename T>
    void addAtomicSel(T sel);

    /// adds a custom selector to the filter. 
    ///  T has to implement bool operator()(const Particle&). 
    ///   works event if T does not implement init() nor derive AtomicSelector
    template<typename T>
    void selectCustom(T& sel);

    
  protected:
    //void acquireSel(AtomicSelector*);
    std::vector<AtomicSelector*> m_selectors;
    //std::vector<AtomicSelector*> m_ownedSel;
  };

  /// Base selection structure to be use with a ParticleSelector.
  ///  Defines the required interface
  struct AtomicSelector {
    AtomicSelector(float c1 =0, float c2=0) : m_cut1(c1),m_cut2(c2) {}
    virtual ~AtomicSelector(){}
    virtual bool operator()(const Particle& p){return false;}
    virtual void init(const ParticleContainer & in){}
    float m_cut1, m_cut2;
  };

  


  // *********************************************
  /// Standard atomic Selectors are defined with 2 components :
  ///  - A class implementing the value on which to select (eta, pt, attributes, ...)
  ///  - A class implementing the selection criteria (above, below or between 2 values)


  // This Macro is a shortcut to define an atomic value calculation base from some Particle simple method 
  // calls, typically Kinematic call
#define SELECT_KINEMATIC(kname, calcval)   template<typename CUT>  struct kname : public AtomicSelector  {\
  kname(float c1, float c2=0) : AtomicSelector(c1,c2){} \
  virtual bool operator()(const Particle& p){return CUT::value(calcval,m_cut1,m_cut2);} \
  };

  /// Define standard kinematic cuts..
  SELECT_KINEMATIC( SelectPt, p.pt() );
  SELECT_KINEMATIC( SelectEta, p.eta() );
  SELECT_KINEMATIC( SelectPhi, p.phi() );
  SELECT_KINEMATIC( SelectAbsEta, fabs(p.eta()) );
  SELECT_KINEMATIC( SelectAbsPhi, fabs(p.phi()) );
  SELECT_KINEMATIC( SelectMass, p.M() );
  SELECT_KINEMATIC( SelectE, p.E() );

  /// SelectAttribute defines a standard atomic calculation from a Particle Attribute
  ///  It makes use of a ParticleAccessor to be as fast as possible (avoids string lookups)
  template<typename T, typename CUT >
  struct SelectAttribute : public AtomicSelector {    
    SelectAttribute( const string_t attname, float  c1,float c2=0) : AtomicSelector(c1,c2), m_attname(attname){}
    virtual void init(const ParticleContainer & in){
      accessor = in.accessor<T>(m_attname);
    }
    virtual bool operator()(const Particle& p){  return CUT::value(accessor[p],m_cut1,m_cut2); }
    string_t m_attname;
    ParticleAccessor<T> accessor;
  };


  // ******************************
  /// Criteria selection class to complement the SelectXXX standard classes
  struct Above { inline static bool value(float v, float c1, float ){return v>=c1;} };
  struct Below { inline static bool value(float v, float c1, float ){return v<=c1;} };
  struct InRange { inline static bool value(float v, float c1, float c2){return ((v<=c2) && (v>=c1));} };

}

#include "KinematicUtils.icc"
#endif
