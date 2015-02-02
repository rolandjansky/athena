/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ROOTCORE
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"
#endif

// FrameWork includes                                                                                                                                          
#include "AsgTools/ToolHandle.h"
#include "AsgTools/AsgTool.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"

#include "assert.h"
#include "TFile.h"

#define private public //ugly hacking way to do this...
#define protected public
#include "METUtilities/METSystematicsTool.h"


static std::string const toolname = "tool";

bool testDynamicCast(){std::cout << __PRETTY_FUNCTION__ << std::endl;

  xAOD::MissingET * testMET = new xAOD::MissingET("myMET");

  const xAOD::IParticleContainer* cont =   dynamic_cast<const xAOD::IParticleContainer*>(testMET->container());
  assert(cont == nullptr );
  const xAOD::MissingETContainer* mcont =  dynamic_cast<const xAOD::MissingETContainer*>(cont);
  assert(mcont == nullptr);
  const xAOD::MissingETContainer* cont2 =  dynamic_cast<const xAOD::MissingETContainer*>(testMET->container());
  assert(cont2 == nullptr);

  delete testMET;

  return true;
}

bool testAddObjToCont(){std::cout << __PRETTY_FUNCTION__ << std::endl;

  xAOD::MissingETContainer * testMETcont  = new xAOD::MissingETContainer();
  assert(testMETcont != nullptr);//passes

  xAOD::MissingET * myObj  =  new xAOD::MissingET("myMETobj");  
  testMETcont->push_back(myObj);
  assert(testMETcont->at(0) == myObj);//passes
  assert(testMETcont->size() == 1); //one element passes


  const xAOD::MissingETContainer* cont =   dynamic_cast<const xAOD::MissingETContainer*>(myObj->container());
  //  dynamic_cast<const xAOD::IParticleContainer*>(myObj->container());
  assert(cont != nullptr );
  std::cout << "checked that the cont isn't empty" << std::endl;
  assert(cont == testMETcont);
  std::cout << "checked the cast worked, and we have the same final address as original" << std::endl;
  assert(*cont == *testMETcont);
  std::cout << "checked the cast worked, and we have the same final object as original" << std::endl;

  delete myObj;

  return true;
}


bool testDefaultHistosFilled(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  assert(tool.shiftpara_pthard_njet_mu!=nullptr);
  assert(tool.resopara_pthard_njet_mu !=nullptr);
  assert(tool.resoperp_pthard_njet_mu !=nullptr);
  assert(tool.jet_systRpt_pt_eta      ==nullptr);

  return true;
}

bool testJetTrkHistosFilled(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  tool.setProperty("ConfigJetTrkFile" ,"METTrack_2012.config");
  assert(tool.initialize());

  assert(tool.shiftpara_pthard_njet_mu!=nullptr);
  assert(tool.resopara_pthard_njet_mu !=nullptr);
  assert(tool.resoperp_pthard_njet_mu !=nullptr);
  assert(tool.jet_systRpt_pt_eta      !=nullptr);

  return true;
}


bool testNoCollection(){std::cout << __PRETTY_FUNCTION__ << std::endl;//test source checking
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());
  xAOD::MissingET softMET("softMet", MissingETBase::Source::softEvent());
  xAOD::MissingET softMETcopy(softMET);

  assert( ! tool.applyCorrection(softMET)) ; //this should fail because the softMET isn't owned by a collection, so we can't applyCorrection on it
  // assert(scaledMET.mpx() != 0);
  // assert(scaledMET.mpy() != 0);
  // assert(scaledMET.sumet() != 0);
  assert(softMET == softMETcopy);


  return true;
}

bool testNonSoftTermFailure(){std::cout << __PRETTY_FUNCTION__ << std::endl;//test source checking

  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  xAOD::MissingET jetMET("jetMet", MissingETBase::Source::jet());

  assert( !tool.applyCorrection(jetMET));//we fail because our object isn't owned by a collection
  //assert(scaledMET.mpx() == 0);
  // assert(scaledMET.mpy() == 0);
  // assert(scaledMET.sumet() == 0);
  //assert(jetMET.source() == MissingETBase::Source::unknown());


  return true;
}

