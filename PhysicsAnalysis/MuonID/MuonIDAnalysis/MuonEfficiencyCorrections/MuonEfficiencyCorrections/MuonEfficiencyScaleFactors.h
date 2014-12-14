/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonEfficiencyScaleFactors.h
 *
 *  Created on: Apr 9, 2014
 *      Author: goblirsc
 */

#ifndef MUONEFFICIENCYSCALEFACTORS_H_
#define MUONEFFICIENCYSCALEFACTORS_H_

#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include "AsgTools/AsgTool.h"
#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include "MuonEfficiencyCorrections/EffiCollection.h"
#include "MuonEfficiencyCorrections/LumiWeightHandler.h"

#include <string>

#include <boost/unordered_map.hpp>

namespace CP {

class MuonEfficiencyScaleFactors: public CP::IMuonEfficiencyScaleFactors, public asg::AsgTool {
    ASG_TOOL_CLASS2( MuonEfficiencyScaleFactors, CP::IMuonEfficiencyScaleFactors, CP::ISystematicsTool )

public:
    MuonEfficiencyScaleFactors( const std::string& name);


    virtual ~MuonEfficiencyScaleFactors();

    /// initialize the tool once all settings are in place!
    virtual StatusCode initialize();


    /// Retrieve the Scale factor and decorate the muon
    virtual CorrectionCode getEfficiencyScaleFactor( const xAOD::Muon& mu,
            float& sf );
    virtual CorrectionCode applyEfficiencyScaleFactor(const  xAOD::Muon& mu );

    /// returns: whether this tool is affected by the given systematis
    virtual bool
    isAffectedBySystematic( const SystematicVariation& systematic ) const;

    /// returns: the list of all systematics this tool can be affected by
    virtual SystematicSet
    affectingSystematics() const;

    /// returns: the list of al l systematics this tool recommends to use
    virtual SystematicSet
    recommendedSystematics() const;

    virtual SystematicCode applySystematicVariation
    ( const SystematicSet& systConfig );


    // Bonus stuff

    // replica generation
    virtual CorrectionCode getEfficiencyScaleFactorReplicas( const xAOD::Muon& mu,
            std::vector<float> & sf_err );
    virtual CorrectionCode applyEfficiencyScaleFactorReplicas( const xAOD::Muon& mu, int nreplicas=50 );


    /// Obtain the muon efficiency measured using the data
    virtual CorrectionCode getRecoEfficiency( const xAOD::Muon& mu,
            float& eff );
    virtual CorrectionCode applyRecoEfficiency(const  xAOD::Muon& mu );

    // audit trail functionality

    bool AlreadyApplied(const xAOD::Muon & mu);
    bool AlreadyApplied(xAOD::Muon & mu);

    // copy constructor, to make reflex happy...
    MuonEfficiencyScaleFactors( const MuonEfficiencyScaleFactors& tocopy);

    // to make coverity happy 
    MuonEfficiencyScaleFactors & operator = (const MuonEfficiencyScaleFactors & tocopy);

private:

    typedef std::map<std::string,double> lumimap;
    typedef std::map<std::string,double>::iterator ilumimap;

    /// the working point to operate on
    std::string m_wp;
    /// the data taking period to work with
    std::string m_year;

    boost::unordered_map<SystematicSet,EffiCollection*> m_sf_sets;
    EffiCollection *m_current_sf;

    /// the lumi configuration. Used to average the scale factors
    lumimap m_lumi_weights_central;
    lumimap m_lumi_weights_calo;
    lumimap m_lumi_weights_forward;
    /// allow the user to specify the lumi composition
    lumimap m_user_lumi_weights;

    std::string m_custom_dir;
    std::string m_custom_file_Combined;
    std::string m_custom_file_Calo;
    std::string m_custom_file_HighEta;

    const char* classname;

    /// load the SF histos
    bool LoadEffiSet (SystematicSet sys);
    bool LoadInputs ();

    /// a tool to help handle the user lumi inputs
    LumiWeightHandler m_lumi_handler;

    /// load the lumi info provided with the input files
    bool LoadLumiFromInput();

    /// construct the name of the input files from the configuration
    std::string filename_Central();
    std::string filename_Calo();
    std::string filename_HighEta();


    // utility method to 'dress' a filename using the path resolver
    std::string resolve_file_location(std::string filename);


    // audit mode
    bool m_doAudit;
    // apply info to muons and log that we processed them
    void ApplyAuditInfo (const xAOD::Muon& mu);

    // reset the audit log at every event
    void CheckAuditEvent();

    // remember what we already looked at
    std::set<std::pair<EffiCollection*, xAOD::Muon*> > m_audit_processed;

    // info to apply to the muon when in audit mode
    std::string m_version_string;
    std::string m_sys_string;
    // the current event
    unsigned long long m_audit_last_evt;

    std::map<CP::SystematicSet, CP::SystematicSet> m_filtered_sys_sets;

    std::string m_efficiency_decoration_name;
    std::string m_sf_decoration_name;
    std::string m_sf_replica_decoration_name;



};

} /* namespace CP */

#endif /* MUONEFFICIENCYSCALEFACTORS_H_ */
