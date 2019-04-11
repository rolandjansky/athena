/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JpsiExample.cxx
// Author: James Catmore (James.Catmore@cern.ch), E. Bouhova-Thacker (e.bouhova@cern.ch)
// This is an example showing the use of the JpsiFinder tool and alg, getting the actual Jpsis
// from StoreGate

#include "JpsiUpsilonTools/JpsiExample.h"

#include "VxVertex/VxTrackAtVertex.h"

#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

//////////////////////////////////////////////////////////////

JpsiExample::JpsiExample(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_muonMass(105.66)
{
  
  // Declare user-defined properties - cuts and vertexing method
  declareProperty("muonMass"        , m_muonMass);
  declareProperty("outputNTupleName", m_userFName);
  declareProperty("JpsiCandidates"  ,m_JpsiCandidatesKey = "JpsiCandidates");

  // Global Counters; for truth statistics
  m_eventCntr = 0;
  m_jpsiCntr = 0;

  m_jpsiMass = 0;
  m_jpsiMassError = 0;
  m_jpsiMassRec = 0;
  m_jpsiMassPullRec = 0;
  m_jpsiMassPullMC = 0;
  m_jpsiChi2 = 0;

  m_trkRefitPx1 = 0;
  m_trkRefitPy1 = 0;
  m_trkRefitPz1 = 0;
  m_trkRefitPx2 = 0;
  m_trkRefitPy2 = 0;
  m_trkRefitPz2 = 0;

  m_vx = 0;
  m_vy = 0;
  m_vz = 0;

  m_trkOrigCharge1 = 0;
  m_trkOrigPx1 = 0;
  m_trkOrigPy1 = 0;
  m_trkOrigPz1 = 0;
  m_trkOrigCharge2 = 0;
  m_trkOrigPx2 = 0;
  m_trkOrigPy2 = 0;
  m_trkOrigPz2 = 0;

  m_outputFile = 0;
  m_auxTree = 0;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiExample::initialize(){


  ATH_MSG_DEBUG("in initialize()");

  // Open n-tuple file
  m_outputFile = new TFile(m_userFName.c_str(),"recreate");
  // Auxiliary n-tuples not covered by the automatic n-tuple makers
  // Decay angles
  initializeBranches(); // 
  m_auxTree = new TTree("AUX","AUX");
  m_auxTree->Branch("jpsiMass", &m_jpsiMass);
  m_auxTree->Branch("jpsiMassError", &m_jpsiMassError);
  m_auxTree->Branch("jpsiMassRec", &m_jpsiMassRec);
  m_auxTree->Branch("jpsiMassPullRec", &m_jpsiMassPullRec);
  m_auxTree->Branch("jpsiMassPullMC", &m_jpsiMassPullMC);
  m_auxTree->Branch("jpsiChi2", &m_jpsiChi2);
  
//  m_auxTree->Branch("trkRefitCharge1",  &m_trkRefitCharge1);
  m_auxTree->Branch("trkRefitPx1",  &m_trkRefitPx1);
  m_auxTree->Branch("trkRefitPy1", &m_trkRefitPy1);
  m_auxTree->Branch("trkRefitPz1", &m_trkRefitPz1);
//  m_auxTree->Branch("trkRefitCharge2",  &m_trkRefitCharge2);
  m_auxTree->Branch("trkRefitPx2",  &m_trkRefitPx2);
  m_auxTree->Branch("trkRefitPy2", &m_trkRefitPy2);
  m_auxTree->Branch("trkRefitPz2", &m_trkRefitPz2);

  m_auxTree->Branch("vx",m_vx);
  m_auxTree->Branch("vy",m_vy);
  m_auxTree->Branch("vz",m_vz);

  m_auxTree->Branch("trkOrigCharge1",  &m_trkOrigCharge1);
  m_auxTree->Branch("trkOrigPx1",  &m_trkOrigPx1);
  m_auxTree->Branch("trkOrigPy1", &m_trkOrigPy1);
  m_auxTree->Branch("trkOrigPz1", &m_trkOrigPz1);
  m_auxTree->Branch("trkOrigCharge2",  &m_trkOrigCharge2);
  m_auxTree->Branch("trkOrigPx2",  &m_trkOrigPx2);
  m_auxTree->Branch("trkOrigPy2", &m_trkOrigPy2);
  m_auxTree->Branch("trkOrigPz2", &m_trkOrigPz2);

//  m_auxTree->Branch("rxyError", &m_rxyError);

  return StatusCode::SUCCESS;
  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiExample::execute() {


  ATH_MSG_DEBUG("in execute()");

  // Read in the Jpsis from StoreGate 
  const xAOD::VertexContainer*    jpsiContainer(0);
  const xAOD::VertexAuxContainer* jpsiAuxContainer(0);
  StatusCode sc  = evtStore()->retrieve(jpsiContainer   , m_JpsiCandidatesKey);
  StatusCode sc2 = evtStore()->retrieve(jpsiAuxContainer, m_JpsiCandidatesKey+"Aux.");
  if (sc.isFailure() || !jpsiContainer || sc2.isFailure() || !jpsiAuxContainer) {
    ATH_MSG_ERROR("No Jpsi Container Found, skipping event");
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Obtained jpsiContainer");
  }
  ATH_MSG_DEBUG("number of jpsi candidates " << jpsiContainer->size());
  m_jpsiCntr += jpsiContainer->size(); // Count the Jpsis


  // Set the auxilliary branches of the ntuple to zero
  // to avoid accumulation 
  clearBranches();

  ++m_eventCntr;     // Increment event counter

  // Extract information from the Jpsi candidates 
  for ( xAOD::VertexContainer::const_iterator vxcItr = jpsiContainer->begin() ; vxcItr != jpsiContainer->end() ; vxcItr++ ) {
    const xAOD::Vertex* jpsiCandidate = (*vxcItr);
    //ATH_MSG_INFO("Cov. matrix " << jpsiCandidate->covariance().size());

    // refitted track parameters
    TLorentzVector refTrk1 = track4Momentum(jpsiCandidate, 0, m_muonMass);
    m_trkRefitPx1->push_back(refTrk1.Px());
    m_trkRefitPy1->push_back(refTrk1.Py());
    m_trkRefitPz1->push_back(refTrk1.Pz());

    TLorentzVector refTrk2 = track4Momentum(jpsiCandidate, 1, m_muonMass);
    m_trkRefitPx2->push_back(refTrk2.Px());
    m_trkRefitPy2->push_back(refTrk2.Py());
    m_trkRefitPz2->push_back(refTrk2.Pz());

    //vertex position
    m_vx->push_back(jpsiCandidate->x());
    m_vy->push_back(jpsiCandidate->y());
    m_vz->push_back(jpsiCandidate->z());

    TLorentzVector origTrk1 = origTrack4Momentum(jpsiCandidate, 0, m_muonMass);
    m_trkOrigCharge1->push_back(trackCharge(jpsiCandidate, 0));
    m_trkOrigPx1->push_back(origTrk1.Px());
    m_trkOrigPy1->push_back(origTrk1.Py());
    m_trkOrigPz1->push_back(origTrk1.Pz());

    TLorentzVector origTrk2 = origTrack4Momentum(jpsiCandidate, 1, m_muonMass);
    m_trkOrigCharge2->push_back(trackCharge(jpsiCandidate, 1));
    m_trkOrigPx2->push_back(origTrk2.Px());
    m_trkOrigPy2->push_back(origTrk2.Py());
    m_trkOrigPz2->push_back(origTrk2.Pz());
    
    // Calculate the invariant masses and their errors
    double orig_mass = (origTrk1+origTrk2).M();                      // mass from original tracks
    double mass      = (refTrk1+refTrk2).M();                        // mass from refitted tracks
    double error     = invariantMassError(jpsiCandidate, std::vector<double>(2, m_muonMass));  // invariant mass error

    m_jpsiMass->push_back(mass);
    m_jpsiMassError->push_back(error);
    m_jpsiChi2->push_back(jpsiCandidate->chiSquared());
    m_jpsiMassRec->push_back(orig_mass);
    m_jpsiMassPullRec->push_back((mass-orig_mass)/error);
    m_jpsiMassPullMC->push_back((mass-3096.88)/error);
    
  }
     
  // Write all data to the nTuple
  m_auxTree->Fill();

  // END OF ANALYSIS
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode JpsiExample::finalize() {

  ATH_MSG_DEBUG("in finalize()");

  std::cout << "===================" << std::endl;
  std::cout << "SUMMARY OF ANALYSIS" << std::endl;
  std::cout << "===================" << std::endl;
  std::cout << " " << std::endl;
  std::cout << "Total number of events analysed: " << m_eventCntr << std::endl;
  std::cout << "Total number of jpsi candidates: " << m_jpsiCntr << std::endl;
  // Save auxiliary n-tuple
  m_outputFile->cd();
  m_auxTree->Write();
  m_auxTree->Print(); 
  // Close file
  m_outputFile->Close();
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void JpsiExample::clearBranches() {

  m_jpsiMass->clear();
  m_jpsiMassError->clear();
  m_jpsiMassRec->clear();
  m_jpsiMassPullRec->clear();
  m_jpsiMassPullMC->clear();
  m_jpsiChi2->clear();

//  m_trkRefitCharge1->clear();
  m_trkRefitPx1->clear();
  m_trkRefitPy1->clear();
  m_trkRefitPz1->clear();
//  m_trkRefitCharge2->clear();
  m_trkRefitPx2->clear();
  m_trkRefitPy2->clear();
  m_trkRefitPz2->clear();

  m_vx->clear();
  m_vy->clear();
  m_vz->clear();

  m_trkOrigCharge1->clear();
  m_trkOrigPx1->clear();
  m_trkOrigPy1->clear();
  m_trkOrigPz1->clear();
  m_trkOrigCharge2->clear();
  m_trkOrigPx2->clear();
  m_trkOrigPy2->clear();
  m_trkOrigPz2->clear();

//  m_rxyError->clear();

  return;

}

// ************************************************************************************************
// -------------------------------------------------------------------------------------------------
// initializeBranches - initializes branches of auxilliary ntuple
// -------------------------------------------------------------------------------------------------

void JpsiExample::initializeBranches(void) {

  m_jpsiMass = new std::vector<double>;
  m_jpsiMassError = new std::vector<double>;
  m_jpsiMassRec = new std::vector<double>;
  m_jpsiMassPullRec = new std::vector<double>;
  m_jpsiMassPullMC = new std::vector<double>;
  m_jpsiChi2 = new std::vector<double>;
  
//  m_trkRefitCharge1 = new std::vector<double>;
  m_trkRefitPx1 = new std::vector<double>;
  m_trkRefitPy1 = new std::vector<double>;
  m_trkRefitPz1 = new std::vector<double>;
//  m_trkRefitCharge2 = new std::vector<double>;
  m_trkRefitPx2 = new std::vector<double>;
  m_trkRefitPy2 = new std::vector<double>;
  m_trkRefitPz2 = new std::vector<double>;
  
  m_vx = new std::vector<double>;
  m_vy = new std::vector<double>;
  m_vz = new std::vector<double>;

  m_trkOrigCharge1 = new std::vector<double>;
  m_trkOrigPx1 = new std::vector<double>;
  m_trkOrigPy1 = new std::vector<double>;
  m_trkOrigPz1 = new std::vector<double>;
  m_trkOrigCharge2 = new std::vector<double>;
  m_trkOrigPx2 = new std::vector<double>;
  m_trkOrigPy2 = new std::vector<double>;
  m_trkOrigPz2 = new std::vector<double>;

//  m_rxyError = new std::vector<double>;
  
  return;

}

// *********************************************************************************
// ---------------------------------------------------------------------------------
// trackMomentum: returns refitted track momentum
// ---------------------------------------------------------------------------------

TVector3 JpsiExample::trackMomentum(const xAOD::Vertex * vxCandidate, uint trkIndex) const
{
  float px = 0., py = 0., pz = 0.;
//  assert(vxCandidate!=0);
//  assert(i<vxCandidate->vxTrackAtVertex().size());
  
//  if (0 != vxCandidate) {
//    const Trk::TrackParameters* aPerigee = vxCandidate->vxTrackAtVertex()[trkIndex].perigeeAtVertex();
//    px = aPerigee->momentum()[Trk::px];
//    py = aPerigee->momentum()[Trk::py];
//    pz = aPerigee->momentum()[Trk::pz];
//  }
  
  static SG::AuxElement::Accessor< std::vector<float> > refTrackPxAcc("RefTrackPx");
  static SG::AuxElement::Accessor< std::vector<float> > refTrackPyAcc("RefTrackPy");
  static SG::AuxElement::Accessor< std::vector<float> > refTrackPzAcc("RefTrackPz");
  const std::vector<float>& refTrackPx = refTrackPxAcc(*vxCandidate);
  const std::vector<float>& refTrackPy = refTrackPyAcc(*vxCandidate);
  const std::vector<float>& refTrackPz = refTrackPzAcc(*vxCandidate);

  if(trkIndex < refTrackPx.size() && refTrackPx.size() == refTrackPy.size() && refTrackPz.size()) {
    px = refTrackPx[trkIndex];
    py = refTrackPy[trkIndex];
    pz = refTrackPz[trkIndex];
  }
  
  TVector3 mom(px,py,pz);
  return mom;
}

// ---------------------------------------------------------------------------------
// track4Momentum: returns refitted track 4-momentum given mass hypothesis
// ---------------------------------------------------------------------------------

TLorentzVector JpsiExample::track4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const
{
  TVector3 mom = trackMomentum(vxCandidate, trkIndex);
  TLorentzVector lorentz;
  lorentz.SetVectM(mom, mass);
  return lorentz;
}


// ---------------------------------------------------------------------------------
// origTrackMomentum: returns original track momentum
// ---------------------------------------------------------------------------------

TVector3 JpsiExample::origTrackMomentum(const xAOD::Vertex * vxCandidate, int trkIndex) const
{
  TVector3 mom;
  const xAOD::TrackParticle* origTrack = vxCandidate->trackParticle(trkIndex);
  if(origTrack==NULL) {
    return mom;
  }
  
  mom.SetPtEtaPhi(origTrack->pt(), origTrack->eta(), origTrack->phi());
  return mom;
}

// ---------------------------------------------------------------------------------
// origTrack4Momentum: returns original track 4-momentum given mass hypothesis
// ---------------------------------------------------------------------------------

TLorentzVector JpsiExample::origTrack4Momentum(const xAOD::Vertex * vxCandidate, int trkIndex, double mass) const
{
  TVector3 mom = origTrackMomentum(vxCandidate, trkIndex);
  TLorentzVector lorentz;
  lorentz.SetVectM(mom, mass);
  return lorentz;
}

// ---------------------------------------------------------------------------------
// invariantMassError: returns invariant mass error
// ---------------------------------------------------------------------------------

double JpsiExample::invariantMassError(const xAOD::Vertex* vxCandidate, std::vector<double> masses) const
{
  uint NTrk = vxCandidate->vxTrackAtVertex().size();
  if (masses.size() != NTrk) {
    ATH_MSG_WARNING("The provided number of masses does not match the number of tracks in the vertex");
    return -999999.;
  }
  
  uint ndim    = vxCandidate->covariance().size();
  uint ndimExp = (3*NTrk+3)*(3*NTrk+3+1)/2;
  if (ndim == ndimExp) {
    return massErrorVKalVrt(vxCandidate,masses);
  } else {
    ATH_MSG_WARNING("Unknown covariance matrix dimension: " << ndim << ", expected: " << ndimExp);
    return -999999.;
  }
}

// ---------------------------------------------------------------------------------
// massErrorVKalVrt: returns invariant mass error for vertex created by VKalVrtFitter
// ---------------------------------------------------------------------------------

double JpsiExample::massErrorVKalVrt(const xAOD::Vertex * vxCandidate, std::vector<double> masses) const
{
  unsigned int NTrk = vxCandidate->vxTrackAtVertex().size();
  
  // construct the full covariance matrix    
  Amg::MatrixX* fullCov = convertVKalCovMatrix(NTrk, vxCandidate->covariance());
  
  std::vector<CLHEP::HepLorentzVector> particleMom(NTrk);
  std::vector<CLHEP::HepMatrix> particleDeriv(NTrk);
  CLHEP::HepLorentzVector totalMom;
  CLHEP::HepMatrix tmpDeriv(3,3);

  double phi=0.,theta=0.,invP=0.;
  for( unsigned int it=0; it<NTrk; it++){
    const Trk::TrackParameters*  bPer = vxCandidate->vxTrackAtVertex()[it].perigeeAtVertex();
    phi   =  bPer->parameters()[Trk::phi];
    theta =  bPer->parameters()[Trk::theta];
    invP  =  bPer->parameters()[Trk::qOverP];
    CLHEP::HepLorentzVector tmp( cos(phi)*sin(theta)/fabs(invP),
                          sin(phi)*sin(theta)/fabs(invP),
                                   cos(theta)/fabs(invP) );
    double esq = tmp.px()*tmp.px() + tmp.py()*tmp.py() + tmp.pz()*tmp.pz() + masses[it]*masses[it];
    double e = (esq>0.) ? sqrt(esq) : 0.;
    tmp.setE(e);
    particleMom[it] = tmp;
    totalMom += tmp;

//  d(Px,Py,Pz)/d(Phi,Theta,InvP)
    tmpDeriv(1,1) = - tmp.py();
    tmpDeriv(2,1) =   tmp.px();
    tmpDeriv(3,1) =   0.;
    tmpDeriv(1,2) =   cos(phi) * tmp.pz();
    tmpDeriv(2,2) =   sin(phi) * tmp.pz();
    tmpDeriv(3,2) = - sin(theta)/fabs(invP);
    tmpDeriv(1,3) = - tmp.px()/invP;
    tmpDeriv(2,3) = - tmp.py()/invP;
    tmpDeriv(3,3) = - tmp.pz()/invP;
    particleDeriv[it] = tmpDeriv;
  }

  double dMdPx=0., dMdPy=0., dMdPz=0., dMdPhi=0., dMdTheta=0., dMdInvP=0.;
  std::vector<double> Deriv(3*NTrk+3, 0.);
  for(unsigned int it=0; it<NTrk; it++){
    dMdPx = ( totalMom.e() * particleMom[it].px()/particleMom[it].e() - totalMom.px() ) / totalMom.m();
    dMdPy = ( totalMom.e() * particleMom[it].py()/particleMom[it].e() - totalMom.py() ) / totalMom.m();
    dMdPz = ( totalMom.e() * particleMom[it].pz()/particleMom[it].e() - totalMom.pz() ) / totalMom.m();

    dMdPhi   = dMdPx*particleDeriv[it](1,1) + dMdPy*particleDeriv[it](2,1) + dMdPz*particleDeriv[it](3,1);
    dMdTheta = dMdPx*particleDeriv[it](1,2) + dMdPy*particleDeriv[it](2,2) + dMdPz*particleDeriv[it](3,2);
    dMdInvP  = dMdPx*particleDeriv[it](1,3) + dMdPy*particleDeriv[it](2,3) + dMdPz*particleDeriv[it](3,3);

    Deriv[3*it + 3 + 0] = dMdPhi;    Deriv[3*it + 3 + 1] = dMdTheta; Deriv[3*it + 3 + 2] = dMdInvP;
  }

  double err = 0;
  for(unsigned int i=0; i<3*NTrk+3; i++){
    for(unsigned int j=0; j<3*NTrk+3; j++){
      err += Deriv[i]*( (*fullCov)(i,j))*Deriv[j];
    }
  }
  
  // cleanup
  delete fullCov;
  
  return (err>0.) ? sqrt(err) : 0.;
}

// ---------------------------------------------------------------------------------
// trackCharge: returns charge of the i-th track
// ---------------------------------------------------------------------------------

double JpsiExample::trackCharge(const xAOD::Vertex * vxCandidate, int i) const
{
  if(vxCandidate!=NULL && vxCandidate->trackParticle(i)!=NULL) {
    return vxCandidate->trackParticle(i)->charge();
  }
  
  return -999999;
}

// ---------------------------------------------------------------------------------
// convertVKalCovMatrix: converts xAOD covariance matrix (i.e. std::vector<float>)
//                       into an actual matrix
// ---------------------------------------------------------------------------------

Amg::MatrixX * JpsiExample::convertVKalCovMatrix(int NTrk, const std::vector<float> & Matrix) const
{
  // stolen from VKalVrtFitter    
  Amg::MatrixX * mtx = new Amg::MatrixX(3+3*NTrk,3+3*NTrk);
  long int ij=0;
  for(int i=1; i<=(3+3*NTrk); i++){
    for(int j=1; j<=i; j++){
      if(i==j) { 
        (*mtx)(i-1,j-1)=Matrix[ij];
      }else{ 
        (*mtx).fillSymmetric(i-1,j-1,Matrix[ij]);
      }
      ij++;
    }
  }
  
  return mtx;
}
