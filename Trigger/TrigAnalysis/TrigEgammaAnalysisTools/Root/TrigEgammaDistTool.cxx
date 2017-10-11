/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaDistTool.h"

#include "xAODTrigger/TrigPassBits.h"
#include "xAODTrigger/TrigPassBitsContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

//**********************************************************************

TrigEgammaDistTool::
TrigEgammaDistTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname) {
    m_detailedHists=false;
}

//**********************************************************************
StatusCode TrigEgammaDistTool::childInitialize(){


    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaDistTool::childBook()
{

    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaDistTool::childExecute()
{

    return StatusCode::SUCCESS;
}

StatusCode TrigEgammaDistTool::childFinalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaDistTool::toolExecute(const std::string basePath,TrigInfo info,
        std::vector<std::pair< const xAOD::Egamma*,const HLT::TriggerElement*>> pairObjs){
    ATH_MSG_DEBUG("Executing Distribution for " << basePath);
  
    // Require that at least 1 offline selected object in event
    // Trigger distributions not entirely dependent from offline analysis
    //
    if(pairObjs.size() == 0) return StatusCode::SUCCESS;
    const std::string dir = basePath+"/"+info.trigName+"/Distributions/";
    const auto fc = (tdt()->features(info.trigName, TrigDefs::alsoDeactivateTEs));
    // Retrieve FeatureContainer for a given trigger
    ATH_MSG_DEBUG("Distributions:: Retrieve features for chain " << info.trigName << " type " << info.trigType);
    if (boost::starts_with(info.trigName, "L1" )){
        const auto initRois = fc.get<TrigRoiDescriptor>();
        ATH_MSG_DEBUG("Size of initialRoI" << initRois.size());
        for(const auto feat : initRois){
            if(feat.te()==nullptr) {
                ATH_MSG_DEBUG("initial RoI feature nullptr");
                continue;
            }
            const TrigRoiDescriptor *roi = feat.cptr();
            cd(dir+"RoI");
            hist1("roi_eta")->Fill(roi->eta());
            hist1("roi_phi")->Fill(roi->phi());
            const auto itEmTau = tdt()->ancestor<xAOD::EmTauRoI>(feat);
            const xAOD::EmTauRoI *l1 = itEmTau.cptr();
            fillL1Calo(dir+"L1Calo",l1);
        }
    }
    else {
        
        for(const auto pairObj : pairObjs){
            fillShowerShapes(dir+"Offline",pairObj.first); // Fill Offline shower shapes
            if(xAOD::EgammaHelpers::isElectron(pairObj.first)) { // Fill offline tracking
                const xAOD::Electron* elOff =static_cast<const xAOD::Electron*> (pairObj.first);
                fillTracking(dir+"Offline",elOff);
            }
        }

        // Only fill distributions for TP triggers 
        if(m_tp){
            if(info.trigType=="electron"){
                const auto vec_el = fc.get<xAOD::ElectronContainer>("egamma_Electrons",TrigDefs::alsoDeactivateTEs);
                for (const auto feat : vec_el){
                    if(feat.te()==nullptr) continue;
                    const auto* cont = getFeature<xAOD::ElectronContainer>(feat.te());
                    std::unique_ptr<xAOD::TrigPassBits> bits = getBits<xAOD::ElectronContainer>(feat.te(),cont);
                    if(cont==nullptr) continue; 
                    cd(dir+"HLT");
                    if(ancestorPassed<xAOD::ElectronContainer>(feat.te()))
                        hist1("rejection")->Fill("HLT",1);
                    for(const auto& obj : *cont){
                        if(!obj) continue;
                        if(bits==nullptr){
                            fillShowerShapes(dir+"HLT",obj); // Fill HLT shower shapes
                            fillTracking(dir+"HLT",obj); // Fill HLT shower shapes
                        }
                        // Only consider passing objects if bits available
                        else if(!bits->isPassing(obj,cont)) continue;
                        fillShowerShapes(dir+"HLT",obj); // Fill HLT shower shapes
                        fillTracking(dir+"HLT",obj); // Fill HLT shower shapes
                    }
                }
            }
            else ATH_MSG_WARNING("Chain type not Electron for TP Trigger");
        }
        else {
            // Get the L1 features for all TEs

            const std::string l1trig=getL1Item(info.trigName);
            ATH_MSG_DEBUG("Distributions:: Retrieve features for chain " << info.trigName << " type " << info.trigType << l1trig);
            const auto fcl1 = (tdt()->features(l1trig, TrigDefs::alsoDeactivateTEs));

            const auto initRois = fcl1.get<TrigRoiDescriptor>();
            ATH_MSG_DEBUG("Size of initialRoI" << initRois.size());
            for(const auto feat : initRois){
                if(feat.te()==nullptr) {
                    ATH_MSG_DEBUG("initial RoI feature nullptr");
                    continue;
                }
                const TrigRoiDescriptor *roi = feat.cptr();
                cd(dir+"HLT");
                hist1("rejection")->Fill("L1Calo",1);
                cd(dir+"RoI");
                hist1("roi_eta")->Fill(roi->eta());
                hist1("roi_phi")->Fill(roi->phi());
                auto itEmTau = tdt()->ancestor<xAOD::EmTauRoI>(feat);
                const xAOD::EmTauRoI *l1 = itEmTau.cptr();
                if(l1==nullptr) continue;
                fillL1Calo(dir+"L1Calo",l1);
            }
            const auto vec_l2em = fc.get<xAOD::TrigEMCluster>("",TrigDefs::alsoDeactivateTEs);
            for (const auto feat : vec_l2em){
                if(feat.te()==nullptr) continue;
                const auto* obj = getFeature<xAOD::TrigEMCluster>(feat.te());
                // Only consider passing objects
                if(!obj) continue;
                cd(dir+"HLT");
                if(ancestorPassed<xAOD::TrigEMCluster>(feat.te()))
                  hist1("rejection")->Fill("L2Calo",1);
                fillL2Calo(dir+"L2Calo",obj); // Fill HLT shower shapes
            }

            /*
            const auto vec_l2ringer = fc.get<xAOD::TrigRingerRings>("",TrigDefs::alsoDeactivateTEs);
            for(const auto feat : vec_l2ringer){
                if(feat.te()==nullptr) continue;
                const auto* obj = getFeature<xAOD::TrigRingerRings>(feat.te());
                // Only consider passing objects
                if(!obj) continue;
                fillRingerShapes(dir+"L2Calo",obj);// Fill HLT ringer shapes
            }*/

            const auto vec_l2rnn = fc.get<xAOD::TrigRNNOutput>("",TrigDefs::alsoDeactivateTEs);
            for(const auto feat : vec_l2rnn){
                if(feat.te()==nullptr) continue;
                const auto* obj = getFeature<xAOD::TrigRNNOutput>(feat.te());
                // Only consider passing objects
                if(!obj) continue;   
                //cd(dir+"HLT");
                //if(ancestorPassed<xAOD::TrigRNNOutput>(feat.te()))
                //  hist1("rejection")->Fill("L2Calo",1);
                fillRnnDistribution(dir+"L2Calo",obj);// Fill HLT Rnn distribution output
            }

            const auto vec_clus = fc.get<xAOD::CaloClusterContainer>("TrigEFCaloCalibFex",TrigDefs::alsoDeactivateTEs);
            for(const auto feat : vec_clus){
                if(feat.te()==nullptr) continue;
                const auto *cont = getFeature<xAOD::CaloClusterContainer>(feat.te(),"TrigEFCaloCalibFex");
                ATH_MSG_DEBUG("Retreive clusters, bits");
                std::unique_ptr<xAOD::TrigPassBits> bits = getBits<xAOD::CaloClusterContainer>(feat.te(),cont);
                if(cont==nullptr) continue;
                ATH_MSG_DEBUG("check bits");
                if(bits==nullptr) continue;
                cd(dir+"HLT");
                if(ancestorPassed<xAOD::CaloClusterContainer>(feat.te(),"TrigEFCaloCalibFex"))
                    hist1("rejection")->Fill("EFCalo",1);
                ATH_MSG_DEBUG("analyze clusters");
                for(const auto& clus : *cont){
                    if(clus==nullptr) continue;
                    if(!bits->isPassing(clus,cont)) continue;
                    fillEFCalo(dir+"EFCalo",clus);           
                }
            }

            // Can we template some of the following
            // Loop over features, get PassBits from TE for Electron
            if(info.trigType=="electron"){
                const auto vec_l2el = fc.get<xAOD::TrigElectronContainer>("",TrigDefs::alsoDeactivateTEs);
                for (const auto feat : vec_l2el){
                    if(feat.te()==nullptr) continue;
                    const auto* cont = getFeature<xAOD::TrigElectronContainer>(feat.te());
                    std::unique_ptr<xAOD::TrigPassBits> bits = getBits<xAOD::TrigElectronContainer>(feat.te(),cont);
                    if(bits==nullptr) continue; 
                    if(cont==nullptr) continue;
                    cd(dir+"HLT");
                    if(ancestorPassed<xAOD::TrigElectronContainer>(feat.te()))
                        hist1("rejection")->Fill("L2",1);
                    for(const auto& obj : *cont){
                        // Only consider passing objects
                        if(!obj) continue;
                        if(!bits->isPassing(obj,cont)) continue;
                        fillL2Electron(dir+"L2Electron",obj); // Fill HLT shower shapes
                    }
                }
                const auto vec_el = fc.get<xAOD::ElectronContainer>("egamma_Electrons",TrigDefs::alsoDeactivateTEs);
                ATH_MSG_DEBUG("Retrieved egamma_Electrons: " << vec_el.size());
                for (const auto feat : vec_el){
                    if(feat.te()==nullptr) continue;
                    const auto* cont = getFeature<xAOD::ElectronContainer>(feat.te());
                    std::unique_ptr<xAOD::TrigPassBits> bits = getBits<xAOD::ElectronContainer>(feat.te(),cont);
                    if(cont==nullptr) continue; 
                    cd(dir+"HLT");
                    if(ancestorPassed<xAOD::ElectronContainer>(feat.te()))
                        hist1("rejection")->Fill("HLT",1);
                    for(const auto& obj : *cont){
                        if(!obj) continue;
                        if(bits==nullptr){
                            fillShowerShapes(dir+"HLT",obj); // Fill HLT shower shapes
                            fillTracking(dir+"HLT",obj); // Fill HLT shower shapes
                        }
                        // Only consider passing objects if bits available
                        else if(!bits->isPassing(obj,cont)) continue;
                        fillShowerShapes(dir+"HLT",obj); // Fill HLT shower shapes
                        fillTracking(dir+"HLT",obj); // Fill HLT shower shapes
                    }
                }
            }
            else if(info.trigType=="photon"){
                const auto vec_ph = fc.get<xAOD::PhotonContainer>("",TrigDefs::alsoDeactivateTEs);
                ATH_MSG_DEBUG("Retrieved egamma_Photons: " << vec_ph.size());
                for (const auto feat : vec_ph){
                    if(feat.te()==nullptr) continue;
                    const auto* cont = getFeature<xAOD::PhotonContainer>(feat.te());
                    std::unique_ptr<xAOD::TrigPassBits> bits = getBits<xAOD::PhotonContainer>(feat.te(),cont);
                    if(cont==nullptr) continue;
                    cd(dir+"HLT");
                    if(ancestorPassed<xAOD::PhotonContainer>(feat.te()))
                        hist1("rejection")->Fill("HLT",1);
                    for(const auto& obj : *cont){
                        // Only consider passing objects
                        if(!obj) continue;
                        if(bits==nullptr){
                            fillShowerShapes(dir+"HLT",obj); // Fill HLT shower shapes
                        }
                        // Only consider passing objects if bits available
                        else if(!bits->isPassing(obj,cont)) continue;
                        fillShowerShapes(dir+"HLT",obj); // Fill HLT shower shapes
                    }
                }
            }
            else ATH_MSG_WARNING("Chain type not specified");
        }

    }
    return StatusCode::SUCCESS;
}

void TrigEgammaDistTool::fillL1Calo(const std::string dir, const xAOD::EmTauRoI *l1){
    cd(dir);
    ATH_MSG_DEBUG("Fill L1Calo distributions" << dir);
    hist1("eta")->Fill(l1->eta());
    hist1("phi")->Fill(l1->phi());
    hist1("energy")->Fill(l1->emClus()/1.e3);
    hist1("roi_et")->Fill(l1->eT()/1.e3);
    hist1("emIso")->Fill(l1->emIsol()/1.e3);
    hist1("hadCore")->Fill(l1->hadCore()/1.e3);
    hist2("emClusVsEta")->Fill(l1->eta(), l1->emClus()*0.001);
    hist2("emClusVsEmIsol")->Fill(l1->emIsol()*0.001, l1->emClus()*0.001);
    hist2("emClusVsHadCore")->Fill(l1->hadCore()*0.001, l1->emClus()*0.001);
}

void TrigEgammaDistTool::fillEFCalo(const std::string dir, const xAOD::CaloCluster *clus){
    cd(dir);
    ATH_MSG_DEBUG("Fill EFCalo distributions" << dir);
    ATH_MSG_DEBUG("Energy " << clus->e()/1.e3);
    ATH_MSG_DEBUG("eta " << clus->eta());
    ATH_MSG_DEBUG("phi " << clus->phi());
    
    double tmpeta = -999.;
    if(!clus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta))
        tmpeta=-999.;
    double tmpphi = -999.;
    if(!clus->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi))
        tmpphi=-999.;
    ATH_MSG_DEBUG("etacalo " << tmpeta);
    ATH_MSG_DEBUG("phicalo " << tmpphi);
    
    hist1("energyBE0")->Fill(clus->energyBE(0)/1.e3);
    hist1("energyBE1")->Fill(clus->energyBE(1)/1.e3);
    hist1("energyBE2")->Fill(clus->energyBE(2)/1.e3);
    hist1("energyBE3")->Fill(clus->energyBE(3)/1.e3);
    hist1("energy")->Fill(clus->e()/1.e3);
    hist1("et")->Fill(clus->et()/1.e3);
    hist1("eta")->Fill(clus->eta());
    hist1("phi")->Fill(clus->phi());
    hist1("eta_calo")->Fill(tmpeta);
    hist1("phi_calo")->Fill(tmpphi);
}

