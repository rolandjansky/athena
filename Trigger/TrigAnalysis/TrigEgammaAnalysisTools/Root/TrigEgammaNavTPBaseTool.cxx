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
#include "xAODEventInfo/EventInfo.h"
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
  declareProperty("ElectronIsEMSelector", m_electronIsEMTool);
  declareProperty("ElectronLikelihoodTool", m_electronLHTool);
  declareProperty("ZeeLowerMass",m_ZeeMassMin=80);
  declareProperty("ZeeUpperMass",m_ZeeMassMax=100);
  declareProperty("OfflineTagSelector",m_offTagTightness="Tight");
  declareProperty("OfflineProbeSelector",m_defaultProbeTightness="Loose"); // new
  declareProperty("ForceProbePid", m_forceProbePid=false); // new
  declareProperty("OppositeCharge",m_oppositeCharge=true);
  declareProperty("OfflineTagMinEt",m_tagMinEt=25);
  declareProperty("TagTriggerList", m_tagTrigList);
  declareProperty("TriggerList",m_trigInputList);
  declareProperty("CategoryList",m_categories);
  declareProperty("RemoveCrack", m_rmCrack=true); //new
  declareProperty("OfflineProbeIsolation", m_offProbeIsolation="Loose");
  declareProperty("ForceProbeIsolation", m_forceProbeIsolation=false);

  // Obsolete, not used... // Maps should be static
  // Obsolete, not used... m_PidToolMap["Tight"]=0;
  // Obsolete, not used... m_PidToolMap["Medium"]=1;
  // Obsolete, not used... m_PidToolMap["Loose"]=2;
  // Obsolete, not used... m_PidToolMap["VLoose"]=3;

  m_offElectrons=nullptr;
  m_jets=nullptr;
  m_eventInfo=nullptr;
  m_truthContainer=nullptr;
  m_applyJetNearProbeSelection=true; 
  m_skipTrigger=false; 
  
}

//**********************************************************************

StatusCode TrigEgammaNavTPBaseTool::childExecute() {

   ATH_MSG_VERBOSE( "child Execute tool " << name() );

   return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavTPBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );
   clearProbeList();
   clearPairList();
   //m_offElectrons->clearDecorations();

   return StatusCode::SUCCESS;
}
//**********************************************************************
StatusCode
TrigEgammaNavTPBaseTool::childInitialize() {

    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    if ( (m_electronIsEMTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    if ( (m_electronLHTool.retrieve()).isFailure() ){
        ATH_MSG_ERROR( "Could not retrieve Selector Tool! Can't work");
        return StatusCode::FAILURE;
    }
    m_offProbeTightness = m_defaultProbeTightness;

    return StatusCode::SUCCESS;
}
StatusCode TrigEgammaNavTPBaseTool::childBook() {
    return StatusCode::SUCCESS;
}
bool TrigEgammaNavTPBaseTool::EventWiseSelection(){

    ATH_MSG_DEBUG("Apply EventWise selection");
    
    // disable trigger tag if there is no trigger in tag list.
    // This will be used to the offline tag and probe analysis
    if(m_tagTrigList.empty()){
      m_skipTrigger=true; 
      ATH_MSG_INFO("Disable trigger tags because trigger tags list is empty.");
    }

    m_eventInfo=0;
    m_offElectrons = 0;
    m_jets = 0;
    m_truthContainer=0;

    
    if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
        ATH_MSG_WARNING("Failed to retrieve eventInfo ");
        return false;
    }
    
    if(m_storeGate->contains<xAOD::TruthParticleContainer>("egammaTruthParticles")){
      if(m_storeGate->retrieve(m_truthContainer,"egammaTruthParticles").isFailure()){
        ATH_MSG_WARNING("Could not retrieve xAOD::TruthParticleContainer 'egammaTruthParticles'");
        return false;
      }
    }// protection

    if (m_eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error) {
        ATH_MSG_DEBUG("Event not passing LAr");
        return false;
    }
    
    hist1(m_anatype+"_CutCounter")->Fill("LAr",1);
    
    if ( (m_storeGate->retrieve(m_offElectrons,m_offElContKey)).isFailure() ){
      ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
	    return false;
    }
    hist1(m_anatype+"_CutCounter")->Fill("RetrieveElectrons",1);
    // Check Size of Electron Container
    if ( m_offElectrons->size() < 2 ) { // Not enough events for T&P
	    ATH_MSG_DEBUG("Not enough Electrons for T&P");
	    return false;
    }

    hist1(m_anatype+"_CutCounter")->Fill("TwoElectrons",1);
    // get jet container of interest

    m_applyJetNearProbeSelection=true;
    StatusCode sc=m_storeGate->retrieve(m_jets,"AntiKt4EMTopoJets" );
    if (sc.isFailure()) {
        ATH_MSG_DEBUG("Failed to retrieve AntiKt4EMTopoJets, not applying jet-electron selection");
        m_applyJetNearProbeSelection=false;
    }

    ATH_MSG_DEBUG("Total container size  " << m_offElectrons->size());

    TrigEgammaAnalysisBaseTool::calculatePileupPrimaryVertex();   

    //Skip the trigger minimal requirement
    if(m_skipTrigger)  return true;

    // missing more selections
    // check Minimal Trigger Requirements
    if ( !MinimalTriggerRequirement() ) return false;
    hist1(m_anatype+"_CutCounter")->Fill("PassTrigger",1);

    
    return true;
}

bool TrigEgammaNavTPBaseTool::MinimalTriggerRequirement(){
    ATH_MSG_DEBUG("Apply Minimal trigger requirements");
    
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
	ATH_MSG_DEBUG("Checking if accpeted by  "<<tag<< "...");

        if ( tdt()->isPassed(tag) ){
	    ATH_MSG_DEBUG(tag<< " is passed!");
            return true;
	}
    }
    ATH_MSG_DEBUG("Trigger selection failed!");
    return false; // nothing passed
}


