/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetFromPseudojet.cxx

#include "JetRec/JetFromPseudojet.h"
#include <iomanip>
#ifdef USE_BOOST_AUTO
#include <boost/typeof/typeof.hpp>
#endif
#ifdef USE_BOOST_FOREACH
#include <boost/foreach.hpp>
#endif
#include "fastjet/PseudoJet.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/JetConstituentFiller.h"

#include "JetEDM/FastJetLink.h"
#include "xAODJet/Jet_PseudoJet.icc"

using std::setw;
using xAOD::JetContainer;
using fastjet::PseudoJet;
using jet::JetConstituentFiller;

typedef std::vector<PseudoJet> PseudoJetVector;
typedef IJetFromPseudojet::NameList NameList;

//**********************************************************************

JetFromPseudojet::JetFromPseudojet(std::string name)
  :   AsgTool(name), m_doArea(false), m_doFourVecArea(false) {
  declareProperty("Attributes", m_atts);
}

//**********************************************************************

StatusCode JetFromPseudojet::initialize() {
  
#ifdef USE_BOOST_FOREACH
  BOOST_FOREACH(std::string att, m_atts) {
#else
  for (const std::string & att : m_atts) {
#endif
    if      ( att == "ActiveArea" )     m_doArea = true;  
    else if ( att == "ActiveArea4vec" ) m_doFourVecArea = true;
    else if ( att == "ActiveAreaFourVector" ) m_doFourVecArea = true;
    else ATH_MSG_WARNING("Unrecognized attribute name ignored: " << att);
  }
  ATH_MSG_INFO("  Record area: " << m_doArea);
  ATH_MSG_INFO("  Record four-vector area: " << m_doFourVecArea);
  return StatusCode::SUCCESS;

}
  

//**********************************************************************
void JetFromPseudojet::
buildAndSetEMScaleMom(xAOD::Jet* jet,
                      xAOD::JetInput::Type inputtype) const {

  if ( jet->getConstituentsSignalState() == xAOD::UncalibratedJetConstituent ) {
    // If constituents are already uncalibrated, the momentum is the same.
    jet->setJetP4(xAOD::JetEMScaleMomentum, jet->jetP4());
    ATH_MSG_DEBUG("  EM scale momentum set to jet scale");
  } else if ( (inputtype == xAOD::JetInput::EMTopoOrigin) ||
              (inputtype == xAOD::JetInput::LCTopoOrigin) ||
              (inputtype == xAOD::JetInput::LCTopo) ||
              (inputtype == xAOD::JetInput::LCPFlow)|| 
              (inputtype == xAOD::JetInput::EMCPFlow) ) {
    // fetch and sum the uncalibrated constituent momenta
    xAOD::JetConstituentVector vec = jet->getConstituents();
    if(! vec.isValid() ) {
      ATH_MSG_ERROR("Jet constituent vector is invalid. Can't set EM scale momentum");
      return;
    }

    xAOD::JetFourMom_t emscaleSum;
    xAOD::JetConstitScale uncal = xAOD::UncalibratedJetConstituent;
    // just sum 4-vectors:
    ATH_MSG_VERBOSE("  Summing four vectors.");
    for (auto it=vec.begin(uncal); it != vec.end(uncal); ++it) {
      emscaleSum +=**it;
    }
  
    ATH_MSG_VERBOSE("  Setting EM scale momentum");
    jet->setJetP4(xAOD::JetEMScaleMomentum, emscaleSum);        
    ATH_MSG_DEBUG("  EM scale momentum set with uncalibrated constituents.");

  } else {
    ATH_MSG_DEBUG("  EM scale momentum not set.");
  }

}
 
//**********************************************************************

xAOD::Jet*
JetFromPseudojet::add(const PseudoJet& pj, xAOD::JetContainer& jets,
                      xAOD::JetInput::Type inputtype, const NameList& ghostlabs) const {
  const xAOD::Jet* pparent = nullptr;
  xAOD::Jet* pjet = addjet(pj, jets, pparent, &ghostlabs);
  if ( pjet == nullptr ) return pjet;
  // Set the jet's input type.
  ATH_MSG_VERBOSE("Setting input type.");
  pjet->setInputType(inputtype);
  // Set the jet's constituent scale.
  // Calibrated for all but EMTopo.
  ATH_MSG_VERBOSE("Done add with input");
  if ( (inputtype == xAOD::JetInput::EMTopo ) || 
       (inputtype == xAOD::JetInput::EMPFlow ) ) {
    ATH_MSG_VERBOSE("Setting constituent state to uncalibrated state");
    pjet->setConstituentsSignalState(xAOD::UncalibratedJetConstituent);
  } else {
    ATH_MSG_VERBOSE("Setting constituent state to calibrated state");
    pjet->setConstituentsSignalState(xAOD::CalibratedJetConstituent);
  }
  // Set the jet momentum at uncalibrated constituent scale.
  ATH_MSG_VERBOSE("Setting EM scale momentum");
  buildAndSetEMScaleMom(pjet, inputtype );
  ATH_MSG_VERBOSE("Done add with input");
  return pjet;
}

//**********************************************************************

xAOD::Jet*
JetFromPseudojet::add(const PseudoJet& pj, xAOD::JetContainer& jets,
                      const xAOD::Jet* pparent) const {
  return addjet(pj, jets, pparent, 0);
}

//**********************************************************************

xAOD::Jet*
JetFromPseudojet::addjet(const PseudoJet& pj, xAOD::JetContainer& jets,
                         const xAOD::Jet* pparent, const NameList* pghostlabs) const {
  ATH_MSG_VERBOSE("Creating jet from PseudoJet @ " << &pj);
  double  px = pj.px();
  double  py = pj.py();
  double  pz = pj.pz();
  double   e = pj.e();
  double   m = pj.m();
  double  pt = pj.pt();
  double eta = pj.eta();
  double phi = pj.phi_std();
  double p2 = px*px + py*py + pz*pz;
  double p = sqrt(p2);
  double dpovere = p/e - 1.0;
  if ( m < 0.0 ) {
    if ( dpovere > 1.e-6 ) {  // Worse than float rounding error
      ATH_MSG_WARNING("...........................");
      ATH_MSG_WARNING("Found jet with negative mass: E, p, m, p/E-1 = "
                   << e << ", " << p << ", " << m << ", " << p/e-1.0);
      ATH_MSG_WARNING(setw(12) << "px" << setw(12) << "py" << setw(12) << "pz"
                   << setw(12) << "E" << setw(12) << "p");
      ATH_MSG_WARNING(setw(12) << "---" << setw(12) << "---" << setw(12) << "---"
                   << setw(12) << "---" << setw(12) << "---");
      const PseudoJetVector cons = pj.constituents();
      for ( PseudoJetVector::const_iterator icon=cons.begin(); icon!=cons.end(); ++icon ) {
        double cpx = icon->px();
        double cpy = icon->py();
        double cpz = icon->pz();
        double  ce = icon->e();
        double cp2 = cpx*cpx + cpy*cpy + cpz*cpz;
        double cp = sqrt(cp2);
        ATH_MSG_WARNING(setw(12) << int(cpx) << setw(12) << int(cpy) << setw(12) << int(cpz)
                     << setw(12) << int(ce) << setw(12) << int(cp));
      }
      ATH_MSG_WARNING("...........................");
    } else {  // Rounding error
      m = 0.0;
    }
  }
  ATH_MSG_VERBOSE("  Jet has pT = " << pt << " MeV, m = " << m << " MeV, eta = " << eta );
                  //<< ", area = " << area << ", FV area pT = " << fvarea.Pt() << " MeV");
  xAOD::Jet* pjet = new xAOD::Jet();
  jets.push_back(pjet);
  pjet->setJetP4( xAOD::JetFourMom_t(pt,eta,phi,m) );

  if ( pj.associated_cluster_sequence() == 0 ) {
    ATH_MSG_VERBOSE("Pseudojet does not have a cluster sequence and so cannot be copied to Jet.");
  }
  pjet->setPseudoJet(&pj);

  // Record the jet-finding momentum, i.e. the one used to find/groom the jet.
  pjet->setJetP4(xAOD::JetConstitScaleMomentum, pjet->jetP4());
  
  if ( m_doArea || m_doFourVecArea ) {
    if ( pj.has_area() ) {
      if ( m_doArea ) {
        pjet->setAttribute("ActiveArea", pj.area());
        ATH_MSG_VERBOSE("Recording jet area: " << pj.has_area());
      }
      if ( m_doFourVecArea ) {
        fastjet::PseudoJet pja = pj.area_4vector();
        xAOD::JetFourMom_t fvarea(pja.pt(), pja.eta(), pja.phi(), pja.m());      
        pjet->setAttribute("ActiveArea4vec", fvarea);
        ATH_MSG_VERBOSE("Recording jet four-vector area.");
      }
    } else {
      ATH_MSG_WARNING("Save of active area attribute requested for jet without area.");
    }
  } else {
    if ( pj.has_area() ) {
      ATH_MSG_VERBOSE("No area recorded for jet with area.");
    }
  }

  const PseudoJetVector pjcons = pj.constituents();
  ATH_MSG_VERBOSE("  Adding constituents: multiplicity is " << pjcons.size());
  JetConstituentFiller confiller;
  if ( pparent == 0 ) {
    ATH_MSG_VERBOSE("  No parent jet.");
    int nconskip = confiller.extractConstituents(*pjet, pghostlabs, &pj);
    if ( nconskip < 0 ) {
      ATH_MSG_WARNING("  Jet constituent filler returned error " << nconskip);
    }
    ATH_MSG_DEBUG("  Real/total/skipped constituents: " << pjet->numConstituents()
		  << "/" << pjcons.size() << "/" << nconskip);
  } else {
    ATH_MSG_VERBOSE("  Adding parent jet properties");
    const JetContainer* pcon = dynamic_cast<const JetContainer*>(pparent->container());
    if ( pcon == 0 ) {
      ATH_MSG_WARNING("Unable to find parent jet container.");
    } else {
      ATH_MSG_VERBOSE("  Creating parent link.");
      ElementLink<JetContainer> el(*pcon, pparent->index());
      ATH_MSG_VERBOSE("  Setting parent.");
      pjet->setAttribute("Parent", el);
      //pjet->setAssociatedObject("Parent", pparent); 
    }
    ATH_MSG_VERBOSE("  Setting input type from parent.");
    xAOD::JetInput::Type inputtype = pparent->getInputType();
    pjet->setInputType(inputtype);
    ATH_MSG_VERBOSE("  Setting algorithm type from parent.");
    pjet->setAlgorithmType(pparent->getAlgorithmType());
    ATH_MSG_VERBOSE("  Setting size parameter from parent.");
    pjet->setSizeParameter(pparent->getSizeParameter());
    ATH_MSG_VERBOSE("  Setting signal state from parent.");
    pjet->setConstituentsSignalState(pparent->getConstituentsSignalState());
    ATH_MSG_VERBOSE("  Setting ghost area from parent.");
    pjet->setAttribute("JetGhostArea", pparent->getAttribute<float>("JetGhostArea"));

    int nconskip = confiller.extractConstituents(*pjet, pghostlabs, &pj);
    if ( nconskip < 0 ) {
      ATH_MSG_WARNING("  Jet constituent filler returned error " << nconskip);
    }
    ATH_MSG_DEBUG("  Real/total/skipped constituents: " << pjet->numConstituents()
		  << "/" << pjcons.size() << "/" << nconskip);

    ATH_MSG_VERBOSE("  Setting EM scale momentum.");
    buildAndSetEMScaleMom(pjet, inputtype );        
  }
  ATH_MSG_VERBOSE("Done add with parent");
  return pjet;
}

//**********************************************************************

void JetFromPseudojet::print() const {
  ATH_MSG_INFO(" Attribute count: " << m_atts.size());
  for ( std::vector<std::string>::const_iterator inam=m_atts.begin();
        inam!=m_atts.end(); ++inam ) {
    std::string name = *inam;
    ATH_MSG_INFO("  " << name);
  }
}

//**********************************************************************
