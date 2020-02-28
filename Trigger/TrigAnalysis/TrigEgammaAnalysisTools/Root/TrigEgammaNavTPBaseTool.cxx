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
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
#include "TrigConfxAOD/xAODConfigTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "PATCore/AcceptData.h"
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
  /* TandP configuration algorithm */
  declareProperty("TagTriggerList"                , m_tagTrigList                         );
  declareProperty("TriggerList"                   , m_trigInputList                       );
  declareProperty("ZeeLowerMass"                  , m_ZeeMassMin=80                       );
  declareProperty("ZeeUpperMass"                  , m_ZeeMassMax=100                      );
  declareProperty("OfflineTagSelector"            , m_offTagTightness="Tight"             );
  declareProperty("OfflineProbeSelector"          , m_offProbeTightness="Loose"           );
  declareProperty("OppositeCharge"                , m_oppositeCharge=true                 );
  declareProperty("OfflineTagMinEt"               , m_tagMinEt=25                         );
  declareProperty("OfflineProbeMinEt"             , m_probeMinEt=4                        );
  declareProperty("OfflineProbeIsolation"         , m_offProbeIsolation="Loose"           );
  declareProperty("RemoveCrack"                   , m_rmCrack=true                        );
  declareProperty("ApplyMinimalTrigger"           , m_applyMinimalTrigger=true            );
  declareProperty("ApplyJetNearProbeSelectionr"   , m_applyJetNearProbeSelection=true     );
  declareProperty("DoJpsiee"                      , m_doJpsiee=false                      );



  /*  Monitoring paths and names */
  declareProperty("BasePath"  , m_dir             );
  declareProperty("Analysis"  , m_anatype="Zee"   );
 

  // Maps should be static
  m_PidToolMap["Tight"]=0;
  m_PidToolMap["Medium"]=1;
  m_PidToolMap["Loose"]=2;
  m_PidToolMap["VLoose"]=3;

}




StatusCode TrigEgammaNavTPBaseTool::childFinalize() {

   ATH_MSG_VERBOSE( "child Finalize tool " << name() );
   return StatusCode::SUCCESS;
}



StatusCode TrigEgammaNavTPBaseTool::childInitialize() {
    
    ATH_MSG_VERBOSE( "child Initialize tool " << name() );
    ATH_CHECK(m_eventInfoKey.initialize());
    ATH_CHECK(m_offElectronKey.initialize());
    ATH_CHECK(m_jetKey.initialize());
    return StatusCode::SUCCESS;
}





bool TrigEgammaNavTPBaseTool::executeTandP( const EventContext& ctx, 
                                            std::vector<const xAOD::Electron*> &probeElectrons) const
{
    
    ToolHandle<GenericMonitoringTool> monGroup;
    //auto monGroup = findGroup( m_dir+"/Expert/Event" );


    SG::ReadHandle<xAOD::EventInfo> eventInfo( m_eventInfoKey, ctx );
    if( !eventInfo.isValid() ){
      ATH_MSG_WARNING("Failed to retrieve EventInfo");
      return false;
    }


    if (eventInfo->errorState(xAOD::EventInfo::LAr) == xAOD::EventInfo::Error) {
        ATH_MSG_WARNING("Event not passing LAr");
        return false;
    }
    //fill( monGroup, m_anatype+"_CutCounter", "LAr");
 
    
    SG::ReadHandle<xAOD::ElectronContainer> offElectrons(m_offElectronKey, ctx);

    if(!offElectrons.isValid())
    {
      ATH_MSG_WARNING("Failed to retrieve offline Electrons ");
	    return false;
    }
    //fill( monGroup, m_anatype+"_CutCounter", "RetrieveElectrons");

    ATH_MSG_DEBUG( "Electron size is " << offElectrons->size() );

    // Check Size of Electron Container
    if ( offElectrons->size() < 2 ) { // Not enough events for T&P
	    ATH_MSG_DEBUG("Not enough Electrons for T&P");
	    return false;
    }
    //fill( monGroup, m_anatype+"_CutCounter", "TwoElectrons");
 


    SG::ReadHandle<xAOD::JetContainer> jets(m_jetKey);
    if(!jets.isValid()){
      ATH_MSG_WARNING("Failed to retrieve JetContainer");
      return false;
    }

    ATH_MSG_DEBUG( "Jet size is " << jets->size());


    if(!m_tagTrigList.empty()){
      if(m_applyMinimalTrigger){ 
        if ( !MinimalTriggerRequirement() ) 
          return false;
      }
        //fill( monGroup, m_anatype+"_CutCounter", "PassTrigger");
    }else{
      ATH_MSG_DEBUG("Disable trigger tags because trigger tags list is empty.");
    }

 


    
    ATH_MSG_DEBUG("Execute TandP BaseTool " << offElectrons->size());
    for(const auto& elTag : *offElectrons)
    {
        if( ! isTagElectron( monGroup, elTag) ) continue;
        
        for(const auto& elProbe : *offElectrons)
        {  // Dress the probes with updated Pid decision
            
            //fill( monGroup, m_anatype+"_ProbeCutCounter", "Electrons");
            if(elProbe==elTag) continue;
            //fill( monGroup, m_anatype+"_ProbeCutCounter", "NotTag");
            // Check opposite charge
            if(m_oppositeCharge && (elProbe->charge() == elTag->charge()) ) continue;
            //fill( monGroup, m_anatype+"_ProbeCutCounter", "OS");
            if(!m_oppositeCharge && (elProbe->charge() != elTag->charge()) ) continue;
            //fill( monGroup, m_anatype+"_ProbeCutCounter", "SS");
            ATH_MSG_DEBUG("Execute TandP BaseTool OS"); 
            
            if(m_doJpsiee){

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

            }
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
                //fill( monGroup, m_anatype+"_ProbeCutCounter", "ZMass");
                ATH_MSG_DEBUG("tag and probe pair in Z mass window");
                // Probe available. Good Probe?
                if(!isGoodProbeElectron(monGroup, elProbe, jets.cptr())) continue;
                //fill( monGroup, m_anatype+"_ProbeCutCounter", "GoodProbe");
                xAOD::Electron *selProbe = new xAOD::Electron(*elProbe);              
                DressPid(selProbe);
                
                probeElectrons.push_back(selProbe);
                //fill( monGroup, m_anatype+"_Mee", pPairMass/1.e3);
            }
        } // end of for in Probe
    } // end of for in Tags


    ATH_MSG_DEBUG( "Number of probes found is " << probeElectrons.size() );
    return true;
}



