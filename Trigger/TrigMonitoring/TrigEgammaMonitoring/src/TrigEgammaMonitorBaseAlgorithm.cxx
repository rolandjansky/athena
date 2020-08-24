/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorBaseAlgorithm.h"



TrigEgammaMonitorBaseAlgorithm::TrigEgammaMonitorBaseAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingToolMT/TrigEgammaMatchingToolMT")
{
  declareProperty( "MatchTool" , m_matchTool );
}


TrigEgammaMonitorBaseAlgorithm::~TrigEgammaMonitorBaseAlgorithm() {}




StatusCode TrigEgammaMonitorBaseAlgorithm::initialize() 
{
    
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_trigdec.retrieve());
  ATH_CHECK(m_electronIsEMTool.retrieve());
  ATH_CHECK(m_electronLHTool.retrieve());

  m_trigdec->ExperimentalAndExpertMethods()->enable();

  return StatusCode::SUCCESS;
}


// ************************************************************************************************




bool TrigEgammaMonitorBaseAlgorithm::ApplyElectronPid( const xAOD::Electron *eg, const std::string pidname) const
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





// ************************************************************************************************



bool TrigEgammaMonitorBaseAlgorithm::isIsolated(const xAOD::Electron *eg, const std::string isolation) const {
  ATH_MSG_DEBUG("Apply Isolation " << isolation);
  float ptcone20;
  eg->isolationValue(ptcone20, xAOD::Iso::ptcone20);
  ATH_MSG_DEBUG("ptcone20 " << ptcone20);
  if (!(fabs(eg->pt()) > 0)) {
    ATH_MSG_DEBUG("Electron pt is zero, can't calculate relative isolation");
    return false;
  }
  float ptcone20_rel = ptcone20/eg->pt();
  ATH_MSG_DEBUG("Relative isolation value " << ptcone20_rel);
  if (isolation == "Loose"){
    if (ptcone20_rel > 0.1) {
      ATH_MSG_DEBUG("Probe failing isolation");
      return false;
    } else {
      ATH_MSG_DEBUG("Probe passing isolation");
      return true;
    }
  }
  else {
    ATH_MSG_DEBUG("No valid working point defined for " << isolation << " continue without isolation");
  }
  return false;
}



bool TrigEgammaMonitorBaseAlgorithm::isPrescaled(const std::string trigger) const {

    bool efprescale=false;
    bool l1prescale=false;
    bool prescale=false;
    bool rerun=true; //assume rerun for l1
    std::string l1item="";

    if(boost::starts_with(trigger, "L1" ))
        l1item=trigger;
    if(boost::starts_with(trigger,"HLT")){
        l1item = getL1Item(trigger);
        const unsigned int bit=tdt()->isPassedBits(trigger);
        efprescale=bit & TrigDefs::EF_prescaled;
        rerun=bit&TrigDefs::EF_resurrected; //Rerun, only check for HLT
    }


    ATH_MSG_DEBUG("Checking prescale for " << trigger << " " << l1item);
    const unsigned int l1bit=tdt()->isPassedBits(l1item);
    bool l1_afterpre=l1bit&TrigDefs::L1_isPassedAfterPrescale;
    bool l1_beforepre=l1bit&TrigDefs::L1_isPassedBeforePrescale;
    l1prescale=l1_beforepre && !l1_afterpre;
    prescale=efprescale || l1prescale;
    ATH_MSG_DEBUG("L1 prescale " << l1item << " " << l1prescale << " before " << l1_beforepre << " after " << l1_afterpre);
    ATH_MSG_DEBUG("EF prescale " << trigger << " " << efprescale << " Prescale " << prescale);
    if(rerun) return false; // Rerun use the event
    if(prescale) return true; // Prescaled, reject event
    return false; // Not prescaled, use event
}




