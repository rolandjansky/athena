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

#include <map>

using namespace HTTxAOD;
using asg::ToolStore;

#define HTT_CHECK( sc ) if(sc.isFailure()) {std::cout<< "ConfigHTT  ERROR   StatusCode failure. Returning a null pointer"<< std::endl; return NULL;} 

namespace ConfigHTT {
  //MSG::Level myMSGlev = MSG::VERBOSE;
  //MSG::Level myMSGlev = MSG::DEBUG;
  MSG::Level myMSGlev = MSG::INFO;

  void configMsgLevl(MSG::Level l){myMSGlev = l;}

}

std::map < TString, IJetCalibrationTool* > caCalibTools;
JetFromPseudojet* jetFromPJ = nullptr;

IJetCalibrationTool* calibTool(const std::string & alg, const std::string & radius, const std::string & input, bool debug = true){
  
  TString jetAlgo = alg.c_str(); //String describing your jet collection, for example AntiKt4TopoEM or AntiKt4LCTopo
  jetAlgo+= radius.c_str() ; // CA calib  
  jetAlgo+= input.c_str();

  if( caCalibTools.find(jetAlgo) != caCalibTools.end() ) {
    if( debug )
      std::cout<<"Use pre-initialized calibration tool: "<< jetAlgo <<std::endl;
    return caCalibTools.at(jetAlgo);
  } else {
    TString config = "CamKt_JES_HTT.config"; //Path to global config used to initialize the tool
    TString calibSeq = "Origin_EtaJES" ; //String describing the calibration sequence to apply (see below)
    bool isData = false ; //bool describing if the events are data or from simulation

    const std::string name = "HTTSubjetcalib"+alg+radius; 
    //Call the constructor. The default constructor can also be used if the arguments are set with python configuration instead
    //JetCalibrationTool *myJES = new JetCalibrationTool(name, jetAlgo, config, calibSeq, isData);
    JetCalibrationTool *myJES = new JetCalibrationTool(name);
    myJES->setProperty("JetCollection", jetAlgo);
    myJES->setProperty("ConfigFile", config);
    myJES->setProperty("CalibSequence", calibSeq);
    myJES->setProperty("IsData", isData);
    //Initialize the tool
    HTT_CHECK(myJES->initializeTool(name));
    
    caCalibTools[jetAlgo] = myJES;
    return myJES;
  }
}


HEPTopTagger * configHTTTool(const std::string & treename="",const std::string & HTTconfig="", bool calibrateFatJet=true, const std::string& CAGroomedName = "SetMe"){
  if( jetFromPJ == nullptr) {
    jetFromPJ = new JetFromPseudojet("jetbuild");
    //ToolStore::put(jetFromPJ);
    // NEEDED when calibrating !
    std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
    //jetFromPJ->setProperty("Attributes", areatts);
    //jetFromPJ->msg().setLevel(MSG::VERBOSE); 
    HTT_CHECK(jetFromPJ->initialize());
  } else {
    std::cout<<"Use pre-initialized tool: jetbuild"<<std::endl;
  }


  TString CAJetsHTTtagged_container = "CamKt15HTTagged";

  if( !treename.empty() ){ 
    CAJetsHTTtagged_container+= "_"; 
    CAJetsHTTtagged_container += treename;
  }
  if( !HTTconfig.empty() ){
    CAJetsHTTtagged_container+= "_"; 
    CAJetsHTTtagged_container+= HTTconfig; 
  }
  
  const char * HTT_name = Form("TheHTT_%s", HTTconfig.c_str() ) ;
  const char * CAJets_HTTtagged = CAJetsHTTtagged_container ;
  const char * topCandidateLink = Form("HTT_%s", HTTconfig.c_str() ) ;
  const char * HTT_settings = HTTconfig.c_str();   

  HEPTopTagger * htt  = new HEPTopTagger(HTT_name);
  //ToolStore::put(htt);


  HTT_CHECK(htt->setProperty("PreDefSettings",HTT_settings));
  HTT_CHECK(htt->setProperty("TopContName" , CAJets_HTTtagged ));
  HTT_CHECK(htt->setProperty("TopCandidateLink",  topCandidateLink));
  HTT_CHECK(htt->setProperty("CalibrateSubJets", true));
  HTT_CHECK(htt->setProperty("JetFromPseudoJetTool", ToolHandle<IJetFromPseudojet>(jetFromPJ)));
  HTT_CHECK(htt->setProperty("MaxFatJetEta", 2.));
  HTT_CHECK(htt->setProperty("MinCandPt", 150.));
  HTT_CHECK(htt->setProperty("FiltContainerName", CAGroomedName.c_str()));
  HTT_CHECK(htt->setProperty("LowerMassCut", 20e3));
  HTT_CHECK(htt->setProperty("UpperMassCut", 250e3));

  ToolHandleArray<IJetCalibrationTool> calibArray;

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


  HTT_CHECK(htt->setProperty("CalibrationTools", calibArray));

  if(calibrateFatJet){
    ToolHandleArray<IJetCalibrationTool> calibArrayCA15;
    IJetCalibrationTool* cal15 = calibTool("CamKt", "150", "LCTopo");
    calibArrayCA15.push_back(ToolHandle<IJetCalibrationTool>(cal15));
    HTT_CHECK(htt->setProperty("CalibrationToolsCA15", calibArrayCA15));
  }

  htt->msg().setLevel(ConfigHTT::myMSGlev);

  HTT_CHECK(htt->initialize());
  return htt;
}


