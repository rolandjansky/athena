/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PMGCrossSectionTool.cxx 810599 2017-09-22 15:50:07Z cohm $

#include <fstream>
#include <sstream>
#include <string>
#include <TSystem.h>
#include <TString.h>
#include <TTree.h>
#include <iostream>
#include <stdlib.h>

// Local include(s):
#include "PMGTools/PMGCrossSectionTool.h"

namespace PMGTools {
  
  PMGCrossSectionTool::
  PMGCrossSectionTool( const std::string& name ) : asg::AsgTool( name )
  {
    
  }
  
  StatusCode PMGCrossSectionTool::initialize() {
    
    // Tell the user what's happening:
    ATH_MSG_INFO( "Initializing " << name() << "..." );
    //ATH_MSG_INFO( "Read in all Xsec Info ..." );

    //readInfosFromDir(inputDir.c_str());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  bool PMGCrossSectionTool::readInfosFromFiles(std::vector<std::string> InputFiles)
  {
    for (const auto& currentFileName : InputFiles) {

      std::ifstream currentFile;
      currentFile.open(currentFileName.c_str());

      // use the TTree functionality for reading in text files - easy way to remove
      // dependency on specific order of the columns in the text file
      TTree* xsecTree = new TTree();
      xsecTree->ReadFile(currentFileName.c_str());
      
      // one branch per variable we care about of the existing ones: DSID/I:DSName/C:GenXsec/D:BR:FilterEff:Xsec:Kfactor:TotSampleXsec
      int dsid(0);
      double genXsec(0);      
      double filterEff(0);
      double kFactor(0);
      TBranch* dsidBranch = xsecTree->GetBranch("DSID");
      dsidBranch->SetAddress(&dsid);
      TBranch* genXsecBranch = xsecTree->GetBranch("crossSection");
      genXsecBranch->SetAddress(&genXsec);
      TBranch* filterEffBranch = xsecTree->GetBranch("genFiltEff");
      filterEffBranch->SetAddress(&filterEff);
      TBranch* kFactorBranch = xsecTree->GetBranch("kFactor");
      kFactorBranch->SetAddress(&kFactor);

      auto nEntries = xsecTree->GetEntries();
      for (Long64_t i = 0; i < nEntries; i++) {
	xsecTree->GetEntry(i);
	AllSampleInfo sample;
	sample.dsid = dsid;
	sample.genXsec = genXsec;
	sample.filterEff = filterEff;
	sample.kFactor = kFactor;
	//sample.containerName = containerName;
	fStoreSampleInfo[sample.dsid] = sample;
      }

      delete xsecTree;
    }
    
    return true;
  }

  bool PMGCrossSectionTool::readInfosFromDir(const std::string& inputDir)
  {

    TString mydir = inputDir;
    gSystem->ExpandPathName (mydir);
    void *dirp = 0;
  
    std::vector<std::string> inFiles={};
  
    try
      {
	dirp = gSystem->OpenDirectory (mydir.Data());
	const char *file = 0;
	while ((file = gSystem->GetDirEntry (dirp)))
	  {
	    std::string myfile = inputDir + "/" + file;
	    if (myfile.size() > 4 && myfile.substr (myfile.size()-4) == ".txt")
	      inFiles.push_back(myfile);
	  }
	gSystem->FreeDirectory (dirp);
      }
    catch (...)
      {
	gSystem->FreeDirectory (dirp);
	//throw;
	return false;
      }
  
    readInfosFromFiles(inFiles);
    return true;
  }


  double PMGCrossSectionTool::getFilterEff(const int dsid) const
  {
  
    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.second.dsid)
	return info.second.filterEff;
    }
  
    std::cout << "ERROR::getFilterEff --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;
  
    return -1;
  
  }


  std::string PMGCrossSectionTool::getSampleName(const int dsid) const
  {
  
    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.second.dsid)
	return info.second.containerName;
    }
  
    std::cout << "ERROR::getSampleName --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;
    
    return "";
    
  }


  double PMGCrossSectionTool::getGeneratorXsection(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.second.dsid)
	return info.second.genXsec;
    }

    std::cout << "ERROR::getGeneratorXsection --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;
  
    return -1;

  } 

  double PMGCrossSectionTool::getKfactor(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.second.dsid)
	return info.second.kFactor;
    }

    std::cout << "ERROR::getKfactor --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;

    return -1;

  }

  double PMGCrossSectionTool::getSampleXsection(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.second.dsid)
	return info.second.genXsec * info.second.kFactor * info.second.filterEff;
    }

    std::cout << "ERROR::getSampleXsection --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;

    return -1;

  }

  std::vector<int> PMGCrossSectionTool::getLoadedDSIDs() const {
    std::vector<int> dsids;
    for (const auto& info : fStoreSampleInfo){
      dsids.push_back(info.second.dsid);
    }
    return dsids;
  }

}
