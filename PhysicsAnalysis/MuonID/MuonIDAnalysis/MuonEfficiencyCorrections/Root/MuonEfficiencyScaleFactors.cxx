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
#include "PATInterfaces/SystematicList.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h"
#include "PathResolver/PathResolver.h"

namespace CP {

    static std::string CURRENT_MEC_VERSION = "MuonEfficiencyCorrections-03-00-10";

    MuonEfficiencyScaleFactors::MuonEfficiencyScaleFactors( const std::string& name):
                                    asg::AsgTool( name ),
                                    m_sf_sets(),
                                    m_current_sf(0),
                                    m_lumi_weights_central(),
                                    m_lumi_weights_calo(),
                                    m_lumi_weights_forward(),
                                    classname(name.c_str()),
                                    m_lumi_handler(),
                                    m_audit_processed(),
                                    m_version_string(CURRENT_MEC_VERSION),
                                    m_sys_string(""),
                                    m_audit_last_evt(-1){

        declareProperty( "WorkingPoint", m_wp = "CB" );
        declareProperty( "DataPeriod", m_year = "2012");
        declareProperty( "doAudit", m_doAudit = false);
        declareProperty( "LumiWeights", m_user_lumi_weights = lumimap());

        // these are for debugging / testing, *not* for general use!
        declareProperty( "CustomInputFolder", m_custom_dir = "");
        declareProperty( "CustomFileCaloTag", m_custom_file_Calo = "");
        declareProperty( "CustomFileCombined", m_custom_file_Combined = "");
        declareProperty( "CustomFileHighPt", m_custom_file_HighEta = "");

        declareProperty( "EfficiencyDecorationName", m_efficiency_decoration_name = "Efficiency");
        declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name = "EfficiencyScaleFactor");
        declareProperty( "ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name = "EfficiencyScaleFactorReplicas");
    }

    MuonEfficiencyScaleFactors::~MuonEfficiencyScaleFactors(){

        // remember to clean up afterwards...
        for (boost::unordered_map<SystematicSet,EffiCollection*>::iterator it = m_sf_sets.begin(); it != m_sf_sets.end();++it){
            if (it->second) delete it->second;
        }
    }

