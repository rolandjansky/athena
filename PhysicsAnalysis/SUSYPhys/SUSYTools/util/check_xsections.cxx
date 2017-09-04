/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SUSYTools/SUSYCrossSection.h"
#include "SUSYTools/SUSYCrossSectionPMG.h"

// For PMG centralized tool's wrapper tool handles
#include "AsgTools/AnaToolHandle.h"
#include "PMGAnalysisInterfaces/IPMGCrossSectionTool.h"

#include <string>
#include <iostream>

#include <TSystem.h>
#include <TString.h>
#include <TError.h>

int main( int argc, char* argv[] ) {

  if(argc < 2){
    ::Error("main()","Please provide a DSID to be checked!");
    return 0;
  }

  bool valfile = false;
  bool genmode = false;
  if(argc > 2){
    if(atoi(argv[2]) == 1){
      std::cout << "--- File Validation Mode ----" << std::endl;
      valfile = true;
    }
    else if(atoi(argv[2]) == 2){
      std::cout << "--- Generator info Mode ----" << std::endl;
      genmode = true;
    }
  }
  if(!valfile)
    std::cout << "--- Wrapper Validation Mode ----" << std::endl;

  //*** Tools
  //SUSYTools
  SUSY::CrossSectionDB *my_XsecDB(0);
  my_XsecDB = new SUSY::CrossSectionDB(gSystem->ExpandPathName("$ROOTCOREBIN/data/SUSYTools/mc15_13TeV/"));
  
  //PMG tool
  asg::AnaToolHandle<PMGTools::IPMGCrossSectionTool> pmgxs;
  pmgxs.setTypeAndName("PMGTools::PMGCrossSectionTool/PMGCrossSectionTool");
  pmgxs.retrieve().ignore(); // Ignore the status code
  pmgxs->readInfosFromDir(gSystem->ExpandPathName("$ROOTCOREBIN/data/PMGTools/"));

  //STPMG wrapper
  SUSY::CrossSectionDBPMG *my_XsecDBPMG(0);
  my_XsecDBPMG = new SUSY::CrossSectionDBPMG(gSystem->ExpandPathName("$ROOTCOREBIN/data/PMGTools/"));




  //--- Validation Wrapper Mode 
  //*** check settings for a given sample as from SUSYTools, PMG, and SUSYToolsPMG wrapper
  if(!valfile){

    int dsid = atoi( argv[1] );

    //ST
    std::cout << "ST_CrossSection name   : " << my_XsecDB->name(dsid) << std::endl;
    std::cout << "ST_CrossSection xs     : " << my_XsecDB->xsectTimesEff(dsid) << std::endl;
    std::cout << "ST_CrossSection raw    : " << my_XsecDB->rawxsect(dsid) << std::endl;
    std::cout << "ST_CrossSection k      : " << my_XsecDB->kfactor(dsid) << std::endl;
    std::cout << "ST_CrossSection eff    : " << my_XsecDB->efficiency(dsid) << std::endl;
    
    //PMG
    std::cout << "PMG_CrossSection name  : " << pmgxs->getSampleName(dsid) << std::endl;
    std::cout << "PMG_CrossSection xs    : " << pmgxs->getSampleXsection(dsid) << std::endl;
    std::cout << "PMG_CrossSection (AMI) : " << pmgxs->getAMIXsection(dsid) << std::endl;
    std::cout << "PMG_CrossSection (k)   : " << pmgxs->getKfactor(dsid) << std::endl;
    std::cout << "PMG_CrossSection (eff) : " << pmgxs->getFilterEff(dsid) << std::endl;
    
    //ST-PMG Wrapper
    std::cout << "STPMG_CrossSection name : " << my_XsecDBPMG->name(dsid) << std::endl;
    std::cout << "STPMG_CrossSection xs   : " << my_XsecDBPMG->xsectTimesEff(dsid) << std::endl;
    std::cout << "STPMG_CrossSection raw  : " << my_XsecDBPMG->rawxsect(dsid) << std::endl;
    std::cout << "STPMG_CrossSection k    : " << my_XsecDBPMG->kfactor(dsid) << std::endl;
    std::cout << "STPMG_CrossSection eff  : " << my_XsecDBPMG->efficiency(dsid) << std::endl;
    
  }

  else if (genmode){
    std::vector<int> ids = pmgxs->getLoadedDSIDs();
    for(auto id : ids){
      std::string genname="SHOWER";
      std::cout << id << "\t"
		<< genname << std::endl;
    }
  }

  else if (valfile){
    //--- Validation File Mode 
    //*** compare settings for all samples in provided file as from PMG vs SUSYTools
    
    std::vector<int> ids = pmgxs->getLoadedDSIDs();

    for(auto id : ids){

      //check if in SUSYTools 
      if(my_XsecDB->xsectTimesEff(id) < 0) continue;

      TString cmd = Form("grep \"%d\" PMGTools/data/*.txt | cut -d: -f1 >> PMG_id_file.txt", id);
      gSystem->Exec(cmd);

      std::string shortname = pmgxs->getSampleName(id).erase(0,18);
      std::string delim="\t";
      //std::string delim=","; // csv format
      std::cout << id << delim
		<< shortname << delim
		<< my_XsecDB->xsectTimesEff(id) << delim
		<< pmgxs->getSampleXsection(id) << delim
		<< my_XsecDB->rawxsect(id) <<  delim
		<< pmgxs->getAMIXsection(id) << delim
		<< my_XsecDB->kfactor(id) <<  delim
		<< pmgxs->getKfactor(id) <<  delim
		<< my_XsecDB->efficiency(id) << delim
		<< pmgxs->getFilterEff(id) * pmgxs->getBR(id) << std::endl;
    }
  }

  
  return 0;
}
