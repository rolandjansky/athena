/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EfficiencyScaleFactor.cxx
 *
 *  Created on: Apr 10, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include <TRandom3.h>

namespace CP {

    EfficiencyScaleFactor::EfficiencyScaleFactor():
        m_sf(0),
        m_eff(0),
        m_sf_sys(0),
        m_eff_sys(0),
        m_sf_replicas(),
        m_eff_replicas(),
        m_etaphi(),
        m_sys(){
    }

    EfficiencyScaleFactor::EfficiencyScaleFactor(std::string file, std::string time_unit, SystematicSet sys):
            m_sf(0),
            m_eff(0),
            m_sf_sys(0),
            m_eff_sys(0),
            m_sf_replicas(),
            m_eff_replicas(),
            m_etaphi(),
            m_sys(sys){

        ReadFromFile(file, time_unit);
    }


    bool EfficiencyScaleFactor::ReadFromFile(std::string file, std::string time_unit){

        // open the file
        TDirectory* dir = gDirectory;
        TFile* f = TFile::Open(file.c_str(),"READ");
        if (!f){
            Error("EfficiencyScaleFactor","Unable to open file %s",file.c_str());
            return false;
        }
        dir->cd();

        // now we can read our four Histos
        m_eff = ReadHistFromFile("Eff",f,time_unit);
        m_sf = ReadHistFromFile("SF",f,time_unit);
        m_sf_sys = ReadHistFromFile("SF_sys",f,time_unit);
        m_eff_sys = ReadHistFromFile("Eff_sys",f,time_unit);

        // and don't forget to close the file again!
        f->Close();

        return true;
    }

    HistHandler* EfficiencyScaleFactor::ReadHistFromFile(std::string name, TFile* f, std::string time_unit){
        TH1* histHolder = 0;
        f->GetObject((name+std::string("_")+time_unit).c_str(),histHolder);
        if (!histHolder){
            Warning("EfficiencyScaleFactor","Unable to read hist %s from file %s",(name+std::string("_")+time_unit).c_str(),f->GetName());
            return 0;
        }
        // replace the histos by clones so that we can close the files again
        return package_histo((TH1*)(histHolder->Clone(Form("%s_%s_%s%s",name.c_str(),f->GetName(),time_unit.c_str(),sysname().c_str()))));
    }


    bool EfficiencyScaleFactor::AddPeriod (std::string file, std::string time_unit, float weight ){

        // open the file
        TDirectory* dir = gDirectory;
        TFile* f = TFile::Open(file.c_str(),"READ");
        if (!f){
            Error("EfficiencyScaleFactor","Unable to open file %s",file.c_str());
            return false;
        }
        dir->cd();
        HistHandler* toadd = 0;

        // add the new contributions
        toadd = ReadHistFromFile("Eff",f,time_unit);
        if (toadd) {
            AddHistos(m_eff,toadd,weight);
            delete toadd;
        }

        toadd = ReadHistFromFile("SF",f,time_unit);
        if (toadd){
            AddHistos(m_sf,toadd,weight);
            delete toadd;
        }

        toadd = ReadHistFromFile("SF_sys",f,time_unit);
        if (toadd){
            AddHistos(m_sf_sys,toadd,weight);
            delete toadd;
        }

        toadd = ReadHistFromFile("Eff_sys",f,time_unit);
        if (toadd){
            AddHistos(m_eff_sys,toadd,weight);
            delete toadd;
        }

        // and close the file
        f->Close();
        return true;
    }
    
    HistHandler *EfficiencyScaleFactor::package_histo (TH1* h){
        // make sure that the correct type of histo is used
        if (dynamic_cast<TH1F*>(h)){
            return new HistHandler_TH1F(dynamic_cast<TH1F*>(h));
        }
        if (dynamic_cast<TH2F*>(h)){
            return new HistHandler_TH2F(dynamic_cast<TH2F*>(h));
        }
        if (dynamic_cast<TH3F*>(h)){
            return new HistHandler_TH3F(dynamic_cast<TH3F*>(h));
        }
        if (dynamic_cast<TH2Poly*>(h)){
            return new HistHandler_TH2Poly(dynamic_cast<TH2Poly*>(h));
        }
        return 0;
    }
    std::string EfficiencyScaleFactor::sysname(void){

        // utility method - we make sure that the different histos have separate names by adding a string produced in here
    	if (m_sys == SystematicSet()) return "central";
    	return m_sys.begin()->name();
        return "";

    }