    StatusCode MuonEfficiencyScaleFactors::initialize(){
        Info(classname,"Trying to initialize, with working point %s and data period %s ",m_wp.c_str(),m_year.c_str());
        if (!m_user_lumi_weights.empty()) Info(classname,"Note: setting up with user specified Luminosities");
        if (m_custom_dir!="") Info(classname,"Note: setting up with user specified input file location %s - this is not encouraged!",m_custom_dir.c_str());
        if (m_custom_file_Calo!="") Info(classname,"Note: setting up with user specified CaloTag input file %s - this is not encouraged!",m_custom_file_Calo.c_str());
        if (m_custom_file_Combined!="") Info(classname,"Note: setting up with user specified Central muon input file %s - this is not encouraged! ",m_custom_file_Combined.c_str());
        if (m_custom_file_HighEta!="") Info(classname,"Note: setting up with user specified High Eta input file %s - this is not encouraged! " ,m_custom_file_HighEta.c_str());
        if ( !LoadLumiFromInput() || !LoadInputs()){
            Error(classname,"Error setting up the tool");
            return StatusCode::FAILURE;
        }
        else {
            // set up for default running without systematics
            if (!applySystematicVariation (SystematicSet ())) {
                Error(classname,"loading the central value systematic set failed");
                return StatusCode::FAILURE;
            }
            // Add the affecting systematics to the global registry
            SystematicRegistry& registry = SystematicRegistry::getInstance();
            if (!registry.registerSystematics(*this)){
                Error (classname, "unable to register the systematics");
                return StatusCode::FAILURE;
            }
            Info(classname,"Successfully initialized! ");
            if (m_doAudit){
                Info(classname, "Note: Running with Audit trail active. If not needed, turn off to save CPU time");
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
        static SG::AuxElement::Decorator< float > sfDec( m_sf_decoration_name );
        sfDec(mu) = sf;
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
        static SG::AuxElement::Decorator< std::vector<float> > sfrDec( m_sf_replica_decoration_name );
        sfrDec(mu) = replicas;
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
        static SG::AuxElement::Decorator< float> effDec( m_efficiency_decoration_name );
        effDec(mu) = eff;
        if (m_doAudit) ApplyAuditInfo(mu);
        return result;
    }

    std::string MuonEfficiencyScaleFactors::resolve_file_location(std::string filename){

        // can be overridden by user defined directory - mainly for development, not for everyday use!
        if (m_custom_dir != "") return m_custom_dir;
        std::string fullPathToFile = PathResolverFindCalibFile(Form("MuonEfficiencyCorrections/%s",filename.c_str()));
        if (fullPathToFile==""){
            Error(classname, "Unable to resolve the input file %s via the PathResolver!",filename.c_str());
        }
        return fullPathToFile;
    }

    std::string MuonEfficiencyScaleFactors::filename_Central(){

        if (m_custom_file_Combined!="") return (resolve_file_location(m_custom_file_Combined));
        else return resolve_file_location(Form("MuonsChain_%s_%s_SF.root",m_wp.c_str(),m_year.c_str()));
    }
    std::string MuonEfficiencyScaleFactors::filename_Calo(){

        if (m_custom_file_Calo!="") return resolve_file_location(m_custom_file_Calo);
        else return resolve_file_location(Form("CaloTag_%s_SF.root",m_year.c_str()));
    }

    std::string MuonEfficiencyScaleFactors::filename_HighEta(){

        if (m_custom_file_HighEta!="") return resolve_file_location(m_custom_file_HighEta);
        else return resolve_file_location("SF_higheta_MUON.root");
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

        EffiCollection* ec = new EffiCollection(
                filename_Central(),
                filename_Calo(),
                filename_HighEta(),
                m_lumi_weights_central,
                m_lumi_weights_calo,
                m_lumi_weights_forward,
                sys);

        m_sf_sets.insert (std::make_pair(sys,ec));

        return ec->CheckConsistency();

    }

    bool MuonEfficiencyScaleFactors::LoadLumiFromInput(){
        // tell the lumi handler what the user requested
        m_lumi_handler.SetLumiWeights(m_user_lumi_weights);
        // and have it read the weights for us
        return  (m_lumi_handler.LoadLumiFromInput(m_lumi_weights_central,filename_Central())
                && m_lumi_handler.LoadLumiFromInput(m_lumi_weights_calo,filename_Calo())
                && m_lumi_handler.LoadLumiFromInput(m_lumi_weights_forward,filename_HighEta()));

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
        result.insert (SystematicVariation ("MUONSFSTAT", 1));
        result.insert (SystematicVariation ("MUONSFSTAT", -1));

        result.insert (SystematicVariation ("MUONSFSYS", 1));
        result.insert (SystematicVariation ("MUONSFSYS", -1));
        return result;
    }

    /// returns: the list of al l systematics this tool recommends to use
    SystematicSet
    MuonEfficiencyScaleFactors::recommendedSystematics() const{
        return affectingSystematics();
    }

    SystematicCode MuonEfficiencyScaleFactors::applySystematicVariation (const SystematicSet& systConfig){
        SystematicSet mySysConf;
        static CP::SystematicSet affectingSys = affectingSystematics();
        if (m_filtered_sys_sets.find(systConfig) == m_filtered_sys_sets.end()){
            if (!SystematicSet::filterForAffectingSystematics(systConfig,affectingSys, mySysConf)){
                Error(classname, "Unsupported combination of systematics passed to the tool! ");
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
            Error(classname, "Illegal combination of systematics passed to the tool! Did you maybe request multiple variations at the same time? ");
            Info(classname," List of relevant systematics included in your combination:");
            for (std::set<SystematicVariation>::iterator t = mySysConf.begin();t!= mySysConf.end();++t){
                Info(classname, (*t).name().c_str());
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
                classname(toCopy.classname),
                m_lumi_handler(toCopy.m_lumi_handler),
                m_audit_processed(toCopy.m_audit_processed),
                m_version_string(toCopy.m_version_string),
                m_sys_string(toCopy.m_sys_string),
                m_audit_last_evt(toCopy.m_audit_last_evt) {

        declareProperty( "WorkingPoint", m_wp = "CB" );
        declareProperty( "DataPeriod", m_year = "2012");
        declareProperty( "doAudit", m_doAudit = false);
        declareProperty( "LumiWeights", m_user_lumi_weights = lumimap());

        // these are for debugging / testing, *not* for general use!
        declareProperty( "CustomInputFolder", m_custom_dir = "");
        declareProperty( "CustomFileCaloTag", m_custom_file_Calo = "");
        declareProperty( "CustomFileCombined", m_custom_file_Combined = "");
        declareProperty( "CustomFileHighPt", m_custom_file_HighEta = "");

        declareProperty( "EfficiencyDecorationName", m_efficiency_decoration_name = "Efficiency");
        declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name = "EfficiencyScaleFactor");
        declareProperty( "ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name = "EfficiencyScaleFactorReplicas");
    }

    MuonEfficiencyScaleFactors & MuonEfficiencyScaleFactors::operator = (const MuonEfficiencyScaleFactors & tocopy){
        
        m_sf_sets = tocopy.m_sf_sets;
        m_current_sf= tocopy.m_current_sf;
        m_lumi_weights_central= tocopy.m_lumi_weights_central;
        m_lumi_weights_calo= tocopy.m_lumi_weights_calo;
        m_lumi_weights_forward= tocopy.m_lumi_weights_forward;
        classname= tocopy.classname;
        m_lumi_handler= tocopy.m_lumi_handler;
        m_audit_processed= tocopy.m_audit_processed;
        m_version_string= tocopy.m_version_string;
        m_sys_string= tocopy.m_sys_string;
        m_audit_last_evt= tocopy.m_audit_last_evt;

        m_wp = tocopy.m_wp;
        m_year = tocopy.m_year;
        m_doAudit = tocopy.m_doAudit;
        m_user_lumi_weights = tocopy.m_user_lumi_weights;
        m_custom_dir = tocopy.m_custom_dir;
        m_custom_file_Calo = tocopy.m_custom_file_Calo;
        
        m_custom_file_Combined = tocopy.m_custom_file_Combined;
        m_custom_file_HighEta = tocopy.m_custom_file_HighEta;
        m_efficiency_decoration_name = tocopy.m_efficiency_decoration_name;
        m_sf_decoration_name = tocopy.m_sf_decoration_name;
        m_sf_replica_decoration_name= tocopy.m_sf_replica_decoration_name;
        
        declareProperty( "WorkingPoint", m_wp) ;
        declareProperty( "DataPeriod", m_year);
        declareProperty( "doAudit", m_doAudit );
        declareProperty( "LumiWeights", m_user_lumi_weights);

        // these are for debugging / testing, *not* for general use!
        declareProperty( "CustomInputFolder", m_custom_dir);
        declareProperty( "CustomFileCaloTag", m_custom_file_Calo );
        declareProperty( "CustomFileCombined", m_custom_file_Combined );
        declareProperty( "CustomFileHighPt", m_custom_file_HighEta);

        declareProperty( "EfficiencyDecorationName", m_efficiency_decoration_name);
        declareProperty( "ScaleFactorDecorationName", m_sf_decoration_name);
        declareProperty( "ScaleFactorReplicaDecorationName", m_sf_replica_decoration_name );
        
        
        return *this;
        
    }

} /* namespace CP */