bool testSoftTermSuccess(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  std::cout << "creating test containers" << std::endl;
  xAOD::MissingETContainer * testMETcont  = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux ); 
  assert(testMETcont != nullptr);//passes

  std::cout << "creating test object" << std::endl;
  xAOD::MissingET * myObj  =  new xAOD::MissingET("myMETobj", MissingETBase::Source::softEvent());
  testMETcont->push_back(myObj);
  assert(testMETcont->at(0) == myObj);//passes
  assert(testMETcont->size() == 1); //one element passes       

  std::cout << "applying correction" << std::endl;
  assert( tool.applyCorrection(*myObj));//this time, we should pass because our object is owned

  delete testMETcont;

  return true;
}

bool testRetrieveMETTerms(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  xAOD::MissingETContainer const * cont = nullptr;

  if( ! tool.evtStore()->retrieve(cont,"MET_RefFinal") )  return true;

  const xAOD::MissingET* refgamma = (*cont)["RefGamma"];

  assert(refgamma != nullptr);//we can get met terms out

  return true;
}


bool testAddAffectingSystematic(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  CP::SystematicSet recSys =  tool.recommendedSystematics();

  std::cout << "sys set" << recSys.name() << std::endl;

  CP::SystematicVariation test("testSystematic");//recommendedSystematic
  assert( !tool.isAffectedBySystematic(test));

  std::cout << "test set " << test.name() << std::endl;
  assert(  tool.addAffectingSystematic(test , true /*recommended */) );

  std::cout << "sys set 2 " << tool.recommendedSystematics().name() << std::endl;
  assert( tool.isAffectedBySystematic(test));

  
  return true;
}

bool testProjectST(){std::cout << __PRETTY_FUNCTION__  << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  xAOD::MissingET yaxisSoftTerm (0., 15., 50.);
  xAOD::MissingET xaxisHardTerm (10., 0., 50.);

  xAOD::MissingET proj = tool.projectST(yaxisSoftTerm, xaxisHardTerm);

  std::cout << "projection "<< proj.mpx() << ", " << proj.mpy()  << "," << proj.sumet() << std::endl;
  assert(proj.mpx() == yaxisSoftTerm.mpx() &&
	 proj.mpy() == (-1)*yaxisSoftTerm.mpy() && //in this case, our "projection" will do a reflection over the x (ptHard) axis
	 proj.sumet()== yaxisSoftTerm.sumet() 
	 );

  xAOD::MissingET projectBack = tool.projectST(proj, xaxisHardTerm);

  assert(projectBack == yaxisSoftTerm);
  // assert(projectBack.mpx() == yaxisSoftTerm.mpx() &&
  //        projectBack.mpy() == yaxisSoftTerm.mpy() && //reflect back
  //        projectBack.sumet()== yaxisSoftTerm.sumet()
  // 	 );

  return true;

}

bool testProjectST2(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  xAOD::MissingET yaxisSoftTerm (0., 15., 50.);
  xAOD::MissingET xaxisHardTerm (10., 0., 50.);

  xAOD::MissingET proj = tool.softTrkSyst_scale(yaxisSoftTerm, xaxisHardTerm, 0. );
  std::cout << "projection: "<< proj.mpx() << ", " << proj.mpy()  << "," << proj.sumet() << std::endl;
  assert(proj == yaxisSoftTerm);
  // assert(proj.mpx() == yaxisSoftTerm.mpx()     &&
  // 	 proj.mpy() == yaxisSoftTerm.mpy()     &&
  // 	 proj.sumet()== yaxisSoftTerm.sumet() 
  // 	 );

  double shift = 1.;
  proj = tool.softTrkSyst_scale(yaxisSoftTerm, xaxisHardTerm, shift); //
  std::cout << "projection: "<< proj.mpx() << ", " << proj.mpy()  << "," << proj.sumet() << std::endl;
  assert(proj.mpx()  == (yaxisSoftTerm.mpx()+shift)  &&
	 proj.mpy()  ==  yaxisSoftTerm.mpy()         &&
	 proj.sumet()==  yaxisSoftTerm.sumet() 
	 );

  return true;
}

