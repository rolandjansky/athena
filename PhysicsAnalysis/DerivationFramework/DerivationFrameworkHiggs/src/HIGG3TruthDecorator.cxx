/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHiggs/HIGG3TruthDecorator.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// Helper includes
#include "FourMomUtils/xAODP4Helpers.h"

namespace DerivationFramework {

  // Constructor
  HIGG3TruthDecorator::HIGG3TruthDecorator(const std::string& t,
                                           const std::string& n,
                                           const IInterface* p) :
    AthAlgTool(t, n, p)
  {

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty( "InputElectronContainerName", m_inElContName  = "Electrons" );
    declareProperty( "InputMuonContainerName", m_inMuContName  = "Muons" );
    declareProperty( "IsSherpa", m_isSherpa = false );
    declareProperty( "isPowPy8EvtGen", m_isPowPy8EvtGen = false );
  }

  // Destructor
  HIGG3TruthDecorator::~HIGG3TruthDecorator() {
  }

  StatusCode HIGG3TruthDecorator::initialize() {
    ATH_MSG_DEBUG ("Initializing " << name() << "...");



    // Print the configuration to the log file
    ATH_MSG_DEBUG( "Using InputElectronContainerName: " << m_inElContName );
    ATH_MSG_DEBUG( "Using InputMuonContainerName: " << m_inMuContName );
    ATH_MSG_DEBUG( "Using IsSherpa: " << m_isSherpa );
    ATH_MSG_DEBUG( "Using isPowPy8EvtGen: " << m_isPowPy8EvtGen );

    return StatusCode::SUCCESS;
  }

  StatusCode HIGG3TruthDecorator::finalize(){
    return StatusCode::SUCCESS;
  }

