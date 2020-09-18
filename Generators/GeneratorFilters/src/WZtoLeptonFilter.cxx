/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/PhysicalConstants.h"
#include "GeneratorFilters/WZtoLeptonFilter.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "GaudiKernel/ObjectList.h"
#include "AtlasHepMC/Relatives.h"

WZtoLeptonFilter::WZtoLeptonFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter( name,pSvcLocator ),
    m_AthenaCalls(0), m_tot_wghts(0.), m_wPass(0.)
{
  // Event number the user expect to persist
  declareProperty( "PrintLeptonsCounters", m_tesit = 0 );

  // Total number of leptons expected directly from W/Z decay
  declareProperty( "BCKGvsSIGNAL", m_signal = 1 );
  declareProperty( "GenerateSurvival", m_survival = 1 );

  // Decay mode of tau, true for full/total branches, false for leptonic.
  declareProperty( "ElectronMuonNumber", m_Nlepton = 2 );
  declareProperty( "NeedWZleps", m_wzlepton = 0 );
  declareProperty( "IdealReconstructionEfficiency", m_reconeffi = 1 );
  declareProperty( "SameElectroCharge", m_samesign = 0 );

  // Eta & pT cuts, tau appear as electron/muon or jet.
  // No eta limit by default
  declareProperty( "Etacut_electron", m_Eta_e = 20. );
  declareProperty( "Etacut_muon", m_Eta_mu = 20. );
  // No pT limit actually
  declareProperty( "Ptcut_electron", m_Pt_e = 0.01 );
  declareProperty( "Ptcut_muon", m_Pt_mu = 0.01 );
  for (int i = 0; i < 45; i++) {
    m_catevti[i]=0.;
    m_catevtf[i]=0.;
  }
}


StatusCode WZtoLeptonFilter::filterInitialize() {
  m_AthenaCalls = 0;
  m_tot_wghts = 0;
  m_wPass = 0;
  for (int cnt = 0; cnt < 45; ++cnt) {
    m_catevti[ cnt ] = 0;
    m_catevtf[ cnt ] = 0;
  }
  return StatusCode::SUCCESS;
}