// bool testPtHardHistosFilled0(){std::cout << __PRETTY_FUNCTION__ << std::endl;
//   met::METSystematicsTool tool(toolname);
//   tool.msg().setLevel(MSG::VERBOSE);
//   assert(tool.initialize());

//   xAOD::MissingET yaxisSoftTerm (15., 0., 50.);
//   xAOD::MissingET xaxisHardTerm (10., 0., 50.);

//   xAOD::MissingET proj = tool.pthardsyst_reso(yaxisSoftTerm, xaxisHardTerm, 0., 0. , 0.);//test with no shift, should end the same
//   std::cout << "projection "<< proj.mpx() << ", " << proj.mpy()  << "," << proj.sumet() << std::endl;
//   assert(proj.mpx() == yaxisSoftTerm.mpx() &&
//          proj.mpy() == yaxisSoftTerm.mpy() &&
//          proj.sumet()== yaxisSoftTerm.sumet()
// 	 );

//   proj = tool.pthardsyst_reso(yaxisSoftTerm, xaxisHardTerm, 1., 0. , 0.); //
//   std::cout << "projection "<< proj.mpx() << ", " << proj.mpy()  << "," << proj.sumet() << std::endl;
//   assert(proj.mpx() == yaxisSoftTerm.mpx() &&
//          proj.mpy() == yaxisSoftTerm.mpy() &&
//          proj.sumet()== yaxisSoftTerm.sumet()
// 	 );

//   return true;
// }

bool testCorrectedCopy(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());

  std::cout << "creating test containers" << std::endl;
  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  assert(testMETcont != nullptr);//passes

  std::cout << "creating test object" << std::endl;
  xAOD::MissingET * myObj  =  new xAOD::MissingET("myMETobj", MissingETBase::Source::softEvent());
  testMETcont->push_back(myObj);
  assert(testMETcont->at(0) == myObj);
  std::cout << "checked object at array position 0,  total size is " << testMETcont->size() << std::endl;
  assert(testMETcont->size() == 1); //one element passes       

  xAOD::MissingET * correctedCopy = nullptr;
  assert( tool.correctedCopy(*myObj, correctedCopy));//this time, we should pass because our object is owned

  assert(correctedCopy != nullptr);
  
  delete testMETcont;

  return true;
}

bool testRetrieveTruth(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  xAOD::MissingETContainer * testMETcont  = new xAOD::MissingETContainer();
  assert(testMETcont != nullptr);

  std::cout << "getting missingET test" << std::endl;
  
  xAOD::MissingET * met = (*testMETcont)["NonInt"];
  assert(met == nullptr ); 

  delete testMETcont;

  return true;
}

bool testSoftTrkPtHardScale(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());
  
  CP::SystematicVariation softTrk_ScaleUp("MET_SoftTrk_ScaleUp");
  //  assert(  tool.addAffectingSystematic(softTrk_scaleUp , true /*recommended */) );

  CP::SystematicSet applySys(softTrk_ScaleUp.name());
  //  assert(  tool.applySystematicVariation(softTrk_scaleUp));
  assert(  tool.applySystematicVariation(applySys ) );

  std::cout << "applied systematics are : " << tool.appliedSystematicsString() << std::endl;

  std::cout << "creating test containers" << std::endl;
  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  assert(testMETcont != nullptr);//passes

  std::cout << "creating test object" << std::endl;
  xAOD::MissingET * myObj  =  new xAOD::MissingET(10., 15., 20., "myMETobj", MissingETBase::Source::softEvent());
  testMETcont->push_back(myObj);

  xAOD::MissingET * correctedCopy = nullptr;
  assert( tool.correctedCopy(*myObj, correctedCopy));

  assert(correctedCopy != nullptr);
  assert(tool.applyCorrection(*myObj));
  std::cout << "correctedCopy :" << correctedCopy->mpx() << ", " << correctedCopy->mpy() << ", " << correctedCopy->sumet() << std::endl;
  std::cout << "myObj :" << myObj->mpx() << ", " << myObj->mpy() << ", " << myObj->sumet() << std::endl;
  assert( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() && //todo relook at this, maybe not a great way of doing this
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;

  return true;
}

