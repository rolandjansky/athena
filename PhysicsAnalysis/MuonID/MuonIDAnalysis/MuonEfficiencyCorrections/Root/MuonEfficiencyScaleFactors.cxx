/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonEfficiencyScaleFactors.cxx
 *
 *  Created on: Apr 9, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"
#include <PileupReweighting/PileupReweightingTool.h>

namespace CP {

static std::string CURRENT_MEC_VERSION = "MuonEfficiencyCorrections-03-03-08";

MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors( const std::string& name):
                                            asg::AsgTool( name ),
                                            m_sf_sets(),
                                            m_current_sf(0),
                                            m_lumi_weights_central(),
                                            m_lumi_weights_calo(),
                                            m_lumi_weights_forward(),
                                            m_lumi_weights_lowpt(),
                                            m_lumi_weights_lowptcalo(),
                                            classname(name.c_str()),
                                            m_lumi_handler(),
                                            m_audit_processed(),
                                            m_version_string(CURRENT_MEC_VERSION),
                                            m_sys_string(""),
                                            m_audit_last_evt(-1),
                                            m_effType(""),m_effDec(0),m_sfDec(0),m_sfrDec(0),
                                            m_prwtool(""){

    declareProperty( "WorkingPoint", m_wp = "Medium" );
    declareProperty( "doAudit", m_doAudit = false);

    // these are for debugging / testing, *not* for general use!
    declareProperty( "CustomInputFolder", m_custom_dir = "");
    declareProperty( "CustomFileCaloTag", m_custom_file_Calo = "");
    declareProperty( "CustomFileCombined", m_custom_file_Combined = "");
    declareProperty( "CustomFileHighEta", m_custom_file_HighEta = "");
    declareProperty( "CustomFileLowPt", m_custom_file_LowPt = "");
    declareProperty( "CustomFileLowPtCalo", m_custom_file_LowPtCalo = "");

    declareProperty( "EfficiencyDecorationName", m_efficiency_decoration_name = "");
    declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name = "");
    declareProperty( "ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name = "");
    declareProperty( "CalibrationRelease", m_calibration_version = "160725_ICHEP");
    declareProperty( "EfficiencyType", m_effType = "");
    declareProperty( "LowPtThreshold", m_lowpt_threshold = 15000.);

    declareProperty( "PileupReweightingTool", m_prwtool);
}

MuonEfficiencyScaleFactors::~MuonEfficiencyScaleFactors(){

    // remember to clean up afterwards...
    for (boost::unordered_map<SystematicSet,EffiCollection*>::iterator it = m_sf_sets.begin(); it != m_sf_sets.end();++it){
        if (it->second) delete it->second;
    }
    if(m_effDec) delete m_effDec;
    if(m_sfDec) delete m_sfDec;
    if(m_sfrDec) delete m_sfrDec;
}

StatusCode MuonEfficiencyScaleFactors::initialize(){
    ATH_MSG_INFO("Trying to initialize, with working point "<<m_wp<<", using calibration release "<<m_calibration_version);

    if (!m_prwtool.retrieve().isSuccess()) {
        ATH_MSG_FATAL("MuonEfficiencyScaleFactors requires an initialized instance of the PileupReweighting tool! Please pass such to the 'PileupReweightingTool' property of this tool.");
        return StatusCode::FAILURE;
    }

      /// so the decoration will work with other Efficiency types

    if(m_effType==""){
    	if (m_wp.find("Iso") != std::string::npos){
    		m_effType = "ISO";
    	}
    	else if (m_wp.find("TTVA") != std::string::npos){
    	    m_effType = "TTVA";
    	}
    	else {
    		m_effType = "EFF";
    	}
    }
    ATH_MSG_INFO( "Efficiency type is = "<< m_effType);
    
    /// for isolation efficiencies, we don't use a low pt component for now - set the low pt threshold to -1
    /// same holds for TTVA SF, and for the HighPt WP
    if (m_effType == "ISO" || m_effType == "TTVA" || (m_effType == "EFF" && m_wp.find("HighPt") != std::string::npos)){
        ATH_MSG_DEBUG("We are running Isolation or TTVA or High Pt reco SF, so we use Zmumu based SF for the whole pt range!");
        m_lowpt_threshold = -1;
    }
    else if (m_lowpt_threshold < 0){
        ATH_MSG_INFO("Low pt SF turned off as crossover threshold is negative! Using Zmumu based SF for all pt values.");
    }
    else {
        ATH_MSG_INFO("JPsi based low pt SF will start to rock below "<<m_lowpt_threshold/1000.<<" GeV!");
    }
    
    if(m_efficiency_decoration_name == "") m_efficiency_decoration_name = (m_effType=="EFF")?"Efficiency":m_effType+"Efficiency";
    ATH_MSG_INFO( "Efficiency decoration name is "<< m_efficiency_decoration_name);
    if(m_effDec) delete m_effDec;
    m_effDec = new SG::AuxElement::Decorator< float>(m_efficiency_decoration_name);
 
    if(m_sf_decoration_name == "") m_sf_decoration_name = (m_effType=="EFF")?"EfficiencyScaleFactor":m_effType+"EfficiencyScaleFactor";
    ATH_MSG_INFO("SF decoration_name is "<< m_sf_decoration_name);
    if(m_sfDec) delete m_sfDec;
    m_sfDec = new SG::AuxElement::Decorator< float>(m_sf_decoration_name);

    if(m_sf_replica_decoration_name == "") m_sf_replica_decoration_name = (m_effType=="EFF")?"EfficiencyScaleFactorReplicas":m_effType+"EfficiencyScaleFactorReplicas";
    ATH_MSG_INFO("SF replica decoration_name is "<< m_sf_replica_decoration_name);
    if(m_sfrDec) delete m_sfrDec;
    m_sfrDec = new SG::AuxElement::Decorator< std::vector<float> >(m_sf_replica_decoration_name);

    /// other
    m_affectingSys = affectingSystematics();

    /// continue with the orignal code
    if (m_custom_dir!="") ATH_MSG_WARNING("Note: setting up with user specified input file location "<<m_custom_dir<<" - this is not encouraged!");
    if (m_custom_file_Calo!="") ATH_MSG_WARNING("Note: setting up with user specified CaloTag input file "<<m_custom_file_Calo<<" - this is not encouraged!");
    if (m_custom_file_Combined!="") ATH_MSG_WARNING("Note: setting up with user specified Central muon input file "<<m_custom_file_Combined<<" - this is not encouraged! ");
    if (m_custom_file_HighEta!="") ATH_MSG_WARNING("Note: setting up with user specified High Eta input file "<<m_custom_file_HighEta<<" - this is not encouraged! ");
    if (m_custom_file_LowPt!="") ATH_MSG_WARNING("Note: setting up with user specified Low Pt input file "<<m_custom_file_LowPt<<" - this is not encouraged! ");
    if (m_custom_file_LowPtCalo!="") ATH_MSG_WARNING("Note: setting up with user specified Low Pt CaloTag input file "<<m_custom_file_LowPtCalo<<" - this is not encouraged! ");
    if ( !LoadLumiFromInput() || !LoadInputs()){
        ATH_MSG_ERROR("Error setting up the MuonEfficiencyScaleFactors tool");
        return StatusCode::FAILURE;
    }
    else {
        // set up for default running without systematics
        if (!applySystematicVariation (SystematicSet ())) {
            ATH_MSG_ERROR("loading the central value systematic set failed");
            return StatusCode::FAILURE;
        }
        // Add the affecting systematics to the global registry
        SystematicRegistry& registry = SystematicRegistry::getInstance();
        if (!registry.registerSystematics(*this)){
            ATH_MSG_ERROR ( "unable to register the systematics");
            return StatusCode::FAILURE;
        }
        ATH_MSG_INFO("Successfully initialized! ");
        if (m_doAudit){
            ATH_MSG_INFO("Note: Running with Audit trail active. If not needed, turn off to save CPU time");
        }
        return StatusCode::SUCCESS;
    }
}


CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactor( const xAOD::Muon& mu,
        float& sf ){
    if (m_doAudit) ApplyAuditInfo(mu);
    return (*m_current_sf)(mu)->ScaleFactor(mu,sf);

}

CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactor( const xAOD::Muon& mu ){
    float sf = 0;
    CorrectionCode result = this->getEfficiencyScaleFactor(mu,sf);
    // Decorate the muon
    (*m_sfDec)(mu) = sf;
    return result;
}


CorrectionCode MuonEfficiencyScaleFactors::getEfficiencyScaleFactorReplicas( const xAOD::Muon& mu,
        std::vector<float> & sfs ){
    if (m_doAudit) ApplyAuditInfo(mu);
    return (*m_current_sf)(mu)->ScaleFactorReplicas(mu,sfs) ;
}
CorrectionCode MuonEfficiencyScaleFactors::applyEfficiencyScaleFactorReplicas( const xAOD::Muon& mu, int nreplicas ){
    std::vector<float> replicas (nreplicas);
    CorrectionCode result =this->getEfficiencyScaleFactorReplicas(mu,replicas);
    // Decorate the muon
    (*m_sfrDec)(mu) = replicas;
    return result;
}
CorrectionCode MuonEfficiencyScaleFactors::getRecoEfficiency( const xAOD::Muon& mu,
        float& eff ){
    if (m_doAudit) ApplyAuditInfo(mu);
    return  (*m_current_sf)(mu)->Efficiency(mu,eff);
}
CorrectionCode MuonEfficiencyScaleFactors::applyRecoEfficiency( const xAOD::Muon& mu ){
    float eff = 0;
    CorrectionCode result = this->getRecoEfficiency(mu,eff);
    // Decorate the muon
    (*m_effDec)(mu) = eff;
    if (m_doAudit) ApplyAuditInfo(mu);
    return result;
}

std::string MuonEfficiencyScaleFactors::resolve_file_location(std::string filename){

    // can be overridden by u   ser defined directory - mainly for development, not for everyday use!
    if (m_custom_dir != "") return m_custom_dir+"/"+filename;
    std::string fullPathToFile = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s/%s",m_calibration_version.c_str(),
            filename.c_str()));
    if (fullPathToFile==""){
        ATH_MSG_ERROR( "Unable to resolve the input file "<<m_calibration_version<<"/"<<filename<<" via the PathResolver!");
    }
    return fullPathToFile;
}

