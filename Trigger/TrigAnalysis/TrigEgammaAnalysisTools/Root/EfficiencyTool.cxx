/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 **********************************************************************/
#include "TrigEgammaAnalysisTools/EfficiencyTool.h"
#include "TrigEgammaAnalysisTools/ValidationException.h"

#include "xAODEventInfo/EventInfo.h"

#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************

EfficiencyTool::EfficiencyTool(const std::string& myname): 
    TrigEgammaAnalysisBaseTool(myname)
{
    m_detailedHists=false;
}

//**********************************************************************
StatusCode EfficiencyTool::childInitialize()
{
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

StatusCode EfficiencyTool::childFinalize()
{
  return StatusCode::SUCCESS;
}

bool EfficiencyTool::analyseIsEMLH(const xAOD::Electron *eg, const std::string pid)
{
    const std::string pidword = "isEM" + pid;
    const std::string fail = "Fail" + pidword;
    const std::string ineff = "Ineff" + pidword;

    bool failIsEMLH = true;
    try{
	    ATH_MSG_DEBUG("Running selectionisEM("<<pidword<<")");
	    unsigned int isem = eg->selectionisEM(pidword);

	    failIsEMLH = false;
	    for (int ii = 0; ii < 11; ii++) {
	        if ((isem >> ii) & 0x1) {
	    	failIsEMLH = true;
	    	hist1(fail)->Fill(ii + 0.5);
	    	hist1(ineff)->Fill(ii + 3.5, 1);
	        }
	    }
    } catch (const ValidationException &e) {
	    ATH_MSG_WARNING("Exception thrown: " << e.msg() );
	    ATH_MSG_WARNING("Is " << pidword << " is a valid one? returning failed....");
	    failIsEMLH = true;
    } catch(...) {
	    ATH_MSG_WARNING("Unknown exception caught in analyseIsEMLH ... Is " << pidword << " is a valid one? returning failed....");
	    failIsEMLH = true;
    }
    return failIsEMLH;

}

bool EfficiencyTool::analyseIsEM(const xAOD::Electron *eg, const std::string pid)
{
    const std::string pidword = "isEM" + pid;
    const std::string fail = "Fail" + pidword;
    const std::string ineff = "Ineff" + pidword;
    const std::string ineffTrk = ineff + "Trk";
    const std::string ineffTrkClus = ineff + "TrkClus";
    const std::string ineffTRT = ineff + "TRT";

    bool failclus = false;
    bool failtrkclus = false;
    bool failtrk = false;
    bool failtrt = false;
    bool failisem = false;

    unsigned int isem = eg->selectionisEM(pidword);

    for (int ii = 0; ii < 29; ii++) {
        if ((isem >> ii) & 0x1) {
            failisem = true;
            hist1(fail)->Fill(ii + 0.5);
            if (ii >= 16 && ii <= 19) {
                failtrk = true;
                hist1(ineffTrk)->Fill(ii - 15.5, 1);
                hist1(ineffTrk)->Fill(5.5, 1);
            }
            if (ii == 27) {
                failtrk = true;
                hist1(ineffTrk)->Fill(4.5, 1);
                hist1(ineffTrk)->Fill(5.5, 1);
            }
            if (ii == 20 || ii == 21 || ii == 22) {
                failtrkclus = true;
                hist1(ineffTrkClus)->Fill(ii - 19.5, 1);
                hist1(ineffTrkClus)->Fill(4.5, 1);
            }
            if (ii == 28) {
                failtrkclus = true;
                hist1(ineffTrkClus)->Fill(3.5, 1);
                hist1(ineffTrkClus)->Fill(4.5, 1);
            }
            if (ii >= 23 && ii <= 26) {
                failtrt = true;
                hist1(ineffTRT)->Fill(ii - 22.5, 1);
                hist1(ineffTRT)->Fill(4.5, 1);
            }
        }

    }
    bool onlyclus = !failtrkclus && !failtrk && !failtrt && failclus;
    bool onlytrkclus = failtrkclus && !failtrk && !failtrt && !failclus;
    bool onlytrk = !failtrkclus && failtrk && !failtrt && !failclus;
    bool trktrt = !failtrkclus && failtrk && failtrt && !failclus;
    bool onlytrt = !failtrkclus && !failtrk && failtrt && !failclus;
    bool trkclustrktrt = failtrkclus && failtrk && failtrt && !failclus;

    if (onlyclus) hist1(ineff)->Fill(3.5, 1);
    else if (onlytrkclus) hist1(ineff)->Fill(4.5, 1);
    else if (onlytrk) hist1(ineff)->Fill(5.5, 1);
    else if (onlytrt) hist1(ineff)->Fill(6.5, 1);
    else if (trktrt) hist1(ineff)->Fill(7.5, 1);
    else if (trkclustrktrt) hist1(ineff)->Fill(8.5, 1);
    else hist1(ineff)->Fill(23.5, 1); // Unknown -- some combination failed

    return failisem;
}

void EfficiencyTool::fillInefficiency(const std::string& pid, const std::string dir,const xAOD::Electron *selEF,const xAOD::Photon *selPh,const xAOD::CaloCluster *clus,const xAOD::TrackParticle *trk,
                                      const asg::AcceptData& acceptData)
{
    cd(dir);
    ATH_MSG_DEBUG("REGTEST::Inefficiency");
    // Currently check the PID on the xAOD
    // xAOD PID for trigger needs validation!
    // First check for the nullptr
    // 36 bins for isEM
    // 11 bins for isEMLH

    // Fill Reco efficiency (find a nearby object)
    /*if(clus) hist1("eff_hltreco")->Fill("Cluster", 1);
    else hist1("eff_hltreco")->Fill("Cluster",0);
    
    if(trk) hist1("eff_hltreco")->Fill("Track", 1);
    else hist1("eff_hltreco")->Fill("Track",0);

    if(selPh) hist1("eff_hltreco")->Fill("Photon", 1);
    else hist1("eff_hltreco")->Fill("Photon",0);

    if(selEF) hist1("eff_hltreco")->Fill("Electron", 1);
    else hist1("eff_hltreco")->Fill("Electron",0);*/
   
    std::bitset<4> reco; //Electron, cluster, track, photon

    bool failiso = false;
    bool failbits = false;
    
    if(selEF != nullptr) reco.set(0);
    if(clus != nullptr) reco.set(1);
    if(trk != nullptr) reco.set(2);
    if(selPh != nullptr) reco.set(3);

    const std::string fail = "FailisEM" + pid;
    const std::string ineff = "IneffisEM" + pid;

    float lastbin = hist1(ineff)->GetNbinsX() - 0.5;
    float sumbin = lastbin - 1;

    if (!acceptData.getCutResult("L2Calo")) {
        hist1(ineff)->Fill(0.5, 1);
        hist1(ineff)->Fill(sumbin, 1);
    }
    else if (!acceptData.getCutResult("L2")) {
        hist1(ineff)->Fill(1.5, 1);
        hist1(ineff)->Fill(sumbin, 1);
    }
    else if (!acceptData.getCutResult("EFCalo")) {
        hist1(ineff)->Fill(2.5, 1);
        hist1(ineff)->Fill(sumbin, 1);
    }
    // else if (!acceptData.getCutResult("EFTrack")) {
    //     hist1(ineff)->Fill(3.5, 1);
    //     hist1(ineff)->Fill(13.5, 1);
    // }
    else if (!acceptData.getCutResult("HLT")) {
        if (reco.test(0)) {
            if (boost::contains(pid, "LH")) failbits = analyseIsEMLH(selEF, pid);
            else failbits = analyseIsEM(selEF, pid);

            float isolation = getIsolation_ptcone20(selEF) / selEF->pt();
            if(isolation > 0.1) failiso = true;
            if (failiso) hist1(fail)->Fill(31 + 0.5);

            if (failbits && !failiso) {
                hist1(ineff)->Fill(lastbin - 11, 1);
                hist1(ineff)->Fill(sumbin, 1);
            }
            else if (!failbits && failiso) {
                hist1(ineff)->Fill(lastbin - 10, 1);
                hist1(ineff)->Fill(sumbin, 1);
            }
            else if (failbits || failiso) {
                hist1(ineff)->Fill(lastbin - 9, 1);
                hist1(ineff)->Fill(sumbin, 1);
            }
            else {
                hist1(ineff)->Fill(lastbin - 2, 1);
                hist1(ineff)->Fill(sumbin, 1);
            }
        }
        else if (!reco.test(0) && reco.test(1) && reco.test(2)) {
            hist1(ineff)->Fill(lastbin - 8, 1); //Track and Cluster
            hist1(ineff)->Fill(sumbin, 1);
        }
        else if (!reco.test(0) && !reco.test(1) && reco.test(2)) {
            hist1(ineff)->Fill(lastbin - 7, 1); // No Cluster w/ track
            hist1(ineff)->Fill(sumbin, 1);
        }
        else if (!reco.test(0) && reco.test(1) && !reco.test(2)) {
            hist1(ineff)->Fill(lastbin - 6, 1); //Cluster Only
            hist1(ineff)->Fill(sumbin, 1);
        }
        else if (!reco.test(0) && !reco.test(1) && !reco.test(2) && !reco.test(3)) {
            hist1(ineff)->Fill(lastbin - 5, 1); // No object in RoI
            hist1(ineff)->Fill(sumbin, 1);
        }
        else if (reco.test(1) || reco.test(2) || reco.test(3)) {
            hist1(ineff)->Fill(lastbin - 4, 1);
            hist1(ineff)->Fill(sumbin, 1);
        }
        else{
            hist1(ineff)->Fill(sumbin, 1);
        }

        // hist1(ineff)->Fill(n + 12.5, 1); // total (HLT)
        
        // if(reco.test(3) && reco.test(1)) hist1(ineff)->Fill(n + 7.5, 1); //Has cluster and photon --> should be same as cluster only?
        
        float lastbinfail = hist1(fail)->GetNbinsX() - 1;
        hist1(fail)->Fill(lastbinfail + 0.5);
      
        if (selPh == nullptr) ATH_MSG_DEBUG("fillIneffiency::No photon found!");
        if (selEF != nullptr){
            ATH_MSG_DEBUG("REGTEST::Inefficiency Electron pt, eta, phi "<< selEF->pt() << " " << selEF->eta() << " " << selEF->phi());
        }
        else {
            if (trk == nullptr && clus != nullptr){
                ATH_MSG_DEBUG("fillInefficiency::No Electron, nearby cluster"); 
                // No electron candidate but we have photon
                // Do something for hasCluster
                hist1(fail)->Fill((lastbinfail - 3) + 0.5);
            }
            if (clus == nullptr && trk != nullptr){
                ATH_MSG_DEBUG("fillInefficiency::No Electron, no cluster"); 
                // No electron candidate but we have photon
                // Do something for hasCluster
                hist1(fail)->Fill((lastbinfail - 2) + 0.5);
            }
            if (clus == nullptr && trk == nullptr){
                ATH_MSG_DEBUG("fillInefficiency::No Electron, no cluster"); 
                // Unknown failure
                hist1(fail)->Fill((lastbinfail - 1) + 0.5);
            }
        }
        if (clus != nullptr) ATH_MSG_DEBUG("REGTEST::Inefficiency Cluster " << clus->et() << " " << clus->eta() << " " << clus->phi());
        if (trk != nullptr) ATH_MSG_DEBUG("REGTEST::Inefficiency Track " << trk->pt() << " " << trk->eta() << " " << trk->phi());
        else ATH_MSG_DEBUG("REGTEST::Inefficiency No track");
    }
    hist1(ineff)->Fill(lastbin, 1);
}

void EfficiencyTool::inefficiency(const std::string& pid, const std::string basePath,const float etthr, 
                                  std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj,
                                  const asg::AcceptData& acceptData)
{
    ATH_MSG_DEBUG("INEFF::Start Inefficiency Analysis ======================= " << basePath);
    cd(basePath);
    // Inefficiency analysis
    float et = 0.0;
    const xAOD::Egamma* eg = pairObj.first;
    const HLT::TriggerElement *feat = pairObj.second; 
    if(xAOD::EgammaHelpers::isElectron(pairObj.first)){
        ATH_MSG_DEBUG("Offline Electron");
        const xAOD::Electron* el = static_cast<const xAOD::Electron*>(eg);
        et = getEt(el) / 1e3;
    }
    else  et = eg->caloCluster()->et() / 1e3;

    float eta = eg->eta();
    float phi = eg->phi();
    ATH_MSG_DEBUG("INEFF::Offline et, eta, phi " << et << " " << eta << " " << phi);
    
    const xAOD::Electron* selEF = nullptr;
    const xAOD::Photon* selPh = nullptr;
    const xAOD::CaloCluster* selClus = nullptr;
    const xAOD::TrackParticle* selTrk = nullptr;

    float dRmax = 0.07;

    const std::string pidword = "isEM" + pid;
    const std::string ineff = "Ineff" + pidword;

    // Ensure L1 passes and offline passes et cut
    if(acceptData.getCutResult("L1Calo") && et > etthr) {
        ATH_MSG_DEBUG("INEFF::Passed L1 and offline et");
        hist1("eff_triggerstep")->Fill("L2Calo",acceptData.getCutResult("L2Calo"));
        hist1("eff_triggerstep")->Fill("L2",acceptData.getCutResult("L2"));
        hist1("eff_triggerstep")->Fill("EFCalo",acceptData.getCutResult("EFCalo"));
        hist1("eff_triggerstep")->Fill("EFTrack",acceptData.getCutResult("EFTrack"));
        hist1("eff_triggerstep")->Fill("HLT",acceptData.getCutResult("HLT"));

        // Fill efficiency plot for HLT trigger steps
        if(!acceptData.getCutResult("HLT")/* || !acceptData.getCutResult("EFTrack")*/ || !acceptData.getCutResult("EFCalo") || 
           !acceptData.getCutResult("L2") || !acceptData.getCutResult("L2Calo")) {
            ATH_MSG_DEBUG("INEFF::Retrieve features for EF containers only ");
            ATH_MSG_DEBUG("INEFF::Retrieve EF Electron");
            const auto* EFEl = getFeature<xAOD::ElectronContainer>(feat);
            ATH_MSG_DEBUG("INEFF::Retrieve EF Photons");
            const auto* EFPh = getFeature<xAOD::PhotonContainer>(feat);
            ATH_MSG_DEBUG("INEFF::Retrieve EF Cluster");
            const auto* EFClus = getFeature<xAOD::CaloClusterContainer>(feat,"TrigEFCaloCalibFex");
            //ATH_MSG_DEBUG("INEFF::Retrieve EF Trk");
            //const auto* L2Trk = getFeature<xAOD::TrackParticleContainer>(feat);
            //const auto* L2Trk = getFeature<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_FTF");
            //const auto* EFIDTrk = getFeature<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_EFID");
            const auto* EFTrkIDTrig = getFeature<xAOD::TrackParticleContainer>(feat,"InDetTrigTrackingxAODCnv_Electron_IDTrig");
            dRmax = 0.15;
            // Find closest object for all objects
            selEF = closestObject<xAOD::Electron,xAOD::ElectronContainer>(pairObj, dRmax, false);
            selPh = closestObject<xAOD::Photon,xAOD::PhotonContainer>(pairObj, dRmax, false);
            selClus = closestObject<xAOD::CaloCluster,xAOD::CaloClusterContainer>(pairObj, dRmax, false,"TrigEFCaloCalibFex");
            selTrk = closestObject<xAOD::TrackParticle,xAOD::TrackParticleContainer>(pairObj, dRmax, false, "InDetTrigTrackingxAODCnv_Electron_IDTrig");
            fillInefficiency(pid, basePath, selEF, selPh, selClus, selTrk, acceptData);
            if (EFClus == nullptr){
                hist1("eff_hltreco")->Fill("ClusterCont", 0);
                hist1("eff_hltreco")->Fill("Cluster", 0);
                hist1("eff_hltreco")->Fill("ClusterMatch", 0);
            }
            else{
                hist1("eff_hltreco")->Fill("ClusterCont",1);
                if (EFClus->size() > 0){
                    hist1("eff_hltreco")->Fill("Cluster",1);
                    if (selClus) hist1("eff_hltreco")->Fill("ClusterMatch",1);
                    else hist1("eff_hltreco")->Fill("ClusterMatch", 0);
                }
                else{
                    hist1("eff_hltreco")->Fill("Cluster", 0);
                    hist1("eff_hltreco")->Fill("ClusterMatch", 0);
                }
            }

            if (EFTrkIDTrig == nullptr){
                hist1("eff_hltreco")->Fill("TrackCont", 0);
                hist1("eff_hltreco")->Fill("Track", 0);
                hist1("eff_hltreco")->Fill("TrackMatch", 0);
            }
            else{
                hist1("eff_hltreco")->Fill("TrackCont",1);
                if (EFTrkIDTrig->size() > 0){
                    hist1("eff_hltreco")->Fill("Track",1);
                    if (selTrk) hist1("eff_hltreco")->Fill("TrackMatch",1);
                    else hist1("eff_hltreco")->Fill("TrackMatch", 0);
                }
                else{
                    hist1("eff_hltreco")->Fill("Track", 0);
                    hist1("eff_hltreco")->Fill("TrackMatch", 0);
                }
            }

            if (EFPh == nullptr){
                hist1("eff_hltreco")->Fill("PhotonCont", 0);
                hist1("eff_hltreco")->Fill("Photon", 0);
                hist1("eff_hltreco")->Fill("PhotonMatch", 0);
            }
            else {
                hist1("eff_hltreco")->Fill("PhotonCont",1);
                if (EFPh->size() > 0){
                    hist1("eff_hltreco")->Fill("Photon",1);
                    if (selPh) hist1("eff_hltreco")->Fill("PhotonMatch",1);
                    else  hist1("eff_hltreco")->Fill("PhotonMatch", 0);
                }
                else{ 
                    hist1("eff_hltreco")->Fill("Photon", 0);
                    hist1("eff_hltreco")->Fill("PhotonMatch", 0);
                }
            }

            if (EFEl == nullptr){
                hist1("eff_hltreco")->Fill("ElectronCont", 0);
                hist1("eff_hltreco")->Fill("Electron", 0);
                hist1("eff_hltreco")->Fill("ElectronMatch", 0);
            }
            else {
                hist1("eff_hltreco")->Fill("ElectronCont",1);
                if (EFEl->size() > 0){
                    hist1("eff_hltreco")->Fill("Electron",1);
                    if(selEF) hist1("eff_hltreco")->Fill("ElectronMatch",1);
                    else hist1("eff_hltreco")->Fill("ElectronMatch", 0);
                }
                else{
                    hist1("eff_hltreco")->Fill("Electron", 0);
                    hist1("eff_hltreco")->Fill("ElectronMatch", 0);
                }
            }
        }
        else {
            if (boost::contains(pid, "LH"))
                for (int ibin = 0; ibin < hist1("IneffisEM" + pid)->GetNbinsX(); ibin++)
                    hist1("IneffisEM" + pid)->Fill(ibin + 0.5, 0);
            else {
               std::vector<std::string> groupnames {"Trk", "TrkClus", "TRT"};
               for (int ibin = 0; ibin < hist1("IneffisEM" + pid)->GetNbinsX(); ibin++)
                  hist1("IneffisEM" + pid)->Fill(ibin + 0.5, 0);
               for (const auto groupname : groupnames)
                  for (int ibin = 0; ibin < hist1("IneffisEM" + pid + groupname)->GetNbinsX(); ibin++)
                      hist1("IneffisEM" + pid + groupname)->Fill(ibin + 0.5, 0);
            }        
        }
    }
    ATH_MSG_DEBUG("End Inefficiency Analysis ======================= " << basePath);
}

void EfficiencyTool::fillEfficiency(const std::string dir,bool isPassed,const float etthr, const std::string pidword, const xAOD::Egamma *eg, bool fill2D)
{
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
    float pt = eg->pt()/1e3;
    float avgmu=getAvgMu();
    float npvtx=getNPVtx();
    ATH_MSG_DEBUG("Mu " << avgmu << " " << getAvgOnlineMu() << " "  << getAvgOfflineMu()); 
    ATH_MSG_DEBUG("PID decision efficiency " << eg->auxdecor<bool>(pidword));
    if(pid){
        hist1("et")->Fill(et);
        hist1("pt")->Fill(pt);
        hist1("highet")->Fill(et);
        if(et > etthr+1.0){
            hist1("eta")->Fill(eta);
            hist1("phi")->Fill(phi);
            hist1("mu")->Fill(avgmu);
            if(fill2D){
                if(m_detailedHists) hist2("et_eta")->Fill(et,eta);
                hist2("coarse_et_eta")->Fill(et,eta);
            }
            if(m_detailedHists)  hist1("npvtx")->Fill(npvtx);
        }
        if(isPassed) {
            hist1("match_et")->Fill(et);
            hist1("match_pt")->Fill(pt);
            hist1("match_highet")->Fill(et);
            if(et > etthr+1.0){
                hist1("match_eta")->Fill(eta);
                hist1("match_phi")->Fill(phi);
                hist1("match_mu")->Fill(avgmu);
                if(fill2D){
                    if(m_detailedHists) hist2("match_et_eta")->Fill(et,eta);
                    hist2("match_coarse_et_eta")->Fill(et,eta);
                }
                if(m_detailedHists)  hist1("match_npvtx")->Fill(npvtx);

            }
            hist1("eff_et")->Fill(et,1);
            hist1("eff_pt")->Fill(pt,1);
            hist1("eff_highet")->Fill(et,1);
            if(et > etthr+1.0){
                hist1("eff_eta")->Fill(eta,1);
                hist1("eff_phi")->Fill(phi,1);
                hist1("eff_mu")->Fill(avgmu,1);
                if(fill2D){
                    if(m_detailedHists) hist2("eff_et_eta")->Fill(et,eta,1);
                    hist2("eff_coarse_et_eta")->Fill(et,eta,1);
                }
                if(m_detailedHists)  hist1("eff_npvtx")->Fill(npvtx,1);
            }
        } // Passes Trigger selection
        else {
            hist1("eff_et")->Fill(et,0);
            hist1("eff_pt")->Fill(pt,0);
            hist1("eff_highet")->Fill(et,0);
            if(et > etthr+1.0){
                hist1("eff_eta")->Fill(eta,0);
                hist1("eff_phi")->Fill(phi,0);
                hist1("eff_mu")->Fill(avgmu,0);
                if(fill2D){
                    if(m_detailedHists) hist2("eff_et_eta")->Fill(et,eta,0);
                    hist2("eff_coarse_et_eta")->Fill(et,eta,0);
                }
                if(m_detailedHists)  hist1("eff_npvtx")->Fill(npvtx,0);
            }
        } // Fails Trigger selection
    } // Passes offline pid, fill histograms
}

StatusCode EfficiencyTool::toolExecute(const std::string basePath,const TrigInfo info, std::vector<std::pair< const xAOD::Egamma*,
                                       const HLT::TriggerElement*>> pairObjs)
{
    if(m_tp) return StatusCode::SUCCESS;
    
    // Removing Prescale check, in reprocessing L1AfterPrescale always false
    //
    /*if(isPrescaled(info.trigName)){
        ATH_MSG_DEBUG(info.trigName << " prescaled, skipping");
        return StatusCode::SUCCESS; 
    }*/
    
    const std::string dir = basePath+"/"+info.trigName;
    const float etthr = info.trigThrHLT;
    const std::string pidword = info.trigPidDecorator;
    const std::string pid = info.trigPidDecorator.substr(2);
    
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
            if(boost::contains(info.trigName,"icalovloose")) {
                if (getIsolation_topoetcone20(pairObj.first)/getCluster_et(pairObj.first) >= 0.065) continue; // pass FixedCutLoose offline isolation
            }
            else {
                if ((getIsolation_topoetcone40(pairObj.first)-2450.0)/getCluster_et(pairObj.first) >= 0.022) continue; // pass FixedCutTightCaloOnly offline isolation
            }
        } // Offline photon


        ATH_MSG_DEBUG("Fill probe histograms");
        
        std::vector<std::string> algnames;
        algnames.push_back("Efficiency");
        if( m_doEmulation ) algnames.push_back("Emulation");
        
        unsigned ialg=0;
        for (auto algname : algnames){
          
          // ialg = 0 is decision from TDT tool (Efficency dir) [default]
          // ialg = 1 is decision from emulator tool (Emulation dir)
          asg::AcceptData acceptData (&getAccept());
          if(ialg==0){
            acceptData = setAccept(pairObj.second,info); //Sets the trigger accepts
          }else{// ialg==1
            ATH_MSG_DEBUG("Fill efficiency from Emulation tool");
            acceptData = emulation()->executeTool(pairObj.second, info.trigName);
          }

          if (pairObj.second!=nullptr) {
              // Inefficiency analysis
              if(!info.trigL1){
                  if(pairObj.first->type()==xAOD::Type::Electron){
                      if(pairObj.first->auxdecor<bool>(pidword)){
                        inefficiency(pid,dir+"/"+algname+"/HLT",etthr,pairObj, acceptData);
                      }
                  }
              }
          } // Features

          if(info.trigL1)
              this->fillEfficiency(dir+"/"+algname+"/L1Calo",acceptData.getCutResult("L1Calo"),etthr,pidword,pairObj.first);
          else {
              this->fillEfficiency(dir+"/"+algname+"/HLT",acceptData.getCutResult("HLT"),etthr,pidword,pairObj.first);
              this->fillEfficiency(dir+"/"+algname+"/L2Calo",acceptData.getCutResult("L2Calo"),etthr,pidword,pairObj.first,m_detailedHists); 
              this->fillEfficiency(dir+"/"+algname+"/L2",acceptData.getCutResult("L2"),etthr,pidword,pairObj.first,m_detailedHists);
              this->fillEfficiency(dir+"/"+algname+"/EFCalo",acceptData.getCutResult("EFCalo"),etthr,pidword,pairObj.first,m_detailedHists);
              this->fillEfficiency(dir+"/"+algname+"/L1Calo",acceptData.getCutResult("L1Calo"),etthr,pidword,pairObj.first);
              if(m_detailedHists){
                  for(const auto pid : m_isemname) {
                      this->fillEfficiency(dir+"/"+algname+"/HLT/"+pid,acceptData.getCutResult("HLT"),etthr,"is"+pid,pairObj.first);
                      if( pairObj.first->auxdecor<bool>("Isolated") ) fillEfficiency(dir+"/"+algname+"/HLT/"+pid+"Iso",
                          acceptData.getCutResult("HLT"),etthr,"is"+pid,pairObj.first);
                  }
                  for(const auto pid : m_lhname) {
                      this->fillEfficiency(dir+"/"+algname+"/HLT/"+pid,acceptData.getCutResult("HLT"),etthr,"is"+pid,pairObj.first);
                      if( pairObj.first->auxdecor<bool>("Isolated") ) fillEfficiency(dir+"/"+algname+"/HLT/"+pid+"Iso",
                          acceptData.getCutResult("HLT"),etthr,"is"+pid,pairObj.first);
                  }
              }
              ATH_MSG_DEBUG("Complete efficiency");
          }
          
          ialg++;
        }// Loop over efficiency (and or) Emulation

    }// Loop over trigger electrons pairs

    return StatusCode::SUCCESS;
}