void TrigEgammaDistTool::fillL2Electron(const std::string dir, const xAOD::TrigElectron *el){
    cd(dir);
    if(!el) ATH_MSG_DEBUG("TrigElectron nullptr");
    else {
        hist1("et")->Fill(el->pt()/1.e3);
        hist1("eta")->Fill(el->eta());
        hist1("phi")->Fill(el->phi());
    }

}

void TrigEgammaDistTool::fillL2Calo(const std::string dir, const xAOD::TrigEMCluster *emCluster){
    cd(dir);
    if(!emCluster) ATH_MSG_DEBUG("Online pointer fails"); 
    else{
        hist1("et")->Fill(emCluster->et()/1.e3);
        hist1("eta")->Fill(emCluster->eta());
        hist1("phi")->Fill(emCluster->phi());

        ATH_MSG_DEBUG("L2 Calo distributions.");
    }
}

void TrigEgammaDistTool::fillRingerShapes(const std::string dir, const xAOD::TrigRingerRings *ringer){
    cd(dir);
    if(!ringer) ATH_MSG_DEBUG("Online pointer fails");  
    else{
      //for(unsigned r=0; r<ringer->rings().size(); ++r){
      //  hist2("ringer_shapes")->Fill(r, ringer->rings()[r]);
      // }
      ATH_MSG_DEBUG("L2 Calo distributions.");
    }
}

