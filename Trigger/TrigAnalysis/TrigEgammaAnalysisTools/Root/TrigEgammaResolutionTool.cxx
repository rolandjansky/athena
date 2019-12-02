/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaResolutionTool.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************

TrigEgammaResolutionTool::
TrigEgammaResolutionTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname) {
    m_detailedHists=false;
}

//**********************************************************************
StatusCode TrigEgammaResolutionTool::childInitialize(){


    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaResolutionTool::childBook()
{

    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaResolutionTool::childExecute()
{

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaResolutionTool::childFinalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaResolutionTool::toolExecute(const std::string basePath,TrigInfo info,
        std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>> pairObjs){

    if(m_tp) return StatusCode::SUCCESS;
    const std::string dir = basePath+"/"+info.trigName;
    bool filliso=false;
    if(boost::contains(info.trigName,"iloose") || boost::contains(info.trigName,"ivarloose")) filliso=true;
    if(boost::contains(info.trigName,"icaloloose") || boost::contains(info.trigName,"icalovloose") || boost::contains(info.trigName,"icalotight")) filliso=true;
    ATH_MSG_DEBUG("Executing resolution for " << dir);
    for(const auto pairObj : pairObjs){
        const xAOD::Egamma* eg =pairObj.first;
        const HLT::TriggerElement *feat = pairObj.second; 

        if ( feat!=nullptr ) {
            // Final cuts done here
            if (info.trigL1) {
                auto itEmTau = tdt()->ancestor<xAOD::EmTauRoI>(feat);
                const xAOD::EmTauRoI *l1 = itEmTau.cptr();
                if (l1) {
                    std::string dir7 = dir + "/Resolutions/L1Calo";
                    std::string dir8 = dir + "/AbsResolutions/L1Calo";
                    fillL1CaloResolution(dir7, l1, eg);
                    fillL1CaloAbsResolution(dir8, l1, eg);
                }
            }
            else {
                if(eg->type()==xAOD::Type::Electron){
                    const xAOD::Electron* el = static_cast<const xAOD::Electron *> (eg);
                    float et = getEt(el)/1e3;
                    if(et < info.trigThrHLT-5.0) continue; 
                    if(!eg->auxdecor<bool>(info.trigPidDecorator)) continue; 
                    resolutionElectron(dir,pairObj,filliso);
                } // Offline object Electron
                else if(eg->type()==xAOD::Type::Photon){
                    float et = getCluster_et(eg)/1e3;
                    if(et < info.trigThrHLT-5.0) continue; 
                    resolutionPhoton(dir,pairObj,filliso);
                } // Offline photon
            }
        }
    }
    return StatusCode::SUCCESS;
}

void TrigEgammaResolutionTool::resolutionPhoton(const std::string basePath,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj,bool filliso){
    ATH_MSG_DEBUG("Resolution photon "<< basePath);
    std::string dir1 = basePath + "/Resolutions/HLT";
    std::string dir2 = basePath + "/AbsResolutions/HLT";

    float dRmax = 100;
    const xAOD::Photon *phEF = nullptr;
    if(pairObj.second){
        if(ancestorPassed<xAOD::PhotonContainer>(pairObj.second))
            phEF=closestObject<xAOD::Photon,xAOD::PhotonContainer>(pairObj,dRmax);
        if(phEF){    
            if(dRmax < 0.05){
                fillHLTResolution(dir1,phEF,pairObj.first);
                if(filliso) fillIsolationResolution(dir1,phEF,pairObj.first);
                if(m_detailedHists) fillHLTAbsResolution(dir2,phEF,pairObj.first);
            }
        }
    }
}

void TrigEgammaResolutionTool::resolutionElectron(const std::string basePath,std::pair<const xAOD::Egamma*,const HLT::TriggerElement*> pairObj,bool filliso){
    ATH_MSG_DEBUG("Resolution Electron " << basePath);
    std::string dir1 = basePath + "/Resolutions/HLT";
    std::string dir2 = basePath + "/AbsResolutions/HLT";
    std::string dir5 = basePath + "/Resolutions/L2Calo";
    std::string dir6 = basePath + "/Resolutions/L2Calo_vs_HLT";
    std::string dir7 = basePath + "/Resolutions/L1Calo";
    std::string dir8 = basePath + "/AbsResolutions/L1Calo";

    
    float dRmax = 100;
    const xAOD::Electron *elEF = nullptr;
    if(pairObj.second){
        if(ancestorPassed<xAOD::ElectronContainer>(pairObj.second))
            elEF=closestObject<xAOD::Electron,xAOD::ElectronContainer>(pairObj,dRmax);
        if(elEF){    
            if(dRmax < 0.05){
                fillHLTResolution(dir1,elEF,pairObj.first);
                if(filliso) fillIsolationResolution(dir1,elEF,pairObj.first);
                if(m_detailedHists) fillHLTAbsResolution(dir2,elEF,pairObj.first);
                const xAOD::TrigEMCluster* clus = getFeature<xAOD::TrigEMCluster>(pairObj.second);
                if ( clus != nullptr ) {
                    if(m_detailedHists) fillL2CaloResolution(dir5,clus,pairObj.first);
                    fillL2CaloResolution(dir6,clus,elEF);
                }
            }
        } 
        // L1 resolutions
        auto itEmTau = tdt()->ancestor<xAOD::EmTauRoI>(pairObj.second);
        const xAOD::EmTauRoI *l1 = itEmTau.cptr();
        if (l1) {
            fillL1CaloResolution(dir7, l1, pairObj.first);
            fillL1CaloAbsResolution(dir8, l1, pairObj.first);
        }
    }
}

void TrigEgammaResolutionTool::resolutionL2Photon(const std::string dir,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    cd(dir);
    ATH_MSG_DEBUG("L2 Photon Resolution");
    float dRmax = 100;
    const xAOD::TrigPhoton *phL2 = nullptr;
    if(getSGContainsTrigPhoton()){
        if(pairObj.second){
            if(ancestorPassed<xAOD::TrigPhotonContainer>(pairObj.second))
                phL2=closestObject<xAOD::TrigPhoton,xAOD::TrigPhotonContainer>(pairObj,dRmax);
            if(phL2){    
                if(dRmax < 0.05){
                    //Do something here
                }
            }
        }
    }
}

void TrigEgammaResolutionTool::resolutionL2Electron(const std::string dir,std::pair< const xAOD::Egamma*,const HLT::TriggerElement*> pairObj){
    cd(dir);
    
    float dRmax = 100;
    const xAOD::TrigElectron *elL2 = nullptr;
    if(pairObj.second){
        if(ancestorPassed<xAOD::TrigElectronContainer>(pairObj.second))
            elL2=closestObject<xAOD::TrigElectron,xAOD::TrigElectronContainer>(pairObj,dRmax);
        if(elL2){    
            if(dRmax < 0.05){
                //Do something here
            }
        }
    }
}

void TrigEgammaResolutionTool::fillL1CaloResolution(const std::string dir,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off){
    cd(dir);
    ATH_MSG_DEBUG("Fill L1CaloResolution");
    if(off->type()==xAOD::Type::Electron){
      const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
      hist2("res_etVsEta")->Fill(l1->eta(),
                                 (l1->emClus()-getEt(eloff))/getEt(eloff));
    }
}

void TrigEgammaResolutionTool::fillL1CaloAbsResolution(const std::string dir,const xAOD::EmTauRoI *l1, const xAOD::Egamma *off){
    cd(dir);
    ATH_MSG_DEBUG("Fill L1CaloAbsResolution");
    if(off->type()==xAOD::Type::Electron){
      const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
      hist2("res_etVsEta")->Fill(l1->eta(),
                                 0.001*(l1->emClus()-getEt(eloff)));
    }
}
void TrigEgammaResolutionTool::fillHLTResolution(const std::string dir,const xAOD::Egamma *onl, const xAOD::Egamma *off){
    // Check for zero before filling
    cd(dir);
    ATH_MSG_DEBUG("Fill Resolution");
    float getOnlEt=0;
    float val_off=0.;
    const float onl_eta=onl->eta();
    const float feta = fabs(onl_eta);
    val_off=off->eta();
    if(val_off!=0.) hist1("res_eta")->Fill((onl_eta-val_off)/val_off);
    val_off=off->phi();
    if(val_off!=0.) hist1("res_phi")->Fill((onl->phi()-val_off)/val_off);
    if(xAOD::EgammaHelpers::isElectron(onl)){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        val_off=getTrack_pt(eloff);
        if(val_off!=0.) hist1("res_pt")->Fill((getTrack_pt(elonl)-val_off)/val_off);
        val_off=getEt(eloff);
        if(val_off!=0.) hist1("res_et")->Fill((getEt(elonl)-val_off)/val_off);
        getOnlEt = getEt(elonl);

        val_off=getEt(eloff);
        if(val_off!=0.) {
            hist2("res_etVsEta")->Fill(onl_eta,
                    (getEt(elonl)-val_off)/val_off);
            hist2("res_etVsEt")->Fill( getEt(elonl)/1e3,
                    (getEt(elonl)-val_off)/val_off);

            if( feta < 1.37 )
                hist1("res_etInEta0")->Fill((getEt(elonl)-val_off)/val_off);
            else if( feta >=1.37 && feta <= 1.52 )
                hist1("res_etInEta1")->Fill((getEt(elonl)-val_off)/val_off);
            else if( feta >= 1.52 && feta < 1.81 )
                hist1("res_etInEta2")->Fill((getEt(elonl)-val_off)/val_off);
            else if( feta >= 1.81 && feta < 2.47 )
                hist1("res_etInEta3")->Fill((getEt(elonl)-val_off)/val_off);
        }

        val_off=getCaloTrackMatch_deltaEta1(eloff);
        if(val_off!=0.) hist1("res_deta1")->Fill((getCaloTrackMatch_deltaEta1(elonl)-val_off)/val_off);
        val_off=getCaloTrackMatch_deltaEta2(eloff);
        hist1("res_deta2")->Fill((getCaloTrackMatch_deltaEta2(elonl)-val_off)/val_off);
        val_off=getCaloTrackMatch_deltaPhi2(eloff);
        if(val_off!=0.) hist1("res_dphi2")->Fill((getCaloTrackMatch_deltaPhi2(elonl)-val_off)/val_off);
        val_off=getCaloTrackMatch_deltaPhiRescaled2(eloff);
        hist1("res_dphiresc")->Fill((getCaloTrackMatch_deltaPhiRescaled2(elonl)-val_off)/val_off);

        // Absolute resolution for impact parameter
        val_off=getTrack_d0(eloff);
        if(val_off!=0.) hist1("res_d0")->Fill(getTrack_d0(elonl)-val_off);
        val_off=getD0sig(eloff);
        if(val_off!=0.) hist1("res_d0sig")->Fill(getD0sig(elonl)-val_off);
        
        // Absolute resolution on track summary ints/floats 
        val_off=getTrackSummaryFloat_eProbabilityHT(eloff);
        hist1("res_eprobht")->Fill( (getTrackSummaryFloat_eProbabilityHT(elonl)-val_off));
        hist2("res_eprobht_onVsOff")->Fill(val_off,
                getTrackSummaryFloat_eProbabilityHT(elonl));
        hist2("res_eprobhtVsPt")->Fill(getTrack_pt(elonl)/1e3,
                (getTrackSummaryFloat_eProbabilityHT(elonl) - val_off));
        hist1("res_npixhits")->Fill(getTrackSummary_numberOfPixelHits(elonl)-getTrackSummary_numberOfPixelHits(elonl));
        hist1("res_nscthits")->Fill(getTrackSummary_numberOfSCTHits(elonl)-getTrackSummary_numberOfSCTHits(elonl));

    } else { 
        val_off=getCluster_et(off);
        if(val_off!=0.){
            hist1("res_et")->Fill((getCluster_et(onl)-val_off)/val_off);

            hist2("res_etVsEta")->Fill(onl_eta,
                    (getCluster_et(onl)-val_off)/val_off);
            hist2("res_etVsEt")->Fill( getCluster_et(onl)/1e3,
                    (getCluster_et(onl)-val_off)/val_off);
            const xAOD::Photon* phoff =static_cast<const xAOD::Photon*> (off);
            if(xAOD::EgammaHelpers::isConvertedPhoton(phoff)) {
                hist1("res_et_cnv")->Fill((getCluster_et(onl)-val_off)/val_off);
                hist2("res_cnv_etVsEta")->Fill(onl_eta,
                        (getCluster_et(onl)-val_off)/val_off);
                hist2("res_cnv_etVsEt")->Fill( getCluster_et(onl)/1e3,
                        (getCluster_et(onl)-val_off)/val_off);

                if( feta < 1.37 )
                    hist1("res_cnv_etInEta0")->Fill((getCluster_et(onl)-val_off)/val_off);
                else if( feta >=1.37 && feta <= 1.52 )
                    hist1("res_cnv_etInEta1")->Fill((getCluster_et(onl)-val_off)/val_off);
                else if( feta >= 1.52 && feta < 1.81 )
                    hist1("res_cnv_etInEta2")->Fill((getCluster_et(onl)-val_off)/val_off);
                else if( feta >= 1.81 && feta < 2.47 )
                    hist1("res_cnv_etInEta3")->Fill((getCluster_et(onl)-val_off)/val_off);
            }
            else {
                hist1("res_et_uncnv")->Fill((getCluster_et(onl)-val_off)/val_off);
                hist2("res_uncnv_etVsEta")->Fill(onl->eta(),
                        (getCluster_et(onl)-val_off)/val_off);
                hist2("res_uncnv_etVsEt")->Fill( getCluster_et(onl)/1e3,
                        (getCluster_et(onl)-val_off)/val_off);

                if( feta < 1.37 )
                    hist1("res_uncnv_etInEta0")->Fill((getCluster_et(onl)-val_off)/val_off);
                else if( feta >=1.37 && feta <= 1.52 )
                    hist1("res_uncnv_etInEta1")->Fill((getCluster_et(onl)-val_off)/val_off);
                else if( feta >= 1.52 && feta < 1.81 )
                    hist1("res_uncnv_etInEta2")->Fill((getCluster_et(onl)-val_off)/val_off);
                else if( feta >= 1.81 && feta < 2.47 )
                    hist1("res_uncnv_etInEta3")->Fill((getCluster_et(onl)-val_off)/val_off);
            }

            if( feta < 1.37 )
                hist1("res_etInEta0")->Fill((getCluster_et(onl)-val_off)/val_off);
            else if( feta >=1.37 && feta <= 1.52 )
                hist1("res_etInEta1")->Fill((getCluster_et(onl)-val_off)/val_off);
            else if( feta >= 1.55 && feta < 1.8 )
                hist1("res_etInEta2")->Fill((getCluster_et(onl)-val_off)/val_off);
            else if( feta >= 1.8 && feta < 2.45 )
                hist1("res_etInEta3")->Fill((getCluster_et(onl)-val_off)/val_off);
        } // Cluster Energy !=0.
    }

    /*val_off=getShowerShape_e011(off);
    if(val_off!=0.) hist1("res_e011")->Fill((getShowerShape_e011(onl)-val_off)/val_off);
    val_off=getShowerShape_e132(off);
    if(val_off!=0.) hist1("res_e132")->Fill((getShowerShape_e132(onl)-val_off)/val_off);
    val_off=getShowerShape_e237(off);
    if(val_off!=0.) hist1("res_e237")->Fill((getShowerShape_e237(onl)-val_off)/val_off);
    val_off=getShowerShape_e277(off);
    if(val_off!=0.) hist1("res_e277")->Fill((getShowerShape_e277(onl)-val_off)/val_off);*/
    val_off=getShowerShape_ethad(off);
    if(val_off!=0.) {
        hist1("res_ethad")->Fill((getShowerShape_ethad(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_ethadVsEta")->Fill(onl->eta(),
                    (getShowerShape_ethad(onl)-val_off)/val_off);
            hist2("res_ethadVsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_ethad(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_ethad1(off);
    if(val_off!=0){
        hist1("res_ethad1")->Fill((getShowerShape_ethad1(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_ethad1VsEta")->Fill(onl->eta(),
                    (getShowerShape_ethad1(onl)-val_off)/val_off);
            hist2("res_ethad1VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_ethad1(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_Rhad(off);
    if(val_off!=0.){
        hist1("res_Rhad")->Fill((getShowerShape_Rhad(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_RhadVsEta")->Fill(onl->eta(),
                    (getShowerShape_Rhad(onl)-val_off)/val_off);
            hist2("res_RhadVsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_Rhad(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_Rhad1(off);
    if(val_off!=0.){
        hist1("res_Rhad1")->Fill((getShowerShape_Rhad1(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_Rhad1VsEta")->Fill(onl->eta(),
                    (getShowerShape_Rhad1(onl)-val_off)/val_off);
            hist2("res_Rhad1VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_Rhad1(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_Reta(off);
    if(val_off!=0.){
        hist1("res_Reta")->Fill((getShowerShape_Reta(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_RetaVsEta")->Fill(onl->eta(),
                    (getShowerShape_Reta(onl)-val_off)/val_off);
            hist2("res_RetaVsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_Reta(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_Rphi(off);
    if(val_off!=0.){
        hist1("res_Rphi")->Fill((getShowerShape_Rphi(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_RphiVsEta")->Fill(onl->eta(),
                    (getShowerShape_Rphi(onl)-val_off)/val_off);
            hist2("res_RphiVsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_Rphi(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_weta1(off);
    if(val_off!=0.){
        hist1("res_weta1")->Fill((getShowerShape_weta1(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_weta1VsEta")->Fill(onl->eta(),
                    (getShowerShape_weta1(onl)-val_off)/val_off);
            hist2("res_weta1VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_weta1(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_weta2(off);
    if(val_off!=0.){
        hist1("res_weta2")->Fill((getShowerShape_weta2(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_weta2VsEta")->Fill(onl->eta(),
                    (getShowerShape_weta2(onl)-val_off)/val_off);
            hist2("res_weta2VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_weta2(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_wtots1(off);
    if(val_off!=0.){
        hist1("res_wtots1")->Fill((getShowerShape_wtots1(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_weta2VsEta")->Fill(onl->eta(),
                    (getShowerShape_wtots1(onl)-val_off)/val_off);
            hist2("res_weta2VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_wtots1(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_f1(off);
    if(val_off!=0.){
        hist1("res_f1")->Fill((getShowerShape_f1(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_f1VsEta")->Fill(onl->eta(),
                    (getShowerShape_f1(onl)-val_off)/val_off);
            hist2("res_f1VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_f1(onl)-val_off)/val_off);
        }
    }
    val_off=getShowerShape_f3(off);
    if(val_off!=0.){
        hist1("res_f3")->Fill((getShowerShape_f3(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_f3VsEta")->Fill(onl->eta(),
                    (getShowerShape_f3(onl)-val_off)/val_off);
            hist2("res_f3VsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_f3(onl)-val_off)/val_off);

        }
    }
    val_off=getShowerShape_Eratio(off);
    if(val_off!=0.){
        hist1("res_eratio")->Fill((getShowerShape_Eratio(onl)-val_off)/val_off);
        if (m_detailedHists ) {
            hist2("res_eratioVsEta")->Fill(onl->eta(),
                    (getShowerShape_Eratio(onl)-val_off)/val_off);
            hist2("res_eratioVsEt")->Fill( getOnlEt/1e3,
                    (getShowerShape_Eratio(onl)-val_off)/val_off);
        }
    }
}

void TrigEgammaResolutionTool::fillIsolationResolution(const std::string dir,const xAOD::Egamma *onl, const xAOD::Egamma *off){
        // ptcone20 isolation
    cd(dir);
    if(xAOD::EgammaHelpers::isElectron(onl)){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        float val_off=getIsolation_ptcone20(eloff);
        hist2("res_ptcone20_onVsOff")->Fill(val_off,
                getIsolation_ptcone20(elonl));
        if (getEt(elonl) > 0. && getEt(eloff) > 0.){ 
            hist2("res_ptvarcone20_rel_onVsOff")->Fill(getIsolation_ptvarcone20(eloff)/getEt(eloff),
                    getIsolation_ptvarcone20(elonl)/getEt(elonl));
            hist2("res_ptcone20_rel_onVsOff")->Fill(getIsolation_ptcone20(eloff)/getEt(eloff),
                    getIsolation_ptcone20(elonl)/getEt(elonl));
        }
        if (val_off > 0.) {
            hist1("res_ptcone20")->Fill((getIsolation_ptcone20(elonl)-val_off)/val_off);
            if (getEt(elonl) > 0. && getEt(eloff) > 0.) {
                const float reliso_onl=getIsolation_ptcone20(elonl)/getEt(elonl);
                const float reliso_off=getIsolation_ptcone20(eloff)/getEt(eloff);
                hist1("res_ptcone20_rel")->Fill((reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptcone20_relVsEta")->Fill(elonl->eta(),
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptcone20_relVsEt")->Fill(getEt(elonl)/1e3,
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptcone20_relVsMu")->Fill(getAvgMu(),
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptcone20VsMu")->Fill(getAvgMu(),
                        (reliso_onl-reliso_off)/reliso_off);
            }
        }

        // ptvarcone20 isolation
        val_off=getIsolation_ptvarcone20(eloff);
        hist2("res_ptvarcone20_onVsOff")->Fill(val_off,
                getIsolation_ptvarcone20(elonl));
        if (val_off > 0.) {
            if (getEt(elonl) > 0. && getEt(eloff) > 0.) {
                hist1("res_ptvarcone20")->Fill((getIsolation_ptvarcone20(elonl)-val_off)/val_off);
                const float reliso_onl=getIsolation_ptvarcone20(elonl)/getEt(elonl);
                const float reliso_off=getIsolation_ptvarcone20(eloff)/getEt(eloff);
                hist1("res_ptvarcone20_rel")->Fill((reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptvarcone20_relVsEta")->Fill(elonl->eta(),
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptvarcone20_relVsEt")->Fill(getEt(elonl)/1e3,
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptvarcone20_relVsMu")->Fill(getAvgMu(),
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_ptvarcone20VsMu")->Fill(getAvgMu(),
                        (reliso_onl-reliso_off)/reliso_off);
            }
        }
    }//Electron
    if(xAOD::EgammaHelpers::isPhoton(onl)){
        // topoetcone20 isolation
        float val_off=getIsolation_topoetcone20(off);
        float etonl=onl->pt();
        float etoff=off->pt();
        if (val_off > 0.) {
            hist1("res_topoetcone20")->Fill((getIsolation_topoetcone20(onl)-val_off)/val_off);
            if (etonl > 0. && etoff > 0.) {
                const float reliso_onl=getIsolation_topoetcone20(onl)/etonl;
                const float reliso_off=getIsolation_topoetcone20(off)/etoff;
                hist1("res_topoetcone20_rel")->Fill((reliso_onl-reliso_off)/reliso_off);
                hist2("res_topoetcone20_relVsEta")->Fill(onl->eta(),
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_topoetcone20_relVsEt")->Fill(etonl/1e3,
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_topoetcone20_relVsMu")->Fill(getAvgMu(),
                        (reliso_onl-reliso_off)/reliso_off);
                hist2("res_topoetcone20VsMu")->Fill(getAvgMu(),
                        (reliso_onl-reliso_off)/reliso_off);
            }
        }
        hist2("res_topoetcone20_onVsOff")->Fill(getIsolation_topoetcone20(off)/1e3,
                    getIsolation_topoetcone20(onl)/1e3);
        hist2("res_topoetcone40_shift_onVsOff")->Fill((getIsolation_topoetcone40(off)-2450)/1e3,
                    (getIsolation_topoetcone40(onl)-2450)/1e3);
        if (etonl > 0. && etoff > 0.) {
            hist2("res_topoetcone20_rel_onVsOff")->Fill(getIsolation_topoetcone20(off)/etoff,
                        getIsolation_topoetcone20(onl)/etonl);
            hist2("res_topoetcone40_shift_rel_onVsOff")->Fill((getIsolation_topoetcone40(off)-2450)/etoff,
                        (getIsolation_topoetcone40(onl)-2450)/etonl);
        }
    }
}

void TrigEgammaResolutionTool::fillHLTAbsResolution(const std::string dir,const xAOD::Egamma *onl, const xAOD::Egamma *off){
   
    cd(dir);
    ATH_MSG_DEBUG("Fill Abs Resolution");
    if(xAOD::EgammaHelpers::isElectron(onl)){
        const xAOD::Electron* elonl =static_cast<const xAOD::Electron*> (onl);
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);
        hist1("res_pt")->Fill((getTrack_pt(elonl)-getTrack_pt(eloff)));
        hist1("res_et")->Fill((getEt(elonl)-getEt(eloff))/getEt(eloff));
        
        const float onl_eta=onl->eta();
        const float feta = fabs(onl_eta);
        const float off_eta=off->eta();
        hist1("res_eta")->Fill(onl_eta-off_eta);
        hist1("res_phi")->Fill((elonl->phi()-eloff->phi()));

        hist2("res_etVsEta")->Fill(elonl->eta(),
                                   (getEt(elonl)-getEt(eloff)));
        hist2("res_etVsEt")->Fill( getEt(elonl)/1e3,
                                   (getEt(elonl)-getEt(eloff)));

        hist1("res_ptcone20")->Fill(getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff));

        //ptcone20/pt
        if (getEt(elonl) > 0 && getEt(eloff) > 0) {
          hist1("res_ptcone20_rel")->Fill(getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
          hist2("res_ptcone20_relVsEta")->Fill(elonl->eta(),
                                               getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
          hist2("res_ptcone20_relVsEt")->Fill(getEt(elonl)/1e3,
                                              getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
          hist2("res_ptcone20_relVsMu")->Fill(getAvgMu(),
                                              getIsolation_ptcone20(elonl)/getEt(elonl)-getIsolation_ptcone20(eloff)/getEt(eloff));
        }
        //ptcone20
        hist2("res_ptcone20VsMu")->Fill(getAvgMu(),
                                        getIsolation_ptcone20(elonl)-getIsolation_ptcone20(eloff));
        hist2("res_ptcone20_onVsOff")->Fill(getIsolation_ptcone20(eloff),
                                        getIsolation_ptcone20(elonl));
        if (getEt(elonl) > 0 && getEt(eloff) > 0) {
          hist2("res_ptcone20_rel_onVsOff")->Fill(getIsolation_ptcone20(eloff)/getEt(eloff),
                                              getIsolation_ptcone20(elonl)/getEt(elonl));
        }

	if( feta < 1.37 )
	  hist1("res_etInEta0")->Fill((getEt(elonl)-getEt(eloff)));
	else if( feta >=1.37 && feta <= 1.52 )
	  hist1("res_etInEta1")->Fill((getEt(elonl)-getEt(eloff)));
	else if( feta >= 1.55 && feta < 1.8 )
	  hist1("res_etInEta2")->Fill((getEt(elonl)-getEt(eloff)));
	else if( feta >= 1.8 && feta < 2.45 )
	  hist1("res_etInEta3")->Fill((getEt(elonl)-getEt(eloff)));
        
        hist1("res_deta1")->Fill((getCaloTrackMatch_deltaEta1(elonl)-getCaloTrackMatch_deltaEta1(eloff)));
        hist1("res_deta2")->Fill((getCaloTrackMatch_deltaEta2(elonl)-getCaloTrackMatch_deltaEta2(eloff)));
        hist1("res_dphi2")->Fill((getCaloTrackMatch_deltaPhi2(elonl)-getCaloTrackMatch_deltaPhi2(eloff)));
        hist1("res_dphiresc")->Fill((getCaloTrackMatch_deltaPhiRescaled2(elonl)-getCaloTrackMatch_deltaPhiRescaled2(eloff)));
        hist1("res_d0")->Fill((getTrack_d0(elonl)-getTrack_d0(eloff)));
        hist1("res_d0sig")->Fill((getD0sig(elonl)-getD0sig(eloff)));
        hist1("res_eprobht")->Fill( (getTrackSummaryFloat_eProbabilityHT(elonl) - getTrackSummaryFloat_eProbabilityHT(eloff)));
        hist1("res_npixhits")->Fill(getTrackSummary_numberOfPixelHits(elonl)-getTrackSummary_numberOfPixelHits(elonl));
        hist1("res_nscthits")->Fill(getTrackSummary_numberOfSCTHits(elonl)-getTrackSummary_numberOfSCTHits(elonl));
    }
    else{ 
      hist1("res_et")->Fill((getCluster_et(onl)-getCluster_et(off)));
      hist1("res_eta")->Fill((onl->eta()-off->eta()));
      hist1("res_phi")->Fill((onl->phi()-off->phi()));

      hist2("res_etVsEta")->Fill(onl->eta(),
				 (getCluster_et(onl)-getCluster_et(off)));
      hist2("res_etVsEt")->Fill( getCluster_et(onl)/1e3,
				 (getCluster_et(onl)-getCluster_et(off)));
      float feta = fabs(onl->eta());
      if( feta < 1.37 )
	hist1("res_etInEta0")->Fill((getCluster_et(onl)-getCluster_et(off)));
      else if( feta >=1.37 && feta <= 1.52 )
	hist1("res_etInEta1")->Fill((getCluster_et(onl)-getCluster_et(off)));
      else if( feta >= 1.55 && feta < 1.8 )
	hist1("res_etInEta2")->Fill((getCluster_et(onl)-getCluster_et(off)));
      else if( feta >= 1.8 && feta < 2.45 )
	hist1("res_etInEta3")->Fill((getCluster_et(onl)-getCluster_et(off)));
    }
    
    /*hist1("res_e011")->Fill((getShowerShape_e011(onl)-getShowerShape_e011(off)));
    hist1("res_e132")->Fill((getShowerShape_e132(onl)-getShowerShape_e132(off)));
    hist1("res_e237")->Fill((getShowerShape_e237(onl)-getShowerShape_e237(off)));
    hist1("res_e277")->Fill((getShowerShape_e277(onl)-getShowerShape_e277(off)));*/
    hist1("res_ethad")->Fill((getShowerShape_ethad(onl)-getShowerShape_ethad(off)));
    hist1("res_ethad1")->Fill((getShowerShape_ethad1(onl)-getShowerShape_ethad1(off)));
    hist1("res_Rhad")->Fill((getShowerShape_Rhad(onl)-getShowerShape_Rhad(off)));
    hist1("res_Rhad1")->Fill((getShowerShape_Rhad1(onl)-getShowerShape_Rhad1(off)));
    hist1("res_Reta")->Fill((getShowerShape_Reta(onl)-getShowerShape_Reta(off)));
    hist1("res_Rphi")->Fill((getShowerShape_Rphi(onl)-getShowerShape_Rphi(off)));
    hist1("res_weta1")->Fill((getShowerShape_weta1(onl)-getShowerShape_weta1(off)));
    hist1("res_weta2")->Fill((getShowerShape_weta2(onl)-getShowerShape_weta2(off)));
    hist1("res_wtots1")->Fill((getShowerShape_wtots1(onl)-getShowerShape_wtots1(off)));
    hist1("res_f1")->Fill((getShowerShape_f1(onl)-getShowerShape_f1(off)));
    hist1("res_f3")->Fill((getShowerShape_f3(onl)-getShowerShape_f3(off)));
    hist1("res_eratio")->Fill((getShowerShape_Eratio(onl)-getShowerShape_Eratio(off)));
}

void TrigEgammaResolutionTool::fillL2CaloResolution(const std::string dir,const xAOD::TrigEMCluster *onl, const xAOD::Egamma *off){

    cd(dir);
    ATH_MSG_DEBUG("Fill Resolution");
    float val_off=0.;
    if(xAOD::EgammaHelpers::isElectron(off)){
        const xAOD::TrigEMCluster* elonl =onl;
        const xAOD::Electron* eloff =static_cast<const xAOD::Electron*> (off);

        val_off=getEt(eloff);
        if(val_off!=0.){
            hist1("res_et")->Fill(((elonl->et())-val_off)/val_off);
            hist2("res_etVsEta")->Fill(elonl->eta(),
                    ((elonl->et())-val_off)/val_off);
            hist2("res_etVsEt")->Fill( (elonl->et())/1e3,
                    ((elonl->et())-val_off)/val_off);
        }
        val_off=eloff->caloCluster()->eta();
        if(val_off!=0.) hist1("res_eta")->Fill((elonl->eta()-val_off)/val_off);
        val_off=eloff->caloCluster()->phi();
        if(val_off!=0.) hist1("res_phi")->Fill((elonl->phi()-val_off)/val_off);


        float elonl_ethad = elonl->energy( CaloSampling::HEC0 ); elonl_ethad += elonl->energy( CaloSampling::HEC1 );
        elonl_ethad += elonl->energy( CaloSampling::HEC2 ); elonl_ethad += elonl->energy( CaloSampling::HEC3 );
        elonl_ethad += elonl->energy( CaloSampling::TileBar0 ); elonl_ethad += elonl->energy( CaloSampling::TileExt0 ); 
        elonl_ethad += elonl->energy( CaloSampling::TileBar1 ); elonl_ethad += elonl->energy( CaloSampling::TileExt1 ); 
        elonl_ethad += elonl->energy( CaloSampling::TileBar2 ); elonl_ethad += elonl->energy( CaloSampling::TileExt2 ); 
        elonl_ethad /= TMath::CosH(elonl->eta() );
        val_off=getShowerShape_ethad(off);
        if(val_off!=0.){
            hist1("res_ethad")->Fill((elonl_ethad-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_ethadVsEta")->Fill(elonl->eta(),
                        (elonl_ethad-val_off)/val_off);
                hist2("res_ethadVsEt")->Fill( elonl->et()/1e3,
                        (elonl_ethad-val_off)/val_off);
            }
        }
        val_off=getShowerShape_ethad1(off);
        if(val_off!=0.){
            hist1("res_ethad1")->Fill(( (onl->ehad1()/TMath::Abs(onl->eta()) )-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_ethad1VsEta")->Fill(elonl->eta(),
                        ( (elonl->ehad1()/TMath::Abs(onl->eta()) )-val_off)/val_off);
                hist2("res_ethad1VsEt")->Fill( elonl->et()/1e3,
                        ( (elonl->ehad1()/TMath::Abs(onl->eta()) )-val_off)/val_off);
            }
        }
        float elonl_Rhad = elonl_ethad / onl->energy() ;
        val_off=getShowerShape_Rhad(off);
        if(val_off!=0.){
            hist1("res_Rhad")->Fill(( elonl_Rhad-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_RhadVsEta")->Fill(elonl->eta(),
                        ( elonl_Rhad-val_off)/val_off);
                hist2("res_RhadVsEt")->Fill( elonl->et()/1e3,
                        ( elonl_Rhad-val_off)/val_off);
            }
        }
        float elonl_Rhad1 = onl->ehad1() / onl->energy() ;
        val_off=getShowerShape_Rhad1(off);
        if(val_off!=0.){
            hist1("res_Rhad1")->Fill(( elonl_Rhad1-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_Rhad1VsEta")->Fill(elonl->eta(),
                        ( elonl_Rhad1-val_off)/val_off);
                hist2("res_Rhad1VsEt")->Fill( elonl->et()/1e3,
                        ( elonl_Rhad1-val_off)/val_off);
            }
        }
        float onl_reta= 999.0;
        if ( fabsf ( onl->e277() ) > 0.01 ) onl_reta = onl->e237() / onl->e277();
        val_off=getShowerShape_Reta(off);
        if(val_off!=0.){
            hist1("res_Reta")->Fill( (onl_reta -val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_RetaVsEta")->Fill(elonl->eta(),
                        ( onl_reta-val_off)/val_off);
                hist2("res_RetaVsEt")->Fill( elonl->et()/1e3,
                        ( onl_reta-val_off)/val_off);
            }
        }
        val_off=getShowerShape_weta2(off);
        if(val_off!=0.){
            hist1("res_weta2")->Fill(( (onl->weta2())-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_weta2VsEta")->Fill(elonl->eta(),
                        ( (elonl->weta2())-val_off)/val_off);
                hist2("res_weta2VsEt")->Fill( elonl->et()/1e3,
                        ( (elonl->weta2())-val_off)/val_off);
            }
        }

        float onl_f1 = onl->energy(CaloSampling::EMB1)+onl->energy(CaloSampling::EME1);
        onl_f1 /= onl->energy();
        val_off=getShowerShape_f1(off);
        if(val_off!=0.){
            hist1("res_f1")->Fill(( (onl_f1)-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_f1VsEta")->Fill(elonl->eta(),
                        ( (onl_f1)-val_off)/val_off);
                hist2("res_f1VsEt")->Fill( elonl->et()/1e3,
                        ( (onl_f1)-val_off)/val_off);
            }
        }
        float onl_f3 = onl->energy(CaloSampling::EMB3)+onl->energy(CaloSampling::EME3);
        onl_f3 /= onl->energy();
        val_off=getShowerShape_f3(off);
        if(val_off!=0.){
            hist1("res_f3")->Fill(( (onl_f3)-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_f3VsEta")->Fill(elonl->eta(),
                        ( (onl_f3)-val_off)/val_off);
                hist2("res_f3VsEt")->Fill( elonl->et()/1e3,
                        ( (onl_f3)-val_off)/val_off);
            }
        }
        float onl_eratio = 999.0;
        if ( fabsf(onl->emaxs1() + onl->e2tsts1()) > 0.01 ) 
            onl_eratio = (onl->emaxs1() - onl->e2tsts1()) / (onl->emaxs1() + onl->e2tsts1());
        val_off=getShowerShape_Eratio(off);
        if(val_off!=0.){
            hist1("res_eratio")->Fill(( (onl_eratio)-val_off)/val_off);
            if (m_detailedHists ) {
                hist2("res_eratioVsEta")->Fill(elonl->eta(),
                        ( (onl_eratio)-val_off)/val_off);
                hist2("res_eratioVsEt")->Fill( elonl->et()/1e3,
                        ( (onl_eratio)-val_off)/val_off);
            }
        }
    } // Electron
}