asg::AcceptData TrigEgammaMonitorBaseAlgorithm::setAccept( const TrigCompositeUtils::Decision *dec, const TrigInfo info) const {
    
    ATH_MSG_DEBUG("setAccept");


    asg::AcceptInfo accept;
    for(const auto& cut:m_trigLevel) accept.addCut(cut,cut);
    asg::AcceptData acceptData (&accept);

   
    bool passedL1Calo=false;
    bool passedL2Calo=false;
    bool passedEFCalo=false;
    bool passedL2=false;
    bool passedEFTrk=false;
    bool passedEF=false;

 
    
    auto trigger = info.trigName;
   
    passedL1Calo = match()->ancestorPassed<TrigRoiDescriptorCollection>( dec , trigger , "initialRois");

    if(!info.trigL1){ // HLT item get full decision
        ATH_MSG_DEBUG("Check for active features: TrigEMCluster,CaloClusterContainer");

        passedL2Calo = match()->ancestorPassed<xAOD::TrigEMClusterContainer>(dec, trigger, "HLT_FastCaloEMClusters");  
        passedEFCalo = match()->ancestorPassed<xAOD::CaloClusterContainer>(dec, trigger, "HLT_CaloEMClusters");


        if(info.trigType == "electron"){
            ATH_MSG_DEBUG("Check for active features: TrigElectron, ElectronContainer, TrackParticleContainer");
            passedL2    = match()->ancestorPassed<xAOD::TrigElectronContainer>(dec, trigger, "HLT_FastElectrons");
            if (info.trigEtcut){
              passedEF=true;
            }else{
              passedEF    = match()->ancestorPassed<xAOD::ElectronContainer>(dec, trigger, "HLT_egamma_Electrons");
            }
            passedEFTrk = true; //match()->ancestorPassed<xAOD::TrackParticleContainer>(dec);
        }
        else if(info.trigType == "photon"){
            ATH_MSG_DEBUG("Check for active features: TrigPhoton, PhotonContainer");
            passedL2 = match()->ancestorPassed<xAOD::TrigPhotonContainer>(dec, trigger, "HLT_FastPhotons");
            if (info.trigEtcut){
              passedEF=true;
            }else{
              passedEF = match()->ancestorPassed<xAOD::PhotonContainer>(dec, trigger, "HLT_egamma_Photons");
            }
            passedEFTrk=true;// Assume true for photons
        }
    }
    
    acceptData.setCutResult("L1Calo",passedL1Calo);
    acceptData.setCutResult("L2Calo",passedL2Calo);
    acceptData.setCutResult("L2",passedL2);
    acceptData.setCutResult("EFCalo",passedEFCalo);
    acceptData.setCutResult("EFTrack",passedEFTrk);
    acceptData.setCutResult("HLT",passedEF);
    ATH_MSG_DEBUG("Accept results:");
    ATH_MSG_DEBUG("L1: "<< passedL1Calo);
    ATH_MSG_DEBUG("L2Calo: " << passedL2Calo);
    ATH_MSG_DEBUG("L2: "<< passedL2);
    ATH_MSG_DEBUG("EFCalo: "<< passedEFCalo);
    ATH_MSG_DEBUG("HLT: "<<passedEF);

    return acceptData;
}




float TrigEgammaMonitorBaseAlgorithm::dR(const float eta1, const float phi1, const float eta2, const float phi2) const {
    float deta = fabs(eta1 - eta2);
    float dphi = fabs(phi1 - phi2) < TMath::Pi() ? fabs(phi1 - phi2) : 2*TMath:: \
                 Pi() - fabs(phi1 - phi2);
    return sqrt(deta*deta + dphi*dphi);
}