void TrigEgammaDistTool::fillRnnDistribution(const std::string dir, const xAOD::TrigRNNOutput *rnn){
    // ringer threshold grid for mc15c, 2016 and 2017       
    float ringer_thres_et_bins[6]={15.,20.,30.,40.,50.,50000.};
    float ringer_thres_eta_bins[6]={0.,0.8,1.37,1.54,2.37,2.5};

    if(!rnn)  ATH_MSG_DEBUG("Online pointer fails");
    else{
      if(rnn->rnnDecision().empty() || rnn->rnnDecision().size()!=3){
        ATH_MSG_DEBUG("Invalid RNN Decision. skip object...");
        return;
      }

      auto emCluster=rnn->ringer()->emCluster();
      float eta=std::abs(emCluster->eta());
      float et=emCluster->et()*1e-3;
      float output=rnn->rnnDecision()[2];
      float avgmu=rnn->rnnDecision()[0];

      cd(dir);
      hist1("discriminant")->Fill(output);
      hist2("discriminantVsMu")->Fill(output,avgmu);
      cd(dir+"/discriminant_binned");
      for (unsigned etBinIdx=0; etBinIdx<6-1; ++etBinIdx){
        for (unsigned etaBinIdx=0; etaBinIdx<6-1; ++etaBinIdx){
          if( ( et < ringer_thres_et_bins[etBinIdx]  && ringer_thres_et_bins[etBinIdx+1] >= et)
           && ( eta < ringer_thres_eta_bins[etaBinIdx]  && ringer_thres_eta_bins[etaBinIdx+1] >= eta)){
            std::stringstream ss1,ss2;
            ss1 << "discriminant_et_"<<etBinIdx<<"_eta_"<<etaBinIdx;
            ss2 << "discriminantVsMu_et_"<<etBinIdx<<"_eta_"<<etaBinIdx;
            hist1(ss1.str())->Fill(output);
            hist2(ss2.str())->Fill(output,avgmu);
           }
        }
      }
      ATH_MSG_DEBUG("L2 Calo distributions.");
    }
}



