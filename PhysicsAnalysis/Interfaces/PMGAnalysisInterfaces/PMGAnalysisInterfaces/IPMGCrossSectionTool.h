// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PMGTOOLS_IPMGCROSSSECTIONTOOL_H
#define PMGTOOLS_IPMGCROSSSECTIONTOOL_H

// Infrastructure include(s):
#include "AsgTools/IAsgTool.h"

#include <vector>

namespace PMGTools {
  
  // store all information for certain DSID in structure
  struct AllSampleInfo{
    int dsid;
    std::string containerName;
    double amiXSec;
    double br;
    double filterEff;
    double higherOrderXsecTotal;
    double kFactor;
    double higherOrderXsecSample;
  };

    
  class IPMGCrossSectionTool : public virtual asg::IAsgTool { 
    
    // Declare the interface that the class provides
    ASG_TOOL_INTERFACE( PMGTools::IPMGCrossSectionTool )
    
    public:
    
    /// read infos from file, store them in the structure and make a vector that keeps all of them 
    virtual bool readInfosFromFiles(std::vector<std::string>) = 0;
    
    /// read infos from all files in dir 
    virtual bool readInfosFromDir(const std::string& inputDir) = 0;
    
    /// return filter efficiency for DSID
    virtual double getFilterEff(const int dsid) const = 0;
    
    /// return the sample name for DSID
    virtual std::string getSampleName(const int dsid) const = 0;
    
    /// return the AMI cross-section for DSID
    virtual double getAMIXsection(const int dsid) const = 0;
    
    /// return the branching ratio for DSID
    virtual double getBR(const int dsid) const = 0;
    
    /// return the k-factor for DSID
    virtual double getKfactor(const int dsid) const = 0;
    
    /// return the sample cross-section for DSID
    virtual double getSampleXsection(const int dsid) const = 0;
    
    /// get a list of the DSID for the loaded samples
    virtual std::vector<int> getLoadedDSIDs() const = 0;
        
  }; // class IPMGCrossSectionTool
  
} // namespace PMGTools

#endif //> !PMGTOOLS_IPMGCROSSSECTIONTOOL_H
