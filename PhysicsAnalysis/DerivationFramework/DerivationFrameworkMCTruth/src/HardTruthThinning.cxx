/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// HardTruthThinning.cxx
// Author: Frank Paige
// Based on MenuTruthThinning and should perhaps be merged with it.
// Intended for use with CompactHardTruth.
// Preserves graph only for decays of selected particles.
//
// No treatment of Geant particles (yet).
//
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkMCTruth/HardTruthThinning.h"
#include "DerivationFrameworkMCTruth/DecayGraphHelper.h"
#include "AthenaKernel/IThinningSvc.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODEventInfo/EventInfo.h"
//#include "EventKernel/PdtPdg.h"
#include "AthenaKernel/errorcheck.h"
#include "HepPID/ParticleIDMethods.hh"
#include "GaudiKernel/SystemOfUnits.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <vector>
#include <string>

using Gaudi::Units::GeV;

////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////

// Required parameters: 
// EventInfo, TruthParticles, TruthVertices, HardParticles
// Parameters that turn on features:
// JetName      If not empty, save constituents with cuts
// KeepIds      If pdgId list not empty, save particles and decay chains
// IsolRadius   If positive, save stable particles in isolation cones

DerivationFramework::HardTruthThinning::HardTruthThinning(
  const std::string& t, const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_jetPtCut(0.),
  m_jetEtaCut(5.0),
  m_jetConstPtCut(0.),
  m_jetPhotonPtCut(0.),
  m_isolR(-1),
  m_isolPtCut(0),  
  m_maxCount(0)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);

  declareProperty("EventInfo", m_eventInfoName,
                  "EventInfo name");

  declareProperty("TruthParticles", m_truthParticleName,
                  "truth particle container name");

  declareProperty("TruthVertices", m_truthVertexName,
                  "truth vertex container name");

  declareProperty("HardParticles", m_hardParticleName,
                  "hard particle container name");

  declareProperty("JetName", m_jetName,
                  "truth jet container name");

  declareProperty("JetPtCut", m_jetPtCut,
                  "truth jet minumum pt");

  declareProperty("JetEtaCut", m_jetEtaCut,
                  "truth jet maximum abs(eta)");

  declareProperty("JetConstPtCut", m_jetConstPtCut,
                  "jet constituent minimum pt");

  declareProperty("JetPhotonPtCut", m_jetPhotonPtCut,
                  "jet constituent photon minimum pt");

  declareProperty("KeepIds", m_keepIds,
                  "list of abs(pdgID) to keep");

  declareProperty("IsolRadius", m_isolR,
                  "isolation radius for leptons and photons");

  declareProperty("IsolPtCut", m_isolPtCut,
                  "isolation particle minimum pt");

  declareProperty("MaxCount",
                  m_maxCount,
                  "maximum number of events to print");
}


////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////

DerivationFramework::HardTruthThinning::~HardTruthThinning() {
}


//////////////////////////////////////////////////
// Athena initialize and finalize
//////////////////////////////////////////////////

StatusCode DerivationFramework::HardTruthThinning::initialize()
{
  m_evtCount = -1;
  m_errCount = 0;

  // Check for unset photon cut
  if( m_jetPhotonPtCut < m_jetConstPtCut ) m_jetPhotonPtCut =m_jetConstPtCut;

  return StatusCode::SUCCESS;
}


StatusCode DerivationFramework::HardTruthThinning::finalize()
{
    ATH_MSG_INFO("finalize() ...");
    if( m_errCount > 0 ){
      ATH_MSG_WARNING("TruthHard/TruthEvent pdgId mismatches " <<m_errCount);
    } else {
      ATH_MSG_INFO("No TruthHard/TruthEvent pdgId mismatches");
    }

    return StatusCode::SUCCESS;
}


////////////////////////////////////////////////////////////
// doThinning
////////////////////////////////////////////////////////////

