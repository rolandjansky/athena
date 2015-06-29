/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavTPBaseTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 * Date: Feb 2015
 * Description:
 *      Trigger e/gamma Zee Tag&Probe Base tool class. Inherits from TrigEgammaAnalysisBaseTool.
 *      Provides methods for selecting T&P pairs, 
 *      matching probes to objects in TE containers.
 *      Creates a vector of pairs with 
 *      offline electrons probes and the last TE with a match.
 *      Relies heavily on TrigNavigation, since we use the TriggerElement.
 *      All derived classes work with list of probes for a given trigger.
 *      As input property, pass a list of triggers to study.
 **********************************************************************/
#include "egammaMVACalib/IegammaMVATool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"

#include "string"
#include <algorithm>
#include "boost/algorithm/string.hpp"
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
//**********************************************************************
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;
using namespace boost;
TrigEgammaNavTPBaseTool::
TrigEgammaNavTPBaseTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname)
{
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ApplyMVACalib", m_applyMVACalib=false);
  declareProperty("ElectronIsEMSelector", m_electronIsEMTool);
  declareProperty("ElectronLikelihoodTool", m_electronLHTool);
  declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
  declareProperty("ZeeLowerMass",m_ZeeMassMin=80);
  declareProperty("ZeeUpperMass",m_ZeeMassMax=100);
  declareProperty("OfflineTagSelector",m_offTagTightness="Tight");
  declareProperty("OfflineProbeSelector",m_defaultProbeTightness="Loose"); // new
  declareProperty("ForceProbePid", m_forceProbePid=false); // new
  declareProperty("OppositeCharge",m_oppositeCharge=true);
  declareProperty("OfflineTagMinEt",m_tagMinEt=25);
  declareProperty("OfflineProbeMinEt",m_probeMinEt=24); // new
  declareProperty("TagTriggerList", m_tagTrigList);
  declareProperty("TriggerList",m_trigInputList);
  declareProperty("CategoryList",m_categories);
  declareProperty("RemoveCrack", m_rmCrack=true); //new
  declareProperty("OfflineProbeIsolation", m_offProbeIsolation="Loose");
  declareProperty("ForceProbeIsolation", m_forceProbeIsolation=false);

  m_PidToolMap["Tight"]=0;
  m_PidToolMap["Medium"]=1;
  m_PidToolMap["Loose"]=2;

  m_offElectrons=nullptr;
  m_jets=nullptr;
  // Maps should be static
  // Make a wrapper function to set map and return value
    
}

//**********************************************************************

StatusCode TrigEgammaNavTPBaseTool::childExecute() {

   ATH_MSG_VERBOSE( "child Execute tool " << name() );

   return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );

   return StatusCode::SUCCESS;
}
//**********************************************************************
StatusCode
TrigEgammaNavTPBaseTool::childInitialize() {
    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    if ( (m_trigdec.retrieve()).isFailure() ){
        ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronIsEMTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronLHTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if( (m_MVACalibTool.retrieve()).isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve " << m_MVACalibTool);
        m_applyMVACalib = false;
        m_MVACalibTool = ToolHandle<IegammaMVATool>();
        return StatusCode::FAILURE; 
    }
    else {
        ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
    }
    m_offProbeTightness = m_defaultProbeTightness;

    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPBaseTool::childBook() {
    return StatusCode::SUCCESS;
}
bool TrigEgammaNavTPBaseTool::EventWiseSelection(){

    ATH_MSG_DEBUG("Sanity check of TDT");
    ATH_MSG_DEBUG( "L1: " << m_trigdec->isPassed( "L1_.*" )
            << ", L2: " << m_trigdec->isPassed( "L2_.*" )
            << ", EF: " << m_trigdec->isPassed( "EF_.*" )
            << ", HLT: " << m_trigdec->isPassed( "HLT_.*" ) );
    ATH_MSG_DEBUG("Apply EventWise selection");
    // Check Size of Electron Container
    m_offElectrons = 0;
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
	return false;
    }
    if ( m_offElectrons->size() < 2 ) { // Not enough events for T&P
	ATH_MSG_DEBUG("Not enough Electrons for T&P");
	return false;
    }
    // get jet container of interest
    m_jets = 0;
    m_applyJetNearProbeSelection=true;
    StatusCode sc=m_storeGate->retrieve(m_jets,"AntiKt4EMTopoJets" );
    if (sc.isFailure()) {
        ATH_MSG_DEBUG("Failed to retrieve AntiKt4EMTopoJets, not applying jet-electron selection");
        m_applyJetNearProbeSelection=false;
    }

    ATH_MSG_DEBUG("Total container size  " << m_offElectrons->size());

    // missing more selections
    // check Minimal Trigger Requirements
    if ( !MinimalTriggerRequirement() ) return false;

    return true;
}

bool TrigEgammaNavTPBaseTool::MinimalTriggerRequirement(){
    ATH_MSG_DEBUG("Apply Minimal trigger requirements");
    
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if ( m_trigdec->isPassed("HLT_"+tag) )
            return true;
    }
    return false; // nothing passed
}