void TrigEgammaDistTool::fillShowerShapes(const std::string dir,const xAOD::Egamma *eg){
    cd(dir);
    ATH_MSG_DEBUG("Fill SS distributions " << dir);
    if(!eg) ATH_MSG_WARNING("Egamma pointer fails"); 
    else {
        ATH_MSG_DEBUG("Shower Shapes");
        //hist1("e011")->Fill(getShowerShape_e011(eg)/1e3);
        //hist1("e132")->Fill(getShowerShape_e132(eg)/1e3);
        //hist1("e237")->Fill(getShowerShape_e237(eg)/1e3);
        //hist1("e277")->Fill(getShowerShape_e277(eg)/1e3);
        hist1("ethad")->Fill(getShowerShape_ethad(eg)/1e3);
        hist1("ethad1")->Fill(getShowerShape_ethad1(eg)/1e3);
        hist1("Rhad")->Fill(getShowerShape_Rhad(eg));
        hist1("Rhad1")->Fill(getShowerShape_Rhad(eg));
        hist1("Reta")->Fill(getShowerShape_Reta(eg));
        hist1("Rphi")->Fill(getShowerShape_Rphi(eg));
        hist1("weta1")->Fill(getShowerShape_weta1(eg));
        hist1("weta2")->Fill(getShowerShape_weta2(eg));
        //hist1("wtots1")->Fill(getShowerShape_wtots1(eg));
        hist1("f1")->Fill(getShowerShape_f1(eg));
        hist1("f3")->Fill(getShowerShape_f3(eg));
        hist1("eratio")->Fill(getShowerShape_Eratio(eg));
        hist1("et")->Fill(eg->pt()/1e3);
        hist1("highet")->Fill(eg->pt()/1e3);
        hist1("eta")->Fill(eg->eta());
        hist1("phi")->Fill(eg->phi());
        hist1("topoetcone20")->Fill(getIsolation_topoetcone20(eg)/1e3);
        hist1("topoetcone40_shift")->Fill((getIsolation_topoetcone40(eg)-2450)/1e3);
        if (eg->pt() > 0) {
            hist1("topoetcone20_rel")->Fill(getIsolation_topoetcone20(eg)/eg->pt());
            hist1("topoetcone40_shift_rel")->Fill((getIsolation_topoetcone40(eg)-2450)/eg->pt());
        }
        
    }
}

