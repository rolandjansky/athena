/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetIsolationTool.h 
//
// Author: P-A. Delsart
//
// Modified Feb 2015 by D. Adams.
//    It now obtains its inputs from a pseudojet getter instead of a specified
// container of ATLAS objects.

#ifndef JetIsolationTool_H
#define JetIsolationTool_H

/////////////////////////////////////////////////////////////////// 
/// \class JetIsolationTool
///
/// Calculate isolation variables for jets and set them as jet attributes.
///
/// Properties:
///   IsolationCalculations: List of calculations to perform (see below)
///   PseudoJetGetter: Pseudojet getter (ToolHandle<IPseudoJetGetter>)
///
/// The isolation variables are calculated from a list of input constituents.
/// which are close to the jet but not part of its constituents.
/// There are multiple options to find this list and then to calculate the  
/// variables.
/// They are passed to the tools by giving a list of string identifiers through
/// the IsolationCalculations property as in
///
///  IsolationCalculations = ["IsoKR:11:Perp","IsoKR:11:Par", "IsoFixedCone:10:SumPt",]
///
/// where the strings have the form "ISOCRITERIA:NN:VAR"
///  - ISOCRITERIA : encode the method used to find isolation particles (IsoKR, IsoDelta, IsoFixedCone, IsoFixedArea, Iso6To8, signification detailed below)
///  - NN : encode the value of the main parameter of ISOCRITERIA : the actual parameter used is NN/10. (same convetion as jet radius in JetContainer names)
///  - VAR : encode the variable to be calculated (Per, Par, SumPt, P, significations detailed below)
///
/// The resulting attribute name is simply the identifier string with ':' removed. 
/// Example "IsoKR:11:Perp" -> "IsoKR11Perp"
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
/// These are solved at the cost of increased complexity (internal helper classes, usage of a special fast lookup map), details in the .cxx file.
///
/// WARNING : currently works well only for LCTopoJets, TrackJets, TruthJets AND small R jets (R ~<0.8)
///
/// WARNING: The constituents are pseudojets objtained from a pseudojet getter. This should be the
///          same pseudojet getter used in the jet finding. The isolation tool must be run in the same
///          job as the jet finding.
/// 
/// Isolation Criteria ("param" below is the main parameter) :
///  - IsoKR    : iso area == cone of size __jetRadius*param__
///  - IsoDelta : iso area == cone of size __jetRadius+param__
///  - IsoFixedCone : iso are == cone of size __param__
///  - IsoFixedArea : iso are == cone of size __sqrt(jetRadius*jetRadius+param/M_PI)__
///  - Iso6To8 : iso area == annulus between R=0.6 and R=0.8
///
/// Isolation Variables (iso4vec = sum of 4-vec in isolation area, not part of jet)
///  - Perp : iso4Vec.Perp( jetDirection )
///  - Par  : iso4Vec.Dot( jetDirection )
///  - SumPt : sum Pt of 4-vec contibuting to iso4Vec
///  - P : iso4Vec.Vect().Mag()
///////////////////////////////////////////////////////////////////////

#include "AsgTools/AsgTool.h"
#include "JetUtils/TiledEtaPhiMap.h"
#include "JetInterface/IJetDecorator.h"
#include "fastjet/PseudoJet.hh"
#include "JetRec/PseudoJetContainer.h"
#include "JetEDM/IConstituentUserInfo.h"
#include "StoreGate/ReadDecorHandleKey.h"
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/WriteDecorHandleKeyArray.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODJet/JetContainer.h"

namespace jet {

namespace JetIsolation {
  class IsolationCalculator;
}

/// \class ParticlePosition
/// Object describing the position of a particle in (eta,phi) and usable within a 
/// TiledEtaPhiMap (see below).
struct ParticlePosition {
  ParticlePosition(const fastjet::PseudoJet* p)
  : m_eta(p->eta()), m_phi(p->phi()), m_part(p) { };
  ParticlePosition(const xAOD::IParticle* p)
  : m_eta(p->eta()), m_phi(p->phi()), m_part(nullptr) { };
  ParticlePosition(double x=0, double y=0)
  : m_eta(x), m_phi(y), m_part(nullptr) { }
  double x() const {return m_eta;}
  double y() const {return m_phi;}
  double px() const { return m_part->px(); }
  double py() const { return m_part->py(); }
  double pz() const { return m_part->pz(); }
  double e() const { return m_part->e(); }
  double pt() const { return m_part->perp(); }
  void setX(double x){m_eta=x;}
  void setY(double x){m_phi=x;}

  const fastjet::PseudoJet* particle() const {return m_part;}

  struct DR2 {
    double operator()(const ParticlePosition &p1,const ParticlePosition &p2) const {
      return JetTiledMap::utils::DR2(p1.x(),p1.y(), p2.x(), p2.y() );
    }
  };

protected:
  double m_eta,m_phi;
  const fastjet::PseudoJet* m_part ;
};

}  // end namespace jet

//**********************************************************************

class JetIsolationTool : public asg::AsgTool,
                         virtual public IJetDecorator { 
  ASG_TOOL_CLASS0(JetIsolationTool)

public: 

  friend class jet::JetIsolation::IsolationCalculator;

  /// Ctor.
  JetIsolationTool(const std::string &myname);

  /// Dtor.
  virtual ~JetIsolationTool(); 

  // Athena algtool Hooks
  virtual StatusCode  initialize() override;
  virtual StatusCode  finalize() override;

  // Jet Modifier methods.
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

private: 
  Gaudi::Property<std::vector<std::string>> m_isolationCodes{this, "IsolationCalculations", {}, "Isolation calculation data vector"};
  Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};

  SG::ReadHandleKey<PseudoJetContainer> m_pjsin{this, "PseudoJetsIn", "", "PseudoJetContainer to read"};
  SG::ReadDecorHandleKey<xAOD::JetContainer> m_inputTypeKey{this, "InputTypeName", "InputType", "Key for the InputType field of a jet"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_perpKeys{this, "PerpName", {}, "SG key for output perpendicular momentum component decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_sumPtKeys{this, "SumPtName", {}, "SG key for output SumPt decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_parKeys{this, "ParName", {}, "SG key for output parallel momentum component decoration (not to be configured manually!)"};
  SG::WriteDecorHandleKeyArray<xAOD::JetContainer> m_pKeys{this, "PName", {}, "SG key for output momentum decoration (not to be configured manually!)"};

  /// the list of isolation calculation objects (they are actually used
  /// only as template objects from which the actual calculators are build
  // and adapted to the jet object, see implementation)
  std::vector<jet::JetIsolation::IsolationCalculator*> m_isoCalculators;

}; 

#endif