StatusCode TrigEgammaNavTPBaseTool::executeTandP(const std::string probeTrigItem){
    
    clearProbeList(); // Clear Probes after each trigger
    ATH_MSG_DEBUG("Execute TandP BaseTool " << m_offElectrons->size());
    float etthr=0;
    float l1thr=0;
    std::string type="";
    std::string l1type="";
    std::string pidname="";
    bool perf=false;
    bool etcut=false;
    parseTriggerName(probeTrigItem,m_defaultProbeTightness,type,etthr,l1thr,l1type,pidname,perf,etcut); // Determines probe PID from trigger
 
    // Set the pid for the Probe after parsing trigger name
    // Not needed, just pass the pidname to the method
    m_offProbeTightness = pidname;
    for(const auto& elTag : *m_offElectrons){
        if( ! isTagElectron(elTag) ) continue;
        for(const auto& elProbe : *m_offElectrons){
            if(elProbe==elTag) continue;
            // Check opposite charge
            if(m_oppositeCharge && (elProbe->charge() == elTag->charge()) ) continue;
            if(!m_oppositeCharge && (elProbe->charge() != elTag->charge()) ) continue;
            ATH_MSG_DEBUG("Execute TandP BaseTool OS"); 
            //Must be an easy way with IParticle
            TLorentzVector el1;
            TLorentzVector el2;
            el1.SetPtEtaPhiE(elTag->pt(), elTag->trackParticle()->eta(), elTag->trackParticle()->phi(), elTag->e());
            el2.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
            float tpPairMass = (el1 + el2).M();
            if( !((tpPairMass > m_ZeeMassMin*1.e3) && (tpPairMass < m_ZeeMassMax*1.e3))){
                ATH_MSG_DEBUG("tag and probe pair not in Z mass window");
                continue;
            } else {
                ATH_MSG_DEBUG("tag and probe pair in Z mass window");
                // Probe available. Good Probe?
                if(!isGoodProbeElectron(elProbe,probeTrigItem,etthr,pidname)) continue; //Ensure passing offline electron selection
                const HLT::TriggerElement *finalFC;

                // Use matching tool and create pair of offline probe and TE
                if ( m_matchTool->match(elProbe, probeTrigItem, finalFC)){
                    std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(elProbe,finalFC);
                    m_probeElectrons.push_back(pairProbe);
                    m_mee.push_back(tpPairMass);                
                } // end of check Probe
                else {
                    std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(elProbe,NULL);
                    m_probeElectrons.push_back(pairProbe);
                    m_mee.push_back(tpPairMass);                
                } // still include the probe
            }
        } // end of for in Probe
    } // end of for in Tags
    ATH_MSG_DEBUG("Execute TandP BaseTool:: Probes " << m_probeElectrons.size());

    return StatusCode::SUCCESS;
}

void TrigEgammaNavTPBaseTool::clearProbeList(){
    m_probeElectrons.clear();
    m_probePhotons.clear();
    m_mee.clear();
}

