///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetIsolationTool.h 
// Header file for class JetIsolationTool
// Author: P-A. Delsart
/////////////////////////////////////////////////////////////////// 
#ifndef JETMOMENTTOOLS_JETISOLATIONTOOL_H
#define JETMOMENTTOOLS_JETISOLATIONTOOL_H 1

#include "JetRec/JetModifierBase.h"
#include "JetUtils/TiledEtaPhiMap.h"
#include "AsgTools/CLASS_DEF.h"

/////////////////////////////////////////////////////////////////// 
/// \class JetIsolationTool
///
/// This JetModifier calculates isolation variables for jets.
///
/// The isolation variables are calculated from a list of input particles
/// which are close to the jet but not part of its constituents.
/// There are multiple options to find this list and then to calculate the  
/// variables.
/// They are passed to the tools by giving a list of string identifiers through
/// the IsolationCalculations property as in
///
///  IsolationCalculations = ["IsoKR:11:Perp","IsoKR:11:Par", "IsoFixedCone:10:SumPt",]
///
/// where the strings have the form "ISOCRITERIA:NN:VAR"
///  - ISOCRITERIA : encode the method used to find isolation particles (IsoKR, IsoDelta, IsoFixedCone, IsoFixedArea, Iso6To8)
///  - NN : encode the value of the main parameter of ISOCRITERIA : the actual parameter used is NN/10. (same convetion as jet radius in JetContainer names)
///  - VAR : encode the variable to be calculated (Per, Par, SumPt, P)
///
/// The input particles container from which constituents come from must be specified 
/// in the ConstituentContainer property. This must correspond to an IParticleContainer in the event store.
/// (longer term : make it possible to pass PseudoJetContainer)
///
///
/// There are several technical difficulties
///  - the multiplicity of possible variables
///  - the dependency on the calibration state of the constituents
///  - the calculation time (looking up the full input container for each jet constituents can be *very* slow)
/// These are solved at the cost of increased multiplicity (internal helper classes, usage of a special fast lookup map) 
///
/// WARNING : currently works well only for LCTopoJets, TrackJets, TruthJets AND small R jets (R ~<0.8)
///
///////////////////////////////////////////////////////////////////////

namespace jet {
  namespace JetIsolation {
    class IsolationCalculator;
  }

  /// \class ParticlePosition
  /// Object describing the position of a particle in (eta,phi) and usable within a 
  /// TiledEtaPhiMap (see below).
  struct ParticlePosition {
    ParticlePosition(const xAOD::IParticle* p, int sigstate=-1): m_eta(p->eta()), m_phi(p->phi()) ,m_part(p), m_sigState(sigstate){};
    ParticlePosition(double x=0, double y=0):m_eta(x),m_phi(y),m_part(NULL), m_sigState(-1){}
    double x() const {return m_eta;}
    double y() const {return m_phi;}
    void setX(double x){m_eta=x;}
    void setY(double x){m_phi=x;}

    const xAOD::IParticle* particle()const {return m_part;}
    void setParticle(const xAOD::IParticle* p){m_part=p;}

    struct DR2 {
      double operator()(const ParticlePosition &p1,const ParticlePosition &p2) const {
        return JetTiledMap::utils::DR2(p1.x(),p1.y(), p2.x(), p2.y() );
      }
    };


  protected:
    double m_eta,m_phi;
    const xAOD::IParticle* m_part ;
    int m_sigState; // if any
  };

  /// This map is a geometric helper. It greatly speeds up the retrieval
  ///  of a set of point around a given position in (eta,phi).
  /// It is saved in the event store, so it's not rebuild for every JetContainer
  ///  (could be factorized in a separate AlgTool if needed)
  typedef JetTiledMap::TiledEtaPhiMap<ParticlePosition> ParticleFastMap;

}


class JetIsolationTool
  : public JetModifierBase
{ 
  ASG_TOOL_CLASS0(JetIsolationTool);

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 
  friend class jet::JetIsolation::IsolationCalculator;

  // Copy constructor: 

  /// Constructor with parameters: 
  JetIsolationTool(const std::string &myname);

  /// Destructor: 
  virtual ~JetIsolationTool(); 

  // Athena algtool's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  finalize();

  virtual int modify(xAOD::JetContainer& jets) const;
  virtual int modifyJet(xAOD::Jet& ) const{return 0;};



  /////////////////////////////////////////////////////////////////// 


protected:

  
  const jet::ParticleFastMap *   retrieveInputMap() const ;
  
 private: 


  std::vector<std::string> m_isolationCodes;
  float m_deltaRmax;
  std::string m_inputPseudoJets;

  std::vector<jet::JetIsolation::IsolationCalculator*> m_isoCalculators;


  bool m_rejectNegE;


}; 


CLASS_DEF(jet::ParticleFastMap , 106240765 , 1)

#endif //> !JETMOMENTTOOLS_JETISOLATIONTOOL_H
