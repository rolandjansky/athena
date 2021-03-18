/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EGTransverseMassTool.cxx, (c) ATLAS Detector software
// Author: Giovanni Marchiori (giovanni.marchiori@cern.ch)
//
// The tool computes the transverse mass between the MET and the
// particles belonging to some collection.
// The MET met and phi values can either be stored in SG under 
// keys specified with METPtBranchName and METPhiBranchName, 
// otherwise the met() and phi() of the MET object stored in
// SG under the METContainerName (default = MET_LocHadTopo)
// will be used.
// A MET>METmin cut can be applied.
// For the particles, only those passing the ObjectRequirements
// selection are retained. Their pt and phi can be stored
// in SG under keys specified with ObjectPtBranchName and
// ObjectPhiBranchName, otherwise the Pt() and Phi() methods
// of the IParticles object of the containred stored in SG
// with the key specified by ObjectContainerName will be used
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkEGamma/EGTransverseMassTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETContainer.h"

#include <vector>
#include <string>
#include <cmath>
using std::abs;
using std::sqrt;

#include "TLorentzVector.h"

namespace DerivationFramework {

  EGTransverseMassTool::EGTransverseMassTool(const std::string& t,
      const std::string& n,
      const IInterface* p) : 
    ExpressionParserUser<AthAlgTool>(t,n,p),
    m_expression1("true"),
    m_METmin(-999.),
    m_sgName(""),
    m_mass1Hypothesis(0.0),
    m_container1Name(""),
    m_container2Name("MET_LocHadTopo"),
    m_pt1BranchName(""),
    m_phi1BranchName(""),
    m_pt2BranchName(""),
    m_phi2BranchName("")
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("ObjectRequirements", m_expression1);
    declareProperty("METmin", m_METmin);
    declareProperty("StoreGateEntryName", m_sgName);
    declareProperty("ObjectMassHypothesis", m_mass1Hypothesis);
    declareProperty("ObjectContainerName", m_container1Name);
    declareProperty("METContainerName", m_container2Name);
    declareProperty("ObjectPtBranchName", m_pt1BranchName);
    declareProperty("ObjectPhiBranchName", m_phi1BranchName);
    declareProperty("METPtBranchName", m_pt2BranchName);
    declareProperty("METPhiBranchName", m_phi2BranchName);
  }

  StatusCode EGTransverseMassTool::initialize()
  {
    if (m_sgName=="") {
      ATH_MSG_ERROR("No SG name provided for the output of the transverse mass tool!");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( initializeParser(m_expression1) );
    return StatusCode::SUCCESS;
  }

  StatusCode EGTransverseMassTool::finalize()
  {
    ATH_CHECK(finalizeParser());
    return StatusCode::SUCCESS;
  }

  StatusCode EGTransverseMassTool::addBranches() const
  {
    // check that container we want to write in SG does not yet exist
    if (evtStore()->contains<std::vector<float> >(m_sgName)) {
      ATH_MSG_ERROR("Tool is attempting to write a StoreGate key " << m_sgName << " which already exists. Please use a different key");
      return StatusCode::FAILURE;
    }

    // create the vector which will hold the values invariant masses
    std::vector<float> *masses = new std::vector<float>();

    // register vector in SG so that it will be stored in SG and available for downstream algorithms
    CHECK(evtStore()->record(masses, m_sgName));      

    // compute the invariant mass values
    CHECK(getTransverseMasses(masses));

    return StatusCode::SUCCESS;
  }  

  StatusCode EGTransverseMassTool::getTransverseMasses(std::vector<float>*& masses) const
  {
    // get the relevant branches
    // if not found in SG for particle 1, will use default pt() and phi() 
    // if not found for MET (particle2), will use default met() and phi()
    std::vector<float>* pt1 = 0; 
    if (m_pt1BranchName!="" && evtStore()->contains<std::vector<float> >(m_pt1BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)pt1,m_pt1BranchName));
    else
      ATH_MSG_VERBOSE("Pt variable for particle to be used to compute transverse mass not specified or not found in SG - will use default one");  

    std::vector<float>* phi1 = 0;
    if (m_phi1BranchName!="" && evtStore()->contains<std::vector<float> >(m_phi1BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)phi1,m_phi1BranchName));
    else
      ATH_MSG_VERBOSE("Phi variable for particle to be used to compute transverse mass not specified or not found in SG - will use default one");  

    std::vector<float>* pt2 = 0; 
    if (m_pt2BranchName!="" && evtStore()->contains<std::vector<float> >(m_pt2BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)pt2,m_pt2BranchName));
    else
      ATH_MSG_VERBOSE("Pt variable for MET to be used to compute transverse mass not specified or not found in SG - will use default one");  

    std::vector<float>* phi2 = 0;
    if (m_phi2BranchName!="" && evtStore()->contains<std::vector<float> >(m_phi2BranchName))
      ATH_CHECK(evtStore()->retrieve((const std::vector<float>*&)phi2,m_phi2BranchName));
    else
      ATH_MSG_VERBOSE("Phi variable for MET to be used to compute transverse mass not specified or not found in SG - will use default one");  


    // retrieve particle and MET collections
    const xAOD::IParticleContainer* particles1 = evtStore()->retrieve< const xAOD::IParticleContainer >( m_container1Name );
    if( ! particles1 ) {
      ATH_MSG_ERROR ("Couldn't retrieve IParticles with key: " << m_container1Name );
      return StatusCode::FAILURE;
    }
    const xAOD::MissingETContainer* particles2 = evtStore()->retrieve< const xAOD::MissingETContainer >( m_container2Name );
    if( ! particles2 ) {
      ATH_MSG_ERROR ("Couldn't retrieve MissingET with key: " << m_container2Name );
      return StatusCode::FAILURE;
    }


    // compute MET
    if (particles2->size()==0) {
      if (!pt2){
	ATH_MSG_WARNING("No MET info found");
	return StatusCode::SUCCESS; 
      }
      if (pt2->size()==0) {
	ATH_MSG_WARNING("No MET info found");
	return StatusCode::SUCCESS; 
      }
    }

    float MET = pt2 ? (*pt2)[0] : particles2->at(0)->met();
    float MET_phi = phi2 ? (*phi2)[0] : particles2->at(0)->phi();

    // apply MET requirement
    if (MET<m_METmin)
      return StatusCode::SUCCESS; 


    // get the positions of the particles which pass the requirement
    std::vector<int> entries1 =  m_parser->evaluateAsVector();
    unsigned int nEntries1 = entries1.size();

    // if there are no particles in one of the two lists to combine, just leave function
    if (nEntries1==0)
      return StatusCode::SUCCESS; 

    // check that the sizes are compatible
    if (particles1->size() != nEntries1 ) { 
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    } 
    if ((pt1 && pt1->size() != nEntries1) || 
        (phi1 && phi1->size() != nEntries1)) {
      ATH_MSG_ERROR("Branch sizes incompatible - returning zero");
      return StatusCode::FAILURE;
    } 


    // Loop over the objects and calculate the transverse mass
    unsigned int iter;
    for (iter=0; iter<nEntries1; ++iter) {
      if (entries1[iter]!=1) continue;
      float apt1 = pt1 ? (*pt1)[iter] : ((*particles1)[iter])->p4().Pt(); 
      float aphi1 = phi1 ? (*phi1)[iter] : ((*particles1)[iter])->p4().Phi(); 

      TLorentzVector v1, v2;
      v1.SetPtEtaPhiM(apt1,0.,aphi1,m_mass1Hypothesis);
      v2.SetPtEtaPhiM(MET,0.,MET_phi,0.);
      float mass = (v1+v2).M();
      masses->push_back(mass);	
    }
    return StatusCode::SUCCESS; 
  }
}