bool TrigEgammaNavTPBaseTool::MinimalTriggerRequirement() const {
    
    ATH_MSG_DEBUG("Apply Minimal trigger requirements");
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
        std::string tag = m_tagTrigList.at(ilist);
        if ( tdt()->isPassed(tag) )
            return true;
    }
    return false;
}



void TrigEgammaNavTPBaseTool::matchObjects(const std::string probeTrigItem, 
                                           std::vector<const xAOD::Electron*> probeElectrons,
                                           std::vector<std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element>> &pairObj ) const
{
    for( const auto *el : probeElectrons)
    {
        TrigEgammaMatchingUtils::Element obj(probeTrigItem);
        match()->match(el, probeTrigItem, obj);
        std::pair<const xAOD::Egamma*, const TrigEgammaMatchingUtils::Element> pairProbe(el,obj);
        pairObj.push_back(pairProbe);
    }
}





bool TrigEgammaNavTPBaseTool::isTagElectron( ToolHandle<GenericMonitoringTool> /*monGroup*/, 
                                               const xAOD::Electron *el) const 
{
    //fill( monGroup, m_anatype+"_TagCutCounter", "Electrons");
    
    // Tag the event
    // Require offline tight electron
    // Match to e24_tight1_L1EM20V
    ATH_MSG_DEBUG("Selecting Tag Electron");

    //Check constituents
    const xAOD::TrackParticle *trk = el->trackParticle();
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }

    //fill( monGroup, m_anatype+"_TagCutCounter", "HasTrack");
    ATH_MSG_DEBUG("Track pt " << trk->pt());
    const xAOD::CaloCluster *clus = el->caloCluster();
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }

    //fill( monGroup, m_anatype+"_TagCutCounter", "HasCluster");

    ATH_MSG_DEBUG("Cluster E "<<clus->e());
    ATH_MSG_DEBUG("Selecting Tag Electron PID");
    if (!ApplyElectronPid(el, m_offTagTightness)) return false;
    //fill( monGroup, m_anatype+"_TagCutCounter", "GoodPid");
    

    ATH_MSG_DEBUG("Selecting Tag Electron Et");
    //Require Et > 25 GeV
    if( !(el->e()/cosh(el->trackParticle()->eta())  > m_tagMinEt*Gaudi::Units::GeV) ){
        return false;
    }
    //fill( monGroup, m_anatype+"_TagCutCounter", "Et");
    

    ATH_MSG_DEBUG("Selecting Tag Electron Eta");
    //fiducial detector acceptance region
    float absEta = fabs(el->caloCluster()->etaBE(2));
    if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
        return false;
    }


    //fill( monGroup, m_anatype+"_TagCutCounter", "Eta");

    ATH_MSG_DEBUG("Checking electron object quality");
    if (!el->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    //fill( monGroup, m_anatype+"_TagCutCounter", "IsGoodOQ");

    
    if(m_tagTrigList.empty())
    {
      ATH_MSG_DEBUG("Found a tag electron"); 
      return true;
    }


    ATH_MSG_DEBUG("Selecting Tag Electron Decision");
    // Check matching to a given trigger
    // The statement below is more general
    bool tagPassed=false;
    for(unsigned int ilist = 0; ilist != m_tagTrigList.size(); ilist++) {
      std::string tag = m_tagTrigList.at(ilist);
      if(tdt()->isPassed(tag)){ 
        if(m_tp){
          std::string p1trigger;
          std::string p2trigger;
          if(splitTriggerName(tag,p1trigger,p2trigger)){
            if(fabs(p1trigger.find("tight"))<14) tag=p1trigger;
            if(fabs(p2trigger.find("tight"))<14) tag=p2trigger;
          }
          if( match()->match(el,tag) )
            tagPassed=true;
        }
        else{
          tagPassed=true; 
        }
      }
    }
    if(!tagPassed) {
        ATH_MSG_DEBUG("Failed tag trigger "); 
        return false;
    }
    
    //fill( monGroup, m_anatype+"_TagCutCounter", "PassTrigger");
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
    //fill( monGroup, m_anatype+"_TagCutCounter", "MatchTrigger");
    ATH_MSG_DEBUG("Found a tag electron");
    return true;
}






