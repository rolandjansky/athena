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
#include <TClass.h>
namespace CP {

EfficiencyScaleFactor::EfficiencyScaleFactor():
                                m_sf(0),
                                m_eff(0),
                                m_sf_sys(0),
                                m_eff_sys(0),
                                m_sf_PtDepsys(0),
                                m_eff_PtDepsys(0),
                                m_sf_replicas(),
                                m_eff_replicas(),
                                m_etaphi(),
                                m_sys(),
                                m_effType("EFF"),
                                m_is_lowpt(false),
                                m_respond_to_PtDepsys(false),
                                m_PtDepsys_weight(0.){
}

EfficiencyScaleFactor::EfficiencyScaleFactor(std::string file, std::string time_unit, SystematicSet sys, std::string effType, bool isLowPt, bool hasPtDepSys):
                                    m_sf(0),
                                    m_eff(0),
                                    m_sf_sys(0),
                                    m_eff_sys(0),
                                    m_sf_PtDepsys(0),
                                    m_eff_PtDepsys(0),
                                    m_sf_replicas(),
                                    m_eff_replicas(),
                                    m_etaphi(),
                                    m_sys(sys),
                                    m_effType(effType),
                                    m_is_lowpt(isLowPt),
                                    m_respond_to_PtDepsys(hasPtDepSys),
                                    m_PtDepsys_weight(0.){

    ReadFromFile(file, time_unit);
}


EfficiencyScaleFactor::EfficiencyScaleFactor(const EfficiencyScaleFactor & other)
{
    m_sf = package_histo(dynamic_cast<TH1*>(other.m_sf->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_sf->GetHist()->GetName()).c_str())));
    m_eff = package_histo(dynamic_cast<TH1*>(other.m_eff->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_eff->GetHist()->GetName()).c_str())));
    m_sf_sys = package_histo(dynamic_cast<TH1*>(other.m_sf_sys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_sf_sys->GetHist()->GetName()).c_str())));
    m_eff_sys = package_histo(dynamic_cast<TH1*>(other.m_eff_sys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_eff_sys->GetHist()->GetName()).c_str())));
    m_sys = other.m_sys;
    m_effType = other.m_effType;
    m_is_lowpt = other.m_is_lowpt;
    m_respond_to_PtDepsys = other.m_respond_to_PtDepsys;
    m_PtDepsys_weight = other.m_PtDepsys_weight;

    if (other.m_respond_to_PtDepsys){
        m_sf_PtDepsys = package_histo(dynamic_cast<TH1*>(other.m_sf_PtDepsys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_sf_PtDepsys->GetHist()->GetName()).c_str())));
        m_eff_PtDepsys = package_histo(dynamic_cast<TH1*>(other.m_eff_PtDepsys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_eff_PtDepsys->GetHist()->GetName()).c_str())));
    }

    for (ciSFvec h = other.m_sf_replicas.begin(); h != other.m_sf_replicas.end();++h) {
        m_sf_replicas.push_back(package_histo(dynamic_cast<TH1*>((*h)->GetHist()->Clone((std::string("EffSFsfReplicaCloneOf")+(*h)->GetHist()->GetName()).c_str()))));
    }
    for (ciSFvec h = other.m_eff_replicas.begin(); h != other.m_eff_replicas.end();++h) {
        m_eff_replicas.push_back(package_histo(dynamic_cast<TH1*>((*h)->GetHist()->Clone((std::string("EffSFeffReplicaCloneOf")+(*h)->GetHist()->GetName()).c_str()))));
    }
}
EfficiencyScaleFactor & EfficiencyScaleFactor::operator = (const EfficiencyScaleFactor & other){

    if (this == &other){
        return *this;               
    }

    delete m_sf;
    delete m_eff;
    delete m_eff_sys;
    delete m_sf_sys;
    if (m_sf_PtDepsys) delete m_sf_PtDepsys;
    if (m_eff_PtDepsys) delete m_eff_PtDepsys;

    for (iSFvec h = m_sf_replicas.begin(); h != m_sf_replicas.end();++h) {
        if (*h) delete *h;
    }
    m_sf_replicas.clear();


    for (iSFvec h = m_eff_replicas.begin(); h != m_eff_replicas.end();++h) {
        if (*h) delete *h;
    }
    m_eff_replicas.clear();

    m_sf = package_histo(dynamic_cast<TH1*>(other.m_sf->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_sf->GetHist()->GetName()).c_str())));
    m_eff = package_histo(dynamic_cast<TH1*>(other.m_eff->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_eff->GetHist()->GetName()).c_str())));
    m_sf_sys = package_histo(dynamic_cast<TH1*>(other.m_sf_sys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_sf_sys->GetHist()->GetName()).c_str())));
    m_eff_sys = package_histo(dynamic_cast<TH1*>(other.m_eff_sys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_eff_sys->GetHist()->GetName()).c_str())));
    if (other.m_respond_to_PtDepsys){
        m_sf_PtDepsys = package_histo(dynamic_cast<TH1*>(other.m_sf_PtDepsys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_sf_PtDepsys->GetHist()->GetName()).c_str())));
        m_eff_PtDepsys = package_histo(dynamic_cast<TH1*>(other.m_eff_PtDepsys->GetHist()->Clone((std::string("EffSFCloneOf")+other.m_eff_PtDepsys->GetHist()->GetName()).c_str())));
    }
    m_sys = other.m_sys;
    m_effType = other.m_effType;
    m_is_lowpt = other.m_is_lowpt;
    m_respond_to_PtDepsys = other.m_respond_to_PtDepsys;
    m_PtDepsys_weight = other.m_PtDepsys_weight;


    for (ciSFvec h = other.m_sf_replicas.begin(); h != other.m_sf_replicas.end();++h) {
        m_sf_replicas.push_back(package_histo(dynamic_cast<TH1*>((*h)->GetHist()->Clone((std::string("EffSFsfReplicaCloneOf")+(*h)->GetHist()->GetName()).c_str()))));
    }
    for (ciSFvec h = other.m_eff_replicas.begin(); h != other.m_eff_replicas.end();++h) {
        m_eff_replicas.push_back(package_histo(dynamic_cast<TH1*>((*h)->GetHist()->Clone((std::string("EffSFeffReplicaCloneOf")+(*h)->GetHist()->GetName()).c_str()))));
    }

    return *this;
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
    // for high pt eff, we also load the pt dependent part
    if (m_respond_to_PtDepsys){
        m_sf_PtDepsys = ReadHistFromFile("SF_PtDep_sys",f,time_unit);
        m_eff_PtDepsys = ReadHistFromFile("Eff_PtDep_sys",f,time_unit);
    }

    // and don't forget to close the file again!
    f->Close();
    return true;
}