bool testSoftTrkPtHardResoPara(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());
  
  CP::SystematicVariation softTrk_ResoPara("MET_SoftTrk_ResoPara");
  //  assert(  tool.addAffectingSystematic(softTrk_scaleUp , true /*recommended */) );

  CP::SystematicSet applySys(softTrk_ResoPara.name());
  //  assert(  tool.applySystematicVariation(softTrk_scaleUp));
  assert(  tool.applySystematicVariation(applySys ) );

  std::cout << "applied systematics are : " << tool.appliedSystematicsString() << std::endl;

  std::cout << "creating test containers" << std::endl;
  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  assert(testMETcont != nullptr);//passes

  std::cout << "creating test object" << std::endl;
  xAOD::MissingET * myObj  =  new xAOD::MissingET(10., 15., 20., "myMETobj", MissingETBase::Source::softEvent());
  testMETcont->push_back(myObj);
  xAOD::MissingET * correctedCopy = nullptr;
  tool.setRandomSeed(199);
  assert( tool.correctedCopy(*myObj, correctedCopy));
  assert(correctedCopy != nullptr);

  tool.setRandomSeed(199);//reset the random seed to redo the same calculation
  assert(tool.applyCorrection(*myObj));

  std::cout << "correctedCopy :" << correctedCopy->mpx() << ", " << correctedCopy->mpy() << ", " << correctedCopy->sumet() << std::endl;
  std::cout << "myObj :" << myObj->mpx() << ", " << myObj->mpy() << ", " << myObj->sumet() << std::endl;
  assert( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() && //todo relook at this, maybe not a great way of doing this
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;

  return true;
}


bool testSoftTrkPtHardResoPerp(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());
  
  CP::SystematicVariation softTrk_ResoPerp("MET_SoftTrk_ResoPerp");
  //  assert(  tool.addAffectingSystematic(softTrk_scaleUp , true /*recommended */) );

  CP::SystematicSet applySys(softTrk_ResoPerp.name());
  //  assert(  tool.applySystematicVariation(softTrk_scaleUp));
  assert(  tool.applySystematicVariation(applySys ) );

  std::cout << "applied systematics are : " << tool.appliedSystematicsString() << std::endl;

  std::cout << "creating test containers" << std::endl;
  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  assert(testMETcont != nullptr);//passes

  std::cout << "creating test object" << std::endl;
  xAOD::MissingET * myObj  =  new xAOD::MissingET(10., 15., 20., "myMETobj", MissingETBase::Source::softEvent());
  testMETcont->push_back(myObj);
  xAOD::MissingET * correctedCopy = nullptr;
  std::cout << "setting random seed" << std::endl;
  tool.setRandomSeed(199);
  assert( tool.correctedCopy(*myObj, correctedCopy));
  assert(correctedCopy != nullptr);

  tool.setRandomSeed(199);//reset the random seed to redo the same calculation
  assert(tool.applyCorrection(*myObj));

  std::cout << "correctedCopy :" << correctedCopy->mpx() << ", " << correctedCopy->mpy() << ", " << correctedCopy->sumet() << std::endl;
  std::cout << "myObj :" << myObj->mpx() << ", " << myObj->mpy() << ", " << myObj->sumet() << std::endl;
  assert( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() && //todo relook at this, maybe not a great way of doing this
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;

  return true;
}



