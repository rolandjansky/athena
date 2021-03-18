/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGInvariantMassTool.cxx, (c) ATLAS Detector software
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkEGamma/EGInvariantMassTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include <vector>
#include <string>

#include <vector>
#include <string>
#include <cmath>
using std::abs;
using std::sqrt;

#include "TLorentzVector.h"


namespace DerivationFramework {

  EGInvariantMassTool::EGInvariantMassTool(const std::string& t,
					   const std::string& n,
					   const IInterface* p) : 
    ExpressionParserUser<AthAlgTool,kNumEGInvariantMassToolParser>(t,n,p),
    m_expression1("true"),
    m_expression2("true"),
    m_sgName(""),
    m_mass1Hypothesis(0.0),
    m_mass2Hypothesis(0.0),
    m_container1Name(""),
    m_container2Name(""),
    m_pt1BranchName(""),
    m_eta1BranchName(""),
    m_phi1BranchName(""), 
    m_pt2BranchName(""),
    m_eta2BranchName(""),
    m_phi2BranchName(""),
    m_checkCharge(true),
    m_mindR(0.0),
    m_doTransverseMass(false)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("Object1Requirements", m_expression1);
    declareProperty("Object2Requirements", m_expression2);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("Mass1Hypothesis", m_mass1Hypothesis);
    declareProperty("Mass2Hypothesis", m_mass2Hypothesis);
    declareProperty("Container1Name", m_container1Name);
    declareProperty("Container2Name", m_container2Name);
    declareProperty("Pt1BranchName", m_pt1BranchName);
    declareProperty("Eta1BranchName", m_eta1BranchName);
    declareProperty("Phi1BranchName", m_phi1BranchName);
    declareProperty("Pt2BranchName", m_pt2BranchName);
    declareProperty("Eta2BranchName", m_eta2BranchName);
    declareProperty("Phi2BranchName", m_phi2BranchName);
    declareProperty("CheckCharge", m_checkCharge);
    declareProperty("MinDeltaR", m_mindR);
    declareProperty("DoTransverseMass", m_doTransverseMass);
  }

  StatusCode EGInvariantMassTool::initialize()
  {
    if (m_sgName.empty()) {
      ATH_MSG_ERROR("No SG name provided for the output of EGInvariantMassTool!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( initializeParser( {m_expression1, m_expression2} ) );
    return StatusCode::SUCCESS;
  }

  StatusCode EGInvariantMassTool::finalize()
  {
    ATH_CHECK( finalizeParser());
    return StatusCode::SUCCESS;
  }

  StatusCode EGInvariantMassTool::addBranches() const
  {
    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<std::vector<float> >(m_sgName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }

    // create the vector which will hold the values invariant masses
    std::unique_ptr<std::vector<float> > masses(new std::vector<float>());

    // compute the invariant mass values
    CHECK(getInvariantMasses(masses.get()));

    // register vector in SG so that it will be stored in SG and available for downstream algorithms
    CHECK(evtStore()->record(std::move(masses), m_sgName));     

    return StatusCode::SUCCESS;
  }  

  StatusCode EGInvariantMassTool::getInvariantMasses(std::vector<float>* masses) const
  {
    // get the relevant branches
    // if not found, will use default pt(), eta() and phi()
    std::vector<float>* pt1 = 0; 
    if (m_pt1BranchName!="" && evtStore()->contains<std::vector<float> >(m_pt1BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)pt1,m_pt1BranchName));
    else
      ATH_MSG_VERBOSE("Pt variable for particle 1 to be used to compute invariant mass not specified or not found in SG - will use default one");  

    std::vector<float>* pt2 = 0; 
    if (m_pt2BranchName!="" && evtStore()->contains<std::vector<float> >(m_pt2BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)pt2,m_pt2BranchName));
    else
      ATH_MSG_VERBOSE("Pt variable for particle 2 to be used to compute invariant mass not specified or not found in SG - will use default one");  


    std::vector<float>* phi1 = 0;
    if (m_phi1BranchName!="" && evtStore()->contains<std::vector<float> >(m_phi1BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)phi1,m_phi1BranchName));
    else
      ATH_MSG_VERBOSE("Phi variable for particle 1 to be used to compute invariant mass not specified or not found in SG - will use default one");  

    std::vector<float>* phi2 = 0;
    if (m_phi2BranchName!="" && evtStore()->contains<std::vector<float> >(m_phi2BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)phi2,m_phi2BranchName));
    else
      ATH_MSG_VERBOSE("Phi variable for particle 2 to be used to compute invariant mass not specified or not found in SG - will use default one");  

    std::vector<float>* eta1 = 0;
    std::vector<float>* eta2 = 0;
    if (!m_doTransverseMass || (m_mindR>0.0)) {
      if (m_eta1BranchName!="" && evtStore()->contains<std::vector<float> >(m_eta1BranchName))
        ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)eta1,m_eta1BranchName));
      else
        ATH_MSG_VERBOSE("Eta variable for particle 1 to be used to compute invariant mass not specified or not found in SG - will use default one");

      if (m_eta2BranchName!="" && evtStore()->contains<std::vector<float> >(m_eta2BranchName))
        ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)eta2,m_eta2BranchName));
      else
        ATH_MSG_VERBOSE("Eta variable for particle 2 to be used to compute invariant mass not specified or not found in SG - will use default one");
    }

    // retrieve particle collections
    const xAOD::IParticleContainer* particles1 = evtStore()->retrieve< const xAOD::IParticleContainer >( m_container1Name );
    if( ! particles1 ) {
      ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_container1Name );
      return StatusCode::FAILURE;
    }
    const xAOD::IParticleContainer* particles2 = evtStore()->retrieve< const xAOD::IParticleContainer >( m_container2Name );
    if( ! particles2 ) {
      ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_container2Name );
      return StatusCode::FAILURE;
    }
 
    // get the positions of the elements which pass the requirement
    std::vector<int> entries1 =  m_parser[kParser1]->evaluateAsVector();
    unsigned int nEntries1 = entries1.size();
    std::vector<int> entries2 =  m_parser[kParser2]->evaluateAsVector();
    unsigned int nEntries2 = entries2.size();

    // if there are no particles in one of the two lists to combine, just leave function
    if (nEntries1==0 || nEntries2==0)
      return StatusCode::SUCCESS; 

    // check the sizes are compatible
    if (particles1->size() != nEntries1 ) { 
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    } 
    if (particles2->size() != nEntries2 ) { 
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    } 
    if ((pt1 && pt1->size() != nEntries1) || 
        ((!m_doTransverseMass || (m_mindR>0.0)) && eta1 && eta1->size() != nEntries1) || 
        (phi1 && phi1->size() != nEntries1)) {
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    } 
    if ((pt2 && pt2->size() != nEntries2) || 
	((!m_doTransverseMass || (m_mindR>0.0)) && eta2 && eta2->size() != nEntries2) || 
	(phi2 && phi2->size() != nEntries2)) {
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    } 

    // Double loop to get the pairs for which the mass should be calculated	
    unsigned int outerIt, innerIt;
    std::vector<std::vector<int> > pairs;
    for (outerIt=0; outerIt<nEntries1; ++outerIt) {
      for (innerIt=0; innerIt<nEntries2; ++innerIt) {
        std::vector<int> tmpPair;
        if (entries1[outerIt]==1 && entries2[innerIt]==1) {
          tmpPair.push_back(outerIt); tmpPair.push_back(innerIt);
          pairs.push_back(tmpPair);
        }
      }
    }

    // since IParticle interface does not provide charge() method
    // we need to identify the particle type in case we want to check
    // its charge (through a type cast)
    xAOD::Type::ObjectType type1 = xAOD::Type::Other;
    xAOD::Type::ObjectType type2 = xAOD::Type::Other;
    if (m_checkCharge) {
      type1 = ((*particles1)[0])->type();
      type2 = ((*particles2)[0])->type();
      if ( (type1 != xAOD::Type::Electron && type1 != xAOD::Type::Muon) ||
	   (type2 != xAOD::Type::Electron && type2 != xAOD::Type::Muon) ) {
	ATH_MSG_WARNING("Cannot check charge for particles not of type electron or muon");
	return StatusCode::FAILURE;
      }
    }

    // Loop over the pairs; calculate the mass; put into vector and return
    std::vector<std::vector<int> >::iterator pairIt;
    for (pairIt=pairs.begin(); pairIt!=pairs.end(); ++pairIt) {
      unsigned int first = (*pairIt)[0];
      unsigned int second = (*pairIt)[1];
      float apt1 = pt1 ? (*pt1)[first] : ((*particles1)[first])->p4().Pt(); 
      float apt2 = pt2 ? (*pt2)[second] : ((*particles2)[second])->p4().Pt();
      float aeta1(-999.), aeta2(-999.);
      if (!m_doTransverseMass || (m_mindR>0.0)) {
	aeta1 = eta1 ? (*eta1)[first] : ((*particles1)[first])->p4().Eta(); 
	aeta2 = eta2 ? (*eta2)[second] : ((*particles2)[second])->p4().Eta();
      }
      float aphi1 = phi1 ? (*phi1)[first] : ((*particles1)[first])->p4().Phi(); 
      float aphi2 = phi2 ? (*phi2)[second] : ((*particles2)[second])->p4().Phi();

      if (m_mindR>0.0) {
	float deta = aeta1-aeta2;
	float dphi = abs(aphi1-aphi2);
	if (dphi > TMath::Pi()) dphi = TMath::TwoPi() - dphi;
	if (sqrt(deta*deta+dphi*dphi)<m_mindR) continue;
      }

      if (m_checkCharge) {
	float q1(0.), q2(0.);
	if (type1 == xAOD::Type::Electron) 
	  q1 = ((xAOD::Electron*) ((*particles1)[first]))->charge();
	else if (type1 == xAOD::Type::Muon) 
	  q1 = ((xAOD::Muon*) ((*particles1)[first]))->primaryTrackParticle()->charge();
	if (type2 == xAOD::Type::Electron)
	  q2 = ((xAOD::Electron*) ((*particles2)[second]))->charge();
	else if (type2 == xAOD::Type::Muon) 
	  q2 = ((xAOD::Muon*) ((*particles2)[second]))->primaryTrackParticle()->charge();
	if (q1*q2>0.) continue;
      }
      TLorentzVector v1, v2, v;
      if (m_doTransverseMass) {
	v1.SetPtEtaPhiM(apt1,0.,aphi1,m_mass1Hypothesis);
	v2.SetPtEtaPhiM(apt2,0.,aphi2,m_mass2Hypothesis);
      }
      else {
	v1.SetPtEtaPhiM(apt1,aeta1,aphi1,m_mass1Hypothesis);
	v2.SetPtEtaPhiM(apt2,aeta2,aphi2,m_mass2Hypothesis);
      }
      float mass = (v1+v2).M();
      masses->push_back(mass);	
    }
    return StatusCode::SUCCESS; 
  }
   
}