void TrigEgammaNavTPBaseTool::executeTandP(){
    
    clearProbeList(); // Clear Probes after each trigger
    ATH_MSG_DEBUG("executeTandP()  m_offElectrons->size() =  " << m_offElectrons->size());
    for(const auto& elTag : *m_offElectrons){
        if( ! isTagElectron(elTag) ) continue;
        for(const auto& elProbe : *m_offElectrons){  // Dress the probes with updated Pid decision
	    ATH_MSG_DEBUG("checking probe electron " << elProbe);
            hist1(m_anatype+"_ProbeCutCounter")->Fill("Electrons",1);
            if(elProbe==elTag) continue;
            hist1(m_anatype+"_ProbeCutCounter")->Fill("NotTag",1);
            // Check opposite charge
            if(m_oppositeCharge && (elProbe->charge() == elTag->charge()) ) continue;
            hist1(m_anatype+"_ProbeCutCounter")->Fill("OS",1);
            if(!m_oppositeCharge && (elProbe->charge() != elTag->charge()) ) continue;
            hist1(m_anatype+"_ProbeCutCounter")->Fill("SS",1);
            ATH_MSG_DEBUG("Execute TandP BaseTool OS"); 
            if(m_doJpsiee){
		ATH_MSG_DEBUG("Doing J/Psi..."); 

              float  Jpsieelifetime = GetPseudoLifetime(elTag,elProbe);

              if(  dR(elTag->caloCluster()->eta(),elTag->caloCluster()->phi(),
                      elProbe->caloCluster()->eta(),elProbe->caloCluster()->phi()) <= 0.2 ){
                ATH_MSG_DEBUG("dR(elTag,elProbe)<0.2");
                continue;

              }
              else if(Jpsieelifetime<-1 || 0.2<Jpsieelifetime){
                ATH_MSG_DEBUG("tag and probe pair not in Jpsi lifetime window");
                continue;
              }

	      ATH_MSG_DEBUG("OK, passes Jpsi lifetime window"); 
            }
            //Must be an easy way with IParticle
            TLorentzVector el1;
            TLorentzVector el2;
            el1.SetPtEtaPhiE(elTag->pt(), elTag->trackParticle()->eta(), elTag->trackParticle()->phi(), elTag->e());
            el2.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
            float tpPairMass = (el1 + el2).M();
            if( !((tpPairMass > m_ZeeMassMin*1.e3) && (tpPairMass < m_ZeeMassMax*1.e3))){
                ATH_MSG_DEBUG("tag and probe pair with mass " << tpPairMass << " not in mass window " << m_ZeeMassMin << ", " << m_ZeeMassMax);
                continue;
            } else {
                hist1(m_anatype+"_ProbeCutCounter")->Fill("ZMass",1);
                ATH_MSG_DEBUG("tag and probe pair with mass = " << tpPairMass << " is within  mass window " << m_ZeeMassMin << " , " << m_ZeeMassMax );
                // Probe available. Good Probe?
                if(!isGoodProbeElectron(elProbe)) continue;
                hist1(m_anatype+"_ProbeCutCounter")->Fill("GoodProbe",1);
                xAOD::Electron *selProbe = new xAOD::Electron(*elProbe);              
                DressPid(selProbe);
                m_probeElectrons.push_back(selProbe);
                hist1(m_anatype+"_Mee")->Fill(tpPairMass/1.e3);
            }
        } // end of for in Probe
    } // end of for in Tags
}