StatusCode WZtoLeptonFilter::filterFinalize() {
  double catsum[2] = { 0., 0. };
  for (int cnt = 0; cnt < 40; ++cnt) {
    catsum[0] += m_catevti[cnt];
    catsum[1] += m_catevtf[cnt];
  }
  catsum[0] /= m_tot_wghts;
  catsum[1] /= m_tot_wghts;

  ATH_MSG_INFO("###############  WZtoLeptonFilter Statistics  #############"         <<
               "\n"                                                                  <<
               "For your following filtering parameters:" << "\n"                    <<
               "===============================================" << "\n"             <<
               "1 to print some leptonic information : " <<  m_tesit << "\n"         <<
               "Signal or background to filter : " << m_signal << "\n"               <<
               "0 to give what can't pass the filter : " << m_survival << "\n"       <<
               "Number of Lepton( electron or muon) : " << m_Nlepton << "\n"         <<
               "1 for >= lepton from W/Z boson : " << m_wzlepton << "\n"             <<
               "1 for signlike if TWO leptons : " << m_samesign << "\n"              <<
               "0 for more leptons ( difficult to recon ) : " << m_reconeffi << "\n" <<
               "Etacut_electron : " << m_Eta_e << "\n"                               <<
               "Etacut_muon     : " << m_Eta_mu << "\n"                              <<
               "Ptcut_electron (GeV) : " << m_Pt_e/Gaudi::Units::GeV << "\n"                       <<
               "Ptcut_muon           : " << m_Pt_mu/Gaudi::Units::GeV);

  if (m_AthenaCalls == 0) {
    ATH_MSG_INFO(" ERROR, Your filter is not interfaced/called at all " << "################   WZtoLeptonFilter Finished  ##############");
  } else {
    ATH_MSG_INFO("Before filtering you have " << "\n"
                 << "    " << m_AthenaCalls << "   events  in   40   catalogs : " << "\n"
                 << "negative weight : "<< 1 - m_tot_wghts/(1.0*m_AthenaCalls) <<"\n"
                 << "W = directly from W/Z boson, w = W + kinematical cuts,"<< "\n"
                 << "b = from B (or other) hadrons  + kinematical cuts " << "\n"
                 << "..................................................." << "\n"
                 << "W=0, w=0, b>=3 : " << m_catevti[ 0 ]/m_tot_wghts << "\n"
                 << "W=0, w=0, b=2  : " << m_catevti[ 1 ]/m_tot_wghts << "\n"
                 << "W=0, w=0, b=1  : " << m_catevti[ 2 ]/m_tot_wghts << "\n"
                 << "W=0, w=0, b=0  : " << m_catevti[ 3 ]/m_tot_wghts << "\n"

                 << "W=1, w=0, b>=3 : " << m_catevti[ 4 ]/m_tot_wghts << "\n"
                 << "W=1, w=0, b=2  : " << m_catevti[ 5 ]/m_tot_wghts << "\n"
                 << "W=1, w=0, b=1  : " << m_catevti[ 6 ]/m_tot_wghts << "\n"
                 << "W=1, w=0, b=0  : " << m_catevti[ 7 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b>=3 : " << m_catevti[ 8 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b=2  : " << m_catevti[ 9 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b=1  : " << m_catevti[ 10 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b=0  : " << m_catevti[ 11 ]/m_tot_wghts << "\n"

                 << "W=2, w=0, b>=3 : " << m_catevti[ 12 ]/m_tot_wghts << "\n"
                 << "W=2, w=0, b=2  : " << m_catevti[ 13 ]/m_tot_wghts << "\n"
                 << "W=2, w=0, b=1  : " << m_catevti[ 14 ]/m_tot_wghts << "\n"
                 << "W=2, w=0, b=0  : " << m_catevti[ 15 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b>=3 : " << m_catevti[ 16 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b=2  : " << m_catevti[ 17 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b=1  : " << m_catevti[ 18 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b=0  : " << m_catevti[ 19 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b>=3 : " << m_catevti[ 20 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b=2  : " << m_catevti[ 21 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b=1  : " << m_catevti[ 22 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b=0  : " << m_catevti[ 23 ]/m_tot_wghts << "\n"

                 << "W>=3, w=0, b>=3 : " << m_catevti[ 24 ]/m_tot_wghts << "\n"
                 << "W>=3, w=0, b=2  : " << m_catevti[ 25 ]/m_tot_wghts << "\n"
                 << "W>=3, w=0, b=1  : " << m_catevti[ 26 ]/m_tot_wghts << "\n"
                 << "W>=3, w=0, b=0  : " << m_catevti[ 27 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b>=3 : " << m_catevti[ 28 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b=2  : " << m_catevti[ 29 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b=1  : " << m_catevti[ 30 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b=0  : " << m_catevti[ 31 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b>=3 : " << m_catevti[ 32 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b=2  : " << m_catevti[ 33 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b=1  : " << m_catevti[ 34 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b=0  : " << m_catevti[ 35 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b>=3 : " << m_catevti[ 36 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b=2  : " << m_catevti[ 37 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b=1  : " << m_catevti[ 38 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b=0  : " << m_catevti[ 39 ]/m_tot_wghts << "\n"
                 << "................................         " << "\n"
                 << "The sum over all 40 catalog == " << catsum[0] << "\n"
                 << "including entirely hadronic  : " << m_catevti[ 40 ]/m_tot_wghts << "\n"
                 << "..............................................  " << "\n"
                 << "................................         " << "\n"
                 << "After the filtering you have " << "\n"
                 << "     "<< m_nPass << "  events persisted :  " << "\n"
                 << " Filtering efficiency = "
                 << m_nPass/(double)(m_AthenaCalls)
                 << " = " << m_nPass << "/" << m_AthenaCalls
                 << " (Accepted/Generated) " << "\n"
                 << "...................                " << "\n"
                 << " by event weight it is " << "\n"
                 << "    "<< m_wPass << "   net weight passed :  " << "\n"
                 << " Weighted Filtering efficiency = "
                 << m_wPass/m_tot_wghts
                 << " = " << m_wPass << "/" << m_tot_wghts  << "\n"
                 << "...................                " << "\n"
                 << "   They distribute among the following catalogs : " << "\n"
                 << "...................................  " << "\n"
                 << "W=0, w=0, b>=3 : " << m_catevtf[ 0 ]/m_tot_wghts << "\n"
                 << "W=0, w=0, b=2  : " << m_catevtf[ 1 ]/m_tot_wghts << "\n"
                 << "W=0, w=0, b=1  : " << m_catevtf[ 2 ]/m_tot_wghts << "\n"
                 << "W=0, w=0, b=0  : " << m_catevtf[ 3 ]/m_tot_wghts << "\n"

                 << "W=1, w=0, b>=3 : " << m_catevtf[ 4 ]/m_tot_wghts << "\n"
                 << "W=1, w=0, b=2  : " << m_catevtf[ 5 ]/m_tot_wghts << "\n"
                 << "W=1, w=0, b=1  : " << m_catevtf[ 6 ]/m_tot_wghts << "\n"
                 << "W=1, w=0, b=0  : " << m_catevtf[ 7 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b>=3 : " << m_catevtf[ 8 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b=2  : " << m_catevtf[ 9 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b=1  : " << m_catevtf[ 10 ]/m_tot_wghts << "\n"
                 << "W=1, w=1, b=0  : " << m_catevtf[ 11 ]/m_tot_wghts << "\n"

                 << "W=2, w=0, b>=3 : " << m_catevtf[ 12 ]/m_tot_wghts << "\n"
                 << "W=2, w=0, b=2  : " << m_catevtf[ 13 ]/m_tot_wghts << "\n"
                 << "W=2, w=0, b=1  : " << m_catevtf[ 14 ]/m_tot_wghts << "\n"
                 << "W=2, w=0, b=0  : " << m_catevtf[ 15 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b>=3 : " << m_catevtf[ 16 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b=2  : " << m_catevtf[ 17 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b=1  : " << m_catevtf[ 18 ]/m_tot_wghts << "\n"
                 << "W=2, w=1, b=0  : " << m_catevtf[ 19 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b>=3 : " << m_catevtf[ 20 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b=2  : " << m_catevtf[ 21 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b=1  : " << m_catevtf[ 22 ]/m_tot_wghts << "\n"
                 << "W=2, w=2, b=0  : " << m_catevtf[ 23 ]/m_tot_wghts << "\n"

                 << "W>=3, w=0, b>=3 : " << m_catevtf[ 24 ]/m_tot_wghts << "\n"
                 << "W>=3, w=0, b=2  : " << m_catevtf[ 25 ]/m_tot_wghts << "\n"
                 << "W>=3, w=0, b=1  : " << m_catevtf[ 26 ]/m_tot_wghts << "\n"
                 << "W>=3, w=0, b=0  : " << m_catevtf[ 27 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b>=3 : " << m_catevtf[ 28 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b=2  : " << m_catevtf[ 29 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b=1  : " << m_catevtf[ 30 ]/m_tot_wghts << "\n"
                 << "W>=3, w=1, b=0  : " << m_catevtf[ 31 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b>=3 : " << m_catevtf[ 32 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b=2  : " << m_catevtf[ 33 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b=1  : " << m_catevtf[ 34 ]/m_tot_wghts << "\n"
                 << "W>=3, w=2, b=0  : " << m_catevtf[ 35 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b>=3 : " << m_catevtf[ 36 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b=2  : " << m_catevtf[ 37 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b=1  : " << m_catevtf[ 38 ]/m_tot_wghts << "\n"
                 << "W>=3, w>=3, b=0  : " << m_catevtf[ 39 ]/m_tot_wghts << "\n"
                 << "................................         " << "\n"
                 << "The sum over all 40 catalog == " << catsum[1] << "\n"
                 << "................................         " << "\n"
                 << "################   WZtoLeptonFilter Finished  ##############"
                 );
  }
  return StatusCode::SUCCESS;
}


StatusCode WZtoLeptonFilter::filterEvent() {

  // Momentum of the products of the tau decay
  CLHEP::HepLorentzVector mom_hadrons;

  // Lepton charge
  int posilep, negalep, chargesame, chargeopposite;

  // Lepton CounTer
  int lepid, abslepid, passfilter, etronCT, muonCT, taulep;
  int iWL, iwl, iwls, iBL, ibl, leps, nullvertex;

  // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {
    m_AthenaCalls ++;

    passfilter = 0;
    etronCT = 0;
    muonCT = 0;
    taulep = 0;
    leps = 0;
    iwls = 0;

    posilep = 0;
    negalep = 0;

    iWL = 0;
    iBL = 0;
    iwl = 0;
    ibl = 0;

    nullvertex = 0;

    const HepMC::GenEvent* genEvt = (*itr);
    auto wgtsC = genEvt->weights();
    double wght = 1;
    if (wgtsC.size() > 0) wght = wgtsC[0];
    m_tot_wghts += wght;

#ifdef HEPMC3
HepMC::ConstGenVertexPtr LePrdVrt;
HepMC::ConstGenVertexPtr TauPrdVrt;
    for ( auto pitr: *genEvt) {
      LePrdVrt = 0;
      TauPrdVrt = 0;
      lepid =  pitr->pdg_id();
      abslepid = std::abs( lepid );

      if ((abslepid != 11 && abslepid != 13) || pitr->status() != 1) continue;
        double leppt = pitr->momentum().perp();
        double lepeta = std::abs( pitr->momentum().pseudoRapidity() );

        LePrdVrt = pitr->production_vertex();
        int anceWZ = 0;
        int momWZ = 0;
        int taufromWZ = 0;

        if (LePrdVrt != NULL) {
          int ancecnt = 0;
          for (auto lepanc: HepMC::ancestor_particles(LePrdVrt) ) {
            int ancepid = lepanc->pdg_id();
            int ancestatus = lepanc->status();
            if ( m_tesit == 1 ) {
              ATH_MSG_DEBUG("lepton=" << lepid << "  " << ancecnt <<
                            "'th  ancestors=" << ancepid << "          status =" << ancestatus);
            }
            if ( std::abs(ancepid) == 24 || std::abs(ancepid) == 23 )  anceWZ ++;
            ancecnt ++;
          }  // end of lepton ancestors( mother, grandmother ... ) test

          
          int momcnt = 0;
          taufromWZ = 0;
          for (auto  lepanc: LePrdVrt->particles_in() ) {
            int mompid =  lepanc->pdg_id();
            int momstatus =  lepanc->status();
            if ( m_tesit == 1 ) {
              ATH_MSG_DEBUG(momcnt << "'th mom with pid= " << mompid << "                mom status = " << momstatus);
            }
            if ( std::abs(mompid) ==15 ) {
              TauPrdVrt = lepanc->production_vertex();
              for (auto taumom: TauPrdVrt->particles_in() ) {
                int wzpdg = taumom->pdg_id();
                if (std::abs(wzpdg) == 24 || std::abs(wzpdg) == 23 || (wzpdg == mompid && anceWZ > 0)) taufromWZ++;
                if ( m_tesit == 1 ) ATH_MSG_DEBUG("tau mother =" << wzpdg);
              }
            }

            if (std::abs(mompid)==24 || std::abs(mompid)==23 || (anceWZ > 0 && mompid==lepid) || (std::abs(mompid) == 15 && taufromWZ > 0)) momWZ++;
          }  // end of lepton mother test loop

          if ( momWZ > 0 && anceWZ > 0 ) iWL++;
          else iBL++;

          if ( ( abslepid == 11 && leppt >= m_Pt_e && lepeta <= m_Eta_e ) ||( abslepid == 13 && leppt >= m_Pt_mu && lepeta <= m_Eta_mu ) ) {
            if ( m_tesit == 1 ) ATH_MSG_DEBUG("Phase space OK");
            if ( momWZ > 0 && anceWZ > 0 ) {
              iwl++;
              if (m_signal != 1) iwls++;
              if ( taufromWZ > 0 ) taulep++;
              else {
                if ( abslepid == 11 ) etronCT++; else muonCT++;
              }
            } else {
              ibl++;
              if ( m_signal != 1 ) {
                if ( abslepid == 11 ) etronCT ++;
                else muonCT ++;
              }
            }
            if ( lepid > 0 ) posilep ++;
            else negalep ++;
          }

          if (m_tesit == 1) {
            ATH_MSG_DEBUG("iWL=" << iWL << " iwl=" << iwl << " iBL=" << iBL << " ibl=" << ibl);
            if ( iWL == 0 && iBL == 0 ) ATH_MSG_WARNING("Check !!! Unexpected filter LEAKAGE !");
          }
        } else {  // I've prayed for the upstream generators to give less chaos/duplications
          nullvertex ++;
          if ( m_tesit == 1 ) ATH_MSG_DEBUG(" NULL production vertex is met !");
          if ( m_signal != 1 ) {
            if ( ( abslepid == 11 && leppt >= m_Pt_e && lepeta <= m_Eta_e ) ||( abslepid == 13 && leppt >= m_Pt_mu && lepeta <= m_Eta_mu ) ) {
              if ( m_tesit == 1 ) ATH_MSG_DEBUG("Phase space OK, NULL prod-vertex");
              if ( abslepid == 11 ) etronCT ++;
              else muonCT ++;

              if ( lepid > 0 ) posilep ++;
              else negalep ++;
            }  //  anyway  need for kinematical requirements
          }   //  only work for conservation for backgrounds
        }  //  End if for muon/electron mom vertex test
    }    //  end of all mc_particles
#else
   HepMC::GenVertex* LePrdVrt;
   HepMC::GenVertex* TauPrdVrt;
    for ( HepMC::GenEvent::particle_const_iterator pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {
      LePrdVrt = 0;
      TauPrdVrt = 0;
      lepid =  (*pitr)->pdg_id();
      abslepid = abs( lepid );

      if ((abslepid == 11 || abslepid == 13) && (*pitr)->status() == 1) {
        double leppt = (*pitr)->momentum().perp();
        double lepeta = std::abs( (*pitr)->momentum().pseudoRapidity() );

        LePrdVrt = (*pitr)->production_vertex();
        int anceWZ = 0;
        int momWZ = 0;
        int taufromWZ = 0;

        if (LePrdVrt != NULL) {
          HepMC::GenVertex::particle_iterator fsrtanc, lstanc;
          fsrtanc = LePrdVrt->particles_begin(HepMC::ancestors);
          lstanc = LePrdVrt->particles_end(HepMC::ancestors);
          HepMC::GenVertex::particle_iterator lepanc = fsrtanc;
          int ancecnt = 0;
          for (; lepanc != lstanc; ++lepanc ) {
            int ancepid = (*lepanc)->pdg_id();
            int ancestatus = (*lepanc)->status();
            if ( m_tesit == 1 ) {
              ATH_MSG_DEBUG("lepton=" << lepid << "  " << ancecnt <<
                            "'th  ancestors=" << ancepid << "          status =" << ancestatus);
            }
            if ( std::abs(ancepid) == 24 || std::abs(ancepid) == 23 )  anceWZ ++;
            ancecnt ++;
          }  // end of lepton ancestors( mother, grandmother ... ) test

          fsrtanc = LePrdVrt->particles_begin(HepMC::parents);
          lstanc = LePrdVrt->particles_end(HepMC::parents);
          lepanc = fsrtanc;
          int momcnt = 0;
          taufromWZ = 0;
          for (; lepanc != lstanc; ++lepanc ) {
            int mompid =  (*lepanc)->pdg_id();
            int momstatus =  (*lepanc)->status();
            if ( m_tesit == 1 ) {
              ATH_MSG_DEBUG(momcnt << "'th mom with pid= " << mompid << "                mom status = " << momstatus);
            }
            if ( std::abs(mompid) ==15 ) {
              TauPrdVrt = (*lepanc)->production_vertex();
              HepMC::GenVertex::particle_iterator fstaum, lstaum;
              fstaum = TauPrdVrt->particles_begin(HepMC::parents);
              lstaum = TauPrdVrt->particles_end(HepMC::parents);
              HepMC::GenVertex::particle_iterator taumom = fstaum;
              for (; taumom != lstaum; ++taumom ) {
                int wzpdg = (*taumom)->pdg_id();
                if (std::abs(wzpdg) == 24 || std::abs(wzpdg) == 23 || (wzpdg == mompid && anceWZ > 0)) taufromWZ++;
                if ( m_tesit == 1 ) ATH_MSG_DEBUG("tau mother =" << wzpdg);
              }
            }

            if (std::abs(mompid)==24 || std::abs(mompid)==23 || (anceWZ > 0 && mompid==lepid) || (std::abs(mompid) == 15 && taufromWZ > 0)) momWZ++;
          }  // end of lepton mother test loop

          if ( momWZ > 0 && anceWZ > 0 ) iWL++;
          else iBL++;

          if ( ( abslepid == 11 && leppt >= m_Pt_e && lepeta <= m_Eta_e ) ||( abslepid == 13 && leppt >= m_Pt_mu && lepeta <= m_Eta_mu ) ) {
            if ( m_tesit == 1 ) ATH_MSG_DEBUG("Phase space OK");
            if ( momWZ > 0 && anceWZ > 0 ) {
              iwl++;
              if (m_signal != 1) iwls++;
              if ( taufromWZ > 0 ) taulep++;
              else {
                if ( abslepid == 11 ) etronCT++; else muonCT++;
              }
            } else {
              ibl++;
              if ( m_signal != 1 ) {
                if ( abslepid == 11 ) etronCT ++;
                else muonCT ++;
              }
            }
            if ( lepid > 0 ) posilep ++;
            else negalep ++;
          }

          if (m_tesit == 1) {
            ATH_MSG_DEBUG("iWL=" << iWL << " iwl=" << iwl << " iBL=" << iBL << " ibl=" << ibl);
            if ( iWL == 0 && iBL == 0 ) ATH_MSG_WARNING("Check !!! Unexpected filter LEAKAGE !");
          }
        } else {  // I've prayed for the upstream generators to give less chaos/duplications
          nullvertex ++;
          if ( m_tesit == 1 ) ATH_MSG_DEBUG(" NULL production vertex is met !");
          if ( m_signal != 1 ) {
            if ( ( abslepid == 11 && leppt >= m_Pt_e && lepeta <= m_Eta_e ) ||( abslepid == 13 && leppt >= m_Pt_mu && lepeta <= m_Eta_mu ) ) {
              if ( m_tesit == 1 ) ATH_MSG_DEBUG("Phase space OK, NULL prod-vertex");
              if ( abslepid == 11 ) etronCT ++;
              else muonCT ++;

              if ( lepid > 0 ) posilep ++;
              else negalep ++;
            }  //  anyway  need for kinematical requirements
          }   //  only work for conservation for backgrounds
        }  //  End if for muon/electron mom vertex test
      }   //  end of leptonic pid
    }    //  end of all mc_particles
#endif

    leps = (m_signal == 1) ? etronCT + muonCT : etronCT + muonCT + taulep;

    // Define some counters for event catalogs
    if ( iWL == 0 && iwl == 0 && ibl >= 3 ) m_catevti[0] += wght;
    if ( iWL == 0 && iwl == 0 && ibl == 2 ) m_catevti[1] += wght;
    if ( iWL == 0 && iwl == 0 && ibl == 1 ) m_catevti[2] += wght;
    if ( iWL == 0 && iwl == 0 && ibl == 0 ) m_catevti[3] += wght;

    if ( iWL == 1 && iwl == 0 && ibl >= 3 ) m_catevti[4] += wght;
    if ( iWL == 1 && iwl == 0 && ibl == 2 ) m_catevti[5] += wght;
    if ( iWL == 1 && iwl == 0 && ibl == 1 ) m_catevti[6] += wght;
    if ( iWL == 1 && iwl == 0 && ibl == 0 ) m_catevti[7] += wght;
    if ( iWL == 1 && iwl == 1 && ibl >= 3 ) m_catevti[8] += wght;
    if ( iWL == 1 && iwl == 1 && ibl == 2 ) m_catevti[9] += wght;
    if ( iWL == 1 && iwl == 1 && ibl == 1 ) m_catevti[10] += wght;
    if ( iWL == 1 && iwl == 1 && ibl == 0 ) m_catevti[11] += wght;

    if ( iWL == 2 && iwl == 0 && ibl >= 3 ) m_catevti[12] += wght;
    if ( iWL == 2 && iwl == 0 && ibl == 2 ) m_catevti[13] += wght;
    if ( iWL == 2 && iwl == 0 && ibl == 1 ) m_catevti[14] += wght;
    if ( iWL == 2 && iwl == 0 && ibl == 0 ) m_catevti[15] += wght;
    if ( iWL == 2 && iwl == 1 && ibl >= 3 ) m_catevti[16] += wght;
    if ( iWL == 2 && iwl == 1 && ibl == 2 ) m_catevti[17] += wght;
    if ( iWL == 2 && iwl == 1 && ibl == 1 ) m_catevti[18] += wght;
    if ( iWL == 2 && iwl == 1 && ibl == 0 ) m_catevti[19] += wght;
    if ( iWL == 2 && iwl == 2 && ibl >= 3 ) m_catevti[20] += wght;
    if ( iWL == 2 && iwl == 2 && ibl == 2 ) m_catevti[21] += wght;
    if ( iWL == 2 && iwl == 2 && ibl == 1 ) m_catevti[22] += wght;
    if ( iWL == 2 && iwl == 2 && ibl == 0 ) m_catevti[23] += wght;

    if ( iWL >= 3 && iwl == 0 && ibl >= 3 ) m_catevti[24] += wght;
    if ( iWL >= 3 && iwl == 0 && ibl == 2 ) m_catevti[25] += wght;
    if ( iWL >= 3 && iwl == 0 && ibl == 1 ) m_catevti[26] += wght;
    if ( iWL >= 3 && iwl == 0 && ibl == 0 ) m_catevti[27] += wght;
    if ( iWL >= 3 && iwl == 1 && ibl >= 3 ) m_catevti[28] += wght;
    if ( iWL >= 3 && iwl == 1 && ibl == 2 ) m_catevti[29] += wght;
    if ( iWL >= 3 && iwl == 1 && ibl == 1 ) m_catevti[30] += wght;
    if ( iWL >= 3 && iwl == 1 && ibl == 0 ) m_catevti[31] += wght;
    if ( iWL >= 3 && iwl == 2 && ibl >= 3 ) m_catevti[32] += wght;
    if ( iWL >= 3 && iwl == 2 && ibl == 2 ) m_catevti[33] += wght;
    if ( iWL >= 3 && iwl == 2 && ibl == 1 ) m_catevti[34] += wght;
    if ( iWL >= 3 && iwl == 2 && ibl == 0 ) m_catevti[35] += wght;
    if ( iWL >= 3 && iwl >= 3 && ibl >= 3 ) m_catevti[36] += wght;
    if ( iWL >= 3 && iwl >= 3 && ibl == 2 ) m_catevti[37] += wght;
    if ( iWL >= 3 && iwl >= 3 && ibl == 1 ) m_catevti[38] += wght;
    if ( iWL >= 3 && iwl >= 3 && ibl == 0 ) m_catevti[39] += wght;
    if ( iWL ==0 && iBL == 0 ) m_catevti[40] += wght;

    // Total number of charged leptons no matter their mothers

    if (( posilep == 2  && negalep == 0 ) || ( posilep == 0  && negalep == 2 )) {
      chargesame = 1;
      chargeopposite = 0;
    } else if (posilep == 1 && negalep == 1) {
      chargesame = 0;
      chargeopposite = 1;
    } else {
      chargesame = 0;
      chargeopposite = 0;
    }

    if (m_tesit == 1) ATH_MSG_DEBUG("charge : " << chargesame <<"  "<< chargeopposite);

    // The logics are troublesome/complex, careful !!!
    /// @todo Wow... yuck. Would love to rewrite this, but...
    if ((m_reconeffi == 0
         && (    (   nullvertex == 0
                     && ibl >= ( m_Nlepton - iwl )
                     )
                 || nullvertex > 0 )
         && (   ( m_wzlepton == 0 && ( iwl - 1 ) <= m_Nlepton )
                || ( m_wzlepton == 1 && iwls >= 1 )
                || ( m_wzlepton == -1 && iwls == 0 )
                )
         && (   (   leps > m_Nlepton
                    && (    leps != 2
                            || (  leps == 2
                                  && (   m_samesign == 0
                                         || ( m_samesign == 1 && chargesame == 1 )
                                         || ( m_samesign == -1 && chargeopposite == 1 )
                                         )
                                  )
                            )
                    )
                || (   leps == m_Nlepton
                       && (   m_Nlepton != 2
                              || (   m_Nlepton == 2
                                     && (   m_samesign == 0
                                            || ( m_samesign == 1 && chargesame == 1 )
                                            || ( m_samesign == -1 && chargeopposite == 1 )
                                            )
                                     )
                              )
                       )
                )
         )
        || (   ( m_reconeffi == 1 && leps == m_Nlepton )
               && (   m_wzlepton == 0
                      || ( m_wzlepton == 1 && iwls >= 1 )
                      || ( m_wzlepton == -1 && iwl == 0 )
                      || ( m_wzlepton == -2 && ibl == 0 )
                      )
               && (    m_Nlepton != 2
                       || (  m_Nlepton == 2
                             && (   m_samesign == 0
                                    || ( m_samesign == 1 && chargesame == 1 )
                                    || ( m_samesign == -1 && chargeopposite == 1 )
                                       )
                             )
                       )
               )
        ) passfilter = 1;

    if ( ( m_survival == 1 && passfilter == 1 ) || ( m_survival == 0 && passfilter == 0 ) ) {
      if ( m_tesit == 1 ) {
        ATH_MSG_DEBUG("LEPS="<<etronCT<<" "<<muonCT);
        ATH_MSG_DEBUG("chrgs="<<posilep<<" "<<negalep);
      }

      if ( iWL == 0 && iwl == 0 && ibl >= 3 ) m_catevtf[0] += wght;
      if ( iWL == 0 && iwl == 0 && ibl == 2 ) m_catevtf[1] += wght;
      if ( iWL == 0 && iwl == 0 && ibl == 1 ) m_catevtf[2] += wght;
      if ( iWL == 0 && iwl == 0 && ibl == 0 ) m_catevtf[3] += wght;

      if ( iWL == 1 && iwl == 0 && ibl >= 3 ) m_catevtf[4] += wght;
      if ( iWL == 1 && iwl == 0 && ibl == 2 ) m_catevtf[5] += wght;
      if ( iWL == 1 && iwl == 0 && ibl == 1 ) m_catevtf[6] += wght;
      if ( iWL == 1 && iwl == 0 && ibl == 0 ) m_catevtf[7] += wght;
      if ( iWL == 1 && iwl == 1 && ibl >= 3 ) m_catevtf[8] += wght;
      if ( iWL == 1 && iwl == 1 && ibl == 2 ) m_catevtf[9] += wght;
      if ( iWL == 1 && iwl == 1 && ibl == 1 ) m_catevtf[10] += wght;
      if ( iWL == 1 && iwl == 1 && ibl == 0 ) m_catevtf[11] += wght;

      if ( iWL == 2 && iwl == 0 && ibl >= 3 ) m_catevtf[12] += wght;
      if ( iWL == 2 && iwl == 0 && ibl == 2 ) m_catevtf[13] += wght;
      if ( iWL == 2 && iwl == 0 && ibl == 1 ) m_catevtf[14] += wght;
      if ( iWL == 2 && iwl == 0 && ibl == 0 ) m_catevtf[15] += wght;
      if ( iWL == 2 && iwl == 1 && ibl >= 3 ) m_catevtf[16] += wght;
      if ( iWL == 2 && iwl == 1 && ibl == 2 ) m_catevtf[17] += wght;
      if ( iWL == 2 && iwl == 1 && ibl == 1 ) m_catevtf[18] += wght;
      if ( iWL == 2 && iwl == 1 && ibl == 0 ) m_catevtf[19] += wght;
      if ( iWL == 2 && iwl == 2 && ibl >= 3 ) m_catevtf[20] += wght;
      if ( iWL == 2 && iwl == 2 && ibl == 2 ) m_catevtf[21] += wght;
      if ( iWL == 2 && iwl == 2 && ibl == 1 ) m_catevtf[22] += wght;
      if ( iWL == 2 && iwl == 2 && ibl == 0 ) m_catevtf[23] += wght;

      if ( iWL >= 3 && iwl == 0 && ibl >= 3 ) m_catevtf[24] += wght;
      if ( iWL >= 3 && iwl == 0 && ibl == 2 ) m_catevtf[25] += wght;
      if ( iWL >= 3 && iwl == 0 && ibl == 1 ) m_catevtf[26] += wght;
      if ( iWL >= 3 && iwl == 0 && ibl == 0 ) m_catevtf[27] += wght;
      if ( iWL >= 3 && iwl == 1 && ibl >= 3 ) m_catevtf[28] += wght;
      if ( iWL >= 3 && iwl == 1 && ibl == 2 ) m_catevtf[29] += wght;
      if ( iWL >= 3 && iwl == 1 && ibl == 1 ) m_catevtf[30] += wght;
      if ( iWL >= 3 && iwl == 1 && ibl == 0 ) m_catevtf[31] += wght;
      if ( iWL >= 3 && iwl == 2 && ibl >= 3 ) m_catevtf[32] += wght;
      if ( iWL >= 3 && iwl == 2 && ibl == 2 ) m_catevtf[33] += wght;
      if ( iWL >= 3 && iwl == 2 && ibl == 1 ) m_catevtf[34] += wght;
      if ( iWL >= 3 && iwl == 2 && ibl == 0 ) m_catevtf[35] += wght;
      if ( iWL >= 3 && iwl >= 3 && ibl >= 3 ) m_catevtf[36] += wght;
      if ( iWL >= 3 && iwl >= 3 && ibl == 2 ) m_catevtf[37] += wght;
      if ( iWL >= 3 && iwl >= 3 && ibl == 1 ) m_catevtf[38] += wght;
      if ( iWL >= 3 && iwl >= 3 && ibl == 0 ) m_catevtf[39] += wght;
      if ( iWL ==0  && iBL == 0 ) m_catevtf[40] += wght;

      setFilterPassed(true);
      m_wPass += wght;
      return StatusCode::SUCCESS;
    }
  } //  End for loop of events

  setFilterPassed(false);
  return StatusCode::SUCCESS;
}