StatusCode DerivationFramework::HardTruthThinning::doThinning() const
{

  ++m_evtCount;
  bool doPrint = m_evtCount < m_maxCount;
  //bool doExtra = false;


  // Retrieve Event/EventInfo -- FIXME

  const EventInfo* eventInfo = 0;
  if( !evtStore()->retrieve( eventInfo, m_eventInfoName).isSuccess() ){
    ATH_MSG_ERROR("No EventInfo found with name " <<m_eventInfoName );
    return StatusCode::FAILURE;
  }
  long long int evtNum = eventInfo->event_ID()->event_number();

  // Retrieve truth particles and vertices

  xAOD::TruthParticleContainer::const_iterator pItr;
  xAOD::TruthParticleContainer::const_iterator pItrE;

  const xAOD::TruthParticleContainer* inTruthParts = 0;
  if( evtStore()->retrieve(inTruthParts, m_truthParticleName).isFailure() ){
    ATH_MSG_ERROR("No TruthParticleContainer found with name "
                  <<m_truthParticleName);
    return StatusCode::FAILURE;
  }

  const xAOD::TruthVertexContainer* inTruthVerts = 0;
  if( evtStore()->retrieve(inTruthVerts, m_truthVertexName).isFailure() ){
    ATH_MSG_ERROR("No TruthVertexContainer found with name "
                  <<m_truthVertexName);
    return StatusCode::FAILURE;
  }

  const xAOD::TruthParticleContainer* inHardParts = 0;
  if( evtStore()->retrieve(inHardParts, m_hardParticleName).isFailure() ){
    ATH_MSG_ERROR("No TruthParticleContainer found with name "
                  <<m_hardParticleName);
    return StatusCode::FAILURE;
  }

  // Hard particles are different objects but have matching barcodes.
  // Find hard particles with status==1.
  // Save 4vectors of leptons and photons for matching if requested.
  // Do we need a photon pt cut for soft photons from quarks?

  std::vector<const xAOD::TruthParticle*> hardPart;
  std::vector<TLorentzVector> pLepGam;

  pItr = inHardParts->begin();
  pItrE = inHardParts->end();
  for(; pItr!=pItrE; ++pItr){
    if( (*pItr)->status() == 1 ){
      hardPart.push_back(*pItr);
      if( m_isolR > 0 ){
        int ida = abs( (*pItr)->pdgId() );
        if( ida==11 || ida==13 || ida==15 || ida==22 ){
          pLepGam.push_back( (*pItr)->p4() );
        }
      }
    }
  }


  // Print full input event
  if( doPrint ){
    printxAODTruth(evtNum, inTruthParts);
  }


  // Set up masks for particles/vertices
  // Default is false for all

  std::vector<bool> partMask;
  std::vector<bool> vertMask;
  unsigned int inPartNum = inTruthParts->size();
  unsigned int inVertNum = inTruthVerts->size();
  partMask.assign(inPartNum, false);
  vertMask.assign(inVertNum, false);

  ATH_MSG_DEBUG("Initial particles/vertices = " <<inPartNum <<" " <<inVertNum);


  // Particles from hard event are stable => keep no parent/child vertices.
  // No descendants (except Geant).
  // Particles from keep list need descendants and their parent vertices.
  // There could be overlap, e.g. for taus. 
  // Expect no pdgId mismatches.

  pItr = inTruthParts->begin();
  pItrE = inTruthParts->end();
  std::vector<const xAOD::TruthParticle*> kids;

  for(; pItr!=pItrE; ++pItr){

    // Stable hard particle matches
    int bc = (*pItr)->barcode();
    bool isHard = false;
    for(unsigned int i=0; i<hardPart.size(); ++i){
      if( bc == (hardPart[i])->barcode() ){
        isHard = true;
        if( (*pItr)->pdgId() != (hardPart[i])->pdgId() ){
          ATH_MSG_WARNING("pdgID mismatch, TruthParticle bc/id "
                          <<(*pItr)->barcode() <<" " <<(*pItr)->pdgId()
                          <<"   Hard bc/id "  <<(hardPart[i])->barcode() 
                          <<" " <<(hardPart[i])->pdgId());
          ++m_errCount;
          break;
        }
      }
    }
    if( isHard ){
      if( doPrint ) ATH_MSG_DEBUG("ParticleMask isHard " <<bc);
      partMask[ (*pItr)->index() ] = true;
    }

    // Keep particles
    int ida = abs((*pItr)->pdgId());
    bool isKeep = false;
    for(unsigned int i=0; i<m_keepIds.size(); ++i){
      if( ida == m_keepIds[i] ){
        isKeep = true;
        break;
      }
    }
    if( isKeep ){
      if( doPrint ) ATH_MSG_DEBUG("ParticleMask isKeep " <<bc);
      partMask[(*pItr)->index()] = true;
      const xAOD::TruthParticle* ppItr = (*pItr);
      int nkids =  getDescendants( ppItr, kids );
      for(int i=0; i<nkids; ++i){
        if( doPrint ) ATH_MSG_DEBUG("ParticleMask isKeep kid " 
                                    <<bc <<" " <<kids[i]->barcode());
        partMask[ (kids[i])->index() ] = true;
        const xAOD::TruthVertex* v = (kids[i])->prodVtx();
        if( v ) vertMask[ v->index() ] = true;
      }
    }

    // Delta(R) matches to hard truth leptons/photons
    if( pLepGam.size() > 0 && (*pItr)->pt() > m_isolPtCut ){
      TLorentzVector pp4 = (*pItr)->p4();
      for(unsigned int lep=0; lep<pLepGam.size(); ++lep){
        double r = pp4.DeltaR( pLepGam[lep] );
        if( r < m_isolR ){
          if( doPrint ) ATH_MSG_DEBUG("ParticleMask isol " <<bc);
          partMask[ (*pItr)->index() ] = true;
        }
      }
    }
  }




  // Retrieve optional jets
  // Add particles that are constituents of selected jets using barcodes.
  // Is index() for JetConstituentVector or TruthParticleContainer or??

  if( m_jetName != "" ){

    const xAOD::JetContainer* inJets = 0;
    if( evtStore()->retrieve(inJets, m_jetName).isFailure() ){
      ATH_MSG_ERROR("No JetContainer found with name " <<m_jetName);
      return StatusCode::FAILURE;
    }

    std::vector<int> bcJetConst;

    for(unsigned int j=0; j<inJets->size(); ++j){
      const xAOD::Jet* ajet = (*inJets)[j];
      if( ajet->pt() < m_jetPtCut ) continue;
      if( std::abs(ajet->eta()) > m_jetEtaCut ) continue;

      xAOD::JetConstituentVector aconst = ajet->getConstituents();
      xAOD::JetConstituentVector::iterator aItr = aconst.begin();
      xAOD::JetConstituentVector::iterator aItrE = aconst.end();
      for( ; aItr != aItrE; aItr++){
        const xAOD::JetConstituent* aip = (*aItr);
        const xAOD::IParticle* aipraw = aip->rawConstituent();
        const xAOD::TruthParticle* pp = 
        dynamic_cast<const xAOD::TruthParticle*>(aipraw);
        if( pp ) {
          if( pp->pt()>m_jetConstPtCut && pp->pdgId()!=22 ){
            bcJetConst.push_back( pp->barcode() );
          }
          if( pp->pt()>m_jetPhotonPtCut && pp->pdgId()==22 ){
            bcJetConst.push_back( pp->barcode() );
          }
        } else {
          ATH_MSG_WARNING("Bad cast for particle in jet " <<j);
        }
      }
    }

    pItr = inTruthParts->begin();
    pItrE = inTruthParts->end();
    for(; pItr!=pItrE; ++pItr){
      int bc = (*pItr)->barcode();
      bool isJet = false;
      for(unsigned int i=0; i<bcJetConst.size(); ++i){
        if( bc == bcJetConst[i] ){
          isJet = true;
          break;
        }
      }
      if( isJet ){
        if( doPrint ) ATH_MSG_DEBUG("ParticleMask isJet " <<bc);
        partMask[ (*pItr)->index() ] = true;
      }
    }

  } //end optional jets


  // Execute the thinning service based on the mask. Finish.
  if(m_thinningSvc->filter(*inTruthParts, partMask, IThinningSvc::Operator::Or).isFailure()) {
    ATH_MSG_ERROR("Application of thinning service failed for truth particles! ");
    return StatusCode::FAILURE;
  }

  if(m_thinningSvc->filter(*inTruthVerts, vertMask, IThinningSvc::Operator::Or).isFailure()) {
    ATH_MSG_ERROR("Application of thinning service failed for truth vertices! ");
    return StatusCode::FAILURE;
  }

  // Final statistics
  int outPartNum = 0;
  for(unsigned int i=0; i<partMask.size(); ++i){
    if( partMask[i] ) ++outPartNum;
  }
  int outVertNum = 0;
  for(unsigned int i=0; i<vertMask.size(); ++i){
    if( vertMask[i] ) ++outVertNum;
  }

  ATH_MSG_DEBUG("Final particles/vertices = " <<outPartNum <<" " <<outVertNum);

  if( doPrint ){
    std::cout <<"======================================================================================" <<std::endl;
    std::cout <<"HardTruthThinning complete for event " <<evtNum <<std::endl;
    std::cout <<"Saved " <<outPartNum <<" particles" <<std::endl;
    std::cout <<"Particle barcodes = ";
    for(unsigned int i=0; i<partMask.size(); ++i){
      if( partMask[i] ) std::cout << ((*inTruthParts)[i])->barcode() <<" ";
    }
    std::cout <<std::endl;

    std::cout <<"Saved " <<outVertNum <<" vertices" <<std::endl;
    std::cout <<"Vertex barcodes = ";
    for(unsigned int i=0; i<vertMask.size(); ++i){
      if( vertMask[i] ) std::cout << ((*inTruthVerts)[i])->barcode() <<" ";
    }
    std::cout <<std::endl;
    std::cout <<"======================================================================================" <<std::endl;
  }

  return StatusCode::SUCCESS;

}