void TrigEgammaNavTPBaseTool::matchObjects(const std::string probeTrigItem){
    ATH_MSG_DEBUG("matchObjects("<< probeTrigItem << ") on " << m_probeElectrons.size() << " probe electrons...");

    clearPairList();
    for(unsigned int i=0;i<m_probeElectrons.size();i++){
        const HLT::TriggerElement *finalFC;

        // Use matching tool and create pair of offline probe and TE
        bool m_IsMatched = match()->match(m_probeElectrons[i], probeTrigItem, finalFC);
	ATH_MSG_DEBUG("matching probe " << i << " to " << probeTrigItem << " resulted in " << (int) m_IsMatched);
        if ( m_IsMatched){
	    ATH_MSG_DEBUG("Probe " << i << " Matched to " << probeTrigItem << "!");
            std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(m_probeElectrons[i],finalFC);
            m_pairObj.push_back(pairProbe);
        } // end of check Probe
        else {
	    ATH_MSG_DEBUG("Probe " << i << " Failed to match  " << probeTrigItem << " Putting a nul pointer on probes list!");
            std::pair<const xAOD::Electron*,const HLT::TriggerElement*> pairProbe(m_probeElectrons[i],nullptr);
            m_pairObj.push_back(pairProbe);
        } // still include the probe
    }
}

void TrigEgammaNavTPBaseTool::clearProbeList(){
    m_probeElectrons.clear();
    m_probePhotons.clear();
}

void TrigEgammaNavTPBaseTool::clearPairList(){
    m_pairObj.clear();
}

void TrigEgammaNavTPBaseTool::clearDecorations(){
    if(m_pairObj.size()!=0) m_offElectrons->clearDecorations();
}

bool TrigEgammaNavTPBaseTool::isTagElectron(const xAOD::Electron *el){

    hist1(m_anatype+"_TagCutCounter")->Fill("Electrons",1);
    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    ATH_MSG_DEBUG("Selecting Tag Electron");
    double GeV = 1000.;

    //Check constituents
    const xAOD::TrackParticle *trk = el->trackParticle();
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("Bad tag: No track Particle");
        return false;
    }
    hist1(m_anatype+"_TagCutCounter")->Fill("HasTrack",1);
    ATH_MSG_DEBUG("Track pt " << trk->pt());
    const xAOD::CaloCluster *clus = el->caloCluster();
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("Bad tag: No caloCluster");
        return false;
    }
    hist1(m_anatype+"_TagCutCounter")->Fill("HasCluster",1);

    ATH_MSG_DEBUG("Cluster E "<<clus->e());
    ATH_MSG_DEBUG("Selecting Tag Electron PID");
    if (!ApplyElectronPid(el, m_offTagTightness)) return false;
    hist1(m_anatype+"_TagCutCounter")->Fill("GoodPid",1);
    ATH_MSG_DEBUG("Selecting Tag Electron Et");
    //Require Et > 25 GeV
    if( !(el->e()/cosh(el->trackParticle()->eta())  > m_tagMinEt*GeV) ){
        return false;
    }
    hist1(m_anatype+"_TagCutCounter")->Fill("Et",1);
    ATH_MSG_DEBUG("Selecting Tag Electron Eta");
    //fiducial detector acceptance region
    float absEta = fabs(el->caloCluster()->etaBE(2));
    if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
        return false;
    }
    hist1(m_anatype+"_TagCutCounter")->Fill("Eta",1);

    ATH_MSG_DEBUG("Checking electron object quality");
    if (!el->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    hist1(m_anatype+"_TagCutCounter")->Fill("IsGoodOQ",1);

    
    if(m_skipTrigger){
      ATH_MSG_DEBUG("Found a tag electron"); 
      return true;
    }

    ATH_MSG_DEBUG("Selecting Tag Electron Decision");
    // Check matching to a given trigger
    // The statement below is more general
    bool tagPassed=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
      std::string tag; // Here I'll put the tag electron chain after interpreting TnP chain
      std::string TnP = m_tagTrigList.at(ilist);
      if(tdt()->isPassed(TnP)){ 
	  ATH_MSG_DEBUG("TnP trigger   "<< TnP << " passed!");
        if(m_tp){
	  ATH_MSG_DEBUG("Interpretting  "<< TnP << " as a Tag and probe trigger...");
          std::string p1trigger;
          std::string p2trigger;
          if(splitTriggerName(TnP,p1trigger,p2trigger)){
	    ATH_MSG_DEBUG("Split trigger name for "<< TnP << " in " << p1trigger << " and "  << p2trigger);
            if(fabs(p1trigger.find("tight"))<14) tag=p1trigger;
            if(fabs(p2trigger.find("tight"))<14) tag=p2trigger;
          } else{
	      ATH_MSG_DEBUG("Failed to split trigger " << tag);
	  }
	  ATH_MSG_DEBUG("Trying to match tag to  "<< tag );
          if( match()->match(el,tag) ){
	    ATH_MSG_DEBUG("Succeeded to match to   "<< tag );
            tagPassed=true;
	  } else {
	    ATH_MSG_DEBUG("Failed to match to   "<< tag );
	  }
	  
        }
        else{
	  ATH_MSG_DEBUG("m_tp flag set to False... using tagPassed=true");
          tagPassed=true; 
        }
      }
    }
    if(!tagPassed) {
        ATH_MSG_DEBUG("Failed tag trigger "); 
        return false;
    }
    ATH_MSG_DEBUG("tag trigger passed "); 
    hist1(m_anatype+"_TagCutCounter")->Fill("PassTrigger",1);
    ATH_MSG_DEBUG("Matching Tag Electron FC");
    bool tagMatched=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if( match()->match(el,tag) )
                tagMatched=true;
    }
    if(!tagMatched){
        ATH_MSG_DEBUG("Failed a match ");
        return false; // otherwise, someone matched!
    }
    hist1(m_anatype+"_TagCutCounter")->Fill("MatchTrigger",1);
    ATH_MSG_DEBUG("Found a tag electron");
    return true;
}