void TrigEgammaDistTool::fillTracking(const std::string dir, const xAOD::Electron *eg){
    cd(dir);  
    ATH_MSG_DEBUG("Fill tracking");
    if(!eg) ATH_MSG_WARNING("Electron pointer fails");
    else {
        float cleta = 0.;
        if(eg->caloCluster()) cleta=eg->caloCluster()->eta();
        else cleta=eg->eta();
        ATH_MSG_DEBUG("Calo-track match");
        hist1("deta1")->Fill(getCaloTrackMatch_deltaEta1(eg));
        if(cleta > 1.375 && cleta < 3.2)
            hist1("deta1_EMECA")->Fill(getCaloTrackMatch_deltaEta1(eg));
        if(cleta < -1.375 && cleta > -3.2)
            hist1("deta1_EMECC")->Fill(getCaloTrackMatch_deltaEta1(eg));
        if(cleta > 0 && cleta < 1.375)
            hist1("deta1_EMEBA")->Fill(getCaloTrackMatch_deltaEta1(eg));
        if(cleta < 0 && cleta > -1.375)
            hist1("deta1_EMEBC")->Fill(getCaloTrackMatch_deltaEta1(eg));
        hist1("deta2")->Fill(getCaloTrackMatch_deltaEta2(eg));
        hist1("dphi2")->Fill(getCaloTrackMatch_deltaPhi2(eg));
        hist1("dphiresc")->Fill(getCaloTrackMatch_deltaPhiRescaled2(eg));
        
        
        hist1("eprobht")->Fill(getTrackSummaryFloat_eProbabilityHT(eg));
        hist1("npixhits")->Fill(getTrackSummary_numberOfPixelHits(eg));
        hist1("nscthits")->Fill(getTrackSummary_numberOfSCTHits(eg));
        hist1("charge")->Fill(eg->charge());
        hist1("ptcone20")->Fill(getIsolation_ptcone20(eg)/1e3);
        
        hist1("ptvarcone20")->Fill(getIsolation_ptvarcone20(eg)/1e3);
        // Quantities directly from tracks
        ATH_MSG_DEBUG("Get track Quantities");
        hist1("d0")->Fill(getTrack_d0(eg));
        hist1("d0sig")->Fill(getD0sig(eg));
        hist1("pt")->Fill(getTrack_pt(eg)/1e3);
        if (eg->pt() > 0) {
            hist1("ptcone20_rel")->Fill(getIsolation_ptcone20(eg)/eg->pt());
            hist1("ptvarcone20_rel")->Fill(getIsolation_ptvarcone20(eg)/eg->pt());
        }
        if (m_detailedHists ) {
            hist2("deta1_vs_clusterEta")->Fill(cleta,getCaloTrackMatch_deltaEta1(eg));
        }

    }
}