  StatusCode HIGG3TruthDecorator::addBranches() const {

    const xAOD::EventInfo* eventInfo = nullptr;
    ATH_CHECK( evtStore()->retrieve(eventInfo) );

    // Only run on MC
    const bool isSim = eventInfo->eventType(xAOD::EventInfo::EventType::IS_SIMULATION);
    if ( !isSim ) {
      ATH_MSG_DEBUG ("It is a data event... nothing to be done...");
      return StatusCode::SUCCESS;
    }

    // Define the decorators outside of the loop as static, such that they
    // will be fully cached
    static SG::AuxElement::Decorator<int>  decFlavourTag ("truthFlavourTag");

    // Retrieve the truth event
    const xAOD::TruthEventContainer* truthEvents(nullptr);
    CHECK(evtStore()->retrieve(truthEvents, "TruthEvents"));
    const xAOD::TruthEvent * event = truthEvents->at(0);

    std::vector<const xAOD::TruthParticle* > m_interestingLeptons;
    std::vector<const xAOD::TruthParticle* > ORLeptons;
    std::vector<const xAOD::TruthParticle* > m_interestingParticles;
    std::vector<const xAOD::TruthParticle* > m_neutrinosFromW;
    std::vector<const xAOD::TruthParticle* > m_leptonsFromW;
    int Nz=0;
    int Nem =0;
    int Ntau=0;
    int NeDirect=0;
    int NmDirect=0;
    int Nnu =0;
    int Nem_acc=0;

    // if this is a sample where the neutrinos and the leptons have the same flavour (so this will be undistinguishable WW/ZZ events),
    //when this is !=0, this can only be a ZZ event
    int CountSherpaLepton=0;

    ///////////////////////////////////////////////////////////////////////////
    // this part is from Pere on H removal in WWW events
    int abs_pdgId;
    int abs_parent_pdgId;
    int nWlep=0;
    int nWnu=0;
    int nWparents=0;
    std::map<float,TLorentzVector> Wbosons;

    //// this is for the OR removal between W+jets and W+gamma samples
    bool hasPhoton=false;
    std::vector<const xAOD::TruthParticle* > photons;
    std::vector<const xAOD::TruthParticle* > leptonsFromWZwithTau;

    // For the truth Higgs bosons
    std::vector<const xAOD::TruthParticle* > higgsBosons;

    // Define some containers of selected objects that are needed for the VBF mjj flag
    std::vector<const xAOD::TruthParticle* > MCTruthPhotonList;
    std::vector<const xAOD::TruthParticle* > MCTruthElectronList;
    std::vector<TLorentzVector > MCTruthTauList;

    // Get the truth particles from the event and loop over them
    std::vector<float> ptLoss;
    for(size_t p = 0; p < event->nTruthParticles(); p++) {
      const xAOD::TruthParticle* truthPart = event->truthParticle(p);
      if ( !truthPart ) continue;
      if ( truthPart->barcode()>2e5 ) break;
      const int pdg       = truthPart->pdgId();
      const int absPdg    = std::abs(pdg);
      const int status    = truthPart->status();
      const double pt     = truthPart->pt();
      double eta = 999999.0;
      if ( pt > 0.1 ) eta = truthPart->eta();
      const double absEta = std::abs(eta);
      const xAOD::TruthVertex* decvtx = truthPart->decayVtx();


      // Select everything that is needed to get the VBF mjj variable correctly

      // Photons
      if ( absPdg == 22 && status == 1 && pt >= 15000.0 && absEta <= 5.0  ) {
        MCTruthPhotonList.push_back( truthPart ) ;
      }

      // Electrons
      if ( absPdg == 11 && status == 1 && pt >= 15000.0 && absEta <= 5.0 ) {
        MCTruthElectronList.push_back( truthPart ) ;
      }

      // Taus
      if ( absPdg == 15 && status != 3 ) {
        const xAOD::TruthParticle* tau = truthPart;
        const xAOD::TruthVertex* taudecvtx = tau->decayVtx();
        int leptonic = 0;

        if (taudecvtx && taudecvtx->nOutgoingParticles()>0 ) {
          for ( std::size_t chil=0; chil<taudecvtx->nOutgoingParticles(); chil++) {
            const xAOD::TruthParticle* testTau = taudecvtx->outgoingParticle(chil);
            if (testTau==0) {
              ATH_MSG_DEBUG(" Found child of tau lepton with NULL pointer!!!!");
              leptonic = -999;
              continue;
            }
            const int childAbsPdg = std::abs(testTau->pdgId());
            if ( childAbsPdg == 12  ) {
              leptonic = 1;
            }
            if ( childAbsPdg == 14  ) {
              leptonic = 2;
            }
            if ( childAbsPdg == 15  ) {
              leptonic = 11;
            }
          }
        }

        if (leptonic == 0) {
          TLorentzVector nutau  = this->sumDaughterNeutrinos( tau );
          TLorentzVector tauvis( tau->px() - nutau.Px(),
                                 tau->py() - nutau.Py(),
                                 tau->pz() - nutau.Pz(),
                                 tau->e()  - nutau.E()  );

          if ( tauvis.Pt() >= 15000.0 && std::abs(tauvis.PseudoRapidity()) <= 5.0 ) {
            MCTruthTauList.push_back( tauvis ) ;
          }
        }
      } // Done with the stuff for the VBF mjj

    } // end loop over all truth particles

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








    //---------------------------------------------//
    // Lepton truth flavour decorator as in Run-I  //
    //---------------------------------------------//

    // Store b, c, s and light truth objects (quarks and hadrons)

    std::vector<const xAOD::TruthParticle*> bTruth; bTruth.clear();
    std::vector<const xAOD::TruthParticle*> cTruth; cTruth.clear();
    std::vector<const xAOD::TruthParticle*> sTruth; sTruth.clear();
    std::vector<const xAOD::TruthParticle*> lTruth; lTruth.clear();
    std::vector<const xAOD::TruthParticle*> eTruth; eTruth.clear();
    std::vector<const xAOD::TruthParticle*> mTruth; mTruth.clear();

    for (size_t p=0; p < event->nTruthParticles(); ++p) {
      const xAOD::TruthParticle* truthPart = event->truthParticle(p);
      if (!truthPart) continue; // Protection against null ptrs
      long int pdg = truthPart->pdgId();
      if (truthPart->pt() <= 5000) continue;

      //Classify the truth particle
      bool hasBottom = 0; bool hasCharm = 0; bool hasStrange = 0; bool hasLight = 0;
      int q1 = (abs(pdg)/1000)%10; int q2 = (abs(pdg)/100)%10; int q3 = (abs(pdg)/10)%10;

      if (q1 == 0 && q2 == 5 && q3 == 5) hasBottom = 1; //BBbar meson
      else if (q1 == 0 && q3 < 5 && q3 > 0 && q2 == 5 ) hasBottom = 1; //Bottom meson
      else if (q1 == 5) hasBottom = 1; //Bottom baryon
      else if (abs(pdg) == 5) hasBottom = 1; //Bottom quark

      else if (q1 == 0 && q3 == 4 && q2 == 4) hasCharm = 1; //CCbar meson
      else if (q1 == 0 && q3 < 4 && q3 > 0 && q2 == 4) hasCharm = 1; //Charmed meson
      else if (q1 == 4) hasCharm = 1; //Charmed baryon
      else if (abs(pdg) == 4) hasCharm = 1; //Charm quark

      else if (q1 == 3) hasStrange = 1; //Strange baryon
      else if ((q1 == 0 && q2 == 3 && q3 < 3 && q3 > 0)|| abs(pdg) == 130) hasStrange = 1; //Strange meson
      else if (abs(pdg) == 3) hasStrange = 1; //Strange quark

      else if (q1 == 2 || q1 == 1) hasLight = 1; //Light baryon
      else if ((q1==0 && (q3 == 1 || q3 == 2) && (q2 == 1|| q2 == 2)) || (q1 == 0&& q3 == 3 && q2 == 3)) hasLight = 1; //Light meson
      else if (abs(pdg) == 2 || abs(pdg) == 1) hasLight = 1; //u,d quarks

      if (hasBottom) bTruth.push_back(truthPart);
      if (hasCharm) cTruth.push_back(truthPart);
      if (hasStrange) sTruth.push_back(truthPart);
      if (hasLight) lTruth.push_back(truthPart);
      if (fabs(pdg) == 11) eTruth.push_back(truthPart);
      if (fabs(pdg) == 13) mTruth.push_back(truthPart);
    }


    // Open the input electron container
    const xAOD::ElectronContainer *inElCont(nullptr);
    ATH_CHECK( evtStore()->retrieve( inElCont, m_inElContName ) );

    //Now classify the electrons
    for ( const xAOD::Electron* electron : *inElCont) {
      int flavTag = -1;

      TLorentzVector elVect; elVect.SetPtEtaPhiE(electron->pt(), electron->eta(), electron->phi(), electron->e());

      bool electronBottom = 0; bool electronCharm = 0; bool electronStrange = 0;
      bool electronLight = 0;

      bool bottomLeptonic = 0; bool bottomHadronic = 0;
      bool charmLeptonic = 0; bool charmHadronic = 0;
      bool strangeLeptonic = 0; bool strangeHadronic = 0;
      bool lightLeptonic = 0; bool lightHadronic = 0;
      bool gluonLeptonic = 0; bool gluonHadronic = 0;

      //See if it is matched to a truth electron
      bool isLeptonic = 0;
      for (size_t m = 0; m < eTruth.size(); ++m){
        TLorentzVector truthVect; truthVect.SetPtEtaPhiE(eTruth[m]->pt(), eTruth[m]->eta(), eTruth[m]->phi(), eTruth[m]->e());
        double dR = elVect.DeltaR(truthVect);
        if (dR < 0.03) isLeptonic = 1;
      }

      //Try to find a bottom-match
      for (size_t b = 0; b < bTruth.size(); ++b){
        TLorentzVector bVect; bVect.SetPtEtaPhiE(bTruth[b]->pt(), bTruth[b]->eta(), bTruth[b]->phi(), bTruth[b]->e());
        double dR = elVect.DeltaR(bVect);
        if (dR < 0.4) electronBottom = 1;
      }

      if (electronBottom){
        if (isLeptonic) bottomLeptonic = 1;
        else bottomHadronic = 1;
      }

      if (!electronBottom){

        //Try to find a charm-match
        for (size_t c = 0; c < cTruth.size(); ++c){
          TLorentzVector cVect; cVect.SetPtEtaPhiE(cTruth[c]->pt(), cTruth[c]->eta(), cTruth[c]->phi(), cTruth[c]->e());
          double dR = elVect.DeltaR(cVect);
          if (dR < 0.4) electronCharm = 1;
        }

        if (electronCharm){
          if (isLeptonic) charmLeptonic = 1;
          else charmHadronic = 1;
        }

        if (!electronCharm){

          //Try to find a strange-match
          for (size_t s = 0; s < sTruth.size(); ++s){
            TLorentzVector sVect; sVect.SetPtEtaPhiE(sTruth[s]->pt(), sTruth[s]->eta(), sTruth[s]->phi(), sTruth[s]->e());
            double dR = elVect.DeltaR(sVect);
            if (dR < 0.4) electronStrange = 1;
          }

          if (electronStrange){
            if (isLeptonic) strangeLeptonic = 1;
            else strangeHadronic = 1;
          }

          if (!electronStrange){

            //Try to find a light-match
            for (size_t l = 0; l < lTruth.size(); ++l){
              TLorentzVector lVect; lVect.SetPtEtaPhiE(lTruth[l]->pt(), lTruth[l]->eta(), lTruth[l]->phi(), lTruth[l]->e());
              double dR = elVect.DeltaR(lVect);
              if (dR < 0.4) electronLight = 1;
            }

            if (electronLight){
              if (isLeptonic) lightLeptonic = 1;
              else lightHadronic = 1;
            }

            if (!electronLight){
              if (isLeptonic) gluonLeptonic = 1;
              else gluonHadronic = 1;

            }
          }
        }
      }

      if (bottomLeptonic) flavTag = 1;
      if (bottomHadronic) flavTag = 2;

      if (charmLeptonic) flavTag = 3;
      if (charmHadronic) flavTag = 4;

      if (strangeLeptonic) flavTag = 5;
      if (strangeHadronic) flavTag = 6;

      if (lightLeptonic) flavTag = 7;
      if (lightHadronic) flavTag = 8;

      if (gluonLeptonic) flavTag = 9;
      if (gluonHadronic) flavTag = 10;

      //Decorate the electron with the flavour tag
      decFlavourTag(*electron) = flavTag;

    } // end: loop over electrons for Run1 truth flavour decoration


    // Open the input muon container
    const xAOD::MuonContainer *inMuCont(nullptr);
    ATH_CHECK( evtStore()->retrieve( inMuCont, m_inMuContName ) );

    //Now classify the muons
    for ( const xAOD::Muon* muon : *inMuCont) {
      int flavTag = -1;

      TLorentzVector muVect; muVect.SetPtEtaPhiE(muon->pt(), muon->eta(), muon->phi(), muon->e());

      bool muonBottom = 0; bool muonCharm = 0; bool muonStrange = 0;
      bool muonLight = 0;

      bool bottomLeptonic = 0; bool bottomHadronic = 0;
      bool charmLeptonic = 0; bool charmHadronic = 0;
      bool strangeLeptonic = 0; bool strangeHadronic = 0;
      bool lightLeptonic = 0; bool lightHadronic = 0;
      bool gluonLeptonic = 0; bool gluonHadronic = 0;

      //See if it is matched to a truth muon
      bool isLeptonic = 0;
      for (size_t m = 0; m < mTruth.size(); ++m){
        TLorentzVector truthVect; truthVect.SetPtEtaPhiE(mTruth[m]->pt(), mTruth[m]->eta(), mTruth[m]->phi(), mTruth[m]->e());
        double dR = muVect.DeltaR(truthVect);
        if (dR < 0.03) isLeptonic = 1;
      }

      //Try to find a bottom-match
      for (size_t b = 0; b < bTruth.size(); ++b){
        TLorentzVector bVect; bVect.SetPtEtaPhiE(bTruth[b]->pt(), bTruth[b]->eta(), bTruth[b]->phi(), bTruth[b]->e());
        double dR = muVect.DeltaR(bVect);
        if (dR < 0.4) muonBottom = 1;
      }

      if (muonBottom){
        if (isLeptonic) bottomLeptonic = 1;
        else bottomHadronic = 1;
      }

      if (!muonBottom){

        //Try to find a charm-match
        for (size_t c = 0; c < cTruth.size(); ++c){
          TLorentzVector cVect; cVect.SetPtEtaPhiE(cTruth[c]->pt(), cTruth[c]->eta(), cTruth[c]->phi(), cTruth[c]->e());
          double dR = muVect.DeltaR(cVect);
          if (dR < 0.4) muonCharm = 1;
        }

        if (muonCharm){
          if (isLeptonic) charmLeptonic = 1;
          else charmHadronic = 1;
        }

        if (!muonCharm){

          //Try to find a strange-match
          for (size_t s = 0; s < sTruth.size(); ++s){
            TLorentzVector sVect; sVect.SetPtEtaPhiE(sTruth[s]->pt(), sTruth[s]->eta(), sTruth[s]->phi(), sTruth[s]->e());
            double dR = muVect.DeltaR(sVect);
            if (dR < 0.4) muonStrange = 1;
          }

          if (muonStrange){
            if (isLeptonic) strangeLeptonic = 1;
            else strangeHadronic = 1;
          }

          if (!muonStrange){

            //Try to find a light-match
            for (size_t l = 0; l < lTruth.size(); ++l){
              TLorentzVector lVect; lVect.SetPtEtaPhiE(lTruth[l]->pt(), lTruth[l]->eta(), lTruth[l]->phi(), lTruth[l]->e());
              double dR = muVect.DeltaR(lVect);
              if (dR < 0.4) muonLight = 1;
            }

            if (muonLight){
              if (isLeptonic) lightLeptonic = 1;
              else lightHadronic = 1;
            }

            if (!muonLight){
              if (isLeptonic) gluonLeptonic = 1;
              else gluonHadronic = 1;

            }
          }
        }
      }

      if (bottomLeptonic) flavTag = 1;
      if (bottomHadronic) flavTag = 2;

      if (charmLeptonic) flavTag = 3;
      if (charmHadronic) flavTag = 4;

      if (strangeLeptonic) flavTag = 5;
      if (strangeHadronic) flavTag = 6;

      if (lightLeptonic) flavTag = 7;
      if (lightHadronic) flavTag = 8;

      if (gluonLeptonic) flavTag = 9;
      if (gluonHadronic) flavTag = 10;

      //Decorate the muon with the flavour tag
      decFlavourTag(*muon) = flavTag;

    } // end: loop over muons for Run1 truth flavour decoration

    return StatusCode::SUCCESS;
  } // end: addBranches() method

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //// some helper function
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  bool HIGG3TruthDecorator::decayIntoItself(const xAOD::TruthParticle* part, int status) const {
    if (!part) return false;
    const int partPDG = part->pdgId();
    const xAOD::TruthVertex* decvtx = part->decayVtx();
    if ( !decvtx )                         return false;
    if ( decvtx->nOutgoingParticles()==0 ) return false;

    for (unsigned int chil=0; chil<decvtx->nOutgoingParticles(); ++chil) {
      const xAOD::TruthParticle* child = decvtx->outgoingParticle(chil);
      if (!child) continue;
      const int barcode = child->barcode();
      if ( barcode>=2e5 ) continue;
      const int PDG = child->pdgId();
      if ( status==-1 ) {
        if ( PDG==partPDG ) return true;
      } else {
        if ( PDG==partPDG && child->status()==status) return true;
      }
    }
    return false;
  }



  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const xAOD::TruthParticle* HIGG3TruthDecorator::lastOfKind(const xAOD::TruthParticle* part) const {
    if (!part) return nullptr;
    const int partPDG = part->pdgId();
    const xAOD::TruthVertex* decvtx = part->decayVtx();
    if ( !decvtx ) return part;
    if ( decvtx->nOutgoingParticles()==0 ) return part;

    for (unsigned int chil=0; chil<decvtx->nOutgoingParticles(); chil++) {
      const xAOD::TruthParticle* childPart = decvtx->outgoingParticle(chil);
      if (!childPart) continue;
      const int barcode = childPart->barcode();
      if ( barcode >= 2e5 ) continue;
      const int PDG = childPart->pdgId();
      if ( PDG == partPDG ) return lastOfKind(childPart);
    }
    return part;
  }



  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  const xAOD::TruthParticle* HIGG3TruthDecorator::firstOfKind(const xAOD::TruthParticle* part) const {
    if (!part) return nullptr;
    int partPDG = part->pdgId();
    const xAOD::TruthVertex* prodvtx = part->prodVtx();
    if ( !prodvtx ) return part;
    const std::size_t nInParts = prodvtx->nIncomingParticles();
    if ( nInParts == 0 ) return part;

    if ( nInParts==1 ) {
      const xAOD::TruthParticle* inPart = prodvtx->incomingParticle(0);
      if (!inPart) return part;
      const int motherPDG = inPart->pdgId();
      if ( partPDG!=motherPDG ) return part;
      return firstOfKind(inPart);
    } else { //needed for Sherpa
      for (unsigned int moth=0; moth<nInParts; moth++) {
        const xAOD::TruthParticle* inMother = prodvtx->incomingParticle(moth);
        if (!inMother) continue;
        const int motherPDG = inMother->pdgId();
        if ( partPDG != motherPDG ) continue;
        if ( inMother->status()==11 ) return inMother;
      }
    }
    return part;
  }



  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  void HIGG3TruthDecorator::printRecursively(const xAOD::TruthParticle* part, std::string preVal) const {
    if (!part) return;
    const xAOD::TruthVertex* decvtx = part->decayVtx();
    if ( !decvtx ) return ;
    const std::size_t nOutParts = decvtx->nOutgoingParticles();
    if ( nOutParts==0 ) return ;

    for (unsigned int chil=0; chil<nOutParts; chil++) {
      const xAOD::TruthParticle* childPart = decvtx->outgoingParticle(chil);
      if (!childPart) continue;
      const int barcode = childPart->barcode();
      if ( barcode>=2e5 ) continue;
      printRecursively(childPart, preVal+" ");
    }
    return ;
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool HIGG3TruthDecorator::isFromW( const xAOD::TruthParticle* part, const std::vector<  const xAOD::TruthParticle*>& Wlist) const {
    if (!part) return false;
    const xAOD::TruthParticle* tmpPart = firstOfKind(part);
    for ( unsigned int iP=0; iP<Wlist.size(); ++iP) {
      if ( tmpPart==Wlist.at(iP) ) return true;
    }
    return false;
  }



  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TLorentzVector HIGG3TruthDecorator::sumDaughterNeutrinos(const xAOD::TruthParticle *part) const {
    TLorentzVector nu(0,0,0,0);
    if ( ( abs( part->pdgId() ) == 12 ) || ( abs( part->pdgId() ) == 14 ) || ( abs( part->pdgId() ) == 16 ) ) {
      nu.SetPx(part->px());
      nu.SetPy(part->py());
      nu.SetPz(part->pz());
      nu.SetE(part->e());
      return nu;
    }

    if ( part->hasDecayVtx() == 0 ) return nu;
    const xAOD::TruthVertex* decvtx = part->decayVtx();
    const std::size_t nOutParts = decvtx->nOutgoingParticles();
    for ( unsigned int chil=0; chil<nOutParts; ++chil) {
      // Protect against null pointer. This should not happen though.
      // Maybe, if that is the case for a tau decay, sum the visible parts and
      // subtract those from the full tau instead?
      const xAOD::TruthParticle* childPart = decvtx->outgoingParticle(chil);
      if (!childPart){ continue; }
      nu += sumDaughterNeutrinos( childPart );
    }
    return nu;
  }



  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool HIGG3TruthDecorator::checkOverlap(const xAOD::IParticle* part, std::vector<const xAOD::TruthParticle* > list) const {
    if (!part) return false;
    for (std::size_t i = 0; i < list.size(); ++i) {
      const xAOD::TruthParticle* truthPart = list[i];
      const double pt = truthPart->pt();
      if (pt > 15000.0) {
        if ( xAOD::P4Helpers::isInDeltaR(*part, *truthPart, 0.3) ) return true;
      }
    }
    return false;
  }



  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  bool HIGG3TruthDecorator::checkOverlap(const xAOD::IParticle* part, std::vector<TLorentzVector > list) const {
    if (!part) return false;
    const double maxDeltaR2 = 0.3*0.3;
    for (size_t i = 0; i < list.size(); ++i) {
      const TLorentzVector& partB = list[i];
      const double pt = partB.Pt();
      if (pt > 15000.0) {
        const double phi      = partB.Phi();
        const double rapidity = partB.PseudoRapidity();
        if ( xAOD::P4Helpers::deltaR2(*part,rapidity,phi) < maxDeltaR2 ) return true;
      }
    }
    return false;
  }



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  double HIGG3TruthDecorator::calculateMT( const xAOD::TruthParticle *lep1, const xAOD::TruthParticle *lep2, const xAOD::MissingET* missET ) const {
    const double mpx( missET->mpx() );
    const double mpy( missET->mpy() );
    const double met( missET->met() );
    TLorentzVector l1 = lep1->p4();
    TLorentzVector l2 = lep2->p4();
    TLorentzVector leptons = l1+l2;

    // Now, actually calculate the result (in two parts)
    const double px( leptons.Px() );
    const double py( leptons.Py() );
    const double part2 = (px+mpx)*(px+mpx) + (py+mpy)*(py+mpy);
    double mll2( leptons.M2() );
    // This is needed for rare cases when the TLorentzVector.M2 returns negative values
    mll2 = mll2 < 0.0 ? -mll2 : mll2;
    const double etll( std::sqrt( (px*px + py*py) + mll2 ) );
    const double part1( (etll+met)*(etll+met) );

    // One last sanity check
    if ( part1 < part2 ) {
      // This should not be... throw an error.
      throw std::runtime_error("Got an invalid mt calculation");
    }
    return std::sqrt( part1 - part2 );
  }

} // end: DerivationFramework namespace
