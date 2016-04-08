/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSimTools/JetTruthParticleSelectorTool.h"
#include "xAODTruth/TruthParticleContainer.h"

#include <string>

#include <map>

#include <functional>


#include "xAODTruth/TruthVertex.h"

#include "JetUtils/JetDistances.h"

namespace {
  // TEMPORARY recopy some helper from TruthHelper and GeneratorUtils packages. 
  // We'll have to use this package when they work properly with xAOD.

  inline bool isStable(const xAOD::TruthParticle* p) {
    if (p->barcode() >= 200000) return false; // This particle is from G4
    if (p->pdgId() == 21 && p->p4().E() == 0) return false; //< Workaround for a gen bug?
    return ((p->status() % 1000 == 1) || //< Fully stable, even if marked that way by G4
            (p->status() % 1000 == 2 && p->hasDecayVtx() && p->decayVtx() != NULL && p->decayVtx()->barcode() < -200000)); //< Gen-stable with G4 decay
    /// @todo Add a no-descendants-from-G4 check?
  }
  

  bool isInteracting( const xAOD::TruthParticle* const p){
      if (! isStable(p)) return false;
      const int apid = abs(p->pdgId() );
      if (apid == 12 || apid == 14 || apid == 16) return false;
      if (p->status() % 1000 == 1 &&
          (apid == 1000022 || apid == 1000024 || apid == 5100022 ||
           apid == 39 || apid == 1000039 || apid == 5000039)) return false;
      return true;      
    }

  bool isMuon(const xAOD::TruthParticle* truthPart) {
    return ( abs(truthPart->pdgId()) == 13) ;        
  }

  bool isWZDecay(const xAOD::TruthParticle* p) {
    int pdg_id = abs(p->pdgId() );
    int mom_pdg_id = pdg_id;
    const xAOD::TruthVertex* vprod = p->prodVtx();
    const xAOD::TruthVertex*oldVprod = vprod;
    //int mom_barcode = barcode;
    // Ascend decay chain looking for when actual decay occurs (not jsut evolution of particle)
    while (pdg_id == mom_pdg_id) {
      const xAOD::TruthParticle* mother = vprod->incomingParticle(0) ;
      if (mother) {
	mom_pdg_id = abs(mother->pdgId());
	//mom_barcode = mother->barcode();
      } else break;
      if (pdg_id != mom_pdg_id) break;
        
      // Protect against strange infinite reference in sherpa samples
      if(!mother->hasProdVtx()) break;
      if(oldVprod==mother->prodVtx())  break;
      oldVprod = vprod;        
      vprod = mother->prodVtx();
      if (!vprod || vprod->nIncomingParticles() == 0) break;
    } // End while loop

      // The following vertex-based identification of W/Z's is needed for SHERPA
      // samples where the W/Z particle is not explicitly in the particle record.
      // At this point if we have a valid vertex, it should be a true decay vertex.
      // If it is a W or Z then two of those decay products should be lepton/neutrino
    int nDecay=0;
    // Prompt W/Z's should come from a vertex with more than one incoming particle
    // This suppresses fave Z's from conversions
    if (vprod && vprod->nIncomingParticles() >1)
      {
	//std::cout << "Looping over vertex daughters: "<< vprod->particles_out_size() << std::endl;
	for( const auto& elTruth : vprod->outgoingParticleLinks() ){
	  if((abs((*elTruth)->pdgId())>10 && abs((*elTruth)->pdgId())<17) ) nDecay++;
	}
      }

    bool isWZ = (nDecay==2);
    return (mom_pdg_id == 23 || mom_pdg_id == 24 || isWZ);
  }