std::string MuonEfficiencyScaleFactors::filename_Central(){

    if (m_custom_file_Combined!="") return (resolve_file_location(m_custom_file_Combined));
    else if (m_wp.find("Iso")!=std::string::npos){
        return resolve_file_location(Form("Iso_%s_Z.root",m_wp.c_str()));
    } else if(m_wp.find("TTVA")!=std::string::npos ){
        return resolve_file_location("TTVA_Z.root");
    }
    else return resolve_file_location(Form("Reco_%s_Z.root",m_wp.c_str()));
}
std::string MuonEfficiencyScaleFactors::filename_Calo(){

    if (m_custom_file_Calo!="") return resolve_file_location(m_custom_file_Calo);
    else if(m_wp.find("Iso")!=std::string::npos ||m_wp.find("TTVA")!=std::string::npos){
        return filename_Central();
    }
    else return resolve_file_location("Reco_CaloTag_Z.root");
}

std::string MuonEfficiencyScaleFactors::filename_HighEta(){

    if (m_custom_file_HighEta!="") return resolve_file_location(m_custom_file_HighEta);
    else if(m_wp.find("Iso")!=std::string::npos ||m_wp.find("TTVA")!=std::string::npos){
        return filename_Central();
    }
    else return resolve_file_location("Reco_HighEta_Z.root");
}