////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////

// Emulate HepMC descendant iterator
// Multiple particles can give same descendant (string/cluster)
// Remove Geant descendants
// MUST check ElementLink validity with isValid() for thinned samples

int DerivationFramework::HardTruthThinning::getDescendants(
              const xAOD::TruthParticle* p,
              std::vector<const xAOD::TruthParticle*>& descendants ) const {
  descendants.clear();
  if( ! (p->hasDecayVtx()) ) return 0;
  const xAOD::TruthVertex* dvtx = p->decayVtx();
  if( !dvtx ) return 0;
  if( dvtx->nOutgoingParticles() <= 0 ) return 0;
  if( dvtx->barcode() < -200000 ) return 0;
  const std::vector< ElementLink< xAOD::TruthParticleContainer > >& outPart =
  dvtx->outgoingParticleLinks();
  for(unsigned int k=0; k<outPart.size(); ++k){
    if( ! (outPart[k]).isValid() ) continue;
    const xAOD::TruthParticle* kid = *(outPart[k]);
    descendants.push_back(kid);
  }

  int nstart = 0;
  int nstop = descendants.size();

  while( nstop > nstart ){
    for(int i=nstart; i<nstop; ++i){
      const xAOD::TruthParticle* pp = descendants[i];
      if( ! (pp->hasDecayVtx()) ) continue;
      const xAOD::TruthVertex* vpp = pp->decayVtx();
      if( !vpp ) continue;
      if( vpp->nOutgoingParticles() <= 0 ) continue;
      if( vpp->barcode() < -200000 ) continue;
      const std::vector< ElementLink< xAOD::TruthParticleContainer > >&
      outPart2 = vpp->outgoingParticleLinks();
      for(unsigned int k=0; k<outPart2.size(); ++k){
        if( ! (outPart2[k]).isValid() ) continue;
        const xAOD::TruthParticle* kpp = *(outPart2[k]);
        if( kpp->barcode() > 200000 ) continue;
        bool isIn = false;
        for(unsigned int kk=0; kk<descendants.size(); ++kk){
          if(kpp==descendants[kk]) isIn = true;
        }
        if( !isIn ) descendants.push_back(kpp);
      }
    }
    nstart = nstop;
    nstop = descendants.size();
  }

  return nstop;
}