  // This part is copied and adapted from PhysicsAnalysis/D3PDMaker/TruthD3PDMaker/trunk/src/TruthJetFilterTool.cxx
  bool isWZDecay(const xAOD::TruthParticle* p, std::vector<const xAOD::TruthParticle*>& wzLeptons, double photonCone2) {
    int pdg_id = abs(p->pdgId() );

    if(! p->hasProdVtx() ) return false;
    const xAOD::TruthVertex* vprod = p->prodVtx();
         
    if (vprod && vprod->nIncomingParticles() > 0) {
      if (isWZDecay(p)) { // paricle decends from W or Z
        //  Save lepton reference for comparison to FSR photons (only for muons and electrons)
        if(p->status()==1 && (  (pdg_id==11) || (pdg_id==13) ) ) {
	  wzLeptons.push_back(p);
	  // std::cout << "isWZDecay found pdgId: " << pdg_id << " status: " << p->status() << std::endl;
	  // std::cout << "isWZDecay: have " << wzLeptons.size() << " W/Z leptons" << std::endl;
	}
 
        // Only exclude photons within deltaR of leptons (if m_photonCone<0, exclude all photons)
        if( pdg_id == 22 && photonCone2>0)
          {
            if(p->status()==1 && (abs(p->pdgId())==22) ) {
	      // std::cout << "isWZDecay: found pdgId: " << p->pdgId() << " status: " << p->status() << std::endl;
	      // std::cout << "isWZDecay: have " << wzLeptons.size() << " W/Z leptons" << std::endl;
	    }
            for( auto lep: wzLeptons) {
              double deltaR2 = jet::JetDistances::deltaR2(*p, *lep);
	      // std::cout << "Photon/lepton dR = " << sqrt(deltaR2) << std::endl;
              if( deltaR2 < photonCone2 ) {
		// std::cout << "  isWZDecay: Killed a photon with pt " << p->pt()
		// 	  << " near to a " << lep->pdgId() << " with pt " << lep->pt() << std::endl;
		return true;
	      }
            }
          }
        else // not a photon so exclude
          return true;
      }
    }// if(vprod && vprod>nIncomingParticles>0)
    return false;
  }


  // This part is copied and adapted from PhysicsAnalysis/D3PDMaker/TruthD3PDMaker/trunk/src/TruthJetFilterTool.cxx
  bool isLeptonFromTau(const xAOD::TruthParticle* part){

    int pdg = part->pdgId();

    if(abs(pdg) != 11 &&
       abs(pdg) != 12 &&
       abs(pdg) != 13 &&
       abs(pdg) != 14 &&
       abs(pdg) != 15 &&
       abs(pdg) != 16) return false; // all leptons including tau.
    if(!part->hasProdVtx()) return false;
    const xAOD::TruthVertex* prod = part->prodVtx();
    if(!prod) return false; // no parent.

    
    for( const auto& elParent : prod->incomingParticleLinks() ){
      int parentId = (*elParent)->pdgId();
      if(abs(parentId) == 15 && isWZDecay(*elParent)) {
	// std::cout << "isLeptonFromTau: particle " << pdg << " with pt " << part->pt()
	// 	  << " has tau parent!" << std::endl;
        return true; // Has tau parent
      }

      if(parentId == pdg) { // Same particle just a different MC status
        // Go up the generator record until a tau is found or not.
        // Note that this requires a connected *lepton* chain, while calling
        //  isFromTau would allow leptons from hadrons from taus
        if(isLeptonFromTau(*elParent)) {
          return true;
        }
      }
    }
    return false;
  }
 

}


JetTruthParticleSelectorTool::JetTruthParticleSelectorTool(const std::string& n) :
  asg::AsgTool(n),
  m_min_pt(-1),
  m_max_pt(-1),
  m_maxAbsEta(-1),
  m_minEta(-5),
  m_maxEta( 5),
  m_wzPhotonCone(0.1),
  // m_includeMuons(false),
  // m_useOnlyInteracting(true),
  m_useOnlyCharged(false),
  m_listPDGofStables(false),
  m_selectionModeName("StableNoMuonNoNu"),
  m_selectionMode()
{

  declareInterface<JetTruthParticleSelectorTool>(this);
  declareProperty("min_pT",                    m_min_pt);
  declareProperty("max_pT",                    m_max_pt);
  declareProperty("max_absEta",                m_maxAbsEta);
  declareProperty("minEta",                    m_minEta);
  declareProperty("maxEta",                    m_maxEta);
  declareProperty("WZModePhotonCone", m_wzPhotonCone);

  declareProperty("useChargedParticlesOnly",   m_useOnlyCharged);
  declareProperty("MakeListOfStableParticles", m_listPDGofStables);


  declareProperty("useChargedParticlesOnly",     m_useOnlyCharged);
  declareProperty("SelectionMode",     m_selectionModeName);


}

