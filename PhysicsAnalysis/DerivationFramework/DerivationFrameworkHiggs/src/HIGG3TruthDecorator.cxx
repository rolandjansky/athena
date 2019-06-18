/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHiggs/HIGG3TruthDecorator.h"

// FrameWork includes
#include "GaudiKernel/SystemOfUnits.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// Helper includes
#include "FourMomUtils/xAODP4Helpers.h"
#include "CxxUtils/fpcompare.h"

using Gaudi::Units::GeV;

bool xaodPtSorting(const xAOD::IParticle *part1,
                   const xAOD::IParticle *part2) {
  return CxxUtils::fpcompare::greater( part1->pt(), part2->pt() );
}

namespace DerivationFramework {

  // Constructor
  HIGG3TruthDecorator::HIGG3TruthDecorator(const std::string& t,
                                           const std::string& n,
                                           const IInterface* p) :
    AthAlgTool(t, n, p),
    m_inElContName("Electrons"),
    m_inMuContName("Muons"),
    m_inEMTopoJetContName("AntiKt4EMTopoJets"),
    m_inPFlowJetContName("AntiKt4EMPFlowJets"),
    m_truthJetPtMin(10.0*GeV),
    m_maxDeltaR(0.3),
    m_isSherpa(false),
    m_isPowPy8EvtGen(false),
    m_GamORTool("VGammaORTool/HIGG3TruthDecoratorVGammaORTool", this)
  {

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty( "InputElectronContainerName",  m_inElContName, "Input reco electron container name" );
    declareProperty( "InputMuonContainerName",      m_inMuContName, "Input reco muon container name" );
    declareProperty( "InputEMTopoJetContainerName", m_inEMTopoJetContName, "Input reco jet container name (EMTopo)" );
    declareProperty( "InputPFlowJetContainerName",  m_inPFlowJetContName, "Input reco jet container name (PFlow)" );
    declareProperty( "IsSherpa",                    m_isSherpa, "Whether or not the sample is treated as Sherpa" );
    declareProperty( "isPowPy8EvtGen",              m_isPowPy8EvtGen, "Whether or not the sample is treated as PowPy8EvtGen" );
    declareProperty( "TruthJetMinPt",               m_truthJetPtMin, "The minimum pt threshold for a truth-jet to be considered for matching" );
    declareProperty( "TruthMatchMaxDeltaR",         m_maxDeltaR, "The maximum deltaR (using rapidity and NOT eta) distance for a successful truth match" );
    declareProperty( "GamORTool",                   m_GamORTool, "The tool used for determining the Vgamma/V+jets overlap" );
  }

  // Destructor
  HIGG3TruthDecorator::~HIGG3TruthDecorator() {
  }

