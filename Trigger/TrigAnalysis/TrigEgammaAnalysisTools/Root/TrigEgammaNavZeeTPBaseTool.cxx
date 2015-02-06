/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************
 * AsgTool: TrigEgammaNavZeeTPBaseTool
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
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "string"

#include "boost/algorithm/string.hpp"
//**********************************************************************
using namespace Trig;
using namespace TrigConf;
using namespace xAOD;
using namespace boost;
TrigEgammaNavZeeTPBaseTool::
TrigEgammaNavZeeTPBaseTool( const std::string& myname )
: TrigEgammaAnalysisBaseTool(myname),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool") 
{
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ApplyMVACalib", m_applyMVACalib=false);
  declareProperty("ElectronIsEMSelector", m_electronPPCutIDTool);
  declareProperty("ElectronLikelihoodTool", m_electronLHTool);
  declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
  declareProperty("MinimumTriggerList",m_minTrig);
  declareProperty("ZeeLowerMass",m_ZeeMassMin=80);
  declareProperty("ZeeUpperMass",m_ZeeMassMax=100);
  declareProperty("OfflineTagIsEM",m_isEMoffTag=egammaPID::ElectronTightPPIso);
  declareProperty("OfflineTagSelector",m_offTagTightness="Tight");// 1=tight, 2=medium, 3=loose
  declareProperty("OppositeCharge",m_oppositeCharge=true);
  declareProperty("OfflineTagMinEt",m_tagMinEt=25);
  declareProperty("TagTrigger", m_tagTrigItem="e28_tight_iloose");
  declareProperty("ProbeTriggerList",m_probeTrigList);
  // just for compile

  HLT::TriggerElement* t = NULL;
  const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
  const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
  bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
  bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
  (void)a; (void)b;
  (void)a1; (void)b1;

  m_PidMap["Tight"]=0;
  m_PidMap["Medium"]=1;
  m_PidMap["Loose"]=2;
}

//**********************************************************************

StatusCode TrigEgammaNavZeeTPBaseTool::childExecute() {

   ATH_MSG_VERBOSE( "child Execute tool " << name() );

   return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavZeeTPBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );

   return StatusCode::SUCCESS;
}
//**********************************************************************
StatusCode
TrigEgammaNavZeeTPBaseTool::childInitialize() {
    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    if ( (m_trigdec.retrieve()).isFailure() ){
        ATH_MSG_ERROR("Could not retrieve Trigger Decision Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronPPCutIDTool.retrieve()).isFailure() ){
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
        m_MVACalibTool = 0;
        return StatusCode::FAILURE; 
    }
    else {
        ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
    }
    m_offProbeTightness = "Tight";
    return StatusCode::SUCCESS;
}
bool TrigEgammaNavZeeTPBaseTool::EventWiseSelection(){

    ATH_MSG_DEBUG("Apply EventWise selection");
    // Check Size of Electron Container
    m_offElectrons = 0;
    if ( (m_storeGate->retrieve(m_offElectrons,"ElectronCollection")).isFailure() ){
        ATH_MSG_ERROR("Failed to retrieve offline Electrons ");
	return false;
    }
    if ( m_offElectrons->size() < 2 ) { // Not enough events for T&P
	ATH_MSG_DEBUG("Not enough Electrons for T&P");
	return false;
    }
    // get jet container of interest
    m_jets = 0;
    StatusCode sc=m_storeGate->retrieve(m_jets,"AntiKt4EMTopoJets" );
    if (sc.isFailure()) {
        ATH_MSG_ERROR("Failed to retrieve AntiKt4EMTopoJets with key ");
    }

    ATH_MSG_DEBUG("Total container size  " << m_offElectrons->size());

    // missing more selections
    // check Minimal Trigger Requirements
    if ( !MinimalTriggerRequirement() ) return false;

    return true;
}

bool TrigEgammaNavZeeTPBaseTool::MinimalTriggerRequirement(){
    ATH_MSG_DEBUG("Apply Minimal trigger requirements");
    for(unsigned int i=0;i<m_minTrig.size();i++){
        std::string& trigItem = m_minTrig[i];
        if ( m_trigdec->isPassed("HLT_"+trigItem) )
            return true;
    }
    return false; // nothing passed
}
void TrigEgammaNavZeeTPBaseTool::setProbePid(const std::string probeTrigItem){
    if(contains(probeTrigItem,"tight")) m_offProbeTightness = "Tight"; 
    else if(contains(probeTrigItem,"medium")) m_offProbeTightness = "Medium"; 
    else if(contains(probeTrigItem,"loose")) m_offProbeTightness = "Loose"; 

}
StatusCode TrigEgammaNavZeeTPBaseTool::executeTandP(const std::string probeTrigItem){

    ATH_MSG_DEBUG("Execute TandP BaseTool" << m_offElectrons->size());
    m_probeElectrons.clear();
    
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
                if(!isGoodProbeElectron(elProbe,probeTrigItem)) continue; //Ensure passing offline electron selection
                const HLT::TriggerElement *finalFC;
                if ( TrigEgammaNavZeeTPBaseTool::isProbeElectron(elProbe, probeTrigItem, finalFC)){
                    std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(elProbe,finalFC);
                    m_probeElectrons.push_back(pairProbe);
                } // end of check Probe
                else {
                    std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(elProbe,NULL);
                    m_probeElectrons.push_back(pairProbe);
                } // still include the probe
            }
        } // end of for in Probe
    } // end of for in Tags
    ATH_MSG_DEBUG("Execute TandP BaseTool:: Probes " << m_probeElectrons.size());

    return StatusCode::SUCCESS;
}