float TrigEgammaMonitorBaseAlgorithm::getEta2(const xAOD::Egamma* eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        return fabs(cluster->etaBE(2));
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::getEt(const xAOD::Electron* eg) const{
    if(eg && (eg->caloCluster()) && (eg->trackParticle())){
        const xAOD::TrackParticle *trk=eg->trackParticle();
        const xAOD::CaloCluster *clus=eg->caloCluster();
        float eta   = fabs(trk->eta());
        return clus->e()/cosh(eta);
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::getEtCluster37(const xAOD::Egamma* eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        float eta2   = fabs(cluster->etaBE(2));
        return cluster->e()/cosh(eta2);
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::getDEmaxs1(const xAOD::Egamma *eg) const{
    if(eg){
        float emax2=0.;
        eg->showerShapeValue(emax2, xAOD::EgammaParameters::e2tsts1);
        float emax=0.;
        eg->showerShapeValue(emax, xAOD::EgammaParameters::emaxs1);
        float val = fabs(emax+emax2)>0. ? (emax-emax2)/(emax+emax2) : 0.;
        return val;
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::rTRT  (const xAOD::Electron* eg) const{
    if(eg && eg->trackParticle()){
        uint8_t trtHits   = 0;
        eg->trackParticleSummaryValue(trtHits,xAOD::numberOfTRTHits);
        uint8_t trtHTHits = 0;
        eg->trackParticleSummaryValue(trtHTHits,xAOD::numberOfTRTHighThresholdHits);
        if(trtHits!=0) {
            return ( (double)trtHTHits / (double)trtHits );
        }
        else return -99.;
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::getSigmaD0(const xAOD::Electron *eg) const{
    const xAOD::TrackParticle* t = eg->trackParticle();
    float d0sigma=0.;
    if (t)
    {
        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
            d0sigma=sqrtf(vard0);
        }
        else return -99.;
        return d0sigma;
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::getD0sig(const xAOD::Electron *eg) const{
    const xAOD::TrackParticle* t = eg->trackParticle();
    float d0sigma=0.;
    if (t)
    {

        float vard0 = t->definingParametersCovMatrix()(0,0);
        if (vard0 > 0) {
            d0sigma=sqrtf(vard0);
        }
        else return -99.;
        return t->d0()/d0sigma;
    }
    else return -99.;
}


float TrigEgammaMonitorBaseAlgorithm::getEnergyBE0(const xAOD::Egamma *eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        return cluster->energyBE(0);
    }
    else return 0;
}


float TrigEgammaMonitorBaseAlgorithm::getEnergyBE1(const xAOD::Egamma *eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        return cluster->energyBE(1);
    }
    else return 0.;
}


float TrigEgammaMonitorBaseAlgorithm::getEnergyBE2(const xAOD::Egamma *eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        return cluster->energyBE(2);
    }
    else return 0.;
}


float TrigEgammaMonitorBaseAlgorithm::getEnergyBE3(const xAOD::Egamma *eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        return cluster->energyBE(3);
    }
    else return 0.;
}


float TrigEgammaMonitorBaseAlgorithm::getEaccordion(const xAOD::Egamma *eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        float ebe1 = cluster->energyBE(1);
        float ebe2 = cluster->energyBE(2);
        float ebe3 = cluster->energyBE(3);
        return (ebe1+ebe2+ebe3);
    }
    else return 0.;
}


float TrigEgammaMonitorBaseAlgorithm::getE0Eaccordion(const xAOD::Egamma *eg) const{
    if(eg && (eg->caloCluster())){
        const xAOD::CaloCluster*   cluster  = eg->caloCluster();
        float ebe0 = cluster->energyBE(0);
        float ebe1 = cluster->energyBE(1);
        float ebe2 = cluster->energyBE(2);
        float ebe3 = cluster->energyBE(3);
        float eacc = ebe1+ebe2+ebe3;
        if(eacc==0.) return 0.;
        return (ebe0/eacc);
    }
    else return 0.;
}






/*! Macros for plotting */
#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getShowerShape_##_name_(const xAOD::Egamma* eg) const \
{ float val{-99}; \
    eg->showerShapeValue(val,xAOD::EgammaParameters::_name_); \
    return val; }
    GETTER(e011)
    GETTER(e132)
    GETTER(e237)
    GETTER(e277)
    GETTER(ethad)
    GETTER(ethad1)
    GETTER(weta1)
    GETTER(weta2)
    GETTER(f1)
    GETTER(e2tsts1)
    GETTER(emins1)
    GETTER(emaxs1)
    GETTER(wtots1)
    GETTER(fracs1)
    GETTER(Reta)
    GETTER(Rphi)
    GETTER(f3)
    GETTER(f3core)
    GETTER(Eratio)
    GETTER(Rhad)
    GETTER(Rhad1)
    GETTER(DeltaE)
#undef GETTER


// GETTER for Isolation monitoring
#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getIsolation_##_name_(const xAOD::Electron* eg) const\
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; }
    GETTER(ptcone20)
    GETTER(ptcone30)
    GETTER(ptcone40)
    GETTER(ptvarcone20)
    GETTER(ptvarcone30)
    GETTER(ptvarcone40)
#undef GETTER


#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getIsolation_##_name_(const xAOD::Egamma* eg) const\
{ float val{-99}; \
    eg->isolationValue(val,xAOD::Iso::_name_); \
    return val; }
    GETTER(etcone20)
    GETTER(etcone30)
    GETTER(etcone40)
    GETTER(topoetcone20)
    GETTER(topoetcone30)
    GETTER(topoetcone40)
#undef GETTER


// GETTERs for CaloCluster monitoring
#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getCluster_##_name_(const xAOD::Egamma* eg) const\
{    if(eg && eg->caloCluster()) \
    return eg->caloCluster()->_name_(); \
    else return -99.;}
    GETTER(et)
    GETTER(phi)
    GETTER(eta)
#undef GETTER


// GETTERs for Track monitoring
#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getTrack_##_name_(const xAOD::Electron* eg) const\
{    if(eg && eg->trackParticle()) \
    return eg->trackParticle()->_name_(); \
    else return -99.;}
    GETTER(pt)
    GETTER(phi)
    GETTER(eta)
    GETTER(d0)
    GETTER(z0)
#undef GETTER
    
  
// GETTERs for Track details monitoring
#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getTrackSummary_##_name_(const xAOD::Electron* eg) const \
{ uint8_t val_uint8{0}; \
    if(eg){ \
        eg->trackParticleSummaryValue(val_uint8,xAOD::_name_); \
        return val_uint8; } \
    else return -99; }
    GETTER(numberOfInnermostPixelLayerHits)
    GETTER(numberOfInnermostPixelLayerOutliers)
    GETTER(numberOfPixelHits)
    GETTER(numberOfPixelOutliers)
    GETTER(numberOfSCTHits)
    GETTER(numberOfSCTOutliers)
    GETTER(numberOfTRTHits)
    GETTER(numberOfTRTHighThresholdHits)
    GETTER(numberOfTRTHighThresholdOutliers)
    GETTER(numberOfTRTOutliers)
    GETTER(expectInnermostPixelLayerHit)
    GETTER(numberOfPixelDeadSensors)
    GETTER(numberOfSCTDeadSensors)
    GETTER(numberOfTRTXenonHits)
#undef GETTER


#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getTrackSummaryFloat_##_name_(const xAOD::Electron* eg) const\
{ float val_float{0}; \
    if(eg){ \
        eg->trackParticleSummaryValue(val_float,xAOD::_name_); \
        return val_float; } \
    else return -99; }
    GETTER(eProbabilityComb)
    GETTER(eProbabilityHT)
    GETTER(pixeldEdx)
#undef GETTER


// GETTERs for Calo-Track monitoring
#define GETTER(_name_) float TrigEgammaMonitorBaseAlgorithm::getCaloTrackMatch_##_name_(const xAOD::Electron* eg) const\
{ float val={-99.}; \
    if(eg){ \
        eg->trackCaloMatchValue(val,xAOD::EgammaParameters::_name_);} \
    return val; }
    GETTER(deltaEta0)
    GETTER(deltaPhi0)
    GETTER(deltaPhiRescaled0)
    GETTER(deltaEta1)
    GETTER(deltaPhi1)
    GETTER(deltaPhiRescaled1)
    GETTER(deltaEta2)
    GETTER(deltaPhi2)
    GETTER(deltaPhiRescaled2)
    GETTER(deltaEta3)
    GETTER(deltaPhi3)
    GETTER(deltaPhiRescaled3)
#undef GETTER



TrigInfo TrigEgammaMonitorBaseAlgorithm::getTrigInfo(const std::string trigger) const{ 
  return m_trigInfo.at(trigger); 
}



// This is not const function and can not be used in execute mode (not thread safe)
// adds entry in TrigInfo map to retrieve later via trigger name
void TrigEgammaMonitorBaseAlgorithm::setTrigInfo(const std::string trigger){

    /********************************************
      std::string trigName; //Trigger Name
      std::string trigType; //Electron or Photon
      std::string trigL1Item; //L1 item for HLT
      std::string trigL1Type; //VHI
      std::string trigPidType; //Loose, Medium, Tight, etc...
      bool trigL1; // Level1 Trigger
      bool trigPerf; // Performance chain
      bool trigEtcut; // Et cut only chain
      float trigThrHLT; // HLT Et threshold
      float trigThrL1; // L1 Et threshold
     *******************************************/

    std::string type="";
    bool isL1=false;
    float etthr=0;
    float l1thr=0;
    std::string l1type="";
    std::string pidname="";
    bool perf=false;
    bool etcut=false;
    parseTriggerName(trigger,m_defaultProbePid,isL1,type,etthr,l1thr,l1type,pidname,etcut,perf); // Determines probe PID from trigger

    std::string l1item = "";
    if(isL1) l1item=trigger;
    else getL1Item(trigger);
    std::string decorator="is"+pidname;

    if(isL1) etthr=l1thr; // Should be handled elsewhere
    TrigInfo info{trigger,type,l1item,l1type,pidname,decorator,isL1,perf,etcut,etthr,l1thr,false};
    m_trigInfo[trigger] = info;
}







bool TrigEgammaMonitorBaseAlgorithm::splitTriggerName(const std::string trigger, 
                                                  std::string &p1trigger, 
                                                  std::string &p2trigger) const {

  p1trigger="";
  p2trigger="";

  std::string hltinfo=trigger;
  if(boost::contains(hltinfo,"HLT")) hltinfo.erase(0,4);
  std::vector<std::string> strs;
  boost::split(strs,hltinfo,boost::is_any_of("_"));

  if((strs.at(0))[0]=='2'){
    ((p1trigger+=("HLT_"+((strs.at(0)).substr(1,(int)strs.at(0).find_last_of(strs.at(0)))))+="_"));

    for(unsigned int i=1; i<strs.size();i++){
      if(strs.at(i)=="Jpsiee") continue;
      (p1trigger+="_")+=strs.at(i);
    }

    p2trigger=p1trigger;
    return true;
  }
  else{

    if(strs.size()<4){
      return false;
    }

    int index=-1;
    p1trigger+=("HLT_"+strs.at(0));

    for(int i=1; index<0;i++)
      {
        (p1trigger+="_")+=strs.at(i);

        if(strs.at(i+1)[0]=='e' || strs.at(i+1)[0]=='g') index=(i+1);
      }

    p2trigger+=("HLT_"+strs.at(index));

    for(unsigned int i=index+1; i< strs.size();i++){
      if(strs.at(i)=="Jpsiee") continue;
      (p2trigger+="_")+=strs.at(i);
    }
    return true;
  }


}



void TrigEgammaMonitorBaseAlgorithm::parseTriggerName(const std::string trigger, 
                                                  std::string defaultPid,
                                                  bool &isL1,
                                                  std::string &type,
                                                  float &threshold, 
                                                  float &l1threshold, 
                                                  std::string &l1type, 
                                                  std::string &pidname, 
                                                  bool &etcut, 
                                                  bool &perf) const {

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
        ATH_MSG_DEBUG("Trigger L1item " << trigger << " " << l1item << " " << hltinfo);
        std::vector<std::string> strs;
        boost::split(strs,hltinfo,boost::is_any_of("_"));
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
            pidname = defaultPid;
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




std::string TrigEgammaMonitorBaseAlgorithm::getProbePid(const std::string pidtype) const {
    // Note vloose/lhvloose trigger mapped to Loose/LHLoose offline PID
    return m_pidMap.at(pidtype);
}



std::string TrigEgammaMonitorBaseAlgorithm::getL1Item(std::string trigger) const{
    auto trig_conf = m_trigdec->ExperimentalAndExpertMethods()->getChainConfigurationDetails(trigger);
    std::string L1_seed = "";
    if(trig_conf != nullptr){
        ATH_MSG_DEBUG("TrigConf available");
        L1_seed = trig_conf->lower_chain_name(); //L1 trigger seed
    }
    return L1_seed;
}





const std::vector<std::string> TrigEgammaMonitorBaseAlgorithm::m_trigLevel = {"L1Calo","L2Calo","L2","EFCalo","EFTrack","HLT"};

const std::map<std::string,std::string> TrigEgammaMonitorBaseAlgorithm::m_trigLvlMap = {{"L1Calo","Trigger L1Calo step"},
                                                                                        {"L2Calo","Trigger L2Calo step"},
                                                                                        {"L2","Trigger L2 step"},
                                                                                        {"EFCalo","Trigger EFCalo step"},
                                                                                        {"EFTrack","Trigger EFTrack step"},
                                                                                        {"HLT","Trigger HLT accept"}};

const std::map<std::string, std::string> TrigEgammaMonitorBaseAlgorithm::m_pidMap = { {"vloose"   , "Loose"   },
                                                                                      {"loose"    , "Loose"   },
                                                                                      {"medium"   , "Medium"  },
                                                                                      {"tight"    , "Tight"   },
                                                                                      {"loose1"   , "Loose"   },
                                                                                      {"medium1"  , "Medium"  },
                                                                                      {"tight1"   , "Tight"   },
                                                                                      {"lhvloose" , "LHLoose" },
                                                                                      {"lhloose"  , "LHLoose" },
                                                                                      {"lhmedium" , "LHMedium"},
                                                                                      {"lhtight"  , "LHTight" } };