std::string MuonEfficiencyScaleFactors::filename_LowPt(){

    if (m_custom_file_LowPt!="") return resolve_file_location(m_custom_file_LowPt);
    // for the no reco WPs, we currently use the existing Z SF also for the low pt regime
    else if(m_wp.find("Iso")!=std::string::npos ||m_wp.find("TTVA")!=std::string::npos){
        return filename_Central();
    }
    else if (m_lowpt_threshold < 0){ // catch cases where JPsi is turned off
        return filename_Central();
    }
    else return resolve_file_location(Form("Reco_%s_JPsi.root",m_wp.c_str()));
}

std::string MuonEfficiencyScaleFactors::filename_LowPtCalo(){

    if (m_custom_file_LowPtCalo!="") return resolve_file_location(m_custom_file_LowPtCalo);
    // for the no reco WPs, we currently use the existing Z SF also for the low pt regime
    else if(m_wp.find("Iso")!=std::string::npos ||m_wp.find("TTVA")!=std::string::npos){
        return filename_Central();
    }
    else if (m_lowpt_threshold < 0){ // catch cases where JPsi is turned off
        return filename_Calo();
    }
    else return resolve_file_location("Reco_CaloTag_JPsi.root");
}


// load the SF histos
// and perform the weighted average
bool MuonEfficiencyScaleFactors::LoadInputs (){
    
    std::set<SystematicVariation>::iterator it;
    CP::SystematicSet sys = affectingSystematics();
    bool success = LoadEffiSet(SystematicSet());
    for (it = sys.begin(); it != sys.end(); ++it)
    {
        CP::SystematicSet bla;
        bla.insert(*it);
        success &= LoadEffiSet(bla);
    }

    return success;
}
bool MuonEfficiencyScaleFactors::LoadEffiSet (SystematicSet sys){

    // delete already existing SF sets first
    if (m_sf_sets.find(sys)!= m_sf_sets.end()){
        delete m_sf_sets[sys];
    }
    
    ATH_MSG_DEBUG("Running with the following lumi weights:");
    for (auto k :  m_lumi_weights_central) {
        ATH_MSG_DEBUG("\t" << k.first << ":\t" << k.second);
    }

    EffiCollection* ec = new EffiCollection(
            filename_Central(),
            filename_Calo(),
            filename_HighEta(),
            filename_LowPt(),
            filename_LowPtCalo(),
            m_lumi_weights_central,
            m_lumi_weights_calo,
            m_lumi_weights_forward,
            m_lumi_weights_lowpt,
            m_lumi_weights_lowptcalo,
            sys, m_effType, m_lowpt_threshold);

    m_sf_sets.insert (std::make_pair(sys,ec));

    return ec->CheckConsistency();

}

