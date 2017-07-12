/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/Chi2LJets.h"

#include "TMinuit.h"

namespace top {

const double Chi2LJets::input_topmass = 172500;
const double Chi2LJets::input_wmass = 80300;

const double Chi2LJets::input_topsigma = 13000;
const double Chi2LJets::input_wsigma = 7000;

Chi2LJets::Chi2LJets() {
    min = new TMinuit(2);
    min->SetPrintLevel(-1);
    min->SetObjectFit(&params);
    min->SetFCN(fcn);
}

Chi2LJets::~Chi2LJets() {
    delete min;
}

bool Chi2LJets::apply(const top::Event& event) const {
    //set lepton
    if (event.m_electrons.size() == 1)
        params.lepton = event.m_electrons.at(0)->p4();

    if (event.m_muons.size() == 1)
        params.lepton = event.m_muons.at(0)->p4();

    //and met
    params.neutrino_px = event.m_met->mpx();
    params.neutrino_py = event.m_met->mpy();

    //Output
    double chi2min = 1e200;
    double topmass = 0.;
    //double wmass = 0.;

    //First loop is for the leptonic b quark
    for (xAOD::JetContainer::const_iterator j1 = event.m_jets.begin(); j1 != event.m_jets.end(); ++j1) {
        params.leptonic_b = (*j1)->p4();

        //Second loop is for one of the jets that makes a W
        for (xAOD::JetContainer::const_iterator j2 = event.m_jets.begin(); j2 != event.m_jets.end(); ++j2) {
            if (j1 == j2)
                continue;

            ///The other jet in the W
            for (xAOD::JetContainer::const_iterator j3 = event.m_jets.begin(); j3 != event.m_jets.end(); ++j3) {
                if (j1 == j3 || j2 == j3)
                    continue;

                params.hadronic_w_mass = ((*j2)->p4() + (*j3)->p4()).M();

                ///The hadronic b quark
                for (xAOD::JetContainer::const_iterator j4 = event.m_jets.begin(); j4 != event.m_jets.end(); ++j4) {
                    if (j1 == j4 || j2 == j4 || j3 == j4)
                        continue;

                    params.hadronic_t_mass = ((*j1)->p4() + (*j2)->p4() + (*j3)->p4()).M();

                    //Set input parameters to something reasonable (a guess)
                    min->DefineParameter(0, "top_mass", 172500., 10000., 0, 500000);         //starting guess, step, min, max
                    min->DefineParameter(1, "neutrino_pz", 10000, 10000, -1000000, 1000000);

                    //Perform the minimisation
                    int status = min->Migrad();

                    double chi2 = 0;
                    double edm = 0;
                    double errdef = 0;
                    int nvpar = 0;
                    int nparx = 0;
                    int icstat = 0;

                    min->mnstat(chi2, edm, errdef, nvpar, nparx, icstat);

                    if (status == 0 && chi2 < chi2min) {
                        double error = 0;
                        chi2min = chi2;
                        min->GetParameter(0, topmass, error);
                        //wmass = params.hadronic_w_mass;
                    } //min chi2
                } //j4 = hadronic b
            } //j3 = j3 + j2 = hadronic W
        } //j2
    } //j1 = leptonic b




    //
    //  577     MiniElectronContainer electrons = event.electrons();
    //  578     if (electrons.size() == 1)
    //  579         params.lepton.SetPtEtaPhiE(electrons[0]->pt(), electrons[0]->eta(), electrons[0]->phi(), electrons[0]->E());
    //  580
    //  581     MiniMuonContainer muons = event.muons();
    //  582     if (muons.size() == 1)
    //  583         params.lepton.SetPtEtaPhiE(muons[0]->pt(), muons[0]->eta(), muons[0]->phi(), muons[0]->E());
    //  584
    //  585     //set met
    //  586     const MiniMET* met = event.MET();
    //  587     params.neutrino_px = met->et()*cos(met->phi());
    //  588     params.neutrino_py = met->et()*sin(met->phi());
    //  589
    //  590     //Out
    //  591     double chi2, edm, errdef;
    //  592     int nvpar, nparx, icstat;
    //  593
    //  594     double chi2min = 1e200;
    //  595     double topmass = 0.;
    //  596     double wmass = 0.;
    //  597
    //  598     //TObjects have a significant creation overhead so move them out of the loop
    //  599     TLorentzVector a;
    //  600     TLorentzVector b;
    //  601     TLorentzVector c;
    //  602
    //  603     const MiniJetContainer jets = event.jets();
    //  604
    //  605     //checks
    //  606     assert((electrons.size() == 1 && muons.empty()) || (muons.size() == 1 && electrons.empty()));
    //  607     assert(jets.size() >= 4);
    //  608
    //  609     //First loop is for the leptonic b quark
    //  610     for (MiniJetContainer::const_iterator j1 = jets.begin(); j1 !=jets.end(); ++j1) {
    //  611
    //  612         params.leptonic_b.SetPtEtaPhiE((*j1)->pt(), (*j1)->eta(), (*j1)->phi(), (*j1)->E());
    //  613
    //  614         //Second loop is for one of the jets that makes a W
    //  615         for (MiniJetContainer::const_iterator j2 = jets.begin(); j2 != jets.end(); ++j2) {
    //  616             if (j1 == j2)
    //  617                 continue;
    //  618
    //  619             a.SetPtEtaPhiE((*j2)->pt(), (*j2)->eta(), (*j2)->phi(), (*j2)->E());
    //  620
    //  621             ///The other jet in the W
    //  622             for (MiniJetContainer::const_iterator j3 = jets.begin(); j3 != jets.end(); ++j3) {
    //  623                 if (j1 == j3 || j2 == j3)
    //  624                     continue;
    //  625
    //  626                 b.SetPtEtaPhiE((*j3)->pt(), (*j3)->eta(), (*j3)->phi(), (*j3)->E());
    //  627                 params.hadronic_w_mass = (a + b).M();
    //  628
    //  629                 ///The hadronic b quark
    //  630                 for (MiniJetContainer::const_iterator j4 = jets.begin(); j4 != jets.end(); ++j4) {
    //  631                     if (j1 == j4 || j2 == j4 || j3 == j4)
    //  632                         continue;
    //  633
    //  634                     c.SetPtEtaPhiE((*j4)->pt(), (*j4)->eta(), (*j4)->phi(), (*j4)->E());
    //  635                     params.hadronic_t_mass = (a + b + c).M();
    //  636
    //  637                     //Set input parameters to something reasonable (a guess)
    //  638                     min->DefineParameter(0, "top_mass", 172500., 10000., 0, 500000);         //starting guess, step, min, max
    //  639                     min->DefineParameter(1, "neutrino_pz", 10000, 10000, -1000000, 1000000);
    //  640
    //  641                     //Perform the minimisation
    //  642                     int status = min->Migrad();
    //  643                     min->mnstat(chi2, edm, errdef, nvpar, nparx, icstat);
    //  644
    //  645                     if (status == 0 && chi2 < chi2min) {
    //  646                         double error;
    //  647                         chi2min = chi2;
    //  648                         min->GetParameter(0, topmass, error);
    //  649                         wmass = params.hadronic_w_mass;
    //  650                     } //min chi2
    //  651
    //  652                 } //j4 = hadronic b
    //  653             } //j3 = j3 + j2 = hadronic W
    //  654         } //j2
    //  655     } //j1 = leptonic b

    return true;
}

}


 /**
  * @brief this is where we write out chi2
  */
