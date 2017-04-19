/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: PMGCrossSectionTool.cxx 764400 2016-07-26 17:47:39Z tripiana $

#include <fstream>
#include <sstream>
#include <string>
#include <TSystem.h>
#include <TString.h>
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

      std::string line;
      while (currentFile.good()) {

	getline(currentFile, line);
      
	std::istringstream input_line(line);
	std::string str_dsid;
	std::string containerName;
	std::string str_amiXsec;
	std::string str_br;
	std::string str_filterEff;
	std::string str_hoXsec;
	std::string str_kFac;
	std::string str_hoSampleXsec;
      
	input_line >> str_dsid;
	input_line >> containerName;
	input_line >> str_amiXsec;
	input_line >> str_br;
	input_line >> str_filterEff;
	input_line >> str_hoXsec;
	input_line >> str_kFac;
	input_line >> str_hoSampleXsec; // this is hoXsec * filter eff

	int dsid = TString(str_dsid).Atoi();
	double BR           = TString(str_br).Atof();
	double kFactor      = TString(str_kFac).Atof(); 
	double filterEff    = TString(str_filterEff).Atof(); 
	double hoXsec       = TString(str_hoXsec).Atof();
	double hoSampleXsec = TString(str_hoSampleXsec).Atof();
	double amiXsec      = TString(str_amiXsec).Atof();
           
	AllSampleInfo help;
	help.dsid = dsid;
	help.containerName = containerName;
	help.br = BR;
	help.amiXSec = amiXsec;
	help.filterEff = filterEff;
	help.higherOrderXsecTotal = hoXsec;
	help.kFactor = kFactor;
	help.higherOrderXsecSample = hoSampleXsec;
      
	fStoreSampleInfo.push_back(help);
      
      }
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
      if(dsid == info.dsid)
	return info.filterEff;
    }
  
    std::cout << "ERROR::getFilterEff --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;
  
    return -1;
  
  }


  std::string PMGCrossSectionTool::getSampleName(const int dsid) const
  {
  
    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.dsid)
	return info.containerName;
    }
  
    std::cout << "ERROR::getSampleName --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;
    
    return "";
    
  }


  double PMGCrossSectionTool::getAMIXsection(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.dsid)
	return info.amiXSec;
    }

    std::cout << "ERROR::getAMIXsection --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;
  
    return -1;

  } 

  double PMGCrossSectionTool::getBR(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.dsid)
	return info.br;
    }

    std::cout << "ERROR::getBR --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;

    return -1;

  }

  double PMGCrossSectionTool::getKfactor(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.dsid)
	return info.kFactor;
    }

    std::cout << "ERROR::getKfactor --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;

    return -1;

  }

  double PMGCrossSectionTool::getSampleXsection(const int dsid) const
  {

    for (const auto& info : fStoreSampleInfo){
      if(dsid == info.dsid)
	return info.higherOrderXsecSample;
    }

    std::cout << "ERROR::getSampleXsection --> Sample with DSID " << dsid << " has no info stored!!! ---> EXIT." << std::endl;

    return -1;

  }

  std::vector<int> PMGCrossSectionTool::getLoadedDSIDs() const {
    std::vector<int> dsids;
    for (const auto& info : fStoreSampleInfo){
      dsids.push_back(info.dsid);
    }
    return dsids;
  }

}