  StatusCode HIGG3TruthDecorator::initialize() {
    ATH_MSG_INFO ("Initializing " << name() << "...");

    // Print the configuration to the log file
    ATH_MSG_INFO( "Using InputElectronContainerName: " << m_inElContName );
    ATH_MSG_INFO( "Using InputMuonContainerName: " << m_inMuContName );
    ATH_MSG_INFO( "Using InputEMTopoJetContainerName: " << m_inEMTopoJetContName );
    ATH_MSG_INFO( "Using InputPFlowJetContainerName: " << m_inPFlowJetContName );
    ATH_MSG_INFO( "Using IsSherpa: " << m_isSherpa );
    ATH_MSG_INFO( "Using isPowPy8EvtGen: " << m_isPowPy8EvtGen );

    ATH_CHECK( m_GamORTool.retrieve() );

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

    // Define the decorators outside of any loop as static, such that they
    // will be fully cached
    static SG::AuxElement::Decorator<int> decFlavourTag ("HIGG3DX_truthFlavourTag"); // used for reco leptons
    static SG::AuxElement::Decorator<int> decTruthOrigin("HIGG3DX_truthOrigin"); // used for reco jets

    // Retrieve the truth event
    const xAOD::TruthEventContainer* truthEvents(nullptr);
    ATH_CHECK(evtStore()->retrieve(truthEvents, "TruthEvents"));
    const xAOD::TruthEvent * event = truthEvents->at(0);

    std::vector<const xAOD::TruthParticle* > interestingLeptons;
    std::vector<const xAOD::TruthParticle* > ORLeptons;
    std::vector<const xAOD::TruthParticle* > interestingParticles;
    std::vector<const xAOD::TruthParticle* > neutrinosFromW;
    std::vector<const xAOD::TruthParticle* > leptonsFromW;
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
    int nWlep=0;
    int nWnu=0;
    int nWparents=0;
    std::map<float,TLorentzVector> Wbosons;

    //// this is for the OR removal between V+jets and V+gamma samples
    bool hasFSRPhoton=false;
    bool hasFSRPhotonLargeDeltaR = false;
    std::vector<const xAOD::TruthParticle* > photons;
    std::vector<const xAOD::TruthParticle* > leptonsFromWZwithTau;

    // Define some containers of selected objects that are needed for the VBF mjj flag
    std::vector<const xAOD::TruthParticle* > MCTruthPhotonList;
    std::vector<const xAOD::TruthParticle* > MCTruthElectronList;
    std::vector<TLorentzVector > MCTruthTauList;

    std::vector<float> ptLoss;

    int nOutgoingPartons = 0;

    std::vector<const xAOD::TruthParticle*> vTopList;

    // Store b, c, s and light truth objects (quarks and hadrons) as well as the truth leptons
    // used for the decoration of reco leptons with truthFlavourTag
    std::vector<const xAOD::TruthParticle*> bTruth; bTruth.clear();
    std::vector<const xAOD::TruthParticle*> cTruth; cTruth.clear();
    std::vector<const xAOD::TruthParticle*> sTruth; sTruth.clear();
    std::vector<const xAOD::TruthParticle*> lTruth; lTruth.clear();
    std::vector<const xAOD::TruthParticle*> eTruth; eTruth.clear();
    std::vector<const xAOD::TruthParticle*> mTruth; mTruth.clear();

    // for truthOrigin of jets
    std::vector<const xAOD::TruthParticle*> selectedTruthWZBosons;
    std::vector<const xAOD::TruthParticle*> selectedTruthHiggsBosons;
    std::vector<bool> wzBosonIsFromHiggs;

    // Get the truth particles from the event and loop over them
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

      if(status==23 && !(absPdg>=11 && absPdg<=16) ){
            nOutgoingPartons++;
      }

      // Top quarks
      if ( absPdg == 6 ) {
        if ( m_isPowPy8EvtGen && status == 62 ) vTopList.push_back(truthPart);
        else if ( status == 3 ) vTopList.push_back(truthPart);
      }

      //Classify the truth particle for truthFlavourTag calculation
      if (pt > 5000) {
        bool hasBottom = 0; bool hasCharm = 0; bool hasStrange = 0; bool hasLight = 0;
        int q1 = (absPdg/1000)%10; int q2 = (absPdg/100)%10; int q3 = (absPdg/10)%10;

        if (q1 == 0 && q2 == 5 && q3 == 5) hasBottom = 1; //BBbar meson
        else if (q1 == 0 && q3 < 5 && q3 > 0 && q2 == 5 ) hasBottom = 1; //Bottom meson
        else if (q1 == 5) hasBottom = 1; //Bottom baryon
        else if (absPdg == 5) hasBottom = 1; //Bottom quark

        else if (q1 == 0 && q3 == 4 && q2 == 4) hasCharm = 1; //CCbar meson
        else if (q1 == 0 && q3 < 4 && q3 > 0 && q2 == 4) hasCharm = 1; //Charmed meson
        else if (q1 == 4) hasCharm = 1; //Charmed baryon
        else if (absPdg == 4) hasCharm = 1; //Charm quark

        else if (q1 == 3) hasStrange = 1; //Strange baryon
        else if ((q1 == 0 && q2 == 3 && q3 < 3 && q3 > 0)|| absPdg == 130) hasStrange = 1; //Strange meson
        else if (absPdg == 3) hasStrange = 1; //Strange quark

        else if (q1 == 2 || q1 == 1) hasLight = 1; //Light baryon
        else if ((q1==0 && (q3 == 1 || q3 == 2) && (q2 == 1|| q2 == 2)) || (q1 == 0&& q3 == 3 && q2 == 3)) hasLight = 1; //Light meson
        else if (absPdg == 2 || absPdg == 1) hasLight = 1; //u,d quarks

        if (hasBottom) bTruth.push_back(truthPart);
        if (hasCharm) cTruth.push_back(truthPart);
        if (hasStrange) sTruth.push_back(truthPart);
        if (hasLight) lTruth.push_back(truthPart);
        if (absPdg == 11) eTruth.push_back(truthPart);
        if (absPdg == 13) mTruth.push_back(truthPart);
      }

      // selected W,Z,H for jet truthOrigin decoration
      if (absPdg == 23 || absPdg == 24 || absPdg == 25) {
        const std::size_t nChildren = truthPart->nChildren();
        if (nChildren >= 2 ) {
          const xAOD::TruthParticle* truthChildA = truthPart->child(0);
          if (truthChildA) {
            const int childAbsPdgId = std::abs(truthChildA->pdgId());
            // Select the interesting particles
            ATH_MSG_VERBOSE("When searching for W,Z,H got a truth particle with status " << status << ", PdgID " << pdg
                            << ", nChildren " << nChildren << ", child0 PdgID " << truthChildA->pdgId()
                            << ", child0 status " << truthChildA->status() );
            if ( (truthPart->isW() || truthPart->isZ()) && nChildren >=2 && childAbsPdgId <= 18 ){
              ATH_MSG_VERBOSE("Found a truth W or Z boson");
              selectedTruthWZBosons.push_back(truthPart);
            }
            else if ( truthPart->isHiggs() && nChildren >=2 && ( childAbsPdgId==23 || childAbsPdgId==24 ) ){
              ATH_MSG_VERBOSE("Found a truth Higgs boson");
              selectedTruthHiggsBosons.push_back(truthPart);
            }
          } // end check on valid child pointer
        } // end check on nChildren
      } // end check for W,Z,H

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
          TLorentzVector nutau  = sumDaughterNeutrinos( tau );
          TLorentzVector tauvis( tau->px() - nutau.Px(),
                                 tau->py() - nutau.Py(),
                                 tau->pz() - nutau.Pz(),
                                 tau->e()  - nutau.E()  );

          if ( tauvis.Pt() >= 15000.0 && std::abs(tauvis.PseudoRapidity()) <= 5.0 ) {
            MCTruthTauList.push_back( tauvis ) ;
          }
        }
      } // Done with the stuff for the VBF mjj


      // Back to the main part
      if ( pdg==22 ) {
        if ( status==1 ) {
          if ( pt>10e3 && absEta<2.5 ) {
            photons.push_back(truthPart);
          }
        }
      }

      // this is the block from Pere:
      if( (absPdg==11) || (absPdg==12) || (absPdg==13) || (absPdg==14) || (absPdg==15) || (absPdg==16) ) {
        nWparents = 0;
        TLorentzVector Wboson;

        //loop over parent truth particles
        for(unsigned int j=0;j< truthPart->nParents();j++) {
          const xAOD::TruthParticle* parent = truthPart->parent(j);
          if (!parent) continue;
          const int absParentPdg = std::abs(parent->pdgId());

          //consider lepton or neutrino truth particles with W-boson parent
          if( absParentPdg==24 ){
            nWparents += 1;

            //fill the parent map only for the lepton truth particles (avoid double counting)
            if( (absPdg==11) || (absPdg==13) || (absPdg==15) ){
              Wboson.SetPtEtaPhiM(parent->pt(), parent->eta(), parent->phi(), parent->m());
              Wbosons.insert( std::map<float,TLorentzVector>::value_type(truthPart->charge()*(parent->m()), Wboson) );
            }
          }
        }//end loop over parent truth particles
      }

      //count number of leptons in event with exactly one W-boson parent
      if ( ((absPdg==11) || (absPdg==13) || (absPdg==15)) && (nWparents == 1) )  nWlep += 1;

      //count number of neutrinos in event with exactly one W-boson parent
      if( ((absPdg==12) || (absPdg==14) || (absPdg==16)) && (nWparents == 1) )   nWnu += 1;

      //////////////// END of Pere's block


      if ( m_isSherpa && status==11 && ( absPdg>10 && absPdg<17 ) ) {
        /*
        ATH_MSG_DEBUG( "  found a s11 with: " << truthPart->pt()
                 << " , " << truthPart->eta()
                 << " , " << truthPart->phi()
                 << " , Status: " << truthPart->status()
                 << " , barcode: " << truthPart->barcode()
                 << "  ID: " <<  truthPart->pdgId() );// << std::endl;
        */

        if ( absPdg==12 || absPdg==14 || absPdg==16 ) {
          neutrinosFromW.push_back( truthPart );
          Nnu++;
          if ( absPdg==12 ) CountSherpaLepton += -11;
          if ( absPdg==14 ) CountSherpaLepton += -13;
          if ( absPdg==16 ) CountSherpaLepton += -15;
        }

        if ( absPdg==11 || absPdg==13 || absPdg==15 ) {
          // this is crazy but Sherpa is really impossible to understand!!!!!
          if ( absPdg==15 &&  decvtx!=0) {
            if (  decayIntoItself(truthPart, 11) ) continue;
          }
          CountSherpaLepton += absPdg;

          if ( absPdg==11 )  {
            Nem++;
            NeDirect++;
          }
          if ( absPdg==13 )  {
            Nem++;
            NmDirect++;
          }
          if ( absPdg==15 ) {
            leptonsFromWZwithTau.push_back(truthPart);
            Ntau++;
            //printRecursively(truthPart);
            const xAOD::TruthParticle* lepton= lastOfKind(truthPart); // overwrite to avoid GenRecord crazyness
            const xAOD::TruthVertex* decLepton = lepton->decayVtx();
            if ( decLepton==0 ) {
              ATH_MSG_DEBUG(" Found tau lepton with EMPTY decay vertex .... BAD!!!!");
              continue;
            }
            for (unsigned int partT=0; partT<decLepton->nOutgoingParticles(); partT++) {
              const xAOD::TruthParticle* lep2 = decLepton->outgoingParticle(partT);
              if ( lep2==0 ) {
                ATH_MSG_DEBUG(" Found child of tau lepton with NULL pointer!!!!");
                continue;
              }
              const int PDGtau=lep2->pdgId();
              ATH_MSG_DEBUG("Found a tau decay with: " << lep2->pt() << " , " << lep2->eta()
                            << " , " << lep2->phi() << " , Status: " << lep2->status() << "  ID: " <<  lep2->pdgId() << "  barcode: " << lep2->barcode() );
              if ( std::abs(PDGtau)==12 || std::abs(PDGtau)==14 || std::abs(PDGtau)==16 ) {
                neutrinosFromW.push_back( lep2 );
                Nnu++;
              } else if ( std::abs(PDGtau)==11 || std::abs(PDGtau)==13 ) {
                leptonsFromW.push_back(lep2);
              }
            }
          } // end of tau block
          if ( absPdg==11 || absPdg==13 ) {
            leptonsFromW.push_back(truthPart);
            leptonsFromWZwithTau.push_back(truthPart);
          }
        }
      } ///end of Sherpa block

      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////
      // W/Z block
      if ( ( absPdg==24 || absPdg==23 ) && !m_isSherpa && decvtx!=0 ) {

        if ( decvtx->nOutgoingParticles()<2 ) {
          ATH_MSG_VERBOSE("Found a W/Z not decaying in at least 2 particles ... status: " << truthPart->status() << " and barcode: " << truthPart->barcode() );
          continue;
        }

        if ( absPdg==23 ) Nz++;

        ATH_MSG_DEBUG( "  found a boson with: " << truthPart->pt() << " , " << eta
                       << " , " << truthPart->phi() << " , Status: " << truthPart->status() << "  ID: " <<  truthPart->pdgId() );// << std::endl;

        interestingParticles.push_back( truthPart );
        for (unsigned int part=0; part<decvtx->nOutgoingParticles(); part++) {
          const xAOD::TruthParticle* lepton = decvtx->outgoingParticle(part);
          if (!lepton) {
            ATH_MSG_DEBUG(" SOMETHING MUST BE REALLY REALLY WRONG HERE ... W/Z direct decay is NULL pointer ");
            continue;
          }
          const int PDGlep = lepton->pdgId();
          const xAOD::TruthVertex* decLepton = lepton->decayVtx();

          if ( std::abs(PDGlep)==15 ||  std::abs(PDGlep)==13 ||  std::abs(PDGlep)==11 ){
            leptonsFromWZwithTau.push_back(lepton);
          }

          /// case of taus
          if ( std::abs(PDGlep)==15 ) {
            Ntau++;
            ATH_MSG_DEBUG( "  found a tau with: " << lepton->pt() << " , " << lepton->eta()
                           << " , " << lepton->phi() << " , Status: " << lepton->status() << "  ID: " <<  lepton->pdgId() );
            lepton = lastOfKind(lepton); // overwrite to avoid GenRecord crazyness
            ATH_MSG_DEBUG( "  found a Final tau with: " << lepton->pt() << " , " << lepton->eta()
                           << " , " << lepton->phi() << " , Status: " << lepton->status() << "  ID: " <<  lepton->pdgId() );
            decLepton= lepton->decayVtx();
            if ( !decLepton ) {
              ATH_MSG_DEBUG(" Found tau lepton with EMPTY decay vertex .... BAD!!!!");
              continue;
            }
            for (unsigned int partT=0; partT<decLepton->nOutgoingParticles(); partT++) {
              const xAOD::TruthParticle* lep2 = decLepton->outgoingParticle(partT);
              if ( !lep2 ) {
                ATH_MSG_DEBUG(" Found child of tau lepton with NULL pointer!!!!");
                continue;
              }
              const int PDGtau = lep2->pdgId();
              ATH_MSG_DEBUG( "     found a tau decay with: " << lep2->pt() << " , " << lep2->eta()
                             << " , " << lep2->phi() << " , Status: " << lep2->status() << "  ID: " <<  lep2->pdgId() << "  barcode: " << lep2->barcode() );
              if ( std::abs(PDGtau)==12 || std::abs(PDGtau)==14 || std::abs(PDGtau)==16 ) {
                neutrinosFromW.push_back( lep2 );
                Nnu++;
              } else if ( std::abs(PDGtau)==11 || std::abs(PDGtau)==13 ) {
                leptonsFromW.push_back(lep2);
              }
            }
          } // end of tau block
          if ( std::abs(PDGlep)==11 || std::abs(PDGlep)==13 ) {
            ATH_MSG_DEBUG( "     found a boson direct decay with: " << lepton->pt() << " , " << lepton->eta() << " , " << lepton->phi()
                           << " , Status: " << lepton->status() << "  ID: " <<  lepton->pdgId() << "  barcode: " << lepton->barcode() );
            //printRecursively( decvtx->outgoingParticle(part), "  ");
            Nem++;
            leptonsFromW.push_back( lepton );
            //if ( std::abs(PDGlep)==13 ) {
            //  const xAOD::TruthParticle* tmpPart2=lastOfKind(decvtx->outgoingParticle(part));
            //  float tmpPtLoss= (tmpPart2->pt()-decvtx->outgoingParticle(part)->pt())/decvtx->outgoingParticle(part)->pt();
            //  ptLoss.push_back(tmpPtLoss);
            //}
            if ( std::abs(PDGlep)==11 )  NeDirect++;
            if ( std::abs(PDGlep)==13 )  NmDirect++;
          }
          if ( std::abs(PDGlep)==12 || std::abs(PDGlep)==14 || std::abs(PDGlep)==16 ) {
            neutrinosFromW.push_back( lepton );
            Nnu++;
          }
        }
      } // end of W/Z block

      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////
      if ( absPdg==25 && decvtx!=0 ) {
        if ( decvtx->nOutgoingParticles()<2 ) ATH_MSG_DEBUG("Found a Higgs boson with not decaying into at least 2 particles");
        else {
          ATH_MSG_DEBUG( "  found a Higgs boson with: " << truthPart->pt() << " , " << eta
                         << " , " << truthPart->phi() << " , Status: " << truthPart->status() << "  ID: " <<  truthPart->pdgId() );// << std::endl;
          interestingParticles.push_back( truthPart );
        }
      }

      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////
      /////////////////////////////////////////////////////////////////////////////

      // Leptons for Sherpa 2.2 Z+jets re-weighting
      if( pt>20e3 && absEta<4.5 && status==1 && (absPdg==13 || absPdg==11) && !decayIntoItself(truthPart) ) {
        ORLeptons.push_back(truthPart);
      }

      if ( pt<5e3 )                  continue;
      if ( absEta>2.5 )                continue;
      if ( status!=1 )                 continue;
      if ( absPdg!=13 && absPdg!=11 )  continue;
      if ( decayIntoItself(truthPart) ) continue;

      if ( absPdg==13 ) {
        ATH_MSG_DEBUG( "  found a muon with: " << truthPart->pt() << " , " << truthPart->eta()
                       << " , " << truthPart->phi() << " , Status: " << truthPart->status() << "  ID: " <<  truthPart->pdgId() );
        interestingLeptons.push_back(truthPart);
        if ( pt>20e3 ) Nem_acc++;
      }

      if ( absPdg==11 ) {
        ATH_MSG_DEBUG("  found an electron with: " << truthPart->pt() << " , " << truthPart->eta() << " , " << truthPart->phi() << " , " << truthPart->barcode()
                      << " , Status: " << truthPart->status() << "  ID: " <<  truthPart->pdgId() );
        interestingLeptons.push_back(truthPart);
        if ( pt>20e3 ) Nem_acc++;
      }

    } // end loop over all truth particles

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // open the truth jet container
    const xAOD::JetContainer* truthJets = 0;
    ATH_CHECK( evtStore()->retrieve( truthJets, "AntiKt4TruthJets") );

    eventInfo->auxdecor<int>("HIGG3DX_truth_nOutgoingPartons")   = nOutgoingPartons;

    std::sort( interestingLeptons.begin(), interestingLeptons.end(), xaodPtSorting);

    for ( unsigned int iL=0; iL<interestingLeptons.size(); iL++) {
      if (  !isFromW( interestingLeptons.at(iL), leptonsFromW ) ) {
        //std::cout << " ... particle not coming from W " << std::endl;
        if ( (interestingLeptons.at(iL))->pt()>20e3 ) Nem_acc--;
      }
    }

    //// photon business
    float minDR = 999.9;
    if ( photons.size()>0 ) {
      for (unsigned int iP=0; iP<photons.size(); iP++) {
        const xAOD::TruthParticle* truthPhoton = photons.at(iP);
        ATH_MSG_DEBUG( "--> photon with: " << truthPhoton->pt()
            << " , " << truthPhoton->eta()
            << " , " << truthPhoton->phi()
            << " , Status: " << truthPhoton->status()
            << " , barcode: " << truthPhoton->barcode()
            << "  ID: " <<  truthPhoton->pdgId() << "    .... " <<  m_isSherpa );// << std::endl;

        bool found=false;
        const xAOD::TruthVertex* prodvtx = truthPhoton->prodVtx();
        ATH_MSG_DEBUG( "--> incoming particle size: " << prodvtx->nIncomingParticles());
        for (unsigned int moth=0; moth<prodvtx->nIncomingParticles(); moth++) {
          const xAOD::TruthParticle* mother= prodvtx->incomingParticle(moth);
          ATH_MSG_DEBUG( "--> lepton from W/Z decay size: " << leptonsFromWZwithTau.size());
          ATH_MSG_DEBUG( "--> mother of photon id: " << mother->pdgId()  << ", status: " << mother->status() << ", barcode: " << mother->barcode());
          if (  m_isSherpa ) {
            if (fabs(mother->pdgId()) == 11 || fabs(mother->pdgId()) == 13 || fabs(mother->pdgId()) == 15) {
              found = true;
              ATH_MSG_DEBUG( "--> found photon from lepton ");
              for ( unsigned int iL=0; iL<interestingLeptons.size(); iL++) {
                const xAOD::TruthParticle* lepAfterRad = interestingLeptons.at(iL);
                //if (fabs(mother->pdgId()) == lepAfterRad->pdgId()) {
                ATH_MSG_DEBUG( "--> Delta R lepton after rad- photon: "<< (truthPhoton->p4()).DeltaR(lepAfterRad->p4()));
                if (minDR > (truthPhoton->p4()).DeltaR(lepAfterRad->p4())) {
                  minDR = (truthPhoton->p4()).DeltaR(lepAfterRad->p4());
                }
                //}
              }
            }
          } else if (mother->pdgId()==24 ||  mother->pdgId()==23) { // for Powheg and Alpgen
            found = true;
            for (unsigned int ref=0; ref<leptonsFromWZwithTau.size(); ref++) {
              const xAOD::TruthParticle* lepFromWZ = leptonsFromWZwithTau.at(ref);
              //const xAOD::TruthVertex* lepprodvtx = lepFromWZ->prodVtx();
              ATH_MSG_DEBUG( "--> lepton with: " << lepFromWZ->pt()
                  << " , " << lepFromWZ->eta()
                  << " , " << lepFromWZ->phi()
                  << " , Status: " << lepFromWZ->status()
                  << " , barcode: " << lepFromWZ->barcode()
                  << "  ID: " <<  lepFromWZ->pdgId()  );// << std::endl;
              /*for (unsigned int lep_moth=0; lep_moth<lepprodvtx->nIncomingParticles(); lep_moth++) {
                const xAOD::TruthParticle* lep_mother= lepprodvtx->incomingParticle(lep_moth);
                ATH_MSG_DEBUG( "--> mother of lepton id: " << lep_mother->pdgId()  << ", status: " << lep_mother->status() << ", barcode: " << lep_mother->barcode());
              }*/
              if (lepFromWZ->status() == 1) {
                TLorentzVector status3lep(0,0,0,0);
                status3lep+=truthPhoton->p4();
                status3lep+=lepFromWZ->p4();
                if (minDR > (truthPhoton->p4()).DeltaR(lepFromWZ->p4())){
                  minDR = (truthPhoton->p4()).DeltaR(lepFromWZ->p4());
                }
                ATH_MSG_DEBUG( "--> lepton+photon with: " << status3lep.Pt() << ", " << status3lep.Eta() << ", " << status3lep.Phi() << ", Delta R "<< (truthPhoton->p4()).DeltaR(lepFromWZ->p4()));
              }
            }
          }
        } // end loop over mothers of photon
        if ( found ) {
          hasFSRPhoton=true;
          if(minDR > 0.1) {
            hasFSRPhotonLargeDeltaR = true;
            ATH_MSG_DEBUG( "--> found photon from lepton with DR > 0.1");
            break;
          }
        }
      } // end loop over photons
    } // end check on > 0 photons
    // end photon business

    bool isZ=false;
    WWType WWtype=NOWW;

    if ( Nz!=0 || CountSherpaLepton!=0 ) {
      isZ=true;
    } else if ( NeDirect+NmDirect+Ntau!=2 ) {
      isZ=true;
    } else {
      if ( Ntau==2 )  WWtype=TAUTAU;
      else if ( Ntau==1 )  {
        if ( NeDirect==1 && NmDirect==0 )      WWtype=TAUE;
        else if ( NeDirect==0 && NmDirect==1 ) WWtype=TAUM;
        else ATH_MSG_ERROR( "  unrecognised configuration: Ntau: " << Ntau << "   ,  Nlep: " << Nem << " (" << NeDirect << "," << NmDirect << ") , neutrinos: " << Nnu );
      } else if ( Ntau==0 &&  Nem==2 ) {
        if ( NeDirect==2 && NmDirect==0 )      WWtype=EE;
        else if ( NeDirect==0 && NmDirect==2 ) WWtype=MM;
        else if ( NeDirect==1 && NmDirect==1 ) WWtype=EM;
        else ATH_MSG_ERROR( "  unrecognised configuration: Ntau: " << Ntau << "   ,  Nlep: " << Nem << " (" << NeDirect << "," << NmDirect << ") , neutrinos: " << Nnu );
      }
    }
    ATH_MSG_DEBUG( "  option: Ntau: " << Ntau << "   ,  Nlep: " << Nem << " (" << NeDirect << "," << NmDirect << ") , neutrinos: " << Nnu << " , " <<  CountSherpaLepton << "  TYPE: " <<   WWtype << "  ... truth: " << Nem_acc << "  ... leptons from W: " << leptonsFromW.size() );

    eventInfo->auxdecor<char>("HIGG3DX_truth_isZZ")             = static_cast<char>(isZ);
    eventInfo->auxdecor<int>("HIGG3DX_truth_WWtype")            = WWtype;
    eventInfo->auxdecor<int>("HIGG3DX_truth_nTruthLep")         = Nem_acc;
    eventInfo->auxdecor<char>("HIGG3DX_truth_hasFSRPhoton")     = static_cast<char>(hasFSRPhoton);
    eventInfo->auxdecor<char>("HIGG3DX_truth_hasFSRPhotonDR01") = static_cast<char>(hasFSRPhotonLargeDeltaR);

    float Mvv=0.0;
    if ( neutrinosFromW.size()>1 ) {
      TLorentzVector myMvv(0,0,0,0);
      myMvv+=(neutrinosFromW.at(0))->p4();
      myMvv+=(neutrinosFromW.at(1))->p4();
      Mvv=myMvv.M();
    }
    eventInfo->auxdecor<float>("HIGG3DX_truth_mvv") = Mvv;

    /*
    if ( ptLoss.size()>0 ) {
      eventInfo->auxdecor<float>("HIGG3DX_truth_ptLoss_0")=ptLoss[0];
      if ( ptLoss.size()>1 ) {
        eventInfo->auxdecor<float>("HIGG3DX_truth_ptLoss_1")=ptLoss[1];
      } else {
        eventInfo->auxdecor<float>("HIGG3DX_truth_ptLoss_1")=1.;
      }
    } else {
      eventInfo->auxdecor<float>("HIGG3DX_truth_ptLoss_0")=1.;
      eventInfo->auxdecor<float>("HIGG3DX_truth_ptLoss_1")=1.;
    }
    */

    // -----------------------------------------------------
    // Calculate the di-boson invariant mass
    // -----------------------------------------------------
    float massVV = 0.0;
    std::vector<const xAOD::TruthParticle*> vBosonList;
    for ( const xAOD::TruthParticle* part : interestingParticles ){
      const int absPDG = part->absPdgId();
      if ( absPDG == 23 || absPDG==24 ){
        vBosonList.push_back(part);
      }
    }
    if ( vBosonList.size() == 2 ){
      const xAOD::TruthParticle* partA = vBosonList[0];
      const xAOD::TruthParticle* partB = vBosonList[1];
      const double diBosonMass = ( partA->p4() + partB->p4() ).M();
      massVV = static_cast<float>(diBosonMass);
    }
    eventInfo->auxdecor<float>("HIGG3DX_truth_VVMass") = massVV;


    //default
    float M01 = -999.;

    // Pere's information
    //consider events with exactly 3 leptons and 3 neutrions, each with exactly one W-boson parent (this selects 100% of events in Sherpa and VBFNLO)
    if ( (nWlep==3) && (nWnu==3) ){
      //get the information of the 3 W-bosons in the event (needed for the sorting)
      TLorentzVector tmp_Wboson0;
      TLorentzVector tmp_Wboson1;
      TLorentzVector tmp_Wboson2;
      float tmp_chargeMass0 = 0.0;
      float tmp_chargeMass1 = 0.0;
      float tmp_chargeMass2 = 0.0;

      std::map<float,TLorentzVector>::iterator it = Wbosons.begin();
      int Wb_count = 0;

      while(it != Wbosons.end()){
        if (Wb_count==0){
          tmp_Wboson0 = (*it).second;
          tmp_chargeMass0 = (*it).first;
        }
        else if (Wb_count==1){
          tmp_Wboson1 = (*it).second;
          tmp_chargeMass1 = (*it).first;
        }
        else if (Wb_count==2){
          tmp_Wboson2 = (*it).second;
          tmp_chargeMass2 = (*it).first;
        }

        Wb_count += 1;
        ++it;

      }

      float tmp_chargeMass01 = tmp_chargeMass0*tmp_chargeMass1;
      float tmp_chargeMass02 = tmp_chargeMass0*tmp_chargeMass2;
      float tmp_chargeMass12 = tmp_chargeMass1*tmp_chargeMass2;

      TLorentzVector tmp_Wboson01 = tmp_Wboson0 + tmp_Wboson1;
      TLorentzVector tmp_Wboson02 = tmp_Wboson0 + tmp_Wboson2;
      TLorentzVector tmp_Wboson12 = tmp_Wboson1 + tmp_Wboson2;
      // TODO: The higgs mass is wrong here, no? It was generated with 125.0
      float tmp_Higgs_mDiff01 = std::abs( (0.001*(tmp_Wboson01.M())) - 125.09 );
      float tmp_Higgs_mDiff02 = std::abs( (0.001*(tmp_Wboson02.M())) - 125.09 );
      float tmp_Higgs_mDiff12 = std::abs( (0.001*(tmp_Wboson12.M())) - 125.09 );

      // now sort the W-bosons: tmp_Wboson0, tmp_Wboson1, tmp_Wboson2 --> Wboson0, Wboson1, Wboson2
      TLorentzVector Wboson0;
      TLorentzVector Wboson1;
      TLorentzVector Wboson2;

      if (tmp_chargeMass01 > 0){
        Wboson0 = tmp_Wboson2;
        if(tmp_Higgs_mDiff02 < tmp_Higgs_mDiff12){
          Wboson1 = tmp_Wboson0;
          Wboson2 = tmp_Wboson1;
        }
        else{
          Wboson1 = tmp_Wboson1;
          Wboson2 = tmp_Wboson0;
        }
      }
      if (tmp_chargeMass02 > 0){
        Wboson0 = tmp_Wboson1;
        if(tmp_Higgs_mDiff01 < tmp_Higgs_mDiff12){
          Wboson1 = tmp_Wboson0;
          Wboson2 = tmp_Wboson2;
        }
        else{
          Wboson1 = tmp_Wboson2;
          Wboson2 = tmp_Wboson0;
        }

      }

      if (tmp_chargeMass12 > 0){
        Wboson0 = tmp_Wboson0;
        if(tmp_Higgs_mDiff01 < tmp_Higgs_mDiff02){
          Wboson1 = tmp_Wboson1;
          Wboson2 = tmp_Wboson2;
        }
        else{
          Wboson1 = tmp_Wboson2;
          Wboson2 = tmp_Wboson1;
        }
      }

      //now that we have sorted the 3 W-bosons, we can compute necessary things
      TLorentzVector Wboson01 = Wboson0 + Wboson1;
      TLorentzVector Wboson02 = Wboson0 + Wboson2;
      TLorentzVector Wboson12 = Wboson1 + Wboson2;

      M01 = 0.001*(Wboson01.M());

    }// if a 3Wlep + 3Wnu event
    eventInfo->auxdecor<float>("HIGG3DX_truth_mW0W1") =  M01;


    // Filter based on rapidity acceptance and sort
    std::vector<const xAOD::Jet* > filteredJets;

    /// now the jets
    unsigned int numberOfTruthJets = 0;
    for ( const auto* jet : *truthJets ) {
      const double jetPt =  jet->pt();
      // jet selection for the VBF mjj calculation
      if ( jetPt < 15e3 ) continue;
      const double absY  = std::abs(jet->rapidity());
      if ( absY < 5.0 ) {
        bool JetOverlapsWithPhoton   = false;
        bool JetOverlapsWithElectron = false;
        bool JetOverlapsWithTau      = false;

        JetOverlapsWithPhoton   = checkOverlap(jet, MCTruthPhotonList  );
        JetOverlapsWithElectron = checkOverlap(jet, MCTruthElectronList);
        JetOverlapsWithTau      = checkOverlap(jet, MCTruthTauList     );

        if (!JetOverlapsWithPhoton && !JetOverlapsWithElectron && !JetOverlapsWithTau ) {
           filteredJets.push_back( jet );
        }
      } // End of the jet selection for the VBF mjj calculation

      // Jet counting for Sherpa 2.2 Z+jets re-weighting
      if( jet->pt()>20e3 && std::abs(jet->eta())<4.5 ){
        float mindR=100;
        for (unsigned int iL=0; iL<ORLeptons.size(); iL++) {
          const xAOD::TruthParticle* lepton = ORLeptons.at(iL);
          float tmpDR=(jet->p4()).DeltaR(lepton->p4());
          if ( tmpDR<mindR ) mindR=tmpDR;
        }
        if ( mindR<0.2 ) continue;
        numberOfTruthJets++;
      }
      //
    }
    // Truth jet multiplicity for Sherpa 2.2 Z+jets re-weighting
    eventInfo->auxdecor<unsigned int>("HIGG3DX_truth_nJet") = numberOfTruthJets;

    // WZ truth jet multiplicity for Sherpa 2.2 Z+jets re-weighting
    const xAOD::JetContainer* truthWZJets = 0;
    ATH_CHECK( evtStore()->retrieve( truthWZJets, "AntiKt4TruthWZJets" ) );
    unsigned int numberOfTruthWZJets = 0;
    for ( const auto* jet : *truthWZJets ) {
      if( jet->pt()>20e3 && std::abs(jet->eta())<4.5 ){
        float mindR=100;
        for (unsigned int iL=0; iL<ORLeptons.size(); iL++) {
          const xAOD::TruthParticle* lepton = ORLeptons.at(iL);
          float tmpDR=(jet->p4()).DeltaR(lepton->p4());
          if ( tmpDR<mindR ) mindR=tmpDR;
        }
        if ( mindR<0.2 ) continue;
        numberOfTruthWZJets++;
      }
    }
    eventInfo->auxdecor<unsigned int>("HIGG3DX_truth_nWZJet") = numberOfTruthWZJets;

    // DressedWZ truth jet multiplicity for Sherpa 2.2 Z+jets re-weighting, if requested
    const xAOD::JetContainer* truthDressedWZJets = 0;
    ATH_CHECK( evtStore()->retrieve( truthDressedWZJets, "AntiKt4TruthDressedWZJets" ) );
    unsigned int numberOfTruthDressedWZJets = 0;
    for ( const auto* jet : *truthDressedWZJets ) {
      if( jet->pt()>20e3 && std::abs(jet->eta())<4.5 ){
        float mindR=100;
        for (unsigned int iL=0; iL<ORLeptons.size(); iL++) {
          const xAOD::TruthParticle* lepton = ORLeptons.at(iL);
          float tmpDR=(jet->p4()).DeltaR(lepton->p4());
          if ( tmpDR<mindR ) mindR=tmpDR;
        }
        if ( mindR<0.2 ) continue;
        numberOfTruthDressedWZJets++;
      }
    }
    eventInfo->auxdecor<unsigned int>("HIGG3DX_truth_nDressedWZJet") = numberOfTruthDressedWZJets;

    // Finally, calculate the VBF mjj
    float truth_VBFMjj = -999.0;
    if ( filteredJets.size()>=2) {
      std::sort( filteredJets.begin(), filteredJets.end(), xaodPtSorting );

      TLorentzVector DiJetSystem(filteredJets[0]->px() + filteredJets[1]->px(),
                                 filteredJets[0]->py() + filteredJets[1]->py(),
                                 filteredJets[0]->pz() + filteredJets[1]->pz(),
                                 filteredJets[0]->e()  + filteredJets[1]->e() );
      truth_VBFMjj = DiJetSystem.M();
    }
    eventInfo->auxdecor<float>("HIGG3DX_truth_VBFMjj")    = truth_VBFMjj;

    // Truth ttbar pt and top pt
    double ttbarpt = 0;
    double toppt = 0;
    if ( vTopList.size() == 2 ){
      const xAOD::TruthParticle* top1 = vTopList[0];
      const xAOD::TruthParticle* top2 = vTopList[1];
      ttbarpt = ( top1->p4() + top2->p4() ).Pt();
      if(top1->pdgId() == 6 && top2->pdgId() == -6) toppt = ( top1->p4() ).Pt();
      else if(top1->pdgId() == -6 && top2->pdgId() == 6) toppt = ( top2->p4() ).Pt();
    }
    eventInfo->auxdecor<float>("HIGG3DX_truth_ttbarpt")          = ttbarpt;
    eventInfo->auxdecor<float>("HIGG3DX_truth_toppt")            = toppt;



    //-----------------------//
    // Truth MT calculation  //
    //-----------------------//

    // first, start with using dressed four vector
    const xAOD::TruthParticleContainer* truthele = 0;
    ATH_CHECK( evtStore()->retrieve( truthele, "TruthElectrons") );
    std::vector<const xAOD::TruthParticle*> selEles;
    unsigned int numberOfTruthEle = 0;
    for(const xAOD::TruthParticle* electron : *truthele){
        ATH_MSG_DEBUG( "--> electron with: " << electron->pt()
           << " , " << electron->eta()
           << " , " << electron->phi()
           << " , Status: " << electron->status()
           << " , barcode: " << electron->barcode()
           << " , particle origin: " <<electron->auxdata<unsigned int>("classifierParticleOrigin")
           << "  ID: " <<  electron->pdgId() );// << std::endl;
        std::vector<ElementLink<xAOD::TruthParticleContainer> > parentLinks = electron->auxdata< std::vector<ElementLink<xAOD::TruthParticleContainer> > >("parentLinks");
        if (parentLinks.size() > 0) {
          if (parentLinks[0].isValid()) {
            const xAOD::TruthParticle* parent = *parentLinks[0];
            if (fabs(parent->pdgId()) <= 24 && electron->pt() > 5000.0) {
              selEles.push_back( electron );
              numberOfTruthEle++;
            }
          }
        }
    }
    ATH_MSG_DEBUG( "Electrons: " << numberOfTruthEle );

    const xAOD::TruthParticleContainer* truthmuon = 0;
    ATH_CHECK( evtStore()->retrieve( truthmuon, "TruthMuons") );
    unsigned int numberOfTruthMu = 0;
    std::vector<const xAOD::TruthParticle*> selMus;
    for(const xAOD::TruthParticle* muon : *truthmuon){
        ATH_MSG_DEBUG( "--> muon with: " << muon->pt()
           << " , " << muon->eta()
           << " , " << muon->phi()
           << " , Status: " << muon->status()
           << " , barcode: " << muon->barcode()
           << " , particle origin: " <<muon->auxdata<unsigned int>("classifierParticleOrigin")
           << "  ID: " <<  muon->pdgId() );// << std::endl;
        std::vector<ElementLink<xAOD::TruthParticleContainer> > parentLinks = muon->auxdata< std::vector<ElementLink<xAOD::TruthParticleContainer> > >("parentLinks");
        if (parentLinks.size() > 0) {
          if (parentLinks[0].isValid()) {
            const xAOD::TruthParticle* parent = *parentLinks[0];
            if (fabs(parent->pdgId()) <= 24 && muon->pt() > 5000.0) {
              selMus.push_back( muon );
              numberOfTruthMu++;
            }
          }
        }
    }
    ATH_MSG_DEBUG( "Muons: " << numberOfTruthMu );

    const xAOD::MissingETContainer* truthmets = 0;
    ATH_CHECK( evtStore()->retrieve( truthmets, "MET_Truth") );
    const xAOD::MissingET* met = (*truthmets)["NonInt"];
    ATH_MSG_DEBUG( "--> met with: " << met->met()
           << " , " << met->name());// << std::endl;
    //setup to calculate MT here
    double truthMT=-999.9;
    if (numberOfTruthMu==1 && numberOfTruthEle==1)
      truthMT=calculateMT(selEles.at(0),selMus.at(0), met);
    else if (numberOfTruthMu==2 && numberOfTruthEle==0)
      truthMT=calculateMT(selMus.at(0),selMus.at(1), met);
    else if (numberOfTruthMu==0 && numberOfTruthEle==2)
      truthMT=calculateMT(selEles.at(0),selEles.at(1), met);
    else
      ATH_MSG_DEBUG( "Either too few or too many leptons found!! Muons: " << numberOfTruthMu << " Electrons: "<< numberOfTruthEle );
    ATH_MSG_DEBUG( "Truth MT: " << truthMT );
    eventInfo->auxdecor<float>("HIGG3DX_truth_MT")    = truthMT;



    //---------------------------------------------//
    // Lepton truth flavour decorator as in Run-I  //
    //---------------------------------------------//

    // Open the input electron container
    const xAOD::ElectronContainer *inElCont(nullptr);
    ATH_CHECK( evtStore()->retrieve( inElCont, m_inElContName ) );

    //Now classify the electrons with truthFlavourTag
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

    //Now classify the muons with truthFlavourTag
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



    //-------------------------------------//
    // Determine truthOrigin of reco jets  //
    //-------------------------------------//

    // First check which W/Z bosons are from a Higgs boson decay
    ATH_MSG_DEBUG("Now going to check if we have a Higgs boson as ancestor");
    for ( const xAOD::TruthParticle* truthPart : selectedTruthWZBosons ){
      if ( selectedTruthHiggsBosons.size() == 0 ){
        wzBosonIsFromHiggs.push_back(false);
        continue;
      }
      bool foundHiggs(false);
      while ( truthPart ){
        if (truthPart->nParents() != 1 ) break;
        truthPart = truthPart->parent();
        if (truthPart->isHiggs()){
          foundHiggs = true;
          break;
        }
      }
      wzBosonIsFromHiggs.push_back(foundHiggs);
    }

    const double maxDR2 = m_maxDeltaR*m_maxDeltaR;

    // EMTopo jets

    // Open the input container
    const xAOD::JetContainer *inEMTopoJetCont(nullptr);
    ATH_CHECK( evtStore()->retrieve( inEMTopoJetCont, m_inEMTopoJetContName ) );

    for ( const xAOD::Jet* jet : *inEMTopoJetCont ) {
      // determine if there is a truth jet which best matches the reco jet
      const xAOD::Jet* bestTruthJet = nullptr;
      for ( const xAOD::Jet* truthJet : * truthJets ) {
        const double dr2 = xAOD::P4Helpers::deltaR2( *jet, *truthJet, true );
        if ( truthJet->pt() > m_truthJetPtMin ) {
          if ( dr2 >= 0.0 && dr2 < maxDR2 ) {
            bestTruthJet = truthJet;
          }
        }
      } // End: loop over truth jets

      // Now, try to figure out the truth origin
      ATH_MSG_DEBUG("Now going to try to figure out the EMTopo jet truth origin");
      int truthOrigin = 0;
      if (bestTruthJet) {
        ATH_MSG_VERBOSE("Have a truth jet with pt " << 0.001*(bestTruthJet->pt())
                        << ", eta " << bestTruthJet->eta() << ", phi " << bestTruthJet->phi());
        const double maxDeltaRSquared = 0.6*0.6;
        for ( std::size_t i=0; i < selectedTruthWZBosons.size(); ++i ) {
          const xAOD::TruthParticle* truthWZPart = selectedTruthWZBosons.at(i);
          if (!truthWZPart){
            ATH_MSG_WARNING("Got a zero pointer to a truth particle... something went wrong");
            continue;
          }
          const bool isFromHiggs = wzBosonIsFromHiggs.at(i);
          // Iterate over the children and find the best deltaR match within DeltaR=0.6
          double minDeltaR2 = maxDeltaRSquared;
          bool foundMatch = false;
          for ( std::size_t iChild=0; iChild < truthWZPart->nChildren(); ++iChild ) {
            const xAOD::TruthParticle* truthWZChild = truthWZPart->child(iChild);
            if (!truthWZChild) continue;
            const int childAbsPdgId = std::abs(truthWZChild->pdgId());
            if ( childAbsPdgId > 6 && childAbsPdgId != 21 ) continue; // we don't have a hadronic decay
            const double childPt = truthWZChild->pt();
            if ( childPt < 1000.0 ){
              ATH_MSG_DEBUG("Got a truth child with pt " << 0.001*childPt << " GeV");
              continue;
            }
            ATH_MSG_VERBOSE("Have a truth W or Z child with pt " << 0.001*childPt
                            << " GeV, eta " << truthWZChild->eta() << ", phi " << truthWZChild->phi()
                            << ", pdgID " << truthWZChild->pdgId() << ", status " << truthWZChild->status());
            const double dr2 = xAOD::P4Helpers::deltaR2( *truthWZChild, *bestTruthJet, true );
            if ( dr2 < minDeltaR2 ) {
              minDeltaR2 = dr2;
              foundMatch = true;
            }
          }
          // If we found a match
          if ( foundMatch ){
            truthOrigin = truthWZPart->pdgId() * ( isFromHiggs ? 25 : 1 );
          }
        } // End: loop over truth WZ bosons
      } // End: best truth jet found
      decTruthOrigin(*jet) = truthOrigin;
    } // End: loop over EMTopo jets

    // PFlow jets

    // Open the input container
    const xAOD::JetContainer *inPFlowJetCont(nullptr);
    ATH_CHECK( evtStore()->retrieve( inPFlowJetCont, m_inPFlowJetContName ) );

    for ( const xAOD::Jet* jet : *inPFlowJetCont ) {
      // determine if there is a truth jet which best matches the reco jet
      const xAOD::Jet* bestTruthJet = nullptr;
      for ( const xAOD::Jet* truthJet : * truthJets ) {
        const double dr2 = xAOD::P4Helpers::deltaR2( *jet, *truthJet, true );
        if ( truthJet->pt() > m_truthJetPtMin ) {
          if ( dr2 >= 0.0 && dr2 < maxDR2 ) {
            bestTruthJet = truthJet;
          }
        }
      } // End: loop over truth jets

      // Now, try to figure out the truth origin
      ATH_MSG_DEBUG("Now going to try to figure out the PFlow jet truth origin");
      int truthOrigin = 0;
      if (bestTruthJet) {
        ATH_MSG_VERBOSE("Have a truth jet with pt " << 0.001*(bestTruthJet->pt())
                        << ", eta " << bestTruthJet->eta() << ", phi " << bestTruthJet->phi());
        const double maxDeltaRSquared = 0.6*0.6;
        for ( std::size_t i=0; i < selectedTruthWZBosons.size(); ++i ) {
          const xAOD::TruthParticle* truthWZPart = selectedTruthWZBosons.at(i);
          if (!truthWZPart){
            ATH_MSG_WARNING("Got a zero pointer to a truth particle... something went wrong");
            continue;
          }
          const bool isFromHiggs = wzBosonIsFromHiggs.at(i);
          // Iterate over the children and find the best deltaR match within DeltaR=0.6
          double minDeltaR2 = maxDeltaRSquared;
          bool foundMatch = false;
          for ( std::size_t iChild=0; iChild < truthWZPart->nChildren(); ++iChild ) {
            const xAOD::TruthParticle* truthWZChild = truthWZPart->child(iChild);
            if (!truthWZChild) continue;
            const int childAbsPdgId = std::abs(truthWZChild->pdgId());
            if ( childAbsPdgId > 6 && childAbsPdgId != 21 ) continue; // we don't have a hadronic decay
            const double childPt = truthWZChild->pt();
            if ( childPt < 1000.0 ){
              ATH_MSG_DEBUG("Got a truth child with pt " << 0.001*childPt << " GeV");
              continue;
            }
            ATH_MSG_VERBOSE("Have a truth W or Z child with pt " << 0.001*childPt
                            << " GeV, eta " << truthWZChild->eta() << ", phi " << truthWZChild->phi()
                            << ", pdgID " << truthWZChild->pdgId() << ", status " << truthWZChild->status());
            const double dr2 = xAOD::P4Helpers::deltaR2( *truthWZChild, *bestTruthJet, true );
            if ( dr2 < minDeltaR2 ) {
              minDeltaR2 = dr2;
              foundMatch = true;
            }
          }
          // If we found a match
          if ( foundMatch ){
            truthOrigin = truthWZPart->pdgId() * ( isFromHiggs ? 25 : 1 );
          }
        } // End: loop over truth WZ bosons
      } // End: best truth jet found
      decTruthOrigin(*jet) = truthOrigin;
    } // End: loop over PFlow jets



    //---------------------------------------//
    // Official Vgamma/Vjet overlap removal  //
    //---------------------------------------//

    bool in_vy_overlap;
    ATH_CHECK( m_GamORTool->inOverlap(in_vy_overlap) );
    eventInfo->auxdecor<char>("HIGG3DX_inVGammaOverlap") = static_cast<char>(in_vy_overlap);

    // also store pT of photons which fall in overlap region
    // useful for varying the photon pT threshold downstream
    std::unique_ptr<std::vector<float> > photon_pts(new std::vector<float>());
    ATH_CHECK( m_GamORTool->photonPtsOutsideDr(*photon_pts.get()) );
    ATH_CHECK( evtStore()->record(std::move(photon_pts), "HIGG3DX_overlapPhoton_pTs") );

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
  bool HIGG3TruthDecorator::isFromW( const xAOD::TruthParticle* part, const std::vector<  const xAOD::TruthParticle*>& WLeptonlist) const {
    if (!part) return false;
    const xAOD::TruthParticle* tmpPart = firstOfKind(part);
    for ( unsigned int iP=0; iP<WLeptonlist.size(); ++iP) {
      if ( tmpPart==WLeptonlist.at(iP) ) return true;
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