    bool EfficiencyScaleFactor::Add ( EfficiencyScaleFactor & to_add, float weight ){

        HistHandler* htoadd = 0;

        htoadd = to_add.get_sf();
        if (htoadd) AddHistos(m_sf,htoadd,weight);
        else return false;

        htoadd = to_add.get_eff();
        if (htoadd) AddHistos(m_eff,htoadd,weight);
        else return false;

        htoadd = to_add.get_sf_sys();
        if (htoadd) AddHistos(m_sf_sys,htoadd,weight);
        else return false;

        htoadd = to_add.get_eff_sys();
        if (htoadd) AddHistos(m_eff_sys,htoadd,weight);
        else return false;

        return true;
    }

    bool EfficiencyScaleFactor::Scale (float scale ){
        ScaleHisto(m_sf,scale);
        ScaleHisto(m_eff,scale);
        ScaleHisto(m_sf_sys,scale);
        ScaleHisto(m_eff_sys,scale);

        return true;
    }

    CorrectionCode EfficiencyScaleFactor::ScaleFactor(const xAOD::Muon& mu, float & SF){
       int bin = -1;
       CorrectionCode cc = m_sf->FindBin(mu,bin);
       if (cc != CorrectionCode::Ok) {
           return cc;
       }
       else {
           SF = m_sf->GetBinContent(bin);
           return CorrectionCode::Ok;
       }
    }

    CorrectionCode EfficiencyScaleFactor::Efficiency(const xAOD::Muon& mu, float & Eff){
        int bin = -1;
        CorrectionCode cc = m_eff->FindBin(mu,bin);
        if (cc != CorrectionCode::Ok) return cc;
        else {
            Eff = m_eff->GetBinContent(bin);
            return CorrectionCode::Ok;
        }
    }

    CorrectionCode EfficiencyScaleFactor::ScaleFactorReplicas(const xAOD::Muon& mu, std::vector<float> & SF){

        if (m_sf_replicas.size() != SF.size()){
            GenerateReplicas(SF.size(),1000. * mu.phi()+mu.eta());
        }
        int bin = -1;
        if (m_sf_replicas.size()== 0 ){
            return CorrectionCode::OutOfValidityRange;
        }
        CorrectionCode res = m_sf_replicas.at(0)->FindBin(mu, bin);
        if (res != CorrectionCode::Ok) return res;
        else {
            for (size_t k = 0; k < SF.size();k++){
                SF[k] = m_sf_replicas.at(k)->GetBinContent(bin);
            }
        }
        return res;
    }
    CorrectionCode EfficiencyScaleFactor::EfficiencyReplicas(const xAOD::Muon& mu, std::vector<float> & eff){
        if (m_eff_replicas.size() != eff.size()){
            GenerateReplicas(eff.size(),1000. * mu.phi()+mu.eta());
        }
        int bin = -1;
        if (m_eff_replicas.size()== 0 ){
            return CorrectionCode::OutOfValidityRange;
        }
        CorrectionCode res = m_eff_replicas.at(0)->FindBin(mu, bin);
        if (res != CorrectionCode::Ok) return res;
        else {
            for (size_t k = 0; k < eff.size();k++){
                eff[k] = m_eff_replicas.at(k)->GetBinContent(bin);
            }
        }
        return res;
    }

    EfficiencyScaleFactor::~EfficiencyScaleFactor(){

       if (m_sf) delete m_sf;
       if (m_eff) delete m_eff;
       if (m_sf_sys) delete m_sf_sys;
       if (m_eff_sys) delete m_eff_sys;

       for (iSFvec h = m_eff_replicas.begin(); h != m_eff_replicas.end();++h) {
           if (*h) delete *h;
       }
       m_eff_replicas.clear();

       for (iSFvec h = m_sf_replicas.begin(); h != m_sf_replicas.end();++h) {
           if (*h) delete *h;
       }
       m_sf_replicas.clear();
    }

