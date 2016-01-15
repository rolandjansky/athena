/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 **********************************************************************/
#include "TrigEgammaAnalysisTools/EfficiencyTool.h"

#include "xAODEventInfo/EventInfo.h"

#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************

EfficiencyTool::
EfficiencyTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname) {
    declareProperty("DetailedHistograms", m_detailedHists=false);
}

//**********************************************************************
StatusCode EfficiencyTool::childInitialize(){


    return StatusCode::SUCCESS;
}

StatusCode EfficiencyTool::childBook()
{

    return StatusCode::SUCCESS;
}
StatusCode EfficiencyTool::childExecute()
{

    return StatusCode::SUCCESS;
}

StatusCode EfficiencyTool::childFinalize(){
  return StatusCode::SUCCESS;
}

void EfficiencyTool::fillInefficiency(const std::string dir,const xAOD::Electron *selEF,const xAOD::Photon *selPh,const xAOD::CaloCluster *clus,const xAOD::TrackParticle *trk){
    cd(dir);
    ATH_MSG_DEBUG("REGTEST::Inefficiency");
    // Currently check the PID on the xAOD
    // xAOD PID for trigger needs validation!
    // First check for the nullptr
    // 36 bins for isEM
    // 11 bins for isEMLH

    // Fill Reco efficiency (find a nearby object)
    /*if(clus) hist1("eff_hltreco")->Fill("Cluster",1);
    else hist1("eff_hltreco")->Fill("Cluster",0);
    
    if(trk) hist1("eff_hltreco")->Fill("Track",1);
    else hist1("eff_hltreco")->Fill("Track",0);

    if(selPh) hist1("eff_hltreco")->Fill("Photon",1);
    else hist1("eff_hltreco")->Fill("Photon",0);

    if(selEF) hist1("eff_hltreco")->Fill("Electron",1);
    else hist1("eff_hltreco")->Fill("Electron",0);*/

    float lastbinIsEM=hist1("IsEmFailTight")->GetNbinsX()-1;
    float lastbinIsEMLH=hist1("IsEmLHFailTight")->GetNbinsX()-1;

    hist1("IsEmFailTight")->Fill(lastbinIsEM+0.5);
    hist1("IsEmFailMedium")->Fill(lastbinIsEM+0.5);
    hist1("IsEmFailLoose")->Fill(lastbinIsEM+0.5);
    hist1("IsEmLHFailTight")->Fill(lastbinIsEMLH+0.5);
    hist1("IsEmLHFailMedium")->Fill(lastbinIsEMLH+0.5);
    hist1("IsEmLHFailLoose")->Fill(lastbinIsEMLH+0.5);
  
    if(selPh==NULL) ATH_MSG_DEBUG("fillIneffiency::No photon found!");
    if(selEF!=NULL){
        ATH_MSG_DEBUG("REGTEST::Inefficiency Electron pt, eta, phi "<< selEF->pt() << " " << selEF->eta() << " " << selEF->phi());

        unsigned int loose = -99;
        unsigned int medium= -99;
        unsigned int tight = -99;
        unsigned int lhloose = -99;
        unsigned int lhmedium= -99;
        unsigned int lhtight = -99;
        selEF->selectionisEM(loose,"isEMLoose");
        selEF->selectionisEM(medium,"isEMMedium");
        selEF->selectionisEM(tight,"isEMTight");
        selEF->selectionisEM(lhloose,"isEMLHLoose");
        selEF->selectionisEM(lhmedium,"isEMLHMedium");
        selEF->selectionisEM(lhtight,"isEMLHTight");

        for(int ii=0;ii<32;ii++){
            if ( (tight>>ii) & 0x1 ){
                hist1("IsEmFailTight")->Fill(ii+0.5);
            }
            if ( (medium>>ii) & 0x1 ){
                hist1("IsEmFailMedium")->Fill(ii+0.5);
            }
            if ( (loose>>ii) & 0x1 ){
                hist1("IsEmFailLoose")->Fill(ii+0.5);
            }
        }
        for(int ii=0;ii<8;ii++){
            if ( (lhtight>>ii) & 0x1 ){
                hist1("IsEmLHFailTight")->Fill(ii+0.5);
            }
            if ( (lhmedium>>ii) & 0x1 ){
                hist1("IsEmLHFailMedium")->Fill(ii+0.5);
            }
            if ( (lhloose>>ii) & 0x1 ){
                hist1("IsEmLHFailLoose")->Fill(ii+0.5);
            }
        }
        
    }
    else {
        if(trk==NULL && clus!=NULL){
            ATH_MSG_DEBUG("fillInefficiency::No Electron, nearby cluster"); 
            // No electron candidate but we have photon
            // Do something for hasCluster
            hist1("IsEmFailTight")->Fill( (lastbinIsEM-3) + 0.5);
            hist1("IsEmFailMedium")->Fill( (lastbinIsEM-3) + 0.5);
            hist1("IsEmFailLoose")->Fill( (lastbinIsEM-3) + 0.5);
            hist1("IsEmLHFailTight")->Fill( (lastbinIsEMLH-3) + 0.5);
            hist1("IsEmLHFailMedium")->Fill( (lastbinIsEMLH-3) + 0.5);
            hist1("IsEmLHFailLoose")->Fill( (lastbinIsEMLH-3) + 0.5);
        }
        if(clus==NULL && trk!=NULL){
            ATH_MSG_DEBUG("fillInefficiency::No Electron, no cluster"); 
            // No electron candidate but we have photon
            // Do something for hasCluster
            hist1("IsEmFailTight")->Fill( (lastbinIsEM-2) + 0.5);
            hist1("IsEmFailMedium")->Fill( (lastbinIsEM-2) + 0.5);
            hist1("IsEmFailLoose")->Fill( (lastbinIsEM-2) + 0.5);
            hist1("IsEmLHFailTight")->Fill( (lastbinIsEMLH-2) + 0.5);
            hist1("IsEmLHFailMedium")->Fill( (lastbinIsEMLH-2) + 0.5);
            hist1("IsEmLHFailLoose")->Fill( (lastbinIsEMLH-2) + 0.5);
        }
        if(clus==NULL && trk==NULL){
            ATH_MSG_DEBUG("fillInefficiency::No Electron, no cluster"); 
            // Unknown failure
            hist1("IsEmFailTight")->Fill( (lastbinIsEM-1) + 0.5);
            hist1("IsEmFailMedium")->Fill( (lastbinIsEM-1) + 0.5);
            hist1("IsEmFailLoose")->Fill( (lastbinIsEM-1) + 0.5);
            hist1("IsEmLHFailTight")->Fill( (lastbinIsEMLH-1) + 0.5);
            hist1("IsEmLHFailMedium")->Fill( (lastbinIsEMLH-1) + 0.5);
            hist1("IsEmLHFailLoose")->Fill( (lastbinIsEMLH-1) + 0.5);
        }
    }
    if(clus!=NULL) ATH_MSG_DEBUG("REGTEST::Inefficiency Cluster " << clus->et() << " " << clus->eta() << " " << clus->phi());
    if(trk!=NULL) ATH_MSG_DEBUG("REGTEST::Inefficiency Track " << trk->pt() << " " << trk->eta() << " " << trk->phi());
    else ATH_MSG_DEBUG("REGTEST::Inefficiency No track");
}

