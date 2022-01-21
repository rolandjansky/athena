/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaMonitorBaseAlgorithm.h"



TrigEgammaMonitorBaseAlgorithm::TrigEgammaMonitorBaseAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name,pSvcLocator),
    m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_matchTool("Trig::TrigEgammaMatchingToolMT/TrigEgammaMatchingToolMT")
{
  declareProperty( "MatchTool" , m_matchTool );
  declareProperty( "EmulationTool" , m_emulatorTool );
}


TrigEgammaMonitorBaseAlgorithm::~TrigEgammaMonitorBaseAlgorithm() {}




StatusCode TrigEgammaMonitorBaseAlgorithm::initialize() 
{

  ATH_MSG_INFO("TrigEgammaMonitorBaseAlgorithm::initialize()...");
  ATH_CHECK(AthMonitorAlgorithm::initialize());
  ATH_CHECK(m_trigdec.retrieve());
  ATH_CHECK(m_photonIsEMTool.retrieve());
  ATH_CHECK(m_electronIsEMTool.retrieve());
  ATH_CHECK(m_electronLHTool.retrieve());
  ATH_CHECK(m_electronDNNTool.retrieve());

  std::vector<std::string> steps = {"L1Calo","L2Calo","L2","EFCalo","EFTrack","HLT"};
  for(const auto& step:steps)
    m_accept.addCut(step,step);



  return StatusCode::SUCCESS;
}


// ************************************************************************************************




bool TrigEgammaMonitorBaseAlgorithm::ApplyElectronPid( const xAOD::Electron *eg, const std::string& pidname) const
{
    const auto& ctx = Gaudi::Hive::currentContext() ;
    if (pidname == "tight"){
        return (bool) this->m_electronIsEMTool[0]->accept(ctx,eg);
    }
    else if (pidname == "medium"){
        return  (bool) this->m_electronIsEMTool[1]->accept(ctx,eg);
    }
    else if (pidname == "loose"){
        return (bool) this->m_electronIsEMTool[2]->accept(ctx,eg);
    }
    else if (pidname == "lhtight"){
        return (bool) this->m_electronLHTool[0]->accept(ctx,eg);
    }
    else if (pidname == "lhmedium"){
        return (bool) this->m_electronLHTool[1]->accept(ctx,eg);
    }
    else if (pidname == "lhloose"){
        return (bool) this->m_electronLHTool[2]->accept(ctx,eg);
    }
    else if (pidname == "lhvloose"){
        return (bool) this->m_electronLHTool[3]->accept(ctx,eg);
    }
    else if (pidname == "dnntight"){
        return (bool) this->m_electronDNNTool[0]->accept(ctx,eg);
    }
    else if (pidname == "dnnmedium"){
        return (bool) this->m_electronDNNTool[1]->accept(ctx,eg);
    }
    else if (pidname == "dnnloose"){
        return (bool) this->m_electronDNNTool[2]->accept(ctx,eg);
    }
    else ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return false;
}



// ************************************************************************************************


bool TrigEgammaMonitorBaseAlgorithm::ApplyPhotonPid( const xAOD::Photon *eg, const std::string& pidname) const
{
    const auto& ctx = Gaudi::Hive::currentContext() ;
    if (pidname == "tight"){
        return (bool) this->m_photonIsEMTool[0]->accept(ctx,eg);
    }
    else if (pidname == "medium"){
        return  (bool) this->m_photonIsEMTool[1]->accept(ctx,eg);
    }
    else if (pidname == "loose"){
        return (bool) this->m_photonIsEMTool[2]->accept(ctx,eg);
    }
    else ATH_MSG_DEBUG("No Pid tool, continue without PID");
    return false;
}


// ************************************************************************************************



