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
    double genXsec;
    double filterEff;
    double kFactor;
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
    virtual double getGeneratorXsection(const int dsid) const = 0;
    
    /// return the k-factor for DSID
    virtual double getKfactor(const int dsid) const = 0;
    
    /// return the sample cross-section for DSID (can we find a more descriptive name for this, PMG? /CO)
    virtual double getSampleXsection(const int dsid) const = 0;
    
    /// get a list of the DSID for the loaded samples
    virtual std::vector<int> getLoadedDSIDs() const = 0;
        
  }; // class IPMGCrossSectionTool
  
} // namespace PMGTools

// overload the output operator for the AllSampleInfo object
std::ostream& operator<<(std::ostream& os, const PMGTools::AllSampleInfo& s) {
  return os << "Name:          " << s.containerName << std::endl
	    << "DSID:          " << s.dsid << std::endl
	    << "Cross section: " << s.genXsec << std::endl
	    << "k-factor:      " << s.kFactor << std::endl
	    << "Filter eff.:   " << s.filterEff << std::endl;
}
#endif //> !PMGTOOLS_IPMGCROSSSECTIONTOOL_H