JetTruthParticleSelectorTool::~JetTruthParticleSelectorTool()
{ }

void JetTruthParticleSelectorTool::setupEvent() const {
  m_wzLeptons.clear();
}
  
StatusCode
JetTruthParticleSelectorTool::initialize()
{


  if(     m_selectionModeName == "StableNoMuonNoNu") m_selectionMode = StableNoMuonNoNu ;
  else if(m_selectionModeName == "MuonOnly") m_selectionMode = MuonOnly ;
  else if(m_selectionModeName == "NuOnly") m_selectionMode = NuOnly ;
  else if(m_selectionModeName == "NoWZDecay") m_selectionMode = NoWZDecay ;
  else {
    ATH_MSG_ERROR(" Unknown truth selection mode "<< m_selectionMode);
    return StatusCode::FAILURE;
  }

  m_pdgList.clear();
  m_avP.clear();
  m_av2P.clear();
  m_avPt.clear();
  m_av2Pt.clear();
  m_avEta.clear();
  m_av2Eta.clear();
  m_avPhi.clear();
  m_av2Phi.clear();
  m_avM.clear();
  m_av2M.clear();
  
  return StatusCode::SUCCESS;
}


bool JetTruthParticleSelectorTool::passKinematics(const xAOD::TruthParticle* truthPart) const{
  auto truth4Mom = truthPart->p4();
  // some safety checks ... both will crash the eta calculation
  if ( truth4Mom.E() <= 0 )                                return false;
  if ( truth4Mom.P() - truth4Mom.Pz() <= 0 )                     return false;
  
  double pt  = truth4Mom.Pt();
  if ( m_min_pt > 0 && pt < m_min_pt )               return false;
  if ( m_max_pt > 0 && pt > m_max_pt )               return false;
  double eta = truth4Mom.Eta();
  if ( m_maxAbsEta > 0 && fabs(eta) > m_maxAbsEta )  return false;
  if ( eta < m_minEta )                              return false;
  if ( eta > m_maxEta )                              return false;
  return true;
}

bool JetTruthParticleSelectorTool::selector(const xAOD::TruthParticle* truthPart) const {

  bool result = false;
  if ( abs(truthPart->pdgId()) == 999) return false; // protect against unexpected geantinos

  switch( m_selectionMode) { // For now only 4 modes used in practice for jets...
                             // a switch statement is probably not optimal here...
  case StableNoMuonNoNu:
    result =( isInteracting(truthPart) && passKinematics(truthPart) && !isMuon(truthPart) );
    break;
  case NuOnly:
    result= ( isStable(truthPart) && passKinematics(truthPart) && !isInteracting(truthPart) );
    break;
  case MuonOnly:
    result = isMuon(truthPart);
    break;
  case NoWZDecay:
    result = ( isStable(truthPart) && passKinematics(truthPart) &&
               !isWZDecay(truthPart, m_wzLeptons, m_wzPhotonCone*m_wzPhotonCone) &&
               !isLeptonFromTau(truthPart) );
    break;
  default:
    break;
  }
  
  if (m_listPDGofStables && result ) {
    int apId = abs(truthPart->pdgId());
    auto truth4Mom = truthPart->p4();
    double p = truth4Mom.P();
    double phi = truth4Mom.Phi();
    double pt  = truth4Mom.Pt();
    double eta  = truth4Mom.Eta();
    PDGList::iterator it = m_pdgList.find(apId);
    if ( it == m_pdgList.end() ) {
      m_pdgList[apId]=1;
      m_avP[apId]=p;
      m_av2P[apId]=p * p;
      m_avPt[apId]=pt;
      m_av2Pt[apId]=pt * pt;
      m_avEta[apId]=eta;
      m_av2Eta[apId]=eta * eta;
      m_avPhi[apId]=phi;
      m_av2Phi[apId]=phi * phi;
      m_avM[apId]=truthPart->m();
      m_av2M[apId]=truthPart->m() * truthPart->m();
    } else {
      it->second++;
      m_avP[apId]+=p;
      m_av2P[apId]+=p * p;
      m_avPt[apId]+=pt;
      m_av2Pt[apId]+=pt * pt;
      m_avEta[apId]+=eta;
      m_av2Eta[apId]+=eta * eta;
      m_avPhi[apId]+=phi;
      m_av2Phi[apId]+=phi * phi;
      m_avM[apId]+=truthPart->m();
      m_av2M[apId]+=truthPart->m() * truthPart->m();
    }
  }

  ATH_MSG_VERBOSE("Truth selection result: pdg=" << truthPart->pdgId()
                  << ", bar=" << truthPart->barcode()
                  << ", result=" << result);

  return result;
}