bool MuonEfficiencyScaleFactors::LoadLumiFromInput(){

    if (!(	   m_lumi_handler.LoadLumiFromInput(m_lumi_weights_central,filename_Central(), m_prwtool)
            && m_lumi_handler.LoadLumiFromInput(m_lumi_weights_calo,filename_Calo(), m_prwtool)
            && m_lumi_handler.LoadLumiFromInput(m_lumi_weights_forward,filename_HighEta(), m_prwtool)
            && m_lumi_handler.LoadLumiFromInput(m_lumi_weights_lowpt,filename_LowPt(), m_prwtool)
            && m_lumi_handler.LoadLumiFromInput(m_lumi_weights_lowptcalo,filename_LowPtCalo(), m_prwtool))){
    	ATH_MSG_ERROR("Failed to load the luminosity compositions from the PRW tool for at least one input file!");
    	return false;
    }

    // check if we have a nonzero int lumi
    std::vector<lumimap*> maps {&m_lumi_weights_central, &m_lumi_weights_calo, &m_lumi_weights_forward, &m_lumi_weights_lowpt, &m_lumi_weights_lowptcalo};
    for (auto & map : maps){
    	float ltot = 0;
        for (const auto & entry : *map){
        	ltot += entry.second;
        }
        if (ltot == 0){
        	ATH_MSG_ERROR("Your ilumicalc file provides zero integrated luminosity for the data periods described by the muon scale factors instance you set up. "<<
        			"Please make sure that both the MuonEfficiencyScaleFactors instance and the PRW tool have been configured with consistent calibration releases / ilumicalc files! "<<
					"The current setup leads to ill-defined scale factors - can not continue.");
        	return false;
        }
    }
    return true;
}

bool
MuonEfficiencyScaleFactors::isAffectedBySystematic( const SystematicVariation& systematic ) const {

    SystematicSet sys = affectingSystematics();
    return sys.find (systematic) != sys.end ();

}

/// returns: the list of all systematics this tool can be affected by
SystematicSet
MuonEfficiencyScaleFactors::affectingSystematics() const{
    SystematicSet result;
    result.insert (SystematicVariation ("MUON_"+m_effType+"_STAT", 1));
    result.insert (SystematicVariation ("MUON_"+m_effType+"_STAT", -1));

    result.insert (SystematicVariation ("MUON_"+m_effType+"_SYS", 1));
    result.insert (SystematicVariation ("MUON_"+m_effType+"_SYS", -1));
    
    if (m_effType == "EFF" && m_lowpt_threshold > 0){
        result.insert (SystematicVariation ("MUON_"+m_effType+"_STAT_LOWPT", 1));
        result.insert (SystematicVariation ("MUON_"+m_effType+"_STAT_LOWPT", -1));

        result.insert (SystematicVariation ("MUON_"+m_effType+"_SYS_LOWPT", 1));
        result.insert (SystematicVariation ("MUON_"+m_effType+"_SYS_LOWPT", -1));
        
    }
    return result;
}