bool TrigEgammaMonitorBaseAlgorithm::isIsolated(const xAOD::Electron *eg, const std::string& isolation) const {
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
  if (isolation == "loose"){
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



bool TrigEgammaMonitorBaseAlgorithm::isPrescaled(const std::string& trigger) const {

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



asg::AcceptData TrigEgammaMonitorBaseAlgorithm::setAccept( const TrigCompositeUtils::Decision *dec, const TrigInfo& info) const {
    
    ATH_MSG_DEBUG("setAccept");

    unsigned int condition=TrigDefs::includeFailedDecisions;

    asg::AcceptData acceptData (&m_accept);
   
    bool passedL1Calo=false;
    bool passedL2Calo=false;
    bool passedEFCalo=false;
    bool passedL2=false;
    bool passedEFTrk=false;
    bool passedEF=false;
    
    if (dec) {

        auto trigger = info.trigger; 
        // Step 1
        passedL1Calo = match()->ancestorPassed<TrigRoiDescriptorCollection>( dec , trigger , "initialRois", condition);

        if( passedL1Calo ){ // HLT item get full decision
            // Step 2
            passedL2Calo = match()->ancestorPassed<xAOD::TrigEMClusterContainer>(dec, trigger, match()->key("FastCalo"), condition);  
          
            if(passedL2Calo){

                // Step 3
                if(info.signature == "Electron"){
                    std::string key = match()->key("FastElectrons");
                    if(info.lrt)  key = match()->key("FastElectrons_LRT");
                    passedL2 = match()->ancestorPassed<xAOD::TrigElectronContainer>(dec, trigger, key, condition);
                }else if(info.signature == "Photon"){
                    passedL2 = match()->ancestorPassed<xAOD::TrigPhotonContainer>(dec, trigger, match()->key("FastPhotons"), condition);
                }

                if(passedL2){

                    // Step 4
                    std::string key = match()->key("PrecisionCalo_Electron");
                    if(info.signature == "Photon") key = match()->key("PrecisionCalo_Photon");
                    if(info.lrt) key = match()->key("PrecisionCalo_LRT");

                    passedEFCalo = match()->ancestorPassed<xAOD::CaloClusterContainer>(dec, trigger, key, condition);

                    if(passedEFCalo){

                        // Step 5
                        passedEFTrk=true;// Assume true for photons

                        // Step 6
                        if(info.signature == "Electron"){
                            if( info.etcut || info.idperf){// etcut or idperf
                                passedEF = true; // since we dont run the preciseElectron step
                            }else{
                                std::string key = match()->key("Electrons");
                                if(info.lrt)  key = match()->key("Electrons_LRT");
                                if(info.gsf)  key = match()->key("Electrons_GSF");
                                passedEF = match()->ancestorPassed<xAOD::ElectronContainer>(dec, trigger, key, condition);
                            }
   
                        }else if(info.signature == "Photon"){
                            if (info.etcut){
                                passedEF = true; // since we dont run the precisePhoton step
                            }else{
                                passedEF = match()->ancestorPassed<xAOD::PhotonContainer>(dec, trigger, match()->key("Photons"), condition);
                            }
                        }
                    } // EFCalo
                }// L2
            }// L2Calo
        }// L2Calo

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



TrigInfo TrigEgammaMonitorBaseAlgorithm::getTrigInfo(const std::string& trigger) const{ 
  return m_trigInfo.at(trigger); 
}



// This is not const function and can not be used in execute mode (not thread safe)
// adds entry in TrigInfo map to retrieve later via trigger name
void TrigEgammaMonitorBaseAlgorithm::setTrigInfo(const std::string& trigger){

    /********************************************
    // Trigger Information struct
    typedef struct _triginfo
    {
        // L1 information
        bool L1Legacy; 
        std::string L1Threshold; //EM22VHI
        // HLT information
        std::string trigger; //Trigger Name
        std::string signature; //Electron or Photon
        float etthr; // HLT Et threshold
        // if trigger is etcut OR idperf, pidname should be default (usually lhloose)
        std::string pidname; // Offline loose, medium, tight, etc...
        // extra HLT information
        bool idperf; // Performance chain
        bool etcut; // Et cut only chain
        bool gsf; // GSF chain
        bool lrt; // LRT chain
        std::string isolation;
        bool isolated;
    } TrigInfo;
     *******************************************/

    std::map<std::string, std::string> pidMap   = { {"vloose"   , "loose"    },
                                                    {"loose"    , "loose"    },
                                                    {"medium"   , "medium"   },
                                                    {"tight"    , "tight"    },
                                                    {"loose1"   , "loose"    },
                                                    {"medium1"  , "medium"   },
                                                    {"tight1"   , "tight"    },
                                                    {"lhvloose" , "lhvloose" },
                                                    {"lhloose"  , "lhloose"  },
                                                    {"lhmedium" , "lhmedium" },
                                                    {"lhtight"  , "lhtight"  },
                                                    {"dnnloose" , "dnnloose" },
                                                    {"dnnmedium", "dnnmedium"},
                                                    {"dnntight" , "dnntight" } };

    std::vector<std::string> isoNames = {"ivarloose","ivarmedium","ivartight","icaloloose","icalomedium","icalotight"};

    bool gsf = false;
    bool lrt = false;
    bool etcut = false;
    bool idperf = false;
    bool isolated = false;
    std::string pidname = m_defaultProbePid;
    std::string isolation="";
    bool l1legacy=true;

    std::string hltinfo=trigger;
    std::string signature = "";
    float threshold = 0;
    // HLT_e/gXX_(pidname/etcut/idperf)_*_L1EMXX to e/gXX_(pidname/etcut/idperf)_*_L1EMXX
    if(boost::contains(hltinfo,"HLT")) hltinfo.erase(0,4);
    

    std::vector<std::string> parts;
    boost::split(parts,hltinfo,boost::is_any_of("_"));

    // e/gXX_(pidname/etcut/idperf)_*_L1EMXX
    if(boost::contains(parts.at(0),"e")) signature = "Electron";
    else if(boost::contains(parts.at(0),"g")) signature = "Photon";
    else ATH_MSG_ERROR("Cannot set trigger type from name");

    ATH_MSG_INFO(parts.at(1));
    if(parts.at(1) == "idperf"){
        ATH_MSG_INFO("This is idperf");
        idperf=true;
    }
    else if( parts.at(1)== "etcut"){
        ATH_MSG_INFO("This is etcut");
        etcut=true;
    }
    else { // remap online pidname to offline pidname
        ATH_MSG_INFO("This is nominal");
        pidname = pidMap.at(parts.at(1));
    }


    // extra information
    gsf = boost::contains(trigger,"gsf");
    lrt = boost::contains(trigger,"lrt");

    for(auto& iso : isoNames){
        if(boost::contains(trigger, iso)){
            isolation=iso; isolated=true; break;
        }
    }

    // Get the threshold
    std::string str_thr = parts.at(0);
    str_thr.erase( 0, 1);
    threshold = atof(str_thr.c_str());

    // L1EMXX
    std::string l1seed = getL1Item(trigger);
    l1legacy = !boost::contains(l1seed, "eEM");


    ATH_MSG_INFO("=================== Chain Parser =======================");
    ATH_MSG_INFO( "trigger     : " << trigger );
    ATH_MSG_INFO( "threshould  : " << threshold);
    ATH_MSG_INFO( "Pidname     : " << pidname );
    ATH_MSG_INFO( "signature   : " << signature);
    ATH_MSG_INFO( "etcut       : " << (etcut?"Yes":"No"));
    ATH_MSG_INFO( "idperf      : " << (idperf?"Yes":"No"));
    ATH_MSG_INFO( "gsf         : " << (gsf?"Yes":"No"));
    ATH_MSG_INFO( "lrt         : " << (lrt?"Yes":"No"));
    ATH_MSG_INFO( "Isolation   : " << isolation);
    ATH_MSG_INFO( "Isolated    : " << (isolated?"Yes":"No"));
    ATH_MSG_INFO( "L1Seed      : " << l1seed << " (Is Legacy? " << (l1legacy?"Yes":"No") << ")");
    ATH_MSG_INFO("========================================================");

    TrigInfo info{false,l1seed,trigger,signature,threshold,pidname,idperf,etcut,gsf,lrt,isolation, isolated};
    m_trigInfo[trigger] = info;

}


// For Run-3, all triggers must have the L1 seed in name (last part)
std::string TrigEgammaMonitorBaseAlgorithm::getL1Item(const std::string& trigger) const{
    std::vector<std::string> parts;
    boost::split(parts,trigger,boost::is_any_of("_"));
    // L1EMXX
    std::string l1seed = parts.back();
    return l1seed;
}


bool TrigEgammaMonitorBaseAlgorithm::isHLTTruncated() const {
    return m_trigdec->ExperimentalAndExpertMethods().isHLTTruncated();
}




//!=============================================================================

// Define the parser
#include "GaudiKernel/ParsersFactory.h"

namespace Gaudi
{
  namespace Parsers
  {
    // Parse function... nothing special, but it must be done explicitely.
    StatusCode parse( VecDict_t& result, const std::string& input ) { return parse_( result, input ); }
  }
}

// We also need to be able to print an object of our type as a string that both
// Python and our parser can understand,
#include "GaudiKernel/ToStream.h"
namespace std
{
  // This is an example valid for any mapping type.
  ostream& operator<<( ostream& s, const Gaudi::Parsers::VecDict_t& vecDict )
  {
    s << '{';
    for ( const auto& dict : vecDict ) {
      Gaudi::Utils::toStream( dict, s );
    }
    s << '}';
    return s;
  }
}