void TrigEgammaNavZeeTPBaseTool::clearProbeList(){
    m_probeElectrons.clear();
}

bool TrigEgammaNavZeeTPBaseTool::isTagElectron(const xAOD::Electron *el){

    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    ATH_MSG_DEBUG("Selecting Tag Electron");
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
    ATH_MSG_DEBUG("Selecting Tag Electron isEM");
    //Require offline Tight++
    //unsigned int isEMobtained = m_electronPPCutIDTool[TagTightness-1]->execute(el,1000.,false);
    //(void)isEMobtained;
    bool passPid = false; 
    if(contains(m_tagTrigItem,"lh")){
        const Root::TAccept& acc = m_electronLHTool[m_PidMap[m_offTagTightness]]->accept(el);
        passPid = (bool) (acc);
    }
    else{
        const Root::TAccept& acc = m_electronPPCutIDTool[m_PidMap[m_offTagTightness]]->accept(el);
        passPid = (bool) (acc);
    }
    if(!passPid) return false;
    //ATH_MSG_DEBUG("Selecting Tag Electron isEM Passes " << isEM << " " << isEMobtained); 
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
    if(!m_trigdec->isPassed("HLT_"+m_tagTrigItem) ) {
        ATH_MSG_DEBUG("Failed trigger " << m_tagTrigItem);
        return false;
    }
    ATH_MSG_DEBUG("Selecting Tag Electron FC");
    // Get the container of online electrons associated to passed items
    Trig::FeatureContainer fc = m_trigdec->features("HLT_"+m_tagTrigItem);
    const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec_el = fc.get<xAOD::ElectronContainer>();

    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
    ATH_MSG_DEBUG("Selecting Tag Electron Match");
    bool matched=false;
    for(auto elfeat : vec_el){
        const xAOD::ElectronContainer *elCont = elfeat.cptr();
        for(const auto& eg : *elCont){
            TLorentzVector elLV;
            elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
            if(elLV.DeltaR(eloffLV) < 0.07) matched = true;
        }
    }
    if(!matched) return false; // otherwise, someone matched!

    ATH_MSG_DEBUG("Found a tag electron");
    return true;
}
bool TrigEgammaNavZeeTPBaseTool::isGoodProbeElectron(const xAOD::Electron *el, const std::string trigItem){

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
    if ( (fabs(el->eta())>1.37 && fabs(el->eta())<1.52) || fabs(el->eta())>2.47 
       ){
        return false;
    }

    bool passPid = false; 
    if(contains(trigItem,"lh")){
        const Root::TAccept& acc = m_electronLHTool[m_PidMap[m_offProbeTightness]]->accept(el);
        passPid = (bool) (acc);
    }
    else{
        const Root::TAccept& acc = m_electronPPCutIDTool[m_PidMap[m_offProbeTightness]]->accept(el);
        passPid = (bool) (acc);
    }
    if(!passPid) return false;
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
    return true; // Good probe electron
}
bool TrigEgammaNavZeeTPBaseTool::isProbeElectron(const xAOD::Electron *el,
        const std::string trigItem,
        const HLT::TriggerElement*& finalFC){
    finalFC=NULL;

    TLorentzVector eloffLV;
    eloffLV.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());

    // Get the container of online electrons associated to passed items
    Trig::FeatureContainer fc = (m_trigdec->features("HLT_"+trigItem,TrigDefs::alsoDeactivateTEs));

    const std::vector< Trig::Feature<xAOD::ElectronContainer> > vec_el = fc.get<xAOD::ElectronContainer>("",TrigDefs::alsoDeactivateTEs);

    bool matched=false;
    for(auto elfeat : vec_el){
        const xAOD::ElectronContainer *elCont = elfeat.cptr();
        for(const auto& eg : *elCont){
            TLorentzVector elLV;
            elLV.SetPtEtaPhiE(eg->pt(), eg->trackParticle()->eta(), eg->trackParticle()->phi(), eg->e());
            if(elLV.DeltaR(eloffLV) < 0.07){
                finalFC = (elfeat.te());
                return true;
            }
        }
    }

    const std::vector< Trig::Feature<xAOD::TrigElectronContainer> > vec_l2el = fc.get<xAOD::TrigElectronContainer>("",TrigDefs::alsoDeactivateTEs);

    matched=false;
    for(auto elfeat : vec_l2el){
        const xAOD::TrigElectronContainer *elCont = elfeat.cptr();
        for(const auto& eg : *elCont){
            TLorentzVector elLV;
            elLV.SetPtEtaPhiE(eg->pt(), eg->eta(), eg->phi(), eg->e());
            if(elLV.DeltaR(eloffLV) < 0.07){
                finalFC = (elfeat.te());
                return true;
            }
        }
    }

    const std::vector< Trig::Feature<TrigRoiDescriptor> > initRois = fc.get<TrigRoiDescriptor>();
    if ( initRois.size() < 1 ) return false;
    Trig::Feature<xAOD::EmTauRoI> itEmTau = m_trigdec->ancestor<xAOD::EmTauRoI>(initRois[0]);

    const xAOD::EmTauRoI *l1 = itEmTau.cptr();
    TLorentzVector elLV;
    elLV.SetPtEtaPhiM(l1->emClus(), l1->eta(), l1->phi(), 0);
    if(elLV.DeltaR(eloffLV) < 0.15) matched = true;

    if ( matched ){
        finalFC = (itEmTau.te());
        return true;
    }

    return false; // otherwise, someone matched!
}

bool TrigEgammaNavZeeTPBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}
