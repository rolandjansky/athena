/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaEmulationTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Date: Feb 2015
 * Description:
 *      Derived class for studying performance triggers (e.g e0_perf)
 *      Can be used to emulate trigger selection with triggered objects.
 *      Or to study performance of new calibrations, selections, etc.
 **********************************************************************/
#include "TrigEgammaAnalysisTools/TrigEgammaEmulationTool.h"
#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************

TrigEgammaEmulationTool::
TrigEgammaEmulationTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname) {
  declareProperty("DirectoryPath",m_dir="EmulationTool");
  declareProperty("ElectronOnlPPSelector", m_electronOnlIsEMTool);
  declareProperty("PhotonOnlPPSelector", m_photonOnlIsEMTool);
  declareProperty("ElectronOnlLHSelector", m_electronOnlLHTool);
  declareProperty("ElectronCaloPPSelector", m_electronCaloIsEMTool);
  declareProperty("PhotonCaloPPSelector", m_photonCaloIsEMTool);
  declareProperty("ElectronCaloLHSelector", m_electronCaloLHTool);
}

//**********************************************************************
StatusCode TrigEgammaEmulationTool::childInitialize(){

    m_eventCounter=0;
    std::cout << "child Initialize" << std::endl;

   
    if ( m_electronOnlIsEMTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }
    if ( m_photonOnlIsEMTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }
    if ( m_electronOnlLHTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }
    if ( m_electronCaloIsEMTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }
    if ( m_photonCaloIsEMTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }
    if ( m_electronCaloLHTool.retrieve().isFailure() ){
        std::cout << "retrieve failed for tools" << std::endl;
    }

      return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEmulationTool::childExecute()
{

    ATH_MSG_DEBUG("Executing NavZeePerf");
   
   
    m_offElectrons=0;
    m_onlElectrons=0;
    m_trigElectrons=0;
    m_caloClusters=0;
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    ATH_MSG_INFO("Offline Electron container size " << m_offElectrons->size());

    if ( (m_storeGate->retrieve(m_onlElectrons,"HLT_xAOD__ElectronContainer_egamma_Electrons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("Online Electron container size " << m_onlElectrons->size());
    if ( (m_storeGate->retrieve(m_onlPhotons,"HLT_xAOD__PhotonContainer_egamma_Photons")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Photons ");
    }
    else ATH_MSG_INFO("Online Photon container size " << m_onlPhotons->size());
    if ( (m_storeGate->retrieve(m_trigElectrons,"HLT_xAOD__TrigElectronContainer_L2ElectronFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("Trig Electron container size " << m_trigElectrons->size());
    if ( (m_storeGate->retrieve(m_emTauRoI,"LVL1EmTauRoIs")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
    }
    else ATH_MSG_INFO("L1 EM container size " << m_emTauRoI->size());
    if( (m_storeGate->retrieve(m_caloClusters,"HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve CaloClusters");
    }
    else ATH_MSG_INFO("CaloCluster size " << m_caloClusters->size());
    if( (m_storeGate->retrieve(m_trigEMClusters,"HLT_xAOD__TrigEMClusterContainer_TrigT2CaloEgamma")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve TrigEMClusters");
    }
    else ATH_MSG_INFO("TrigEMCluser size " << m_trigEMClusters->size());


    std::string l1item_chk = "L1_EM7";
    if(m_trigdec->isPassed(l1item_chk)){
        ATH_MSG_DEBUG("Passed: " << l1item_chk);
        if(!EmulationL1(l1item_chk))
            ATH_MSG_DEBUG("L1EmulationFail::Finish emulation for failed events");
    }
    if(!m_trigdec->isPassed(l1item_chk)){
        ATH_MSG_DEBUG("Failed: " << l1item_chk);
        if(EmulationL1(l1item_chk))
            ATH_MSG_DEBUG("L1EmulationPass::Finish emulation for failed events");
    }
    std::string efitem_chk ="HLT_e10_etcut_L1EM7";
    if(m_trigdec->isPassed(efitem_chk)){
        ATH_MSG_DEBUG("Passed: " << efitem_chk);
        if(!EmulationEF(efitem_chk))
            ATH_MSG_DEBUG("EFEmulationFail::Finish emulation for failed events");
    }
    if(!m_trigdec->isPassed(efitem_chk)){
        ATH_MSG_DEBUG("Failed: " << efitem_chk);
        if(EmulationEF(efitem_chk))
            ATH_MSG_DEBUG("EFEmulationPass::Finish emulation for failed events");
    }

    StatusCode sc = TDCounts();

    if(sc != StatusCode::SUCCESS)
        ATH_MSG_DEBUG("TDCounts Fails");

    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaEmulationTool::childFinalize()
{

    ATH_MSG_INFO ("Finalizing " << name() << "..." << "Total counts " << m_eventCounter);
    std::ofstream countsFile ("TriggerCounts.txt");
    if(countsFile.is_open()){
        for (auto iter = m_counterBits.begin(); iter != m_counterBits.end(); iter++) {
            ATH_MSG_INFO(iter->first << " == " << iter->second << " emulated " << m_counterEmulationHLT[iter->first]);
            countsFile << std::setw(50) << iter->first 
                << " ---- Passed " <<  std::setw(10) << iter->second 
                << " ----- Emulated " << std::setw(10) << m_counterEmulationHLT[iter->first] << "\n";
        }
    }
    std::ofstream countsFileL1 ("TriggerCountsL1.txt");
    if(countsFileL1.is_open()){
        for (auto iter = m_counterBitsL1.begin(); iter != m_counterBitsL1.end(); iter++) {
            ATH_MSG_INFO(iter->first << " == " << iter->second << " emulated " << m_counterEmulationL1[iter->first]);
            countsFileL1 << std::setw(50) << iter->first 
                << " ---- Passed " <<  std::setw(10) << iter->second 
                << " ----- Emulated " << std::setw(10) << m_counterEmulationL1[iter->first] << "\n";
        }
    }
    return StatusCode::SUCCESS;

}

StatusCode TrigEgammaEmulationTool::TDCounts(){

  ATH_MSG_DEBUG( "L1: " << m_trigdec->isPassed( "L1_.*" )
          << ", L2: " << m_trigdec->isPassed( "L2_.*" )
          << ", EF: " << m_trigdec->isPassed( "EF_.*" )
          << ", HLT: " << m_trigdec->isPassed( "HLT_.*" ) );

  const auto l1chainGroups = m_trigdec->getChainGroup("L1_EM.*");
  for(const auto &trig : l1chainGroups->getListOfTriggers()) {
      if(m_trigdec->isPassed(trig))
          ATH_MSG_DEBUG("Passed: " << trig);
      else {
          ATH_MSG_DEBUG("L1FAILURE::Check Emulation for failed events");
          EmulationL1(trig);
          ATH_MSG_DEBUG("Finish emulation for failed events");
      }

      m_counterBitsL1[trig]+=m_trigdec->isPassed(trig);
      if(EmulationL1(trig)) m_counterEmulationL1[trig]++;
  }

  const auto chainGroups = m_trigdec->getChainGroup("HLT_e.*");

 
  for(const auto &trig : chainGroups->getListOfTriggers()) {
      if(m_trigdec->isPassed(trig))
          ATH_MSG_DEBUG("Passed: " << trig);
      m_counterBits[trig]+=m_trigdec->isPassed(trig);
  }
  std::string l1item="";
  for(const auto &trig : chainGroups->getListOfTriggers()) {
      ATH_MSG_DEBUG("Emulation: " << trig);
      if(boost::contains(trig,"eb")) continue;
        l1item = getL1Item(trig);
      if(EmulationL1(l1item)){
          
          if(EmulationEF(trig)){
              m_counterEmulationHLT[trig]++;
              ATH_MSG_DEBUG("Emulation Passed: " << trig);  
          }
      }
  }
  const auto gchainGroups = m_trigdec->getChainGroup("HLT_g.*");

 
  for(const auto &trig : gchainGroups->getListOfTriggers()) {
      if(m_trigdec->isPassed(trig))
          ATH_MSG_DEBUG("Passed: " << trig);
      m_counterBits[trig]+=m_trigdec->isPassed(trig);
  }
  
  for(const auto &trig : gchainGroups->getListOfTriggers()) {
      ATH_MSG_DEBUG("Emulation: " << trig);
      if(boost::contains(trig,"eb")) continue;
        l1item = getL1Item(trig);
      if(EmulationL1(l1item)){
          if(EmulationPhotonEF(trig)){
              m_counterEmulationHLT[trig]++;
              ATH_MSG_DEBUG("Emulation Passed: " << trig);  
          }
      }
  }
  return StatusCode::SUCCESS;
}


bool TrigEgammaEmulationTool::EmulationL1V(const std::string L1item, const float energy, const float eta){
    float cut=0.;
    if(L1item=="50V"){
        if( fabs(eta) <= 1.2 && fabs(eta) > 0.8)  cut = 51;
        else if( fabs(eta) <= 1.6 && fabs(eta) > 1.2) cut = 50;
        else if( fabs(eta) <= 2.0 && fabs(eta) > 1.6) cut = 51;
        else cut = 52;

    }
    else if(L1item=="8VH"){
        if( fabs(eta) < 2.0 && fabs(eta) > 0.8)  cut = 8;
        else cut = 9;
    }
    else if(L1item=="10VH"){
        if( fabs(eta) < 2.0 && fabs(eta) > 0.8)  cut = 10.;
        else cut = 11.;
    }
    else if(L1item=="13VH"){
        if( fabs(eta) < 2.0 && fabs(eta) > 0.8)  cut = 13;
        else cut = 14;
    }
    else if(L1item=="15VH"){
        if( fabs(eta) < 1.2 && fabs(eta) > 0.8)  cut = 16;
        else if( fabs(eta) < 1.6 && fabs(eta) > 1.2) cut = 15;
        else if( fabs(eta) < 2.0 && fabs(eta) > 1.6) cut = 16;
        else cut = 17;
    }
    else if(L1item == "18VH"){
        if( fabs(eta) < 1.2 && fabs(eta) > 0.8)  cut = 19;
        else if( fabs(eta) < 1.6 && fabs(eta) > 1.2) cut = 18;
        else if( fabs(eta) < 2.0 && fabs(eta) > 1.6) cut = 19;
        else cut = 20;
    }
    else if(L1item == "20VH"){
        if( fabs(eta) <= 1.2 && fabs(eta) > 0.8)  cut = 21;
        else if( fabs(eta) <= 1.6 && fabs(eta) > 1.2) cut = 20;
        else if( fabs(eta) <= 2.0 && fabs(eta) > 1.6) cut = 21;
        else cut = 22;
    }
    else if(L1item == "20VHI"){
        if( fabs(eta) < 1.2 && fabs(eta) > 0.8)  cut = 21;
        else if( fabs(eta) < 1.6 && fabs(eta) > 1.2) cut = 20;
        else if( fabs(eta) < 2.0 && fabs(eta) > 1.6) cut = 21;
        else cut = 22;
    }
    else if(L1item == "22VHI"){
        if( fabs(eta) < 1.2 && fabs(eta) > 0.8)  cut = 23;
        else if( fabs(eta) < 1.6 && fabs(eta) > 1.2) cut = 22;
        else if( fabs(eta) < 2.0 && fabs(eta) > 1.6) cut = 23;
        else cut = 24;
    }

    ATH_MSG_DEBUG("L1 V " << L1item << " cut = " << cut << " eta " << eta << " energy " << energy);
    if(energy <= cut) return false;
    return true;



}
bool TrigEgammaEmulationTool::EmulationL1(const std::string trigger){
    bool isPassed=false;
    std::string L1Item = trigger;
    ATH_MSG_DEBUG("Emulation L1 item " << L1Item);
    L1Item.erase( 0, 5); //Removes L1EM prefix from name

    std::string l1type = boost::trim_copy_if(L1Item, boost::is_digit());
    std::string l1cut = boost::trim_copy_if(L1Item, !boost::is_digit());
    float l1threshold = atof(l1cut.c_str()); // Is the L1 threshold +1 GeV

    ATH_MSG_DEBUG("L1 item " << L1Item << " " << l1threshold << " " << l1type << " " << m_emTauRoI->size());
    // L1 selection -- move to separate method
    float hadCoreCut =0.;
    float emIsoCut=0.;
    float emE=0.;

    for(const auto& l1: *m_emTauRoI){
        emE=l1->emClus()/1.e3;
        if(emE<50){
            if(boost::contains(l1type,"H")){
                hadCoreCut = std::max(1.0,-0.2 + emE/23.0);
                //hadCoreCut = round( hadCoreCut );
                //hadCoreCut = 1.0;
                ATH_MSG_DEBUG("Apply L1 hadCore cut " << hadCoreCut<< " cluster E " << l1->hadCore()/1.e3); 
                if( l1->hadCore()/1.e3 > hadCoreCut) continue;
            }
            if(boost::contains(l1type,"I")){
                emIsoCut= std::max(1.0, -1.8 + emE / 8.0);
                //emIsoCut = round( emIsoCut );
                //emIsoCut = 2.0;
                ATH_MSG_DEBUG("Apply L1 emIso cut " << emIsoCut<< " cluster E " << l1->emIsol()/1.e3); 
                if(l1->emIsol()/1.e3 > emIsoCut) continue;
            }
        }
        ATH_MSG_DEBUG("Apply L1 Et cut " << l1threshold << " cluster E " << emE << " eta " << l1->eta());
        if(boost::contains(l1type,"V")){
            // Variable energy cut as function of eta
            if(!EmulationL1V(L1Item,emE,l1->eta()))
                    continue;
        }
        else if(l1->emClus()/1.e3<=l1threshold)
            continue;
        isPassed=true;
    }
    return isPassed;
}

bool TrigEgammaEmulationTool::EmulationPhotonEF(const std::string trigger){
    
    std::string trigItem = trigger;
    trigItem.erase( 0, 4); //Removes HLT_ prefix from name
    bool isPassed=false;
    std::string type="";
    float etthr=0;
    float l1thr=0;
    std::string l1type="";
    std::string pidname="";
    bool perf=false;
    bool etcut=false;
    bool isolation = false;
    bool idperf = false;
    if(boost::contains(trigItem,"iloose")) isolation = true;
    if(boost::contains(trigItem,"idperf")) idperf = true;
    parseTriggerName(trigItem,"Loose",type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
    ATH_MSG_DEBUG(trigItem <<" Etcut " << etthr << " l1 cut " << l1thr << " l1type " << l1type << " pid " << pidname << " isolation " << isolation 
            << " etcut " << etcut << " idperf " << idperf << " perf " << perf);


   
    if(boost::contains(pidname,"1")) return false; // Not emulating Run1
   
    for(const auto& eg : *m_onlPhotons){
        if(perf){
            ATH_MSG_DEBUG(trigItem << " Etcut trigger cut " << etthr);
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            ATH_MSG_DEBUG("Passed " << eg->caloCluster()->et());
            isPassed=true; //at least one electron
        }
        else if(etcut){
            ATH_MSG_DEBUG(trigItem << " Etcut trigger cut " << etthr);
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            ATH_MSG_DEBUG("Passed " << eg->caloCluster()->et());
         
            isPassed=true; //at least one electron
        }
        else {
            ATH_MSG_DEBUG("Apply Pid " << pidname);
            if(pidname == "Tight"){
                if(!m_photonOnlIsEMTool[0]->accept(eg))
                    continue;
            }
            else if(pidname == "Medium"){
                if(!m_photonOnlIsEMTool[1]->accept(eg))
                    continue;
            }
            else if(pidname == "Loose"){
                if(!m_photonOnlIsEMTool[2]->accept(eg))
                    continue;
            }
            else ATH_MSG_DEBUG("No Pid tool, continue without PID"); 
            ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            ATH_MSG_DEBUG("Passed");
            isPassed=true;
        }
    }
    return isPassed; 
}

bool TrigEgammaEmulationTool::ApplyElectronCaloPid(const xAOD::Electron *eg, const std::string pidname){
            ATH_MSG_DEBUG("Apply Pid " << pidname);

            if(pidname == "Tight"){
                return m_electronCaloIsEMTool[0]->accept(eg);
            }
            else if(pidname == "Medium"){
                return m_electronCaloIsEMTool[1]->accept(eg);
            }
            else if(pidname == "Loose"){
                return m_electronCaloIsEMTool[2]->accept(eg);
            }
            else if(pidname == "VLoose"){
                return m_electronCaloIsEMTool[3]->accept(eg);
            }
            else if(pidname == "LHTight"){
                return m_electronCaloLHTool[0]->accept(eg);
            }
            else if(pidname == "LHMedium"){
                return m_electronCaloLHTool[1]->accept(eg);
            }
            else if(pidname == "LHLoose"){
                return m_electronCaloLHTool[2]->accept(eg);
            }
            else if(pidname == "LHVLoose"){
                return m_electronCaloLHTool[3]->accept(eg);
            }
            else ATH_MSG_DEBUG("No Pid tool, continue without PID");
            return false;

}
bool TrigEgammaEmulationTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname){
            ATH_MSG_DEBUG("Apply Pid " << pidname);

            if(pidname == "Tight"){
                return m_electronOnlIsEMTool[0]->accept(eg);
            }
            else if(pidname == "Medium"){
                return m_electronOnlIsEMTool[1]->accept(eg);
            }
            else if(pidname == "Loose"){
                return m_electronOnlIsEMTool[2]->accept(eg);
            }
            else if(pidname == "VLoose"){
                return m_electronOnlIsEMTool[3]->accept(eg);
            }
            else if(pidname == "LHTight"){
                return m_electronOnlLHTool[0]->accept(eg);
            }
            else if(pidname == "LHMedium"){
                return m_electronOnlLHTool[1]->accept(eg);
            }
            else if(pidname == "LHLoose"){
                return m_electronOnlLHTool[2]->accept(eg);
            }
            else if(pidname == "LHVLoose"){
                return m_electronOnlLHTool[3]->accept(eg);
            }
            else ATH_MSG_DEBUG("No Pid tool, continue without PID");
            return false;

}
bool TrigEgammaEmulationTool::EmulationEF(const std::string trigger){
    
    std::string trigItem = trigger;
    trigItem.erase( 0, 4); //Removes HLT_ prefix from name
    bool isPassed=false;
    std::string type ="";
    float etthr=0;
    float l1thr=0;
    std::string l1type="";
    std::string pidname="";
    bool perf=false;
    bool etcut=false;
    bool isolation = false;
    bool idperf = false;
    ATH_MSG_DEBUG("Emulating EF with " << m_onlElectrons->size() << " Electrons");
    if(boost::contains(trigItem,"iloose")) isolation = true;
    if(boost::contains(trigItem,"idperf")) idperf = true;
    parseTriggerName(trigItem,"Loose",type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger
    ATH_MSG_DEBUG(trigItem <<" Etcut " << etthr << " l1 cut " << l1thr << " l1type " << l1type << " pid " << pidname << " isolation " << isolation 
            << " etcut " << etcut << " idperf " << idperf << " perf " << perf);


    if(perf) return true;
   
    if(boost::contains(pidname,"1")) return false; // Not emulating Run1
    ATH_MSG_DEBUG("Emulating EF with " << m_onlElectrons->size() << " Electrons");
    for(const auto& eg : *m_onlElectrons){
        // Check Run2 only tracks
        //if(!eg->trackParticle()->patternRecoInfo()[xAOD::FastTrackFinderSeed]) continue;
        if(idperf){
            if(!ApplyElectronCaloPid(eg,pidname)) continue;
            ATH_MSG_DEBUG("Apply Pid " << pidname);
            ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            ATH_MSG_DEBUG("Passed");
            isPassed=true;
        }
        else if(etcut){
            ATH_MSG_DEBUG(trigItem << " Etcut trigger cut " << etthr);
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            ATH_MSG_DEBUG("Passed " << eg->caloCluster()->et());
            ATH_MSG_DEBUG("Track pattern " << eg->trackParticle()->patternRecoInfo());
            isPassed=true; //at least one electron
        }
        else if(isolation){
            if(!ApplyElectronCaloPid(eg,pidname)) continue;
            if(!ApplyElectronPid(eg,pidname)) continue;
            ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            float val=0.;
            ATH_MSG_DEBUG("Isolation");
            eg->isolationValue(val,xAOD::Iso::ptcone20);
            if( (val/eg->trackParticle()->pt()) > 0.1)
                continue;
            ATH_MSG_DEBUG("Passed");
            isPassed=true;
        }
        else {
            if(!ApplyElectronCaloPid(eg,pidname)) continue;
            if(!ApplyElectronPid(eg,pidname)) continue;
            ATH_MSG_DEBUG("Apply Pid " << pidname);
            ATH_MSG_DEBUG("Apply Et cut " << etthr << " cluster Et " << eg->caloCluster()->et());
            if(eg->caloCluster()->et()/1.e3 < etthr)
                continue;
            ATH_MSG_DEBUG("Passed");
            isPassed=true;
        }
    }
    return isPassed; 
}