void EfficiencyTool::inefficiency(const std::string basePath,
        const unsigned int runNumber, const unsigned int eventNumber, const float etthr, 
        std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    ATH_MSG_DEBUG("INEFF::Start Inefficiency Analysis ======================= " << basePath);
    cd(basePath);
    // Inefficiency analysis
    float et=0.;
    const xAOD::Egamma* eg =pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(xAOD::EgammaHelpers::isElectron(pairObj.first)){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    ATH_MSG_DEBUG("INEFF::Offline et, eta, phi " << et << " " << eta << " " << phi);
    const xAOD::Electron* selEF = NULL;
    const xAOD::Photon* selPh = NULL;
    const xAOD::CaloCluster* selClus = NULL;
    const xAOD::TrackParticle* selTrk = NULL;
    
    // Can we acquire L1 information 
    //
    //auto initRois = fc.get<TrigRoiDescriptor>();
    //if ( initRois.size() < 1 ) ATH_MSG_DEBUG("No L1 RoI"); 
    //auto itEmTau = m_trigDecTool->ancestor<xAOD::EmTauRoI>(initRois[0]);
    //ATH_MSG_DEBUG("INEFF::Retrieve L1");
    //const auto* EmTauRoI = getFeature<xAOD::EmTauRoI>(feat);
    ATH_MSG_DEBUG("INEFF::Retrieve EF Electron");
    const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
    ATH_MSG_DEBUG("INEFF::Retrieve EF Photons");
    const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
    ATH_MSG_DEBUG("INEFF::Retrieve EF Cluster");
    const auto* EFClus = getFeature<xAOD::CaloClusterContainer>(feat);
    //ATH_MSG_DEBUG("INEFF::Retrieve EF Trk");
    //const auto* L2Trk = getFeature<xAOD::TrackParticleContainer>(feat);
    //const auto* L2Trk = getFeature<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_FTF");
    //const auto* EFIDTrk = getFeature<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_EFID");
    //const auto* EFTrkIDTrig = getFeature<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_IDTrig");
    

    //xAOD::TrackParticleContainer *EFTrk=0;
    //if(EFTrkEFID!=NULL) EFTrk=EFTrkEFID;
    //else if(EFTrkIDTrig!=NULL) EFTrk=EFTrkIDTrig;

    float dRmax=0.07;
    
    bool passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(feat);
    bool passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(feat);
    bool passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(feat);
    bool passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(feat,"TrigEFCaloCalibFex");
    //bool passedEFTrkEFID = ancestorPassed<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_EFID");
    //bool passedEFTrkIDTrig = ancestorPassed<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_IDTrig");
    //bool passedEFTrk = passedEFTrkEFID || passedEFTrkIDTrig;
    bool passedEF = ancestorPassed<xAOD::ElectronContainer>(feat);

    // Ensure L1 passes
    // And offline passes et cut

    if(passedL1Calo && et > etthr) {
        ATH_MSG_DEBUG("INEFF::Passed L1 and offline et");
        ATH_MSG_DEBUG("INEFF:: " << passedL2Calo << passedL2 << passedEFCalo << passedEF);
        if(passedL2Calo){
            ATH_MSG_DEBUG("INEFF::Passes L2 Calo");
            hist1("eff_triggerstep")->Fill("L2Calo",1);
        }
        else{
            ATH_MSG_DEBUG("INEFF::Fails L2 Calo");
            hist1("eff_triggerstep")->Fill("L2Calo",0);
        }
        if(passedL2){
            ATH_MSG_DEBUG("INEFF::Passes L2");
            hist1("eff_triggerstep")->Fill("L2",1);
        }
        else {
            ATH_MSG_DEBUG("INEFF::Fails L2");
            hist1("eff_triggerstep")->Fill("L2",0);
        }
        /*if(L2Trk==NULL){
            hist1("eff_triggerstep")->Fill("L2TrackCont",0);
        }
        else{
            hist1("eff_triggerstep")->Fill("L2TrackCont",1);
        }*/
        if(passedEFCalo){
            ATH_MSG_DEBUG("INEFF::Passes EFCalo");
            hist1("eff_triggerstep")->Fill("EFCalo",1);
        }
        else{
            ATH_MSG_DEBUG("INEFF::Fails EFCalo");
            hist1("eff_triggerstep")->Fill("EFCalo",0);
        }
        if(EFClus==NULL)    hist1("eff_triggerstep")->Fill("EFCaloCont",0);
        else             hist1("eff_triggerstep")->Fill("EFCaloCont",1);
        /*if(passedEFTrk){
            ATH_MSG_DEBUG("INEFF:: Passes Track Hypo!");
            hist1("eff_triggerstep")->Fill("EFTrack",1);
            //hist1("eff_triggerstep")->Fill("EFTrackCont",1);
        }
        else {
            //ATH_MSG_INFO("INEFF:: Fails Track Hypo!");
            //ATH_MSG_INFO("INEFF:: EFCalo result " << passedEFCalo);
            hist1("eff_triggerstep")->Fill("EFTrack",0);
        }
        if(EFTrk==NULL){
            hist1("eff_triggerstep")->Fill("EFTrackCont",0);
            ATH_MSG_WARNING("TRKTEST: NULL Track Container! Run, Event " << runNumber << " " << eventNumber);
        }
        else{
            hist1("eff_triggerstep")->Fill("EFTrackCont",1);
            ATH_MSG_DEBUG("TRKTEST: Container has " << EFTrk->size() << " ntracks");
        }*/
        if(passedEF){
            ATH_MSG_DEBUG("INEFF::Passes EF");
            hist1("eff_triggerstep")->Fill("HLT",1);
        }
        else{
            ATH_MSG_DEBUG("INEFF::Fails EF");
            hist1("eff_triggerstep")->Fill("HLT",0);
        }
        if(EFEl==NULL)    hist1("eff_triggerstep")->Fill("HLTCont",0);
        else     hist1("eff_triggerstep")->Fill("HLTCont",1);

        // Fill efficiency plot for HLT trigger steps
        if(!passedEF && passedEFCalo){
            ATH_MSG_DEBUG("REGEST::Fails EF Electron, passes EFCalo Hypo Run " << runNumber << " Event " << eventNumber);

            dRmax=0.15;
            if ( EFEl != NULL ){
                ATH_MSG_DEBUG("Retrieved ElectronContainer for inefficiency " << EFEl->size());
                for(const auto& el : *EFEl){
                    float dr=dR(eta,phi,el->eta(),el->phi());
                    if ( dr<dRmax){
                        dRmax=dr;
                        selEF = el;
                    } // dR
                } // loop over EFEl
                ATH_MSG_DEBUG("Closest electron dR " << dRmax);
            } //FC exists
            else ATH_MSG_DEBUG("Electron Container NULL");
            dRmax=0.15;
            if ( EFPh != NULL ){
                ATH_MSG_DEBUG("Retrieved PhotonnContainer for inefficiency " << EFPh->size());
                for(const auto& ph : *EFPh){
                    float dr=dR(eta,phi,ph->eta(),ph->phi());
                    if ( dr<dRmax){
                        dRmax=dr;
                        selPh = ph;
                    } // dR
                } // loop over EFEl
                ATH_MSG_DEBUG("Closest electron dR " << dRmax);
            } //FC exists
            else ATH_MSG_DEBUG("Photon Container NULL");
            dRmax=0.15;
            if ( EFClus != NULL ){
                ATH_MSG_DEBUG("Retrieved ClusterContainer for inefficiency " << EFClus->size());
                for(const auto& clus : *EFClus){
                    float dr=dR(eta,phi,clus->eta(),clus->phi());
                    if(dr<dRmax){
                        dRmax=dr;
                        selClus = clus;
                    } // dR
                } // loop over EFPh
                ATH_MSG_DEBUG("Closest cluster dR " << dRmax);
            }
            else ATH_MSG_DEBUG("CaloCluster Container NULL");
            /*dRmax=0.15;
            if ( EFTrk != NULL ){
                ATH_MSG_DEBUG("Retrieved TrackContainer for inefficiency " << EFTrk->size());
                for(const auto& trk : *EFTrk){
                    float dr=dR(eta,phi,trk->eta(),trk->phi());
                    if(dr<dRmax){
                        dRmax=dr;
                        selTrk = trk;
                    } // dR
                } // loop over EFPh
                ATH_MSG_DEBUG("Closest track dR " << dRmax);
            } //FC exists
            else ATH_MSG_DEBUG("TrackParticle Container NULL");*/
            if(EFClus==NULL){
                hist1("eff_hltreco")->Fill("ClusterCont",0);
                hist1("eff_hltreco")->Fill("Cluster",0);
                hist1("eff_hltreco")->Fill("ClusterMatch",0);
            }
            else{
                hist1("eff_hltreco")->Fill("ClusterCont",1);
                if(EFClus->size() > 0){
                    hist1("eff_hltreco")->Fill("Cluster",1);
                    if(selClus) hist1("eff_hltreco")->Fill("ClusterMatch",1);
                    else hist1("eff_hltreco")->Fill("ClusterMatch",0);
                }
                else{
                    hist1("eff_hltreco")->Fill("Cluster",0);
                    hist1("eff_hltreco")->Fill("ClusterMatch",0);
                }
            }

            /*if(EFTrk==NULL){
                hist1("eff_hltreco")->Fill("TrackCont",0);
                hist1("eff_hltreco")->Fill("Track",0);
                hist1("eff_hltreco")->Fill("TrackMatch",0);
            }
            else{
                hist1("eff_hltreco")->Fill("TrackCont",1);
                if(EFTrk->size() > 0){
                    hist1("eff_hltreco")->Fill("Track",1);
                    if(selTrk) hist1("eff_hltreco")->Fill("TrackMatch",1);
                    else hist1("eff_hltreco")->Fill("TrackMatch",0);
                }
                else{
                    hist1("eff_hltreco")->Fill("Track",0);
                    hist1("eff_hltreco")->Fill("TrackMatch",0);
                }
            }*/

            if(EFPh==NULL){
                hist1("eff_hltreco")->Fill("PhotonCont",0);
                hist1("eff_hltreco")->Fill("Photon",0);
                hist1("eff_hltreco")->Fill("PhotonMatch",0);
            }
            else {
                hist1("eff_hltreco")->Fill("PhotonCont",1);
                if(EFPh->size() > 0){
                    hist1("eff_hltreco")->Fill("Photon",1);
                    if(selPh) hist1("eff_hltreco")->Fill("PhotonMatch",1);
                    else  hist1("eff_hltreco")->Fill("PhotonMatch",0);
                }
                else{ 
                    hist1("eff_hltreco")->Fill("Photon",0);
                    hist1("eff_hltreco")->Fill("PhotonMatch",0);
                }
            }

            if(EFEl==NULL){
                hist1("eff_hltreco")->Fill("ElectronCont",0);
                hist1("eff_hltreco")->Fill("Electron",0);
                hist1("eff_hltreco")->Fill("ElectronMatch",0);
            }
            else {
                hist1("eff_hltreco")->Fill("ElectronCont",1);
                if(EFEl->size() > 0){
                    hist1("eff_hltreco")->Fill("Electron",1);
                    if(selEF) hist1("eff_hltreco")->Fill("ElectronMatch",1);
                    else hist1("eff_hltreco")->Fill("ElectronMatch",0);
                }
                else{
                    hist1("eff_hltreco")->Fill("Electron",0);
                    hist1("eff_hltreco")->Fill("ElectronMatch",0);
                }
            }

            fillInefficiency(basePath,selEF,selPh,selClus,selTrk);
        }
    }
    ATH_MSG_DEBUG("End Inefficiency Analysis ======================= " << basePath);
}
void EfficiencyTool::fillEfficiency(const std::string dir,bool isPassed,const float etthr, const std::string pidword, const xAOD::Egamma *eg){

    cd(dir);
    float et=0.;
    bool pid=true;
    ATH_MSG_DEBUG("Default pid " << pid << " te " << isPassed);
    if(xAOD::EgammaHelpers::isElectron(eg)){
        ATH_MSG_DEBUG("Offline Electron with pidword " << pidword);
        const xAOD::Electron* el =static_cast<const xAOD::Electron*> (eg);
        pid=el->auxdecor<bool>(pidword);
        ATH_MSG_DEBUG("Electron pid " << pid);
        et = getEt(el)/1e3;
    }
    else  et=eg->caloCluster()->et()/1e3;

    float eta = eg->caloCluster()->etaBE(2);
    float phi = eg->phi();
    float pt = eg->pt();
    float avgmu=0.;
    if(m_lumiTool)
        avgmu = m_lumiTool->lbAverageInteractionsPerCrossing();
   
    ATH_MSG_DEBUG("PID decision efficiency " << eg->auxdecor<bool>(pidword));
    if(pid){
        hist1("et")->Fill(et);
        hist1("pt")->Fill(pt);
        hist1("highet")->Fill(et);
        if(et > etthr+1.0){
            hist1("eta")->Fill(eta);
            hist1("phi")->Fill(phi);
            hist1("mu")->Fill(avgmu);
            hist2("et_eta")->Fill(et,eta);
            hist2("coarse_et_eta")->Fill(et,eta);
        }
        if(isPassed) {
            hist1("match_et")->Fill(et);
            hist1("match_pt")->Fill(pt);
            hist1("match_highet")->Fill(et);
            if(et > etthr+1.0){
                hist1("match_eta")->Fill(eta);
                hist1("match_phi")->Fill(phi);
                hist1("match_mu")->Fill(avgmu);
                hist2("match_et_eta")->Fill(et,eta);
                hist2("match_coarse_et_eta")->Fill(et,eta);

            }
            hist1("eff_et")->Fill(et,1);
            hist1("eff_pt")->Fill(pt,1);
            hist1("eff_highet")->Fill(et,1);
            if(et > etthr+1.0){
                hist1("eff_eta")->Fill(eta,1);
                hist1("eff_phi")->Fill(phi,1);
                hist1("eff_mu")->Fill(avgmu,1);
                hist2("eff_et_eta")->Fill(et,eta,1);
                hist2("eff_coarse_et_eta")->Fill(et,eta,1);
            }
        }
        else {
            hist1("eff_et")->Fill(et,0);
            hist1("eff_pt")->Fill(pt,0);
            hist1("eff_highet")->Fill(et,0);
            if(et > etthr+1.0){
                hist1("eff_eta")->Fill(eta,0);
                hist1("eff_phi")->Fill(phi,0);
                hist1("eff_mu")->Fill(avgmu,0);
                hist2("eff_et_eta")->Fill(et,eta,0);
                hist2("eff_coarse_et_eta")->Fill(et,eta,0);
            }
        }
    }
}

StatusCode EfficiencyTool::toolExecute(const std::string basePath,const TrigInfo info,
        std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>> pairObjs){
    const std::string dir = basePath+"/"+info.trigName;
    const float etthr = info.trigThrHLT;
    const std::string pidword = info.trigPidDecorator;
    
    unsigned int runNumber=0;
    unsigned int eventNumber=0;
    // This will retrieve eventInfo for each trigger
    // Retrieve once and access via a private member
    /*const xAOD::EventInfo *eventInfo;
    if ( (m_storeGate->retrieve(eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve eventInfo ");
    }
    else {
        runNumber=eventInfo->runNumber();
        eventNumber=eventInfo->eventNumber();
    }*/
    ATH_MSG_DEBUG("Efficiency for " << info.trigName << " " <<pidword);
    for(const auto pairObj : pairObjs){
        // Final cuts done here
        if(pairObj.first->type()==xAOD::Type::Electron){
            const xAOD::Electron* el = static_cast<const xAOD::Electron *> (pairObj.first);
            float et = getEt(el)/1e3;
            if(et < info.trigThrHLT-5.0) continue; // return StatusCode::SUCCESS;

        } // Offline object Electron
        else if(pairObj.first->type()==xAOD::Type::Photon){
            float et = getCluster_et(pairObj.first)/1e3;
            if(et < info.trigThrHLT-5.0) continue; // return StatusCode::SUCCESS;
        } // Offline photon

        ATH_MSG_DEBUG("Fill probe histograms");
        bool passedL1Calo=false;
        bool passedL2Calo=false;
        bool passedL2=false;
        bool passedEFCalo=false;
        bool passedEF=false;
        if ( pairObj.second ) {
            ATH_MSG_DEBUG("Retrieve Ancestor passed");
            passedL1Calo=ancestorPassed<xAOD::EmTauRoI>(pairObj.second);
            passedL2Calo = ancestorPassed<xAOD::TrigEMCluster>(pairObj.second);
            if(xAOD::EgammaHelpers::isElectron(pairObj.first))
                passedL2 = ancestorPassed<xAOD::TrigElectronContainer>(pairObj.second);

            else 
                passedL2 = ancestorPassed<xAOD::TrigPhotonContainer>(pairObj.second);

            passedEFCalo = ancestorPassed<xAOD::CaloClusterContainer>(pairObj.second);

            if(xAOD::EgammaHelpers::isElectron(pairObj.first))
                passedEF = ancestorPassed<xAOD::ElectronContainer>(pairObj.second);
            else 
                passedEF = ancestorPassed<xAOD::PhotonContainer>(pairObj.second);

        } // Features
        this->fillEfficiency(dir+"/Efficiency/L1Calo",passedL1Calo,etthr,pidword,pairObj.first);
        this->fillEfficiency(dir+"/Efficiency/L2Calo",passedL2Calo,etthr,pidword,pairObj.first);
        this->fillEfficiency(dir+"/Efficiency/L2",passedL2,etthr,pidword,pairObj.first);
        this->fillEfficiency(dir+"/Efficiency/EFCalo",passedEFCalo,etthr,pidword,pairObj.first);
        this->fillEfficiency(dir+"/Efficiency/HLT",passedEF,etthr,pidword,pairObj.first);
        if(m_detailedHists){
            for(const auto pid : m_isemname) {
                this->fillEfficiency(dir+"/Efficiency/HLT/"+pid,passedEF,etthr,"is"+pid,pairObj.first);
                if( pairObj.first->auxdecor<bool>("Isolated") ) fillEfficiency(dir+"/Efficiency/HLT/"+pid+"Iso",passedEF,etthr,"is"+pid,pairObj.first);
            }
            for(const auto pid : m_lhname) {
                this->fillEfficiency(dir+"/Efficiency/HLT/"+pid,passedEF,etthr,"is"+pid,pairObj.first);
                if( pairObj.first->auxdecor<bool>("Isolated") ) fillEfficiency(dir+"/Efficiency/HLT/"+pid+"Iso",passedEF,etthr,"is"+pid,pairObj.first);
            }
        }

        // Inefficiency analysis
        // 
        if(pairObj.first->type()==xAOD::Type::Electron){
            if(!pairObj.first->auxdecor<bool>(info.trigPidDecorator)) continue;
            inefficiency(dir+"/Efficiency/HLT",runNumber,eventNumber,etthr,pairObj);
        }
        else inefficiency(dir+"/Efficiency/HLT",runNumber,eventNumber,etthr,pairObj);
        ATH_MSG_DEBUG("Complete efficiency");
    }
    return StatusCode::SUCCESS;
}