bool EfficiencyScaleFactor::CheckConsistency(){

    bool OK = (m_eff_sys != NULL && m_eff != NULL && m_sf_sys != NULL && m_sf != NULL && (!m_respond_to_PtDepsys || (m_sf_PtDepsys != NULL && m_eff_PtDepsys != NULL)));
    return OK;
}

HistHandler* EfficiencyScaleFactor::ReadHistFromFile(std::string name, TFile* f, std::string time_unit){
    TH1* histHolder = 0;
    f->GetObject((name+std::string("_")+time_unit).c_str(),histHolder);
    if (!histHolder){
        // if no period weighting, the histo may also not have a time period in the name at all
        if (time_unit == "All"){    
            f->GetObject(name.c_str(),histHolder);
        }
        if (! histHolder){
            Error("EfficiencyScaleFactor","Unable to read hist %s from file %s. Something wrong with the inputs! ",(name+std::string("_")+time_unit).c_str(),f->GetName());
            return 0;
        }
    }
    // replace the histos by clones so that we can close the files again
    HistHandler* out =  package_histo((TH1*)(histHolder->Clone(Form("%s_%s_%s%s",name.c_str(),f->GetName(),time_unit.c_str(),sysname().c_str()))));
    return out;
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

    if (m_respond_to_PtDepsys){
        toadd = ReadHistFromFile("Eff_PtDep_sys",f,time_unit);
        if (toadd){
            AddHistos(m_eff_PtDepsys,toadd,weight);
            delete toadd;
        }
        toadd = ReadHistFromFile("SF_PtDep_sys",f,time_unit);
        if (toadd){
            AddHistos(m_sf_PtDepsys,toadd,weight);
            delete toadd;
        }
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
    else if (dynamic_cast<TH2F*>(h)){
        return new HistHandler_TH2F(dynamic_cast<TH2F*>(h));
    }
    else if (dynamic_cast<TH3F*>(h)){
        return new HistHandler_TH3F(dynamic_cast<TH3F*>(h));
    }
    else if (dynamic_cast<TH2Poly*>(h)){
        return new HistHandler_TH2Poly(dynamic_cast<TH2Poly*>(h));
    }
    else {
        Error("EfficiencyScaleFactor","Unable to package histo %s (%s) in a known HistHandler",h->GetName(),h->IsA()->GetName());
        return 0;
    }
}
std::string EfficiencyScaleFactor::sysname(void){

    // utility method - we make sure that the different histos have separate names by adding a string produced in here
    if (m_sys == SystematicSet()) return "central";
    return m_sys.begin()->name();

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

    if (m_respond_to_PtDepsys){

        htoadd = to_add.get_eff_PtDepsys();
        if (htoadd) AddHistos(m_eff_PtDepsys,htoadd,weight);
        else return false;

        htoadd = to_add.get_sf_PtDepsys();
        if (htoadd) AddHistos(m_sf_PtDepsys,htoadd,weight);
        else return false;
    }

    return true;
}

bool EfficiencyScaleFactor::Scale (float scale ){
    ScaleHisto(m_sf,scale);
    ScaleHisto(m_eff,scale);
    ScaleHisto(m_sf_sys,scale);
    ScaleHisto(m_eff_sys,scale);
    if (m_respond_to_PtDepsys){
        ScaleHisto(m_sf_PtDepsys,scale);
        ScaleHisto(m_eff_PtDepsys,scale);
    }

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
    }
    // if we need to, apply an additional relative pt dependent contribution
    if (m_respond_to_PtDepsys && m_PtDepsys_weight != 0 && mu.pt() > 200000.){
        int binsys = -1;
        cc = m_sf_PtDepsys->FindBin(mu,binsys);
        if (cc != CorrectionCode::Ok) {
            return cc;
        }
        // pt dependent sys is stored as relative error per TeV of pT
        SF = SF * (1 + m_PtDepsys_weight* m_sf_PtDepsys->GetBinContent(binsys) * mu.pt() / 1.0e6 );
       }
    return CorrectionCode::Ok;
}

