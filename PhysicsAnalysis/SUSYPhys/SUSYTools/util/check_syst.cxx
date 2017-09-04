/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: check_syst.cxx tripiana $

// System include(s):
#include <string>
#include <iostream>

// ROOT include(s):
#include <TError.h>
#include <TString.h>
#include <TSystem.h>

// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

// Other includes
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"
#include "PATInterfaces/CorrectionCode.h"

std::string getFW(std::string in, unsigned int width=30){
  for(unsigned int i=in.size() ; i < width; i++)
    in+=" ";
  return in;
}

int main( int argc, char* argv[] ) {

  using namespace asg::msgUserCode;
  ANA_CHECK_SET_TYPE (int);

  StatusCode::enableFailure();
  CP::SystematicCode::enableFailure();
  CP::CorrectionCode::enableFailure();

  // The application's name:
  const char* APP_NAME = argv[0];

  std::string config_file = getenv("ROOTCOREBIN");
  config_file += "/data/SUSYTools/SUSYTools_Default.conf";

  // Check if we received a config file
  if ( argc > 1 ){
    config_file = argv[1];
  }

  // Create the tool(s) to test:
  ST::SUSYObjDef_xAOD objTool("SUSYObjDef_xAOD");

  ///////////////////////////////////////////////////////////////////////////////////////////
  // Configure the SUSYObjDef instance
  ANA_CHECK( objTool.setProperty("DataSource", ST::ISUSYObjDef_xAODTool::FullSim) );
  if(!config_file.empty())
    ANA_CHECK( objTool.setProperty("ConfigFile", config_file) );

  std::vector<std::string> prw_conf;
  prw_conf.push_back("dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root");
  prw_conf.push_back("dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root");

  ANA_CHECK( objTool.setProperty("PRWConfigFiles", prw_conf) );
  std::vector<std::string> prw_lumicalc;
  prw_lumicalc.push_back("dev/SUSYTools/ilumicalc_histograms_None_276262-284154_IBLOFF.root");
  prw_lumicalc.push_back("dev/SUSYTools/ilumicalc_histograms_None_297730-299243.root");
  ANA_CHECK( objTool.setProperty("PRWLumiCalcFiles", prw_lumicalc) );
  ///////////////////////////////////////////////////////////////////////////////////////////

  objTool.msg().setLevel( MSG::FATAL );

  if ( objTool.initialize() != StatusCode::SUCCESS) {
    Error( APP_NAME, "Cannot intialize SUSYObjDef_xAOD..." );
    Error( APP_NAME, "Exiting... " );
    exit(-1);
  } else {
    Info( APP_NAME, "SUSYObjDef_xAOD initialized... " );
  }

  // Counter for cuts:
  std::vector<ST::SystInfo> systInfoList;
  systInfoList = objTool.getSystInfoList();

  Info( APP_NAME, "\n\n");
  Info( APP_NAME, "---- SYSTEMATICS LIST -----------------------------------------------------------------------------------------\n");

  auto isys = 0;
  for (const auto& sysInfo : systInfoList) {
    const CP::SystematicSet& sys = sysInfo.systset;
    if(sys.name()=="") continue;
    Info( APP_NAME, "      %s\t  Affects : %s  (%s)", 
                getFW(sys.name(),45).c_str(), 
                getFW(ST::testAffectsObject(sysInfo.affectsType),12).c_str(), 
                sysInfo.affectsKinematics ? "Kin." : "Weight");
    //Info( APP_NAME, ("      "+getFW(sys.name(),45)+"\t  Affects : "+getFW(ST::testAffectsObject(sysInfo.affectsType),12)+"  ("+(sysInfo.affectsKinematics ? "Kin.)" : "Weight)")).c_str() );
    ++isys;
  }

  Info( APP_NAME, "---------------------------------------------------------------------------------------------------------------\n");

  // Return gracefully:
  return 0;
}