bool TrigEgammaNavTPBaseTool::isGoodProbeElectron( ToolHandle<GenericMonitoringTool> /*monGroup*/, 
                                                   const xAOD::Electron *el,
                                                   const xAOD::JetContainer *jets ) const 
{
    //Check constituents
    if(!el->trackParticle()){
        ATH_MSG_DEBUG("No track Particle");
        return false;
    }
    //fill( monGroup, m_anatype+"_ProbeCutCounter", "HasTrack" );
    if(!el->caloCluster()){
        ATH_MSG_DEBUG("No caloCluster");
        return false;
    }

    //fill( monGroup, m_anatype+"_ProbeCutCounter", "HasCluster");
    //fiducial detector acceptance region
    if(m_rmCrack){
        float absEta = fabs(el->caloCluster()->etaBE(2));
        if ((absEta > 1.37 && absEta < 1.52) || absEta > 2.47) {
            return false; 
        }
    }

    //fill( monGroup, m_anatype+"_ProbeCutCounter", "Eta");

    ATH_MSG_DEBUG("Checking electron object quality");
    if (!el->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) return false;
    //fill( monGroup, m_anatype+"_ProbeCutCounter", "IsGoodOQ");
    //fill( monGroup, m_anatype+"_ProbeCutCounter", "GoodPid");

    if(m_applyJetNearProbeSelection){
        TLorentzVector probeCandidate;
        probeCandidate.SetPtEtaPhiE(el->pt(), el->trackParticle()->eta(), el->trackParticle()->phi(), el->e());
        Int_t jetsAroundProbeElectron = 0; 
        for(const auto &i_jet : *jets){
            TLorentzVector jet;
            jet.SetPtEtaPhiE(i_jet->pt(), i_jet->eta(), i_jet->phi(), i_jet->e());
            if( (jet.Et() > 20*Gaudi::Units::GeV) && (jet.DeltaR(probeCandidate) < 0.4)) jetsAroundProbeElectron++;
        }
        //reject if more than 1 jet close to the probe electron
        if ( jetsAroundProbeElectron >= 2 ) {
            //ATH_MSG_DEBUG("too many jets around object");
            return false; 
        }
    }
    //fill( monGroup. m_anatype+"_ProbeCutCounter", "NearbyJet");
    return true; // Good probe electron
}



void TrigEgammaNavTPBaseTool::DressPid(const xAOD::Electron *eg) const
{    
    auto ctx = Gaudi::Hive::currentContext() ;
    for(int ipid=0;ipid<3;ipid++){
        bool accept = (bool) this->m_electronIsEMTool[ipid]->accept(ctx,eg);
        const std::string pidname="is"+m_isemname[ipid];
        eg->auxdecor<bool>(pidname)=static_cast<bool>(accept);
    }
    for(int ipid=0;ipid<2;ipid++){
        bool accept = (bool) this->m_electronLHTool[ipid]->accept(ctx,eg);
        const std::string pidname="is"+m_lhname[ipid];
        eg->auxdecor<bool>(pidname)=static_cast<bool>(accept);
    }
    eg->auxdecor<bool>("Isolated")=isIsolated(eg, m_offProbeIsolation);
}





bool TrigEgammaNavTPBaseTool::ApplyElectronPid( const xAOD::Electron *eg, const std::string pidname) const
{
    auto ctx = Gaudi::Hive::currentContext() ;
    if (pidname == "Tight"){
        return (bool) this->m_electronIsEMTool[0]->accept(ctx,eg);
    }
    else if (pidname == "Medium"){
        return  (bool) this->m_electronIsEMTool[1]->accept(ctx,eg);
    }
    else if (pidname == "Loose"){
        return (bool) this->m_electronIsEMTool[2]->accept(ctx,eg);
    }
    else if (pidname == "LHTight"){
        return (bool) this->m_electronLHTool[0]->accept(ctx,eg);
    }
    else if (pidname == "LHMedium"){
        return (bool) this->m_electronLHTool[1]->accept(ctx,eg);
    }
    else if (pidname == "LHLoose"){
        return (bool) this->m_electronLHTool[2]->accept(ctx,eg);
    }
    else ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return false;
}





float TrigEgammaNavTPBaseTool::GetPseudoLifetime(const xAOD::Electron *el1,const xAOD::Electron *el2) const 
{

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

  float  ptEECalo  = (el1track+el2track).Pt();

  return lxy*3096.916/(0.299792458*ptEECalo);

}

double TrigEgammaNavTPBaseTool::simple_lxy(int flag,   double d1,  double d2, double phi1, double phi2, 
                                                       double pt1, double pt2, double vx, double vy) const 
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


