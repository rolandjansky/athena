// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_PMGCROSSSECTIONTOOL_H
#define PMGTOOLS_PMGCROSSSECTIONTOOL_H

// System include(s):
#include <array>

// Infrastructure include(s):
#include "AsgTools/AsgTool.h"

// Interface include(s):
#include "PMGTools/IPMGCrossSectionTool.h"

/// Tool providing sample cross-sections and k-factors etc
///

namespace PMGTools {
  
  class PMGCrossSectionTool : public virtual IPMGCrossSectionTool,
			      public asg::AsgTool { 
    
  public:
    //constructor for athena can be created using special macro
    ASG_TOOL_CLASS( PMGCrossSectionTool, PMGTools::IPMGCrossSectionTool )
    
    /// Standard tool constructor, with name
    PMGCrossSectionTool( const std::string& name =
			 "PMGCrossSectionTool" );
    
    /// @name Interface inherited from asg::IAsgTool
    /// @{
    
    /// initialize() is required by AsgTool base class
    StatusCode  initialize();
    
    /// read infos from file, store them in the structure and make a vector that keeps all of them                                                                                   
    bool readInfosFromFiles(std::vector<std::string>);
    
    /// read infos from all files in dir                                                                                                                                               
    bool readInfosFromDir(const std::string& inputDir);
    
    /// return filter efficiency for DSID
    double getFilterEff(const int dsid) const;
    
    /// return the sample name for DSID
    std::string getSampleName(const int dsid) const;
    
    /// return the AMI cross-section for DSID
    double getGeneratorXsection(const int dsid) const;
    
    
    /// return the k-factor for DSID
    double getKfactor(const int dsid) const;
   
    /// return the sample cross-section for DSID
    double getSampleXsection(const int dsid) const;
   
    /// get a list of the DSID for the loaded samples
    std::vector<int> getLoadedDSIDs() const;
   
  private:
      
    // store vector of structures, each structure contains full info for DSID
    std::map<int,PMGTools::AllSampleInfo> fStoreSampleInfo;
    std::string m_InputFileName;
    
  }; // class PMGCrossSectionTool
  
}

#endif //> !PMGTOOLS_PMGCROSSSECTIONTOOL_H