/// returns: the list of all systematics this tool recommends to use
SystematicSet
MuonEfficiencyScaleFactors::recommendedSystematics() const{
    return affectingSystematics();
}

SystematicCode MuonEfficiencyScaleFactors::applySystematicVariation (const SystematicSet& systConfig){
    SystematicSet mySysConf;
    if (m_filtered_sys_sets.find(systConfig) == m_filtered_sys_sets.end()){
        if (!SystematicSet::filterForAffectingSystematics(systConfig,m_affectingSys, mySysConf)){
            ATH_MSG_ERROR("Unsupported combination of systematics passed to the tool! ");
            return SystematicCode::Unsupported;
        }
        m_filtered_sys_sets[systConfig] = mySysConf;
    }
    mySysConf = m_filtered_sys_sets[systConfig] ;
    boost::unordered_map<SystematicSet,EffiCollection*>::iterator SFiter = m_sf_sets.find (mySysConf);
    if (SFiter != m_sf_sets.end()){
        m_current_sf = SFiter->second;
        if (m_doAudit){
            m_sys_string= m_current_sf->sysname();
        }
        return SystematicCode::Ok;
    }
    else {
        ATH_MSG_ERROR("Illegal combination of systematics passed to the tool! Did you maybe request multiple variations at the same time? ");
        ATH_MSG_DEBUG(" List of relevant systematics included in your combination:");
        for (std::set<SystematicVariation>::iterator t = mySysConf.begin();t!= mySysConf.end();++t){
            ATH_MSG_DEBUG("\t"<<(*t).name());
        }
        return SystematicCode::Unsupported;
    }
}

bool MuonEfficiencyScaleFactors::AlreadyApplied(const xAOD::Muon & mu){
    if (m_doAudit) CheckAuditEvent();
    return (m_audit_processed.find(std::make_pair(m_current_sf,const_cast<xAOD::Muon*>(&mu)))!=m_audit_processed.end());
}
bool MuonEfficiencyScaleFactors::AlreadyApplied(xAOD::Muon & mu){
    if (m_doAudit) CheckAuditEvent();
    return (m_audit_processed.find(std::make_pair(m_current_sf,&mu))!=m_audit_processed.end());
}

void MuonEfficiencyScaleFactors::ApplyAuditInfo (const xAOD::Muon& mu){
    if (m_doAudit) CheckAuditEvent();
    if (m_audit_processed.find(std::make_pair(m_current_sf,const_cast<xAOD::Muon*>(&mu))) == m_audit_processed.end()){
        m_audit_processed.insert(std::make_pair(m_current_sf,const_cast<xAOD::Muon*>(&mu)));
    }
    static SG::AuxElement::Decorator< bool > effboolDec( "MuonEfficiencyCorrections" );
    effboolDec(mu) = true;
    static SG::AuxElement::Decorator< std::string  > versionDec( "MuonEfficiencyCorrectionsVersion" );
    versionDec(mu) = m_version_string+"_"+m_sys_string;
    static SG::AuxElement::Decorator <std::string > appliedDec("AppliedCorrections");
    appliedDec(mu) += " MuonEfficiencyCorrections";
    if(m_effType!="EFF") appliedDec(mu) += m_effType;
}

void MuonEfficiencyScaleFactors::CheckAuditEvent(){
    const xAOD::EventInfo* ei = 0;
    if( evtStore()->retrieve( ei, "EventInfo" ).isFailure() ) {
        ATH_MSG_WARNING( "No EventInfo object could be retrieved" );
        ATH_MSG_WARNING( "Random number generation not configured correctly" );
    }
    const unsigned long long eventNumber = ei ? ei->eventNumber() : 0;
    if (m_audit_last_evt != eventNumber){
        m_audit_processed.clear();
        m_audit_last_evt = eventNumber;
    }
}


MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors( const MuonEfficiencyScaleFactors& toCopy):
                        asg::AsgTool( std::string(toCopy.classname)+"_copy" ),
                        m_sf_sets(toCopy.m_sf_sets),
                        m_current_sf(toCopy.m_current_sf),
                        m_lumi_weights_central(toCopy.m_lumi_weights_central),
                        m_lumi_weights_calo(toCopy.m_lumi_weights_calo),
                        m_lumi_weights_forward(toCopy.m_lumi_weights_forward),
                        m_lumi_weights_lowpt(toCopy.m_lumi_weights_lowpt),
                        m_lumi_weights_lowptcalo(toCopy.m_lumi_weights_lowptcalo),
                        classname(toCopy.classname),
                        m_lumi_handler(toCopy.m_lumi_handler),
                        m_audit_processed(toCopy.m_audit_processed),
                        m_version_string(toCopy.m_version_string),
                        m_sys_string(toCopy.m_sys_string),
                        m_audit_last_evt(toCopy.m_audit_last_evt),
                        m_effType(toCopy.m_effType),m_lowpt_threshold(toCopy.m_lowpt_threshold), m_effDec(0),m_sfDec(0),m_sfrDec(0){

    declareProperty( "WorkingPoint", m_wp = "Medium" );
    declareProperty( "doAudit", m_doAudit = false);

    // these are for debugging / testing, *not* for general use!
    declareProperty( "CustomInputFolder", m_custom_dir = "");
    declareProperty( "CustomFileCaloTag", m_custom_file_Calo = "");
    declareProperty( "CustomFileCombined", m_custom_file_Combined = "");
    declareProperty( "CustomFileHighEta", m_custom_file_HighEta = "");
    declareProperty( "CustomFileLowPt", m_custom_file_LowPt = "");
    declareProperty( "CustomFileLowPtCalo", m_custom_file_LowPtCalo = "");

    declareProperty( "EfficiencyDecorationName", m_efficiency_decoration_name = "");
    declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name = "");
    declareProperty( "ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name = "");

    declareProperty( "CalibrationRelease", m_calibration_version = "160725_ICHEP");
    declareProperty( "EfficiencyType", m_effType = "");
    declareProperty( "LowPtThreshold", m_lowpt_threshold = 15000.);

    declareProperty( "PileupReweightingTool", m_prwtool);
}

MuonEfficiencyScaleFactors & MuonEfficiencyScaleFactors::operator = (const MuonEfficiencyScaleFactors & tocopy){

    if (this == &tocopy){
        return *this;
    }

    m_sf_sets = tocopy.m_sf_sets;
    m_current_sf= tocopy.m_current_sf;
    m_lumi_weights_central= tocopy.m_lumi_weights_central;
    m_lumi_weights_calo= tocopy.m_lumi_weights_calo;
    m_lumi_weights_forward= tocopy.m_lumi_weights_forward;
    m_lumi_weights_lowpt= tocopy.m_lumi_weights_lowpt;
    m_lumi_weights_lowptcalo= tocopy.m_lumi_weights_lowptcalo;
    classname= tocopy.classname;
    m_lumi_handler= tocopy.m_lumi_handler;
    m_audit_processed= tocopy.m_audit_processed;
    m_version_string= tocopy.m_version_string;
    m_sys_string= tocopy.m_sys_string;
    m_audit_last_evt= tocopy.m_audit_last_evt;

    m_wp = tocopy.m_wp;
	m_doAudit = tocopy.m_doAudit;
    m_custom_dir = tocopy.m_custom_dir;
    m_custom_file_Calo = tocopy.m_custom_file_Calo;
    m_custom_file_LowPt = tocopy.m_custom_file_LowPt;
    m_custom_file_LowPtCalo = tocopy.m_custom_file_LowPtCalo;

    m_custom_file_Combined = tocopy.m_custom_file_Combined;
    m_custom_file_HighEta = tocopy.m_custom_file_HighEta;
    m_efficiency_decoration_name = tocopy.m_efficiency_decoration_name;
    m_sf_decoration_name = tocopy.m_sf_decoration_name;
    m_sf_replica_decoration_name= tocopy.m_sf_replica_decoration_name;

    m_calibration_version= tocopy.m_calibration_version;
    m_effType = tocopy.m_effType;
    m_lowpt_threshold = tocopy.m_lowpt_threshold;
    m_effDec = 0;
    m_sfDec = 0;
    m_sfrDec = 0;

    return *this;

}

} /* namespace CP */