StatusCode
JetTruthParticleSelectorTool::finalize()
{
  if (m_listPDGofStables)
    {
      msg(MSG::INFO) << "Counts of PDGs of all stable particles :" << endreq;
      msg(MSG::INFO) << "========================================" << endreq;
      msg(MSG::INFO) << "|    PDG    |  # particles  |" << endreq;
      for ( PDGList::iterator it = m_pdgList.begin(); it != m_pdgList.end(); it++ )
        {
          msg(MSG::INFO) << "|"
                         << std::setw(10) << it->first << " |"
                         << std::setw(10) << it->second << " |"
                         << endreq;
        }
      msg(MSG::INFO) << "|    PDG    |    <p>    |    rms    |    <pt>    |    rms    |    <eta>    |    rms    |" << endreq;
      for ( PDGList::iterator it = m_pdgList.begin(); it != m_pdgList.end(); it++ )
        {
          int n=it->second;
          double p=m_avP[it->first]/n;
          double p2=sqrt(m_av2P[it->first]/n-
                         m_avP[it->first]*m_avP[it->first]/n/n);
          double pt=m_avPt[it->first]/n;
          double pt2=sqrt(m_av2Pt[it->first]/n-
                          m_avPt[it->first]*m_avPt[it->first]/n/n);
          double eta=m_avEta[it->first]/n;
          double eta2=sqrt(m_av2Eta[it->first]/n-
                           m_avEta[it->first]*m_avEta[it->first]/n/n);
          msg(MSG::INFO) << "|"
                         << std::setw(15) << it->first << " |"
                         << std::setw(15) << p << " |"
                         << std::setw(15) << p2 << " |"
                         << std::setw(15) << pt << " |"
                         << std::setw(15) << pt2 << " |"
                         << std::setw(15) << eta << " |"
                         << std::setw(15) << eta2 << " |"
                         << endreq;
        }
      msg(MSG::INFO) << "|    PDG    |   <phi>   |    rms    |     <m>     |     rms    |" << endreq;
      for ( PDGList::iterator it = m_pdgList.begin(); it != m_pdgList.end(); it++ )
        {
          int n=it->second;
          double phi=m_avPhi[it->first]/n;
          double phi2=sqrt(m_av2Phi[it->first]/n-
                           m_avPhi[it->first]*m_avPhi[it->first]/n/n);
          double m=m_avM[it->first]/n;
          double m2=0;
          if ( m_av2M[it->first]/n > m_avM[it->first]*m_avM[it->first]/n/n)
            m2=sqrt(m_av2M[it->first]/n-
                    m_avM[it->first]*m_avM[it->first]/n/n);
          msg(MSG::INFO) << "|"
                         << std::setw(15) << it->first << " |"
                         << std::setw(15) << phi << " |"
                         << std::setw(15) << phi2 << " |"
                         << std::setw(15) << m << " |"
                         << std::setw(15) << m2 << " |"
                         << endreq;
        }
    }
  return StatusCode::SUCCESS;
}