void TrigEgammaNavTPBaseTool::DressPid(const xAOD::Electron *eg){
    for(int ipid=0;ipid<3;ipid++){
        const Root::TAccept& accept=m_electronIsEMTool[ipid]->accept(eg);
        const std::string pidname="is"+m_isemname[ipid];
        eg->auxdecor<bool>(pidname)=static_cast<bool>(accept);
    }
    for(int ipid=0;ipid<3;ipid++){
        const Root::TAccept& accept=m_electronLHTool[ipid]->accept(eg);
        const std::string pidname="is"+m_lhname[ipid];
        eg->auxdecor<bool>(pidname)=static_cast<bool>(accept);
    }
    eg->auxdecor<bool>("Isolated")=isIsolated(eg, m_offProbeIsolation);
}

bool TrigEgammaNavTPBaseTool::ApplyElectronPid(const xAOD::Electron *eg, const std::string pidname){
    ATH_MSG_DEBUG("ApplyElectronPid(" << eg << " , " << pidname << ")");
    
    if (pidname == "Tight"){
        const Root::TAccept& accept=m_electronIsEMTool[0]->accept(eg);
	ATH_MSG_DEBUG("Pass " << pidname << " -> " << accept );
        return static_cast<bool>(accept);
    }
    else if (pidname == "Medium"){
        const Root::TAccept& accept=m_electronIsEMTool[1]->accept(eg);
	ATH_MSG_DEBUG("Pass " << pidname << " -> " << accept );
        return static_cast<bool>(accept);
    }
    else if (pidname == "Loose"){
        const Root::TAccept& accept=m_electronIsEMTool[2]->accept(eg);
	ATH_MSG_DEBUG("Pass " << pidname << " -> " << accept );
        return static_cast<bool>(accept);
    }
    else if (pidname == "LHTight"){
        const Root::TAccept& accept=m_electronLHTool[0]->accept(eg);
	ATH_MSG_DEBUG("Pass " << pidname << " -> " << accept );
        return static_cast<bool>(accept);
    }
    else if (pidname == "LHMedium"){
        const Root::TAccept& accept=m_electronLHTool[1]->accept(eg);
	ATH_MSG_DEBUG("Pass " << pidname << " -> " << accept );
        return static_cast<bool>(accept);
    }
    else if (pidname == "LHLoose"){
        const Root::TAccept& accept=m_electronLHTool[2]->accept(eg);
	ATH_MSG_DEBUG("Pass " << pidname << " -> " << accept );
        return static_cast<bool>(accept);
    }
    else ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return false;
}