void fcn(int& /*npar*/, double* /*deriv*/, double& result, double par[], int /*flag*/) {
    const top::FitInfo* params = (top::FitInfo*) gMinuit->GetObjectFit();

    double mytopmass = par[0];
    double pz = par[1];

    TLorentzVector neutrino(params->neutrino_px, params->neutrino_py, pz, sqrt(params->neutrino_px * params->neutrino_px + params->neutrino_py * params->neutrino_py + pz * pz));
    double w_lep_term = ((params->lepton + neutrino).M() - top::Chi2LJets::input_wmass)  / top::Chi2LJets::input_wsigma;
    double t_lep_term = ((params->lepton + neutrino + params->leptonic_b).M() - mytopmass) / top::Chi2LJets::input_topsigma;

    double w_term = (params->hadronic_w_mass - top::Chi2LJets::input_wmass) / top::Chi2LJets::input_wsigma;
    double t_term = (params->hadronic_t_mass - mytopmass) / top::Chi2LJets::input_topsigma;

    result = w_lep_term * w_lep_term +
            t_lep_term * t_lep_term +
            w_term * w_term +
            t_term * t_term;

    //  cout << "SJH     fcn hadw " << fit->hadronicW << " " << TopWidth::input_wmass << " " << TopWidth::input_wsigma << endl;
    //  cout << "SJH     fcn hadt " << fit->hadronicTop << " " << mytopmass << " " << TopWidth::input_topsigma << endl;
}
