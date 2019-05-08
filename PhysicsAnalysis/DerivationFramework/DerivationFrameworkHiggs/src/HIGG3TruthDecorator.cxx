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
#include "CxxUtils/fpcompare.h"

bool xaodPtSorting(const xAOD::IParticle *part1,
                   const xAOD::IParticle *part2) {
  return CxxUtils::fpcompare::greater( part1->pt(), part2->pt() );
}

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
    int nWlep=0;
    int nWnu=0;
    int nWparents=0;
    std::map<float,TLorentzVector> Wbosons;

    //// this is for the OR removal between V+jets and V+gamma samples
    bool hasFSRPhoton=false;
    bool hasFSRPhotonLargeDeltaR = false;
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
          m_neutrinosFromW.push_back( truthPart );
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
                m_neutrinosFromW.push_back( lep2 );
                Nnu++;
              } else if ( std::abs(PDGtau)==11 || std::abs(PDGtau)==13 ) {
                m_leptonsFromW.push_back(lep2);
              }
            }
          } // end of tau block
          if ( absPdg==11 || absPdg==13 ) {
            m_leptonsFromW.push_back(truthPart);
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
          ATH_MSG_VERBOSE("Found a W/Z not decaying in at least 2 particles ... " << truthPart->status() << " and barcode: " << truthPart->barcode() );
          continue;
        }

        if ( absPdg==23 ) Nz++;

        ATH_MSG_DEBUG( "  found a boson with: " << truthPart->pt() << " , " << eta
                       << " , " << truthPart->phi() << " , Status: " << truthPart->status() << "  ID: " <<  truthPart->pdgId() );// << std::endl;

        m_interestingParticles.push_back( truthPart );
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
                m_neutrinosFromW.push_back( lep2 );
                Nnu++;
              } else if ( std::abs(PDGtau)==11 || std::abs(PDGtau)==13 ) {
                m_leptonsFromW.push_back(lep2);
              }
            }
          } // end of tau block
          if ( std::abs(PDGlep)==11 || std::abs(PDGlep)==13 ) {
            ATH_MSG_DEBUG( "     found a boson direct decay with: " << lepton->pt() << " , " << lepton->eta() << " , " << lepton->phi()
                           << " , Status: " << lepton->status() << "  ID: " <<  lepton->pdgId() << "  barcode: " << lepton->barcode() );
            //printRecursively( decvtx->outgoingParticle(part), "  ");
            Nem++;
            m_leptonsFromW.push_back( lepton );
            //if ( std::abs(PDGlep)==13 ) {
            //  const xAOD::TruthParticle* tmpPart2=lastOfKind(decvtx->outgoingParticle(part));
            //  float tmpPtLoss= (tmpPart2->pt()-decvtx->outgoingParticle(part)->pt())/decvtx->outgoingParticle(part)->pt();
            //  ptLoss.push_back(tmpPtLoss);
            //}
            if ( std::abs(PDGlep)==11 )  NeDirect++;
            if ( std::abs(PDGlep)==13 )  NmDirect++;
          }
          if ( std::abs(PDGlep)==12 || std::abs(PDGlep)==14 || std::abs(PDGlep)==16 ) {
            m_neutrinosFromW.push_back( lepton );
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
          m_interestingParticles.push_back( truthPart );
        }
      }
      if ( absPdg==25 && truthPart->status() == 62 ) {
        higgsBosons.push_back(truthPart);
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
        m_interestingLeptons.push_back(truthPart);
        if ( pt>20e3 ) Nem_acc++;
      }

      if ( absPdg==11 ) {
        ATH_MSG_DEBUG("  found an electron with: " << truthPart->pt() << " , " << truthPart->eta() << " , " << truthPart->phi() << " , " << truthPart->barcode()
                      << " , Status: " << truthPart->status() << "  ID: " <<  truthPart->pdgId() );
        m_interestingLeptons.push_back(truthPart);
        if ( pt>20e3 ) Nem_acc++;
      }

    } // end loop over all truth particles

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::sort( m_interestingLeptons.begin(), m_interestingLeptons.end(), xaodPtSorting);

    for ( unsigned int iL=0; iL<m_interestingLeptons.size(); iL++) {
      if (  !isFromW( m_interestingLeptons.at(iL), m_leptonsFromW ) ) {
        //std::cout << " ... particle not coming from W " << std::endl;
        if ( (m_interestingLeptons.at(iL))->pt()>20e3 ) Nem_acc--;
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
              for ( unsigned int iL=0; iL<m_interestingLeptons.size(); iL++) {
                const xAOD::TruthParticle* lepAfterRad = m_interestingLeptons.at(iL);
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
    ATH_MSG_DEBUG( "  option: Ntau: " << Ntau << "   ,  Nlep: " << Nem << " (" << NeDirect << "," << NmDirect << ") , neutrinos: " << Nnu << " , " <<  CountSherpaLepton << "  TYPE: " <<   WWtype << "  ... truth: " << Nem_acc << "  ... leptons from W: " << m_leptonsFromW.size() );

    eventInfo->auxdecor<char>("truth_isZZ")             = static_cast<char>(isZ);
    eventInfo->auxdecor<int>("truth_WWtype")            = WWtype;
    eventInfo->auxdecor<int>("truth_nTruthLep")         = Nem_acc;
    eventInfo->auxdecor<char>("truth_hasFSRPhoton")     = static_cast<char>(hasFSRPhoton);
    eventInfo->auxdecor<char>("truth_hasFSRPhotonDR01") = static_cast<char>(hasFSRPhotonLargeDeltaR);

















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