// Print xAODTruth Event. The printout is particle oriented, unlike the
// HepMC particle/vertex printout. Geant and pileup particles with
// barcode>100000 are omitted.

void DerivationFramework::HardTruthThinning::printxAODTruth(long long evnum,
                          const xAOD::TruthParticleContainer* truths) const{

  xAOD::TruthParticleContainer::const_iterator tpItr = truths->begin();
  xAOD::TruthParticleContainer::const_iterator tpItrE = truths->end();
  std::vector<int> bcPars;
  std::vector<int> bcKids;

  std::cout <<"======================================================================================" <<std::endl;
  std::cout <<"xAODTruth Event " <<evnum <<std::endl;
  std::cout <<"   Barcode      PDG Id  Status   px(GeV)   py(GeV)   pz(GeV)    E(GeV)   Parent: Decay" <<std::endl;
  std::cout <<"   -----------------------------------------------------------------------------------" <<std::endl;

  for(; tpItr != tpItrE; ++tpItr ) {
    int bc = (*tpItr)->barcode();
    if( bc > 100000 ) continue;
    int id = (*tpItr)->pdgId();
    int stat = (*tpItr)->status();
    float px = (*tpItr)->px()/1000.;
    float py = (*tpItr)->py()/1000.;
    float pz = (*tpItr)->pz()/1000.;
    float e = (*tpItr)->e()/1000.;
    bcPars.clear();
    bcKids.clear();

    if( (*tpItr)->hasProdVtx() ){
      const xAOD::TruthVertex* pvtx = (*tpItr)->prodVtx();
      if( pvtx ){
        const std::vector< ElementLink< xAOD::TruthParticleContainer > >& pars =
        pvtx->incomingParticleLinks();
        for(unsigned int k=0; k<pars.size(); ++k){
          if( ! (pars[k]).isValid() ) continue;
          const xAOD::TruthParticle* par = *(pars[k]);
          bcPars.push_back(par->barcode());
        }
      }
    }
    if( (*tpItr)->hasDecayVtx() ){
      const xAOD::TruthVertex* dvtx = (*tpItr)->decayVtx();
      if( dvtx ){
        const std::vector< ElementLink< xAOD::TruthParticleContainer > >& kids =
        dvtx->outgoingParticleLinks();
        for(unsigned int k=0; k<kids.size(); ++k){
          if( ! (kids[k]).isValid() ) continue;
          const xAOD::TruthParticle* kid = *(kids[k]);
          bcKids.push_back(kid->barcode());
        }
      }
    }

    std::cout <<std::setw(10)<<bc <<std::setw(12)<<id
              <<std::setw(8)<<stat
              <<std::setprecision(2)<<std::fixed
              <<std::setw(10)<<px <<std::setw(10)<<py
              <<std::setw(10)<<pz <<std::setw(10)<<e <<"   ";
    std::cout <<"P: ";
    for(unsigned int k=0; k<bcPars.size(); ++k){
      std::cout <<bcPars[k] <<" ";
    }
    std::cout <<"  D: ";
    for(unsigned int k=0; k<bcKids.size(); ++k){
      std::cout <<bcKids[k] <<" ";
    }
    std::cout <<std::endl;
  }
  std::cout <<"======================================================================================" <<std::endl;
}
