/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaEmulationTool/TrigEgammaSelectorBaseTool.h"
using namespace Trig;

/*****************************************************************************************/
TrigEgammaSelectorBaseTool::TrigEgammaSelectorBaseTool(const std::string& myname):
  AsgTool(myname),
  m_storeGate(nullptr),
  m_trigdec(nullptr)
{
  // just for compile
  HLT::TriggerElement* t = NULL;
  const xAOD::TrigElectronContainer* a = getFeature<xAOD::TrigElectronContainer>(t);
  const xAOD::ElectronContainer* b = getFeature<xAOD::ElectronContainer>(t);
  bool a1 = ancestorPassed<xAOD::TrigElectronContainer>(t);
  bool b1 = ancestorPassed<xAOD::ElectronContainer>(t);
  (void)a; (void)b;
  (void)a1; (void)b1;
}
//!==========================================================================
StatusCode TrigEgammaSelectorBaseTool::initialize(){
  return StatusCode::SUCCESS;
}
//!==========================================================================
StatusCode TrigEgammaSelectorBaseTool::finalize(){
  return StatusCode::SUCCESS;
}
//!==========================================================================
const xAOD::TrigRingerRings* TrigEgammaSelectorBaseTool::getTrigCaloRings( const xAOD::TrigEMCluster *emCluster){

  const xAOD::TrigRingerRings *ringer = nullptr;
  if(!emCluster)  return nullptr;

  Trig::FeatureContainer fc = (m_trigdec->features("HLT_.*",TrigDefs::alsoDeactivateTEs));
  const std::vector< Trig::Feature<xAOD::TrigRingerRings > > vec_featRinger = fc.get< xAOD::TrigRingerRings >("",TrigDefs::alsoDeactivateTEs);
  ATH_MSG_DEBUG("L2Calo Rings FC Size " << vec_featRinger.size());
  for( Trig::Feature<xAOD::TrigRingerRings > featRinger : vec_featRinger ){
    ringer = featRinger.cptr();
    if(emCluster->RoIword() ==  (getFeature<xAOD::TrigEMCluster>(featRinger.te()))->RoIword() ){
      ATH_MSG_DEBUG("L2Calo Rings matched with TrigEMCluster object.");
      return ringer;
    }
  }
  return nullptr;
}
//!==========================================================================
bool TrigEgammaSelectorBaseTool::getCaloRings( const xAOD::Electron *el, std::vector<float> &ringsE ){
  if(!el) return false;
  ringsE.clear();
 
  /*auto m_ringsELReader = xAOD::getCaloRingsReader();

  // First, check if we can retrieve decoration:
  const xAOD::CaloRingsELVec *caloRingsELVec(nullptr);
  try {
    caloRingsELVec = &(m_ringsELReader->operator()(*el));
  } catch ( const std::exception &e) {
    ATH_MSG_WARNING("Couldn't retrieve CaloRingsELVec. Reason: " << e.what());
  }

  if ( caloRingsELVec->empty() ){
    ATH_MSG_WARNING("Particle does not have CaloRings decoratorion.");
    return false;
  }


  // For now, we are using only the first cluster
  const xAOD::CaloRings *clrings = *(caloRingsELVec->at(0));
  // For now, we are using only the first cluster
 
  if(clrings) clrings->exportRingsTo(ringsE);
  else{
    ATH_MSG_WARNING("There is a problem when try to attack the rings vector using exportRigsTo() method.");
    return false;
  }
  */
  return true;
}
//!==========================================================================
void TrigEgammaSelectorBaseTool::parseTriggerName(const std::string trigger, std::string defaultPid,bool &isL1,std::string &type,
        float &threshold, float &l1threshold, std::string &l1type, std::string &pidname, bool &etcut, bool &perf){
   
    // Analyze L1 or HLT item
    bool result = boost::starts_with( trigger , "L1" );
    if (result) {
        std::string l1info = trigger;
        l1info.erase(0,4);
        l1type = boost::trim_copy_if(l1info, boost::is_digit());
        std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
        l1threshold = atof(l1cut.c_str());
        threshold = l1threshold;
        isL1=true;
        pidname = defaultPid;
        type = "electron"; // for now only electron L1 studies
    }
    else {
        std::string hltinfo=trigger;
        if(boost::contains(hltinfo,"HLT")) hltinfo.erase(0,4);
        std::string l1item = getL1Item(trigger);
        ATH_MSG_DEBUG("Trigger L1item " << trigger << " " << l1item);
        std::vector<std::string> strs;
        boost::split(strs,trigger,boost::is_any_of("_"));
        for (std::vector<std::string>::iterator it = strs.begin(); it != strs.end(); ++it)
        {
            ATH_MSG_DEBUG("Trigger parse "  << *it);
        }
        // Set probe Pid from second part of trigger name
        // Non pid triggers use default Probe which is set as a property


        if(boost::contains(strs.at(0),"e")) type = "electron";
        else if(boost::contains(strs.at(0),"g")) type = "photon";
        else ATH_MSG_ERROR("Cannot set trigger type from name");
        if(boost::contains(strs.at(1),"perf")){
            pidname = "";
            perf=true;
            ATH_MSG_DEBUG("Perf " << perf << " " << pidname );
        }
        else if(boost::contains(strs.at(1),"L2Star")){
            pidname = defaultPid;
            perf=true;
            ATH_MSG_DEBUG("L2Star " << perf << " " << pidname );
        }
        else if(boost::contains(strs.at(1),"hiptrt")){
            pidname = defaultPid;
            perf=true;
            ATH_MSG_DEBUG("hiptrt " << perf << " " << pidname );
        }
        else if( strs.at(1)== "etcut"){
            pidname = defaultPid;
            etcut=true;
        }
        else pidname = getPid(strs.at(1));


        //Get the L1 information
        if(boost::contains(strs.back(),"L1")){
            std::string l1info = strs.back();
            l1info.erase(0,4);
            l1type = boost::trim_copy_if(l1info, boost::is_digit());
            std::string l1cut = boost::trim_copy_if(l1info, !boost::is_digit());
            l1threshold = atof(l1cut.c_str());

            ATH_MSG_DEBUG("L1 item " << l1info << " " << l1threshold << " " << l1type);
        }

        // Get the threshold
        std::string str_thr = strs.at(0);
        str_thr.erase( 0, 1);
        threshold = atof(str_thr.c_str());

        isL1=false;
        ATH_MSG_DEBUG(trigger << " " << type << " " << pidname << " " << threshold);
    }
}
//!==========================================================================
bool TrigEgammaSelectorBaseTool::splitTriggerName(const std::string trigger, std::string &p1trigger, std::string &p2trigger){

  p1trigger="";
  p2trigger="";

  std::vector<std::string> strs;
  boost::split(strs,trigger,boost::is_any_of("_"));

  if((strs.at(0))[0]=='2'){
    ((p1trigger+=((strs.at(0)).substr(1,(int)strs.at(0).find_last_of(strs.at(0)))))+="_")+=strs.at(1);
    p2trigger=p1trigger;
    return true;
  }
  else{

    if(strs.size()<4){
      return false;
    }

    ((p1trigger+=strs.at(0))+="_")+=strs.at(1);
    ((p2trigger+=strs.at(2))+="_")+=strs.at(3);
    return true;
  }
}
//!==========================================================================
std::string TrigEgammaSelectorBaseTool::getPid(const std::string pidtype){
  static std::map<std::string,std::string> PidMap; //no longer class member but static
  if(PidMap.empty()){
      PidMap["vloose"]="Loose";
      PidMap["loose"]="Loose";
      PidMap["medium"]="Medium";
      PidMap["tight"]="Tight";
      PidMap["loose1"]="Loose";
      PidMap["medium1"]="Medium";
      PidMap["tight1"]="Tight";
      PidMap["lhvloose"]="LHLoose";
      PidMap["lhloose"]="LHLoose";
      PidMap["lhmedium"]="LHMedium";
      PidMap["lhtight"]="LHTight";
  }
  return PidMap[pidtype];
}
//!==========================================================================
     
std::string TrigEgammaSelectorBaseTool::getL1Item(std::string trigger){
    if(boost::contains(trigger,"HLT")) trigger.erase(0,4);
    auto trig_conf = m_trigdec->ExperimentalAndExpertMethods()->getChainConfigurationDetails("HLT_"+trigger);
    std::string L1_seed = "";
    if(trig_conf != NULL){
        ATH_MSG_DEBUG("TrigConf available");
        L1_seed = trig_conf->lower_chain_name(); //L1 trigger seed             
    }
    return L1_seed;
}
//!==========================================================================

float TrigEgammaSelectorBaseTool::dR(const float eta1, const float phi1, const float eta2, const float phi2){
    float deta = fabs(eta1 - eta2);
    float dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
                 Pi() - fabs(phi1 - phi2);
    return sqrt(deta*deta + dphi*dphi);
}
//!==========================================================================
void TrigEgammaSelectorBaseTool::dressDecision(const SG::AuxElement *aux, std::string label, bool pass)
{
  aux->auxdecor<bool>("TrigEgammaEmulationTool_"+label) = pass; 
}
//!==========================================================================