    void EfficiencyScaleFactor::AddHistos (HistHandler* & add_to, HistHandler* add_this, float weight){
    	if (!add_to){
    		add_to = package_histo(static_cast<TH1*>(add_this->GetHist()->Clone(
                Form("%s_localclone",add_this->GetHist()->GetName()))));
    		ScaleHisto(add_to,weight);
    		return;
    	}
        for (int t = 0; t < add_to->NBins(); t++){
            double binc = add_to->GetBinContent(t);
            double addc = weight * add_this->GetBinContent(t);
            double err = add_to->GetBinError(t);
            double adderr = weight * add_this->GetBinError(t);
            add_to->SetBinContent(t, binc + addc);
            add_to->SetBinError(t, sqrt(err*err+adderr*adderr));
        }
    }
    void EfficiencyScaleFactor::ScaleHisto (HistHandler* & h, float weight){
        if (!h) return;
        for (int t = 0; t < h->NBins(); t++){
            double binc = h->GetBinContent(t);
            double err = h->GetBinError(t);
            h->SetBinContent(t, weight * binc);
            h->SetBinError(t, weight * err);
        }
    }
    void EfficiencyScaleFactor::GenerateReplicas(int nrep, int seed){

        // clean up and roll new replicas

        for (iSFvec h = m_eff_replicas.begin(); h != m_eff_replicas.end();++h) {
            if (*h) delete *h;
        }
        m_eff_replicas = GenerateReplicasFromHist(m_eff,nrep,seed);

        for (iSFvec h = m_sf_replicas.begin(); h != m_sf_replicas.end();++h) {
            if (*h) delete *h;
        }
        m_sf_replicas = GenerateReplicasFromHist(m_sf,nrep,seed);

    }

    EfficiencyScaleFactor::SFvec EfficiencyScaleFactor::GenerateReplicasFromHist(HistHandler* h, int nrep, int seed){

        TRandom3 Rndm(seed);
        SFvec replicas(0);
        int nbins = h->NBins();
        for (int t = 0; t < nrep; t++){
            HistHandler* replica = package_histo((TH1*)h->GetHist()->Clone(
                        Form("%s_rep%d",h->GetHist()->GetName(),t)));
            for (int bin = 0; bin < nbins;bin++ ){
                replica->SetBinContent(bin,
                        Rndm.Gaus(h->GetBinContent(bin), h->GetBinError(bin)));
            }
            replicas.push_back(replica);
        }
        return replicas;
    }


    void EfficiencyScaleFactor::ApplySysVariation(void) {

    	if (m_sys == SystematicSet()) return;
	    if (*(m_sys.begin()) == SystematicVariation ("MUONSFSTAT", -1)) AddStatErrors(-1.);
	    if (*(m_sys.begin()) == SystematicVariation ("MUONSFSTAT", 1)) AddStatErrors(1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUONSFSYS", -1)) AddSysErrors(-1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUONSFSYS", 1)) AddSysErrors(1.);

    }
	void EfficiencyScaleFactor::AddStatErrors (float weight){

    	AddStatErrors_histo(m_sf,weight);
        AddStatErrors_histo(m_eff,weight);

	}

	void EfficiencyScaleFactor::AddSysErrors (float weight){

	    AddSysErrors_histo(m_sf,m_sf_sys,weight);
	    AddSysErrors_histo(m_eff,m_eff_sys,weight);

	    // in case there are replicas, also update those
	    for (iSFvec h = m_sf_replicas.begin();h!=m_sf_replicas.end();++h){
	        AddSysErrors_histo(*h,m_sf_sys,weight);
	    }
        for (iSFvec h = m_eff_replicas.begin();h!=m_eff_replicas.end();++h){
            AddSysErrors_histo(*h,m_eff_sys,weight);
        }

	}
    void EfficiencyScaleFactor::AddSysErrors_histo (HistHandler* h, HistHandler *hsys,float weight){
        for (int t = 0; t < h->NBins(); ++t){
            double binc = h->GetBinContent(t);
            binc += weight * hsys->GetBinContent(t);
            h->SetBinContent(t, binc);
        }
    }
    void EfficiencyScaleFactor::AddStatErrors_histo (HistHandler* h, float weight){
        for (int t = 0; t < h->NBins(); t++){
            double binc = h->GetBinContent(t);
            binc += weight * h->GetBinError(t);
            h->SetBinContent(t, binc);
        }
    }

    void EfficiencyScaleFactor::DebugPrint (void){
        m_eff->GetHist()->Print();
        m_sf->GetHist()->Print();
        m_eff_sys->GetHist()->Print();
        m_sf_sys->GetHist()->Print();
    }



} /* namespace CP */