bool TrigEgammaNavTPBaseTool::isGoodProbeElectron(const xAOD::Electron *el){
    ATH_MSG_DEBUG("isGoodProbeElectron()");

    double GeV = 1000.;
    //Check constituents
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("Bad probe: No track Particle");
        return false;
    }
    hist1(m_anatype+"_ProbeCutCounter")->Fill("HasTrack",1);
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("Bad probe: No caloCluster");
        return false;
    }
    hist1(m_anatype+"_ProbeCutCounter")->Fill("HasCluster",1);
    //fiducial detector acceptance region
    if(m_rmCrack){
        float absEta = fabs(el->caloCluster()->etaBE(2));
        if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
        ATH_MSG_DEBUG("Bad probe: Outside eta");
            return false; 
        }
    }
    hist1(m_anatype+"_ProbeCutCounter")->Fill("Eta",1);
    /*if( !(el->e()/cosh(el->trackParticle()->eta())  > (etthr-5.0)*GeV) ){
        return false;
    }
    hist1("ProbeCutCounter")->Fill("Et",1);*/

    ATH_MSG_DEBUG("Checking electron object quality");
    if (!el->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    hist1(m_anatype+"_ProbeCutCounter")->Fill("IsGoodOQ",1);
    //DressPid(el);
    hist1(m_anatype+"_ProbeCutCounter")->Fill("GoodPid",1);
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
            ATH_MSG_DEBUG("Bad probe: too many jets around object");
            return false; 
        }
    }
    hist1(m_anatype+"_ProbeCutCounter")->Fill("NearbyJet",1);
    ATH_MSG_DEBUG("Is a good probe!");
    return true; // Good probe electron
}

bool TrigEgammaNavTPBaseTool::passedTrigger(const HLT::TriggerElement* obj){
    bool passed=false;
    if ( obj->getActiveState() ) passed = true;
    return passed;
}

float TrigEgammaNavTPBaseTool::GetPseudoLifetime(const xAOD::Electron *el1,const xAOD::Electron *el2){

  TLorentzVector el1track;
  TLorentzVector el2track;

  float Et1=hypot(el1->caloCluster()->m(),el1->caloCluster()->pt())/cosh(el1->trackParticle()->eta());
  float Et2=hypot(el2->caloCluster()->m(),el2->caloCluster()->pt())/cosh(el1->trackParticle()->eta());

  el1track.SetPtEtaPhiM(Et1, el1->trackParticle()->eta(), el1->trackParticle()->phi(),0.511);
  el2track.SetPtEtaPhiM(Et2, el2->trackParticle()->eta(), el2->trackParticle()->phi(), 0.511);

  float lxy=simple_lxy(0,
                       el1->trackParticle()->d0() , el2->trackParticle()->d0(),
                       el1->trackParticle()->phi(),   el2->trackParticle()->phi(),
                       Et1,              Et2,
                       0.0,                  0.0);

  float  m_ptEECalo  = (el1track+el2track).Pt();

  return lxy*3096.916/(0.299792458*m_ptEECalo);

}

double TrigEgammaNavTPBaseTool::simple_lxy(int flag,   double d1,  double d2, double phi1, double phi2, double pt1, double pt2, double vx, double vy)
{
  double simple = -99999.;

  //require minimum opening angle of 1 microradian.                                                                                                                                                                 
  if(fabs(phi1 - phi2) < 1e-6) return simple;

  double simpleXv = (-d2*cos(phi1) + d1*cos(phi2)) / sin(phi2-phi1);
  double simpleYv = (-d2*sin(phi1) + d1*sin(phi2)) / sin(phi2-phi1);

  double rxy  = sqrt((simpleXv-vx)*(simpleXv-vx) +
                     (simpleYv-vy)*(simpleYv-vy) );

  double f1 = (fabs(pt1)*cos(phi1)+fabs(pt2)*cos(phi2));
  double f2 = (fabs(pt1)*sin(phi1)+fabs(pt2)*sin(phi2));
  double  c = sqrt( f1*f1 + f2*f2 );

  if ( c == 0 ) return simple;

  double a =  (simpleXv-vx)*f1;
  double b =  (simpleYv-vy)*f2;

  if (flag == 1)
    return rxy;
  else
    return (a+b)/c;
}
