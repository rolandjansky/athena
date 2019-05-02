/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkHiggs/HIGG3TruthDecorator.h"

// EDM includes
#include "xAODEventInfo/EventInfo.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace DerivationFramework {

  // Constructor
  DerivationFramework::HIGG3TruthDecorator::HIGG3TruthDecorator(const std::string& t,
                                            const std::string& n,
                                            const IInterface* p) : 
    AthAlgTool(t, n, p)
  {

    declareInterface<DerivationFramework::IAugmentationTool>(this);

    declareProperty( "InputElectronContainerName", m_inElContName  = "Electrons" );
  }

  // Destructor
  DerivationFramework::HIGG3TruthDecorator::~HIGG3TruthDecorator() {
  }

  StatusCode HIGG3TruthDecorator::initialize() {
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

    // Define the decorators outside of the loop as a static, such that they
    // will be fully cached
    static SG::AuxElement::Decorator<int>  decFlavourTag ("flavourTag");

    //---------------------------------------------//
    // Lepton truth flavour decorator as in Run-I  //
    //---------------------------------------------//

    // Open the input electron container
    const xAOD::ElectronContainer *inElCont(0);
    ATH_CHECK( evtStore()->retrieve( inElCont, m_inElContName ) );

    for ( const xAOD::Electron* electron : *inElCont) {
      int flavTag = -1;

      // Store b, c, s and light truth objects (quarks and hadrons)
      const xAOD::TruthParticleContainer* truthParticles = 0;
      ATH_CHECK(evtStore()->retrieve( truthParticles, "TruthParticles" ));
      xAOD::TruthParticleContainer::const_iterator truthItr = truthParticles->begin();
      xAOD::TruthParticleContainer::const_iterator truthItrE = truthParticles->end();

      std::vector<const xAOD::TruthParticle*> bTruth; bTruth.clear();
      std::vector<const xAOD::TruthParticle*> cTruth; cTruth.clear();
      std::vector<const xAOD::TruthParticle*> sTruth; sTruth.clear();
      std::vector<const xAOD::TruthParticle*> lTruth; lTruth.clear();
      std::vector<const xAOD::TruthParticle*> eTruth; eTruth.clear();

      for (truthItr = truthParticles->begin(); truthItr != truthItrE; ++truthItr) {
        const xAOD::TruthParticle * part = (*truthItr); long int pdg = part->pdgId();
        if (part->pt() <= 5000) continue;

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

        if (hasBottom) bTruth.push_back(part);
        if (hasCharm) cTruth.push_back(part);
        if (hasStrange) sTruth.push_back(part);
        if (hasLight) lTruth.push_back(part);
        if (fabs(pdg) == 11) eTruth.push_back(part);
      }

      //Now classify the electron
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

    return StatusCode::SUCCESS;
  }

}