bool TrigEgammaNavTPBaseTool::isTagElectron(const xAOD::Electron *el){

    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    ATH_MSG_DEBUG("Selecting Tag Electron");
    double GeV = 1000.;

    //Check constituents
    const xAOD::TrackParticle *trk = el->trackParticle();
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }
    ATH_MSG_DEBUG("Track pt " << trk->pt());
    const xAOD::CaloCluster *clus = el->caloCluster();
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }
   

    ATH_MSG_DEBUG("Cluster E "<<clus->e());
    ATH_MSG_DEBUG("Selecting Tag Electron PID");
    if(!el->passSelection(m_offTagTightness)) return false;
    
    ATH_MSG_DEBUG("Selecting Tag Electron Et");
    //Require Et > 25 GeV
    if( !(el->e()/cosh(el->trackParticle()->eta())  > m_tagMinEt*GeV) ){
        return false;
    }
    ATH_MSG_DEBUG("Selecting Tag Electron Eta");
    //fiducial detector acceptance region
    if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 ){
        return false;
    }
    ATH_MSG_DEBUG("Selecting Tag Electron Decision");
    // Check matching to a given trigger
    // The statement below is more general
    bool tagPassed=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if(m_trigdec->isPassed("HLT_"+tag) ) 
            tagPassed=true; 
    }
    if(!tagPassed) {
        ATH_MSG_DEBUG("Failed tag trigger "); 
        return false;
    }
    ATH_MSG_DEBUG("Matching Tag Electron FC");
    bool tagMatched=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if( m_matchTool->matchHLT(el,tag) )
                tagMatched=true;
    }
    if(!tagMatched){
        ATH_MSG_DEBUG("Failed a match ");
        return false; // otherwise, someone matched!
    }

    ATH_MSG_DEBUG("Found a tag electron");
    return true;
}

bool TrigEgammaNavTPBaseTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname){
  ATH_MSG_DEBUG("Apply Pid " << pidname);
  if (pidname == "Tight"){
    return m_electronIsEMTool[0]->accept(eg);
  }
  else if (pidname == "Medium"){
    return m_electronIsEMTool[1]->accept(eg);
  }
  else if (pidname == "Loose"){
    return m_electronIsEMTool[2]->accept(eg);
  }
  else if (pidname == "LHTight"){
    return m_electronLHTool[0]->accept(eg);
  }
  else if (pidname == "LHMedium"){
    return m_electronLHTool[1]->accept(eg);
  }
  else if (pidname == "LHLoose"){
    return m_electronLHTool[2]->accept(eg);
  }
  else ATH_MSG_DEBUG("No Pid tool, continue without PID");
  return false;
}

bool TrigEgammaNavTPBaseTool::isGoodProbeElectron(const xAOD::Electron *el, const std::string trigItem, const float etthr, const std::string pidname){

    double GeV = 1000.;
    //Check constituents
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }
    //fiducial detector acceptance region
    if(m_rmCrack){
        if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 
           ){
            return false;
        }
    }
    if( !(el->e()/cosh(el->trackParticle()->eta())  > (etthr-5.0)*GeV) ){
        return false;
    }
    if(m_forceProbePid){ // Use common probe pid for all triggers
        if(!el->passSelection(m_defaultProbeTightness)) return false;
        // Rerun offline selection
        if(!ApplyElectronPid(el,m_defaultProbeTightness)) return false;
    }
    else {
        ATH_MSG_DEBUG("Probe PID " << trigItem << " " << m_offProbeTightness << " " << pidname);
        //if(!el->passSelection(pidname)) return false;
        // Rerun offline selection
        if(!ApplyElectronPid(el,pidname)) return false;
    }
    if(m_applyJetNearProbeSelection){
        TLorentzVector probeCandidate;
        probeCandidate.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
        Int_t jetsAroundProbeElectron = 0; 
        for(const auto &i_jet : *m_jets){
            TLorentzVector jet;
            jet.SetPtEtaPhiE(i_jet->pt(), i_jet->eta(), i_jet->phi(), i_jet->e());
            if( (jet.Et() > 20*GeV) && (jet.DeltaR(probeCandidate) < 0.4)) jetsAroundProbeElectron++;
        }
        //reject if more than 1 jet close to the probe electron
        if ( jetsAroundProbeElectron >= 2 ) {
            //ATH_MSG_DEBUG("too many jets around object");
            return false; 
        }
    }
    if (m_forceProbeIsolation) {
      if (!isIsolated(el, m_offProbeIsolation)) {
	return false;
      }
    }
    return true; // Good probe electron
}

bool TrigEgammaNavTPBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}

