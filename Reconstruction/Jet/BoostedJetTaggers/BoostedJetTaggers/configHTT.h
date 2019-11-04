// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////
/// This header is an example to demonstrate how to configure & use the HEPTopTagger
/// within RootCore.
/// Typically the usage is in 3 steps :
/// 1)
/// JetRecTool* m_fullJetTool; // declare a JetRecTool (ex : as a class/tool/alg member)
/// 2)
/// m_fullJetTool = buildFullHTTagger(); // in an initialize step
///
/// 3)
///   m_fullJetTool->execute(); // in an execute() step, once per event.
/// --> this will produce a CamKt15HTTagged JetContainer (possibly empty) in the TStore.
/// Don't forget to clear the TStore, if your framework doesn't do it for you.
/////////////////////////////////////////////////////////

#include "BoostedJetTaggers/HEPTopTagger.h"

#include "JetInterface/IJetModifier.h"
#include "JetRec/JetRecTool.h"
#include "JetRec/PseudoJetGetter.h"
#include "JetRec/JetFromPseudojet.h"
#include "JetRec/JetFinder.h"
#include "JetCalibTools/JetCalibrationTool.h"
#include "xAODRootAccess/TStore.h"

using namespace HTTxAOD;
using asg::ToolStore;

#define HTT_CHECK( sc ) if(sc.isFailure()) {std::cout<< "ConfigHTT  ERROR   StatusCode failure. Returning a null pointer"<< std::endl; return NULL;} 

//MSG::Level myMSGlev = MSG::DEBUG;
namespace ConfigHTT {
  MSG::Level myMSGlev = MSG::INFO;
  
  void configMsgLevl(MSG::Level l){myMSGlev = l;}

}

IJetCalibrationTool* calibTool(const std::string & alg, const std::string & radius, const std::string & input){
  
  TString jetAlgo = alg.c_str(); //String describing your jet collection, for example AntiKt4TopoEM or AntiKt4LCTopo
  //jetAlgo+= int(r*10) ; jetAlgo+= input.c_str();
  jetAlgo+= radius.c_str() ; // CA calib  
  jetAlgo+= input.c_str();  

  TString config = "CamKt_JES_HTT.config"; //Path to global config used to initialize the tool
  TString calibSeq = "Origin_EtaJES" ; //String describing the calibration sequence to apply (see below)
  bool isData = false ; //bool describing if the events are data or from simulation

  const std::string name = "HTTSubjetcalib"; 
  //Call the constructor. The default constructor can also be used if the arguments are set with python configuration instead
  //  JetCalibrationTool *myJES = new JetCalibrationTool(name, jetAlgo, config, calibSeq, isData);
  JetCalibrationTool *myJES = new JetCalibrationTool(name);
  myJES->setProperty("JetCollection", jetAlgo);
  myJES->setProperty("ConfigFile", config);
  myJES->setProperty("CalibSequence", calibSeq);
  myJES->setProperty("IsData", isData);
  //Initialize the tool
  HTT_CHECK(myJES->initializeTool(name));
  return myJES;
}


HEPTopTagger * configHTTTool(){

  JetFromPseudojet* jetFromPJ = new JetFromPseudojet("jetbuild");
  //ToolStore::put(jetFromPJ);
  // NEEDED when calibrating !
  std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
  //jetFromPJ->setProperty("Attributes", areatts);
  //jetFromPJ->msg().setLevel(MSG::VERBOSE);
  HTT_CHECK(jetFromPJ->initialize());

  
  HEPTopTagger * htt  = new HEPTopTagger("TheHTT");
  //ToolStore::put(htt);
  
  HTT_CHECK(htt->setProperty("PreDefSettings", "normal"));
  HTT_CHECK(htt->setProperty("TopContName" , "CamKt15HTTagged"));
  HTT_CHECK(htt->setProperty("TopCandidateLink", "HTTnormal" ) );
  HTT_CHECK(htt->setProperty("CalibrateSubJets", true));
  HTT_CHECK(htt->setProperty("JetFromPseudoJetTool", ToolHandle<IJetFromPseudojet>(jetFromPJ)));
  HTT_CHECK(htt->setProperty("MaxFatJetEta", 5.));

  ToolHandleArray<IJetCalibrationTool> calibArray;  
  ToolHandleArray<IJetCalibrationTool> calibArrayCA15;

  IJetCalibrationTool* cal02 = calibTool("CamKt", "020", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal02));
  IJetCalibrationTool* cal025 = calibTool("CamKt", "025", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal025));

  IJetCalibrationTool* cal03 = calibTool("CamKt", "030", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal03));
  IJetCalibrationTool* cal035 = calibTool("CamKt", "035", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal035));

  IJetCalibrationTool* cal04 = calibTool("CamKt", "040", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal04));
  IJetCalibrationTool* cal045 = calibTool("CamKt", "045", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal045));

  IJetCalibrationTool* cal05 = calibTool("CamKt", "050", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal05));
  IJetCalibrationTool* cal055 = calibTool("CamKt", "055", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal055));

  IJetCalibrationTool* cal06 = calibTool("CamKt", "060", "LCTopo");
  calibArray.push_back(ToolHandle<IJetCalibrationTool>(cal06));

  IJetCalibrationTool* cal15 = calibTool("CamKt", "150", "LCTopo");
  calibArrayCA15.push_back(ToolHandle<IJetCalibrationTool>(cal15));
                       
  HTT_CHECK(htt->setProperty("CalibrationTools", calibArray));
  HTT_CHECK(htt->setProperty("CalibrationToolsCA15", calibArrayCA15));
  
  htt->msg().setLevel(ConfigHTT::myMSGlev);



  HTT_CHECK(htt->initialize());
  return htt;
}



