//This is an example ROOT macro for running PRW
//If you want me to help you debug a problem, please 
//copy this macro and modify it so that it reproduces your issue
//please locate all the input files on public (afs) space

//execute with: root debugExample.C

#define GAUDI_V20_COMPAT
#include "GaudiKernel/ToolHandle.h"

void debugExample() {
  
  POOL::TEvent evt(POOL::TEvent::kClassAccess);

  evt.readFrom("$ASG_TEST_FILE_MC"); //replace with your input file

  ToolHandle<CP::IPileupReweightingTool> prw("CP::PileupReweightingTool/tool");

  //update the following two lines to point to your config and lumicalc files
  //(if they are not in the calib area)
  AAH::setProperty( prw, "ConfigFiles" , "['dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root']");
  AAH::setProperty( prw, "LumiCalcFiles" , "['GoodRunsLists/data15_13TeV/20160720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-005.root','GoodRunsLists/data16_13TeV/20170215/physics_25ns_20.7.lumicalc.OflLumi-13TeV-008.root']");

  prw.retrieve(); //initializes the tool


  evt.getEntry(0); //update to go to the problematic event;

  const xAOD::EventInfo_v1* ei = 0;
  evt.retrieve( ei );

  prw->getCombinedWeight( *ei ); //change this to reproduce your problem
		   
  


}
