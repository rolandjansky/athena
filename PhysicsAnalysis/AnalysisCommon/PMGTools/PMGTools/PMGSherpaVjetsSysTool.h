// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PMGSherpaVjetsSysTool.h tripiana $
//
// Tool calculating event weights for systematic variations of Sherpa V+jets (V=Z,W,gamma) samples
//
// @author Martin Tripiana <martin.tripiana@cern.ch>
//         John Anders     <john.kenneth.anders@cern.ch>
//
// $Date: 2016-07-12 $
//

#ifndef PMGTOOLS_PMGSHERPAVJETSSYSTOOL_H
#define PMGTOOLS_PMGSHERPAVJETSSYSTOOL_H

// Infrastructure include(s):
#include "AsgTools/AsgTool.h"

// Interface include(s):
#include "PMGAnalysisInterfaces/IPMGSherpaVjetsSysTool.h"

// System include(s):
#include <map>
#include <vector>
#include <memory>

// ROOT include(s):
#include "TFile.h"

// Forward declaration
class TH2F;

namespace PMGTools {

  class PMGSherpaVjetsSysTool : public virtual IPMGSherpaVjetsSysTool,
                                public asg::AsgTool { 

    //proper constructor for athena 
    ASG_TOOL_CLASS( PMGSherpaVjetsSysTool, PMGTools::IPMGSherpaVjetsSysTool )

  public:
    /// Standard tool constructor, with name
    PMGSherpaVjetsSysTool( const std::string& name );

    /// Initialize is required by AsgTool base class
    StatusCode initialize() override;

    /// Return weight for a given systematic variation
    double getWeight( const std::string& sysVariation = "nom") override;

  protected:
    /// Some setters to speed up the calculation for multiple variations/events
    void setNjets(int njets) override;
    void cacheDSID(bool do_it=true) override;

    /// Helpers to get the list of supported variations for Z/W+jets and gamma+jets
    std::vector<std::string> getSupportedVariations(SysParType ptype) override;

    /// get number of truth jets as defined in the parametrization analysis
    unsigned int getNtruthJets() override;

  private:
    int getPtMapping(SysParType& pType);

    /// Current settings
    int m_current_njets;

    /// Cache MC DSID (to avoid checking it everytime)
    bool m_use_cache;
    SysParType m_cache_ptype;
    unsigned int m_cache_ptidx;

    ///Parametrization files
    std::unique_ptr< TFile > m_file;

    ///Parametrization maps
    std::map<std::string, TH2F*> m_map_w;
    std::map<std::string, TH2F*> m_map_zll;
    std::map<std::string, TH2F*> m_map_znunu;
    std::map<std::string, TH2F*> m_map_gamma;

    std::vector<std::string> m_supported_w;
    std::vector<std::string> m_supported_zll;
    std::vector<std::string> m_supported_znunu;
    std::vector<std::string> m_supported_gamma;

    /// Tool properties

    ///Parametrizations input file
    std::string m_weightsFile;

    /// The truth jet container to use for the calculation
    std::string m_truthJetContainer;
    /// The truth particle container to use for the calculation
    std::string m_truthParticleContainer;

  }; // class PMGSherpaVjetsSysTool

} //namespace PMGTools

#endif //> !PMGTOOLS_PMGSHERPAVJETSSYSTOOL_H