CorrectionCode EfficiencyScaleFactor::Efficiency(const xAOD::Muon& mu, float & Eff){
    int bin = -1;
    CorrectionCode cc = m_eff->FindBin(mu,bin);
    if (cc != CorrectionCode::Ok) return cc;
    else {
        Eff = m_eff->GetBinContent(bin);
    }
    // if we need to, apply an additional relative pt dependent contribution
    if (m_respond_to_PtDepsys && m_PtDepsys_weight != 0 && mu.pt() > 200000.){
        int binsys = -1;
        cc = m_eff_PtDepsys->FindBin(mu,binsys);
        if (cc != CorrectionCode::Ok) {
            return cc;
        }
        // pt dependent sys is stored as relative error per TeV of pT
        Eff = Eff * (1 + m_PtDepsys_weight* m_eff_PtDepsys->GetBinContent(binsys) * mu.pt() / 1.0e6 );
    }
    return CorrectionCode::Ok;
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

    for (size_t h = 0; h < m_sf_replicas.size();++h) {
        if (m_sf_replicas[h] != NULL) delete m_sf_replicas[h];
        m_sf_replicas[h] = 0;
    }
    m_sf_replicas.clear();

    for (size_t h = 0; h < m_eff_replicas.size();++h) {
        if (m_eff_replicas[h] != NULL) delete m_eff_replicas[h];
        m_eff_replicas[h] = 0;
    }
    m_eff_replicas.clear();

    if (m_sf) delete m_sf;
    if (m_eff) delete m_eff;
    if (m_sf_sys) delete m_sf_sys;
    if (m_eff_sys) delete m_eff_sys;
    if (m_sf_PtDepsys) delete m_sf_PtDepsys;
    if (m_eff_PtDepsys) delete m_eff_PtDepsys;
}

void EfficiencyScaleFactor::AddHistos (HistHandler* & add_to, HistHandler* add_this, float weight){
    if (NULL == add_to){
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
    if (NULL == h) return;
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
    SFvec replicas(nrep);
    int nbins = h->NBins();
    for (int t = 0; t < nrep; t++){
        HistHandler* replica = package_histo((TH1*)h->GetHist()->Clone(
                Form("rep%d_%s",t,h->GetHist()->GetName())));
        for (int bin = 0; bin < nbins;bin++ ){
            replica->SetBinContent(bin,
                    Rndm.Gaus(h->GetBinContent(bin), h->GetBinError(bin)));
        }
        replicas.at(t) = replica;
    }
    return replicas;
}


void EfficiencyScaleFactor::ApplySysVariation(){

    if (m_sys == SystematicSet()) return;
    if (!m_is_lowpt){
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_STAT", -1)) AddStatErrors(-1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_STAT", 1)) AddStatErrors(1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_SYS", -1)) AddSysErrors(-1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_SYS", 1)) AddSysErrors(1.);
    }
    else{
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_STAT_LOWPT", -1)) AddStatErrors(-1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_STAT_LOWPT", 1)) AddStatErrors(1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_SYS_LOWPT", -1)) AddSysErrors(-1.);
        if (*(m_sys.begin()) == SystematicVariation ("MUON_"+m_effType+"_SYS_LOWPT", 1)) AddSysErrors(1.);
    }

}
void EfficiencyScaleFactor::AddStatErrors (float weight){

    AddStatErrors_histo(m_sf,weight);
    AddStatErrors_histo(m_eff,weight);

}

void EfficiencyScaleFactor::AddSysErrors (float weight){

    // turn on the pt dependent contribution, if there is any
    if (m_respond_to_PtDepsys){
        m_PtDepsys_weight = weight;
    }

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
    if (!h || !hsys) return;
    for (int t = 0; h && t < h->NBins(); ++t){
        double binc = h->GetBinContent(t);
        binc += weight * hsys->GetBinContent(t);
        h->SetBinContent(t, binc);
    }
}
void EfficiencyScaleFactor::AddStatErrors_histo (HistHandler* h, float weight){
    if (!h) return;
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
    if (m_eff_PtDepsys) m_eff_PtDepsys->GetHist()->Print();
    if (m_sf_PtDepsys) m_sf_PtDepsys->GetHist()->Print();
}



} /* namespace CP */