JetRecTool * buildFullHTTagger(){

  ToolHandleArray<IJetExecuteTool> handleExec;

  ToolHandleArray<IPseudoJetGetter> getterArray;
  // Create a PseudoJet builder.
  std::cout << "Creating pseudojet builder." << std::endl;
  PseudoJetGetter* lcgetter = new PseudoJetGetter("lcget");
  //ToolStore::put(lcgetter);
  HTT_CHECK(lcgetter->setProperty("InputContainer", "CaloCalTopoClusters"));
  HTT_CHECK(lcgetter->setProperty("OutputContainer", "PseudoJetLCTopo"));
  HTT_CHECK(lcgetter->setProperty("Label", "LCTopo"));
  HTT_CHECK(lcgetter->setProperty("SkipNegativeEnergy", true));
  HTT_CHECK(lcgetter->setProperty("GhostScale", 0.0));
  HTT_CHECK(lcgetter->initialize());

  //ToolHandle<IPseudoJetGetter> hlcget(lcgetter);
  getterArray.push_back( ToolHandle<IPseudoJetGetter>(lcgetter) );




  JetFromPseudojet* jetFromPJ = new JetFromPseudojet("jetbuild");
  //ToolStore::put(jetFromPJ);
  std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
  //jetFromPJ->setProperty("Attributes", areatts);
  jetFromPJ->msg().setLevel(MSG::ERROR);
  //jetFromPJ->msg().setLevel(MSG::VERBOSE);

  HTT_CHECK(jetFromPJ->initialize());

  std::cout << "Creating jet finder." << std::endl;
  JetFinder* finder = new JetFinder("CamKt15Finder");
  //ToolStore::put(finder);
  HTT_CHECK(finder->setProperty("JetAlgorithm", "CamKt"));
  HTT_CHECK(finder->setProperty("JetRadius", 1.5));
  HTT_CHECK(finder->setProperty("PtMin", 50000.0));
  HTT_CHECK(finder->setProperty("GhostArea", 0.0));
  HTT_CHECK(finder->setProperty("RandomOption", 1));
  HTT_CHECK(finder->setProperty("JetBuilder", ToolHandle<IJetFromPseudojet>(jetFromPJ)));

  finder->msg().setLevel(ConfigHTT::myMSGlev);
  HTT_CHECK(finder->initialize());

  // Create list of modifiers.
  ToolHandleArray<IJetModifier> modArray;
  modArray.push_back( ToolHandle<IJetModifier>( configHTTTool() ) );

  std::cout << "Creating jetrec tool." << std::endl;
  JetRecTool* fullJetTool = new JetRecTool("FullJetRecTool");
  HTT_CHECK(fullJetTool->setProperty("OutputContainer", "CamKt15LCTopoJets_HTT"));
  HTT_CHECK(fullJetTool->setProperty("PseudoJetGetters", getterArray));
  HTT_CHECK(fullJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(finder)));
  HTT_CHECK(fullJetTool->setProperty("JetModifiers", modArray));

  fullJetTool->msg().setLevel(ConfigHTT::myMSGlev);
  HTT_CHECK(fullJetTool->initialize());
  

  
  return fullJetTool;
}

#undef HTT_CHECK