bool testSoftCaloPtScale(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  assert(tool.initialize());
  
  CP::SystematicVariation softCalo_scaleUp("MET_SoftCalo_ScaleUp");
  //  assert(  tool.addAffectingSystematic(softCalo_scaleUp , true /*recommended */) );

  CP::SystematicSet recSys(softCalo_scaleUp.name());
  //  assert(  tool.applySystematicVariation(softCalo_scaleUp));
  assert(  tool.applySystematicVariation(recSys) );

  std::cout << "applied systematics are : " << tool.appliedSystematicsString() << std::endl;

  std::cout << "creating test containers" << std::endl;
  xAOD::MissingETContainer *   testMETcont    = new xAOD::MissingETContainer();
  xAOD::MissingETAuxContainer* testMETcontAux = new xAOD::MissingETAuxContainer();
  testMETcont->setStore( testMETcontAux );
  assert(testMETcont != nullptr);//passes

  std::cout << "creating test object" << std::endl;
  xAOD::MissingET * myObj  =  new xAOD::MissingET(10., 15., 20., "myMETobj", MissingETBase::Source::softEvent());
//  xAOD::MissingET * myObj  =  new xAOD::MissingET("myMETobj", MissingETBase::Source::softEvent());
  testMETcont->push_back(myObj);

  xAOD::MissingET * correctedCopy = nullptr;
  assert( tool.correctedCopy(*myObj, correctedCopy));

  assert(correctedCopy != nullptr);
  assert(tool.applyCorrection(*myObj));
  std::cout << "correctedCopy :" << correctedCopy->mpx() << ", " << correctedCopy->mpy() << ", " << correctedCopy->sumet() << std::endl;
  std::cout << "myObj :" << myObj->mpx() << ", " << myObj->mpy() << ", " << myObj->sumet() << std::endl;
  assert( (int)correctedCopy->mpx()  ==  (int)myObj->mpx() && //todo relook at this, maybe not a great way of doing this
	  (int)correctedCopy->mpy()  ==  (int)myObj->mpy() &&
	  (int)correctedCopy->sumet()  ==  (int)myObj->sumet());

  delete testMETcont;

  return true;
}

bool testJetTrackMET(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  tool.setProperty("ConfigJetTrkFile" ,"METTrack_2012.config");

  assert(tool.initialize());



  return true;
}


bool testNoConfigFiles(){std::cout << __PRETTY_FUNCTION__ << std::endl;
  met::METSystematicsTool tool(toolname);
  tool.msg().setLevel(MSG::VERBOSE);
  tool.setProperty("ConfigSoftTrkFile","");
  tool.setProperty("ConfigJetTrkFile","");
  tool.setProperty("ConfigSoftCaloFile","");

  assert(tool.initialize());

  return true;
}

int main(){std::cout << __PRETTY_FUNCTION__ << std::endl;
   // Initialise the application:
   xAOD::Init() ;

   // CP::CorrectionCode::enableFailure();
   // StatusCode::enableFailure();
   // xAOD::TReturnCode::enableFailure();
   TString const fileName = "AOD.pool.root";
   //   Info( APP_NAME, "Opening file: %s", fileName.Data() );
   std::auto_ptr< TFile > ifile( TFile::Open( fileName, "READ" ) );
   assert( ifile.get() );

   // Create a TEvent object:                                                                                                                                              
   //   xAOD::TEvent event( xAOD::TEvent::kClassAccess );
   xAOD::TEvent * event;
   event = new xAOD::TEvent( xAOD::TEvent::kClassAccess );
   assert( event->readFrom( ifile.get() ) );
   //   event = wk()->xaodEvent();

   //   Info( APP_NAME, "Number of events in the file: %i",
   //   static_cast< int >( event.getEntries() ) );

   // Create a transient object store. Needed for the tools.  
   xAOD::TStore store;

   assert(testAddObjToCont());
   assert(testRetrieveTruth());
   assert(testDynamicCast());
   assert(testDefaultHistosFilled());
   assert(testJetTrkHistosFilled());
   //   assert(testNoCollection());
   assert(testNonSoftTermFailure());
   assert(testSoftTermSuccess());//todo readd this one?
   assert(testRetrieveMETTerms());
   assert(testAddAffectingSystematic());
   assert(testProjectST());
   assert(testProjectST2());
   assert(testCorrectedCopy());  

   assert(testSoftTrkPtHardScale());
   assert(testSoftTrkPtHardResoPerp());
   assert(testSoftTrkPtHardResoPara());
   assert(testSoftCaloPtScale());

   assert(testJetTrackMET());
   assert(testNoConfigFiles());

   delete event;

   return 0;

 }
