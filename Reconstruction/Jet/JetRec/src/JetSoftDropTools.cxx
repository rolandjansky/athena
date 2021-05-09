/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRec/JetSoftDropTools.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/tools/Filter.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "fastjet/contrib/RecursiveSoftDrop.hh"
#include "fastjet/contrib/BottomUpSoftDrop.hh"
#include "fastjet/Selector.hh"

#include "JetEDM/PseudoJetVector.h"

#include "JetRec/PseudoJetTranslator.h"

using namespace JetGrooming;

namespace {
  // tool implementing the operations to convert fastjet::PseudoJet -> xAOD::Jet
  const static PseudoJetTranslator s_pjTranslator(false, false); // false => do not save jet areas



  /// A common helper to perform the creation and insertion of a groomed jet given a SoftDrop algorithm
  template<typename SDHELPER>
  xAOD::Jet & buildAndInsertSDJet(SDHELPER& sdhelper, const xAOD::Jet& parentjet, const PseudoJetContainer& inpjcont, xAOD::JetContainer& outcont, PseudoJetVector& groomedpjvec ){

    const static SG::AuxElement::Accessor<const fastjet::PseudoJet*> s_pjAcc("PseudoJet");
    const static SG::AuxElement::ConstAccessor<const fastjet::PseudoJet*> s_pjConstAcc("PseudoJet");
    
    // retrieve the PseudoJet from the parent :
    const fastjet::PseudoJet& parentPJ = *s_pjConstAcc(parentjet);

    // call the sdhelper to build the groomed PSeudoJet
    fastjet::PseudoJet groomedPJ = sdhelper(parentPJ);

    // build the xAOD::Jet from the PseudoJet, and put it in the container 
    xAOD::Jet& jet = s_pjTranslator.translate(groomedPJ, inpjcont, outcont, parentjet);
    
    // now assign the groomedPJ to its  vector
    //  Note: the caller (JetGroomer) guarantees groomedpjvec has the correct size,
    //  however, it is not 100% sure (?) that parentjet.index()==jet.index(), so we can't assign before
    groomedpjvec[jet.index()] = groomedPJ;
   
    // decorate with the pointer to the PJ we keep in the evt store.
    //  (the decoration is done in the PseudoJetTranslator but we have to redo-it here since we must point to
    //   the instance instance inside groomedpjvec)    
    s_pjAcc(jet) = & groomedpjvec[jet.index()];
    
    return jet;
  }
}

// a shortcut to check variable range with a 1-liner (We can't use CheckedProperty...)
#define CHECK_VAR_RANGE( name, var, expr )			       \
  if( ! ( expr ) ) {						       \
  ATH_MSG_ERROR("Property "<< name << " = "<< var << " out of range"); \
  return StatusCode::SUCCESS; }

//**********************************************************************

StatusCode SoftDrop::initialize() {
  ATH_CHECK( JetGroomer::initialize() );  

  CHECK_VAR_RANGE("ZCut", m_zcut, (m_zcut>=0)&&(m_zcut<=10.) );
  CHECK_VAR_RANGE("beta", m_beta, (m_beta>=0)&&(m_beta<=10.) );
  CHECK_VAR_RANGE("R0", m_R0, (m_R0>=0)&&(m_R0<=10.) );

  return StatusCode::SUCCESS;
}


//**********************************************************************
StatusCode RecSoftDrop::initialize() {
  ATH_CHECK( SoftDrop::initialize() );  
  CHECK_VAR_RANGE("N", m_N, (m_zcut>-1) );
  return StatusCode::SUCCESS;
}


void SoftDrop::insertGroomedJet(const xAOD::Jet& parentjet, const PseudoJetContainer& inpjcont, xAOD::JetContainer& outcont, PseudoJetVector& groomedpjvec) const {

  //configure soft drop tool
  // see http://fastjet.hepforge.org/svn/contrib/contribs/RecursiveTools/tags/1.0.0/SoftDrop.hh
  fastjet::contrib::SoftDrop softdropper(m_beta, m_zcut, m_R0);

  xAOD::Jet & jet = buildAndInsertSDJet(softdropper,
					parentjet, inpjcont, outcont, groomedpjvec);
  
  if(m_saveSDatt) {
    setSoftDropAttributes(jet, groomedpjvec[jet.index()].pieces().size() );
  }
}

void SoftDrop::setSoftDropAttributes(xAOD::Jet& groomedjet, int nsdsubjets) const {
  groomedjet.setAttribute<float>("ZCut", m_zcut);
  groomedjet.setAttribute<float>("SoftDropBeta", m_beta);
  groomedjet.setAttribute<float>("SoftDropR0", m_R0);
  groomedjet.setAttribute<int>("NSoftDropSubjets", nsdsubjets);
}




//**********************************************************************

void RecSoftDrop::insertGroomedJet(const xAOD::Jet& parentjet, const PseudoJetContainer& inpjcont, xAOD::JetContainer& outcont, PseudoJetVector& groomedpjvec) const {

  //configure recursive soft drop tool
  //https://fastjet.hepforge.org/trac/browser/contrib/contribs/RecursiveTools/tags/2.0.0-beta1
  fastjet::contrib::RecursiveSoftDrop softdropper(m_beta, m_zcut, m_N, m_R0);

  xAOD::Jet & jet = buildAndInsertSDJet(softdropper,
					parentjet, inpjcont, outcont, groomedpjvec);
  
  if(m_saveSDatt) {
    setSoftDropAttributes(jet, groomedpjvec[jet.index()].pieces().size() );
    jet.setAttribute<int>("RSoftDropN", m_N);
  }
}



//**********************************************************************
void BottomUpSoftDrop::insertGroomedJet(const xAOD::Jet& parentjet, const PseudoJetContainer& inpjcont, xAOD::JetContainer& outcont, PseudoJetVector& groomedpjvec) const {

  //configure recursive soft drop tool
  //https://fastjet.hepforge.org/trac/browser/contrib/contribs/RecursiveTools/tags/2.0.0-beta1
  fastjet::contrib::BottomUpSoftDrop softdropper(m_beta, m_zcut, m_R0);

  xAOD::Jet & jet = buildAndInsertSDJet(softdropper,
					parentjet, inpjcont, outcont, groomedpjvec);
  
  if(m_saveSDatt) {
    setSoftDropAttributes(jet, groomedpjvec[jet.index()].pieces().size() );
  }
}