JetRecTool * buildFullHTTagger(const std::string & treename="",
  const std::string & outputContainerName="CA15LCTopoJets_HTT",
  const std::vector< std::string > &HTTconfigs=std::vector< std::string >() ){

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

  if( jetFromPJ == nullptr) {
    jetFromPJ = new JetFromPseudojet("jetbuild");
    //ToolStore::put(jetFromPJ);
    std::vector<std::string> areatts = {"ActiveArea", "ActiveAreaFourVector"};
    //jetFromPJ->setProperty("Attributes", areatts);
    jetFromPJ->msg().setLevel(MSG::ERROR);
    //jetFromPJ->msg().setLevel(MSG::VERBOSE);
    HTT_CHECK(jetFromPJ->initialize());
  } else {
    std::cout<<"Use pre-initialized tool: jetbuild"<<std::endl;
  }

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
  bool calibrateFatJet = true;
  if(HTTconfigs.size() != 0){
    for(auto config : HTTconfigs){
      modArray.push_back( ToolHandle<IJetModifier>( configHTTTool( treename, config, calibrateFatJet, outputContainerName + "GroomedFatJet" ) ) );
      calibrateFatJet = false; // only calibrate fatjet inside HEPTopTagger.cxx once...
    }
  }
  else
    modArray.push_back( ToolHandle<IJetModifier>( configHTTTool( treename, "",  calibrateFatJet, outputContainerName + "GroomedFatJet" ) ) );

  TString CAJets_HTT_container = outputContainerName;
  if( !treename.empty() ){
    CAJets_HTT_container+= "_"; 
    CAJets_HTT_container += treename;
  }
  const char * CAJets_HTT = CAJets_HTT_container ;


  std::cout << "Creating jetrec tool." << std::endl;
  JetRecTool* fullJetTool = new JetRecTool("FullJetRecTool");
  std::cout << "container string " << CAJets_HTT << std::endl;
  HTT_CHECK(fullJetTool->setProperty("OutputContainer", CAJets_HTT ));
  HTT_CHECK(fullJetTool->setProperty("PseudoJetGetters", getterArray));
  HTT_CHECK(fullJetTool->setProperty("JetFinder", ToolHandle<IJetFinder>(finder)));
  HTT_CHECK(fullJetTool->setProperty("JetModifiers", modArray));

  fullJetTool->msg().setLevel(ConfigHTT::myMSGlev);
  HTT_CHECK(fullJetTool->initialize());

  std::cout << "ok fullJetTool " << std::endl;

  return fullJetTool;
}

#undef HTT_CHECK
